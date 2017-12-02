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
local timeout = 10000

local client = mqtt.Client {
	serverURI = address,
	clientID  = clientID,
}
client:connect{}

local token = client:publish(topic, {
	payload = payload,
	qos = qos,
	retained = false,
})
io.write(string.format("Waiting for up to %d seconds for publication of %s\n",
	timeout / 1000, payload))
io.write(string.format("on topic %s for client with ClientID: %s\n", topic, clientID))
token:wait(timeout)
io.write(string.format("Message with delivery token %d delivered\n", token:getValue()))

client:disconnect(100)
client:destroy()
