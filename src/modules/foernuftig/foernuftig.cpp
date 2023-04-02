/**
 * @file foernuftig.cpp
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

#include "foernuftig.h"
#include "config/foernuftig_config.h"
/**
 * module namespace
 */
#define MODULE_NAME                "foernuftig"
/**
 * local variables
 */
static bool initialized = false;
foernuftig_config_t foernuftig_config;              /** @brief foernuftig config struct */
volatile int rpm_counter = 0;                       /** @brief rpm counter */
int current_rpm = 0;                                /** @brief current rpm */ 
uint64_t rpm_counter_time_stamp = 0;                /** @brief time stamp for rpm counter */
/**
 * local callback functions with local scope
 */
static void registration( void );
static bool initialize( EventBits_t event, void *arg );
static bool deinitialize( EventBits_t event, void *arg );
static bool loop( EventBits_t event, void *arg );
static bool mqttclient_cb( EventBits_t event, void *arg );
static bool webserver_cb( EventBits_t event, void *arg );
static void set_usb_power( bool enable );
static void set_rpm( int rpm );
static void set_level( int level );
/**
 * @brief setup function for vindriktning, called by autocall function
 */
static int registed = module_autocall_function( &registration );           /** @brief module autocall function */


/**
 * @brief foernuftig rpm pulse counter
 */
void IRAM_ATTR foernuftig_IRQ( void ) {
    rpm_counter++;
}

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
 * @brief init function for foernuftig
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
    foernuftig_config.load();
    /**
     *  initialize your hardware
     */
    pinMode( foernuftig_config.button_0_pin, INPUT );
    pinMode( foernuftig_config.button_1_pin, INPUT );
    pinMode( foernuftig_config.button_2_pin, INPUT );
    pinMode( foernuftig_config.button_3_pin, INPUT );

    pinMode( foernuftig_config.supply_pin, OUTPUT );
    digitalWrite( foernuftig_config.supply_pin, LOW );
    pinMode( foernuftig_config.usb_power_pin, OUTPUT );
    digitalWrite( foernuftig_config.usb_power_pin, HIGH );
    pinMode( foernuftig_config.led_pin, OUTPUT );
    digitalWrite( foernuftig_config.led_pin, LOW );

    ledcSetup( 0, 100, 8 );
    ledcAttachPin( foernuftig_config.clk_pin, 0 );
    ledcWrite( 0, 128 );
    pinMode( foernuftig_config.clk_pin, OUTPUT );

    pinMode( foernuftig_config.fg_pin, INPUT_PULLDOWN );
    attachInterrupt( foernuftig_config.fg_pin, &foernuftig_IRQ, GPIO_INTR_POSEDGE );
    rpm_counter_time_stamp = millis();
    /**
     * restore last state
     */
    if( foernuftig_config.usb_power_state )
        set_usb_power( true );
    else
        set_usb_power( false );
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
 * @brief deinit function for foernuftig
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
     * save current config
     */
    foernuftig_config.save();
    /**
     *  deinitialized your hardware
     */
    pinMode( foernuftig_config.supply_pin, INPUT );
    pinMode( foernuftig_config.usb_power_pin, INPUT );
    pinMode( foernuftig_config.led_pin, INPUT );
    ledcDetachPin( foernuftig_config.clk_pin );
    pinMode( foernuftig_config.clk_pin, INPUT );
    pinMode( foernuftig_config.fg_pin, INPUT );
    detachInterrupt( foernuftig_config.fg_pin );
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
 * @brief loop function for foernuftig
 * 
 * @param event         event from loop
 * @param arg           pointer to data
 * @return true         loop was successfull
 * @return false        loop was not successfull
 */
static bool loop( EventBits_t event, void *arg ) {
    static uint64_t NextMillis = millis();
    static uint64_t NextRpmMillis = millis();
    static int button_state = 0;
    /**
     * check if module is initialized
     */
    ASSERT( initialized, MODULE_NAME " are not initialized, check your code" );
    /**
     * run loop every 100ms
     */
    if( NextMillis < millis() ) {
        NextMillis = millis() + 50l;
        int button_state_tmp = 0;
        int button = BUTTON_NONE;
        /**
         * read button state into one byte
         */
        button_state_tmp  = digitalRead( foernuftig_config.button_0_pin ) ? 0 : 1;
        button_state_tmp |= digitalRead( foernuftig_config.button_1_pin ) ? 0 : 2;
        button_state_tmp |= digitalRead( foernuftig_config.button_2_pin ) ? 0 : 4;
        button_state_tmp |= digitalRead( foernuftig_config.button_3_pin ) ? 0 : 8;
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
                    button = BUTTON_OFF;
                    log_i("button off");
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
        /**
         * check if button is pressed and set rpm
         */
        switch( button ) {
            case BUTTON_OFF:
                set_rpm( 0 );
                log_i("foernuftig off");
                break;
            case BUTTON_I:
                set_rpm( foernuftig_config.min_rpm );
                log_i("foernuftig min");
                break;
            case BUTTON_II:
                set_rpm( foernuftig_config.mid_rpm );
                log_i("foernuftig mid");
                break;
            case BUTTON_III:
                set_rpm( foernuftig_config.max_rpm );
                log_i("foernuftig max");
                break;
            default:
                break;
        }
    }
    /**
     * run loop every 1s
     */
    if( NextRpmMillis < millis() ) {
        NextRpmMillis += 1000l;

        if( rpm_counter_time_stamp != 0 ) {
            current_rpm = ( rpm_counter * 1000 / ( millis() - rpm_counter_time_stamp ) ) * foernuftig_config.rpm_factor;
            rpm_counter = 0;
            rpm_counter_time_stamp = millis();
        }
    }    /**
     * return success
     */
    return( true );
}
/**
 * @brief set usb power
 * 
 * @param enable    true = enable, false = disable
 */
static void set_usb_power( bool enable ) {
    digitalWrite( foernuftig_config.usb_power_pin, enable );
    foernuftig_config.usb_power_state = enable;
    foernuftig_config.save();
}
/**
 * @brief set level
 * 
 * @param level     0 = off, 1 = min, 2 = mid, 3 = max
 */
static void set_level( int level ) {
    if( !initialized )
        return;

    switch( level ) {
        case 0:
            set_rpm( 0 );
            break;
        case 1:
            set_rpm( foernuftig_config.min_rpm );
            break;
        case 2:
            set_rpm( foernuftig_config.mid_rpm );
            break;
        case 3:
            set_rpm( foernuftig_config.max_rpm );
            break;
    }
}
/**
 * @brief set rpm
 * 
 * @param rpm 
 */
static void set_rpm( int rpm ) {
    if( !initialized )
        return;

    if( rpm >= foernuftig_config.min_rpm && rpm <= foernuftig_config.max_rpm ) {
        ledcSetup( 0, rpm, 8 );
        ledcWrite( 0, 128 );
        digitalWrite( foernuftig_config.supply_pin, HIGH );
    }
    else if( rpm == 0 )
        digitalWrite( foernuftig_config.supply_pin, LOW );
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
                foernuftig_config.save();
                asyncwebserver_send_websocket_msg( "status\\Save" );
            }
            /**
             * check get status command
             */
            else if ( !strcmp( "get_" MODULE_NAME "_status", cmd ) ) {
                asyncwebserver_send_websocket_msg( MODULE_NAME "_current_rpm\\%d", current_rpm );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_usb_power_state\\%s", foernuftig_config.usb_power_state ? "on" : "off" );
            }
            /**
             * check get settings command
             */
            else if ( !strcmp("get_" MODULE_NAME "_settings", cmd ) ) {
                asyncwebserver_send_websocket_msg( MODULE_NAME "_button_0_pin\\%d", foernuftig_config.button_0_pin );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_button_1_pin\\%d", foernuftig_config.button_1_pin );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_button_2_pin\\%d", foernuftig_config.button_2_pin );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_button_3_pin\\%d", foernuftig_config.button_3_pin );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_fg_pin\\%d", foernuftig_config.fg_pin );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_usb_power_pin\\%d", foernuftig_config.usb_power_pin );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_led_pin\\%d", foernuftig_config.led_pin );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_clk_pin\\%d", foernuftig_config.clk_pin );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_supply_pin\\%d", foernuftig_config.supply_pin );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_rpm_factor\\%f", foernuftig_config.rpm_factor );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_min_rpm\\%d", foernuftig_config.min_rpm );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_mid_rpm\\%d", foernuftig_config.mid_rpm );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_max_rpm\\%d", foernuftig_config.max_rpm );
                asyncwebserver_send_websocket_msg( MODULE_NAME "_current_rpm\\%d", current_rpm );
            }
            else if ( !strcmp( MODULE_NAME "_button_0_pin", cmd ) ) {
                foernuftig_config.button_0_pin = atoi( value );
                asyncwebserver_send_websocket_msg(  MODULE_NAME "_button_0_pin\\%d", foernuftig_config.button_0_pin );
            }
            else if ( !strcmp( MODULE_NAME "_button_1_pin", cmd ) ) {
                foernuftig_config.button_1_pin = atoi( value );
                asyncwebserver_send_websocket_msg(  MODULE_NAME "_button_1_pin\\%d", foernuftig_config.button_1_pin );
            }
            else if ( !strcmp( MODULE_NAME "_button_2_pin", cmd ) ) {
                foernuftig_config.button_2_pin = atoi( value );
                asyncwebserver_send_websocket_msg(  MODULE_NAME "_button_2_pin\\%d", foernuftig_config.button_2_pin );
            }
            else if ( !strcmp( MODULE_NAME "_button_3_pin", cmd ) ) {
                foernuftig_config.button_3_pin = atoi( value );
                asyncwebserver_send_websocket_msg(  MODULE_NAME "_button_3_pin\\%d", foernuftig_config.button_3_pin );
            }
            else if ( !strcmp( MODULE_NAME "_fg_pin", cmd ) ) {
                foernuftig_config.fg_pin = atoi( value );
                asyncwebserver_send_websocket_msg(  MODULE_NAME "_fg_pin\\%d", foernuftig_config.fg_pin );
            }
            else if ( !strcmp( MODULE_NAME "_led_pin", cmd ) ) {
                foernuftig_config.led_pin = atoi( value );
                asyncwebserver_send_websocket_msg(  MODULE_NAME "_led_pin\\%d", foernuftig_config.led_pin );
            }
            else if ( !strcmp( MODULE_NAME "_usb_power_pin", cmd ) ) {
                foernuftig_config.usb_power_pin = atoi( value );
                asyncwebserver_send_websocket_msg(  MODULE_NAME "_usb_power_pin\\%d", foernuftig_config.usb_power_pin );
            }
            else if ( !strcmp( MODULE_NAME "_clk_pin", cmd ) ) {
                foernuftig_config.clk_pin = atoi( value );
                asyncwebserver_send_websocket_msg(  MODULE_NAME "_clk_pin\\%d", foernuftig_config.clk_pin );
            }
            else if( !strcmp(  MODULE_NAME "_supply_pin", cmd ) ) {
                foernuftig_config.supply_pin = atoi( value );
                asyncwebserver_send_websocket_msg(  MODULE_NAME "_supply_pin\\%d", foernuftig_config.supply_pin );
            }
            else if( !strcmp(  MODULE_NAME "_rpm_factor", cmd ) ) {
                foernuftig_config.rpm_factor = atof( value );
                asyncwebserver_send_websocket_msg(  MODULE_NAME "_rpm_factor\\%f", foernuftig_config.rpm_factor );
            }
            else if( !strcmp(  MODULE_NAME "_min_rpm", cmd ) ) {
                foernuftig_config.min_rpm = atoi( value );
                asyncwebserver_send_websocket_msg(  MODULE_NAME "_min_rpm\\%d", foernuftig_config.min_rpm );
            }
            else if( !strcmp(  MODULE_NAME "_mid_rpm", cmd ) ) {
                foernuftig_config.mid_rpm = atoi( value );
                asyncwebserver_send_websocket_msg(  MODULE_NAME "_mid_rpm\\%d", foernuftig_config.mid_rpm );
            }
            else if( !strcmp(  MODULE_NAME "_max_rpm", cmd ) ) {
                foernuftig_config.max_rpm = atoi( value );
                asyncwebserver_send_websocket_msg(  MODULE_NAME "_max_rpm\\%d", foernuftig_config.max_rpm );
            }
            else if( !strcmp(  MODULE_NAME "_usb_power", cmd ) ) {
                set_usb_power( atoi( value ) ? true : false );
            }
            else if( !strcmp(  MODULE_NAME "_set_level", cmd ) ) {
                set_level( atoi( value ) );
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
            html += foernuftig_config_page;
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
            foernuftig_config.save();
            retval = true;
            break;
        case RESET_CONFIG:
            /**
             * reset config
             */
            foernuftig_config.resetToDefault();
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
             * insert foernuftig data
             */
            doc["rpm"] = current_rpm;
            doc["usb_power"] = foernuftig_config.usb_power_state ? true : false;
            retval = true;
            break;
        case MQTTCMND_JSON_DATA:
            /**
             * check json data
             */
            ASSERT( mqttData->doc, MODULE_NAME " mqttstat json data is NULL" );
            /**
             * insert foernuftig data
             */
            if( doc.containsKey( "rpm" ) ) {
                set_rpm( doc["rpm"] );
            }
            if( doc.containsKey( "set_level" ) ) {
                set_level( doc["set_level"] );
            }
            if( doc.containsKey( "usb_power" ) ) {
                set_usb_power( doc["usb_power"] );
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

    return( retval );
}