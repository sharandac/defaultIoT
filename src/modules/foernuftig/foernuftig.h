/**
 * @file foernuftig.h
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
     * @brief button states
     */
    enum {
        BUTTON_NONE = 0,
        BUTTON_OFF = 1,
        BUTTON_I = 2,
        BUTTON_II = 4,
        BUTTON_III = 8,
    };
    /**
     * @brief foenuftig config page
     */
    static const char foernuftig_config_page[] =
        "<h2>foernuftig hardware settings</h2>\n"
        "<script>set_settings_namespace(\"get_foernuftig_settings\");</script>"
        "<script>set_status_namespace(\"get_foernuftig_status\");</script>\n"
        "<script>set_save_namespace(\"save_foernuftig_settings\");</script>\n"
        "<div id=\"foernuftig_settings\" class=\"hbox\">\n"
        "  <div class=\"vbox\">\n"
        "    <label>button OFF pin</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='foernuftig_button_0_pin'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>button I pin</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='foernuftig_button_1_pin'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>button II pin</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='foernuftig_button_2_pin'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>button III pin</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='foernuftig_button_3_pin'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>power supply pin</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='foernuftig_supply_pin'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>clk pin</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='foernuftig_clk_pin'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>fg pin</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='foernuftig_fg_pin'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>led pin</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='foernuftig_led_pin'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>USB-Power pin ( optinal )</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='foernuftig_usb_power_pin'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>rpm factor</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='foernuftig_rpm_factor'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <h2>foernuftig fan limits</h2>\n"
        "  <div class=\"vbox\">\n"
        "    <label>min rpm</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='foernuftig_min_rpm'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>mid rpm</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='foernuftig_mid_rpm'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>max rpm</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='foernuftig_max_rpm'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <h2>foernuftig hardware monitor</h2>\n"
        "  <div class=\"vbox\">\n"
        "    <label>set fan level</label><br>\n"
        "    <div class=\"box\">\n"
        "      <button type='button' onclick='sendCMD(\"foernuftig_set_level\\\\0\");' class=\"smallbutton\">OFF</button>"
        "      <button type='button' onclick='sendCMD(\"foernuftig_set_level\\\\1\");' class=\"smallbutton\">I</button>"
        "      <button type='button' onclick='sendCMD(\"foernuftig_set_level\\\\2\");' class=\"smallbutton\">II</button>"
        "      <button type='button' onclick='sendCMD(\"foernuftig_set_level\\\\3\");' class=\"smallbutton\">III</button>"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>set usb power</label><br>\n"
        "    <div class=\"box\">\n"
        "      <button type='button' onclick='sendCMD(\"foernuftig_usb_power\\\\0\");' class=\"midbutton\">OFF</button>"
        "      <button type='button' onclick='sendCMD(\"foernuftig_usb_power\\\\1\");' class=\"midbutton\">ON</button>"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>current rpm</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='foernuftig_current_rpm' disabled>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>usb power state</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='foernuftig_usb_power_state' disabled>\n"
        "    </div>\n"
        "  </div>\n"
        "</div>\n"
        "<br>\n"
        "<br>\n"
        "<button type='button' onclick='SendSetting(\"foernuftig_button_0_pin\");SendSetting(\"foernuftig_button_1_pin\");SendSetting(\"foernuftig_button_2_pin\");SendSetting(\"foernuftig_button_3_pin\");SendSetting(\"foernuftig_led_pin\");SendSetting(\"foernuftig_fg_pin\");SendSetting(\"foernuftig_clk_pin\");SendSetting(\"foernuftig_supply_pin\");SendSetting(\"foernuftig_usb_power_pin\");SendSetting(\"foernuftig_rpm_factor\");SendSetting(\"foernuftig_min_rpm\");SendSetting(\"foernuftig_mid_rpm\");SendSetting(\"foernuftig_max_rpm\");SaveSettings();get_settings();' class='button'>save</button>";
