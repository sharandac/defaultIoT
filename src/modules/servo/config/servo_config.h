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

    #define     SERVO_JSON_CONFIG_FILE    "/servo.json" /** @brief defines json config file name */
    #define     MAX_SERVOS                4             /** @brief defines max inputs */
    /**
     * @brief input config structure
     */
    class servo_config_t : public BaseJsonConfig {
        private:
            /**
             * @brief servo pin config structure
             */
            typedef struct {
                bool enaled = true;                     // enable input      
                int pin;                                // pin number
                size_t current_angle = 0;               // current angle
                size_t destination_angle = 0;           // destination angle
                size_t current_value = 90;              // current value
                size_t min_angle = 0;                   // min angle
                size_t neutal_angle = 90;               // neutral angle
                size_t max_angle = 180;                 // max angle
                size_t min_value = 0;                   // min value
                size_t max_value = 180;                 // max value
                bool state = false;                     // state
                bool neutral_on_start = false;          // neutral on start
                bool neutral_after_move = false;        // neutral after movement
                size_t detach_time = 1000;              // detach time
                uint64_t millis = 0;                    // millis
            } servo_pin_cfg_t;

        public:
            servo_config_t();                           // constructor
            size_t count = 0;                           // number of servos
            servo_pin_cfg_t device[ MAX_SERVOS ];        // servo config
            bool mqtt_msg_on_change = false;            // send mqtt message on change
            bool mqtt_msg_stat = false;                 // send mqtt status message

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 4096; }
    };

