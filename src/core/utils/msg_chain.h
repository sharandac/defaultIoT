/**
 * @file msg_chain.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 1.0
 * @date 2020-10-05
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once

    #include <stdint.h>
    #include <sys/time.h>
    /**
     * @brief msg chain entry structure
     */
    struct msg_chain_entry_t {
        msg_chain_entry_t *prev_msg;                    /** @brief pointer to the preview msg chain entry, if NULL no preview entry available */
        time_t timestamp;                               /** @brief timestamp, set when created */
        const char *msg;                                /** @brief pointer to the msg itself, terminated with \0 */
        msg_chain_entry_t *next_msg;                    /** @brief pointer to the next msg chain entry, if NULL no next entry available */
    };
    /**
     * @brief msg chain head structure
     */
    struct msg_chain_t {
        size_t entrys;                                  /** @brief number of entry, count by it self */
        msg_chain_entry_t *first_msg_chain_entry;       /** @brief pointer to the first msg_chain_entry */
        size_t current_entry;                           /** @brief number of the current entry */
        msg_chain_entry_t *current_msg_chain_entry;     /** @brief pointer to the current entry */
    };
    /**
     * @brief add a message to the msg_chain
     * 
     * @param   msg_chain   pointer to the msg_chain, if NULL a new msg_chain is make. don't forget to save the return pointer
     * @param   msg         the message to store
     * 
     * @return  pointer     to the msg_chain structure, NULL if failed
     */
    msg_chain_t *msg_chain_add_msg( msg_chain_t *msg_chain, const char *msg );
    /**
     * @brief delete an msg from the msg_chain
     * 
     * @param   msg_chain   pointer to the msg_chain
     * @param   entry       entry number to delete
     * 
     * @return  true if success, false if failed
     */
    bool msg_chain_delete_msg_entry( msg_chain_t *msg_chain, size_t entry );
    /**
     * @brief get an messges timestamp entry from the msg_chain
     * 
     * @param   msg_chain   pointer to the msg_chain
     * @param   entry       entry numger to get
     * 
     * @return  pointer to the messages timestamp of NULL if failed
     */
    time_t* msg_chain_get_msg_timestamp_entry( msg_chain_t *msg_chain, size_t entry );
    /**
     * @brief get an messges entry from the msg_chain
     * 
     * @param   msg_chain   pointer to the msg_chain
     * @param   entry       entry numger to get
     * 
     * @return  pointer to the messages of NULL if failed
     */
    const char* msg_chain_get_msg_entry( msg_chain_t *msg_chain, size_t entry );
    /**
     * @brief get the number of entry in the msg_chain
     * 
     * @return  number of entrys
     */
    size_t msg_chain_get_entrys( msg_chain_t *msg_chain );
    /**
     * @brief delete all msg from the chain and the msg_chain by itself
     * 
     * @return  new NULL pointer for msg_chain_t * variable or msg_chain pointer when failed
     */
    msg_chain_t * msg_chain_delete( msg_chain_t *msg_chain );
    /**
     * @brief printf all messages from the chain
     * 
     * @param   msg_chain   pointer to the msg_chain
     */
    void msg_chain_printf_msg_chain( msg_chain_t *msg_chain );
