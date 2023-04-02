/**
 * @file servo.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
    /**
     * @brief output config page
     */
    static const char servo_config_head[] =
        "<h2>pin settings</h2>\n"
        "<script>set_settings_namespace(\"get_servo_settings\");</script>\n"
        "<script>set_status_namespace(\"get_servo_status\");</script>\n"
        "<script>set_save_namespace(\"save_servo_settings\");</script>\n"
        "<div id=\"output_settings\" class=\"hbox\">\n";
    static const char servo_config_footer[] =
        "  <h2>mqtt msg settings</h2>\n"
/*
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"        
        "      <input type=\"checkbox\" id=\"servo_mqtt_msg_on_change\" ><label for=\"servo_mqtt_msg_on_change\">send servo status on change via mqtt status</label>\n"
        "    </div>\n"
        "  </div>\n"
*/
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"        
        "      <input type=\"checkbox\" id=\"servo_mqtt_msg_stat\" ><label for=\"servo_mqtt_msg_stat\">send servo status via mqtt status</label>\n"
        "    </div>\n"
        "  </div>\n"
        "</div>\n"
        "<br>\n"
        "<br>\n"
        "<button type='button' onclick='";
    static const char servo_config_sendBTN[] =
        "SendCheckboxSetting(\"servo_mqtt_msg_on_change\");SendCheckboxSetting(\"servo_mqtt_msg_stat\");SaveSettings();get_settings();' class='button'>save</button>";
