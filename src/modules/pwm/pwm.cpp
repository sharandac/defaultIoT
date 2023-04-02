/**
 * @file pwm.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <Arduino.h>
#include "config.h"
#include "core/webserver.h"
#include "core/mqttclient.h"
#include "core/modul_mgmt.h"
#include "core/utils/alloc.h"

#include "pwm.h"
#include "config/pwm_config.h"
/**
 * analog module namespace
 */
#define MODULE_NAME                "pwm"
#define MODULE_CONFIG_NAME         pwm_config
/**
 * local variables
 */
static bool initialized = false;
pwm_config_t MODULE_CONFIG_NAME;
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

/**
 * @brief pwm module init callback function
 * 
 * @param event         event from the event handler
 * @param arg           pointer to the argument
 * @return true         if success
 * @return false        if failed
 */
static bool initialize( EventBits_t event, void *arg ) {
    /**
     * check if already initialized
     */
    ASSERT( !initialized, MODULE_NAME " are not deinitialized, check your code" );
    /**
     * load config
     */
    pwm_config.load();
    /**
     * hardware initialization
     */
    for( int i = 0 ; i < pwm_config.pwm_count && i < MAX_PWM; i++ ) {
        /**
         * setup PWM port with 25kHz
         */
        if( pwm_config.device[ i ].enaled ) {
            pinMode( pwm_config.device[ i ].pin, OUTPUT );
            ledcSetup( i, pwm_config.device[ i ].frequency, 8 );
            ledcAttachPin( pwm_config.device[ i ].pin, i );
            ledcWrite( i, pwm_config.device[ i ].min );
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

/**
 * @brief pwm module deinit callback function
 * 
 * @param event     event from the event handler
 * @param arg       pointer to the argument
 * @return true     if success
 * @return false    if failed
 */
static bool deinitialize( EventBits_t event, void *arg ) {
    /**
     * check if already initialized
     */
    ASSERT( initialized, MODULE_NAME " are not initialized, check your code" );
    /**
     * load config
     */
    pwm_config.save();
    /**
     * hardware deinitialization
     */
    for( int i = 0 ; i < pwm_config.pwm_count && i < MAX_PWM; i++ ) {
        /**
         * setup PWM port with 25kHz
         */
        if( pwm_config.device[ i ].enaled ) {
            ledcDetachPin( pwm_config.device[ i ].pin );
            pinMode( pwm_config.device[ i ].pin, INPUT );
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

/**
 * @brief pwm module loop callback function
 * 
 * @param event     event from the event handler
 * @param arg       pointer to the argument
 * @return true     if success
 * @return false    if failed
 */
static bool loop( EventBits_t event, void *arg ) {
    static uint64_t NextMillis = 0;
    /**
     * check if already init
     */
    ASSERT( initialized, MODULE_NAME " are not in initialized, check your code" );
    /**
     * first run init all things
     */
    if( NextMillis == 0 ) {
        NextMillis = millis();
    }
    /**
     * work in output
     */
    if( NextMillis < millis() ) {
        NextMillis += 50l;
        
    }
    /**
     * loop function
     */
    return( true );
}

/**
 * @brief pwm module mqtt client callback function
 * 
 * @param event     event from the event handler
 * @param arg       pointer to the argument
 * @return true     if success
 * @return false    if failed
 */
static bool mqttclient_cb( EventBits_t event, void *arg ) {
    bool retval = false;
    mqttData_t *mqttData = (mqttData_t*)arg;
    DynamicJsonDocument& doc = *mqttData->doc;
    /**
     * check if servo is initialized
     */
    ASSERT( initialized, MODULE_NAME " not initialized" );
    /**
     * check event type
     */
    switch( event ) {
        case MQTTSTAT_JSON_DATA:
            /**
             * check json data
             */
            ASSERT( mqttData->doc, MODULE_NAME " mqttstat json data is NULL" );
            /*
             *
             */
            if( pwm_config.mqtt_msg_stat ) {
                doc[ MODULE_NAME ]["count"] = pwm_config.pwm_count;
                for( int i = 0 ; i < pwm_config.pwm_count && i < MAX_PWM; i++ ) {
                    if( pwm_config.device[ i ].enaled ) {
                        doc[ MODULE_NAME ]["pin"][i]["pin"] = pwm_config.device[ i ].pin;
                        doc[ MODULE_NAME ]["pin"][i]["value"] = pwm_config.device[ i ].value;
                        doc[ MODULE_NAME ]["pin"][i]["frequency"] = pwm_config.device[ i ].frequency;
                        doc[ MODULE_NAME ]["pin"][i]["min"] = pwm_config.device[ i ].min;
                        doc[ MODULE_NAME ]["pin"][i]["max"] = pwm_config.device[ i ].max;
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
                if( doc.containsKey("value") && channel >= 0 && channel < MAX_PWM ) {
                    if( pwm_config.device[ channel ].enaled ) {
                        pwm_config.device[ channel ].value = doc["value"];
                        if( pwm_config.device[ channel ].value > pwm_config.device[ channel ].max ) {
                            pwm_config.device[ channel ].value = pwm_config.device[ channel ].max;
                        }
                        else if( pwm_config.device[ channel ].value < pwm_config.device[ channel ].min ) {
                            pwm_config.device[ channel ].value = pwm_config.device[ channel ].min;
                        }
                        ledcWrite( channel, pwm_config.device[ channel ].value );
                        asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_value\\%d", channel, pwm_config.device[ channel ].value );
                    }
                }
                if( doc.containsKey("frequency") && channel >= 0 && channel < MAX_PWM ) {
                    if( pwm_config.device[ channel ].enaled ) {
                        pwm_config.device[ channel ].frequency = doc["frequency"];
                        ledcSetup( channel, pwm_config.device[ channel ].frequency, 8 );
                        ledcWrite( channel, pwm_config.device[ channel ].value );
                        asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_frequency\\%d", channel, pwm_config.device[ channel ].frequency );
                    }
                }
                if( doc.containsKey("max") && channel >= 0 && channel < MAX_PWM ) {
                    if( pwm_config.device[ channel ].enaled ) {
                        pwm_config.device[ channel ].max = doc["max"];
                        ledcSetup( channel, pwm_config.device[ channel ].frequency, 8 );
                        ledcWrite( channel, pwm_config.device[ channel ].value );
                        asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_max\\%d", channel, pwm_config.device[ channel ].max );
                    }
                }
                if( doc.containsKey("min") && channel >= 0 && channel < MAX_PWM ) {
                    if( pwm_config.device[ channel ].enaled ) {
                        pwm_config.device[ channel ].min = doc["min"];
                        ledcSetup( channel, pwm_config.device[ channel ].frequency, 8 );
                        ledcWrite( channel, pwm_config.device[ channel ].value );
                        asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_min\\%d", channel, pwm_config.device[ channel ].min );
                    }
                }
            }
            if( doc.containsKey( "store" ) ) {
                if( doc[ "store" ] == true )
                    pwm_config.save();
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
    /**
     * mqtt client callback function
     */
    return( retval );
}

/**
 * @brief pwm module webserver callback function
 * 
 * @param event     event from the event handler
 * @param arg       pointer to the argument
 * @return true     if success
 * @return false    if failed
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
                pwm_config.save();
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
                asyncwebserver_send_websocket_msg("option\\" MODULE_NAME "_count\\%d", pwm_config.pwm_count );

                for( int i = 0 ; i < pwm_config.pwm_count && i < MAX_PWM; i++ ) {
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_pin\\%d", i, pwm_config.device[ i ].pin );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_frequency\\%d", i, pwm_config.device[ i ].frequency );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_value\\%d", i, pwm_config.device[ i ].value );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_min\\%d", i, pwm_config.device[ i ].min );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_max\\%d", i, pwm_config.device[ i ].max );
                }

                asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_mqtt_msg_stat\\%s", pwm_config.mqtt_msg_stat ? "true" : "false " );
                asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_mqtt_msg_on_change\\%s", pwm_config.mqtt_msg_on_change ? "true" : "false " );
            }
            /**
             * store and set pin count
             */
            else if ( !strcmp ( MODULE_NAME "_count", cmd ) ) {
                pwm_config.pwm_count = atoi( value );
                asyncwebserver_send_websocket_msg("option\\" MODULE_NAME "_count\\%d", pwm_config.pwm_count );
            }
            /**
             * store and set mqtt_msg_stat
             */ 
            else if ( !strcmp ( MODULE_NAME "_mqtt_msg_stat", cmd ) ) {
                pwm_config.mqtt_msg_stat = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_mqtt_msg_stat\\%s", pwm_config.mqtt_msg_stat ? "true" : "false " );
            }
            /**
             * store and set mqtt_msg_on_change
             */
            else if ( !strcmp( MODULE_NAME "_mqtt_msg_on_change", cmd ) ) {
                pwm_config.mqtt_msg_on_change = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_mqtt_msg_on_change\\%s", pwm_config.mqtt_msg_on_change ? "true" : "false " );
            }
            /**
             * crawl through all output pins and thair options
             */
            for( int i = 0 ; i < pwm_config.pwm_count && i < MAX_PWM; i++ ) {
                char temp[32];
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_pin", i );
                if ( !strcmp( temp, cmd ) ) {
                    ledcDetachPin( pwm_config.device[ i ].pin );
                    pwm_config.device[ i ].pin = atoi( value );
                    pwm_config.device[ i ].value = 0;
                    ledcSetup( i, pwm_config.device[ i ].frequency, 8 );
                    ledcAttachPin( pwm_config.device[ i ].pin, i );
                    ledcWrite( i, pwm_config.device[ i ].value );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_pin\\%d", i, pwm_config.device[ i ].pin );
                }
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_frequency", i );
                if ( !strcmp( temp, cmd ) ) {
                    pwm_config.device[ i ].frequency = atoi( value );
                    ledcSetup( i, pwm_config.device[ i ].frequency, 8 );
                    ledcWrite( i, pwm_config.device[ i ].value );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_frequency\\%d", i, pwm_config.device[ i ].frequency );
                }
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_value", i );
                if ( !strcmp( temp, cmd ) ) {
                    pwm_config.device[ i ].value = atoi( value );
                    if( pwm_config.device[ i ].value > pwm_config.device[ i ].max ) {
                        pwm_config.device[ i ].value = pwm_config.device[ i ].max;
                    }
                    else if( pwm_config.device[ i ].value < pwm_config.device[ i ].min ) {
                        pwm_config.device[ i ].value = pwm_config.device[ i ].min;
                    }
                    ledcWrite( i, pwm_config.device[ i ].value );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_value\\%d", i, pwm_config.device[ i ].value );
                }
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_min", i );
                if ( !strcmp( temp, cmd ) ) {
                    pwm_config.device[ i ].min = atoi( value );
                    if( pwm_config.device[ i ].value < pwm_config.device[ i ].min ) {
                        ledcWrite( i, pwm_config.device[ i ].min );
                        pwm_config.device[ i ].value = pwm_config.device[ i ].min;
                    }
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_value\\%d", i, pwm_config.device[ i ].value );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_min\\%d", i, pwm_config.device[ i ].min );
                }
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_max", i );
                if ( !strcmp( temp, cmd ) ) {
                    pwm_config.device[ i ].max = atoi( value );
                    if( pwm_config.device[ i ].value > pwm_config.device[ i ].max ) {
                        ledcWrite( i, pwm_config.device[ i ].max );
                        pwm_config.device[ i ].value = pwm_config.device[ i ].max;
                    }
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_value\\%d", i, pwm_config.device[ i ].value );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_max\\%d", i, pwm_config.device[ i ].max );
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
            html += pwm_config_page_head;
            html += "  <div class='vbox'>\n"
                    "    <label>pin count</label><br>\n"
                    "    <div class='box'>\n"
                    "      <select id='" MODULE_NAME "_count' onchange='SendSetting(\"" MODULE_NAME "_count\");location.reload();'>";
            for( int i = 0 ; i <= MAX_PWM ; i++ )
                html += "<option value='" + String( i ) + "'>" + String( i ) + "</option>";
            html += "</select>\n"
                    "    </div>\n"
                    "  </div>\n";
            for( int i = 0 ; i < pwm_config.pwm_count && i < MAX_PWM; i++ ) {
                html += "  <div class='vbox'>\n"
                        "    <label>" MODULE_NAME " " + String( i ) + "</label><br>\n"
                        "    <div class='box'>\n"
                        "      <label>pin</label><input type='text' size='32' id='" MODULE_NAME "_" + String( i ) + "_pin'>\n"
                        "      <label>frequency</label><input type='text' size='32' id='" MODULE_NAME "_" + String( i ) + "_frequency'>\n"
                        "      <label>current value</label><input type='text' size='32' id='" MODULE_NAME "_" + String( i ) + "_value'>\n"
                        "      <label>min value</label><input type='text' size='32' id='" MODULE_NAME "_" + String( i ) + "_min'>\n"
                        "      <label>max value</label><input type='text' size='32' id='" MODULE_NAME "_" + String( i ) + "_max'>\n"
                        "    </div>\n"
                        "  </div>\n";
            }
            html += pwm_config_page_footer;
            html += "SendSetting(\"" MODULE_NAME "_count\");";
            for( int i = 0 ; i < pwm_config.pwm_count && i < MAX_PWM; i++ ) {
                html += "SendSetting(\"" MODULE_NAME "_" + String( i ) + "_pin\");"
                        "SendSetting(\"" MODULE_NAME "_" + String( i ) + "_frequency\");"
                        "SendSetting(\"" MODULE_NAME "_" + String( i ) + "_value\");"
                        "SendSetting(\"" MODULE_NAME "_" + String( i ) + "_min\");"
                        "SendSetting(\"" MODULE_NAME "_" + String( i ) + "_max\");";
            }
            html += pwm_config_page_sendBTN;
            html += html_footer;
            request->send(200, "text/html", html);
            retval = true;
            break;
        case WEB_MENU:
            asyncwebserver_set_menu_entry( "/" MODULE_NAME ".htm", MODULE_NAME );
            retval = true;
            break;
        case SAVE_CONFIG:
            pwm_config.save();
            retval = true;
            break;
        case RESET_CONFIG:
            pwm_config.resetToDefault();
            retval = true;
            break;  
    }
    /**
     * webserver callback function
     */
    return( retval );
}