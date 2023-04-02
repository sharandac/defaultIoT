/**
 * @file servo_config.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

    #include <Arduino.h>
    #include "core/utils/basejsonconfig.h"

    #define     ANALOG_JSON_CONFIG_FILE     "/analog.json"  /** @brief defines json config file name */
    #define     MAX_ANALOG                  3               /** @brief defines max analog inputs */
    /**
     * @brief input config structure
     */
    class analog_config_t : public BaseJsonConfig {
        private:
            /**
             * @brief servo pin config structure
             */
            typedef struct {
                bool enaled = false;                    // enable input      
                int pin = -1;                           // pin number
                int value = 0;                          // value
                int trigger_low = 0;                    // min value
                int trigger_high = 4095;                // max value
                int analog_min = 4095;                  // min value
                int analog_max = 0;                     // max value
                bool analog_trigger = false;            // trigger on change
                int trigger_count = 0;                  // trigger count
                bool trigger_count_reset = false;       // trigger on change
            } analog_pin_cfg_t;

        public:
            analog_config_t();                          // constructor
            size_t count = 0;                           // number of servos
            analog_pin_cfg_t device[ MAX_ANALOG ];      // servo config
            bool mqtt_msg_on_change = false;            // send mqtt message on change
            bool mqtt_msg_stat = false;                 // send mqtt status message

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 4096; }
    };

