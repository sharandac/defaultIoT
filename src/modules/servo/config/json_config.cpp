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
    for( size_t i = 0 ; i < count && i < MAX_SERVOS; i++ ) {
        doc["servo"][i]["pin"] = device[i].pin;
        doc["servo"][i]["id"] = device[i].id;
        doc["servo"][i]["min_angle"] = device[i].min_angle;
        doc["servo"][i]["neutal_angle"] = device[i].neutal_angle;
        doc["servo"][i]["max_angle"] = device[i].max_angle;
        doc["servo"][i]["min_value"] = device[i].min_value;
        doc["servo"][i]["max_value"] = device[i].max_value;
        doc["servo"][i]["neutral_on_start"] = device[i].neutral_on_start;
        doc["servo"][i]["neutral_after_move"] = device[i].neutral_after_move;
        doc["servo"][i]["detach_time"] = device[i].detach_time;
        doc["servo"][i]["state"] = device[i].state;
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

bool CLASS_NAME_T::onLoad(JsonDocument& doc) {
    /**
     * load the servo count
     */
    count = doc["count"] | 0;
    /**
     * crawl through all servos and check if the pin is valid
     */
    for( size_t i = 0 ; i < count && i < MAX_SERVOS ; i++ ) {
        device[i].pin = doc["servo"][i]["pin"] | 0;
        strncpy( device[i].id, doc["servo"][i]["id"] | "servo", sizeof( device[i].id ) );
        device[i].min_angle = doc["servo"][i]["min_angle"] | 0;
        device[i].neutal_angle = doc["servo"][i]["neutal_angle"] | 90;
        device[i].max_angle = doc["servo"][i]["max_angle"] | 180;
        device[i].min_value = doc["servo"][i]["min_value"] | 0;
        device[i].max_value = doc["servo"][i]["max_value"] | 180;
        device[i].neutral_on_start = doc["servo"][i]["neutral_on_start"] | false;
        device[i].neutral_after_move = doc["servo"][i]["neutral_after_move"] | false;
        device[i].detach_time = doc["servo"][i]["detach_time"] | 1000;
        device[i].state = doc["servo"][i]["state"] | false;
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

bool CLASS_NAME_T::onDefault( void ) {
    return true;
}
