/**
 * @file webserver.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

    #include <core/utils/callback.h>
    #include <AsyncTCP.h>
    #include <ESPAsyncWebServer.h>
    /**
     * @brief webserver callback event mask
     */
    #define WS_DATA                 _BV(0)      /** @brief event mask for powermgm standby */
    #define WEB_DATA                _BV(1)      /** @brief event mask for powermgm wakeup */  
    #define WEB_MENU                _BV(2)      /** @brief event mask for powermgm wakeup */
    #define SAVE_CONFIG             _BV(3)      /** @brief event mask for powermgm wakeup */
    #define RESET_CONFIG            _BV(4)      /** @brief event mask for powermgm wakeup */
    /**
     * @brief webserver callback data structure
     */
    typedef struct {
        AsyncWebSocket * server = NULL;         /** @brief pointer to the server */
        AsyncWebSocketClient * client = NULL;   /** @brief pointer to the client */
        AwsEventType type;                      /** @brief type of the event */
        AsyncWebServerRequest *request = NULL;  /** @brief pointer to the request */
        void * arg = NULL;                      /** @brief pointer to the argument */
        uint8_t *data = NULL;                   /** @brief pointer to the data */
        size_t len;                             /** @brief length of the data */
        const char *cmd = "";                   /** @brief pointer to the command */
        const char *value = "";                 /** @brief pointer to the argument */
    } wsData_t;
    /**
     * @brief webserver default port
     */
    #define WEBSERVERPORT       80
    /**
     * @brief register a callback function for a event
     * 
     * @param event             event mask
     * @param callback_func     pointer to the callback function
     * @param id                id of the callback function
     * @return true 
     * @return false 
     */
    bool asyncwebserver_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id );
    /**
     * @brief set an callback function active or inactive
     * 
     * @param callback_func     pointer to the callback function
     * @param active            true or false
     * @return true             on success
     * @return false            on fail
     */
    bool asyncwebserver_set_cb_active( CALLBACK_FUNC callback_func, bool active );
    /**
     * @brief register a callback function for a event with a priority
     * 
     * @param event             event mask
     * @param callback_func     pointer to the callback function
     * @param id                id of the callback function
     * @param prio              priority of the callback function
     * @return true             on success
     * @return false            on fail
     */
    bool asyncwebserver_register_cb_with_prio( EventBits_t event, CALLBACK_FUNC callback_func, const char *id, callback_prio_t prio );
    /**
     * @brief set a menu entry
     * 
     * @param filename      filename of the menu entry
     * @param entry         menu entry
     */
    void asyncwebserver_set_menu_entry( const char *filename, const char *entry );
    /**
     * @brief send a websocket message to all connected clients
     * 
     * @param msg       message to send
     */
    void asyncwebserver_send_websocket_msg( const char * fmt, ... );
    /**
     * @brief get the menu html page
     * 
     * @return String 
     */
    String asyncwebserver_get_menu( void );
    /**
     * @brief html page header
     */
    static const char html_header[] = 
        "<html>\r\n"
        "<head>\r\n"
        "<meta charset='utf-8'>\r\n"
        "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\r\n"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n"
        "<link rel=\"stylesheet\" href=\"styles.css\">\r\n"
        "<script src=\"jquery-latest.min.js\" type=\"text/javascript\"></script>\r\n"
        "<script src=\"websocket.js\" type=\"text/javascript\"></script>\r\n"
        "<script src=\"menu.js\"></script>\r\n"
        "</head>\r\n";
    /**
     * @brief html page footer
     */
    static const char html_footer[] = 
        "<br><br><br><br><br>\r\n"
        "<div id='fixedfooter'><span id='status'>offline</span></div>\r\n"
        "</body>\r\n"
        "</html>\r\n";
    /**
     * @brief html page for update by browser
     */
    static const char html_update[] =
        "<!DOCTYPE html>\n <html><head>\n <script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
        "\n <script src='/jquery.min.js'></script>"
        "\n <style>"
        "\n #progressbarfull {"
        "\n background-color: #20201F;"
        "\n border-radius: 20px;"
        "\n width: 320px;"
        "\n padding: 4px;"
        "\n}"
        "\n #progressbar {"
        "\n background-color: #20CC00;"
        "\n width: 3%;"
        "\n height: 16px;"
        "\n border-radius: 10px;"
        "\n}"
        "\n</style>"
        "\n </head><body>"
        "<h2>Update by Browser</h2>"
        "\n <form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
        "\n <input type='file' name='update'>"
        "\n <br><br><input type='submit' value='Update'>"
        "\n </form>"
        "\n <div id='prg'>Progress: 0%</div>"
        "\n <div id=\"progressbarfull\"><div id=\"progressbar\"></div></div>"
        "\n <script>"
        "\n $('form').submit(function(e){"
        "\n e.preventDefault();"
        "\n var form = $('#upload_form')[0];"
        "\n var data = new FormData(form);"
        "\n $.ajax({"
        "\n url: '/update',"
        "\n type: 'POST',"
        "\n data: data,"
        "\n contentType: false,"
        "\n processData:false,"
        "\n xhr: function() {"
        "\n var xhr = new window.XMLHttpRequest();"
        "\n xhr.upload.addEventListener('progress', function(evt) {"
        "\n if (evt.lengthComputable) {"
        "\n var per = evt.loaded / evt.total;"
        "\n document.getElementById(\"prg\").innerHTML = 'Progress: ' + Math.round(per*100) + '%';"
        "\n document.getElementById(\"progressbar\").style.width=Math.round(per*100)+ '%';"
        "}"
        "}, false);"
        "return xhr;"
        "},"
        "\n success:function(d, s) {"
        "\n document.getElementById(\"prg\").innerHTML = 'Progress: success';"
        "\n console.log('success!')"
        "},"
        "\n error: function (a, b, c) {"
        "\n document.getElementById(\"prg\").innerHTML = 'Progress: error';"
        "}"
        "});"
        "});"
        "\n </script>"
        "\n </body></html>";
