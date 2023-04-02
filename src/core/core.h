/**
 * @file core.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
    /**
     * @brief watchdog timeout in seconds
     */
    #define WDT_TIMEOUT 60
    /**
     * @brief core autocall type definition
     */
    typedef void ( * CORE_AUTOCALL_FUNC ) ( void );
    /**
     * @brief callback table entry structure
     */
    typedef struct {
        CORE_AUTOCALL_FUNC function;          /** @brief pointer to a callback function */
        size_t prio;                            /** @brief priority of the callback function */
    } core_autocall_table_t;
    /**
     * @brief autocall function for setup all modules
     */
    int core_autocall_function( MODULE_AUTOCALL_FUNC function, size_t prio );
        /**
     * @brief core setup
     */
    void core_setup( void );
    /**
     * @brief reset the watchdog timer
     */
    void core_reset_wdt( void );
    /**
     * @brief output config page
     */
    static const char core_config_page[] =
        "<script>set_settings_namespace(\"get_core_settings\");</script>\n"
        "<script>set_status_namespace(\"get_core_status\");</script>\n"
        "<script>set_save_namespace(\"save_core_settings\");</script>\n"
        "<div id=\"core_settings\" class=\"hbox\">\n"
        "  <h2>cpu settings</h2>\n"
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"        
        "      <label>cpu frequency</label><select id='core_frequency'><option value='240'>240Mhz</option><option value='160'>160Mhz</option><option value='80'>80Mhz</option></select>"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"        
        "      <input type=\"checkbox\" id=\"core_brownout_detection\" ><label for=\"core_brownout_detection\">enable brown out detection</label>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"        
        "      <input type=\"checkbox\" id=\"core_watchdog\" ><label for=\"core_watchdog\">enable watchdog for main loop</label>\n"
        "    </div>\n"
        "  </div>\n"
    #if CONFIG_PM_ENABLE
        "  <h2>power savings</h2>\n"
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"        
        "      <input type=\"checkbox\" id=\"core_frequency_scaling\" ><label for=\"core_frequency_scaling\">enable dynamic frequency scaling</label>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"        
        "      <input type=\"checkbox\" id=\"core_light_sleep\" ><label for=\"core_light_sleep\">enable light sleep</label>\n"
        "    </div>\n"
        "  </div>\n"
    #endif
        "</div>\n"
        "<br>\n"
        "<br>\n"
        "<button type='button' onclick='SendSetting(\"core_frequency\");SendCheckboxSetting(\"core_brownout_detection\");SendCheckboxSetting(\"core_watchdog\");SendCheckboxSetting(\"core_frequency_scaling\");SendCheckboxSetting(\"core_light_sleep\");SaveSettings();get_settings();' class='button'>save</button>";
