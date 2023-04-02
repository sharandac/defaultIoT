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
    
    #define     NTP_JSON_CONFIG_FILE     "/ntp.json"    /** @brief defines json config file name */
    /**
     * @brief 
     */
    #define     NTP_MAX_TEXT_SIZE   64
    #define     NTP_RENEW_INTERVAL  3600 * 24
    /**
     * @brief ioport config structure
     */
    class ntp_config_t : public BaseJsonConfig {
        public:
            ntp_config_t();
            bool enable = true;
            char server[ NTP_MAX_TEXT_SIZE ] = "pool.ntp.org";
            char timezone[ NTP_MAX_TEXT_SIZE ] = "CET-1CEST,M3.5.0,M10.5.0/3";
            int interval = NTP_RENEW_INTERVAL;

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 2048; }
    };
#endif // _NTP_CONFIG_H
