/**
 * @file output_config.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.2
 * @date 2023-03-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

    #include <Arduino.h>
    #include "core/utils/basejsonconfig.h"

    #define     OUTPUT_JSON_CONFIG_FILE    "/output.json"   /** @brief defines json config file name */
    #define     MAX_OUTPUTS                4                /** @brief defines max inputs */
    /**
     * @brief input config structure
     */
    class output_config_t : public BaseJsonConfig {
        private:
            /**
             * @brief output pin config structure
             */
            typedef struct {
                bool enaled = true;                     // enable input    
                int pin;                                // pin number
                bool state = false;                     // state of output
            } output_pin_cfg_t;

        public:
            output_config_t();                          // constructor
            size_t output_count = 0;                       // number of outputs
            output_pin_cfg_t device[ MAX_OUTPUTS ];     // output config
            bool mqtt_msg_on_change = false;            // send mqtt message on change
            bool mqtt_msg_stat = false;                 // send mqtt status message

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 4096; }
    };

