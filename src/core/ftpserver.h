/**
 * @file ftpserver.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

    /**
     * @brief wificlient config page
     */
    static const char ftpserver_config_page[] =
        "<h1>ftp settings</h1>\n"
        "<script>set_settings_namespace(\"get_ftp_settings\");</script>\n"
        "<script>set_save_namespace(\"save_ftp_settings\");</script>\n"
        "<div id=\"ftp_settings\" class=\"hbox\">\n"
        "  <div class=\"vbox\">\n"
        "    <div class=\"box\">\n"
        "      <input type=\"checkbox\" id=\"ftp_enable\" ><label for=\"ftp_enable\"> enable ftp server</label>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>username</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='ftp_username'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>password</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='ftp_password'>\n"
        "    </div>\n"
        "  </div>\n"
        "</div>\n"
        "<br>\n"
        "<br>\n"
        "<button type='button' onclick='SendCheckboxSetting(\"ftp_enable\");SendSetting(\"ftp_username\");SendSetting(\"ftp_password\");SaveSettings();get_settings();' class='button'>save</button>";
