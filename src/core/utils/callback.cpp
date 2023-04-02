/**
 * @file callback.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 1.1
 * @date 2020-09-14
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <Arduino.h>
#include "config.h"
#include "core/utils/callback.h"
#include "core/utils/alloc.h"

callback_t *callback_head = NULL;

void callback_print( void ) {
    /**
     * check if callback head table allocated
     */
    ABORT_FUNC( callback_head, "no initialized callback head structure found" );
    /**
     * print out all callback tables
     */
    callback_t *callback_counter = callback_head;
    do {
        log_i(" |");
        log_i(" +--'%s' ( %p / %d )", callback_counter->name, callback_counter, callback_counter->entrys );
        for( int32_t i = 0 ; i < callback_counter->entrys ; i++ ) {
            log_i(" |  +--id:'%s', event mask:%04x, prio: %01x, active: %s, counter: %d", callback_counter->table[ i ].id, callback_counter->table[ i ].event, callback_counter->table[ i ].prio, callback_counter->table[ i ].active ? "true":"false", callback_counter->table[ i ].counter );
        }
        callback_counter = callback_counter->next_callback_t;
    }
    while ( callback_counter );
}

int callback_get_entrys( void ) {
    /**
     * check if callback head exist
     */
    if( callback_head ) {
        return( callback_head->entrys );
    }
    return( 0 );
}

callback_t *callback_init( const char *name ) {
    /**
     * allocate an callback table
     */
    callback_t* callback = (callback_t*)CALLOC_ASSERT( sizeof( callback_t ), 1, "callback_t structure calloc faild for: %s", name );
    /**
     * if this the first callback table, set it as head table
     */
    if ( callback_head == NULL ) {
        callback_head = callback;
    }
    /**
     * clear callback table
     */
    callback->entrys = 0;
    callback->debug = false;
    callback->table = NULL;
    callback->name = name;
    callback->next_callback_t = NULL;
    /**
     * add the callback table to the callback table chain
     */
    callback_t *callback_counter = callback_head;
    if ( callback_head != callback ) {
        while( callback_counter->next_callback_t ) {
            callback_counter = callback_counter->next_callback_t;
        }
        callback_counter->next_callback_t = callback;
    }
    log_d("init callback_t structure success for: %s", name );

    return( callback );
}

bool callback_register( callback_t *callback, EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    bool retval = false;
    /**
     * check if callback table not NULL
     */
    ABORT_FUNC_WITH_VALUE( callback, retval, "no callback_t structure found for: %s", id );
    /**
     *  check if callback function already exist
     */
    if( callback->entrys > 0 ) {
        for( int i = 0 ; i < callback->entrys; i++ ) {
            if( callback->table[ i ].callback_func == callback_func && callback->table[ i ].prio == CALL_CB_MIDDLE ) {
                log_e("callback (%s) already exist", id );
                while( true );
            }
        }
    }
    /**
     * increment callback entry counter
     */
    callback->entrys++;
    /**
     * check if this entry is the first one for allocation or reallocate
     */
    if ( callback->table == NULL ) {
        callback->table = ( callback_table_t * )CALLOC_ASSERT( sizeof( callback_table_t ) * callback->entrys, 1, "callback_table_t calloc faild for: %s", id );
        retval = true;
    }
    else {
        callback->table = ( callback_table_t * )REALLOC_ASSERT( callback->table, sizeof( callback_table_t ) * callback->entrys, "callback_table_t realloc faild for: %s", id );
        retval = true;
    }
    /**
     * set new entry
     */
    callback->table[ callback->entrys - 1 ].event = event;
    callback->table[ callback->entrys - 1 ].callback_func = callback_func;
    callback->table[ callback->entrys - 1 ].id = id;
    callback->table[ callback->entrys - 1 ].active = true;
    callback->table[ callback->entrys - 1 ].prio = CALL_CB_MIDDLE;
    callback->table[ callback->entrys - 1 ].counter = 0;
    if ( callback->debug ) {
        log_d("register callback_func for %s success (%p:%s)", callback->name, callback->table[ callback->entrys - 1 ].callback_func, callback->table[ callback->entrys - 1 ].id );
    }
    return( retval );
}

bool callback_set_active( callback_t *callback, CALLBACK_FUNC callback_func, bool active ) {
    bool retval = false;
    /**
     * if callback table set?
     */
    ABORT_FUNC_WITH_VALUE( callback, retval, "no initialized callback structure found" );
    /**
     *  check if callback function already exist
     */
    if( callback->entrys > 0 ) {
        for( int i = 0 ; i < callback->entrys; i++ ) {
            if( callback->table[ i ].callback_func == callback_func ) {
                callback->table[ i ].active = active;
                retval = true;
            }
        }
    }
    return( retval );
}

int callback_get_entry_by_id( callback_t *callback, const char *id ) {
    int retval = -1;
    /**
     * if callback table set?
     */
    ABORT_FUNC_WITH_VALUE( callback, retval, "no initialized callback structure found" );
    /**
     *  check if callback function already exist
     */
    if( callback->entrys ) {
        for( int i = 0 ; i < callback->entrys; i++ ) {
            if( !strcmp( callback->table[ i ].id, id ) ) {
                retval = i;
                break;
            }
        }
    }
    return( retval );
}

bool callback_set_active_by_id( callback_t *callback, const char *id, bool active ) {
    bool retval = false;
    /**
     * if callback table set?
     */
    ABORT_FUNC_WITH_VALUE( callback, retval, "no initialized callback structure found" );
    /**
     *  check if callback function already exist
     */
    if( callback->entrys > 0 ) {
        /**
         * search for the callback function
         */
        for( int i = 0 ; i < callback->entrys; i++ ) {
            /**
             * check if callback function and id match
             */
            if( !strcmp( callback->table[ i ].id, id ) ) {
                callback->table[ i ].active = active;
                retval = true;
                break;
            }
        }
    }
    return( retval );
}

bool callback_register_with_prio( callback_t *callback, EventBits_t event, CALLBACK_FUNC callback_func, const char *id, callback_prio_t prio ) {
    bool retval = false;
    /**
     * check if callback table not NULL
     */
    ABORT_FUNC_WITH_VALUE( callback, retval, "no callback_t structure found for: %s", id );
    /**
     *  check if callback function already exist
     */
    if( callback->entrys > 0 ) {
        for( int i = 0 ; i < callback->entrys; i++ ) {
            if( callback->table[ i ].callback_func == callback_func  && callback->table[ i ].prio == prio ) {
                log_e("callback (%s) already exist", id );
                while( true );
            }
        }
    }
    /**
     * increment callback entry counter
     */
    callback->entrys++;
    /**
     * check if this entry is the first one for allocation or reallocate
     */
    if ( callback->table == NULL ) {
        callback->table = ( callback_table_t * )CALLOC_ASSERT( sizeof( callback_table_t ) * callback->entrys, 1, "callback_table_t calloc faild for: %s", id  );
        retval = true;
    }
    else {
        callback->table = ( callback_table_t * )REALLOC_ASSERT( callback->table, sizeof( callback_table_t ) * callback->entrys,"callback_table_t realloc faild for: %s", id );
        retval = true;
    }
    /**
     * set new entry
     */
    callback->table[ callback->entrys - 1 ].event = event;
    callback->table[ callback->entrys - 1 ].callback_func = callback_func;
    callback->table[ callback->entrys - 1 ].id = id;
    callback->table[ callback->entrys - 1 ].active = true;
    callback->table[ callback->entrys - 1 ].prio = prio;
    callback->table[ callback->entrys - 1 ].counter = 0;
    if ( callback->debug ) {
        log_d("register callback_func for %s success (%p:%s)", callback->name, callback->table[ callback->entrys - 1 ].callback_func, callback->table[ callback->entrys - 1 ].id );
    }
    return( retval );
}

bool callback_send_to_id( callback_t *callback, EventBits_t event, void *arg, const char *id ) {
    bool retval = false;
    /**
     * if callback table set?
     */
    ABORT_FUNC_WITH_VALUE( callback, retval, "no initialized callback structure found" );
    /**
     * has callback table entrys?
     */
    ABORT_FUNC_WITH_VALUE( callback->entrys, retval, "no callback entrys found [%s]", callback->name );
    /**
     * crowl all callback entrys with their right mask and order
     */
    for( int prio = CALL_CB_NUM_START ; prio < CALL_CB_NUM ; prio++ ) {
        for ( int entry = 0 ; entry < callback->entrys ; entry++ ) {
            yield();
            if ( event & callback->table[ entry ].event && callback->table[ entry ].prio == prio && callback->table[ entry ].active && !strcmp( callback->table[ entry ].id, id ) ) {
                /**
                 * print out callback event
                 */
                if ( callback->debug ) {
                    log_i("call %s cb (%p:%04x:%s:%d)", callback->name, callback->table[ entry ].callback_func, event, callback->table[ entry ].id, callback->table[ entry ].prio );
                }
                /**
                 * increment callback counter
                 */
                callback->table[ entry ].counter++;
                /**
                 * check if callback function is set
                 */
                ASSERT( callback->table[ entry ].callback_func, "callback_func is NULL");
                /**
                 * call callback an check the returnvalue
                 */
                if ( !callback->table[ entry ].callback_func( event, arg ) ) {
                    log_i("cb %s returns false", callback->table[ entry ].id );
                    retval = false;
                }
            }
        }
    }
    return( retval );
}

bool callback_send( callback_t *callback, EventBits_t event, void *arg ) {
    bool retval = false;
    /**
     * if callback table set?
     */
    ABORT_FUNC_WITH_VALUE( callback, retval, "no initialized callback structure found" );
    /**
     * has callback table entrys?
     */
    ABORT_FUNC_WITH_VALUE( callback->entrys, retval, "no callback entrys found [%s]", callback->name );
    retval = true;
    /**
     * crowl all callback entrys with their right mask and order
     */
    for( int prio = CALL_CB_NUM_START ; prio < CALL_CB_NUM ; prio++ ) {
        for ( int entry = 0 ; entry < callback->entrys ; entry++ ) {
            yield();
            if ( event & callback->table[ entry ].event && callback->table[ entry ].prio == prio && callback->table[ entry ].active ) {
                /**
                 * print out callback event
                 */
                if ( callback->debug ) {
                    log_i("call %s cb (%p:%04x:%s:%d)", callback->name, callback->table[ entry ].callback_func, event, callback->table[ entry ].id, callback->table[ entry ].prio );
                }
                /**
                 * increment callback counter
                 */
                callback->table[ entry ].counter++;
                /**
                 * check if callback function is set
                 */
                ASSERT( callback->table[ entry ].callback_func, "callback_func is NULL");
                /**
                 * call callback an check the returnvalue
                 */
                if ( !callback->table[ entry ].callback_func( event, arg ) ) {
                    log_d("cb %s returns false", callback->table[ entry ].id );
                    retval = false;
                }
            }
        }
    }
    return( retval );
}

bool callback_send_reverse( callback_t *callback, EventBits_t event, void *arg ) {
    bool retval = false;
    /**
     * if callback table set?
     */
    ABORT_FUNC_WITH_VALUE( callback, retval, "no initialized callback structure found" );
    /**
     * has callback table entrys?
     */
    ABORT_FUNC_WITH_VALUE( callback->entrys, retval, "no callback entrys found [%s]", callback->name );

    retval = true;
    /**
     * crowl all callback entrys with their right mask
     */
    for( int prio = CALL_CB_NUM ; prio >= CALL_CB_NUM_START ; prio-- ) {
        for ( int entry = callback->entrys - 1; entry >= 0 ; entry-- ) {
            yield();
            if ( event & callback->table[ entry ].event  && callback->table[ entry ].prio == prio && callback->table[ entry ].active ) {
                /**
                 * print out callback event
                 */
                if ( callback->debug ) {
                    log_i("call %s cb (%p:%04x:%s:%d)", callback->name, callback->table[ entry ].callback_func, event, callback->table[ entry ].id, callback->table[ entry ].prio );
                }
                /**
                 * increment callback counter
                 */
                callback->table[ entry ].counter++;
                /**
                 * check if callback function is set
                 */
                ASSERT( callback->table[ entry ].callback_func, "callback_func is NULL");
                /**
                 * call callback an check the returnvalue
                 */
                if ( !callback->table[ entry ].callback_func( event, arg ) ) {
                    retval = false;
                }
            }
        }
    }
    return( retval );
}

bool callback_send_no_log( callback_t *callback, EventBits_t event, void *arg ) {
    bool retval = false;
    /**
     * if callback table set?
     */
    if ( callback == NULL ) {
        return( retval );
    }
    /**
     * has callback table entrys?
     */
    if ( callback->entrys == 0 ) {
        return( retval );
    }
    retval = true;
    /**
     * crowl all callback entrys with their right mask
     */
    for( int prio = CALL_CB_NUM_START ; prio < CALL_CB_NUM ; prio++ ) {
        for ( int entry = 0 ; entry < callback->entrys ; entry++ ) {
            yield();
            if ( event & callback->table[ entry ].event  && callback->table[ entry ].prio == prio && callback->table[ entry ].active ) {
                /**
                 * increment callback counter
                 */
                callback->table[ entry ].counter++;
                /**
                 * check if callback function is set
                 */
                ASSERT( callback->table[ entry ].callback_func, "callback_func is NULL");
                /**
                 * call callback an check the returnvalue
                 */
                if ( !callback->table[ entry ].callback_func( event, arg ) ) {
                    retval = false;
                }
            }
        }
    }
    return( retval );
}
