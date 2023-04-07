/**
 * @file irsta_config.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _IRSTA_CONFIG_H
    #define _IRSTA_CONFIG_H

    #include "core/utils/basejsonconfig.h"

    #define     IRSTA_JSON_CONFIG_FILE    "/irsta.json" /** @brief defines json config file name */
    /**
     * @brief button config structure
     */
    class irsta_config_t : public BaseJsonConfig {
        public:
            irsta_config_t();
            int clk_pin = 18;
            int led_count = 512;
            int max_led = 112;
            int max = 64;
            int red = 255;
            int green = 255;
            int blue = 255;
            int button_0 = 21;
            int button_1 = 22;
            int button_2 = 32;
            int button_3 = 25;
            int threshold = 25;
            bool mqtt_msg_stat = true;

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 2048; }
    };
#endif // _IRSTA_CONFIG_H
