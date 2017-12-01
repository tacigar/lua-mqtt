--
-- lua-mqtt
-- Copyright (c) 2017 tacigar
--

local ClientBase = require "mqtt.ClientBase"
local CoreClientBase = require "mqtt.core.ClientBase"

--- Sync client class.
local Client = {}
Client.__index = Client

setmetatable(Client, {
	--- Creates a new sync client.
	__call = function(_, options)
		return setmetatable(ClientBase(options), Client)
	end,
	__index = ClientBase,
})

return Client
