/**
 * @file vindriktning_config.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "json_config.h"

CLASS_NAME_T::CLASS_NAME_T() : BaseJsonConfig( JSON_CONFIG_FILE ) {}

bool CLASS_NAME_T::onSave(JsonDocument& doc) {

    doc["RXpin"] = RXpin;
    doc["mqtt_msg_stat"] = mqtt_msg_stat;

    return true;
}

bool CLASS_NAME_T::onLoad(JsonDocument& doc) {

    RXpin = doc["RXpin"] | 16;
    mqtt_msg_stat = doc["mqtt_msg_stat"] | true;

    return true;
}

bool CLASS_NAME_T::onDefault( void ) {

    return true;
}
