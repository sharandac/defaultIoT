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
#include <Wire.h>

#include "config.h"
#include "core/webserver.h"
#include "core/mqttclient.h"
#include "core/modul_mgmt.h"
#include "core/utils/alloc.h"

#include "i2c.h"
#include "config/i2c_config.h"
/**
 * module namespace
 */
#define MODULE_NAME                "i2c"
/**
 * local variables
 */
i2c_config_t i2c_config;
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
    int scan_hit = 0;
    /**
     * check if already initialized
     */
    ASSERT( !initialized, MODULE_NAME " are not deinitialized, check your code" );
    /**
     *  load config
     */
    i2c_config.load();
    /**
     * init wire interface and scan
     */
    Wire.begin( i2c_config.sda, i2c_config.scl, i2c_config.frequency * 1000ul  );
    for( int i = 0; i < 0x7f; i++ ) {
        Wire.beginTransmission( i );
        if( !Wire.endTransmission() ) {
            scan_hit++;
            log_i("device found at %x", i );
        }
    }
    log_i("scan i2c done, found %d devices", scan_hit );
    /**
     * set webserver and mqtt client callback function active
     */
    asyncwebserver_set_cb_active( webserver_cb, true );
    mqtt_client_set_cb_active( mqttclient_cb, true );
    /**
     * set initialized flag
     */
    initialized = true;
    log_i( MODULE_NAME " module initialized" );
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
    i2c_config.save();
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
        NextMillis = millis() + 1000l;
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
                i2c_config.save();
                asyncwebserver_send_websocket_msg( "status\\Save" );
            }
            /**
             * check get status command
             */
            else if ( !strcmp("get_" MODULE_NAME "_status", cmd ) ) {
            }
            /**
             * check get settings command
             */
            else if ( !strcmp("get_" MODULE_NAME "_settings", cmd ) ) {
                asyncwebserver_send_websocket_msg( MODULE_NAME "_frequency\\%d", i2c_config.frequency );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_scl\\%d", i2c_config.scl );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_sda\\%d", i2c_config.sda );
                asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_mqtt_msg_stat\\%s", i2c_config.mqtt_msg_stat ? "true" : "false " );
            }
            /**
             * set cpu frequency
             */
            else if ( !strcmp ( MODULE_NAME "_frequency", cmd ) ) {
                i2c_config.frequency = atoi( value );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_frequency\\%d", i2c_config.frequency );
            }
            /**
             * store and set pin
             */
            else if ( !strcmp( MODULE_NAME "_scl", cmd ) ) {
                i2c_config.scl = atoi( value );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_scl\\%d", i2c_config.scl );
            }
            /**
             * store and set pin
             */
            else if ( !strcmp( MODULE_NAME "_sda", cmd ) ) {
                i2c_config.sda = atoi( value );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_sda\\%d", i2c_config.sda );
            }
            /**
             * store and set mqtt_msg_stat
             */
            else if ( !strcmp ( MODULE_NAME "_mqtt_msg_stat", cmd ) ) {
                i2c_config.mqtt_msg_stat = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_mqtt_msg_stat\\%s", i2c_config.mqtt_msg_stat ? "true" : "false " );
            }
            /**
             * if i2c config valid and enabled, init i2c
             */
            else if ( !strcmp( MODULE_NAME "_reinit", cmd ) ) {
                if( i2c_config.enabled ) {
                    int scan_hit = 0;
                    Wire.begin( i2c_config.sda, i2c_config.scl, i2c_config.frequency * 1000ul  );
                    for( int i = 0; i < 0x7f; i++ ) {
                        Wire.beginTransmission( i );
                        if( !Wire.endTransmission() ) {
                            scan_hit++;
                            log_i("device found at %x", i );
                        }
                    }
                    log_i("scan i2c done, found %d devices", scan_hit );                
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
            html += i2c_config_head;
            html += i2c_config_footer;
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
            i2c_config.save();
            retval = true;
            break;
        case RESET_CONFIG:
            /**
             * reset config
             */
            i2c_config.resetToDefault();
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
             * insert input state data
             */
            doc[ MODULE_NAME ]["enabled"] = i2c_config.enabled;
            doc[ MODULE_NAME ]["frequency"] = i2c_config.frequency;
            doc[ MODULE_NAME ]["pin"]["scl"] = i2c_config.scl;
            doc[ MODULE_NAME ]["pin"]["sda"] = i2c_config.sda;
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