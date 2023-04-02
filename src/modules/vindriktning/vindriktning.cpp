/**
 * @file vindriktning.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-19
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

#include "vindriktning.h"
#include "config/vindriktning_config.h"

#include "pm1006.hpp"
/**
 * module namespace
 */
#define MODULE_NAME                "vindriktning"
/**
 * local variables
 */
static bool initialized = false;
vindriktning pm1006;                                                                    /** @brief pm1006 object */
vindriktning_config_t vindriktning_config;                                              /** @brief vindriktning config */
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
 * @brief init function for vindriktning
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
     * load config
     */
    vindriktning_config.load();
    /**
     *  initialize your hardware
     */
    pm1006.begin( vindriktning_config.RXpin );
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
 * @brief deinit function for vindriktning
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
    ASSERT( initialized, MODULE_NAME " are not deinitialized, check your code" );
    /**
     *  deinitialized your hardware
     */
    pm1006.end();
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
 * @brief loop function for vindriktning
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
    ASSERT( initialized, MODULE_NAME " are not initialized, check your code" );
    /**
     * run loop every 100ms
     */
    if( NextMillis < millis() ) {
        NextMillis = millis() + 25l;
        pm1006.loop();
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
                vindriktning_config.save();
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
            else if ( !strcmp("get_vindriktning_settings", cmd ) ) {
                asyncwebserver_send_websocket_msg("vindriktning_rxpin\\%d", vindriktning_config.RXpin );
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_mqtt_msg_stat\\%s", vindriktning_config.mqtt_msg_stat ? "true" : "false " );
            }
            else if ( !strcmp( "get_vindriktning_data", cmd ) ) {
                asyncwebserver_send_websocket_msg("vindriktning_pm1\\%d", pm1006.isPm1Valid() ? pm1006.getPm1Value() : 0 );
                asyncwebserver_send_websocket_msg("vindriktning_pm25\\%d", pm1006.isPm25Valid() ? pm1006.getPm25Value() : 0 );
                asyncwebserver_send_websocket_msg("vindriktning_pm10\\%d", pm1006.isPm10Valid() ? pm1006.getPm10Value() : 0 );
                asyncwebserver_send_websocket_msg("vindriktning_pm1006_packet\\%s", pm1006.getDataFrame() );
            }
            /**
             * store and set vindriktning_rxpin
             */
            else if ( !strcmp( "vindriktning_rxpin", cmd ) ) {
                vindriktning_config.RXpin = atoi( value );
                asyncwebserver_send_websocket_msg("vindriktning_rxpin\\%d", vindriktning_config.RXpin );
            }
            /**
             * store and set mqtt_msg_stat
             */ 
            else if ( !strcmp ( MODULE_NAME "_mqtt_msg_stat", cmd ) ) {
                vindriktning_config.mqtt_msg_stat = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_mqtt_msg_stat\\%s", vindriktning_config.mqtt_msg_stat ? "true" : "false " );
            }
            retval = true;
            break;
        case WEB_DATA:
            ASSERT( request, MODULE_NAME " request is NULL" );
            /**
             * html page and send it
             */
            html = html_header;
            html += asyncwebserver_get_menu();
            html += vindriktning_config_page;
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
            vindriktning_config.save();
            retval = true;
            break;
        case RESET_CONFIG:
            /**
             * reset config
             */
            vindriktning_config.resetToDefault();
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
    /**
     * check mqtt events
     */
    switch( event ) {
        case MQTTSTAT_JSON_DATA:
            /**
             * check json data
             */
            ASSERT( mqttData->doc, MODULE_NAME " mqttstat json data is NULL" );
            /**
             * insert pm1006 data
             */
            if( vindriktning_config.mqtt_msg_stat ) {
                doc["sensor"]["pm1006"]["pm1"] = pm1006.isPm1Valid() ? pm1006.getPm1Value() : 0; 
                doc["sensor"]["pm1006"]["pm25"] = pm1006.isPm25Valid() ? pm1006.getPm25Value() : 0; 
                doc["sensor"]["pm1006"]["pm10"] = pm1006.isPm10Valid() ? pm1006.getPm10Value() : 0; 
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