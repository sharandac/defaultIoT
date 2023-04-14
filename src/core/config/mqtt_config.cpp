/**
 * @file mqtt_config.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "mqtt_config.h"

CLASS_NAME_T::CLASS_NAME_T() : BaseJsonConfig( JSON_CONFIG_FILE ) {}

bool CLASS_NAME_T::onSave(JsonDocument& doc) {

    doc["server"] = server;
    doc["port"] = port;
    doc["username"] = username;
    doc["password"] = password;
    doc["topic"] = topic;
    doc["interval"] = interval;
    doc["sendstats"] = sendstats;

    return true;
}

bool CLASS_NAME_T::onLoad(JsonDocument& doc) {

    strlcpy( server, doc["server"] | "", sizeof( server ) );
    port = doc["port"] | 1883;
    strlcpy( username, doc["username"] | "", sizeof( username ) );
    strlcpy( password, doc["password"] | "", sizeof( password ) );
    strlcpy( topic, doc["topic"] | "", sizeof( topic ) );
    interval = doc["interval"] | 15;
    sendstats = doc["sendstats"] | true;
    
    return true;
}

bool CLASS_NAME_T::onDefault( void ) {

    return true;
}
