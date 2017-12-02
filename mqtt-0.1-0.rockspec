package = "mqtt"
version = "0.1-0"

source = {
  url = "git://github.com/tacigar/lua-mqtt",
  tag = "v0.1-0",
}

description = {
  summary = "A MQTT Client Library",
  detailed = [[
    A MQTT Client Library
  ]],
  license = "GPLv3",
}

build = {
  type = "builtin",
  modules = {
    ["mqtt.core.ClientBase"] = {
      sources = { "src/client_base.c", "src/token.c", "src/message.c" },
      incdirs = { "/src", "$(PAHO_MQTT_INCDIR)" },
      libdirs = { "$(PAHO_MQTT_LIBDIR)" },
      libraries = { "paho-mqtt3c" },
    },
    ["mqtt.Token"] = {
      sources = { "src/token.c" },
      incdirs = { "/src", "$(PAHO_MQTT_INCDIR)" },
      libdirs = { "$(PAHO_MQTT_LIBDIR)" },
      libraries = { "paho-mqtt3c" },
    },
    ["mqtt.core"] = {
      sources = { "src/mqtt.c" },
      incdirs = { "/src", "$(PAHO_MQTT_INCDIR)" },
      libdirs = { "$(PAHO_MQTT_LIBDIR)" },
      libraries = { "paho-mqtt3c" },
    },
    ["mqtt"] = "src/init.lua",
    ["mqtt.ClientBase"] = "src/client_base.lua",
    ["mqtt.Client"] = "src/client.lua",
    ["mqtt.AsyncClient"] = "src/async_client.lua",
  },
}
