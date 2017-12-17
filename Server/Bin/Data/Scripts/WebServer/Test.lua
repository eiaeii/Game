local ffi = require "ffi"

function TestLog()
	SaveAssertLog("SaveAssertLog≤‚ ‘")
end

function TestAPI()
	local paramIn = ffi.new("struct TestStruct", 1)
	paramIn.a = 123
	paramIn.b = 456
	local paramOut = ffi.C.API_TestStruct(paramIn)
	print(paramOut.a)
	print(paramOut.b)
end