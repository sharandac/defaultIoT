/**
 * @file vindriktning_config.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "json_config.h"

CLASS_NAME_T::CLASS_NAME_T() : BaseJsonConfig( JSON_CONFIG_FILE ) {}

bool CLASS_NAME_T::onSave(JsonDocument& doc) {

    doc["fan"]["hardware"]["button_0_pin"] = button_0_pin;
    doc["fan"]["hardware"]["button_1_pin"] = button_1_pin;
    doc["fan"]["hardware"]["button_2_pin"] = button_2_pin;
    doc["fan"]["hardware"]["button_3_pin"] = button_3_pin;
    doc["fan"]["hardware"]["supply_pin"] = supply_pin;
    doc["fan"]["hardware"]["clk_pin"] = clk_pin;
    doc["fan"]["hardware"]["fg_pin"] = fg_pin;
    doc["fan"]["hardware"]["led_pin"] = led_pin;
    doc["fan"]["hardware"]["usb_power_pin"] = usb_power_pin;
    doc["fan"]["hardware"]["usb_power_state"] = usb_power_state;
    doc["fan"]["min_rpm"] = min_rpm;
    doc["fan"]["mid_rpm"] = mid_rpm;
    doc["fan"]["max_rpm"] = max_rpm;
    doc["fan"]["rpm_factor"] = rpm_factor;
    doc["mqtt_msg_stat"] = mqtt_msg_stat;

    return true;
}

bool CLASS_NAME_T::onLoad(JsonDocument& doc) {

    button_0_pin = doc["fan"]["hardware"]["button_0_pin"] | 21;
    button_1_pin = doc["fan"]["hardware"]["button_1_pin"] | 22;
    button_2_pin = doc["fan"]["hardware"]["button_2_pin"] | 32;
    button_3_pin = doc["fan"]["hardware"]["button_3_pin"] | 25;
    supply_pin = doc["fan"]["hardware"]["supply_pin"] | 19;
    clk_pin= doc["fan"]["hardware"]["clk_pin"] | 18;
    fg_pin= doc["fan"]["hardware"]["fg_pin"] | 33;
    led_pin = doc["fan"]["hardware"]["led_pin"] | 35;
    usb_power_pin = doc["fan"]["hardware"]["usb_power_pin"] | 17;
    usb_power_state = doc["fan"]["hardware"]["usb_power_state"] | false;
    min_rpm= doc["fan"]["min_rpm"] | 50;
    mid_rpm= doc["fan"]["mid_rpm"] | 150;
    max_rpm= doc["fan"]["max_rpm"] | 300; 
    rpm_factor= doc["fan"]["rpm_factor"] | 2.0;
    mqtt_msg_stat = doc["mqtt_msg_stat"] | true;

    return true;
}

bool CLASS_NAME_T::onDefault( void ) {

    return true;
}
