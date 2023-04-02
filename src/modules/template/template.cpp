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
#include "core/webserver.h"
#include "core/mqttclient.h"
#include "core/modul_mgmt.h"
#include "core/utils/alloc.h"
/**
 * module namespace
 */
#define MODULE_NAME                "template"
/**
 * local variables
 */
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
    mqtt_client_register_cb( MQTTSTAT_JSON_DATA | MQTTCMND_JSON_DATA | MQTTTELE_JSON_DATA, mqttclient_cb, MODULE_NAME );
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
     *  initialize your hardware
     */
    pinMode( GPIO_NUM_26, OUTPUT );
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
     *  deinitialize your hardware
     */
    pinMode( GPIO_NUM_26, INPUT );
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
    /**
     * check if module is initialized
     */
    ASSERT( initialized, MODULE_NAME " are not initialized, check your code" );
    /**
     * to crazy stuff
     */
    if( digitalRead( GPIO_NUM_26 ) )
        digitalWrite( GPIO_NUM_26, LOW );
    else
        digitalWrite( GPIO_NUM_26, HIGH );
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

    return( retval );
}