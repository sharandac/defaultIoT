/**
 * @file alloc.h
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

    #include <stdio.h>
    #include <stdlib.h>

    #include <stddef.h>
    #include <stdbool.h>
    #include <esp32-hal-psram.h>
    #include <Arduino.h>

    #if defined ( BOARD_HAS_PSRAM )
        #define MALLOC         ps_malloc            /** @brief malloac from PSRAM */
        #define CALLOC         ps_calloc            /** @brief calloc from PSRAM */
        #define REALLOC        ps_realloc           /** @brief realloc from PSRAM */
    #else
        #define MALLOC         malloc               /** @brief malloac from normal heap */
        #define CALLOC         calloc               /** @brief calloc from normal heap */
        #define REALLOC        realloc              /** @brief realloc from normal heap */
    #endif

    #define ALLOCA         alloca                   /** @brief alloac from local stack */

    #define     ABORT_FUNC( test, message, ... ) do { if( !(test) ) { log_w( message, ##__VA_ARGS__); return; } } while ( 0 )                                       /** @brief check if a condition is true, otherweise it return with a retval */
    #define     ABORT_FUNC_WITH_VALUE( test, retval, message, ... ) do { if( !(test) ) { log_w( message, ##__VA_ARGS__); return( retval ); } } while ( 0 )          /** @brief check if a condition is true, otherweise it return with a retval */
    #define     ASSERT( test, message, ... ) do { if( !(test) ) { log_e( message, ##__VA_ARGS__); while( true ); } } while ( 0 )                                    /** @brief check if a condition is true, otherweise it stop */
    #define     MALLOC_ASSERT( size, message, ... ) ( { void *p = (void*)MALLOC( size ); ASSERT( p, message, ##__VA_ARGS__ ); p; } )                                /** @brief allocate with malloc and check if allocation was successfull */
    #define     CALLOC_ASSERT( nmemb, size, message, ... ) ( { void *p = (void*)CALLOC( nmemb, size ); ASSERT( p, message, ##__VA_ARGS__ ); p; } )                  /** @brief allocate with calloc and check if allocation was successfull */
    #define     REALLOC_ASSERT( ptr, size, message, ... ) ( { void *p = (void*)REALLOC( ptr, size ); ASSERT( p, message, ##__VA_ARGS__ ); p; } )                    /** @brief allocate with realloc and check if allocation was successfull */
    #define     ALLOCA_ASSERT( size, message, ... ) ( { void *p = (void*)ALLOCA( size ); ASSERT( p, message, ##__VA_ARGS__ ); p; } )                                /** @brief allocate with alloca and check if allocation was successfull */  
    #define     CALLOCA_ASSERT( size, message, ... ) ( { void *p = (void*)ALLOCA( size ); memset( p, 0, size ); ASSERT( p, message, ##__VA_ARGS__ ); p; } )         /** @brief allocate with alloca and check if allocation was successfull */
