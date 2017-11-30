/* 
** lua-mqtt
** Copyright (C) 2017 tacigar
*/

#ifndef MQTT_TOKEN_H
#define MQTT_TOKEN_H

#include <MQTTClient.h>
#include <lua.h>

#define MQTT_TOKEN_CLASS "mqtt.Token"

typedef struct Token
{
    MQTTClient m_client;
    MQTTClient_deliveryToken m_token;
} Token;

Token *tokenCreate(lua_State *L, MQTTClient client, int tk);

#endif /* MQTT_TOKEN_H */
