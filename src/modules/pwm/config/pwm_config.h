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

    #define     PWM_JSON_CONFIG_FILE    "/pwm.json"     /** @brief defines json config file name */
    #define     MAX_PWM                 4               /** @brief defines max pwm */
    /**
     * @brief input config structure
     */
    class pwm_config_t : public BaseJsonConfig {
        private:
            /**
             * @brief output pin config structure
             */
            typedef struct {
                bool enaled = true;                     // enable input    
                int pin;                                // pin number
                int frequency = 8000;                   // pwm frequency
                int value = 0;                          // state of output
                int min = 0;                            // pwm min
                int max = 255;                          // pwm max
            } pwm_pin_cfg_t;

        public:
            pwm_config_t();                             // constructor
            size_t pwm_count = 0;                       // number of pwm
            pwm_pin_cfg_t device[ MAX_PWM ];               // pwm config
            bool mqtt_msg_on_change = false;            // send mqtt message on change
            bool mqtt_msg_stat = false;                 // send mqtt status message

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 4096; }
    };

