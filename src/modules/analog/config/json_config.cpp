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
#include "json_config.h"

CLASS_NAME_T::CLASS_NAME_T() : BaseJsonConfig( JSON_CONFIG_FILE ) {}

bool CLASS_NAME_T::onSave(JsonDocument& doc) {
    /**
     * save the servo count
     */
    doc["count"] = count;
    /**
     * crawl through all servos and check if the pin is valid
     */
    for( size_t i = 0 ; i < count && i < MAX_ANALOG; i++ ) {
        doc["analog"][i]["enable"] = device[i].enable;
        doc["analog"][i]["pin"] = device[i].pin;
        doc["analog"][i]["id"] = device[i].id;
        doc["analog"][i]["trigger_low"] = device[i].trigger_low;
        doc["analog"][i]["trigger_high"] = device[i].trigger_high;
        doc["analog"][i]["trigger_count_reset"] = device[i].trigger_count_reset;
        /**
         * check if the pin is valid
         */
        if( ( device[i].pin >= 32 && device[i].pin <= 36 ) || device[i].pin == 39 )
            device[i].enable = true;
        else
            device[i].enable = false;
    }
    /**
     * save the mqtt settings
     */
    doc["mqtt_msg_on_change"] = mqtt_msg_on_change;
    doc["mqtt_msg_stat"] = mqtt_msg_stat;

    return true;
}

bool CLASS_NAME_T::onLoad(JsonDocument& doc) {
    /**
     * load the servo count
     */
    count = doc["count"] | 0;
    /**
     * crawl through all servos and check if the pin is valid
     */
    for( size_t i = 0 ; i < count && i < MAX_ANALOG ; i++ ) {
        device[i].enable = doc["analog"][i]["enable"] | false;
        device[i].pin = doc["analog"][i]["pin"] | 0;
        strncpy( device[i].id, doc["analog"][i]["id"] | "analog", sizeof( device[i].id ) );
        device[i].trigger_low = doc["analog"][i]["trigger_low"] | 0;
        device[i].trigger_high = doc["analog"][i]["trigger_high"] | 0;
        device[i].trigger_count_reset = doc["analog"][i]["trigger_count_reset"] | false;
        /**
         * check if the pin is valid
         */
        if( ( device[i].pin >= 32 && device[i].pin <= 36 ) || device[i].pin == 39 )
            device[i].enable = true;
        else
            device[i].enable = false;
    }
    /**
     * load the mqtt settings
     */
    mqtt_msg_stat = doc["mqtt_msg_stat"] | false;
    mqtt_msg_on_change = doc["mqtt_msg_on_change"] | false;

    return true;
}

bool CLASS_NAME_T::onDefault( void ) {
    return true;
}
