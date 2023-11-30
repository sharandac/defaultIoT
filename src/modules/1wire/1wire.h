/**
 * @file onewire.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
    /**
     * @brief typedef for temp sensors
     */
    typedef struct {
        bool valid;                 // true if sensor is valid
        unsigned char addr[8];      // sensor address
        String addr_str;            // sensor address
        float temp = 0.0f;          // temperature
        String temp_str;            // temperature as string
    } temp_sensors_t;
    /**
     * @brief onewire config page
     */
    static const char onewire_config_head[] =
        "<h2>OneWire settings</h2>\n"
        "<script>set_settings_namespace(\"get_onewire_settings\");</script>\n"
        "<script>set_status_namespace(\"get_onewire_status\");</script>\n"
        "<script>set_save_namespace(\"save_onewire_settings\");</script>\n"
        "<div id=\"onewire_settings\" class=\"hbox\">\n"
        "  <div class='vbox'>\n"
        "    <div class='box'>\n"
        "      <label>pin</label><input type='text' size='32' id='onewire_pin'>\n"
        "      <input type=\"checkbox\" id=\"onewire_report_in_fahrenheit\" ><label for=\"onewire_report_in_fahrenheit\">report temperature in fahrenheit</label>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class='vbox'>\n"
        "    <div class='box'>\n"
        "      <label>temp read interval</label><input type='text' size='32' id='onewire_interval'>\n"
        "    </div>\n"
        "  </div>\n";
    static const char onewire_config_footer[] =
        "  <h2>mqtt msg settings</h2>\n"
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"        
        "      <input type=\"checkbox\" id=\"onewire_mqtt_msg_stat\" ><label for=\"onewire_mqtt_msg_stat\">send onewire status via mqtt status</label>\n"
        "    </div>\n"
        "  </div>\n"
        "</div>\n"
        "<br>\n"
        "<br>\n"
        "<button type='button' onclick='SendSetting(\"onewire_interval\");SendSetting(\"onewire_pin\");SendCheckboxSetting(\"onewire_mqtt_msg_stat\");SendCheckboxSetting(\"onewire_report_in_fahrenheit\");SaveSettings();get_settings();' class='button'>save</button>";
