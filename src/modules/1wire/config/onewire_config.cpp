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
#include "onewire_config.h"

CLASS_NAME_T::CLASS_NAME_T() : BaseJsonConfig( JSON_CONFIG_FILE ) {}

bool CLASS_NAME_T::onSave(JsonDocument& doc) {

    if( pin <= 0 || pin >= 40 )
        enabled = false;
    else
        enabled = true;

    doc["enaled"] = enabled;
    doc["pin"] = pin;   
    doc["interval"] = interval;
    doc["reportInFahrenheit"] = reportInFahrenheit;     
    doc["mqtt_msg_stat"] = mqtt_msg_stat;

    return true;
}

bool CLASS_NAME_T::onLoad(JsonDocument& doc) {

    if( pin <= 0 || pin >= 40 )
        enabled = false;

    enabled = doc["enaled"] | true;
    pin = doc["pin"] | -1;
    interval = doc["interval"] | 15;
    reportInFahrenheit = doc["reportInFahrenheit"] | false;
    mqtt_msg_stat = doc["mqtt_msg_stat"] | false;

    return true;
}

bool CLASS_NAME_T::onDefault( void ) {
    return true;
}
