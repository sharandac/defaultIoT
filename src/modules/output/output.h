/**
 * @file output.h
 * @author Dirk Broßwick ( dirk.brosswick@uni-jena.de )
 * @brief 
 * @version 0.1
 * @date 2022-01-18
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once
    /**
     * @brief output config page
     */
    static const char output_config_page_head[] =
        "<h2>pin settings</h2>\n"
        "<script>set_settings_namespace(\"get_output_settings\");</script>\n"
        "<script>set_status_namespace(\"get_output_status\");</script>\n"
        "<script>set_save_namespace(\"save_output_settings\");</script>\n"
        "<div id=\"output_settings\" class=\"hbox\">\n";
    static const char output_config_page_footer[] =
        "  <h2>mqtt msg settings</h2>\n"
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"        
        "      <input type=\"checkbox\" id=\"output_mqtt_msg_on_change\" ><label for=\"output_mqtt_msg_on_change\">send output status on change via mqtt status</label>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"        
        "      <input type=\"checkbox\" id=\"output_mqtt_msg_stat\" ><label for=\"output_mqtt_msg_stat\">send output status via mqtt status</label>\n"
        "    </div>\n"
        "  </div>\n"
        "  <h2>hardware monitor</h2>\n"
        "  <div class=\"vbox\">\n"
        "    <label>output_state</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='output_state' disabled>\n"
        "    </div>\n"
        "  </div>\n"
        "</div>\n"
        "<br>\n"
        "<br>\n"
        "<button type='button' onclick='";
    static const char output_config_page_sendBTN[] =
        "SendCheckboxSetting(\"output_mqtt_msg_on_change\");SendCheckboxSetting(\"output_mqtt_msg_stat\");SaveSettings();get_settings();' class='button'>save</button>";
