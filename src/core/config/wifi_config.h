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
    /**
     * @brief class settings like namespace, filename and max length for strings
     */
    #define     CLASS_NAME_T            wificlient_config_t     /** @brief defines namespace */
    #define     JSON_CONFIG_FILE        "/wifi.json"            /** @brief defines json config file name */
    #define     MAX_LENGTH              64                      /** @brief defines max length for strings */
    /**
     * @brief ioport config structure
     */
    class CLASS_NAME_T : public BaseJsonConfig {
        public:
            CLASS_NAME_T();
            char hostname_unique[ MAX_LENGTH ] = DEVICE_NAME;   /** @brief defines unique hostname */
            char hostname[ MAX_LENGTH ] = DEVICE_NAME;          /** @brief defines hostname */
            char ssid[ MAX_LENGTH ] = WIFI_SSID;                /** @brief defines ssid */
            char password[ MAX_LENGTH ] = WIFI_PASSWORD;        /** @brief defines password */
            bool enable_softap = true;                          /** @brief defines if softap is enabled */
            char softap_ssid[ MAX_LENGTH ] = DEVICE_NAME;       /** @brief defines softap ssid */
            char softap_password[ MAX_LENGTH ] = DEVICE_NAME;   /** @brief defines softap password */
            int timeout = 15;                                   /** @brief defines timeout */
            bool low_bandwidth = false;                         /** @brief defines if low bandwidth is enabled */
            bool low_power = false;                             /** @brief defines if low power is enabled */
            bool enable_led = false;                            /** @brief defines if led is enabled */
            int led_pin = -1;                                   /** @brief defines led pin for status information */

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 2048; }
    };
#endif // _WIFICLIENT_CONFIG_H
