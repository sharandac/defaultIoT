/**
 * @file servo_config.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "analog_config.h"

analog_config_t::analog_config_t() : BaseJsonConfig( ANALOG_JSON_CONFIG_FILE ) {}

bool analog_config_t::onSave(JsonDocument& doc) {
    /**
     * save the servo count
     */
    doc["count"] = count;
    /**
     * crawl through all servos and check if the pin is valid
     */
    for( size_t i = 0 ; i < count && i < MAX_ANALOG; i++ ) {
        doc["analog"][i]["enaled"] = device[i].enaled;
        doc["analog"][i]["pin"] = device[i].pin;
        doc["analog"][i]["trigger_low"] = device[i].trigger_low;
        doc["analog"][i]["trigger_high"] = device[i].trigger_high;
        doc["analog"][i]["trigger_count_reset"] = device[i].trigger_count_reset;
        /**
         * check if the pin is valid
         */
        if( device[i].pin <= 0 || device[i].pin >= 40 )
            device[i].enaled = false;
        else
            device[i].enaled = true;
    }
    /**
     * save the mqtt settings
     */
    doc["mqtt_msg_on_change"] = mqtt_msg_on_change;
    doc["mqtt_msg_stat"] = mqtt_msg_stat;

    return true;
}

bool analog_config_t::onLoad(JsonDocument& doc) {
    /**
     * load the servo count
     */
    count = doc["count"] | 0;
    /**
     * crawl through all servos and check if the pin is valid
     */
    for( size_t i = 0 ; i < count && i < MAX_ANALOG ; i++ ) {
        device[i].enaled = doc["analog"][i]["enaled"] | false;
        device[i].pin = doc["analog"][i]["pin"] | 0;
        device[i].trigger_low = doc["analog"][i]["trigger_low"] | 0;
        device[i].trigger_high = doc["analog"][i]["trigger_high"] | 0;
        device[i].trigger_count_reset = doc["analog"][i]["trigger_count_reset"] | false;
        /**
         * check if the pin is valid
         */
        if( device[i].pin <= 0 || device[i].pin >= 40 )
            device[i].enaled = false;
    }
    /**
     * load the mqtt settings
     */
    mqtt_msg_stat = doc["mqtt_msg_stat"] | false;
    mqtt_msg_on_change = doc["mqtt_msg_on_change"] | false;

    return true;
}

bool analog_config_t::onDefault( void ) {
    return true;
}
