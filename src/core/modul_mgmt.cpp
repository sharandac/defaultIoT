/**
 * @file modul_mgmt.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "Arduino.h"
#include "config.h"
#include "core/core.h"
#include "core/webserver.h"
#include "core/utils/alloc.h"
#include "core/utils/callback.h"

#include "modul_mgmt.h"
#include "config/module_config.h"
/**
 * analog module namespace
 */
#define MODULE_NAME                "module"
/**
 * local variables
 */
static bool initialized = false;                        /** @brief flag if module is initialized */
module_config_t module_config;                          /** @brief module config */
callback_t *module_loop_callback = NULL;                /** @brief callback function for loop */
callback_t *module_init_callback = NULL;                /** @brief callback function for init */
callback_t *module_deinit_callback = NULL;              /** @brief callback function for deinit */
static int module_autocall_counter = 0;                 /** @brief counter for the registered modules */
module_autocall_table_t *module_autocall_table = NULL;  /** @brief table for the registered modules */
/**
 * local static funtions
 */
static void module_setup( void );
static void module_mgmt_call_by_id( EventBits_t event, const char *id );
static bool webserver_cb( EventBits_t event, void *arg );
/**
 * @brief register a module via autocall function
 * 
 * @param function      pointer to the registration function
 * @return int 
 */
int module_autocall_function( MODULE_AUTOCALL_FUNC function ) {
    /**
     * register a setup function
     */
    module_autocall_counter++;
    /**
     * alloc or realloc the table for a new entry
     */
    if( !module_autocall_table ) 
        module_autocall_table = (module_autocall_table_t *)CALLOC_ASSERT( module_autocall_counter, sizeof( module_autocall_table_t ), MODULE_NAME " autocall table calloc failed" );
    else
        module_autocall_table = (module_autocall_table_t *)REALLOC_ASSERT( module_autocall_table, ( module_autocall_counter ) * sizeof( module_autocall_table_t ), MODULE_NAME " autocall table realloc failed" );
    /**
     * store registration function
     */
    module_autocall_table[ module_autocall_counter - 1 ].function = function;
    
    return( 1 );
}
/**
 * @brief call all registered setup functions
 */
void module_mgmt_call_setup( void ) {
    /**
     * call setup function for all modules
     */
    if( module_autocall_counter && module_autocall_table ) {
        /**
         * call all registered setup functions
         */
        for( int i = 0 ; i < module_autocall_counter ; i++ ) {
            module_autocall_table[ i ].function();
        }
        /**
         * free the table
         */
        module_autocall_counter = 0;
        free( module_autocall_table );
        module_autocall_table = NULL;
    }
    module_mgmt_call_init();
}
/**
 * @brief call the init function of all modules if enabled
 */
void module_mgmt_call_init( void ) {
    for( size_t i = 0 ; i < module_config.count && i < MAX_MODULES ; i++ ) {
        if( module_config.module[ i ].enaled ) {
            module_mgmt_call_by_id( MODULE_MGMT_INIT, module_config.module[ i ].id );
        }
    }
}
/**
 * @brief call the deinit function of all modules if enabled
 */
void module_mgmt_call_deinit( void ) {
    for( size_t i = 0 ; i < module_config.count && i < MAX_MODULES ; i++ ) {
        if( module_config.module[ i ].enaled ) {
            module_mgmt_call_by_id( MODULE_MGMT_DEINIT, module_config.module[ i ].id );
        }
    }
}
/**
 * @brief call the loop function of all modules if enabled
 */
void module_mgmt_call_loop( void ) {
    static uint64_t NextMillis = millis();

    if( NextMillis < millis() ) {
        uint64_t roundtrip = millis();
        NextMillis += module_config.roundtrip;
        callback_send( module_loop_callback, MODULE_MGMT_LOOP, NULL );
        module_config.used_roundtrip_time = millis() - roundtrip;
    }
}
/**
 * @brief call a module by id and send a event
 * 
 * @param event         event to send
 * @param id            id of the module
 */
static void module_mgmt_call_by_id( EventBits_t event, const char *id ) {
    /**
     * check if all callback tables are initialized and id is set
     */
    if( !module_init_callback && !module_deinit_callback && !module_loop_callback && !id )
        return;
    /**
     * get callback tabled depend on event
     */
    callback_t *temp_cb = NULL;
    switch( event ) {
        case MODULE_MGMT_INIT:
            temp_cb = module_init_callback;
            break;
        case MODULE_MGMT_DEINIT:
            temp_cb = module_deinit_callback;
            break;
        case MODULE_MGMT_LOOP:
            temp_cb = module_loop_callback;
            break;
        default:
            return;
    }
    /**
     * search for the id and call the callback function
     */
    for( size_t i = 0 ; i < temp_cb->entrys ; i++ ) {
        if( !strcmp( id, temp_cb->table[i].id ) && temp_cb->table[ i ].active ) {
            temp_cb->table[i].callback_func( event, NULL );
        }
    }
}

bool module_mgmt_register( CALLBACK_FUNC init_func, CALLBACK_FUNC deinit_func, CALLBACK_FUNC loop_func, const char *id ) {
    bool module_hit = false;
    /**
     * check if module is already initialized
     */
    if( !initialized )
        module_setup();
    /**
     * register init callback function
     */
    if ( module_init_callback == NULL ) {
        module_init_callback = callback_init( MODULE_NAME " init" );
        ASSERT( module_init_callback, MODULE_NAME " init alloc failed" );
    }    
    callback_register( module_init_callback, MODULE_MGMT_INIT, init_func, id );
    /**
     * register deinit callback function
     */
    if ( module_deinit_callback == NULL ) {
        module_deinit_callback = callback_init( MODULE_NAME " deinit" );
        ASSERT( module_deinit_callback, MODULE_NAME " deinit alloc failed" );
    }    
    callback_register( module_deinit_callback, MODULE_MGMT_DEINIT, deinit_func, id );
    /**
     * register loop callback function
     */
    if ( module_loop_callback == NULL ) {
        module_loop_callback = callback_init( MODULE_NAME " loop" );
        ASSERT( module_loop_callback, MODULE_NAME " loop alloc failed" );
    }    
    callback_register( module_loop_callback, MODULE_MGMT_LOOP, loop_func, id );
    /**
     * check if module is enabled/disabled by config
     * and set the callback active or not
     */
    for( size_t i = 0 ; i < module_config.count && i < MAX_MODULES ; i++ ) {
        if( !strcmp( id, module_config.module[ i ].id ) ) {
            if( module_config.module[ i ].enaled ) {
                callback_set_active_by_id( module_loop_callback, module_config.module[ i ].id, true );
            }
            else {
                callback_set_active_by_id( module_loop_callback, module_config.module[ i ].id, false );
            }
            module_hit = true;
            break;
        }
    }
    /**
     * if module is not in config, add it and set the loop callback inactive
     */
    if( module_hit == false ) {
        module_config.module[ module_config.count ].enaled = false;
        strncpy( module_config.module[ module_config.count ].id, id , sizeof( module_config.module[ module_config.count ].id ) );
        module_config.count++;
        callback_set_active( module_init_callback, init_func, true );
        callback_set_active( module_deinit_callback, deinit_func, true );
        callback_set_active( module_loop_callback, loop_func, false );
        log_d("register new module %s", id );
    }
    else {
        log_d("register known module %s", id );
    }

    return( true );
}
/**
 * @brief init module management
 * 
 */
static void module_setup( void ) {
    /**
     * check if already initialized
     */
    ASSERT( !initialized, MODULE_NAME " already initialized, check your code" );
    /**
     * load module config
     */
    module_config.load();
    /**
     * register webserver callback
     */
    asyncwebserver_register_cb_with_prio( WS_DATA | WEB_DATA | WEB_MENU | SAVE_CONFIG | RESET_CONFIG, webserver_cb, "/" MODULE_NAME ".htm", CALL_CB_MODUL );
    asyncwebserver_set_cb_active( webserver_cb, true );
    /**
     * set initialized
     */
    initialized = true;
    log_i( MODULE_NAME " mgmt initialized");
}
/**
 * @brief webserver callback
 * 
 * @param event         event
 * @param arg           pointer to wsData_t struct
 * @return true         success
 * @return false        fail
 */
static bool webserver_cb( EventBits_t event, void *arg ) {
    bool retval = false;
    wsData_t *wsData = (wsData_t*)arg;
    AsyncWebSocketClient *client = wsData->client;
    AsyncWebServerRequest *request = wsData->request;
    const char *cmd = wsData->cmd ? wsData->cmd : "";
    const char *value = wsData->value ? wsData->value : "";
    String html = "";
    /**
     * check if servo is initialized
     */
    ASSERT( initialized, MODULE_NAME " not initialized" );

    switch( event ) {
        case WS_DATA:
            ASSERT( client, MODULE_NAME " client is NULL" );
            /**
             * check for cmd
             */
            if ( !strcmp( "save_" MODULE_NAME "_settings", cmd ) ) {
                core_enter_critical(); 
                module_config.save();
                core_exit_critical();
                asyncwebserver_send_websocket_msg( "status\\Save" );
            }
            else if ( !strcmp( "get_" MODULE_NAME "_status", cmd ) ) {
                asyncwebserver_send_websocket_msg( MODULE_NAME "_used_roundtrip_time\\%lld", module_config.used_roundtrip_time );
            }
            /**
             * check get settings command
             */
            else if ( !strcmp("get_" MODULE_NAME "_settings", cmd ) ) {
                for( size_t i = 0 ; i < module_config.count && i < MAX_MODULES; i++ ) {
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_roundtrip\\%d", module_config.roundtrip );
                    asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_%d_state\\%s", i, module_config.module[ i ].enaled ? "true" : "false " );
                }
            }
            else if ( !strcmp ( MODULE_NAME "_roundtrip", cmd ) ) {
                module_config.roundtrip = atoi( value );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_roundtrip\\%d", module_config.roundtrip );
            }
            /**
             * crawl all modules and check if state changed
             */
            for( int i = 0 ; i < module_config.count && i < MAX_MODULES; i++ ) {
                char temp[32];
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_state", i );
                if ( !strcmp( temp, cmd ) ) {
                    /**
                     * check if state changed
                     */
                    if( module_config.module[ i ].enaled == atoi( value ) ? true : false )
                        continue;
                    /**
                     * set new state
                     */
                    module_config.module[ i ].enaled = atoi( value ) ? true : false;
                    /**
                     * call init or deinit
                     */
                    if( module_config.module[ i ].enaled )
                        module_mgmt_call_by_id( MODULE_MGMT_INIT, module_config.module[ i ].id );
                    else
                        module_mgmt_call_by_id( MODULE_MGMT_DEINIT, module_config.module[ i ].id );
                    /**
                     * set callback active or not
                     */
                    callback_set_active_by_id( module_loop_callback, module_config.module[ i ].id, module_config.module[ i ].enaled );
                    /**
                     * send new state to all connected clients
                     */
                    asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_%d_state\\%s", i, module_config.module[ i ].enaled ? "true" : "false " );
                }
            }
            retval = true;
            break;
        case WEB_DATA:
            ASSERT( request, MODULE_NAME " request is NULL" );
            /**
             * build html page
             */
            html = html_header;
            html += asyncwebserver_get_menu();
            html += module_mgmt_config_page_head;
            html += "  <div class='vbox'>\n"
                    "    <label>loop roundtrip time in ms</label><br>\n"
                    "    <div class='box'>\n"
                    "      <input type='text' size='32' id='" MODULE_NAME "_roundtrip' disabled>\n"
                    "    </div>\n"
                    "  </div>\n"
                    "  <div class='vbox'>\n"
                    "    <label>used roundtrip time in ms</label><br>\n"
                    "    <div class='box'>\n"
                    "      <input type='text' size='32' id='" MODULE_NAME "_used_roundtrip_time' disabled>\n"
                    "    </div>\n"
                    "  </div>\n";

            for( int i = 0 ; i < module_config.count && i < MAX_MODULES; i++ ) {
                html += "  <div class='vbox'>\n"
                        "    <label>" MODULE_NAME ": " + String( module_config.module[ i ].id ) + "</label><br>\n"
                        "    <div class='box'>\n"
                        "      <input type='checkbox' id='" MODULE_NAME "_" + String( i ) + "_state' " + String( module_config.module[ i ].enaled ? "checked" : "" ) + "><label for=\"" MODULE_NAME "_" + String( i ) + "_state\">enabled</label>\n"
                        "    </div>\n"
                        "  </div>\n";
            }
            html += module_mgmt_config_page_footer;
            html += "SendSetting(\"" MODULE_NAME "_roundtrip\");";
            for( int i = 0 ; i < module_config.count && MAX_MODULES; i++ ) {
                html += "SendCheckboxSetting(\"" MODULE_NAME "_" + String( i ) + "_state\");";
            }
            html += module_mgmt_config_page_sendBTN;
            html += html_footer;
            request->send(200, "text/html", html);
            retval = true;
            break;
        case WEB_MENU:
            asyncwebserver_set_menu_entry( "/" MODULE_NAME ".htm", MODULE_NAME );
            retval = true;
            break;
        case SAVE_CONFIG:    
            core_enter_critical(); 
            module_config.save();
            core_exit_critical();
            retval = true;
            break;
        case RESET_CONFIG:
            core_enter_critical(); 
            module_config.resetToDefault();
            core_exit_critical();
            retval = true;
            break;  
    }
    /**
     * webserver callback function
     */
    return( retval );
}