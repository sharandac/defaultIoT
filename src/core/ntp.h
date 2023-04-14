/**
 * @file ntp.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

    #include <core/utils/callback.h>
    /**
     * @brief ntp config page
     */
    static const char ntp_config_page[] =
        "<h1>ntp settings</h1>\n"
        "<script>set_settings_namespace(\"get_ntp_settings\");</script>\n"
        "<script>set_save_namespace(\"save_ntp_settings\");</script>\n"
        "<div id=\"ntp_settings\" class=\"hbox\">\n"
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"
        "      <input type=\"checkbox\" id=\"ntp_enable\"><label for=\"enable_ntp\"> enable ntp</label>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>ntp server</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='ntp_server'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>ntp timezone ( <a href=\"https://raw.githubusercontent.com/nayarsystems/posix_tz_db/master/zones.json\" target=\"_blank\" rel=\"noopener noreferrer\">need help?</a> ) </label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='ntp_timezone'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>ntp update interval</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='ntp_interval'>\n"
        "    </div>\n"
        "  </div>\n"
        "</div>\n"
        "<br>\n"
        "<br>\n"
        "<button type='button' onclick='SendSetting(\"ntp_server\");SendSetting(\"ntp_timezone\");SendSetting(\"ntp_interval\");SendCheckboxSetting(\"ntp_enable\");SaveSettings();get_settings();' class='button'>save</button>";

