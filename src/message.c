/*
** lua-mqtt
** Copyright (C) 2017 tacigar
*/

#include "message.h"
#include <MQTTClient.h>
#include <lua.h>

/*
** Creates a new message and push to the stack.
*/
void messageCreate(lua_State *L, MQTTClient_message *message)
{
    lua_newtable(L);
    lua_pushlstring(L, message->payload, message->payloadlen);
    lua_setfield(L, -2, "payload");
    lua_pushnumber(L, message->qos);
    lua_setfield(L, -2, "qos");
    lua_pushboolean(L, message->retained);
    lua_setfield(L, -2, "retained");
    lua_pushboolean(L, message->dup);
    lua_setfield(L, -2, "duplicate");
}
