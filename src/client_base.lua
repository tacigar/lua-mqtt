--
-- lua-mqtt
-- Copyright (c) 2017 tacigar
--

local CoreClientBase = require "mqtt.core.ClientBase"
local Token = require "mqtt.Token"

local ClientBase = {}
ClientBase.__index = Client

setmetatable(ClientBase, {
	__call = function(_, options)
		local core = CoreClientBase.new(options.serverURI, options.clientID)
		local newClient = { _core = core }

		return setmetatable(newClient, ClientBase)
	end,
})

--- This function attempts to connect a previously-created client to an MQTT
-- server using the specified options.
function ClientBase:connect(connOpts)
	self._core:connect(connOpts)
end

--- This function attempts to connect a previously-created client to an MQTT
-- server using the specified options.
function ClientBase:disconnect(timeout)
	self._core:disconnect(timeout)
end

--- This function allows the client application to test whether or not a client
-- is currently connected to the MQTT server.
function ClientBase:isConnected()
	return self._core:isConnected()
end

--- This function attempts to subscribe a client to a single topic, which may
-- contain wildcards
function ClientBase:subscribe(topic, qos)
	if type(topic) == "table" then
		self._core:subscribeMany(topic)
	else
		self._core:subscribe(topic, qos)
	end
end

--- This function attempts to remove an existing subscription made by the
-- specified client.
function ClientBase:unsubscribe(topic)
	if type(topic) == "table" then
		self._core:unsubscribeMany(topic)
	else
		self._core:unsubscribe(topic)
	end
end

--- This function attempts to publish a message to a given topic. A token is
-- issued when this function returns successfully.
function ClientBase:publish(topicName, msg)
	if type(msg) == "string" then
		msg = { payload = msg }
	end
	return self._core:publish(topicName, msg)
end

--- This function frees the memory allocated to an MQTT client. It should be
-- called when the client is no longer required.
function ClientBase:destroy()
	self._core:destroy()
end

return ClientBase
