--
-- lua-mqtt
-- Copyright (c) 2017 tacigar
--

local ClientBase = require "mqtt.ClientBase"
local CoreClientBase = require "mqtt.core.ClientBase"

--- Async client class.
local AsyncClient = {}
AsyncClient.__index = AsyncClient

setmetatable(AsyncClient, {
	--- Creates a new async client.
	__call = function(_, options)
		return setmetatable(ClientBase(options), AsyncClient)
	end,
	__index = ClientBase,
})

--- This function sets the callback functions for a specific client. If your
-- client application doesn't use a particular callback, set the relevant
-- parameter to nil.
function AsyncClient:setCallbacks(onConnectionLostCB, onMessageArrivedCB,
	onDeliveryCompleteCB)
	if type(onConnectionLostCB) == "table" then
		local tbl = onConnectionLostCB
		self._core:setCallbacks(tbl.onConnectionLost, tbl.onMessageArrived,
			tbl.onDeliveryComplete)
	else
		self._core:setCallbacks(onConnectionLostCB, onMessageArrivedCB,
			onDeliveryCompleteCB)
	end
end

return AsyncClient
