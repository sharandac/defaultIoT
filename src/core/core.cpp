/**
 * @file template.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <Arduino.h>
#include "esp_err.h"
#include "esp_pm.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <esp_task_wdt.h>
#include "SPIFFS.h"

#include "config.h"
#include "webserver.h"
#include "core/utils/alloc.h"
#include "core/utils/callback.h"

#include "core.h"
#include "config/core_config.h"
/**
 * module namespace
 */
#define MODULE_NAME                "core"
/**
 * local variables
 */
static bool initialized = false;
core_config_t core_config;
esp_pm_config_esp32_t pm_config;
size_t core_autocall_counter = 0;
core_autocall_table_t *core_autocall_table = NULL;
/**
 * local callback functions with local scope
 */
static bool webserver_cb( EventBits_t event, void *arg );
/**
 * @brief register a core module with prio
 * 
 * @param function      pointer to a function
 * @param prio          priority of the function, 0 first, 1 second, ...
 * @return int 
 */
int core_autocall_function( CORE_AUTOCALL_FUNC function, size_t prio ) {
    /**
     * register a setup function
     */
    core_autocall_counter++;
    /**
     * alloc or realloc the table for a new entry
     */
    if( !core_autocall_table ) 
        core_autocall_table = (core_autocall_table_t *)CALLOC_ASSERT( core_autocall_counter, sizeof( core_autocall_table_t ), MODULE_NAME " autocall table calloc failed" );
    else
        core_autocall_table = (core_autocall_table_t *)REALLOC_ASSERT( core_autocall_table, ( core_autocall_counter ) * sizeof( core_autocall_table_t ), MODULE_NAME " autocall table realloc failed" );
    /**
     * store registration function
     */
    core_autocall_table[ core_autocall_counter - 1 ].function = function;
    core_autocall_table[ core_autocall_counter - 1 ].prio = prio;
    
    return( 1 );
}

void core_setup( void ) {
    /**
     * check if already initialized
     */
    ASSERT( !initialized, MODULE_NAME " are not deinitialized, check your code" );
    /**
     * start serial
     */
    Serial.begin( 115200 );
    log_i( MODULE_NAME " start %s", DEVICE_NAME );
    /**
     * mount SPIFFS
     */
    if ( !SPIFFS.begin() ) {        
        /*
         * format SPIFFS if not aviable
         */
        SPIFFS.format();
        log_i( MODULE_NAME " formating SPIFFS");
    }  
    /**
     * load config
     */
    core_config.load();
    /**
     * register webserver callback function
     */
    asyncwebserver_register_cb_with_prio( WS_DATA | WEB_DATA | WEB_MENU | SAVE_CONFIG | RESET_CONFIG, webserver_cb, "/index.htm", CALL_CB_FIRST );
    asyncwebserver_set_cb_active( webserver_cb, true );
    /*
     * enable watchdog
     */
    if( core_config.watchdog ) {
        esp_task_wdt_init( WDT_TIMEOUT, true );
        esp_task_wdt_add( NULL );
    }
    /**
     * set cpu frequency
     */
    setCpuFrequencyMhz( core_config.frequency );
    /**
     * set brownout detection
     */
    if( !core_config.brownout_detection ) {
        WRITE_PERI_REG( RTC_CNTL_BROWN_OUT_REG, 0 );
        log_i( MODULE_NAME " brown out detection disabled");
    }
    /**
     * set frequency scaling if enabled
     */
    #if CONFIG_PM_ENABLE
        if( core_config.frequency_scaling ) {
            pm_config.max_freq_mhz = core_config.frequency;
            pm_config.min_freq_mhz = 80;
            pm_config.light_sleep_enable = core_config.light_sleep ? true : false ;
            ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
            log_i( MODULE_NAME "custom arduino-esp32 framework detected, enable PM/DFS support, %d/80MHz with light sleep %s", core_config.frequency, core_config.light_sleep ? "enabled" : "disabled" );
        }
    #endif
    /**
     * start core servies
     */
    if( core_autocall_counter && core_autocall_table ) {
        /**
         * call all registered setup functions
         */
        for( size_t prio = 0 ; prio < 16 ; prio++ ) {
            for( size_t i = 0 ; i < core_autocall_counter ; i++ ) {
                if( core_autocall_table[ i ].prio == prio )
                    core_autocall_table[ i ].function();
            }
        }
        /**
         * free the table
         */
        core_autocall_counter = 0;
        free( core_autocall_table );
        core_autocall_table = NULL;
    }
    /**
     * set initialized flag
     */
    initialized = true;
    log_i( MODULE_NAME " module initialized");
}
/**
 * @brief reset the watchdog
 * 
 */
void core_reset_wdt( void ) {
    if( core_config.watchdog )
        esp_task_wdt_reset();
}
/**
 * @brief callback function for webserver
 * 
 * @param event         event from webserver
 * @param arg           pointer to data
 * @return true         callback function was successfull
 * @return false        callback function was not successfull
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
    ASSERT( initialized, MODULE_NAME " are not initialized, check your code" );
    /**
     * handle webserver events
     */
    switch( event ) {
        case WS_DATA:
            ASSERT( client, MODULE_NAME " client is NULL" );
            /**
             * save config
             */
            if ( !strcmp( "save_" MODULE_NAME "_settings", cmd ) ) {
                core_config.save();
                asyncwebserver_send_websocket_msg( "status\\Save" );
            }
            /**
             * check get status command
             */
            else if ( !strcmp( "get_" MODULE_NAME "_status", cmd ) ) {
            }
            /**
             * check get settings command
             */
            else if ( !strcmp("get_" MODULE_NAME "_settings", cmd ) ) {
                asyncwebserver_send_websocket_msg( MODULE_NAME "_frequency\\%d", core_config.frequency );
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_brownout_detection\\%s", core_config.brownout_detection ? "true" : "false" );
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_watchdog\\%s", core_config.watchdog ? "true" : "false" );
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_frequency_scaling\\%s", core_config.frequency_scaling ? "true" : "false" );
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_light_sleep\\%s", core_config.light_sleep ? "true" : "false" );
            }
            /**
             * set cpu frequency
             */
            else if ( !strcmp ( MODULE_NAME "_frequency", cmd ) ) {
                core_config.frequency = atoi( value );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_frequency\\%d", core_config.frequency );
            }
            /**
             * set brownout detection
             */
            else if ( !strcmp ( MODULE_NAME "_brownout_detection", cmd ) ) {
                core_config.brownout_detection = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_brownout_detection\\%s", core_config.brownout_detection ? "true" : "false" );
            }
            /**
             * set watchdog
             */
            else if ( !strcmp ( MODULE_NAME "_watchdog", cmd ) ) {
                core_config.watchdog = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_watchdog\\%s", core_config.watchdog ? "true" : "false" );
            }
            /**
             * enable/disable frequency scaling
             */
            else if ( !strcmp ( MODULE_NAME "_frequency_scaling", cmd ) ) {
                core_config.frequency_scaling = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_frequency_scaling\\%s", core_config.frequency_scaling ? "true" : "false" );
            }
            /**
             * senable/disable light sleep
             */
            else if ( !strcmp ( MODULE_NAME "_light_sleep", cmd ) ) {
                core_config.light_sleep = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_light_sleep\\%s", core_config.light_sleep ? "true" : "false" );
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
            html += core_config_page;
            html += html_footer;
            request->send(200, "text/html", html);
            retval = true;
            break;
        case WEB_MENU:
            asyncwebserver_set_menu_entry( "/" MODULE_NAME ".htm", MODULE_NAME );
            retval = true;
            break;
        case SAVE_CONFIG:    
            /**
             * save config
             */
            core_config.save();
            retval = true;
            break;
        case RESET_CONFIG:
            /**
             * reset config
             */
            core_config.resetToDefault();
            retval = true;
            break;  
    }

    return( retval );
}
