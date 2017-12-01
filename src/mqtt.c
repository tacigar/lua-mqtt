/*
** lua-mqtt
** Copyright (C) 2017 tacigar
*/

#include <MQTTClient.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

/*
** Global init of mqtt library. Call once on program start to set global
** behaviour. do_openssl_init - if mqtt library should initialize OpenSSL
** (1) or rely on the caller to do it before using the library (0).
*/
static int mqttGlobalInit(lua_State *L)
{
    int doOpenSSLInit = lua_toboolean(L, 1);
    MQTTClient_init_options opts = MQTTClient_init_options_initializer;

    opts.do_openssl_init = doOpenSSLInit;
    MQTTClient_global_init(&opts);

    return 0;
}

/*
** This function returns version information about the library. no trace
** information will be returned.
*/
static int mqttGetVersionInfo(lua_State *L)
{
    int i;
    MQTTClient_nameValue *infos = MQTTClient_getVersionInfo();

    lua_newtable(L);
    for (;;) {
        if (infos[i].name == NULL) {
            break;
        }
        lua_pushstring(L, infos[i].value);
        lua_setfield(L, -2, infos[i].name);
    }

    return 1;
}

/*
** When implementing a single-threaded client, call this function periodically
** to allow processing of message retries and to send MQTT keepalive pings.
*/
static int mqttYield(lua_State *L)
{
    MQTTClient_yield();
    return 0;
}

/*
** Module entry point.
*/
LUALIB_API int luaopen_mqtt_core(lua_State *L)
{
    struct luaL_Reg *ptr;
    struct luaL_Reg funcs[] = {
        { "globalInit",     mqttGlobalInit     },
        { "getVersionInfo", mqttGetVersionInfo },
        { "yield",          mqttYield          },
        { NULL, NULL }
    };

    lua_newtable(L);

    do {
        lua_pushstring(L, ptr->name);
        lua_pushcfunction(L, ptr->func);
        lua_rawset(L, -3);
        ptr++;
    } while (ptr->name);

    return 1;
}
