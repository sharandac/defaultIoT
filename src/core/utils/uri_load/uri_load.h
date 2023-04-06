/**
 * @file uri_load.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.8
 * @date 2020-08-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _URI_LOAD__H
    #define _URI_LOAD__H

    #include <stdint.h>

    #define URI_LOAD_INFO_LOG   log_i
    #define URI_LOAD_LOG        log_d
    #define URI_LOAD_ERROR_LOG  log_e

    #define URI_BLOCK_SIZE      4096

    /**
     * @brief typedef for the callback function call
     * 
     * @param percent   download in percent
     */
    typedef void ( progress_cb_t ) ( int32_t percent );
    /**
     * @brief typedef for uri_load_discriptor
     */
    typedef struct {
        char *filename;             /** @brief pointer to the downloaded filename */
        char *uri;                  /** @brief pointer to the downloaded filename */
        uint32_t size;              /** @brief filesize in bytes */
        uint64_t timestamp;         /** @brief download timestamp */
        uint8_t *data;              /** @brief pointer to the downloaded data */
        progress_cb_t *progresscb;  /** @brief progress call back */
    } uri_load_dsc_t;
    /**
     * @brief doenload a file from a webserver into ram
     * 
     * @param   uri requested url to get a file from
     * 
     * @return  uri_load_dsc structure
     */
    uri_load_dsc_t *uri_load_to_ram( const char *uri );
    /**
     * @brief doenload a file from a webserver into ram
     * 
     * @param   uri requested url to get a file from
     * @param   progresscb  pointer to a call back funtion
     * 
     * @return  uri_load_dsc structure
     */
    uri_load_dsc_t *uri_load_to_ram( const char *uri, progress_cb_t *progresscb );
    /**
     * @brief doenload a file from a webserver into a file
     * 
     * @param   uri requested url to get a file from
     * @param   path path to write the downloaded file, e.g.: "/spiffs/"
     * 
     * @return  true if success
     */
    bool uri_load_to_file( const char *uri, const char *path );
    /**
     * @brief doenload a file from a webserver into a file
     * 
     * @param   uri requested url to get a file from
     * @param   path path to write the downloaded file, e.g.: "/spiffs/"
     * 
     * @return  true if success
     */
    bool uri_load_to_file( const char *uri, const char *path, const char *dest_filename );
    /**
     * @brief doenload a file from a webserver into a file
     * 
     * @param   uri requested url to get a file from
     * @param   path path to write the downloaded file, e.g.: "/spiffs/"
     * @param   progresscb  pointer to a call back funtion
     * 
     * @return  true if success
     */
    bool uri_load_to_file( const char *uri, const char *path, const char *dest_filename, progress_cb_t *progresscb );
    /**
     * @brief delete the complete uri_load_dsc structure and free all allocated memory
     * 
     * @param uri_load_dsc pointer to a uri_load_dsc structure
     */
    void uri_load_free_all( uri_load_dsc_t *uri_load_dsc );
    /**
     * @brief delete the filename from the uri_load_dsc structure and free their allocated memory
     * 
     * @param uri_load_dsc pointer to a uri_load_dsc structure
     */
    void uri_load_free_name( uri_load_dsc_t *uri_load_dsc );
    /**
     * @brief delete the url from the uri_load_dsc structure and free their allocated memory
     * 
     * @param uri_load_dsc pointer to a uri_load_dsc structure
     */
    void uri_load_free_url( uri_load_dsc_t *uri_load_dsc );
    /**
     * @brief delete the data from the uri_load_dsc structure and free their allocated memory
     * 
     * @param uri_load_dsc pointer to a uri_load_dsc structure
     */
    void uri_load_free_data( uri_load_dsc_t *uri_load_dsc );
    /**
     * @brief delete the uri_load_dsc structure and leave data in allocated
     * 
     * @param uri_load_dsc pointer to a uri_load_dsc structure
     */
    void uri_load_free_without_data( uri_load_dsc_t *uri_load_dsc );
    /**
     * @brief delete only the allocated uri_load_sdc structure and leave the name/data memory allocated
     * for later use
     * 
     * @param uri_load_dsc pointer to a uri_load_dsc structure
     */
    void uri_load_free_dsc( uri_load_dsc_t *uri_load_dsc );

#endif // _URI_LOAD__H
