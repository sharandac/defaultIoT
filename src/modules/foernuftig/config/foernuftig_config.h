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
     * @brief defines json config file name
     */
    #define     FOERNUFTIG_JSON_CONFIG_FILE     "/foeruftig.json"    /** @brief defines json config file name */
    /**
     * @brief ioport config structure
     */
    class foernuftig_config_t : public BaseJsonConfig {
        public:
            foernuftig_config_t();
            int button_0_pin = 21;              // Button Off pin
            int button_1_pin = 22;              // Button stage 1 pin
            int button_2_pin = 32;              // Button stage 2 pin
            int button_3_pin = 25;              // Button stage 3 pin
            int supply_pin = 19;                // Supply pin
            int clk_pin = 18;                   // Clock pin
            int fg_pin = 33;                    // Fan good pin
            int led_pin = 35;                   // LED pin
            int usb_power_pin = 17;             // USB power pin
            bool usb_power_state = false;       // USB power state
            int min_rpm = 50;                   // Minimum RPM
            int mid_rpm = 150;                  // Mid RPM
            int max_rpm = 300;                  // Max RPM
            float rpm_factor = 2.0;             // RPM factor
            bool mqtt_msg_stat = true;          // MQTT message status
            
        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 8192; }
    };
