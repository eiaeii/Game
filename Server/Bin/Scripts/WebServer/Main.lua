ffi = require "ffi"
ServerAPI = ffi.load("LuaServer")

function Main()
	require "Common"
	require "test"
end

Main()

MyTest()