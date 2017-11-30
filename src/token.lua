--
-- lua-mqtt
-- Copyright (c) 2017 tacigar
--

local TokenBase = require "mqtt.TokenBase"

local Token = {}
Token.__index = Token

local DEFAULT_TIMEOUT = 30

setmetatable(Token, {
	__call = function(_, base)
		local token = { _base = base }

		return setmetatable(token, {
			__index = function(tbl, key)
				local val = rawget(Token, key)

				if val then
					return val
				end

				return function(tbl, ...)
					return tbl._base[key](tbl._base, ...)
				end
			end,

			__call = function(tbl, timeout)
				tbl:wait(timeout)
			end,
		})
	end,
})

function Token:wait(timeout)
	timeout = timeout or DEFAULT_TIMEOUT
	self._base:wait(timeout)
end

return Token
