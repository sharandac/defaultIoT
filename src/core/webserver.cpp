/**
 * @file webserver.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <WiFi.h>
#include <WiFiClient.h>
#include <Update.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <esp_task_wdt.h>
#include <ESPmDNS.h>
#include "core.h"
#include "config.h"
#include "core/wificlient.h"
#include "core/utils/alloc.h"

#include "webserver.h"
#include "webserver_files.h"
/**
 * analog module namespace
 */
#define MODULE_NAME                "webserver"
/**
 * local variables
 */
AsyncWebServer asyncserver( WEBSERVERPORT );    /** @brief webserver on port 80 */
AsyncWebSocket ws("/ws");                       /** @brief websocket on /ws */
TaskHandle_t _WEBSERVER_Task;                   /** @brief handle for webserver task */
callback_t *webwebser_callback = NULL;          /** @brief callback function for webserver */
String cssmenu = "";                            /** @brief css menu */

static void registration( void );
/**
 * @brief setup function for webserver, called by core autocall function
 */
static int registed = core_autocall_function( &registration, 1 );           /** @brief module autocall function */
/*
 * local static functions
 */
static void Task( void * pvParameters );
static void initialize( void );

static void registration ( void ) {
    /**
     * check if already registered
     */
    ASSERT( registed, MODULE_NAME " setup is called without module registration, check your code [%d]", registed );
    /**
     * start webserver task
     */
    xTaskCreatePinnedToCore(    Task,
                                "webserver Task",
                                20000,
                                NULL,
                                1,
                                &_WEBSERVER_Task,
                                1 );
}
/**
 * @brief webserver task
 * 
 * @param pvParameters 
 */
void Task( void * pvParameters ) {
    log_i( "Start Webserver on Core: %d", xPortGetCoreID() );
    /**
     * setup webserver
     */
    initialize();
    /**
     * add watchdog
     */
    esp_task_wdt_add( NULL );
    /**
     * loop forever
     */
    while( true ) {
        /**
         * work on websockets
         */
        ws.cleanupClients();
        /**
         * reset watchdog
         */
        esp_task_wdt_reset();
        vTaskDelay( 1 );
    }
}
/**
 * @brief websocket event handler
 * 
 * @param server    pointer to the server
 * @param client    pointer to the client
 * @param type      type of the event
 * @param arg       pointer to the argument
 * @param data      pointer to the data
 * @param len       length of the data
 */
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
    /*
    * queue the event-type
    */
    switch (type) {
        case WS_EVT_CONNECT: { break; }
        case WS_EVT_ERROR: { break; }
        case WS_EVT_PONG: { break; }
        case WS_EVT_DISCONNECT: { break; }
        case WS_EVT_DATA: {
            /*
             * copy data into an separate allocated buffer and terminate it with \0
             */
            char * cmd = (char*)CALLOC_ASSERT( len + 1, sizeof(uint8_t), "calloc failed ..." );
            memcpy( cmd, data, len );
            /*
             * separate command and his correspondening value
             */
            char * value = cmd;
            while( *value ) {
                if ( *value == '\\' ) {
                    *value = '\0';
                    value++;
                    break;
                }
                value++;
            }
            /*
             * queue commands
             * save settings and status group
             */
            if ( !strcmp("STS", cmd ) ) {
                asyncwebserver_send_websocket_msg( "status\\online" );
            }
            else if ( !strcmp( "SAV", cmd ) ) {
                asyncwebserver_send_websocket_msg( "status\\Save" );
            }
            /**
             * trigger ws data callback
             */
            wsData_t wsData;
            wsData.client = client;
            wsData.cmd = cmd;
            wsData.value = value;
            core_enter_critical();
            callback_send( webwebser_callback, WS_DATA, (void*)&wsData );
            core_exit_critical();
            free( cmd );
        }
        default: break;
    }
}

/*
 * based on: https://github.com/lbernstone/asyncUpdate/blob/master/AsyncUpdate.ino
 */
void handleUpdate( AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
    if ( !index ) {
        /*
         * if filename includes spiffs, update the spiffs partition
         */
        int cmd = ( filename.indexOf("spiffs") > 0 ) ? U_SPIFFS : U_FLASH;
        if (!Update.begin( UPDATE_SIZE_UNKNOWN, cmd ) )
            Update.printError(Serial);
    }
    /*
     * Write Data an type message if fail
     */
    if ( Update.write( data, len ) != len )
        Update.printError( Serial );
    /*
     * After write Update restart
     */
    if (final) {
        AsyncWebServerResponse *response = request->beginResponse( 302, "text/plain", "Please wait while the device reboots\r\n" );
        response->addHeader( "Refresh", "20" );  
        response->addHeader( "Location", "/" );
        request->send(response);

        if( !Update.end( true ) )
            Update.printError(Serial);
        else {
            log_i( "Update complete" );
            
            wsData_t wsData;
            wsData.request = request;
            callback_send( webwebser_callback, SAVE_CONFIG, (void*)&wsData );

            ESP.restart();
        }
    }
}

static void initialize( void ) {
    asyncserver.on( "/info", HTTP_GET, [](AsyncWebServerRequest * request ) { request->send( 200, "text/plain", "firmware: " __DATE__ " " __TIME__ "\r\nGCC-Version: " __VERSION__ "\r\n" ); } );
    asyncserver.addHandler( new SPIFFSEditor( SPIFFS ) );
    asyncserver.onFileUpload([](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
        if( !index )
            log_i( "UploadStart: %s\n", filename.c_str());
        log_i("%s", (const char*)data);
        if( final )
            log_i( "UploadEnd: %s (%u)\n", filename.c_str(), index+len);
    });
    asyncserver.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
        if( !index )
            log_i( "BodyStart: %u\n", total );
        log_i( "%s", (const char*)data);
        if( index + len == total )
            log_i( "BodyEnd: %u\n", total );
    });
    asyncserver.on("/default", HTTP_GET, []( AsyncWebServerRequest * request ) {
        wsData_t wsData;
        wsData.request = request;
        request->send( 200, "text/plain", "Reset the device to default settings\r\n" );
        callback_send( webwebser_callback, RESET_CONFIG, (void*)&wsData );
        delay( 500 );
        ESP.restart();    
    });
    asyncserver.on("/format", HTTP_GET, []( AsyncWebServerRequest * request ) {        
        request->send( 200, "text/plain", "format SPIFFS\r\n" );
        SPIFFS.end();
        SPIFFS.format();
        SPIFFS.begin();
        delay( 500 );
        ESP.restart();    
    });
    asyncserver.on("/clear", HTTP_GET, []( AsyncWebServerRequest * request ) {    
        wsData_t wsData;
        wsData.request = request;    
        request->send( 200, "text/plain", "format SPIFFS and write config\r\n" );
        SPIFFS.end();
        SPIFFS.format();
        SPIFFS.begin();
        callback_send( webwebser_callback, SAVE_CONFIG, (void*)&wsData );
        delay( 500 );
        ESP.restart();    
    });
    asyncserver.on("/memory", HTTP_GET, [](AsyncWebServerRequest *request) {
        String html = (String) "<html><head><meta charset=\"utf-8\"></head><body><h3>Memory Details</h3>" +
                    "<b>Heap size: </b>" + ESP.getHeapSize() + "<br>" +
                    "<b>Heap free: </b>" + ESP.getFreeHeap() + "<br>" +
                    "<b>Heap free min: </b>" + ESP.getMinFreeHeap() + "<br>" +
                    "<b>Psram size: </b>" + ESP.getPsramSize() + "<br>" +
                    "<b>Psram free: </b>" + ESP.getFreePsram() + "<br>" +
                    "<br><b><u>System</u></b><br>" +
                    "<b>Uptime: </b>" + millis() / 1000 + "<br>" +
                    "</body></html>";
        request->send(200, "text/html", html);
    });
    asyncserver.on("/reset", HTTP_GET, []( AsyncWebServerRequest * request ) {
        AsyncWebServerResponse *response = request->beginResponse( 302, "text/plain", "Please wait while the device reboots" );
        response->addHeader( "Refresh", "20" );  
        response->addHeader( "Location", "/" );
        request->send(response);
        delay( 500 );
        ESP.restart();    
    });
    asyncserver.on("/update", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send(200, "text/html", html_update );
    });
    asyncserver.on("/update", HTTP_POST,
        [](AsyncWebServerRequest *request) {},
        [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) { handleUpdate(request, filename, index, data, len, final); }
    );
    asyncserver.onNotFound([](AsyncWebServerRequest *request){
        wsData_t wsData;
        wsData.request = request;
        wsData.cmd = "";
        wsData.value = "";
        /**
         * build menu structure
         */
        cssmenu = "<div id='cssmenu'>\r\n  <ul>\r\n";
        callback_send( webwebser_callback, WEB_MENU, (void*)&wsData );
        cssmenu += "  </ul>\r\n</div>\r\n";
        /**
         * call all modules to handle the request and handle "/" as "/index.htm"
         */
        if( !callback_send_to_id( webwebser_callback, WEB_DATA, (void*)&wsData, request->url().c_str() ) ) {
            AsyncWebServerResponse *response = request->beginResponse( 302, "text/plain", "redirect to index.htm" );
            response->addHeader( "Refresh", "1" );  
            response->addHeader( "Location", "/index.htm" );
            request->send(response);
        }
    });

    asyncserver.on("/menu.js", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send_P(200, "application/javascript", menu_js, sizeof( menu_js ), nullptr );
    });
    asyncserver.on("/websocket.js", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send_P(200, "application/javascript", websocket_js, sizeof( websocket_js ), nullptr );
    });
    asyncserver.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send_P(200, "text/css", styles_css, sizeof( styles_css ), nullptr );
    });
    asyncserver.on("/jquery-latest.min.js", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send_P( 200, "application/javascript", jquery_latest_min_js,  sizeof( jquery_latest_min_js ), nullptr );
    });

    asyncserver.serveStatic( "/", SPIFFS, "/").setDefaultFile("index.htm");
    ws.onEvent( onWsEvent );
    asyncserver.addHandler( &ws );
    asyncserver.begin();

    ASSERT( MDNS.begin( wificlient_get_hostname() ), MODULE_NAME " start mDNS service failed" );
    MDNS.addService( "http", "tcp", 80 );
    MDNS.addService( "ftp", "tcp", 21 );
}

void asyncwebserver_set_menu_entry( const char *filename, const char *entry ) {
    cssmenu += (String) "    <li><a href=\"" + filename + "\">" + entry + "</a></li>\r\n";
}

String asyncwebserver_get_menu( void ) {
    return( cssmenu );
}

void asyncwebserver_send_websocket_msg( const char * fmt, ... ) {
    va_list args;
    va_start(args, fmt);

    char *msg = NULL;
    int size = vasprintf( &msg, fmt, args );
    va_end(args);

    if( size )
        ws.textAll( msg );

    if( msg )
        free( msg );
}

bool asyncwebserver_set_cb_active( CALLBACK_FUNC callback_func, bool active ) {
    return( callback_set_active( webwebser_callback, callback_func, active ) );
}

bool asyncwebserver_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    if ( webwebser_callback == NULL ) {
        webwebser_callback = callback_init( "webserver" );
        ASSERT( webwebser_callback,"webserver callback alloc failed" );
    }
    callback_register( webwebser_callback, event, callback_func, id );
    return( asyncwebserver_set_cb_active( callback_func, false ) );
}

bool asyncwebserver_register_cb_with_prio( EventBits_t event, CALLBACK_FUNC callback_func, const char *id, callback_prio_t prio ) {
    if ( webwebser_callback == NULL ) {
        webwebser_callback = callback_init( "webserver" );
        ASSERT( webwebser_callback,"webserver callback alloc failed" );
    }    
    callback_register_with_prio( webwebser_callback, event, callback_func, id, prio );
    return( asyncwebserver_set_cb_active( callback_func, false ) );
}
