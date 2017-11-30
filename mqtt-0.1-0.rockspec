package = "mqtt"
version = "0.1-0"

source = {
  url = "git://github.com/tacigar/lua-mqtt",
  tag = "v0.1-0",
}

description = {
  summary = "A MQTT Client Library",
  detailed = [[
    
  ]],
  license = "GPLv3",
}

build = {
  type = "builtin",
  modules = {
    ["mqtt.ClientBase"] = {
      sources = { "src/client_base.c", "src/token_base.c" },
      incdirs = {
        "/src",
        "$(PAHO_MQTT_INCDIR)",        
      },
      libdirs = {
        "$(PAHO_MQTT_LIBDIR)",
      },
      libraries = {
        "paho-mqtt3c",
      },
    },
    ["mqtt.TokenBase"] = {
      sources = { "src/token_base.c" },
      incdirs = {
        "/src",
        "$(PAHO_MQTT_INCDIR)",
      },
      libdirs = {
        "$(PAHO_MQTT_LIBDIR)",
      },
      libraries = {
        "paho-mqtt3c",
      },
    },
    ["mqtt.Client"] = "src/client.lua",
    ["mqtt.Token"] = "src/token.lua",
  },
}