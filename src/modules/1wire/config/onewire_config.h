/**
 * @file onewire_config.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

    #include <Arduino.h>
    #include "core/utils/basejsonconfig.h"
    /**
     * @brief class settings like namespace, filename and max length for strings
     */
    #define     CLASS_NAME_T            onewire_config_t        /** @brief defines namespace */
    #define     JSON_CONFIG_FILE        "/onewire.json"         /** @brief defines json config file name */
    #define     MAX_LENGTH              64                      /** @brief defines max length for strings */
    #define     MAX_SENSORS             16                      /** @brief defines max inputs */
    /**
     * @brief input config structure
     */
    class CLASS_NAME_T : public BaseJsonConfig {
        public:
            CLASS_NAME_T();                                     
            bool enabled = false;                               /** @brief defines if the module enabled */
            int pin = -1;                                       /** @brief defines the pin for the 1wire bus */
            int interval = 15;                                  /** @brief defines the internal interval for the 1wire bus */
            bool reportInFahrenheit = false;                    /** @brief defines if the temperature is reported in Fahrenheit */
            bool mqtt_msg_stat = false;                         /** @brief defines if the mqtt message is send with stat */

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 4096; }
    };

