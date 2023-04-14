/**
 * @file onewire.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "config.h"
#include "core/webserver.h"
#include "core/mqttclient.h"
#include "core/modul_mgmt.h"
#include "core/utils/alloc.h"

#include "1wire.h"
#include "config/onewire_config.h"
/**
 * module namespace
 */
#define MODULE_NAME                "onewire"
/**
 * local variables
 */
OneWire oneWire;
DallasTemperature sensors( &oneWire );
temp_sensors_t *tempsensor = NULL;
static int sensorcount = 0;
onewire_config_t onewire_config;
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

/**
 * @brief init function for template
 * 
 * @param event         event from init
 * @param arg           pointer to data
 * @return true         init was successfull
 * @return false        init was not successfull
 */
static bool initialize( EventBits_t event, void *arg ) {
    /**
     * check if already initialized
     */
    ASSERT( !initialized, MODULE_NAME " are not deinitialized, check your code" );
    /**
     *  load config
     */
    onewire_config.load();
    /**
     * init sensors and get all sensors and store thair address
     */
    tempsensor = (temp_sensors_t *)CALLOC_ASSERT( sizeof( temp_sensors_t ), MAX_SENSORS, "onewire calloac failed" );
    oneWire.begin( onewire_config.pin );
    /**
     * search for sensors
     */
    while( oneWire.search( tempsensor[ sensorcount ].addr ) && sensorcount < MAX_SENSORS )
        sensorcount++;
    /**
     * request temperature
     */
    oneWire.reset_search();
    delay(250);
    sensors.requestTemperatures();
    delay(250);
    /**
     * print all sensors
     */
    for( int i = 0 ; i < sensorcount && i < MAX_SENSORS; i++ ) {
        tempsensor[ i ].temp = onewire_config.reportInFahrenheit ? sensors.getTempF( tempsensor[ i ].addr ) : sensors.getTempC( tempsensor[ i ].addr );
        tempsensor[ i ].temp_str = String( tempsensor[ i ].temp ) + "°" + ( onewire_config.reportInFahrenheit ? "F" : "C" );
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
    log_i( MODULE_NAME " module initialized, found %d sensors", sensorcount );
    /**
     * return success
     */
    return( true );
}

/**
 * @brief deinit function for template
 * 
 * @param event         event from deinit
 * @param arg           pointer to data
 * @return true         deinit was successfull
 * @return false        deinit was not successfull
 */
static bool deinitialize( EventBits_t event, void *arg ) {
    /**
     * check if servo is deinitialized
     */
    if( !initialized ) {
        log_e( MODULE_NAME " already deinitialized");
        return( false );
    }
    /**
     *  save config
     */
    onewire_config.save();
    /**
     *  deinitialize your hardware
     */
    pinMode( onewire_config.pin, INPUT );
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
 * @brief loop function for template
 * 
 * @param event         event from loop
 * @param arg           pointer to data
 * @return true         loop was successfull
 * @return false        loop was not successfull
 */
static bool loop( EventBits_t event, void *arg ) {
    static uint64_t NextMillis = millis();
    static uint64_t NextTempMillis = 0;
    /**
     * check if module is initialized
     */
    if( !initialized ) {
        log_e( MODULE_NAME " not initialized");
        return( false );
    }
    /**
     * run loop every 60s
     */
    if( NextMillis < millis() ) {
        /**
         * reset NextMillis time to run loop in 60s
         */
        NextMillis = millis() + onewire_config.interval * 1000l;
        /**
         * request new sensor data and set NextTempMillis time to get temperature in 1s
         */
        sensors.requestTemperatures();
        NextTempMillis = millis() + 2000l;
    }
    /**
     * loop to get temperature when NextTempMillis is reached AND NextTempMillis is not 0
     */
    if( NextTempMillis < millis() && NextTempMillis != 0 ) {
        NextTempMillis = 0;
        /**
         * get new sensor data
         */
        for( size_t i = 0 ; i < sensorcount && i < MAX_SENSORS; i++ ) {
            /**
             * get temperature
             */
            float temp = onewire_config.reportInFahrenheit ? sensors.getTempF( tempsensor[ i ].addr ) : sensors.getTempC( tempsensor[ i ].addr );
            /**
             * store temp and convert to string
             */
            if( temp > -20.0 && temp < 100.0 ) {
                tempsensor[ i ].temp = temp;
                tempsensor[ i ].temp_str = String( tempsensor[ i ].temp ) + "°" + String( onewire_config.reportInFahrenheit ? "F" : "C" );
            }
        }
    }
    /**
     * return success
     */
    return( true );
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
    ASSERT( initialized, MODULE_NAME " not initialized" );

    switch( event ) {
        case WS_DATA:
            ASSERT( client, MODULE_NAME " client is NULL" );
            /**
             * check for SAV command
             */
            if ( !strcmp( "save_" MODULE_NAME "_settings", cmd ) ) {
                onewire_config.save();
                asyncwebserver_send_websocket_msg( "status\\Save" );
            }
            /**
             * check get status command
             */
            else if ( !strcmp("get_" MODULE_NAME "_status", cmd ) ) {
                for( size_t i = 0 ; i < sensorcount && MAX_SENSORS; i++ ) {
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_sensor_%d\\%s", i, tempsensor[ i ].temp_str.c_str() );
                }
            }
            /**
             * check get settings command
             */
            else if ( !strcmp("get_" MODULE_NAME "_settings", cmd ) ) {
                asyncwebserver_send_websocket_msg( MODULE_NAME "_pin\\%d", onewire_config.pin );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_interval\\%d", onewire_config.interval );
                asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_mqtt_msg_stat\\%s", onewire_config.mqtt_msg_stat ? "true" : "false " );
                for( int i = 0 ; i < sensorcount && i < MAX_SENSORS; i++ )
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_sensor_%d\\%s", i, tempsensor[ i ].temp_str.c_str() );
            }
            /**
             * store and set pin
             */
            else if ( !strcmp( MODULE_NAME "_pin", cmd ) ) {
                onewire_config.pin = atoi( value );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_pin\\%d", onewire_config.pin );
            }
            /**
             * store and set temp read interval
             */
            else if ( !strcmp( MODULE_NAME "_interval", cmd ) ) {
                onewire_config.interval = atoi( value );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_interval\\%d", onewire_config.interval );
            }
            /**
             * store and set mqtt_msg_stat
             */
            else if ( !strcmp ( MODULE_NAME "_mqtt_msg_stat", cmd ) ) {
                onewire_config.mqtt_msg_stat = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_mqtt_msg_stat\\%s", onewire_config.mqtt_msg_stat ? "true" : "false " );
            }
            /**
             * store and set report_in_fahrenheit
             */
            else if ( !strcmp( MODULE_NAME "_report_in_fahrenheit", cmd ) ) {
                onewire_config.reportInFahrenheit = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_report_in_fahrenheit\\%s", onewire_config.reportInFahrenheit ? "true" : "false " );
                for( int i = 0 ; i < sensorcount && i < MAX_SENSORS; i++ )
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_sensor_%d\\%s", i, tempsensor[ i ].temp_str.c_str() );
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
            html += onewire_config_head;
            html += "  <h2>sensor data</h2>\n"
                    "  <div class='vbox'>\n"
                    "    <div class='box'>\n";
            for( size_t i = 0 ; i < sensorcount && i < MAX_SENSORS ; i++ ) {
                html += "      <label>sensor " + String( i ) + "</label><input type='text' size='32' id='" + MODULE_NAME + "_sensor_" + String( i ) + "' value='" + String( tempsensor[ i ].temp_str ) + "' disabled>\n";
            }
            html += "    </div>\n"
                    "  </div>\n";
            html += onewire_config_footer;
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
            onewire_config.save();
            retval = true;
            break;
        case RESET_CONFIG:
            /**
             * reset config
             */
            onewire_config.resetToDefault();
            retval = true;
            break;  
    }

    return( retval );
}

/**
 * @brief callback function for mqtt client
 * 
 * @param event         event from mqtt client
 * @param arg           pointer to data
 * @return true         callback function was successfull
 * @return false        callback function was not successfull
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
            ASSERT( mqttData->doc, MODULE_NAME " mqttstat json data is NULL" );
            /**
             * send sensor data
             */
            for( size_t i = 0 ; i < sensorcount; i++ ) {
                char addr[32] = "";
                snprintf( addr, sizeof( addr ), "%02x:%02x:%02x:%02x:%02x:%02x", tempsensor[ i ].addr[ 5 ], tempsensor[ i ].addr[ 4 ], tempsensor[ i ].addr[ 3 ], tempsensor[ i ].addr[ 2 ], tempsensor[ i ].addr[ 1 ], tempsensor[ i ].addr[ 0 ] );
                doc["sensor"][ i ]["addr"] = addr;
                doc["sensor"][ i ]["temp"] = tempsensor[ i ].temp;
                doc["sensor"][ i ]["temp_str"] = tempsensor[ i ].temp_str;
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

    return( retval );
}