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
    /**
     * @brief class settings like namespace, filename and max length for strings
     */
    #define     CLASS_NAME_T            module_config_t     /** @brief defines namespace */
    #define     JSON_CONFIG_FILE        "/module.json"      /** @brief defines json config file name */
    #define     MAX_MODULES             16                  /** @brief max number of modules */
    #define     MAX_LENGTH              32                  /** @brief defines max length for strings */
    /**
     * @brief ioport config structure
     */
    class CLASS_NAME_T : public BaseJsonConfig {
        private:
            /**
             * @brief output pin config structure
             */
            typedef struct {
                bool enaled = false;                        /** @brief enable module */  
                char id[ MAX_LENGTH ] = "";                 /** @brief id of module */
            } module_cfg_t;
        public:
            /**
             * @brief module config structure
             */
            CLASS_NAME_T();
            size_t count = 0;                               /** @brief number of modules */
            size_t roundtrip = 25;                          /** @brief roundtrip time in ms */
            uint64_t used_roundtrip_time = 0;               /** @brief used roundtrip time in ms */
            module_cfg_t module[ MAX_MODULES ];             /** @brief module config */


        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 2048; }
    };

