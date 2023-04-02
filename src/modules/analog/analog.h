/**
 * @file analog.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
    /**
     * @brief input config page
     */
    static const char analog_config_head[] =
        "<h2>pin settings</h2>\n"
        "<script>set_settings_namespace(\"get_analog_settings\");</script>\n"
        "<script>set_status_namespace(\"get_analog_status\");</script>\n"
        "<script>set_save_namespace(\"save_analog_settings\");</script>\n"
        "<div id=\"analog_settings\" class=\"hbox\">\n";
    static const char analog_config_footer[] =
        "  <h2>mqtt msg settings</h2>\n"
/*
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"        
        "      <input type=\"checkbox\" id=\"analog_mqtt_msg_on_change\" ><label for=\"analog_mqtt_msg_on_change\">send analog status on change via mqtt status</label>\n"
        "    </div>\n"
        "  </div>\n"
*/
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"        
        "      <input type=\"checkbox\" id=\"analog_mqtt_msg_stat\" ><label for=\"analog_mqtt_msg_stat\">send ananlog status via mqtt status</label>\n"
        "    </div>\n"
        "  </div>\n"
        "</div>\n"
        "<br>\n"
        "<br>\n"
        "<button type='button' onclick='";
    static const char analog_config_sendBTN[] =
        "SendCheckboxSetting(\"analog_mqtt_msg_on_change\");SendCheckboxSetting(\"analog_mqtt_msg_stat\");SaveSettings();get_settings();' class='button'>save</button>";
