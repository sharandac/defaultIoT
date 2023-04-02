/**
 * @file wifi_config.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _WIFICLIENT_CONFIG_H
    #define _WIFICLIENT_CONFIG_H

    #include "core/utils/basejsonconfig.h"
    #include "config.h"
    
    #define     WIFICLIENT_JSON_CONFIG_FILE     "/wifi.json"    /** @brief defines json config file name */
    /**
     * @brief 
     */
    #define     WIFICLIENT_MAX_TEXT_SIZE   64
    /**
     * @brief ioport config structure
     */
    class wificlient_config_t : public BaseJsonConfig {
        public:
            wificlient_config_t();
            char hostname_unique[ WIFICLIENT_MAX_TEXT_SIZE ] = DEVICE_NAME;
            char hostname[ WIFICLIENT_MAX_TEXT_SIZE ] = DEVICE_NAME;
            char ssid[ WIFICLIENT_MAX_TEXT_SIZE ] = WIFI_SSID;
            char password[ WIFICLIENT_MAX_TEXT_SIZE ] = WIFI_PASSWORD;
            bool enable_softap = true;
            char softap_ssid[ WIFICLIENT_MAX_TEXT_SIZE ] = DEVICE_NAME;
            char softap_password[ WIFICLIENT_MAX_TEXT_SIZE ] = DEVICE_NAME;
            int timeout = 15;
            bool low_bandwidth = false;
            bool low_power = false;
            bool enable_led = false;
            int led_pin = -1;

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 2048; }
    };
#endif // _WIFICLIENT_CONFIG_H
