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

    doc["pwm_count"] = pwm_count;

    for( size_t i = 0 ; i < pwm_count && i < MAX_PWM; i++ ) {
        
        if( device[ i ].pin <= 0 || device[ i ].pin >= 40 )
            device[ i ].enaled = false;
        else
            device[ i ].enaled = true;
            
        doc["pwm"][ i ]["enaled"] = device[ i ].enaled;
        doc["pwm"][ i ]["pin"] = device[ i ].pin;
        doc["pwm"][ i ]["id"] = device[ i ].id;
        doc["pwm"][ i ]["min"] = device[ i ].min;
        doc["pwm"][ i ]["max"] = device[ i ].max;
        doc["pwm"][ i ]["frequency"] = device[ i ].frequency;
    }

    doc["mqtt_msg_on_change"] = mqtt_msg_on_change;
    doc["mqtt_msg_stat"] = mqtt_msg_stat;

    return true;
}

bool CLASS_NAME_T::onLoad(JsonDocument& doc) {

    pwm_count = doc["pwm_count"] | 0;

    for( size_t i = 0 ; i < pwm_count && i < MAX_PWM ; i++ ) {
        device[ i ].enaled = doc["pwm"][ i ]["enaled"] | true;
        device[ i ].pin = doc["pwm"][ i ]["pin"] | -1;
        strncpy( device[ i ].id, doc["pwm"][ i ]["id"] | "pwm", sizeof( device[ i ].id ) );
        device[ i ].min = doc["pwm"][ i ]["min"] | 0;
        device[ i ].max = doc["pwm"][ i ]["max"] | 255;
        device[ i ].frequency = doc["pwm"][ i ]["frequency"] | 8000;

        if( device[ i ].pin <= 0 || device[ i ].pin >= 40 )
            device[ i ].enaled = false;
    }

    mqtt_msg_stat = doc["mqtt_msg_stat"] | false;
    mqtt_msg_on_change = doc["mqtt_msg_on_change"] | false;

    return true;
}

bool CLASS_NAME_T::onDefault( void ) {
    return true;
}
