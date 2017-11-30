/* 
** lua-mqtt
** Copyright (C) 2017 tacigar
*/

#include <stdlib.h>
#include <string.h>
#include <MQTTClient.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "token.h"

#define MQTT_CLIENT_CLASS "mqtt.client"

/*
** Synchronous client class.
*/
typedef struct Client
{
    MQTTClient m_client;
} Client;

/*
** This function creates an MQTT client ready for connection to the specified 
** server and using the specified persistent storage.
*/
static int clientCreate(lua_State *L)
{
    int rc;
    const char *serverURI = luaL_checkstring(L, 1);
    const char *clientID = luaL_checkstring(L, 2);
    
    int persistenceType = MQTTCLIENT_PERSISTENCE_NONE;
    void *persistenceContext = NULL;

    Client *client = (Client *)lua_newuserdata(L, sizeof(Client));
    rc = MQTTClient_create(&(client->m_client), serverURI, clientID, 
                           persistenceType, persistenceContext);

    if (rc != MQTTCLIENT_SUCCESS) {
        lua_pushnil(L);
        lua_pushfstring(L, "cannot create a new client : %d", rc);
        return 2;
    }

    luaL_getmetatable(L, MQTT_CLIENT_CLASS);
    lua_setmetatable(L, -2);
    return 1;
}

/*
** This function attempts to connect a previously-created client to an MQTT 
** server using the specified options.
*/
static int clientConnect(lua_State *L)
{
    int rc;
    Client *client;
    client = (Client *)luaL_checkudata(L, 1, MQTT_CLIENT_CLASS);
    
    MQTTClient_connectOptions connOpts = MQTTClient_connectOptions_initializer;
    MQTTClient_SSLOptions sslOpts = MQTTClient_SSLOptions_initializer;
    MQTTClient_willOptions willOpts = MQTTClient_willOptions_initializer;

    lua_pushnil(L);
    while (lua_next(L, 2)) {
        const char *key = lua_tostring(L, -2);
        
        if (strcmp(key, "keepAliveInterval") == 0) {
            connOpts.keepAliveInterval = luaL_checkinteger(L, -1);
        } else if (strcmp(key, "cleanSession") == 0) {
            connOpts.cleansession = lua_toboolean(L, -1);
        } else if (strcmp(key, "reliable") == 0) {
            connOpts.reliable = lua_toboolean(L, -1);
        } else if (strcmp(key, "username") == 0) {
            connOpts.username = luaL_checkstring(L, -1);
        } else if (strcmp(key, "password") == 0) {
            connOpts.password = luaL_checkstring(L, -1);
        } else if (strcmp(key, "connectTimeout") == 0) {
            connOpts.connectTimeout = luaL_checkinteger(L, -1);
        } else if (strcmp(key, "retryInterval") == 0) {
            connOpts.retryInterval = luaL_checkinteger(L, -1);
        } else if (strcmp(key, "mqttVersion") == 0) {
            connOpts.MQTTVersion = luaL_checkinteger(L, -1);
        } else if (strcmp(key, "will") == 0) {
            lua_pushnil(L);
            while (lua_next(L, -1)) {
                const char *key = lua_tostring(L, -2);
                
                if (strcmp(key, "topicName") == 0) {
                    willOpts.topicName = luaL_checkstring(L, -1);
                } else if (strcmp(key, "message") == 0) {
                    willOpts.message = luaL_checkstring(L, -1);
                } else if (strcmp(key, "retained") == 0) {
                    willOpts.retained = lua_toboolean(L, -1);
                } else if (strcmp(key, "qos") == 0) {
                    willOpts.qos = luaL_checkinteger(L, -1);
                }
                lua_pop(L, 1);
            }
            connOpts.will = &willOpts;

        } else if (strcmp(key, "ssl") == 0) {
            lua_pushnil(L);
            while (lua_next(L, -1)) {
                const char *key = lua_tostring(L, -2);

                if (strcmp(key, "trustStore") == 0) {
                    sslOpts.trustStore = luaL_checkstring(L, -1);
                } else if (strcmp(key, "keyStore") == 0) {
                    sslOpts.keyStore = luaL_checkstring(L, -1);
                } else if (strcmp(key, "privateKey") == 0) {
                    sslOpts.privateKey = luaL_checkstring(L, -1);
                } else if (strcmp(key, "privateKeyPassword") == 0) {
                    sslOpts.privateKeyPassword = luaL_checkstring(L, -1);
                } else if (strcmp(key, "enabledCipherSuites") == 0) {
                    sslOpts.enabledCipherSuites = luaL_checkstring(L, -1);
                } else if (strcmp(key, "enableServerCertAuth") == 0) {
                    sslOpts.enableServerCertAuth = lua_toboolean(L, -1);
                }
                lua_pop(L, 1);
            }
            connOpts.ssl = &sslOpts;

        } else if (strcmp(key, "serverURIs") == 0) {
            int i;
            size_t len = lua_rawlen(L, -1);

            char **serverURIs = malloc(len * sizeof(char *));;
            connOpts.serverURIcount = len;
            
            for (i = 0; i < len; ++i) {
                lua_pushnumber(L, i + 1);
                lua_gettable(L, -2);

                const char *serverURI = luaL_checkstring(L, -1);
                char *buffer = (char *)malloc(sizeof(char) * (strlen(serverURI) + 1));
                strcpy(buffer, serverURI);
                
                serverURIs[i] = buffer;
                lua_pop(L, 1);
            }
            connOpts.serverURIs = serverURIs;
        } 
        lua_pop(L, 1);
    }

    rc = MQTTClient_connect(client->m_client, &connOpts);

    if (connOpts.serverURIs) {
        int i;
        for (i = 0; i < connOpts.serverURIcount; ++i) {
            free(connOpts.serverURIs[i]);
        }
        free((char **)connOpts.serverURIs);
    }

    lua_pushnumber(L, rc);
    return 1;
}

/*
** This function attempts to disconnect the client from the MQTT server.
*/
static int clientDisconnect(lua_State *L)
{
    int rc;
    Client *client = (Client *)luaL_checkudata(L, 1, MQTT_CLIENT_CLASS);
    int timeout = luaL_checkinteger(L, 2);

    rc = MQTTClient_disconnect(client->m_client, timeout);

    lua_pushnumber(L, rc);
    return 1;
}

/*
** This function allows the client application to test whether or not a client
** is currently connected to the MQTT server.
*/
static int clientIsConnected(lua_State *L)
{
    int res;
    Client *client = (Client *)luaL_checkudata(L, 1, MQTT_CLIENT_CLASS);

    res = MQTTClient_isConnected(client->m_client);
 
    lua_pushboolean(L, res);
    return 1;
}

/*
** This function attempts to publish a message to a given topic.
*/
static int clientPublish(lua_State *L)
{
    int rc;
    Client *client = (Client *)luaL_checkudata(L, 1, MQTT_CLIENT_CLASS);
    const char *topicName = luaL_checkstring(L, 2);
    MQTTClient_message msg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken dt;
    char *buffer;

    lua_pushnil(L);
    while (lua_next(L, 3)) {
        const char *key = luaL_checkstring(L, -2);

        if (strcmp(key, "payload") == 0) {
            int i;

            const char *payload = luaL_checkstring(L, -1);
            buffer = (char *)malloc(sizeof(char) * (strlen(payload) + 1));

            msg.payloadlen = strlen(payload) + 1;
            strcpy(buffer, payload);

            msg.payload = (void *)payload;

        } else if (strcmp(key, "qos") == 0) {
            msg.qos = luaL_checkinteger(L, -1);
        } else if (strcmp(key, "retained") == 0) {
            msg.retained = lua_toboolean(L, -1);
        } else if (strcmp(key, "duplicate") == 0) {
            msg.dup = lua_toboolean(L, -1);
        }
        lua_pop(L, 1);
    }

    MQTTClient_publishMessage(client->m_client, topicName, &msg, &dt);
    if (buffer) {
        free(buffer);
    }

    tokenCreate(L, client->m_client, dt);
    return 1;
}


/*
** Module entry point.
*/
LUALIB_API int luaopen_mqtt_Client(lua_State *L)
{
    struct luaL_Reg *ptr;
    struct luaL_Reg methods[] = {
        { "connect",      clientConnect     },
        { "disconnect",   clientDisconnect  },
        { "isConnected",  clientIsConnected },
        { "publish",      clientPublish     },
        { NULL, NULL }
    };

    luaL_newmetatable(L, MQTT_CLIENT_CLASS);

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

    lua_newtable(L);
    lua_pushstring(L, "new");
    lua_pushcfunction(L, clientCreate);
    lua_rawset(L, -3);

    return 1;
}
