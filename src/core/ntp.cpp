/**
 * @file ntp.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <Arduino.h>
#include <WiFi.h>
#include <esp_task_wdt.h>
#include <time.h>

#include "core.h"
#include "webserver.h"
#include "config.h"
#include "core/utils/alloc.h"
#include "core/utils/callback.h"

#include "ntp.h"
#include "config/ntp_config.h"
/**
 * module namespace
 */
#define MODULE_NAME                 "ntp"
/**
 * local variables
 */
static TaskHandle_t _Task;          /** @brief handle for ntp task */
ntp_config_t ntp_config;            /** @brief ntp config */
/**
 * local static funtions
 */
static void registration( void );
static void Task( void * pvParameters );
static bool webserver_cb( EventBits_t event, void *arg );
/**
 * @brief setup function for ntp, called by core autocall function
 */
static int registed = core_autocall_function( &registration, 0 );           /** @brief module autocall function */

static void registration( void ) {  
    /**
     * check if already registered
     */
    ASSERT( registed, MODULE_NAME " setup is called without module registration, check your code [%d]", registed );
    /**
     * load ntp config
     */
    ntp_config.load();
    /**
     * register webserver callback function
     */
    asyncwebserver_register_cb_with_prio( WS_DATA | WEB_DATA | WEB_MENU | SAVE_CONFIG | RESET_CONFIG, webserver_cb, "/" MODULE_NAME ".htm", CALL_CB_CORE ); 
    asyncwebserver_set_cb_active( webserver_cb, true );
    /**
     * start ntp task
     */
    xTaskCreatePinnedToCore(    Task,               /* Function to implement the task */
                                MODULE_NAME " Task",/* Name of the task */
                                2500,               /* Stack size in words */
                                NULL,               /* Task input parameter */
                                1,                  /* Priority of the task */
                                &_Task,         /* Task handle. */
                                1 );                /* Core where the task should run */   
}

/**
 * @brief ntp task
 * 
 * @param pvParameters 
 */
static void Task( void * pvParameters ) {
    struct tm info;                         /** @brief current time */
    char time_str[32] = "";                 /** @brief current time string */
    static uint64_t NextMillis = millis();  /** @brief next update time */
    
    log_i("Start NTP Task on Core: %d", xPortGetCoreID() );
    /**
     * wait for wifi
     */
    while( !WiFi.isConnected() ){};
    /**
     * add watchdog
     */
    esp_task_wdt_add( NULL );
    /**
     * set timezone
     */
    while ( true ) {
        /**
         * update needed?
         */
        if ( NextMillis < millis() ) {
            /**
             * set next update
             */
            NextMillis += 15 * 1000l;
            /**
             * check if wifi is connected
             */
            if ( WiFi.isConnected() && ntp_config.enable ) {
                /**
                 * set timezone and server
                 */
                configTzTime( ntp_config.timezone, ntp_config.server );
                /**
                 * get time
                 */
                if( getLocalTime( &info ) ) {
                    NextMillis = millis() + NTP_RENEW_INTERVAL * 1000l;
                    strftime( time_str, sizeof( time_str ), "%Y-%m-%d %H:%M.%S", &info );
                    log_d("NTP-client: time is %s", time_str );
                }
                else
                    log_e( "NTP-client: Failed to obtain time" );
            }
        }
        /**
         * wait and reset watchdog
         */
        esp_task_wdt_reset();
        vTaskDelay( 5000 );
    }
}

/**
 * @brief webserver callback for index.htm
 * 
 * @param event     event bitmask
 * @param arg       pointer to additional data
 * @return true 
 * @return false 
 */
static bool webserver_cb( EventBits_t event, void *arg ) {
    bool retval = false;
    wsData_t *ws_data = (wsData_t *)arg;
    AsyncWebSocketClient * client = ws_data->client;
    AsyncWebServerRequest * request = ws_data->request;
    const char *cmd = ws_data->cmd ? ws_data->cmd : "";
    const char *value = ws_data->value ? ws_data->value : "";
    String html = "";
    /**
     * check event
     */
    switch( event ) {
        case WS_DATA:
            ASSERT( client, MODULE_NAME "client is NULL, check your code!");
            /**
             * check all commands
             */
            if ( !strcmp( "save_" MODULE_NAME "_settings", cmd ) ) {
                ntp_config.save();
                asyncwebserver_send_websocket_msg( "status\\Save" );
            }
            else if ( !strcmp("get_" MODULE_NAME "_settings", cmd ) ) {
                asyncwebserver_send_websocket_msg( MODULE_NAME "_server\\%s", ntp_config.server );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_timezone\\%s", ntp_config.timezone );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_interval\\%d", ntp_config.interval );
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_enable\\%s", ntp_config.enable ? "true" : "false ");
            }
            else if ( !strcmp( MODULE_NAME "_server", cmd ) ) {
                strncpy( ntp_config.server, value, sizeof( ntp_config.server ) );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_server\\%s", ntp_config.server );
            }
            else if ( !strcmp( MODULE_NAME "_timezone", cmd ) ) {
                strncpy( ntp_config.timezone, value, sizeof( ntp_config.timezone ) );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_timezone\\%s", ntp_config.timezone );
            }
            else if ( !strcmp( MODULE_NAME "_interval", cmd ) ) {
                ntp_config.interval = atoi( value );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_interval\\%d", ntp_config.interval );
            }
            else if ( !strcmp( MODULE_NAME "_enable", cmd ) ) {
                ntp_config.enable = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_enable\\%s", ntp_config.enable ? "true" : "false " );
            }
            retval = true;
            break;
        case WEB_DATA:
            ASSERT( request, MODULE_NAME " request is NULL, check your code!");
            /**
             * html page and send it
             */
            html = html_header;
            html += asyncwebserver_get_menu();
            html += ntp_config_page;
            html += html_footer;
            request->send(200, "text/html", html);
            retval = true;
            break;
        case WEB_MENU:
            asyncwebserver_set_menu_entry( "/" MODULE_NAME ".htm", MODULE_NAME );
            retval = true;
            break;
        case SAVE_CONFIG:
            ntp_config.save();
            retval = true;
            break;
        case RESET_CONFIG:
            ntp_config.resetToDefault();
            retval = true;
            break;
    }
    return( retval );
}