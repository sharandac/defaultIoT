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
/**
 * include all core modules
 */
#include "config.h"
#include "core/core.h"
#include "core/modul_mgmt.h"

void setup() {
    /**
     * start core services with own task
     */
    core_setup();
    /**
     * module registration
     */
    module_mgmt_call_setup();
    /**
     * start all modules dependent on the configuration
     */
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