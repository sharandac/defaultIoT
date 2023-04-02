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

module_config_t::module_config_t() : BaseJsonConfig( MODULE_JSON_CONFIG_FILE ) {}

bool module_config_t::onSave(JsonDocument& doc) {

    doc["count"] = count;

    for( size_t i = 0 ; i < count && i < MAX_MODULES; i++ ) {
        doc["module"][ i ]["id"] = module[ i ].id;
        doc["module"][ i ]["enaled"] = module[ i ].enaled;
    }

    return true;
}

bool module_config_t::onLoad(JsonDocument& doc) {
    
    count = doc["count"] | 0;

    for( size_t i = 0 ; i < count && i < MAX_MODULES; i++ ) {
        module[ i ].enaled = doc["module"][ i ]["enaled"] | true;
        strlcpy( module[ i ].id, doc["module"][ i ]["id"] | "", sizeof( module[ i ].id ) );
    }

    return true;
}

bool module_config_t::onDefault( void ) {

    return true;
}
