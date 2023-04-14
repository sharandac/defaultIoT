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
    /**
     * @brief class settings like namespace, filename and max length for strings
     */
    #define     CLASS_NAME_T            servo_config_t  /** @brief defines namespace */
    #define     JSON_CONFIG_FILE        "/servo.json"   /** @brief defines json config file name */
    #define     MAX_LENGTH              32              /** @brief defines max length for strings */
    #define     MAX_SERVOS              4               /** @brief defines max inputs */
    /**
     * @brief input config structure
     */
    class CLASS_NAME_T : public BaseJsonConfig {
        private:
            /**
             * @brief servo pin config structure
             */
            typedef struct {
                bool enaled = true;                     /** @brief defines if the servo pin config is enabled */
                char id[ MAX_LENGTH ] = "";             /** @brief defines the id of the servo pin */
                int pin;                                /** @brief defines the servo pin */
                size_t current_angle = 0;               /** @brief defines the current angle of the servo */
                size_t destination_angle = 0;           /** @brief defines the destination angle of the servo */
                size_t current_value = 90;              /** @brief defines the current value of the servo */
                size_t min_angle = 0;                   /** @brief defines the min angle of the servo */
                size_t neutal_angle = 90;               /** @brief defines the neutral angle of the servo */
                size_t max_angle = 180;                 /** @brief defines the max angle of the servo */
                size_t min_value = 0;                   /** @brief defines the min value of the servo, scaled to min angle */
                size_t max_value = 180;                 /** @brief defines the max value of the servo, scaled to max angle */
                bool state = false;                     /** @brief defines the state of the servo pin */
                bool neutral_on_start = false;          /** @brief defines if the servo is neutral on start */
                bool neutral_after_move = false;        /** @brief defines if the servo is neutral after move */
                size_t detach_time = 1000;              /** @brief defines the detach time of the servo */
                uint64_t millis = 0;                    /** @brief defines the millis of the servo */
            } servo_pin_cfg_t;

        public:
            CLASS_NAME_T();                             
            size_t count = 0;                           /** @brief defines servo pin count */
            servo_pin_cfg_t device[ MAX_SERVOS ];       /** @brief defines pin config */
            bool mqtt_msg_on_change = false;            /** @brief defines if the mqtt message is send on change */
            bool mqtt_msg_stat = false;                 /** @brief defines if the mqtt message is send with stat */

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 4096; }
    };

