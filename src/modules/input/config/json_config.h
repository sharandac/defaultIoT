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
    /**
     * @brief class settings like namespace, filename and max length for strings
     */
    #define     CLASS_NAME_T            input_config_t      /** @brief defines namespace */
    #define     JSON_CONFIG_FILE        "/input.json"       /** @brief defines json config file name */
    #define     MAX_LENGTH              32                  /** @brief defines max length for strings */
    #define     MAX_INPUTS              4                   /** @brief defines max inputs */
    /**
     * @brief input config structure
     */
    class CLASS_NAME_T : public BaseJsonConfig {
        private:
            /**
             * @brief input pin config structure
             */
            typedef struct {
                bool enaled = true;                         /** @brief defines if the input pin config is enabled */
                int pin;                                    /** @brief defines the input pin */
                char id[ MAX_LENGTH ] = "";                 /** @brief defines the id of the input pin */
                size_t pin_cfg = INPUT;                     /** @brief defines the pin config */
                bool invert = false;                        /** @brief defines if the input pin is inverted */
            } input_pin_cfg_t;

        public:
            CLASS_NAME_T();                         
            size_t input_count = 0;                         /** @brief defines input pin count */
            input_pin_cfg_t device[ MAX_INPUTS ];           /** @brief defines pin config */
            bool mqtt_msg_on_change = false;                /** @brief defines if the mqtt message is send on change */
            bool mqtt_msg_stat = false;                     /** @brief defines if the mqtt message is send with stat */

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 4096; }
    };

