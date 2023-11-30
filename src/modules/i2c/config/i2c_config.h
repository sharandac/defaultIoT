/**
 * @file onewire_config.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-19
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
    #define     CLASS_NAME_T            i2c_config_t            /** @brief defines namespace */
    #define     JSON_CONFIG_FILE        "/i2c.json"             /** @brief defines json config file name */
    #define     MAX_LENGTH              64                      /** @brief defines max length for strings */
    #define     MAX_SENSORS             16                      /** @brief defines max inputs */
    /**
     * @brief input config structure
     */
    class CLASS_NAME_T : public BaseJsonConfig {
        public:
            CLASS_NAME_T();                                     
            bool enabled = false;                               /** @brief defines if the module enabled */
            int frequency = 1000;                               /** @brief defines the bitrate for the i2c bus */
            int scl = -1;                                       /** @brief defines the pin for the i2c bus */
            int sda = 15;                                       /** @brief defines the internal interval for the i2c bus */
            bool mqtt_msg_stat = false;                         /** @brief defines if the module send mqtt messages */

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 4096; }
    };

