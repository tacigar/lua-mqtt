--
-- lua-mqtt
-- Copyright (c) 2017 tacigar
--

local mqtt = require "mqtt"

local address = "tcp://localhost:1883"
local topic = "MQTT Examples"
local clientID = "ExampleClientSub"
local qos = 1

local function onConnectionLost()
	io.write("\nConnection lost\n")
end

local function onMessageArrived(topicName, message)
	io.write("Message arrived\n")
	io.write(string.format("  topic: %s\n", topicName))
	io.write(string.format("  message: %s\n", message.payload))
end

local client = mqtt.AsyncClient {
	serverURI = address,
	clientID  = clientID,
}
client:setCallbacks(onConnectionLost, onMessageArrived, nil)
client:connect{}

io.write(string.format("Subscribing to topic %s\n", topic))
io.write(string.format("for client %s using QoS%d\n\n", clientID, qos))
client:subscribe(topic, qos)

local c = ' '
while c ~= 'q' do
	c = io.read()
end

client:disconnect(100)
client:destroy()
