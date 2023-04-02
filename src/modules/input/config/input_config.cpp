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
#include "input_config.h"

input_config_t::input_config_t() : BaseJsonConfig( INPUT_JSON_CONFIG_FILE ) {}

bool input_config_t::onSave(JsonDocument& doc) {

    doc["input_count"] = input_count;

    for( size_t i = 0 ; i < input_count && i < MAX_INPUTS; i++ ) {
        
        if( device[i].pin <= 0 || device[i].pin >= 40 )
            device[i].enaled = false;
        else
            device[i].enaled = true;
            
        doc["input"][i]["enaled"] = device[i].enaled;
        doc["input"][i]["pin"] = device[i].pin;
        doc["input"][i]["pin_cfg"] = device[i].pin_cfg;
        doc["input"][i]["invert"] = device[i].invert;
    }

    doc["mqtt_msg_on_change"] = mqtt_msg_on_change;
    doc["mqtt_msg_stat"] = mqtt_msg_stat;

    return true;
}

bool input_config_t::onLoad(JsonDocument& doc) {

    input_count = doc["input_count"] | 0;

    for( size_t i = 0 ; i < input_count && i < MAX_INPUTS ; i++ ) {
        device[i].enaled = doc["input"][i]["enaled"] | true;
        device[i].pin = doc["input"][i]["pin"] | -1;
        device[i].pin_cfg = doc["input"][i]["pin_cfg"] | INPUT;
        device[i].invert = doc["input"][i]["invert"] | false;

        if( device[i].pin <= 0 || device[i].pin >= 40 )
            device[i].enaled = false;
    }

    mqtt_msg_stat = doc["mqtt_msg_stat"] | false;
    mqtt_msg_on_change = doc["mqtt_msg_on_change"] | false;

    return true;
}

bool input_config_t::onDefault( void ) {
    return true;
}
