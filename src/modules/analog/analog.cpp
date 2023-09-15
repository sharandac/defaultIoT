/**
 * @file analog.cpp
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

#include "analog.h"
#include "config/json_config.h"
/**
 * analog module namespace
 */
#define MODULE_NAME                "analog"
/**
 * local variables
 */
static bool initialized = false;
analog_config_t analog_config;
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
     *  load config
     */
    analog_config.load();
    /**
     * hardware initialization
     */
    for( size_t i = 0 ; i < analog_config.count && i < MAX_ANALOG; i++ ) {
        if( analog_config.device[i].enable ) {
            pinMode( analog_config.device[i].pin, INPUT );
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
     * check if already deinitialized
     */
    ASSERT( initialized, MODULE_NAME " are not initialized, check your code" );
    /**
     * save config
     */
    analog_config.save();
    /**
     * hardware deinitialization
     */
    for( size_t i = 0 ; i < analog_config.count && i < MAX_ANALOG; i++ ) {
        if( analog_config.device[i].enable ) {
            pinMode( analog_config.device[i].pin, INPUT );
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
    ASSERT( initialized, MODULE_NAME " are not in initialized, check your code" );
    /**
     * run loop every 100ms
     */
    if( NextMillis < millis() ) {
        NextMillis = millis() + 100l;

        for( size_t i = 0 ; i < analog_config.count && i < MAX_ANALOG; i++ ) {
            if( analog_config.device[i].enable ) {
                /**
                 * read analog value
                 */
                uint16_t analogValue = analogRead( analog_config.device[i].pin );
                analog_config.device[i].value = analogValue;
                /**
                 * set min and max value
                 */
                if( analog_config.device[i].value > analog_config.device[i].analog_max ) {
                    analog_config.device[i].analog_max = analog_config.device[i].value;
                }
                if( analog_config.device[i].value < analog_config.device[i].analog_min ) {
                    analog_config.device[i].analog_min = analog_config.device[i].value;
                }
                /**
                 * check high trigger
                 */
                if ( analog_config.device[i].value > analog_config.device[ i ].trigger_high ) {
                    if ( analog_config.device[i].analog_trigger == false ) {
                        analog_config.device[i].analog_trigger = true;
                    }
                }
                /**
                 * check low trigger and count
                 */
                if ( analog_config.device[i].value < analog_config.device[ i ].trigger_low ) {
                    if ( analog_config.device[i].analog_trigger == true ) {
                        analog_config.device[i].analog_trigger = false;
                        analog_config.device[i].trigger_count++;
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
            /**
             * send analog pin count
             */
            doc[ MODULE_NAME ]["count"] = analog_config.count;
            /**
             * send analog data
             */
            for( size_t i = 0 ; i < analog_config.count && MAX_ANALOG ; i++ ) {
                doc[ MODULE_NAME ]["pin"][ i ]["pin"] = analog_config.device[ i ].pin;
                doc[ MODULE_NAME ]["pin"][ i ]["id"] = analog_config.device[ i ].id;
                doc[ MODULE_NAME ]["pin"][ i ]["value"] = analog_config.device[ i ].value;
                doc[ MODULE_NAME ]["pin"][ i ]["min"] = analog_config.device[ i ].analog_min;
                doc[ MODULE_NAME ]["pin"][ i ]["max"] = analog_config.device[ i ].analog_max;
                doc[ MODULE_NAME ]["pin"][ i ]["triggercount"] = analog_config.device[ i ].trigger_count;

                if( analog_config.device[ i ].trigger_count && analog_config.device[ i ].trigger_count_reset ) {
                    analog_config.device[ i ].analog_min = 4095;
                    analog_config.device[ i ].analog_max = 0;
                    analog_config.device[ i ].trigger_count = 0;
                }
            }
            retval = true;
            break;
        case MQTTCMND_JSON_DATA:
            /**
             * check json data
             */
            ASSERT( mqttData->doc, MODULE_NAME " mqttstat json data is NULL" );
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
             * check SAV command
             */
            if ( !strcmp( "save_" MODULE_NAME "_settings", cmd ) ) {
                analog_config.save();
                asyncwebserver_send_websocket_msg( "status\\Save" );
            }
            /**
             * check get status command
             */
            else if ( !strcmp( "get_" MODULE_NAME "_status", cmd ) ) {
                for( size_t i = 0 ; i < analog_config.count && i < MAX_ANALOG; i++ ) {
                    if( analog_config.device[i].enable ) {
                        asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_value\\%d, min %d, max %d", i, analog_config.device[ i ].value, analog_config.device[ i ].analog_min, analog_config.device[ i ].analog_max );
                        asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_trigger_count\\%d", i, analog_config.device[ i ].trigger_count );
                    }
                }
            }
            /**
             * check get settings command
             */
            else if ( !strcmp("get_" MODULE_NAME "_settings", cmd ) ) {
                asyncwebserver_send_websocket_msg("option\\" MODULE_NAME "_count\\%d", analog_config.count );
                for( int i = 0 ; i < analog_config.count && i < MAX_ANALOG; i++ ) {
                    asyncwebserver_send_websocket_msg( "option\\" MODULE_NAME "_%d_pin\\%d", i, analog_config.device[ i ].pin );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_id\\%s", i, analog_config.device[ i ].id );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_trigger_low\\%d", i, analog_config.device[ i ].trigger_low );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_trigger_high\\%d", i, analog_config.device[ i ].trigger_high );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_trigger_count\\%d", i, analog_config.device[ i ].trigger_count );
                    asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_%d_trigger_count_reset\\%s", i, analog_config.device[i].trigger_count_reset ? "true" : "false " );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_value\\%d", i, analog_config.device[ i ].value );
                }
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_mqtt_msg_stat\\%s", analog_config.mqtt_msg_stat ? "true" : "false " );
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_mqtt_msg_on_change\\%s", analog_config.mqtt_msg_on_change ? "true" : "false " );
            }
            /**
             * store and set pin count
             */
            else if ( !strcmp ( MODULE_NAME "_count", cmd ) ) {
                analog_config.count = atoi( value );
                asyncwebserver_send_websocket_msg( "option\\" MODULE_NAME "_count\\%d", analog_config.count );
            }
            /**
             * store and set mqtt_msg_stat
             */
            else if ( !strcmp ( MODULE_NAME "_mqtt_msg_stat", cmd ) ) {
                analog_config.mqtt_msg_stat = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_mqtt_msg_stat\\%s", analog_config.mqtt_msg_stat ? "true" : "false " );
            }
            /**
             * store and set mqtt_msg_on_change
             */
            else if ( !strcmp( MODULE_NAME "_mqtt_msg_on_change", cmd ) ) {
                analog_config.mqtt_msg_on_change = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_mqtt_msg_on_change\\%s", analog_config.mqtt_msg_on_change ? "true" : "false " );
            }
            /**
             * crawl through all analog pins and thair options
             */
            for( int i = 0 ; i < analog_config.count && i < MAX_ANALOG; i++ ) {
                char temp[32];
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_pin", i );
                if ( !strcmp( temp, cmd ) ) {
                    pinMode( analog_config.device[ i ].pin, INPUT );
                    analog_config.device[ i ].pin = atoi( value );
                    asyncwebserver_send_websocket_msg( "option\\" MODULE_NAME "_%d_pin\\%d", i, analog_config.device[ i ].pin );
                }
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_id", i );
                if ( !strcmp( temp, cmd ) ) {
                    strncpy( analog_config.device[ i ].id, value, sizeof( analog_config.device[ i ].id ) );
                    asyncwebserver_send_websocket_msg( "option\\" MODULE_NAME "_%d_id\\%s", i, analog_config.device[ i ].id );
                }
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_trigger_low", i );
                if ( !strcmp( temp, cmd ) ) {
                    analog_config.device[ i ].trigger_low = atoi( value );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_trigger_low\\%d", i, analog_config.device[ i ].trigger_low );
                }
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_trigger_high", i );
                if ( !strcmp( temp, cmd ) ) {
                    analog_config.device[ i ].trigger_high = atoi( value );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_%d_trigger_high\\%d", i, analog_config.device[ i ].trigger_high );
                }
                snprintf( temp, sizeof( temp ), MODULE_NAME "_%d_trigger_count_reset", i );
                if ( !strcmp( temp, cmd ) ) {
                    analog_config.device[ i ].trigger_count_reset = atoi( value ) ? true : false;
                    asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_%d_trigger_count_reset\\%s", i, analog_config.device[i].trigger_count_reset ? "true" : "false " );
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
            html += analog_config_head;
            html += "  <div class='vbox'>\n"
                    "    <label>count</label><br>\n"
                    "    <div class='box'>\n"
                    "      <select id='" MODULE_NAME "_count' onchange='SendSetting(\"" MODULE_NAME "_count\");location.reload();'>";
            for( int i = 0 ; i <= MAX_ANALOG ; i++ )
                html += "<option value='" + String( i ) + "'>" + String( i ) + "</option>";
            html += "</select>\n"
                    "    </div>\n"
                    "  </div>\n";
            for( int i = 0 ; i < analog_config.count && i < MAX_ANALOG; i++ ) {
                html += "  <div class='vbox'>\n"
                        "    <label>" MODULE_NAME " " + String( i ) + "</label><br>\n"
                        "    <div class='box'>\n"
                        "      <label>pin</label><select id='" MODULE_NAME "_" + String( i ) + "_pin'><option value='36'>ADC1-CH0-GPIO36 (VP)</option><option value='39'>ADC1-CH3 GPIO39 (VN)</option><option value='32'>ADC1-CH4 GPIO32</option><option value='33'>ADC1-CH5 GPIO33</option><option value='34'>ADC1-CH6 GPIO34</option><option value='35'>ADC1-CH7 GPIO35</option></select>"
                        "      <label>id</label><input type='text' size='32' id='" MODULE_NAME "_" + String( i ) + "_id'>\n"
                        "      <label>value</label><input type='text' size='32' id='" MODULE_NAME "_" + String( i ) + "_value' disabled>\n"
                        "      <label>trigger low</label><input type='text' size='32' id='" MODULE_NAME "_" + String( i ) + "_trigger_low'>\n"
                        "      <label>trigger high</label><input type='text' size='32' id='" MODULE_NAME "_" + String( i ) + "_trigger_high'>\n"
                        "      <input type='checkbox' id='" MODULE_NAME "_" + String( i ) + "_trigger_count_reset'><label>reset trigger count after mqtt status msg</label></input>\n"
                        "      <label>trigger count</label><input type='text' size='32' id='" MODULE_NAME "_" + String( i ) + "_trigger_count' disabled>\n"
                        "    </div>\n"
                        "  </div>\n";
            }
            html += analog_config_footer;
            for( int i = 0 ; i < analog_config.count && i < MAX_ANALOG; i++ ) {
                html += "SendSetting(\"" MODULE_NAME "_" + String( i ) + "_pin\");"
                        "SendSetting(\"" MODULE_NAME "_" + String( i ) + "_id\");"
                        "SendSetting(\"" MODULE_NAME "_" + String( i ) + "_trigger_low\");"
                        "SendSetting(\"" MODULE_NAME "_" + String( i ) + "_trigger_high\");"
                        "SendCheckboxSetting(\"" MODULE_NAME "_" + String( i ) + "_trigger_count_reset\");";
            }
            html += analog_config_sendBTN;
            html += html_footer;
            request->send(200, "text/html", html);
            retval = true;
            break;
        case WEB_MENU:
            asyncwebserver_set_menu_entry( "/" MODULE_NAME ".htm", MODULE_NAME );
            retval = true;
            break;
        case SAVE_CONFIG:    
            analog_config.save();
            retval = true;
            break;
        case RESET_CONFIG:
            analog_config.resetToDefault();
            retval = true;
            break;  
    }
    /**
     * webserver callback function
     */
    return( retval );
}