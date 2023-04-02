/**
 * @file output.c
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

#include "output.h"
#include "config/output_config.h"
/**
 * module namespace
 */
#define MODULE_NAME                "output"
/**
 * local variables
 */
output_config_t output_config;
static int output_state = 0;
static String output_state_bitmask = "";
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
static String gen_bitmask( int output );
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
     * register webserver and mqtt client callback function
     */
    mqtt_client_register_cb( MQTTSTAT_JSON_DATA | MQTTCMND_JSON_DATA | MQTTTELE_JSON_DATA, mqttclient_cb, MODULE_NAME );
    asyncwebserver_register_cb( WS_DATA | WEB_DATA | WEB_MENU | SAVE_CONFIG | RESET_CONFIG, webserver_cb, "/" MODULE_NAME ".htm" );
    /**
     * register init, deinit and loop function
     */
    module_mgmt_register( initialize, deinitialize, loop, MODULE_NAME );
}

static bool initialize( EventBits_t event, void *arg ) {
    /**
     * check if module already initialized
     */
    ASSERT( !initialized, MODULE_NAME " are not deinitialized, check your code" );
    /**
     * load config
     */
    output_config.load();
    /**
     * setup output pin
     */
    for( int i = 0 ; i < output_config.output_count && i < MAX_OUTPUTS; i++ ) {
        if( output_config.device[ i ].enaled ) {
            pinMode( output_config.device[i].pin, OUTPUT );
            digitalWrite( output_config.device[i].pin, output_config.device[i].state );
            output_state |= output_config.device[i].state ? 1 << i : 0;
            log_d( MODULE_NAME " %d on pin %d, state %s", i, output_config.device[i].pin, output_config.device[ i ].state ? "true" : "false" );
        }
    }
    /**
     * generate bitmask
     */
    output_state_bitmask = gen_bitmask( output_state );
    /**
     * set webserver and mqtt client callback function active
     */
    asyncwebserver_set_cb_active( webserver_cb, true );
    mqtt_client_set_cb_active( mqttclient_cb, true );
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
     * check if module is deinitialized
     */
    ASSERT( initialized, MODULE_NAME " are not initialized, check your code" );
    /**
     * save config
     */
    output_config.save();
    /**
     * setup output pin
     */
    for( int i = 0 ; i < output_config.output_count && i < MAX_OUTPUTS; i++ ) {
        if( output_config.device[ i ].enaled ) {
            pinMode( output_config.device[i].pin, INPUT );
        }
    }
    /**
     * set webserver and mqtt client callback function inactive
     */
    asyncwebserver_set_cb_active( webserver_cb, false );
    mqtt_client_set_cb_active( mqttclient_cb, false );
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
    static uint64_t NextMillis = 0;
    /**
     * check if already init
     */
    ASSERT( initialized, MODULE_NAME " are not in initialized, check your code" );
    /**
     * first run init all things
     */
    if( NextMillis == 0 )
        NextMillis = millis();
    /**
     * work in output
     */
    if( NextMillis < millis() ) {
        NextMillis += 50l;
        int output_state_tmp = 0;
        /**
         * read output state into one value
         */
        for( int i = 0 ; i < output_config.output_count && i < MAX_OUTPUTS; i++) {
            output_state_tmp |= digitalRead( output_config.device[ i ].pin ) ? 1 << i : 0;
        }
        /**
         * check if output state changed
         */
        if( output_state_tmp != output_state ) {
            String json;
            DynamicJsonDocument doc(4096);
            /**
             * get changed bits
             */
            int output_changed = output_state ^ output_state_tmp;
            /**
             * check output state
             */
            doc[ MODULE_NAME "_update"] = true;
            doc[ MODULE_NAME ]["count"] = output_config.output_count;
            /**
             * add output bitmask to json
             */
            output_state_bitmask = gen_bitmask( output_state_tmp );
            doc[ MODULE_NAME ]["bitmask"] = output_state_bitmask;
            doc[ MODULE_NAME ]["value"] = output_state_tmp;
            /**
             * add output state to json
             */
            for( int i = 0 ; i < output_config.output_count && i < MAX_OUTPUTS; i++) {
                doc[ MODULE_NAME ]["pin"][ i ][ "changed" ] = ( 1 << i ) & output_changed ? true : false;
                doc[ MODULE_NAME ]["pin"][ i ][ "state" ] = ( 1 << i ) & output_state_tmp ? true : false;
            }
            /**
             * save new output state
             */
            output_state = output_state_tmp;
            /**
             * send json to mqtt server if enabled
             */
            if( output_config.mqtt_msg_on_change ) {
                serializeJson( doc, json );
                mqtt_client_stat_publish( (char*)json.c_str() );
            }
            /**
             * send update to websocket
             */
            asyncwebserver_send_websocket_msg( MODULE_NAME "_state\\%s", output_state_bitmask.c_str() );
        }
    }
    return( true );
}
/**
 * @brief mqtt client output callback function
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
            ASSERT( mqttData->doc, "mqttstat json data is NULL");
            /**
             * insert output state data
             */
            if( output_config.mqtt_msg_stat ) {
                doc[ MODULE_NAME ]["count"] = output_config.output_count;
                doc[ MODULE_NAME ]["bitmask"] = output_state_bitmask;
                for( int i = 0 ; i < output_config.output_count && i < MAX_OUTPUTS; i++) {
                    doc[ MODULE_NAME ]["pin"][i]["pin"] = output_config.device[ i ].pin;
                    doc[ MODULE_NAME ]["pin"][ i ][ "state" ] = ( 1 << i ) & output_state ? true : false;
                }
            }
            retval = true;
            break;
        case MQTTCMND_JSON_DATA:
            /**
             * check json data
             */
            ASSERT( mqttData->doc, "mqttstat json data is NULL");
            /**
             * check output cmnd
             */
            if( doc.containsKey( "set_" MODULE_NAME ) ) {
                int pin = doc["set_" MODULE_NAME];
                if( output_config.device[ pin ].enaled && pin < MAX_OUTPUTS && pin >= 0 ) {
                    digitalWrite( output_config.device[ pin ].pin, HIGH );
                }
            }

            if( doc.containsKey( "clear_" MODULE_NAME ) ) {
                int pin = doc["clear_" MODULE_NAME ];
                if( output_config.device[ pin ].enaled && pin < MAX_OUTPUTS && pin >= 0 ) {
                    digitalWrite( output_config.device[ pin ].pin, LOW );
                }
            }
            retval = true;
            break;
        case MQTTTELE_JSON_DATA:
            /**
             * check json data
             */
            ASSERT( mqttData->doc, "mqttstat json data is NULL");
            /**
             * insert output tele data
             */
            retval = true;
            break;
    }
    return( retval );
}
/**
 * @brief webserver output callback function
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
            ASSERT( client, "client is NULL, check your code" );
            /**
             * check all commands
             */
            if ( !strcmp( "save_" MODULE_NAME "_settings", cmd ) ) {
                output_config.save();
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
                asyncwebserver_send_websocket_msg("option\\" MODULE_NAME "_count\\%d", output_config.output_count );

                for( int i = 0 ; i < output_config.output_count && i < MAX_OUTPUTS; i++ ) {
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_pin\\%d", i, output_config.device[ i ].pin );
                    asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_%d_pin_state\\%s", i, output_config.device[ i ].state ? "true" : "false " );
                }

                asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_mqtt_msg_stat\\%s", output_config.mqtt_msg_stat ? "true" : "false " );
                asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_mqtt_msg_on_change\\%s", output_config.mqtt_msg_on_change ? "true" : "false " );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_state\\%s", output_state_bitmask.c_str() );
            }
            /**
             * store and set pin count
             */
            else if ( !strcmp ( MODULE_NAME "_count", cmd ) ) {
                output_config.output_count = atoi( value );
                asyncwebserver_send_websocket_msg("option\\" MODULE_NAME "_count\\%d", output_config.output_count );
            }
            /**
             * store and set mqtt_msg_stat
             */  
            else if ( !strcmp ( MODULE_NAME "_mqtt_msg_stat", cmd ) ) {
                output_config.mqtt_msg_stat = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_mqtt_msg_stat\\%s", output_config.mqtt_msg_stat ? "true" : "false " );
            }
            /**
             * store and set mqtt_msg_on_change
             */
            else if ( !strcmp( MODULE_NAME "_mqtt_msg_on_change", cmd ) ) {
                output_config.mqtt_msg_on_change = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_mqtt_msg_on_change\\%s", output_config.mqtt_msg_on_change ? "true" : "false " );
            }
            /**
             * crawl through all output pins and thair options
             */
            for( int i = 0 ; i < output_config.output_count && i < MAX_OUTPUTS; i++ ) {
                char temp[32];
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_pin", i );
                if ( !strcmp( temp, cmd ) ) {
                    pinMode( output_config.device[ i ].pin, OUTPUT );
                    output_config.device[ i ].pin = atoi( value );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_pin\\%d", i, output_config.device[ i ].pin );
                }
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_pin_state", i );
                if ( !strcmp( temp, cmd ) ) {
                    output_config.device[ i ].state = atoi( value ) ? true : false;
                    asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_%d_pin_state\\%s", i, output_config.device[ i ].state ? "true" : "false " );
                }
            }
            retval = true;
            break;
        case WEB_DATA:
            ASSERT( request, "request is NULL, check your code" );
            /**
             * html page and send it
             */
            html = html_header;
            html += asyncwebserver_get_menu();
            html += output_config_page_head;
            html += "  <div class='vbox'>\n"
                    "    <label>pin count</label><br>\n"
                    "    <div class='box'>\n"
                    "      <select id='" MODULE_NAME "_count' onchange='SendSetting(\"" MODULE_NAME "_count\");location.reload();'>";
            for( int i = 0 ; i <= MAX_OUTPUTS ; i++ )
                html += "<option value='" + String( i ) + "'>" + String( i ) + "</option>";
            html += "</select>\n"
                    "    </div>\n"
                    "  </div>\n";
            for( int i = 0 ; i < output_config.output_count && i < MAX_OUTPUTS; i++ ) {
                html += "  <div class='vbox'>\n"
                        "    <label>" MODULE_NAME " " + String( i ) + " pin</label><br>\n"
                        "    <div class='box'>\n"
                        "      <input type='text' size='32' id='output_" + String( i ) + "_pin'>\n"
                        "      <input type='checkbox' id='" MODULE_NAME "_" + String( i ) + "_pin_state'><label>set " MODULE_NAME " on start</label>\n"
                        "    </div>\n"
                        "  </div>\n";
            }
            html += output_config_page_footer;
            html += "SendSetting(\"" MODULE_NAME "_count\");";
            for( int i = 0 ; i < output_config.output_count && i < MAX_OUTPUTS; i++ ) {
                html += "SendSetting(\"" MODULE_NAME "_" + String( i ) + "_pin\");"
                        "SendCheckboxSetting(\"" MODULE_NAME "_" + String( i ) + "_pin_state\");";
            }
            html += output_config_page_sendBTN;
            html += html_footer;
            request->send(200, "text/html", html);
            retval = true;
            break;
        case WEB_MENU:
            asyncwebserver_set_menu_entry( "/" MODULE_NAME ".htm", MODULE_NAME );
            retval = true;
            break;
        case SAVE_CONFIG:
            output_config.save();
            retval = true;
            break;
        case RESET_CONFIG:
            output_config.resetToDefault();
            retval = true;
            break;
    }
    return( retval );
}

static String gen_bitmask( int output ) {
    String bitmask = "";
    for( int i = output_config.output_count - 1; i >= 0; i-- ) {
        if( output_config.device[ i ].enaled )
            bitmask += ( 1 << i ) & output ? "1" : "0";
        else
            bitmask += "?";
    }
    bitmask += "b";

    asyncwebserver_send_websocket_msg( MODULE_NAME "_state\\%s", output_state_bitmask.c_str() );

    return( bitmask );
}
