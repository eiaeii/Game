local ffi = require "ffi"

function TestLog()
	SaveAssertLog("SaveAssertLog≤‚ ‘")
end

function TestAPI()
	local paramIn = ffi.new("struct TestStruct", {123, 456})
	local paramOut = ffi.new("struct TestStruct", {111, 222})
	local paramResult = ffi.C.API_TestStruct(paramIn, paramOut)
		
	print("=============paramOut=============")
	print(paramOut.a)
	print(paramOut.b)
	
	print("=============paramResult=============")
	print(paramResult.a)
	print(paramResult.b)
end

function TestP()
	local paramIn = ffi.new("struct TestStruct[3]", {{1,2},{1,2}})
	print(ffi.sizeof(paramIn))

	local paramOut = ffi.new("struct TestStruct[3]")
	local paramOutNum = ffi.new("int[1]")
	local paramResult = ffi.C.API_TestArray(paramIn, 3, paramOut, paramOutNum)
	
	print("=============paramOut=============")
	for i = 1, paramOutNum[0] do
		print(paramOut[i-1].a)
		print(paramOut[i-1].b)
	end
	
	print("=============paramResult=============")
	print(paramResult.a)
	print(paramResult.b)
end

function TestPointer()
	local paramIn = ffi.new("struct TestStruct", {1,2})
	
	local paramResult = ffi.C.API_TestPointer(paramIn, 3)
	
	print(paramResult)
end