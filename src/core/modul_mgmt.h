/**
 * @file modul_mgmt.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

    #include "core/utils/callback.h"
    typedef void ( * MODULE_AUTOCALL_FUNC ) ( void );
    /**
     * @brief callback table entry structure
     */
    typedef struct {
        MODULE_AUTOCALL_FUNC function;          /** @brief pointer to a callback function */
    } module_autocall_table_t;
    /**
     * @brief autocall function for setup all modules
     */
    int module_autocall_function( MODULE_AUTOCALL_FUNC function );
    /**
     * @brief module management event mask
     */
    #define MODULE_MGMT_INIT        _BV(0)      /** @brief module init event mask */
    #define MODULE_MGMT_DEINIT      _BV(1)      /** @brief module deinit event mask */
    #define MODULE_MGMT_LOOP        _BV(2)      /** @brief module loop event mask */
    /**
     * @brief call all modules setup function
     */
    void module_mgmt_call_setup( void );
    /**
     * @brief call all modules init function
     */
    void module_mgmt_call_init( void );
    /**
     * @brief call all modules deinit function
     */
    void module_mgmt_call_deinit( void );
    /**
     * @brief call all modules loop function
     */
    void module_mgmt_call_loop( void );
    /**
     * @brief register a module
     * 
     * @param init_func         init function
     * @param deinit_func       deinit function
     * @param loop_func         loop function
     * @param id                id of the module
     * @return true             if success
     * @return false            if failed
     */
    bool module_mgmt_register( CALLBACK_FUNC init_func, CALLBACK_FUNC deinit_func, CALLBACK_FUNC loop_func, const char *id );
    /**
     * @brief input config page
     */
    static const char module_mgmt_config_page_head[] =
        "<h2>modul settings</h2>\n"
        "<script>set_settings_namespace(\"get_module_settings\");</script>\n"
        "<script>set_save_namespace(\"save_module_settings\");</script>\n"
        "<script>set_status_namespace(\"get_module_status\");</script>\n"
        "<div id=\"modul_settings\" class=\"hbox\">\n";
    static const char module_mgmt_config_page_footer[] =
        "</div>\n"
        "<br>\n"
        "<br>\n"
        "<button type='button' onclick='";
    static const char module_mgmt_config_page_sendBTN[] =
        "SaveSettings();get_settings();location.reload();' class='button'>save</button>";
