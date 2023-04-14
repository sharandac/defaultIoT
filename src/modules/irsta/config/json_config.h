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
    /**
     * @brief class settings like namespace, filename and max length for strings
     */
    #define     CLASS_NAME_T            irsta_config_t      /** @brief defines namespace */
    #define     JSON_CONFIG_FILE        "/irsta.json"       /** @brief defines json config file name */
    #define     MAX_LENGTH              32                  /** @brief defines max length for strings */
    /**
     * @brief button config structure
     */
    class CLASS_NAME_T : public BaseJsonConfig {
        public:
            CLASS_NAME_T();
            int clk_pin = 18;                               /** @brief defines neopixel clk pin */
            int led_count = 512;                            /** @brief defines neopixel led count */
            int max_led = 112;                              /** @brief defines max led count */
            int max = 64;                                   /** @brief defines max value */
            int red = 255;                                  /** @brief defines red value */
            int green = 255;                                /** @brief defines green value */
            int blue = 255;                                 /** @brief defines blue value */
            int button_0 = 21;                              /** @brief defines button 0 pin */
            int button_1 = 22;                              /** @brief defines button 1 pin */                        
            int button_2 = 32;                              /** @brief defines button 2 pin */
            int button_3 = 25;                              /** @brief defines button 3 pin */
            int threshold = 25;                             /** @brief defines threshold value */
            bool mqtt_msg_stat = true;                      /** @brief defines if mqtt msg stat is enabled */

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 2048; }
    };
#endif // _IRSTA_CONFIG_H
