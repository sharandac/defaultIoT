/**
 * @file ftpserver.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */
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
#include <ESP8266FtpServer.h>

#include "core.h"
#include "webserver.h"
#include "core/utils/alloc.h"

#include "ftpserver.h"
#include "config/ftpserver_config.h"
/**
 * analog module namespace
 */
#define MODULE_NAME                         "ftp"
/**
 * local variables
 */
static TaskHandle_t _Task;                  /** @brief wifi task handle */
static FtpServer *ftpSrv = NULL;            /** @brief ftp server handle */
static ftpserver_config_t ftpserver_config; /** @brief ftpserver config */
/**
 * local static functions
 */
static void registration( void );
static bool webserver_cb( EventBits_t event, void *arg );
static void Task( void * pvParameters );
/**
 * @brief setup function for ftpserver, called by core autocall function
 */
static int registed = core_autocall_function( &registration, 0 );           /** @brief module autocall function */
/**
 * @brief register ftpserver module
 */
static void registration( void ) {
    /**
     * check if already registered
     */
    ASSERT( registed, MODULE_NAME " setup is called without module registration, check your code [%d]", registed );
    /**
     * register webserver callback functions
     */
    asyncwebserver_register_cb_with_prio( WS_DATA | WEB_DATA | WEB_MENU | SAVE_CONFIG | RESET_CONFIG, webserver_cb, "/" MODULE_NAME ".htm", CALL_CB_CORE );
    asyncwebserver_set_cb_active( webserver_cb, true );
    /**
     * start wifi client task
     */
    xTaskCreatePinnedToCore(    Task,                   /* Function to implement the task */
                                "ftpserver Task",       /* Name of the task */
                                5000,                   /* Stack size in words */
                                NULL,                   /* Task input parameter */
                                1,                      /* Priority of the task */
                                &_Task,                 /* Task handle. */
                                1 );                    /* Core where the task should run */
}

/**
 * @brief wificlient task to manage the ftpserver connection
 * 
 * @param pvParameters 
 */
static void Task( void * pvParameters ) {
    /**
     * wait until wifi is connected
     */
    while( !WiFi.isConnected() ){};
    /**
     * add task to watchdog
     */
    esp_task_wdt_add( NULL );
    /**
     * load config
     */
    ftpserver_config.load();
    /**
     * fet ftp user and password and start the ftpserver
     */
    if ( !ftpSrv && ftpserver_config.enable ) {
        log_i( "Start FTP server on Core: %d", xPortGetCoreID() );
        /**
         * get a new instance
         */
        ftpSrv = new FtpServer;
        /**
         * start ftp server
         */
        if ( ftpSrv )
            ftpSrv->begin( ftpserver_config.username, ftpserver_config.password );
        else
            log_e("start ftp server failed");
    }
    /**
     * loop forever
     */
    while( true ) {
        /**
         * handle ftp server
         */
        if ( ftpSrv )
            ftpSrv->handleFTP();
        /**
         * wait 1 sec
         */
        esp_task_wdt_reset();
        vTaskDelay( 10 );
    }
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
    wsData_t *wsData = (wsData_t *)arg;
    String html;
    /**
     * check event
     */
    switch( event ) {
        case WS_DATA:
            /**
             * check all commands
             */
            if ( !strcmp( "save_" MODULE_NAME "_settings", wsData->cmd ) ) {
                ftpserver_config.save();
                asyncwebserver_send_websocket_msg( "status\\Save" );
            }
            else if ( !strcmp("get_" MODULE_NAME "_settings", wsData->cmd ) ) {
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_enable\\%s", ftpserver_config.enable ? "true" : "false ");
                asyncwebserver_send_websocket_msg( MODULE_NAME "_username\\%s", ftpserver_config.username );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_password\\%s", "********" );
            }
            else if ( !strcmp( MODULE_NAME "_username", wsData->cmd ) ) {
                strncpy( ftpserver_config.username, wsData->value, sizeof( ftpserver_config.username ) );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_username\\%s", ftpserver_config.username );
            }
            else if ( !strcmp( MODULE_NAME "_password", wsData->cmd ) ) {
                if ( strcmp( "********", wsData->value ) ) {
                    strncpy( ftpserver_config.password, wsData->value, sizeof( ftpserver_config.password ) );
                    asyncwebserver_send_websocket_msg( MODULE_NAME "_password\\%s", "********" );
                }
            }
            else if ( !strcmp("ftpserver_enable", wsData->cmd ) ) {
                ftpserver_config.enable = atoi( wsData->value ) ? true : false;
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_enable\\%s", ftpserver_config.enable ? "true" : "false ");
            }
            retval = true;
            break;
        case WEB_DATA:
            ASSERT( wsData->request, MODULE_NAME " request is NULL, check your code!");
            /**
             * html page and send it
             */
            html = html_header;
            html += asyncwebserver_get_menu();
            html += ftpserver_config_page;
            html += html_footer;
            wsData->request->send(200, "text/html", html);
            retval = true;
            break;
        case WEB_MENU:
            asyncwebserver_set_menu_entry( "/" MODULE_NAME ".htm", MODULE_NAME );
            retval = true;
            break;
        case SAVE_CONFIG:
            ftpserver_config.save();
            retval = true;
            break;
        case RESET_CONFIG:
            ftpserver_config.resetToDefault();
            retval = true;
            break;
    }
    return( retval );
}
