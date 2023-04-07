/**
 * @file template.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <Arduino.h>
#include <NeoPixelBus.h>
#include "core/webserver.h"
#include "core/mqttclient.h"
#include "core/modul_mgmt.h"
#include "core/utils/alloc.h"

#include "irsta.h"
#include "config/irsta_config.h"
/**
 * module namespace
 */
#define MODULE_NAME                "irsta"
/**
 * local variables
 */
static bool initialized = false;
static bool irsta_state = false;
static int button_state = 0;
QueueHandle_t touch_queue;
irsta_config_t irsta_config;
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip = NULL;
/**
 * local callback functions with local scope
 */
static void registration( void );
static bool initialize( EventBits_t event, void *arg );
static bool deinitialize( EventBits_t event, void *arg );
static bool loop( EventBits_t event, void *arg );
static bool mqttclient_cb( EventBits_t event, void *arg );
static bool webserver_cb( EventBits_t event, void *arg );
static void set_led( bool enable );
static void set_level( int level );
static void touch_inject( int key );
static int touch_get( void );
/**
 * @brief setup function for vindriktning, called by autocall function
 */
static int registed = module_autocall_function( &registration );           /** @brief module auto registration function */
/**
 * @brief initial setup function
 */
static void registration( void ) {
    /**
     * check if already registered
     */
    ASSERT( registed, MODULE_NAME " setup is called without module registration, check your code [%d]", registed );
    /**
     * register webserver and mqtt client callback function
     */
    mqtt_client_register_cb( MQTTSTAT_JSON_DATA | MQTTCMND_JSON_DATA | MQTTTELE_JSON_DATA | MQTTCMND_RAW_DATA, mqttclient_cb, MODULE_NAME );
    asyncwebserver_register_cb( WS_DATA | WEB_DATA | WEB_MENU | SAVE_CONFIG | RESET_CONFIG, webserver_cb, "/" MODULE_NAME ".htm" );
    /**
     * register initialize, deinitialize and loop function
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
     * load config
     */
    irsta_config.load();
    /**
     *  initialize your hardware
     */
    /**
     * create button queue
     */
    if( !touch_queue )
        touch_queue = xQueueCreate( 10, sizeof( int ) );
    /**
     * setup pins
     */
    if( !strip ) {
        strip = new NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>( 512, 16 );
        strip->Begin();
        for( int i = 0 ; i < 512 ; i++ ) {
            strip->SetPixelColor(i, RgbColor( 0, 0, 0 ) );
        }
        strip->Show();
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
    ASSERT( initialized, MODULE_NAME " are not initialized, check your code" );
    /**
     * save config
     */
    irsta_config.save();
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
    static uint64_t MextMillis = millis();
    /**
     * check if module is initialized
     */
    ASSERT( initialized, MODULE_NAME " are not initialized, check your code" );
    /**
     * to crazy stuff
     */
    if( MextMillis < millis() ) {
        MextMillis += 100l;

        int button_state_tmp = 0;
        int button = BUTTON_NONE;
        int touch[ 4 ] = { 0, 0, 0, 0 };
        /**
         * read button state into one byte
         */
        for( int i = 0 ; i < 8 ; i++ ) {
            if( irsta_config.button_0 ) touch[ 0 ] += touchRead( irsta_config.button_0 ); else touch[ 0 ] = 1000;
            if( irsta_config.button_1 ) touch[ 1 ] += touchRead( irsta_config.button_1 ); else touch[ 1 ] = 1000;
            if( irsta_config.button_2 ) touch[ 2 ] += touchRead( irsta_config.button_2 ); else touch[ 2 ] = 1000; 
            if( irsta_config.button_3 ) touch[ 3 ] += touchRead( irsta_config.button_3 ); else touch[ 3 ] = 1000;
        }
        button_state_tmp = touch[ 0 ] < irsta_config.threshold ? 1 : 0;
        button_state_tmp |= touch[ 1 ] < irsta_config.threshold ? 2 : 0;
        button_state_tmp |= touch[ 2 ] < irsta_config.threshold ? 4 : 0;
        button_state_tmp |= touch[ 3 ] < irsta_config.threshold ? 8 : 0;
        /**
         * check if button state changed
         */
        if( button_state_tmp != button_state ) {
            button_state = button_state_tmp;
            /**
             * check button state
             */
            switch( button_state ) {
                case 0b0001:
                    button = BUTTON_0;
                    log_i("button 0");
                    break;
                case 0b0010:
                    button = BUTTON_I;
                    log_i("button I");
                    break;
                case 0b0100:
                    button = BUTTON_II;
                    log_i("button II");
                    break;
                case 0b1000:
                    button = BUTTON_III;
                    log_i("button III");
                    break;
            }
        }
        asyncwebserver_send_websocket_msg( "button_state\\%d", button_state );            
        /**
         * send button event to queue
         */
        if( button != BUTTON_NONE ) {
            xQueueSend( touch_queue, &button, 0 );
        }
        /**
         * check button
         */
        switch( touch_get() ) {
            case BUTTON_0:
                if( irsta_state ) {
                    irsta_state = false;
                    set_led( irsta_state );
                    mqtt_client_stat_publish( "OFF" );
                }
                else {
                    irsta_state = true;
                    set_led( irsta_state );
                    mqtt_client_stat_publish( "ON" );
                }
                asyncwebserver_send_websocket_msg( "irsta_max\\%d", irsta_config.max );
                log_i("irsta in/off");
                break;
            case BUTTON_II:
                if( irsta_state ) {
                    if( irsta_config.max > 16 )
                        irsta_config.max -= 16;
                    if( irsta_state )
                        set_level( irsta_config.max );
                        
                    irsta_config.save();
                    asyncwebserver_send_websocket_msg( "irsta_max\\%d", irsta_config.max );
                    log_i("irsta max--");
                }
                break;
            case BUTTON_III:
                if( irsta_state ) {
                    if( irsta_config.max < irsta_config.max_led )
                        irsta_config.max += 16;
                    if( irsta_state )  
                        set_level( irsta_config.max );

                    irsta_config.save();
                    asyncwebserver_send_websocket_msg( "irsta_max\\%d", irsta_config.max );
                    log_i("irsta max++");
                }
                break;
            default:
                break;
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
    AsyncWebSocketClient * client = wsData->client;
    AsyncWebServerRequest * request = wsData->request;
    const char *cmd = wsData->cmd ? wsData->cmd : "";
    const char *value = wsData->value ? wsData->value : "";
    String html = "";
    /**
     * check if already init
     */
    ASSERT( initialized, MODULE_NAME " are not in initialized, check your code" );
    /**
     * check event
     */
    switch( event ) {
        case WS_DATA:
            ASSERT( client, MODULE_NAME " client is NULL, check your code" );
            /**
             * check all commands
             */
            if ( !strcmp( "save_" MODULE_NAME "_settings", cmd ) ) {
                irsta_config.save();
                asyncwebserver_send_websocket_msg( "status\\Save" );
            }
            else if ( !strcmp("get_" MODULE_NAME "_settings", cmd ) ) {
                asyncwebserver_send_websocket_msg( MODULE_NAME "_clk_pin\\%d", irsta_config.clk_pin );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_max\\%d", irsta_config.max );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_red\\%d", irsta_config.red );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_green\\%d", irsta_config.green );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_blue\\%d", irsta_config.blue );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_off_pin\\%d", irsta_config.button_0 );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_i_pin\\%d", irsta_config.button_1 );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_ii_pin\\%d", irsta_config.button_2 );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_iii_pin\\%d", irsta_config.button_3 );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_threshold\\%d", irsta_config.threshold );
                asyncwebserver_send_websocket_msg( "checkbox\\" MODULE_NAME "_mqtt_msg_stat\\%s", irsta_config.mqtt_msg_stat ? "true" : "false " );
            }
            else if ( !strcmp( MODULE_NAME "_clk_pin", cmd ) ) {
                irsta_config.clk_pin = atoi( value );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_clk_pin\\%d", irsta_config.clk_pin );
            }
            else if( !strcmp( MODULE_NAME "_max", cmd ) ) {
                irsta_config.max = atoi( value );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_max\\%d", irsta_config.max );
            }
            else if( !strcmp( MODULE_NAME "_red", cmd ) ) {
                irsta_config.red = atoi( value );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_red\\%d", irsta_config.red );
            }
            else if( !strcmp( MODULE_NAME "_green", cmd ) ) {
                irsta_config.green = atoi( value );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_green\\%d", irsta_config.green );
            }
            else if( !strcmp( MODULE_NAME "_blue", cmd ) ) {
                irsta_config.blue = atoi( value );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_blue\\%d", irsta_config.blue );
            }
            else if ( !strcmp( MODULE_NAME "_off_pin", cmd ) ) {
                irsta_config.button_0 = atoi( value );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_off_pin\\%d", irsta_config.button_0 );
            }
            else if ( !strcmp( MODULE_NAME "_i_pin", cmd ) ) {
                irsta_config.button_1 = atoi( value );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_i_pin\\%d", irsta_config.button_1 );
            }
            else if ( !strcmp( MODULE_NAME "_ii_pin", cmd ) ) {
                irsta_config.button_2 = atoi( value );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_ii_pin\\%d", irsta_config.button_2 );
            }
            else if ( !strcmp( MODULE_NAME "_iii_pin", cmd ) ) {
                irsta_config.button_3 = atoi( value );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_iii_pin\\%d", irsta_config.button_3 );
            }
            else if( !strcmp( MODULE_NAME "_threshold", cmd ) ) {
                irsta_config.threshold = atoi( value );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_threshold\\%d", irsta_config.threshold );
            }
            else if ( !strcmp ( MODULE_NAME "_mqtt_msg_stat", cmd ) ) {
                irsta_config.mqtt_msg_stat = atoi( value ) ? true : false;
                asyncwebserver_send_websocket_msg("checkbox\\" MODULE_NAME "_mqtt_msg_stat\\%s", irsta_config.mqtt_msg_stat ? "true" : "false " );
            }
            else if( !strcmp( MODULE_NAME "_inject_button", cmd ) ) {
                touch_inject( atoi( value ) );
            }
            else if( !strcmp( MODULE_NAME "_set_level", cmd ) ) {
                set_level( atoi( value ) );
            }
            retval = true;
            break;
        case WEB_DATA:
            ASSERT( request, MODULE_NAME " request is NULL, check your code" );
            /**
             * html page and send it
             */
            html = html_header;
            html += asyncwebserver_get_menu();
            html += irsta_config_page;
            html += html_footer;
            request->send(200, "text/html", html);
            retval = true;
            break;
        case WEB_MENU:
            asyncwebserver_set_menu_entry( "/" MODULE_NAME ".htm", MODULE_NAME );
            retval = true;
            break;
        case SAVE_CONFIG:
            irsta_config.save();
            log_i("save " MODULE_NAME " config");
            retval = true;
            break;
        case RESET_CONFIG:
            irsta_config.resetToDefault();
            log_i("delete " MODULE_NAME " config");
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
            ASSERT( mqttData->doc, MODULE_NAME " mqttstat json data is NULL");
            /**
             * insert input state data
             */
            if( irsta_config.mqtt_msg_stat ) {
                doc["irsta"]["state"] = irsta_state;
                doc["irsta"]["max"] = irsta_config.max;
                doc["irsta"]["max_led"] = irsta_config.max_led;
                doc["irsta"]["red"] = irsta_config.red;
                doc["irsta"]["green"] = irsta_config.green;
                doc["irsta"]["blue"] = irsta_config.blue;
                doc["irsta"]["button"] = button_state;
            }
            retval = true;
            break;
        case MQTTCMND_JSON_DATA:
            /**
             * check json data
             */
            ASSERT( mqttData->doc, "mqttstat json data is NULL");
            /**
             * insert output state data
             */
            if ( doc.containsKey("set_led") ) {
                set_led( doc["set_led"] );
            }
            if ( doc.containsKey("set_level") ) {
                set_level( doc["set_level"] );
            }
            if( doc.containsKey("store") ) {
                irsta_config.save();
            }
            retval = true;
            break;
        case MQTTCMND_RAW_DATA:
            /**
             * check raw data
             */
            ASSERT( mqttData->msg, MODULE_NAME " mqttcmnd raw data is NULL");
            /**
             * check for command
             */
            if( !strcmp( mqttData->msg, "ON" ) ) {
                set_led( true );
            }
            else if( !strcmp( mqttData->msg, "OFF" ) ) {
                set_led( false );
            }            
            break;
    }

    return( retval );
}

/**
 * @brief Set the led object
 * 
 * @param enable 
 */
static void set_led( bool enable ) {
    if( !initialized )
        return;

    if( enable )
        set_level( irsta_config.max );
    else
        set_level( 0 );

    irsta_state = enable;
}

/**
 * @brief Set the level object
 * 
 * @param level 
 */
static void set_level( int level ) {
    if( !initialized )
        return;

    if( level > irsta_config.max_led )
        level = irsta_config.max_led;

    if( level < 0 )
        level = 0;

    uint32_t level_red = ( level * irsta_config.max * irsta_config.red ) / 65536;
    uint32_t level_green = ( level * irsta_config.max * irsta_config.green ) / 65536;
    uint32_t level_blue = ( level * irsta_config.max * irsta_config.blue ) / 65536;

    for( int i = 0 ; i < 512 ; i++ ) {
        strip->SetPixelColor(i, RgbColor( level_red, level_green, level_blue ) );
    }
    strip->Show();

    return;
}

static void touch_inject( int key ) {
    if( initialized )
        xQueueSend( touch_queue, &key, 0 );  
}

static int touch_get( void ) {
    int retval = BUTTON_NONE;
    
    if( initialized )
        xQueueReceive( touch_queue, &retval, 0 );        

    return( retval );
}