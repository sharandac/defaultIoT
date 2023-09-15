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
    #include <core/utils/callback.h>
    /**
     * @brief watchdog timeout in seconds
     */
    #define WDT_TIMEOUT 60
    /**
     * @brief webserver callback event mask
     */
    #define CORE_ENTER_CRITICAL          _BV(0)      /** @brief event mask for powermgm wakeup */
    #define CORE_EXIT_CRITICAL           _BV(1)      /** @brief event mask for powermgm wakeup */
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
     * @brief autocall function for module setup
     * 
     * @param function          pointer to the module registration function
     * @param prio              priority of the module
     */
    int core_autocall_function( CORE_AUTOCALL_FUNC function, size_t prio );
    /**
     * @brief register an externel callbackfunction
     *
     * @param event             event mask
     * @param callback_func     pointer to a callback function
     * @param id                id for the callback
     * @param active            callback function is activated
     * @param prio              order to call cb functions, see utils/callback.h
     * @return true             if success
     * @return false            if failed
     */
    bool core_register_callback_with_prio( EventBits_t event, CALLBACK_FUNC callback_func, const char *id, bool active, callback_prio_t prio );
    /**
     * @brief set the callback active state
     * 
     * @param callback_func     pointer to the callback function
     * @param active            callback function is activated
     * @return true             if success
     * @return false            if failed
     */
    bool core_set_callback_active( CALLBACK_FUNC callback_func, bool active );
    /**
     * @brief register an enter critical callback function
     * 
     * @param callback_func     pointer to the callback function
     * @param id                id for the callback
     * @return true             if success
     * @return false            if failed
     */
    bool core_register_enter_critical_function( CALLBACK_FUNC callback_func, const char *id );
    /**
     * @brief register an exit critical callback function
     * 
     * @param callback_func     pointer to the callback function
     * @param id                id for the callback
     * @return true             if success
     * @return false            if failed
     */
    bool core_register_exit_critical_function( CALLBACK_FUNC callback_func, const char *id );
    /**
     * @brief call all enter critical callback functions
     * 
     * @return true             if success
     * @return false            if failed
     */
    bool core_enter_critical( void );
    /**
     * @brief call all exit critical callback functions
     * 
     * @return true             if success
     * @return false            if failed
     */
    bool core_exit_critical( void );
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
        "    <label>cpu frequency</label><br>\n"
        "    <div class=\"box\">\n"        
        "      <select id='core_frequency'><option value='240'>240Mhz</option><option value='160'>160Mhz</option><option value='80'>80Mhz</option></select>"
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
        "  <h2>restart ESP32</h2>\n"
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"        
        "       <button type='button' onclick='sendCMD(\"core_reset\");' class='button'>reset</button>"
        "    </div>\n"
        "  </div>\n"
        "</div>\n"
        "<br>\n"
        "<br>\n"
        "<button type='button' onclick='SendSetting(\"core_frequency\");SendCheckboxSetting(\"core_brownout_detection\");SendCheckboxSetting(\"core_watchdog\");SendCheckboxSetting(\"core_frequency_scaling\");SendCheckboxSetting(\"core_light_sleep\");SaveSettings();get_settings();' class='button'>save</button>";
