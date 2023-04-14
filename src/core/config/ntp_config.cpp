/**
 * @file ntp_config.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "ntp_config.h"

CLASS_NAME_T::CLASS_NAME_T() : BaseJsonConfig( JSON_CONFIG_FILE ) {}

bool CLASS_NAME_T::onSave(JsonDocument& doc) {

    doc["enable"] = enable;
    doc["server"] = server;
    doc["timezone"] = timezone;
    doc["interval"] = interval;

    return true;
}

bool CLASS_NAME_T::onLoad(JsonDocument& doc) {

    enable = doc["enable"] | true;
    strlcpy( server, doc["server"] | "pool.ntp.org", sizeof( server ) );
    strlcpy( timezone, doc["timezone"] | "CET-1CEST,M3.5.0,M10.5.0/3", sizeof( timezone ) );
    interval = doc["interval"] | NTP_RENEW_INTERVAL;
    
    return true;
}

bool CLASS_NAME_T::onDefault( void ) {
    /*
     * make an uniqe Hostname for the SoftAp SSID
     */
    enable = true;
    strlcpy( server, "pool.ntp.org", sizeof( server ) );
    strlcpy( timezone, "CET-1CEST,M3.5.0,M10.5.0/3", sizeof( timezone ) );
    interval = NTP_RENEW_INTERVAL;

    return true;
}
