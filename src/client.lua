--
-- lua-mqtt
-- Copyright (c) 2017 tacigar
--

local ClientBase = require "mqtt.ClientBase"
local Token = require "mqtt.Token"
local TokenBase = require "mqtt.TokenBase"

local Client = {}
Client.__index = Client

setmetatable(Client, {
	__call = function(_, options)
		local base = ClientBase.new(options.serverURI, options.clientID)

		local newClient = { _base = base }

		return setmetatable(newClient, {
			__index = function(tbl, key)
				local val = rawget(Client, key)

				if val then
					return val
				end

				return function(tbl, ...)
					return tbl._base[key](tbl._base, ...)
				end
			end,
		})
	end
})

function Client:publish(topicName, msg)
	if type(msg) == "string" then
		msg = { payload = msg }
	end

	local tokenBase = self._base:publish(topicName, msg)
	return Token(tokenBase)
end

return Client
