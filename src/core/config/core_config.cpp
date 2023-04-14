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

CLASS_NAME_T::CLASS_NAME_T() : BaseJsonConfig( JSON_CONFIG_FILE ) {
}

bool CLASS_NAME_T::onSave(JsonDocument& doc) {

    doc["frequency"] = frequency;
    doc["brownout_detection"] = brownout_detection;
    doc["watchdog"] = watchdog;
    doc["frequency_scaling"] = frequency_scaling;
    doc["light_sleep"] = light_sleep;

    return true;
}

bool CLASS_NAME_T::onLoad(JsonDocument& doc) {

    frequency = doc["frequency"] | 240;
    brownout_detection = doc["brownout_detection"] | true;
    watchdog = doc["watchdog"] | true;
    frequency_scaling = doc["frequency_scaling"] | false;
    light_sleep = doc["light_sleep"] | false;
    
    return true;
}

bool CLASS_NAME_T::onDefault( void ) {

    return true;
}
