/**
 * @file pwm.h
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
     * @brief pwm config page
     */
    static const char pwm_config_page_head[] =
        "<h2>PWM settings</h2>\n"
        "<script>set_settings_namespace(\"get_pwm_settings\");</script>\n"
        "<script>set_status_namespace(\"get_pwm_status\");</script>\n"
        "<script>set_save_namespace(\"save_pwm_settings\");</script>\n"
        "<div id=\"pwm_settings\" class=\"hbox\">\n";
    static const char pwm_config_page_footer[] =
        "  <h2>mqtt msg settings</h2>\n"
/*
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"        
        "      <input type=\"checkbox\" id=\"pwm_mqtt_msg_on_change\" ><label for=\"pwm_mqtt_msg_on_change\">send pwm status on change via mqtt status</label>\n"
        "    </div>\n"
        "  </div>\n"
*/
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"        
        "      <input type=\"checkbox\" id=\"pwm_mqtt_msg_stat\" ><label for=\"pwm_mqtt_msg_stat\">send pwm status via mqtt status</label>\n"
        "    </div>\n"
        "  </div>\n"
        "</div>\n"
        "<br>\n"
        "<br>\n"
        "<button type='button' onclick='";
    static const char pwm_config_page_sendBTN[] =
        "SendCheckboxSetting(\"pwm_mqtt_msg_on_change\");SendCheckboxSetting(\"pwm_mqtt_msg_stat\");SaveSettings();get_settings();' class='button'>save</button>";
