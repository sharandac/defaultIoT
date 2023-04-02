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

mqtt_config_t::mqtt_config_t() : BaseJsonConfig( MQTT_JSON_CONFIG_FILE ) {
}

bool mqtt_config_t::onSave(JsonDocument& doc) {

    doc["server"] = server;
    doc["port"] = port;
    doc["username"] = username;
    doc["password"] = password;
    doc["topic"] = topic;
    doc["interval"] = interval;
    doc["realtimestats"] = realtimestats;
    doc["sendstats"] = sendstats;

    return true;
}

bool mqtt_config_t::onLoad(JsonDocument& doc) {

    strlcpy( server, doc["server"] | "", sizeof( server ) );
    port = doc["port"] | 1883;
    strlcpy( username, doc["username"] | "", sizeof( username ) );
    strlcpy( password, doc["password"] | "", sizeof( password ) );
    strlcpy( topic, doc["topic"] | "", sizeof( topic ) );
    interval = doc["interval"] | 15;
    realtimestats = doc["realtimestats"] | true;
    sendstats = doc["sendstats"] | true;
    
    return true;
}

bool mqtt_config_t::onDefault( void ) {

    return true;
}
