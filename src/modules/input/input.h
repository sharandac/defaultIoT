/**
 * @file input.h
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
     * @brief input states
     */
    enum {
        INPUT_NONE = 0,
    };
    /**
     * @brief inject a input event
     * 
     * @param key 
     */
    void input_inject( int key );
    /**
     * @brief get the last pressed input
     * 
     * @return int 
     */
    int input_get( void );
    /**
     * @brief input config page
     */
    static const char input_config_page_head[] =
        "<h2>pin settings</h2>\n"
        "<script>set_settings_namespace(\"get_input_settings\");</script>\n"
        "<script>set_status_namespace(\"get_input_status\");</script>\n"
        "<script>set_save_namespace(\"save_input_settings\");</script>\n"
        "<div id=\"input_settings\" class=\"hbox\">\n";
    static const char input_config_page_footer[] =
        "  <h2>mqtt msg settings</h2>\n"
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"        
        "      <input type=\"checkbox\" id=\"input_mqtt_msg_on_change\" ><label for=\"input_mqtt_msg_on_change\">send input status on change via mqtt status</label>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"        
        "      <input type=\"checkbox\" id=\"input_mqtt_msg_stat\" ><label for=\"input_mqtt_msg_stat\">send input status via mqtt status</label>\n"
        "    </div>\n"
        "  </div>\n"
        "  <h2>hardware monitor</h2>\n"
        "  <div class=\"vbox\">\n"
        "    <label>input_state</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='input_state' disabled>\n"
        "    </div>\n"
        "  </div>\n"
        "</div>\n"
        "<br>\n"
        "<br>\n"
        "<button type='button' onclick='";
    static const char input_config_page_sendBTN[] =
        "SendCheckboxSetting(\"input_mqtt_msg_on_change\");SendCheckboxSetting(\"input_mqtt_msg_stat\");SaveSettings();get_settings();' class='button'>save</button>";
