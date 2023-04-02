/**
 * @file input_config.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.3
 * @date 2023-03-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

    #include <Arduino.h>
    #include "core/utils/basejsonconfig.h"

    #define     INPUT_JSON_CONFIG_FILE    "/input.json" /** @brief defines json config file name */
    #define     MAX_INPUTS                4             /** @brief defines max inputs */
    /**
     * @brief input config structure
     */
    class input_config_t : public BaseJsonConfig {
        private:
            /**
             * @brief input pin config structure
             */
            typedef struct {
                bool enaled = true;                 // enable input    
                int pin;                            // pin number
                size_t pin_cfg = INPUT;                // pin config
                bool invert = false;                // invert input
            } input_pin_cfg_t;

        public:
            input_config_t();                       // constructor
            size_t input_count = 0;                    // number of inputs
            input_pin_cfg_t device[ MAX_INPUTS ];    // input config
            bool mqtt_msg_on_change = false;        // send mqtt message on change
            bool mqtt_msg_stat = false;                 // send mqtt status message

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 4096; }
    };

