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

    #define     ONEWIRE_JSON_CONFIG_FILE    "/onewire.json" /** @brief defines json config file name */
    #define     MAX_SENSORS                 16              /** @brief defines max inputs */
    /**
     * @brief input config structure
     */
    class onewire_config_t : public BaseJsonConfig {
        public:
            onewire_config_t();                     // constructor
            bool enabled = false;                   // enable input
            int pin = -1;                           // pin number
            int interval = 15;                      // interval in s
            bool reportInFahrenheit = false;        // report in fahrenheit
            bool mqtt_msg_on_change = false;        // send mqtt message on change
            bool mqtt_msg_stat = false;             // send mqtt status message

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 4096; }
    };

