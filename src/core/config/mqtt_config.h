/**
 * @file mqtt_config.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _MQTT_CONFIG_H
    #define _MQTT_CONFIG_H

    #include "core/utils/basejsonconfig.h"
    
    #define     MQTT_JSON_CONFIG_FILE     "/mqtt.json"    /** @brief defines json config file name */
    /**
     * @brief 
     */
    #define     MQTT_MAX_TEXT_SIZE   64
    /**
     * @brief ioport config structure
     */
    class mqtt_config_t : public BaseJsonConfig {
        public:
            mqtt_config_t();
            char server[ MQTT_MAX_TEXT_SIZE ] = "";
            int port = 1883;
            char username[ MQTT_MAX_TEXT_SIZE ] = "";
            char password[ MQTT_MAX_TEXT_SIZE ] = "";
            char topic[ MQTT_MAX_TEXT_SIZE ] = "";
            int interval = 15;
            bool realtimestats = true;
            bool sendstats = true;

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 2048; }
    };
#endif // _MQTT_CONFIG_H
