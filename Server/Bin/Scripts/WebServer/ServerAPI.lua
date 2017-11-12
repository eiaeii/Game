local ffi = require "ffi"
ffi.cdef[[
	struct TestStruct TestAPI(struct TestStruct stIn)
]]