/**
 * @file i2c.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
    /**
     * @brief i2c config page
     */
    static const char i2c_config_head[] =
        "<h2>I2C settings</h2>\n"
        "<script>set_settings_namespace(\"get_i2c_settings\");</script>\n"
        "<script>set_status_namespace(\"get_i2c_status\");</script>\n"
        "<script>set_save_namespace(\"save_i2c_settings\");</script>\n"
        "<div id=\"i2c_settings\" class=\"hbox\">\n"
        "  <div class='vbox'>\n"
        "    <div class=\"box\">\n"        
        "      <select id='i2c_frequency'><option value='100'>100kHz</option><option value='400'>400kHz</option><option value='1000'>1MHz</option></select>"
        "    </div>\n"
        "  </div>\n"
        "  <div class='vbox'>\n"
        "    <div class='box'>\n"
        "      <label>SCL pin</label><input type='text' size='32' id='i2c_scl'>\n"
        "      <label>SDA pin</label><input type='text' size='32' id='i2c_sda'>\n"
        "    </div>\n"
        "  </div>\n";
    static const char i2c_config_footer[] =
        "  <h2>mqtt msg settings</h2>\n"
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"        
        "      <input type=\"checkbox\" id=\"i2c_mqtt_msg_stat\" ><label for=\"i2c_mqtt_msg_stat\">send i2c status via mqtt status</label>\n"
        "    </div>\n"
        "  </div>\n"
        "</div>\n"
        "<br>\n"
        "<br>\n"
        "<button type='button' onclick='SendSetting(\"i2c_frequency\");SendSetting(\"i2c_scl\");SendSetting(\"i2c_sda\");SendCheckboxSetting(\"i2c_mqtt_msg_stat\");SaveSettings();sendCMD(\"i2c_reinit\");get_settings();' class='button'>save</button>";
