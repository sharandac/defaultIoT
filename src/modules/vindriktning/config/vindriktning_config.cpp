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
#include "vindriktning_config.h"

vindriktning_config_t::vindriktning_config_t() : BaseJsonConfig( VINDRIKTNING_JSON_CONFIG_FILE ) {}

bool vindriktning_config_t::onSave(JsonDocument& doc) {

    doc["RXpin"] = RXpin;
    doc["mqtt_msg_stat"] = mqtt_msg_stat;

    return true;
}

bool vindriktning_config_t::onLoad(JsonDocument& doc) {

    RXpin = doc["RXpin"] | 16;
    mqtt_msg_stat = doc["mqtt_msg_stat"] | true;

    return true;
}

bool vindriktning_config_t::onDefault( void ) {

    return true;
}
