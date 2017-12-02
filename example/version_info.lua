--
-- lua-mqtt
-- Copyright (c) 2017 tacigar
--

local mqtt = require "mqtt"

local versionInfo = mqtt.getVersionInfo()

for k, v in pairs(versionInfo) do
	io.write(string.format("%14s : %s\n", k, v))
end
