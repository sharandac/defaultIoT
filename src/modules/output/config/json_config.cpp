/**
 * @file output_config.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.2
 * @date 2023-03-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "json_config.h"

CLASS_NAME_T::CLASS_NAME_T() : BaseJsonConfig( JSON_CONFIG_FILE ) {}

bool CLASS_NAME_T::onSave(JsonDocument& doc) {

    doc["output_count"] = output_count;

    for( size_t i = 0 ; i < output_count && i < MAX_OUTPUTS; i++ ) {
        
        if( device[i].pin <= 0 || device[i].pin >= 40 )
            device[i].enaled = false;
        else
            device[i].enaled = true;
            
        doc["output"][i]["enaled"] = device[i].enaled;
        doc["output"][i]["pin"] = device[i].pin;
        doc["output"][i]["id"] = device[i].id;
        doc["output"][i]["state"] = device[i].state;
    }

    doc["mqtt_msg_on_change"] = mqtt_msg_on_change;
    doc["mqtt_msg_stat"] = mqtt_msg_stat;

    return true;
}

bool CLASS_NAME_T::onLoad(JsonDocument& doc) {

    output_count = doc["output_count"] | 0;

    for( size_t i = 0 ; i < output_count && i < MAX_OUTPUTS ; i++ ) {
        device[i].enaled = doc["output"][i]["enaled"] | true;
        device[i].pin = doc["output"][i]["pin"] | -1;
        strncpy( device[i].id, doc["output"][i]["id"] | "output", sizeof( device[i].id ) );
        device[i].state = doc["output"][i]["state"] | false;

        if( device[i].pin <= 0 || device[i].pin >= 40 )
            device[i].enaled = false;
    }

    mqtt_msg_stat = doc["mqtt_msg_stat"] | false;
    mqtt_msg_on_change = doc["mqtt_msg_on_change"] | false;

    return true;
}

bool CLASS_NAME_T::onDefault( void ) {
    return true;
}
