/**
 * @file wifi_config.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <WiFi.h>
#include "wifi_config.h"

CLASS_NAME_T::CLASS_NAME_T() : BaseJsonConfig( JSON_CONFIG_FILE ) {
}

bool CLASS_NAME_T::onSave(JsonDocument& doc) {

    doc["hostname_unique"] = hostname_unique;
    doc["hostname"] = hostname;
    doc["ssid"] = ssid;
    doc["password"] = password;
    doc["enable_softap"] = enable_softap;
    doc["softap_ssid"] = softap_ssid;
    doc["softap_password"] = softap_password;
    doc["timeout"] = timeout;
    doc["low_bandwidth"] = low_bandwidth;
    doc["low_power"] = low_power;
    doc["enable_led"] = enable_led;
    doc["led_pin"] = led_pin;

    return true;
}

bool CLASS_NAME_T::onLoad(JsonDocument& doc) {
    /*
     * make an uniqe Hostname for the SoftAp SSID
     */
    uint8_t mac[6];
    WiFi.macAddress( mac );
    snprintf( hostname_unique, sizeof( hostname_unique ), DEVICE_NAME "_%02x%02x%02x", mac[3], mac[4], mac[5] );
    
    strlcpy( hostname, doc["hostname"] | hostname_unique, sizeof( hostname ) );
    strlcpy( ssid, doc["ssid"] | WIFI_SSID, sizeof( ssid ) );
    strlcpy( password, doc["password"] | WIFI_PASSWORD, sizeof( password ) );
    enable_softap = doc["enable_softap"] | true;
    strlcpy( softap_ssid, doc["softap_ssid"] | hostname_unique, sizeof( softap_ssid ) );
    strlcpy( softap_password, doc["softap_password"] | DEVICE_NAME, sizeof( softap_password ) );
    timeout = doc["timeout"] | 15;
    low_bandwidth = doc["low_bandwidth"] | false;
    low_power = doc["low_power"] | false;
    enable_led = doc["enable_led"] | false;
    led_pin = doc["led_pin"] | -1;
    
    return true;
}

bool CLASS_NAME_T::onDefault( void ) {
    /*
     * make an uniqe Hostname for the SoftAp SSID
     */
    uint8_t mac[6];
    WiFi.macAddress( mac );
    snprintf( hostname, sizeof( hostname ), DEVICE_NAME "_%02x%02x%02x", mac[3], mac[4], mac[5] );
    snprintf( softap_ssid, sizeof( softap_ssid ), DEVICE_NAME "_%02x%02x%02x", mac[3], mac[4], mac[5] );

    return true;
}
