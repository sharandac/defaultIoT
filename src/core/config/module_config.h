/**
 * @file module_config.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

    #include "core/utils/basejsonconfig.h"
    #include "config.h"
    
    #define     MODULE_JSON_CONFIG_FILE     "/module.json"      /** @brief defines json config file name */
    /**
     * @brief maximum number of modules
     */
    #define     MAX_MODULES                 16                  /** @brief max number of modules */
    #define     MAX_MODULE_ID_NAME_LEN      32                  /** @brief max length of module id */
    /**
     * @brief ioport config structure
     */
    class module_config_t : public BaseJsonConfig {
        private:
            /**
             * @brief output pin config structure
             */
            typedef struct {
                bool enaled = false;                            // enable input    
                char id[ MAX_MODULE_ID_NAME_LEN ] = "";         // id of module
            } module_cfg_t;
        public:
            /**
             * @brief module config structure
             */
            module_config_t();
            size_t count = 0;                                   // number of modules
            module_cfg_t module[ MAX_MODULES ];                 // module config


        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 2048; }
    };

