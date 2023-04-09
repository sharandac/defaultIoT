/**
 * @file servo.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <Arduino.h>
#include <Servo.h>
#include "config.h"
#include "core/webserver.h"
#include "core/mqttclient.h"
#include "core/modul_mgmt.h"
#include "core/utils/alloc.h"

#include "servo.h"
#include "config/servo_config.h"
/**
 * module namespace
 */
#define MODULE_NAME                "servo"
/**
 * local variables
 */
servo_config_t servo_config;
Servo device[ MAX_SERVOS ];
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
     * check if already initialized
     */
    ASSERT( !initialized, MODULE_NAME " are not deinitialized, check your code" );
    /**
     * load config
     */
    servo_config.load();
    /**
     * setup servo
     */
    for( int i = 0 ; i < servo_config.count && i < MAX_SERVOS ; i++ ) {
        if( servo_config.device[ i ].enaled ) {
            if( servo_config.device[ i ].neutral_on_start ) {
                device[ i ].attach( servo_config.device[ i ].pin );
                device[ i ].write( servo_config.device[ i ].neutal_angle );
                servo_config.device[ i ].destination_angle = servo_config.device[ i ].neutal_angle; 
                servo_config.device[ i ].millis = servo_config.device[ i ].detach_time ? millis() + servo_config.device[ i ].detach_time : 0;
            }
        }
    }
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
     * check if servo is deinitialized
     */
    ASSERT( initialized, MODULE_NAME " are not initialized, check your code" );
    /**
     * save config
     */
    servo_config.save();
    /**
     * detach all servos
     */
    for( int i = 0 ; i < servo_config.count && i < MAX_SERVOS ; i++ ) {
        if( servo_config.device[ i ].enaled ) {
            device[ i ].detach();
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
    static uint64_t NextMillis = millis();
    /**
     * check if module is initialized
     */
    if( !initialized ) {
        log_e( MODULE_NAME " not initialized");
        return( false );
    }

    if( NextMillis < millis() ) {
        NextMillis = millis() + 100l;

        for( int i = 0 ; i < servo_config.count && i < MAX_SERVOS ; i++ ) {
            if( servo_config.device[ i ].enaled ) {
                /**
                 * check if servo movement is requested
                 */
                if( servo_config.device[ i ].destination_angle != servo_config.device[ i ].current_angle ) {
                    /**
                     * limit servo angle
                     */
                    if( servo_config.device[ i ].destination_angle < servo_config.device[ i ].min_angle )
                        servo_config.device[ i ].destination_angle = servo_config.device[ i ].min_angle;
                    else if( servo_config.device[ i ].destination_angle > servo_config.device[ i ].max_angle )
                        servo_config.device[ i ].destination_angle = servo_config.device[ i ].max_angle;
                    /**
                     * attach servo and move to destination angle
                     */
                    device[ i ].attach( servo_config.device[ i ].pin );
                    device[ i ].write( servo_config.device[ i ].destination_angle );
                    servo_config.device[ i ].current_angle = servo_config.device[ i ].destination_angle;
                    servo_config.device[ i ].millis = servo_config.device[ i ].detach_time ? millis() + servo_config.device[ i ].detach_time : 0;
                }
                /**
                 * check if servo should be detached
                 */
                if( servo_config.device[ i ].millis && servo_config.device[ i ].millis < millis() ) {
                    /**
                     * check if servo should be moved to neutral position, otherweise detach servo
                     */
                    if( servo_config.device[ i ].neutral_after_move && servo_config.device[ i ].current_angle != servo_config.device[ i ].neutal_angle ) {
                        servo_config.device[ i ].destination_angle = servo_config.device[ i ].neutal_angle;
                    }
                    else {
                        device[ i ].detach();
                        servo_config.device[ i ].millis = 0;
                    }
                }
            }
        }
    }
    /**
     * return success
     */
    return( true );
}

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
                servo_config.save();
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
                asyncwebserver_send_websocket_msg("option\\" MODULE_NAME "_count\\%d", servo_config.count );
                for( int i = 0 ; i < servo_config.count && i < MAX_SERVOS; i++ ) {
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_pin\\%d", i, servo_config.device[ i ].pin );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_id\\%s", i, servo_config.device[ i ].id );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_min_angle\\%d", i, servo_config.device[ i ].min_angle );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_neutal_angle\\%d", i, servo_config.device[ i ].neutal_angle );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_max_angle\\%d", i, servo_config.device[ i ].max_angle );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_min_value\\%d", i, servo_config.device[ i ].min_value );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_max_value\\%d", i, servo_config.device[ i ].max_value );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_detach_time\\%d", i, servo_config.device[ i ].detach_time );
                    asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_%d_neutral_on_start\\%s", i, servo_config.device[ i ].neutral_on_start ? "true" : "false" );
                    asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_%d_neutral_after_move\\%s", i, servo_config.device[ i ].neutral_after_move ? "true" : "false" );
                }
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_mqtt_msg_stat\\%s", servo_config.mqtt_msg_stat ? "true" : "false " );
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_mqtt_msg_on_change\\%s", servo_config.mqtt_msg_on_change ? "true" : "false " );
            }
            /**
             * store and set pin count
             */
            else if ( !strcmp ( MODULE_NAME "_count", cmd ) ) {
                servo_config.count = atoi( value );
                asyncwebserver_send_websocket_msg( "option\\" MODULE_NAME "_count\\%d", servo_config.count );
            }
            /**
             * store and set mqtt_msg_stat
             */ 
            else if ( !strcmp ( MODULE_NAME "_mqtt_msg_stat", cmd ) ) {
                servo_config.mqtt_msg_stat = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_mqtt_msg_stat\\%s", servo_config.mqtt_msg_stat ? "true" : "false " );
            }
            /**
             * store and set mqtt_msg_on_change
             */
            else if ( !strcmp( MODULE_NAME "_mqtt_msg_on_change", cmd ) ) {
                servo_config.mqtt_msg_on_change = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_mqtt_msg_on_change\\%s", servo_config.mqtt_msg_on_change ? "true" : "false " );
            }
            /**
             * crawl through all input pins and their options
             */
            for( int i = 0 ; i < servo_config.count && i < MAX_SERVOS; i++ ) {
                char temp[32];
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_pin", i );
                if ( !strcmp( temp, cmd ) ) {
                    servo_config.device[ i ].pin = atoi( value );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_pin\\%d", i, servo_config.device[ i ].pin );
                }
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_id", i );
                if ( !strcmp( temp, cmd ) ) {
                    strncpy( servo_config.device[ i ].id, value, sizeof( servo_config.device[ i ].id ) );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_id\\%s", i, servo_config.device[ i ].id );
                }
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_min_angle", i );
                if ( !strcmp( temp, cmd ) ) {
                    servo_config.device[ i ].min_angle = atoi( value );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_min_angle\\%d", i, servo_config.device[ i ].min_angle );
                }
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_neutal_angle", i );
                if ( !strcmp( temp, cmd ) ) {
                    servo_config.device[ i ].neutal_angle = atoi( value );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_neutal_angle\\%d", i, servo_config.device[ i ].neutal_angle );
                }
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_max_angle", i );
                if ( !strcmp( temp, cmd ) ) {
                    servo_config.device[ i ].max_angle = atoi( value );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_max_angle\\%d", i, servo_config.device[ i ].max_angle );
                }
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_min_value", i );
                if ( !strcmp( temp, cmd ) ) {
                    servo_config.device[ i ].min_value = atoi( value );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_min_value\\%d", i, servo_config.device[ i ].min_value );
                }
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_max_value", i );
                if ( !strcmp( temp, cmd ) ) {
                    servo_config.device[ i ].max_value = atoi( value );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_max_value\\%d", i, servo_config.device[ i ].max_value );
                }
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_detach_time", i );
                if ( !strcmp( temp, cmd ) ) {
                    servo_config.device[ i ].detach_time = atoi( value );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_detach_time\\%d", i, servo_config.device[ i ].detach_time );
                }
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_neutral_on_start", i );
                if ( !strcmp( temp, cmd ) ) {
                    servo_config.device[ i ].neutral_on_start = atoi( value ) ? true : false;
                    asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_%d_neutral_on_start\\%s", i, servo_config.device[ i ].neutal_angle ? "true" : "false " );
                }
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_neutral_after_move", i );
                if ( !strcmp( temp, cmd ) ) {
                    servo_config.device[ i ].neutral_after_move = atoi( value ) ? true : false;
                    asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_%d_neutral_after_move\\%s", i, servo_config.device[ i ].neutral_after_move ? "true" : "false " );
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
            html += servo_config_head;
            html += "  <div class='vbox'>\n"
                    "    <label>count</label><br>\n"
                    "    <div class='box'>\n"
                    "      <select id='" MODULE_NAME "_count' onchange='SendSetting(\"" MODULE_NAME "_count\");location.reload();'>";
            for( int i = 0 ; i <= MAX_SERVOS ; i++ )
                html += "<option value='" + String( i ) + "'>" + String( i ) + "</option>";
            html += "</select>\n"
                    "    </div>\n"
                    "  </div>\n";
            for( int i = 0 ; i < servo_config.count && i < MAX_SERVOS; i++ ) {
                html += "  <div class='vbox'>\n"
                        "    <label>" MODULE_NAME " " + String( i ) + "</label><br>\n"
                        "    <div class='box'>\n"
                        "      <label>pin</label><input type='text' size='32' id='" MODULE_NAME "_" + String( i ) + "_pin'>\n"
                        "      <label>id</label><input type='text' size='32' id='" MODULE_NAME "_" + String( i ) + "_id'>\n"
                        "      <label>min angle</label><input type='text' size='32' id='" MODULE_NAME "_" + String( i ) + "_min_angle'>\n"
                        "      <label>neutral angle</label><input type='text' size='32' id='" MODULE_NAME "_" + String( i ) + "_neutal_angle'>\n"
                        "      <label>max angle</label><input type='text' size='32' id='" MODULE_NAME "_" + String( i ) + "_max_angle'>\n"
                        "      <label>min value (scaled to min angle)</label><input type='text' size='32' id='" MODULE_NAME "_" + String( i ) + "_min_value'>\n"
                        "      <label>max value (scaled to max angle)</label><input type='text' size='32' id='" MODULE_NAME "_" + String( i ) + "_max_value'>\n"
                        "      <label>detach time</label><input type='text' size='32' id='" MODULE_NAME "_" + String( i ) + "_detach_time'>\n"
                        "      <input type='checkbox' id='" MODULE_NAME "_" + String( i ) + "_neutral_on_start' onclick='SendCheckboxSetting(\"" MODULE_NAME "_" + String( i ) + "_neutral_on_start\");'><label>neutral on start</label>\n"
                        "      <input type='checkbox' id='" MODULE_NAME "_" + String( i ) + "_neutral_after_move' onclick='SendCheckboxSetting(\"" MODULE_NAME "_" + String( i ) + "_neutral_after_move\");'><label>neutral after movement (only when detach is set)</label>\n"
                        "    </div>\n"
                        "  </div>\n";
            }            
            html += servo_config_footer;
            for( int i = 0 ; i < servo_config.count && i < MAX_SERVOS; i++ ) {
                html += "SendSetting(\"" MODULE_NAME "_" + String( i ) + "_pin\");"
                        "SendSetting(\"" MODULE_NAME "_" + String( i ) + "_id\");"
                        "SendSetting(\"" MODULE_NAME "_" + String( i ) + "_min_angle\");"
                        "SendSetting(\"" MODULE_NAME "_" + String( i ) + "_neutal_angle\");"
                        "SendSetting(\"" MODULE_NAME "_" + String( i ) + "_max_angle\");"
                        "SendSetting(\"" MODULE_NAME "_" + String( i ) + "_min_value\");"
                        "SendSetting(\"" MODULE_NAME "_" + String( i ) + "_max_value\");"
                        "SendSetting(\"" MODULE_NAME "_" + String( i ) + "_detach_time\");";
            }
            html += servo_config_sendBTN;
            html += html_footer;
            request->send(200, "text/html", html);
            retval = true;
            break;
        case WEB_MENU:
            asyncwebserver_set_menu_entry( "/" MODULE_NAME ".htm", MODULE_NAME );
            retval = true;
            break;
        case SAVE_CONFIG:    
            servo_config.save();
            retval = true;
            break;
        case RESET_CONFIG:
            servo_config.resetToDefault();
            retval = true;
            break;  
    }

    return( retval );
}

static bool mqttclient_cb( EventBits_t event, void *arg ) {
    bool retval = false;
    mqttData_t *mqttData = (mqttData_t*)arg;
    DynamicJsonDocument& doc = *mqttData->doc;

    switch( event ) {
        case MQTTSTAT_JSON_DATA:
            /**
             * check json data
             */
            ASSERT( mqttData->doc, MODULE_NAME " mqttstat json data is NULL" );
            /*
             *
             */
            if( servo_config.mqtt_msg_stat ) {
                doc[ MODULE_NAME ]["count"] = servo_config.count;
                for( int i = 0 ; i < servo_config.count && i < MAX_SERVOS; i++ ) {
                    if( servo_config.device[ i ].enaled ) {
                        doc[ MODULE_NAME ]["pin"][i]["pin"] = servo_config.device[ i ].pin;
                        doc[ MODULE_NAME ]["pin"][i]["id"] = servo_config.device[ i ].id;
                        doc[ MODULE_NAME ]["pin"][i]["current_angle"] = servo_config.device[ i ].current_angle;
                        doc[ MODULE_NAME ]["pin"][i]["current_value"] = servo_config.device[ i ].current_value;
                        doc[ MODULE_NAME ]["pin"][i]["state"] = servo_config.device[ i ].state;
                    }
                }
            }
            retval = true;
            break;
        case MQTTCMND_JSON_DATA:
            /**
             * check json data
             */
            ASSERT( mqttData->doc, MODULE_NAME " mqttstat json data is NULL" );
            /*
             *
             */
            if( doc.containsKey(  MODULE_NAME "_channel") ) {
                int channel = doc[ MODULE_NAME "_channel"];
                if( servo_config.device[ channel ].enaled ) {
                    if( doc.containsKey("angle") && channel >= 0 && channel < MAX_SERVOS ) {
                        servo_config.device[ channel ].destination_angle = doc["angle"];
                    }
                    else if( doc.containsKey("value") && channel >= 0 && channel < MAX_SERVOS ) {
                        servo_config.device[ channel ].current_value = doc["value"];
                        servo_config.device[ channel ].destination_angle = ( ( servo_config.device[ channel ].max_angle - servo_config.device[ channel ].min_angle ) * ( servo_config.device[ channel ].current_value - servo_config.device[ channel ].min_value ) ) / ( servo_config.device[ channel ].max_value - servo_config.device[ channel ].min_value );
                    }
                    else if( doc.containsKey("min") && channel >= 0 && channel < MAX_SERVOS ) {
                        servo_config.device[ channel ].destination_angle = servo_config.device[ channel ].min_angle;
                        if( servo_config.device[ channel ].state ) {
                            servo_config.device[ channel ].state = false;
                            servo_config.save();
                        }
                    }
                    else if( doc.containsKey("max") && channel >= 0 && channel < MAX_SERVOS ) {
                        servo_config.device[ channel ].destination_angle = servo_config.device[ channel ].max_angle;
                        if( !servo_config.device[ channel ].state ) {
                            servo_config.device[ channel ].state = true;
                            servo_config.save();
                        }
                    }
                }
            }
            retval = true;
            break;
        case MQTTTELE_JSON_DATA:
            /**
             * check json data
             */
            ASSERT( mqttData->doc, MODULE_NAME " mqttstat json data is NULL" );
            retval = true;
            break;
    }

    return( retval );
}