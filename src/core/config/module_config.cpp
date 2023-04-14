/**
 * @file module_config.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "module_config.h"

CLASS_NAME_T::CLASS_NAME_T() : BaseJsonConfig( JSON_CONFIG_FILE ) {}

bool CLASS_NAME_T::onSave(JsonDocument& doc) {

    doc["count"] = count;
    doc["roundtrip"] = roundtrip;

    for( size_t i = 0 ; i < count && i < MAX_MODULES; i++ ) {
        doc["module"][ i ]["id"] = module[ i ].id;
        doc["module"][ i ]["enaled"] = module[ i ].enaled;
    }

    return true;
}

bool CLASS_NAME_T::onLoad(JsonDocument& doc) {
    
    count = doc["count"] | 0;
    roundtrip = doc["roundtrip"] | 25;

    for( size_t i = 0 ; i < count && i < MAX_MODULES; i++ ) {
        module[ i ].enaled = doc["module"][ i ]["enaled"] | true;
        strlcpy( module[ i ].id, doc["module"][ i ]["id"] | "", sizeof( module[ i ].id ) );
    }

    return true;
}

bool CLASS_NAME_T::onDefault( void ) {

    return true;
}
