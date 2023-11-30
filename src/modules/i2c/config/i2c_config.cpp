/**
 * @file input_config.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.3
 * @date 2023-03-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "i2c_config.h"

CLASS_NAME_T::CLASS_NAME_T() : BaseJsonConfig( JSON_CONFIG_FILE ) {}

bool CLASS_NAME_T::onSave(JsonDocument& doc) {

    if( scl > 0 && scl < 40 && sda > 0 && sda < 40 )
        enabled = true;
    else
        enabled = false;

    doc["enaled"] = enabled;
    doc["frequency"] = frequency;
    doc["pin"]["scl"] = scl;  
    doc["pin"]["sda"] = sda;  
    doc["mqtt_msg_stat"] = mqtt_msg_stat;

    return true;
}

bool CLASS_NAME_T::onLoad(JsonDocument& doc) {

    enabled = doc["enaled"] | true;
    frequency = doc["frequency"] | 1000;
    scl = doc["pin"]["scl"] | -1;
    sda = doc["pin"]["sda"] | -1;
    mqtt_msg_stat = doc["mqtt_msg_stat"] | false;

    if( scl > 0 && scl < 40 && sda > 0 && sda < 40 )
        enabled = true;
    else
        enabled = false;

    return true;
}

bool CLASS_NAME_T::onDefault( void ) {
    return true;
}
