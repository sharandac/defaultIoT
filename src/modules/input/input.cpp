/**
 * @file input.c
 * @author Dirk Broßwick ( dirk.brosswick@uni-jena.de )
 * @brief 
 * @version 0.1
 * @date 2022-01-18
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <Arduino.h>
#include "config.h"
#include "core/webserver.h"
#include "core/mqttclient.h"
#include "core/modul_mgmt.h"
#include "core/utils/alloc.h"

#include "input.h"
#include "config/input_config.h"
/**
 * module namespace
 */
#define MODULE_NAME                "input"
/**
 * local variables
 */
input_config_t input_config;
QueueHandle_t input_queue;
static int input_state = 0;
static String input_state_bitmask = "";
static bool initialized = false;
/**
 * local callback functions with local scope
 */
static void registration( void );
static bool initialize( EventBits_t event, void *arg );
static bool deinitialize( EventBits_t event, void *arg );
static bool loop( EventBits_t event, void *arg );
static bool mqttclient_cb( EventBits_t event, void *arg );
static bool webserver_cb( EventBits_t event, void *arg );
static String input_gen_bitmask( int input );
/**
 * @brief setup function for vindriktning, called by autocall function
 */
static int registed = module_autocall_function( &registration );           /** @brief module autocall function */


static void registration( void ) {
    /**
     * check if already registered
     */
    ASSERT( registed, MODULE_NAME " setup is called without module registration, check your code [%d]", registed );
    /**
     * register webserver callback
     */
    mqtt_client_register_cb( MQTTSTAT_JSON_DATA | MQTTCMND_JSON_DATA, mqttclient_cb, MODULE_NAME );
    asyncwebserver_register_cb( WS_DATA | WEB_DATA | WEB_MENU | SAVE_CONFIG | RESET_CONFIG, webserver_cb, "/" MODULE_NAME ".htm" );
    /**
     * register init, deinit and loop function
     */
    module_mgmt_register( initialize, deinitialize, loop, MODULE_NAME );
}

static bool initialize( EventBits_t event, void *arg ) {
    /**
     * check if already init
     */
    ASSERT( !initialized, MODULE_NAME " are not deinitialized, check your code" );
    /**
     * load config
     */
    input_config.load();
    /**
     * setup input pin
     */
    for( int i = 0 ; i < input_config.input_count && i < MAX_INPUTS; i++ ) {
        if( input_config.device[ i ].enaled ) {
            pinMode( input_config.device[i].pin, input_config.device[ i ].pin_cfg );
            input_state |= digitalRead( input_config.device[i].pin ) ? 1 << i : 0;
            log_d( MODULE_NAME " %d on pin %d, pin cfg %d", i, input_config.device[i].pin, input_config.device[ i ].pin_cfg );
        }
    }
    /**
     * set webserver callback function active
     */
    mqtt_client_set_cb_active( mqttclient_cb, true );
    asyncwebserver_set_cb_active( webserver_cb, true );
    /**
     * generate bitmask
     */
    input_state_bitmask = input_gen_bitmask( input_state );
    /**
     * create input queue
     */
    if( !input_queue )
        input_queue = xQueueCreate( 10, sizeof( int ) );
    /**
     * set initialized flag
     */
    initialized = true;
    log_i( MODULE_NAME " module initialized");
    /**
     * return success
     */
    return( true );
}

static bool deinitialize( EventBits_t event, void *arg ) {
    /**
     * check if already init
     */
    ASSERT( initialized, MODULE_NAME " are not initialized, check your code" );
    /**
     * store config
     */
    input_config.save();
    /**
     * restore input pin
     */
    for( int i = 0 ; i < input_config.input_count && i < MAX_INPUTS; i++ ) {
        if( input_config.device[ i ].enaled ) {
            pinMode( input_config.device[i].pin, INPUT );
        }
    }
    /**
     * set webserver callback function inactive
     */
    mqtt_client_set_cb_active( mqttclient_cb, false );
    asyncwebserver_set_cb_active( webserver_cb, false );
    /**
     * set initialized flag
     */
    initialized = false;
    log_i( MODULE_NAME " module deinitialized");
    /**
     * return success
     */
    return( true );
}

static bool loop( EventBits_t event, void *arg ) {
    static uint64_t NextInputMillis = 0;
    /**
     * check if already init
     */
    ASSERT( initialized, MODULE_NAME " are not in initialized, check your code" );
    /**
     * first run init all things
     */
    if( NextInputMillis == 0 )
        NextInputMillis = millis();
    /**
     * work in input
     */
    if( NextInputMillis < millis() ) {
        NextInputMillis += 50l;
        int input_state_tmp = 0;
        int input = INPUT_NONE;
        /**
         * read input state into one value
         */
        for( int i = 0 ; i < input_config.input_count && i < MAX_INPUTS; i++) {
            if( input_config.device[ i ].invert )
                input_state_tmp |= digitalRead( input_config.device[ i ].pin ) ? 0 : 1 << i;
            else
                input_state_tmp |= digitalRead( input_config.device[ i ].pin ) ? 1 << i : 0;
        }
        /**
         * check if input state changed
         */
        if( input_state_tmp != input_state ) {
            String json;
            DynamicJsonDocument doc(4096);
            /**
             * get changed bits
             */
            int input_changed = input_state ^ input_state_tmp;
            /**
             * check input state
             */
            doc[ MODULE_NAME "_update"] = true;
            doc[ MODULE_NAME ]["count"] = input_config.input_count;
            /**
             * add input bitmask to json
             */
            input_state_bitmask = input_gen_bitmask( input_state_tmp );
            doc[ MODULE_NAME ]["bitmask"] = input_state_bitmask;
            doc[ MODULE_NAME ]["value"] = input_state_tmp;
            /**
             * add input state to json
             */
            for( int i = 0 ; i < input_config.input_count && i < MAX_INPUTS; i++) {
                doc[ MODULE_NAME ]["pin"][ i ][ "changed" ] = ( 1 << i ) & input_changed ? true : false;
                doc[ MODULE_NAME ]["pin"][ i ][ "state" ] = ( 1 << i ) & input_state_tmp ? true : false;
                input |= ( 1 << i ) & input_changed ? ( 1 << i ) & input_state_tmp : INPUT_NONE;
            }
            /**
             * save new input state
             */
            input_state = input_state_tmp;
            /**
             * send json to mqtt server if enabled
             */
            if( input_config.mqtt_msg_on_change ) {
                serializeJson( doc, json );
                mqtt_client_stat_publish( (char*)json.c_str() );
            }
            /**
             * send update to websocket
             */
            asyncwebserver_send_websocket_msg( MODULE_NAME "_state\\%s", input_state_bitmask.c_str() );
            /**
             * send input event to queue
             */
            if( input != INPUT_NONE ) {
                log_i( MODULE_NAME ": %d", input );
                xQueueSend( input_queue, &input, 0 );
            }
        }
    }
    return( true );
}

/**
 * @brief mqtt client input callback function
 * 
 * @param event         event bitmask
 * @param arg           pointer to optional data
 * @return true 
 * @return false 
 */
static bool mqttclient_cb( EventBits_t event, void *arg ) {
    bool retval = false;
    mqttData_t *mqttData = (mqttData_t*)arg;
    DynamicJsonDocument& doc = *mqttData->doc;

    switch( event ) {
        case MQTTSTAT_JSON_DATA:
            /**
             * check json data
             */
            ASSERT( mqttData->doc, MODULE_NAME " mqttstat json data is NULL");
            /**
             * insert input state data
             */
            if( input_config.mqtt_msg_stat ) {
                doc[ MODULE_NAME ]["count"] = input_config.input_count;
                doc[ MODULE_NAME ]["bitmask"] = input_state_bitmask;
                for( int i = 0 ; i < input_config.input_count && i < MAX_INPUTS; i++) {
                    doc[ MODULE_NAME ]["pin"][i]["pin"] = input_config.device[ i ].pin;
                    doc[ MODULE_NAME ]["pin"][ i ][ "state" ] = ( 1 << i ) & input_state ? true : false;
            }
            }
            retval = true;
            break;
        case MQTTCMND_JSON_DATA:
            /**
             * check json data
             */
            ASSERT( mqttData->doc, MODULE_NAME " mqttstat json data is NULL");
            /**
             * check input cmnd
             */
            if( doc.containsKey( "rpm" ) ) {

            }
            retval = true;
            break;
        case MQTTTELE_JSON_DATA:
            /**
             * check json data
             */
            ASSERT( mqttData->doc, MODULE_NAME " mqttstat json data is NULL");
            /**
             * insert input tele data
             */
            retval = true;
            break;
    }
    return( retval );
}
/**
 * @brief webserver input callback function
 * 
 * @param event         event bitmask
 * @param arg           pointer to optional data
 * @return true 
 * @return false 
 */
static bool webserver_cb( EventBits_t event, void *arg ) {
    bool retval = false;
    wsData_t *ws_data = (wsData_t *)arg;
    AsyncWebSocketClient * client = ws_data->client ? ws_data->client : NULL;
    AsyncWebServerRequest * request = ws_data->request ? ws_data->request : NULL;
    const char *cmd = ws_data->cmd ? ws_data->cmd : "";
    const char *value = ws_data->value ? ws_data->value : "";
    String html = "";
    /**
     * check if already init
     */
    ASSERT( initialized, MODULE_NAME " are not in initialized, check your code" );
    /**
     * check event
     */
    switch( event ) {
        case WS_DATA:
            ASSERT( client, MODULE_NAME " client is NULL, check your code" );
            /**
             * check all commands
             */
            if ( !strcmp( "save_" MODULE_NAME "_settings", cmd ) ) {
                input_config.save();
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
                asyncwebserver_send_websocket_msg("option\\" MODULE_NAME "_count\\%d", input_config.input_count );
                for( int i = 0 ; i < input_config.input_count && i < MAX_INPUTS; i++ ) {
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_pin\\%d", i, input_config.device[ i ].pin );
                    asyncwebserver_send_websocket_msg( "option\\" MODULE_NAME "_%d_pin_cfg\\%d", i, input_config.device[ i ].pin_cfg );
                    asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_%d_pin_invert\\%s", i, input_config.device[ i ].invert ? "true" : "false " );
                }
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_mqtt_msg_stat\\%s", input_config.mqtt_msg_stat ? "true" : "false " );
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_mqtt_msg_on_change\\%s", input_config.mqtt_msg_on_change ? "true" : "false " );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_state\\%s", input_state_bitmask.c_str() );
            }
            /**
             * store and set pin count
             */
            else if ( !strcmp ( MODULE_NAME "_count", cmd ) ) {
                input_config.input_count = atoi( value );
                asyncwebserver_send_websocket_msg( "option\\" MODULE_NAME "_count\\%d", input_config.input_count );
            }
            /**
             * store and set mqtt_msg_stat
             */            
            else if ( !strcmp ( MODULE_NAME "_mqtt_msg_stat", cmd ) ) {
                input_config.mqtt_msg_stat = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_mqtt_msg_stat\\%s", input_config.mqtt_msg_stat ? "true" : "false " );
            }
            /**
             * store and set mqtt_msg_on_change
             */
            else if ( !strcmp( MODULE_NAME "_mqtt_msg_on_change", cmd ) ) {
                input_config.mqtt_msg_on_change = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_mqtt_msg_on_change\\%s", input_config.mqtt_msg_on_change ? "true" : "false " );
            }
            /**
             * crawl through all input pins and thair options
             */
            for( int i = 0 ; i < input_config.input_count && i < MAX_INPUTS; i++ ) {
                char temp[32];
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_pin", i );
                if ( !strcmp( temp, cmd ) ) {
                    pinMode( input_config.device[ i ].pin, INPUT );
                    input_config.device[ i ].pin = atoi( value );
                    pinMode( input_config.device[ i ].pin, input_config.device[ i ].pin_cfg );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_pin\\%d", i, input_config.device[ i ].pin );
                }
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_pin_cfg", i );
                if ( !strcmp( temp, cmd ) ) {
                    pinMode( input_config.device[ i ].pin, INPUT );
                    input_config.device[ i ].pin_cfg = atoi( value );
                    pinMode( input_config.device[ i ].pin, input_config.device[ i ].pin_cfg );
                    asyncwebserver_send_websocket_msg("option\\" MODULE_NAME "_%d_pin_cfg\\%d", i, input_config.device[ i ].pin_cfg );
                }
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_pin_invert", i );
                if ( !strcmp( temp, cmd ) ) {
                    input_config.device[ i ].invert = atoi( value ) ? true : false;
                    asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_%d_pin_invert\\%s", i, input_config.device[ i ].invert ? "true" : "false " );
                }
            }
            retval = true;
            break;
        case WEB_DATA:
            ASSERT( request, MODULE_NAME " request is NULL, check your code" );
            /**
             * html page and send it
             */
            html = html_header;
            html += asyncwebserver_get_menu();
            html += input_config_page_head;
            html += "  <div class='vbox'>\n"
                    "    <label>count</label><br>\n"
                    "    <div class='box'>\n"
                    "      <select id='" MODULE_NAME "_count' onchange='SendSetting(\"" MODULE_NAME "_count\");location.reload();'>";
            for( int i = 0 ; i <= MAX_INPUTS ; i++ )
                html += "<option value='" + String( i ) + "'>" + String( i ) + "</option>";
            html += "</select>\n"
                    "    </div>\n"
                    "  </div>\n";
            for( int i = 0 ; i < input_config.input_count && i < MAX_INPUTS; i++ ) {
                html += "  <div class='vbox'>\n"
                        "    <label>" MODULE_NAME " " + String( i ) + " pin</label><br>\n"
                        "    <div class='box'>\n"
                        "      <input type='text' size='32' id='" MODULE_NAME "_" + String( i ) + "_pin'>\n"
                        "    </div>\n"
                        "    <div class='box'>\n"
                        "      <select id='" MODULE_NAME "_" + String( i ) + "_pin_cfg' onclick='SendSetting(\"" MODULE_NAME "_" + String( i ) + "_pin_cfg\");get_settings();'><option value='1'>INPUT</option><option value='5'>INPUT_PULLUP</option><option value='9'>INPUT_PULLDOWN</option></select>\n"
                        "      <input type='checkbox' id='" MODULE_NAME "_" + String( i ) + "_pin_invert' onclick='SendCheckboxSetting(\"" MODULE_NAME "_" + String( i ) + "_pin_invert\");get_settings();'><label for='" MODULE_NAME "_" + String( i ) + "_pin_invert'>invert " MODULE_NAME "</label>\n"
                        "    </div>\n"
                        "  </div>\n";
            }
            html += input_config_page_footer;
            html += "SendSetting(\"" MODULE_NAME "_count\");";
            for( int i = 0 ; i < input_config.input_count && i < MAX_INPUTS; i++ )
                html += "SendSetting(\"" MODULE_NAME "_" + String( i ) + "_pin\");";
            html += input_config_page_sendBTN;
            html += html_footer;
            request->send(200, "text/html", html);
            retval = true;
            break;
        case WEB_MENU:
            asyncwebserver_set_menu_entry( "/" MODULE_NAME ".htm", MODULE_NAME );
            retval = true;
            break;
        case SAVE_CONFIG:
            input_config.save();
            retval = true;
            break;
        case RESET_CONFIG:
            input_config.resetToDefault();
            retval = true;
            break;
    }
    return( retval );
}

static String input_gen_bitmask( int input ) {
    String bitmask = "";
    
    for( int i = input_config.input_count - 1; i >= 0; i-- ) {
        if( input_config.device[ i ].enaled )
            bitmask += ( 1 << i ) & input ? "1" : "0";
        else
            bitmask += "?";
    }
    bitmask += "b";

    asyncwebserver_send_websocket_msg( MODULE_NAME "_state\\%s", input_state_bitmask.c_str() );

    return( bitmask );
}

void input_inject( int key ) {
    if( initialized )
        xQueueSend( input_queue, &key, 0 );  
}

int input_get( void ) {
    int retval = INPUT_NONE;
    
    if( initialized )
        xQueueReceive( input_queue, &retval, 0 );        

    return( retval );
}

void input_save_settings( void ) {
    input_config.save();
}