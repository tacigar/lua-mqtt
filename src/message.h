/*
** lua-mqtt
** Copyright (C) 2017 tacigar
*/

#ifndef MQTT_MESSAGE_H
#define MQTT_MESSAGE_H

#include <MQTTClient.h>
#include <lua.h>

void messageCreate(lua_State *L, MQTTClient_message *message);

#endif /* MQTT_MESSAGE_H */
