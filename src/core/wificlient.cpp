/**
 * @file wificlient.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_task_wdt.h>

#include "core.h"
#include "webserver.h"
#include "core/utils/alloc.h"

#include "wificlient.h"
#include "config/wifi_config.h"
/**
 * analog module namespace
 */
#define MODULE_NAME                "wificlient"
/**
 * local variables
 */
wificlient_config_t wificlient_config;                              /** @brief wifi config */
TaskHandle_t _wificlient_Task;                                      /** @brief wifi task handle */
static bool APMODE = false;
/**
 * local static functions
 */
static void registration( void );
static bool webserver_cb( EventBits_t event, void *arg );
static bool update_webserver_cb( EventBits_t event, void *arg );
static bool info_webserver_cb( EventBits_t event, void *arg );
static void start_softAP( void );
static void Task( void * pvParameters );
/**
 * @brief setup function for wificlient, called by core autocall function
 */
static int registed = core_autocall_function( &registration, 0 );           /** @brief module autocall function */

static void registration( void ) {
    /**
     * check if already registered
     */
    ASSERT( registed, MODULE_NAME " setup is called without module registration, check your code [%d]", registed );
    /**
     * load wifi config
     */
    wificlient_config.load();
    /**
     * print hostname
     */
    log_i( "hostname: %s", wificlient_get_hostname() );
    /**
     * set wifi mode to STA
     */
    WiFi.mode( WIFI_STA );
    /**
     * set wifi bandwidth and power save mode
     * 
     */
    #if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 0, 0)
        if(  wificlient_config.low_bandwidth )
            esp_wifi_set_bandwidth( ESP_IF_WIFI_STA, WIFI_BW_HT20 );
        else
            esp_wifi_set_bandwidth( ESP_IF_WIFI_STA, WIFI_BW_HT40 );

        if( wificlient_config.low_power )
            esp_wifi_set_ps( WIFI_PS_MODEM );
        else
            esp_wifi_set_ps( WIFI_PS_NONE );
    #endif
    /**
     * set hostname
     */
    WiFi.setHostname( wificlient_config.hostname );
    /**
     * set wifi led pin
     */
    if( wificlient_config.enable_led )
        pinMode( wificlient_config.led_pin, OUTPUT );
    /**
     * register webserver callback functions
     */
    asyncwebserver_register_cb_with_prio( WS_DATA | WEB_DATA | WEB_MENU | SAVE_CONFIG | RESET_CONFIG, webserver_cb, "/wlan.htm", CALL_CB_CORE );
    asyncwebserver_register_cb_with_prio( WS_DATA | WEB_DATA | WEB_MENU, update_webserver_cb, "/update.htm", CALL_CB_LAST );
    asyncwebserver_register_cb_with_prio( WS_DATA | WEB_DATA | WEB_MENU, info_webserver_cb, "/info.htm", CALL_CB_LAST );
    asyncwebserver_set_cb_active( webserver_cb, true );
    asyncwebserver_set_cb_active( update_webserver_cb, true );
    asyncwebserver_set_cb_active( info_webserver_cb, true );
    /**
     * start wifi client task
     */
    xTaskCreatePinnedToCore(    Task,                   /* Function to implement the task */
                                "wificlient Task",      /* Name of the task */
                                5000,                   /* Stack size in words */
                                NULL,                   /* Task input parameter */
                                1,                      /* Priority of the task */
                                &_wificlient_Task,      /* Task handle. */
                                1 );                    /* Core where the task should run */
}

/**
 * @brief wificlient task to manage the wifi connection
 * 
 * @param pvParameters 
 */
static void Task( void * pvParameters ) {
    log_i( "Start Wifi-Client on Core: %d", xPortGetCoreID() );
    /**
     * add task to watchdog
     */
    esp_task_wdt_add( NULL );
    /**
     * loop forever
     */
    while( true ) {
        /*
         * Check if WiFi connected
         */
        if ( WiFi.status() != WL_CONNECTED ) {
            if( wificlient_config.enable_led )
                digitalWrite( wificlient_config.led_pin, LOW );
            int wlan_timeout = wificlient_config.timeout;
            
            if( strlen( wificlient_config.ssid ) > 1 )
                WiFi.begin( wificlient_config.ssid , wificlient_config.password );
            else
                start_softAP();

            while ( WiFi.status() != WL_CONNECTED ){
                delay(1000);
                if ( wlan_timeout <= 0 ) {
                    start_softAP();
                    break;
                }
                wlan_timeout--;
            }
            if ( WiFi.status() == WL_CONNECTED ) {
                log_i("connected, IP address: %s", WiFi.localIP().toString().c_str() );
            }
        }
        else {
            if( wificlient_config.enable_led ) {
                if ( WiFi.status() == WL_CONNECTED )
                    digitalWrite( wificlient_config.led_pin, HIGH );
                else {
                    if( APMODE )
                        digitalWrite( wificlient_config.led_pin, digitalRead( wificlient_config.led_pin ) ? false : true );
                    else
                        digitalWrite( wificlient_config.led_pin, LOW );
                }
            }
        }
        /**
         * wait 1 sec
         */
        esp_task_wdt_reset();
        vTaskDelay( 100 );
    }
}

/**
 * @brief webserver callback for info page /info,htm
 * 
 * @param event     event bitmask
 * @param arg       pointer to additional data
 * @return true 
 * @return false 
 */
static bool info_webserver_cb( EventBits_t event, void *arg ) {
    bool retval = false;
    wsData_t *ws_data = (wsData_t *)arg;
    AsyncWebServerRequest * request = ws_data->request;
    String html = "";
    /**
     * check event
     */
    switch( event ) {
        case WEB_DATA:
            ASSERT( request, MODULE_NAME " request is NULL, check your code!");
            /**
             * html page and send it
             */
            html = html_header;
            html += asyncwebserver_get_menu();
            html += wificlient_info_config_page;
            html += html_footer;
            request->send(200, "text/html", html);
            retval = true;
            break;
        case WEB_MENU:
            asyncwebserver_set_menu_entry( "/info.htm", "info" );
            retval = true;
            break;
    }
    return( retval );
}

/**
 * @brief webserver callback for update page /update.htm
 * 
 * @param event     event bitmask
 * @param arg       pointer to additional data
 * @return true 
 * @return false 
 */
static bool update_webserver_cb( EventBits_t event, void *arg ) {
    bool retval = false;
    wsData_t *ws_data = (wsData_t *)arg;
    AsyncWebServerRequest * request = ws_data->request;
    String html = "";
    /**
     * check event
     */
    switch( event ) {
        case WEB_DATA:
            ASSERT( request, MODULE_NAME " request is NULL, check your code!");
            /**
             * html page and send it
             */
            html = html_header;
            html += asyncwebserver_get_menu();
            html += wificlient_update_page;
            html += html_footer;
            request->send(200, "text/html", html);
            retval = true;
            break;
        case WEB_MENU:
            asyncwebserver_set_menu_entry( "/update.htm", "web update" );
            retval = true;
            break;
    }
    return( retval );
}

/**
 * @brief webserver callback for wificlient config page wlan.htm
 * 
 * @param event     event mask
 * @param arg       pointer to optional data
 * @return true 
 * @return false 
 */
static bool webserver_cb( EventBits_t event, void *arg ) {
    bool retval = true;
    wsData_t *ws_data = (wsData_t *)arg;
    AsyncWebSocketClient * client = ws_data->client;
    AsyncWebServerRequest * request = ws_data->request;
    const char *cmd = ws_data->cmd ? ws_data->cmd : "";
    const char *value = ws_data->value ? ws_data->value : "";
    String html;
    /**
     * check event
     */
    switch( event ) {
        case WS_DATA:
            ASSERT( client, MODULE_NAME " client is NULL, check your code!");
            /**
             * check all commands
             */
            if ( !strcmp( "save_wlan_settings", cmd ) ) {
                core_enter_critical();
                wificlient_config.save();
                core_exit_critical();
                asyncwebserver_send_websocket_msg( "status\\Save" );
            }
            else if ( !strcmp("get_wlan_settings", cmd ) ) {
                asyncwebserver_send_websocket_msg( "unique_hostname\\%s", wificlient_get_hostname_unique() );
                asyncwebserver_send_websocket_msg( "hostname\\%s", wificlient_get_hostname() );
                asyncwebserver_send_websocket_msg( "wifi_ssid\\%s", wificlient_config.ssid );
                asyncwebserver_send_websocket_msg( "wifi_password\\%s", "********" );
                asyncwebserver_send_websocket_msg( "checkbox\\wifi_low_power\\%s", wificlient_config.low_power ? "true" : "false ");
                asyncwebserver_send_websocket_msg( "checkbox\\wifi_low_bandwidth\\%s", wificlient_config.low_bandwidth ? "true" : "false ");
                asyncwebserver_send_websocket_msg( "checkbox\\wifi_enable_softap\\%s", wificlient_config.enable_softap ? "true" : "false " );
                asyncwebserver_send_websocket_msg( "checkbox\\wifi_enable_led\\%s", wificlient_config.enable_led ? "true" : "false " );
                asyncwebserver_send_websocket_msg( "wifi_led_pin\\%d", wificlient_config.led_pin );
                asyncwebserver_send_websocket_msg( "wifi_softap_ssid\\%s", wificlient_config.softap_ssid );
                asyncwebserver_send_websocket_msg( "wifi_softap_password\\%s", "********" );
            }
            else if ( !strcmp("hostname", cmd ) ) {
                strncpy( wificlient_config.hostname, value, sizeof( wificlient_config.hostname ) );
                asyncwebserver_send_websocket_msg( "hostname\\%s", wificlient_config.hostname );
            }
            else if ( !strcmp("wifi_ssid", cmd ) ) {
                strncpy( wificlient_config.ssid, value, sizeof( wificlient_config.ssid ) );
                asyncwebserver_send_websocket_msg( "wifi_ssid\\%s", wificlient_config.ssid );
            }
            else if ( !strcmp("wifi_password", cmd ) ) {
                if ( strcmp( "********", value ) ) {
                    strncpy( wificlient_config.password, value, sizeof( wificlient_config.password ) );
                    asyncwebserver_send_websocket_msg( "wifi_password\\%s", "********" );
                }
            }
            else if ( !strcmp("wifi_softap_ssid", cmd ) ) {
                strncpy( wificlient_config.softap_ssid, value, sizeof( wificlient_config.softap_ssid ) );
                asyncwebserver_send_websocket_msg( "wifi_softap_ssid\\%s", wificlient_config.softap_ssid );
            }
            else if ( !strcmp("wifi_softap_password", cmd ) ) {
                if ( strcmp( "********", value ) ) {
                    strncpy( wificlient_config.softap_password, value, sizeof( wificlient_config.softap_password ) );
                    asyncwebserver_send_websocket_msg( "wifi_softap_password\\%s", "********" );
                }
            }
            else if ( !strcmp("wifi_enable_softap", cmd ) ) {
                wificlient_config.enable_softap = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg( "checkbox\\wifi_enable_softap\\%s", wificlient_config.enable_softap ? "true" : "false " );
            }
            else if ( !strcmp("wifi_low_power", cmd ) ) {
                wificlient_config.low_power = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg( "checkbox\\wifi_low_power\\%s", wificlient_config.low_power ? "true" : "false " );
            }
            else if ( !strcmp("wifi_low_bandwidth", cmd ) ) {
                wificlient_config.low_bandwidth = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg( "checkbox\\wifi_low_bandwidth\\%s", wificlient_config.low_bandwidth ? "true" : "false " );
            }
            else if ( !strcmp("wifi_enable_led", cmd ) ) {
                wificlient_config.enable_led = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg( "checkbox\\wifi_enable_led\\%s", wificlient_config.enable_led ? "true" : "false " );
            }
            else if ( !strcmp("wifi_led_pin", cmd ) ) {
                wificlient_config.led_pin = atoi( value );
                asyncwebserver_send_websocket_msg( "wifi_led_pin\\%d", wificlient_config.led_pin );
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
            html += wificlient_config_page;
            html += html_footer;
            request->send(200, "text/html", html);
            retval = true;
            break;
        case WEB_MENU:
            asyncwebserver_set_menu_entry( "/wlan.htm", "wlan" );
            retval = true;
            break;
        case SAVE_CONFIG:
            core_enter_critical();
            wificlient_config.save();
            core_exit_critical();
            retval = true;
            break;
        case RESET_CONFIG:
            core_enter_critical();
            wificlient_config.resetToDefault();
            core_exit_critical();
            retval = true;
            break;
    }
    return( retval );
}

/**
 * @brief start softAP
 */
static void start_softAP( void ) {
    if( !APMODE && wificlient_config.enable_softap ) {
        WiFi.softAP( wificlient_config.softap_ssid, wificlient_config.softap_password );
        log_i("starting Wifi-AP with SSID \"%s\" / \"%s\"", wificlient_config.softap_ssid, wificlient_config.softap_password );
        log_i("AP IP address: %s", WiFi.softAPIP().toString().c_str() );
        APMODE = true;
    }
}

const char *wificlient_get_hostname( void ) {
    return( (const char*)wificlient_config.hostname );
}

const char *wificlient_get_hostname_unique( void ) {
    return( (const char*)wificlient_config.hostname_unique );
}