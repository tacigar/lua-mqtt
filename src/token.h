/* 
** lua-mqtt
** Copyright (C) 2017 tacigar
*/

#include <MQTTClient.h>
#include <lua.h>

#define MQTT_TOKEN_CLASS "mqtt.token"

typedef struct Token 
{
    MQTTClient m_client;
    MQTTClient_deliveryToken m_token;
} Token;

Token *tokenCreate(lua_State *L, MQTTClient client, int tk);

