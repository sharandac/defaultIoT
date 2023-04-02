/**
 * @file core_config.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "core_config.h"

core_config_t::core_config_t() : BaseJsonConfig( CORE_JSON_CONFIG_FILE ) {
}

bool core_config_t::onSave(JsonDocument& doc) {

    doc["frequency"] = frequency;
    doc["brownout_detection"] = brownout_detection;
    doc["watchdog"] = watchdog;
    doc["frequency_scaling"] = frequency_scaling;
    doc["light_sleep"] = light_sleep;

    return true;
}

bool core_config_t::onLoad(JsonDocument& doc) {

    frequency = doc["frequency"] | 240;
    brownout_detection = doc["brownout_detection"] | true;
    watchdog = doc["watchdog"] | true;
    frequency_scaling = doc["frequency_scaling"] | false;
    light_sleep = doc["light_sleep"] | false;
    
    return true;
}

bool core_config_t::onDefault( void ) {

    return true;
}
