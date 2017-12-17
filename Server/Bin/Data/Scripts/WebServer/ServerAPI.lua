local ffi = require "ffi"
ffi.cdef[[
	struct TestStruct API_TestStruct(struct TestStruct stIn)
]]