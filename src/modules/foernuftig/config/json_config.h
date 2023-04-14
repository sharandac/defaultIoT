/**
 * @file foernuftig_config.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

    #include "core/utils/basejsonconfig.h"
    /**
     * @brief class settings like namespace, filename and max length for strings
     */
    #define     CLASS_NAME_T            foernuftig_config_t     /** @brief defines namespace */
    #define     JSON_CONFIG_FILE        "/foernuftig.json"      /** @brief defines json config file name */
    #define     MAX_LENGTH              32                      /** @brief defines max length for strings */
    /**
     * @brief ioport config structure
     */
    class CLASS_NAME_T : public BaseJsonConfig {
        public:
            CLASS_NAME_T();
            int button_0_pin = 21;                              /** @brief defines the pin for button 0 */
            int button_1_pin = 22;                              /** @brief defines the pin for button 1 */
            int button_2_pin = 32;                              /** @brief defines the pin for button 2 */
            int button_3_pin = 25;                              /** @brief defines the pin for button 3 */
            int supply_pin = 19;                                /** @brief defines the pin for supply */
            int clk_pin = 18;                                   /** @brief defines the pin for clock */
            int fg_pin = 33;                                    /** @brief defines the pin for fg */
            int led_pin = 35;                                   /** @brief defines the pin for led */
            int usb_power_pin = 17;                             /** @brief defines the pin for usb power */
            bool usb_power_state = false;                       /** @brief defines the last state for usb power */
            int min_rpm = 50;                                   /** @brief defines the min rpm */
            int mid_rpm = 150;                                  /** @brief defines the mid rpm */
            int max_rpm = 300;                                  /** @brief defines the max rpm */
            float rpm_factor = 2.0;                             /** @brief defines the rpm factor */
            bool mqtt_msg_stat = true;                          /** @brief defines if mqtt msg stat is enabled */
            
        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 8192; }
    };
