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
    /**
     * @brief class settings like namespace, filename and max length for strings
     */
    #define     CLASS_NAME_T            pwm_config_t    /** @brief defines namespace */
    #define     JSON_CONFIG_FILE        "/pwm.json"     /** @brief defines json config file name */
    #define     MAX_LENGTH              32              /** @brief defines max length for strings */
    #define     MAX_PWM                 4               /** @brief defines max pwm */
    /**
     * @brief input config structure
     */
    class CLASS_NAME_T : public BaseJsonConfig {
        private:
            /**
             * @brief output pin config structure
             */
            typedef struct {
                bool enaled = true;                     /** @brief defines if the output pin config is enabled */
                int pin;                                /** @brief defines the output pin */
                char id[ MAX_LENGTH ] = "";             /** @brief defines the id of the output pin */
                int frequency = 8000;                   /** @brief defines the frequency of the pwm */
                int value = 0;                          /** @brief defines the value of the pwm */
                int min = 0;                            /** @brief defines the min value of the pwm */
                int max = 255;                          /** @brief defines the max value of the pwm */
            } pwm_pin_cfg_t;

        public:
            CLASS_NAME_T();                             
            size_t pwm_count = 0;                       /** @brief defines output pin count */
            pwm_pin_cfg_t device[ MAX_PWM ];            /** @brief defines pin config */
            bool mqtt_msg_on_change = false;            /** @brief defines if the mqtt message is send on change */
            bool mqtt_msg_stat = false;                 /** @brief defines if the mqtt message is send with stat */

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 4096; }
    };

