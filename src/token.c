/*
** lua-mqtt
** Copyright (C) 2017 tacigar
*/

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "token.h"

/*
** This function creates a Token object.
*/
Token *tokenCreate(lua_State *L, MQTTClient client, int tk)
{
    Token *token = (Token *)lua_newuserdata(L, sizeof(Token));
    token->m_client = client;
    token->m_token = tk;

    luaL_getmetatable(L, MQTT_TOKEN_CLASS);
    lua_setmetatable(L, -2);
    return token;
}

/*
** This function is called by the client application to synchronize execution
**  of the main thread with completed publication of a message.
*/
static int tokenWait(lua_State *L)
{
    int rc;
    Token *token = (Token *)luaL_checkudata(L, 1, MQTT_TOKEN_CLASS);
    int timeout = luaL_checkinteger(L, 2);

    rc = MQTTClient_waitForCompletion(token->m_client, token->m_token, timeout);

    lua_pushnumber(L, rc);
    return 1;
}

/*
** This function returns a value of the token.
*/
static int tokenGetValue(lua_State *L)
{
    Token *token = (Token *)luaL_checkudata(L, 1, MQTT_TOKEN_CLASS);
    lua_pushnumber(L, token->m_token);
    return 1;
}

/*
** Module entry point.
*/
LUALIB_API int luaopen_mqtt_Token(lua_State *L)
{
    struct luaL_Reg *ptr;
    struct luaL_Reg methods[] = {
        { "wait",     tokenWait     },
        { "getValue", tokenGetValue },
        { NULL, NULL }
    };

    luaL_newmetatable(L, MQTT_TOKEN_CLASS);

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
