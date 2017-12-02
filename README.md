LUA MQTT
========

Lua bindings to the [paho.mqtt.c](https://github.com/eclipse/paho.mqtt.c) client library.

※ THIS LIBRARY NEEDS LUA DEFINED `lua_lock`, AND `lua_unlock`.

See [here](http://lua-users.org/wiki/ThreadsTutorial).

INSTALL
-------

Please enter the following command to build.

```sh
git clone https://github.com/tacigar/lua-mqtt.git
cd lua-mqtt
luarocks make PAHO_MQTT_INCDIR="<paho.mqtt.c incdir>" PAHO_MQTT_LIBDIR="<paho.mqtt.c libdir>"
```
