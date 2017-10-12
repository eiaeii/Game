local ffi = require "ffi"
ServerAPI = ffi.load("LuaServer")
ffi.cdef[[
	void MyAPITest();
]]
