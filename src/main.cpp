/**
 * @file main.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 1.1
 * @date 2022-10-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <Arduino.h>
#include <esp_task_wdt.h>
#include "SPIFFS.h"
#include "core/utils/callback.h"
/**
 * include all core modules
 */
#include "config.h"
#include "core/core.h"
#include "core/modul_mgmt.h"

void setup() {
    /**
     * start serial
     */
    Serial.begin( 115200 );
    log_i("start %s", DEVICE_NAME );
    /**
     * mount SPIFFS
     */
    if ( !SPIFFS.begin() ) {        
        /*
         * format SPIFFS if not aviable
         */
        SPIFFS.format();
        log_i("formating SPIFFS");
    }  
    /**
     * module registration
     */
    module_mgmt_call_setup();
    /**
     * start core services with own task
     */
    core_setup();
    /**
     * start all modules dependent on the configuration
     */
    vTaskDelay( 1000 / portTICK_PERIOD_MS );
    module_mgmt_call_init();
}

void loop() {
    /**
     * reset watchdag
     */
    core_reset_wdt();
    /**
     * loop all registed and active modules
     */
    module_mgmt_call_loop();
}