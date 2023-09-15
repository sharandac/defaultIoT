/**
 * @file wificlient.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

    #include "config.h"
    #define WLAN_CONNECT_TIMEOUT    15      /* connection timeout */
    /**
     * @brief get the hostname
     * 
     * @return const char*
     */
    const char *wificlient_get_hostname( void );
    /**
     * @brief unique hostname
     * 
     * @return const char*
     */
    const char *wificlient_get_hostname_unique( void );
    /**
     * @brief wificlient config page
     */
    static const char wificlient_config_page[] =
        "<h1>Wlan settings</h1>\n"
        "<script>set_settings_namespace(\"get_wlan_settings\");</script>\n"
        "<script>set_save_namespace(\"save_wlan_settings\");</script>\n"
        "<div id=\"hostname_settings\" class=\"hbox\">\n"
        "  <div class=\"vbox\">\n"
        "    <label>unique hostname</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='unique_hostname' disabled>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>hostname</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='hostname'>\n"
        "    </div>\n"
        "  </div>\n"
        "</div>\n"
        "<div id=\"wlan_settings\" class=\"hbox\">\n"
        "  <div class=\"vbox\">\n"
        "    <label>Wlan SSID</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='wifi_ssid'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>Wlan Password</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='wifi_password'>\n"
        "    </div>\n"
    #if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 0, 0)
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <input type=\"checkbox\" id=\"wifi_low_power\" ><label for=\"wifi_low_power\"> enable low power radio</label>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <input type=\"checkbox\" id=\"wifi_low_bandwidth\" ><label for=\"wifi_low_bandwidth\"> enable low bandwidth radio</label>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <input type=\"checkbox\" id=\"wifi_enable_softap\" ><label for=\"wifi_enable_softap\"> enable softAP fallback</label>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>SoftAP SSID</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='wifi_softap_ssid'>\n"
        "    </div>\n"
        "  </div>\n"
    #endif
        "  <div class=\"vbox\">\n"
        "    <label>SoftAP Password</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='wifi_softap_password'>\n"
        "    </div>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <input type=\"checkbox\" id=\"wifi_enable_led\" ><label for=\"wifi_enable_led\"> enable wifi led indicator</label>\n"
        "  </div>\n"
        "  <div class=\"vbox\">\n"
        "    <label>led pin</label><br>\n"
        "    <div class=\"box\">\n"
        "      <input type='text' size='32' id='wifi_led_pin'>\n"
        "    </div>\n"
        "  </div>\n"
        "</div>\n"
        "<br>\n"
        "<br>\n"
        "<button type='button' onclick='SendSetting(\"hostname\");SendSetting(\"wifi_ssid\");SendSetting(\"wifi_password\");SendSetting(\"wifi_led_pin\");SendCheckboxSetting(\"wifi_enable_softap\");SendCheckboxSetting(\"wifi_enable_led\");SendCheckboxSetting(\"wifi_low_power\");SendCheckboxSetting(\"wifi_low_bandwidth\");SendSetting(\"wifi_softap_ssid\");SendSetting(\"wifi_softap_password\");SaveSettings();get_settings();' class='button'>save</button>";
    /**
     * @brief wificlient update page
     */
    static const char wificlient_update_page[] =
        "<br>\n"
        "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>\n"
        "  <input type='file' name='update'><br><br>\n"
        "  <input id=\"button\" type='submit' value='Update' class='button'>\n"
        "</form>\n"
        "<script>\n"
        "    $('form').submit(function(e){\n"
        "        e.preventDefault();\n"
        "        var form = $('#upload_form')[0];\n"
        "        var data = new FormData(form);\n"
        "        $.ajax({\n"
        "        url: '/update',\n"
        "        type: 'POST',\n"
        "        data: data,\n"
        "        contentType: false,\n"
        "        processData:false,\n"
        "        xhr: function() {\n"
        "            var xhr = new window.XMLHttpRequest();\n"
        "            xhr.upload.addEventListener('progress', function(evt) {\n"
        "            if (evt.lengthComputable) {\n"
        "                var per = evt.loaded / evt.total;\n"
        "                document.getElementById(\"button\").value = 'Update Progress: ' + Math.round(per*100) + '%';\n"
        "            }\n"
        "            }, false);\n"
        "            return xhr;\n"
        "        },\n"
        "        success:function(d, s) {\n"
        "          document.getElementById(\"button\").innerHTML = 'Progress: success';\n"
        "        },\n"
        "        error: function (a, b, c) {\n"
        "            document.getElementById(\"button\").value = 'Progress: error';\n"
        "        }\n"
        "        });\n"
        "    });\n"
        "</script>";
    /**
     * @brief wificlient info page
     */
    static const char wificlient_info_config_page[] =
        "<br><center>firmware: " __FIRMWARE__ ", " __DATE__ " " __TIME__ ", GCC-Version: " __VERSION__ "</center>\n"
        "<br><center>&copy; 2023 / Dirk Bro&szlig;wick / Email: dirk.brosswick@googlemail.com</center>";
