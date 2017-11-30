/* 
** lua-mqtt
** Copyright (C) 2017 tacigar
*/

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "token_base.h"

/*
** This function creates a Token object.
*/
TokenBase *tokenBaseCreate(lua_State *L, MQTTClient client, int tk)
{
    TokenBase *token = (TokenBase *)lua_newuserdata(L, sizeof(TokenBase));
    token->m_client = client;
    token->m_token = tk;

    luaL_getmetatable(L, MQTT_TOKEN_BASE_CLASS);
    lua_setmetatable(L, -2);
    return token;
}

/*
** This function is called by the client application to synchronize execution
**  of the main thread with completed publication of a message. 
*/
static int tokenBaseWait(lua_State *L)
{
    int rc;
    TokenBase *token = (TokenBase *)luaL_checkudata(L, 1, MQTT_TOKEN_BASE_CLASS);
    int timeout = luaL_checkinteger(L, 2);
    
    rc = MQTTClient_waitForCompletion(token->m_client, token->m_token, timeout);

    lua_pushnumber(L, rc);
    return 1;
}

/*
** Module entry point.
*/
LUALIB_API int luaopen_mqtt_TokenBase(lua_State *L)
{
    struct luaL_Reg *ptr;
    struct luaL_Reg methods[] = {
        { "wait", tokenBaseWait },
        { NULL, NULL }
    };

    luaL_newmetatable(L, MQTT_TOKEN_BASE_CLASS);

    lua_pushstring(L, "__index");
    lua_newtable(L);
    ptr = methods;
    do {
        lua_pushstring(L, ptr->name);
        lua_pushcfunction(L, ptr->func);
        lua_rawset(L, -3);
        ptr++;
    } while (ptr->name);
    lua_rawset(L, -3);

    lua_pop(L, 1); /* pop the metatable. */
    return 0;
}
