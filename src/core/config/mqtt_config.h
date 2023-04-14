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
    /**
     * @brief class settings like namespace, filename and max length for strings
     */
    #define     CLASS_NAME_T            mqtt_config_t       /** @brief defines namespace */
    #define     JSON_CONFIG_FILE        "/mqtt.json"        /** @brief defines json config file name */
    #define     MAX_LENGTH              64                  /** @brief defines max length for strings */
    /**
     * @brief ioport config structure
     */
    class CLASS_NAME_T : public BaseJsonConfig {
        public:
            CLASS_NAME_T();
            char server[ MAX_LENGTH ] = "";                 /** @brief defines server */
            int port = 1883;                                /** @brief defines port */
            char username[ MAX_LENGTH ] = "";               /** @brief defines username */
            char password[ MAX_LENGTH ] = "";               /** @brief defines password */
            char topic[ MAX_LENGTH ] = "";                  /** @brief defines topic */
            int interval = 15;                              /** @brief defines status interval */
            bool sendstats = true;                          /** @brief defines if stats are send */

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 2048; }
    };
#endif // _MQTT_CONFIG_H
