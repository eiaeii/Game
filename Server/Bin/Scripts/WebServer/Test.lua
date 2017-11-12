local ffi = require "ffi"

function TestLog()
	SaveDebugLog("SaveDebugLog≤‚ ‘")
	SaveAssertLog("SaveAssertLog≤‚ ‘")
end

function TestAPI()
	local paramIn = ffi.new("struct TestStruct", 1)
	paramIn.a = 123
	paramIn.b = 456
	local paramOut = ffi.C.TestAPI(paramIn)
	print(paramOut.a)
	print(paramOut.b)
end