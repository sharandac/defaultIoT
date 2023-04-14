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
    /**
     * @brief class settings like namespace, filename and max length for strings
     */
    #define     CLASS_NAME_T            ftpserver_config_t  /** @brief defines namespace */
    #define     JSON_CONFIG_FILE        "/ftp.json"         /** @brief defines json config file name */
    #define     MAX_LENGTH              32                  /** @brief defines max length for strings */
    /**
     * @brief ioport config structure
     */
    class CLASS_NAME_T : public BaseJsonConfig {
        public:
            CLASS_NAME_T();
            bool enable = true;                             /** @brief enable ftp server */
            char username[ MAX_LENGTH ] = "user";           /** @brief username for ftp server */
            char password[ MAX_LENGTH ] = "password";       /** @brief password for ftp server */

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 2048; }
    };
