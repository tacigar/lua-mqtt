/* 
** lua-mqtt
** Copyright (C) 2017 tacigar
*/

#ifndef TOKEN_BASE_H
#define TOKEN_BASE_H

#include <MQTTClient.h>
#include <lua.h>

#define MQTT_TOKEN_BASE_CLASS "mqtt.TokenBase"

typedef struct TokenBase
{
    MQTTClient m_client;
    MQTTClient_deliveryToken m_token;
} TokenBase;

TokenBase *tokenBaseCreate(lua_State *L, MQTTClient client, int tk);

#endif /* TOKEN_BASE_H */
