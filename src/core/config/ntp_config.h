/**
 * @file ntp_config.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _NTP_CONFIG_H
    #define _NTP_CONFIG_H

    #include "core/utils/basejsonconfig.h"
    #include "config.h"
    /**
     * @brief class settings like namespace, filename and max length for strings
     */
    #define     CLASS_NAME_T            ntp_config_t        /** @brief defines namespace */
    #define     JSON_CONFIG_FILE        "/ntp.json"         /** @brief defines json config file name */
    #define     MAX_LENGTH              64                  /** @brief defines max length for strings */
    #define     NTP_RENEW_INTERVAL  3600 * 24
    /**
     * @brief ioport config structure
     */
    class CLASS_NAME_T : public BaseJsonConfig {
        public:
            CLASS_NAME_T();
            bool enable = true;                             /** @brief defines if ntp is enabled */
            char server[ MAX_LENGTH ] = "pool.ntp.org";     /** @brief defines server */
            char timezone[ MAX_LENGTH ] = "CET-1CEST,M3.5.0,M10.5.0/3"; /** @brief defines timezone */
            int interval = NTP_RENEW_INTERVAL;              /** @brief defines ntp update interval */

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 2048; }
    };
#endif // _NTP_CONFIG_H
