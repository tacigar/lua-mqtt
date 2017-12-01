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

#define MQTT_CLIENT_BASE_CLASS "mqtt.ClientBase"

/*
** Client class.
*/
typedef struct ClientBase
{
    MQTTClient m_client;
    lua_State *m_L;
    int m_onMessageArrived;
    int m_onDeliveryComplete;
    int m_onConnectionLost;
} ClientBase;

/*
** This is a callback function. The client application must provide an
** implementation of this function to enable asynchronous receipt of messages.
*/
static int onMessageArrivedCB(void *context, char *topicName,
                              int topicLen, MQTTClient_message *message)
{
    int res;
    ClientBase *client = (ClientBase *)context;

    lua_rawgeti(client->m_L, LUA_REGISTRYINDEX, client->m_onMessageArrived);
    if (topicLen == 0) {
        lua_pushstring(client->m_L, topicName);
    } else {
        lua_pushlstring(client->m_L, topicName, topicLen);
    }

    lua_newtable(client->m_L);
    lua_pushlstring(client->m_L, message->payload, message->payloadlen);
    lua_setfield(client->m_L, -2, "payload");
    lua_pushnumber(client->m_L, message->qos);
    lua_setfield(client->m_L, -2, "qos");
    lua_pushboolean(client->m_L, message->retained);
    lua_setfield(client->m_L, -2, "retained");
    lua_pushboolean(client->m_L, message->dup);
    lua_setfield(client->m_L, -2, "duplicate");

    lua_call(client->m_L, 2, 0);

    return 1;
}

/*
** This is a callback function. The client application must provide an
** implementation of this function to enable asynchronous notification of
** delivery of messages.
*/
static void onDeliveryCompleteCB(void *context, MQTTClient_deliveryToken dt)
{
    ClientBase *client = (ClientBase *)context;

    lua_rawgeti(client->m_L, LUA_REGISTRYINDEX, client->m_onDeliveryComplete);
    tokenCreate(client->m_L, client->m_client, dt);

    lua_call(client->m_L, 1, 0);
}

/*
** This is a callback function. The client application must provide an
** implementation of this function to enable asynchronous notification of the
** loss of connection to the server.
*/
static void onConnectionLostCB(void *context, char *cause)
{
    ClientBase *client = (ClientBase *)context;

    lua_rawgeti(client->m_L, LUA_REGISTRYINDEX, client->m_onConnectionLost);
    lua_pushstring(client->m_L, cause);

    lua_call(client->m_L, 1, 0);
}

/*
** This function sets the callback functions for a specific client.
*/
static int clientBaseSetCallbacks(lua_State *L)
{
    int rc;
    ClientBase *client = (ClientBase *)luaL_checkudata(L, 1, MQTT_CLIENT_BASE_CLASS);
    MQTTClient_connectionLost *onConnectionLost = NULL;
    MQTTClient_messageArrived *onMessageArrived = NULL;
    MQTTClient_deliveryComplete *onDeliveryComplete = NULL;

    if (lua_type(L, 2) == LUA_TFUNCTION) {
        lua_pushvalue(L, 2);
        client->m_onConnectionLost = luaL_ref(L, LUA_REGISTRYINDEX);
        onConnectionLost = onConnectionLostCB;
    }
    if (lua_type(L, 3) == LUA_TFUNCTION) {
        lua_pushvalue(L, 3);
        client->m_onMessageArrived = luaL_ref(L, LUA_REGISTRYINDEX);
        onMessageArrived = onMessageArrivedCB;
    }
    if (lua_type(L, 4) == LUA_TFUNCTION) {
        lua_pushvalue(L, 4);
        client->m_onDeliveryComplete = luaL_ref(L, LUA_REGISTRYINDEX);
        onDeliveryComplete = onDeliveryCompleteCB;
    }

    rc = MQTTClient_setCallbacks(client->m_client, client, onConnectionLost,
                                 onMessageArrived, onDeliveryComplete);

    lua_pushnumber(L, rc);
    return 1;
}

/*
** This function creates an MQTT client ready for connection to the specified
** server and using the specified persistent storage.
*/
static int clientBaseCreate(lua_State *L)
{
    int rc;
    const char *serverURI = luaL_checkstring(L, 1);
    const char *clientID = luaL_checkstring(L, 2);

    int persistenceType = MQTTCLIENT_PERSISTENCE_NONE;
    void *persistenceContext = NULL;

    ClientBase *client = (ClientBase *)lua_newuserdata(L, sizeof(ClientBase));
    rc = MQTTClient_create(&(client->m_client), serverURI, clientID,
                           persistenceType, persistenceContext);
    client->m_L = L;

    if (rc != MQTTCLIENT_SUCCESS) {
        lua_pushnil(L);
        lua_pushfstring(L, "cannot create a new client : %d", rc);
        return 2;
    }

    luaL_getmetatable(L, MQTT_CLIENT_BASE_CLASS);
    lua_setmetatable(L, -2);
    return 1;
}

/*
** This function attempts to connect a previously-created client to an MQTT
** server using the specified options.
*/
static int clientBaseConnect(lua_State *L)
{
    int rc;
    ClientBase *client;
    char **serverURIs = NULL;

    MQTTClient_connectOptions connOpts = MQTTClient_connectOptions_initializer;
    MQTTClient_SSLOptions sslOpts = MQTTClient_SSLOptions_initializer;
    MQTTClient_willOptions willOpts = MQTTClient_willOptions_initializer;

    client = (ClientBase *)luaL_checkudata(L, 1, MQTT_CLIENT_BASE_CLASS);

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

            serverURIs = malloc(len * sizeof(char *));;
            connOpts.serverURIcount = len;

            for (i = 0; i < len; ++i) {
                lua_pushnumber(L, i + 1);
                lua_gettable(L, -2);

                const char *serverURI = luaL_checkstring(L, -1);
                serverURIs[i] =
                    (char *)malloc(sizeof(char) * (strlen(serverURI) + 1));
                strcpy(serverURIs[i], serverURI);

                lua_pop(L, 1);
            }
            connOpts.serverURIs = serverURIs;
        }
        lua_pop(L, 1);
    }

    rc = MQTTClient_connect(client->m_client, &connOpts);

    if (connOpts.serverURIs != NULL) {
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
static int clientBaseDisconnect(lua_State *L)
{
    int rc;
    ClientBase *client = (ClientBase *)luaL_checkudata(L, 1, MQTT_CLIENT_BASE_CLASS);
    int timeout = luaL_checkinteger(L, 2);

    rc = MQTTClient_disconnect(client->m_client, timeout);

    lua_pushnumber(L, rc);
    return 1;
}

/*
** This function allows the client application to test whether or not a client
** is currently connected to the MQTT server.
*/
static int clientBaseIsConnected(lua_State *L)
{
    int res;
    ClientBase *client = (ClientBase *)luaL_checkudata(L, 1, MQTT_CLIENT_BASE_CLASS);

    res = MQTTClient_isConnected(client->m_client);

    lua_pushboolean(L, res);
    return 1;
}

/*
** This function attempts to subscribe a client to a single topic, which may
** contain wildcards (see Subscription wildcards).
*/
static int clientBaseSubscribe(lua_State *L)
{
    int rc;
    ClientBase *client = (ClientBase *)luaL_checkudata(L, 1, MQTT_CLIENT_BASE_CLASS);
    const char *topic = luaL_checkstring(L, 2);
    int qos = luaL_checkinteger(L, 3);

    rc = MQTTClient_subscribe(client->m_client, topic, qos);
    lua_pushnumber(L, rc);

    return 1;
}

/*
** This function attempts to subscribe a client to a list of topics, which may
** contain wildcards.
*/
static int clientBaseSubscribeMany(lua_State *L)
{
    int i;
    int rc;
    ClientBase *client = (ClientBase *)luaL_checkudata(L, 1, MQTT_CLIENT_BASE_CLASS);

    size_t len = lua_rawlen(L, 2);

    int *qoss = malloc(sizeof(int) * len);
    char **topics = malloc(sizeof(char *) * len);

    for (i = 0; i < len; ++i) {
        lua_pushnumber(L, i + 1);
        lua_gettable(L, -2);

        lua_pushnil(L);
        while (lua_next(L, -2)) {
            const char *key = lua_tostring(L, -2);

            if (strcmp(key, "topic") == 0) {
                const char *topic = luaL_checkstring(L, -1);
                topics[i] = (char *)malloc(sizeof(char) * (strlen(topic) + 1));
                strcpy(topics[i], topic);
            } else if (strcmp(key, "qos") == 0) {
                qoss[i] = luaL_checkinteger(L, -1);
            }
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }

    rc = MQTTClient_subscribeMany(client->m_client, len, topics, qoss);
    lua_pushnumber(L, rc);

    for (i = 0; i < len; ++i) {
        free(topics[i]);
    }
    free(topics);
    free(qoss);

    return 1;
}

/*
** This function attempts to remove an existing subscription made by the
** specified client.
*/
static int clientBaseUnsubscribe(lua_State *L)
{
    int rc;
    ClientBase *client = (ClientBase *)luaL_checkudata(L, 1, MQTT_CLIENT_BASE_CLASS);
    const char *topic = luaL_checkstring(L, 2);

    rc = MQTTClient_unsubscribe(client->m_client, topic);
    lua_pushnumber(L, rc);

    return 1;
}

/*
** This function attempts to remove existing subscriptions to a list of topics
** made by the specified client.
*/
static int clientBaseUnsubscribeMany(lua_State *L)
{
    int i;
    int rc;
    ClientBase *client = (ClientBase *)luaL_checkudata(L, 1, MQTT_CLIENT_BASE_CLASS);

    size_t len = lua_rawlen(L, 2);

    char **topics = malloc(sizeof(char *) * len);

    for (i = 0; i < len; ++i) {
        lua_pushnumber(L, i + 1);
        lua_gettable(L, -2);

        const char *topic = luaL_checkstring(L, -1);
        topics[i] = (char *)malloc(sizeof(char) * (strlen(topic) + 1));
        strcpy(topics[i], topic);

        lua_pop(L, 1);
    }

    rc = MQTTClient_unsubscribeMany(client->m_client, len, topics);
    lua_pushnumber(L, rc);

    for (i = 0; i < len; ++i) {
        free(topics[i]);
    }

    return 1;
}

/*
** This function attempts to publish a message to a given topic.
*/
static int clientBasePublish(lua_State *L)
{
    int rc;
    ClientBase *client = (ClientBase *)luaL_checkudata(L, 1, MQTT_CLIENT_BASE_CLASS);
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
** This function frees the memory allocated to an MQTT client
*/
static int clientBaseDestroy(lua_State *L)
{
    ClientBase *client = (ClientBase *)luaL_checkudata(L, 1, MQTT_CLIENT_BASE_CLASS);
    MQTTClient_destroy(client->m_client);
    return 0;
}

/*
** Module entry point.
*/
LUALIB_API int luaopen_mqtt_core_ClientBase(lua_State *L)
{
    struct luaL_Reg *ptr;
    struct luaL_Reg methods[] = {
        { "setCallbacks",    clientBaseSetCallbacks    },
        { "connect",         clientBaseConnect         },
        { "disconnect",      clientBaseDisconnect      },
        { "isConnected",     clientBaseIsConnected     },
        { "subscribe",       clientBaseSubscribe       },
        { "subscribeMany",   clientBaseSubscribeMany   },
        { "unsubscribe",     clientBaseUnsubscribe     },
        { "unsubscribeMany", clientBaseUnsubscribeMany },
        { "publish",         clientBasePublish         },
        { "destroy",         clientBaseDestroy         },
        { NULL, NULL }
    };

    luaL_newmetatable(L, MQTT_CLIENT_BASE_CLASS);

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
    lua_pushcfunction(L, clientBaseCreate);
    lua_rawset(L, -3);

    return 1;
}
