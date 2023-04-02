/**
 * @file vindriktning_config.h
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
    
    #define     VINDRIKTNING_JSON_CONFIG_FILE     "/vindriktning.json"    /** @brief defines json config file name */
    /**
     * @brief 
     */
    #define     MQTT_MAX_TEXT_SIZE   64
    /**
     * @brief ioport config structure
     */
    class vindriktning_config_t : public BaseJsonConfig {
        public:
            vindriktning_config_t();
            int RXpin = 16;
            bool mqtt_msg_stat = true;
            
        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 8192; }
    };
