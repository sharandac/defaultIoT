/**
 * @file irsta.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

    #define COLOR_ORDER GRB
    #define CHIPSET     WS2811
    #define NUM_LEDS    8*32
    /**
     * @brief button states
     */
    enum {
        BUTTON_NONE = 0,
        BUTTON_0 = 1,
        BUTTON_I = 2,
        BUTTON_II = 4,
        BUTTON_III = 8,
    };
    /**
     * @brief  irsta config page
     */
    static const char irsta_config_page[] =
        "<h2>irsta hardware settings</h2>\n"
        "<script>set_settings_namespace(\"get_irsta_settings\");</script>"
        "<script>set_status_namespace(\"get_irsta_status\");</script>\n"
        "<script>set_save_namespace(\"save_irsta_settings\");</script>\n"
        "<div id=\"irsta_settings\" class=\"hbox\">\n"
        "  <div class=\"vbox\">\n"
        "    <label>off pin</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='irsta_off_pin'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>I pin</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='irsta_i_pin'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>II pin</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='irsta_ii_pin'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>III pin</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='irsta_iii_pin'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>threshold</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='irsta_threshold'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>clk pin</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='irsta_clk_pin'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>max</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='irsta_max'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>red</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='irsta_red'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>green</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='irsta_green'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>blue</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='irsta_blue'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <h2>mqtt msg settings</h2>\n"
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"        
        "      <input type=\"checkbox\" id=\"irsta_mqtt_msg_stat\" ><label for=\"irsta_mqtt_msg_stat\">send input status via mqtt status</label>\n"
        "    </div>\n"
        "  </div>\n"
        "  <h2>irsta hardware monitor</h2>\n"
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"
        "      <button type='button' onclick='sendCMD(\"irsta_inject_button\\\\1\");get_settings();' class=\"smallbutton\">0</button>"
        "      <button type='button' onclick='sendCMD(\"irsta_inject_button\\\\2\");get_settings();' class=\"smallbutton\">I</button>"
        "      <button type='button' onclick='sendCMD(\"irsta_inject_button\\\\4\");get_settings();' class=\"smallbutton\">II</button>"
        "      <button type='button' onclick='sendCMD(\"irsta_inject_button\\\\8\");get_settings();' class=\"smallbutton\">III</button>"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>button_state</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='button_state' disabled>\n"
        "    </div>\n"
        "  </div>\n"
        "</div>\n"
        "<br>\n"
        "<br>\n"
        "<button type='button' onclick='SendSetting(\"irsta_mqtt_msg_stat\");SendSetting(\"irsta_off_pin\");SendSetting(\"irsta_i_pin\");SendSetting(\"irsta_ii_pin\");SendSetting(\"irsta_iii_pin\");SendSetting(\"irsta_threshold\");SendSetting(\"irsta_clk_pin\");SendSetting(\"irsta_max\");SendSetting(\"irsta_red\");SendSetting(\"irsta_green\");SendSetting(\"irsta_blue\");SaveSettings();get_settings();' class='button'>save</button>";
