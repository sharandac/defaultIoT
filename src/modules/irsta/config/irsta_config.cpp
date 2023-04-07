/****************************************************************************
 *   Tu May 22 21:23:51 2020
 *   Copyright  2020  Dirk Brosswick
 *   Email: dirk.brosswick@googlemail.com
 ****************************************************************************/
 
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "irsta_config.h"

irsta_config_t::irsta_config_t() : BaseJsonConfig(IRSTA_JSON_CONFIG_FILE) {}

bool irsta_config_t::onSave(JsonDocument& doc) {

    doc["led"]["max_led"] = max_led;
    doc["led"]["count"] = led_count;
    doc["led"]["max"] = max;
    doc["led"]["red"] = red;
    doc["led"]["green"] = green;
    doc["led"]["blue"] = blue;
    doc["led"]["hardware"]["clk_pin"] = clk_pin;
    doc["pin"]["button_0"] = button_0;
    doc["pin"]["button_1"] = button_1;
    doc["pin"]["button_2"] = button_2;
    doc["pin"]["button_3"] = button_3;
    doc["pin"]["threshold"] = threshold;
    doc["mqtt_msg_stat"] = mqtt_msg_stat;

    if( max >= max_led )
        max = max_led;

    return true;
}

bool irsta_config_t::onLoad(JsonDocument& doc) {

    max_led = doc["led"]["max_led"] | 112;
    led_count = doc["led"]["count"] | 512;
    max = doc["led"]["max"] | 64; 
    red = doc["led"]["red"] | 255;
    green = doc["led"]["green"] | 255;
    blue = doc["led"]["blue"] | 255;
    clk_pin = doc["led"]["hardware"]["clk_pin"] | 18;
    button_0 = doc["pin"]["button_0"] | 21;
    button_1 = doc["pin"]["button_1"] | 22;
    button_2 = doc["pin"]["button_2"] | 32;
    button_3 = doc["pin"]["button_3"] | 25;
    threshold = doc["pin"]["threshold"] | 25;
    mqtt_msg_stat = doc["mqtt_msg_stat"] | true;

    if( max >= max_led )
        max = max_led;

    return true;
}

bool irsta_config_t::onDefault( void ) {
    return true;
}
