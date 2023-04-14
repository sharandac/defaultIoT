/**
 * @file core_config.cpp
 * @author Dirk Broßwick (dirk.brosswick@googlemail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "ftpserver_config.h"

CLASS_NAME_T::CLASS_NAME_T() : BaseJsonConfig( JSON_CONFIG_FILE ) {
}

bool CLASS_NAME_T::onSave(JsonDocument& doc) {

    doc["enable"] = enable;
    doc["username"] = username;
    doc["password"] = password;

    return true;
}

bool CLASS_NAME_T::onLoad(JsonDocument& doc) {

    enable = doc["enable"] | true;
    strncpy( username, doc["username"] | "user", MAX_LENGTH );
    strncpy( password, doc["password"] | "password", MAX_LENGTH );

    return true;
}

bool CLASS_NAME_T::onDefault( void ) {

    return true;
}
