<p align="center">
<img src="https://img.shields.io/github/last-commit/sharandac/defaultIoT.svg?style=for-the-badge" />
&nbsp;
<img src="https://img.shields.io/github/license/sharandac/defaultIoT.svg?style=for-the-badge" />
&nbsp;
<a href="https://www.buymeacoffee.com/sharandac" target="_blank"><img src="https://img.shields.io/badge/Buy%20me%20a%20coffee-%E2%82%AC5-orange?style=for-the-badge&logo=buy-me-a-coffee" /></a>
</p>
<hr/>

# Table of Contents

* 1\. [modules](#modules)
* 2\. [minimal example](#minimal-example)
    * 2.1\. [registration()](#registration)
    * 2.2\. [initialize()](#initialize)
    * 2.3\. [deinitialize()](#deinitialize)
    * 2.4\. [loop()](#loop)
    * 2.5\. [Example C code](#example-c-code)
* 3\. [a simple web service](#a-simple-web-service)
* 4\. [a simple mqtt service](#a-simple-mqtt-service)
* 5\. [the magic behind autoregistration](#the-magic-behind-autoregistration)

# modules

Modules are a central part of the basic concept of defaultIoT. This makes it relatively easy to write new modules without reinventing everything every time.
The procedure is quite simple:

* create a new directory in src/modules/
* store your modules files
* recompile the software

And if everything works correctly, not a single line of code in the rest of the program needs to be changed. Everything should be included automatically. This makes it convenient to maintain the rest of the code independently from the modules.

The following modules are implemented:

* [analog](/src/modules/analog/README.md)
* [input](/src/modules/input/README.md)
* [foernuftig](/src/modules/foernuftig/README.md)
* [servo](/src/modules/servo/README.md)
* [pwm](/src/modules/pwm/README.md)
* [output](/src/modules/output/README.md)
* [oneWire](/src/modules/oneWire/README.md)
* [vindriktning](/src/modules/vindriktning/README.md)

# minimal example

First a minimal example of a module. This contains only the most necessary. The most important functions are:

## registration()
This is the most important function. This function is used with the help of this function
```c
static int registed = module_autocall_function( &registration );
```
and is already registered with the module manager at boot time. More about this magic later. This function includes the important task like registering the initialize(), deinitialize() and loop() function with the module manager. Additionally the callback functions for the web server and MQTT service can be set up. Example:
```c
/**
 * register webserver and mqtt client callback function
 */
mqtt_client_register_cb( MQTTSTAT_JSON_DATA | MQTTCMND_JSON_DATA | MQTTTELE_JSON_DATA, mqttclient_cb, MODULE_NAME );
asyncwebserver_register_cb( WS_DATA | WEB_DATA | WEB_MENU | SAVE_CONFIG | RESET_CONFIG, webserver_cb, "/" MODULE_NAME ".htm" );
/**
 * register initialize, deinitialize and loop function
 */
module_mgmt_register( initialize, deinitialize, loop, MODULE_NAME );
```

## initialize()
This function should take all the necessary steps to get the module into a safe startup state. Set up inputs and outputs or load the configuration from a file for example.
```c
/**
 *  initialize your hardware
 */
pinMode( GPIO_NUM_26, OUTPUT );
```
## deinitialize()
This function should do all the necessary steps to get the module into a safe state. Set inputs and outputs back to normal INPUT or save the configuration.
```c
/**
 *  deinitialize your hardware
 */
pinMode( GPIO_NUM_26, INPUT );
```
## loop()
And the loop() function. This needs no explanation. This is called every 25ms by the module manager when the module is activated.

## Example C code
Here is a minimal working example. A module with the name "template" is registered and after activation in the module manager the GPIO_NUM_26 pin is toggled between on and off in an interval of 25ms.
This minimal example can simply be copied and placed anywhere in the src/ directory. However, the src/modules/ directory is preferred. For the sake of order. The file is automatically recognized and the registration() function is executed at boot time. A closer look also reveals that all functions are static and have a local scope on the file. This makes it possible to use this file as a template.

```c
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
 * @brief setup function for template, called by modul autocall function
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
```
After compiling, the module manager look like this:

![module manager](/images/modulemanager.png)

But remember, is this template a web or mqtt service is not implemented yet.

# the magic behind autoregistration

How do I make sure that a function is called automatically without being called explicitly in the setup() function in main.c?
Actually this is quite simple. In the file src/core/modul_mgmt.h is a function and type declared which looks like this:

```c
typedef void ( * MODULE_AUTOCALL_FUNC ) ( void );
int module_autocall_function( MODULE_AUTOCALL_FUNC function );
```

You pass a function pointer to this function. In this case the registration function ( ```registration()```) for our module. So far not very exciting.
The trick is to call this function in another file and store the return value in a static variable that has the file as scope. This ensures that this function is called at initialization before the actual program starts. We only need to store this function pointer in a list for later use like.

So only the following code snippet has to be in the file:
```c
static int registed = module_autocall_function( &registration );
```

Later in the program we can simply process the list with functions pointers at a suitable place.
That was it already. Simple if you think about it.

# a simple web service

# a simple mqtt service

# Contributors

Every Contribution to this repository is highly welcome! Don't fear to create pull requests which enhance or fix the project, you are going to help everybody.
<p>
If you want to donate to the author then you can buy me a coffee.
<br/><br/>
<a href="https://www.buymeacoffee.com/sharandac" target="_blank"><img src="https://img.shields.io/badge/Buy%20me%20a%20coffee-%E2%82%AC5-orange?style=for-the-badge&logo=buy-me-a-coffee" /></a>
</p>


