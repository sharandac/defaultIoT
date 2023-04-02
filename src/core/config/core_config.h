/**
 * @file wifi_config.h
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
    #include "config.h"
    
    #define     CORE_JSON_CONFIG_FILE     "/core.json"    /** @brief defines json config file name */
    /**
     * @brief ioport config structure
     */
    class core_config_t : public BaseJsonConfig {
        public:
            core_config_t();
            int frequency = 240;
            bool brownout_detection = true;
            bool watchdog = true;
            bool frequency_scaling = false;
            bool light_sleep = false;

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 2048; }
    };
