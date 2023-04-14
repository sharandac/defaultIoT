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
    #define     CLASS_NAME_T            analog_config_t         /** @brief defines namespace */
    #define     JSON_CONFIG_FILE        "/analog.json"          /** @brief defines json config file name */
    #define     MAX_LENGTH              32                      /** @brief defines max length for strings */
    #define     MAX_ANALOG              3                       /** @brief defines max analog inputs */
    /**
     * @brief input config structure
     */
    class CLASS_NAME_T : public BaseJsonConfig {
        private:
            /**
             * @brief servo pin config structure
             */
            typedef struct {
                bool enaled = false;                            /** @brief defines if the analog pin config is enabled */     
                int pin = -1;                                   /** @brief defines the analog pin */
                char id[ MAX_LENGTH ] = "";                     /** @brief defines the id of the analog pin */
                int value = 0;                                  /** @brief defines the value of the analog pin */
                int trigger_low = 0;                            /** @brief defines the trigger low value */
                int trigger_high = 4095;                        /** @brief defines the trigger high value */
                int analog_min = 4095;                          /** @brief defines the min value */
                int analog_max = 0;                             /** @brief defines the max value */
                bool analog_trigger = false;                    /** @brief defines if the analog pin is triggered */
                int trigger_count = 0;                          /** @brief defines the trigger count */
                bool trigger_count_reset = false;               /** @brief defines if the trigger count is reset */
            } analog_pin_cfg_t;

        public:
            CLASS_NAME_T();                          
            size_t count = 0;                                   /** @brief defines the pin count */
            analog_pin_cfg_t device[ MAX_ANALOG ];              /** @brief defines the analog pin config */
            bool mqtt_msg_on_change = false;                    /** @brief defines if the mqtt message is send on change */
            bool mqtt_msg_stat = false;                         /** @brief defines if the mqtt message is send with stat */

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 4096; }
    };

