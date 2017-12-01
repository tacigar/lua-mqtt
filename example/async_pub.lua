--
-- lua-mqtt
-- Copyright (c) 2017 tacigar
--

local mqtt = require "mqtt"

local address = "tcp://localhost:1883"
local topic = "MQTT Examples"
local clientID = "ExampleClientPub"
local qos = 1
local payload = "Hello World!"

local deliveredToken

local function onConnectionLost()
	io.write("\nConnection lost\n")
end

local function onMessageArrived(topicName, message)
	io.write("Message arrived\n")
	io.write(string.format("  topic: %s\n", topicName))
	io.write(string.format("  message: %s\n", message.payload))
end

local function onDeliveryComplete(token)
	io.write(string.format("Message with token value %d delivery confirmed\n",
		token:getValue()))
	deliveredToken = token:getValue()
end

local client = mqtt.AsyncClient {
	serverURI = address,
	clientID  = clientID,
}
client:setCallbacks(onConnectionLost, onMessageArrived, onDeliveryComplete)
client:connect{}

deliveredToken = 0
local token = client:publish(topic, {
	payload = payload,
	qos = qos,
	retained = false,
})

io.write(string.format("Waiting for publication of %s\n", payload))
io.write(string.format("on topic %s for client with ClientID: %s\n",
	topic, clientID))
while deliveredToken ~= token:getValue() do
	-- Wait ...
end

client:disconnect(100)
client:destroy()
