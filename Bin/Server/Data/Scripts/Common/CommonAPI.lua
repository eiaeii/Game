local ffi = require "ffi"
ffi.cdef[[
	void SaveDebugLog_Lua(const char *pszLog);
	
	void SaveAssertLog_Lua(const char *pszLog);
	
	// 测试结构体
	struct TestStruct API_TestStruct(struct TestStruct stIn, struct TestStruct stOut);
	
	// 测试指针
	struct TestStruct API_TestArray(struct TestStruct *stIn, int nIn, struct TestStruct *stOut, int *nOut);
	int API_TestPointer(struct TestStruct *stIn, int nIn);
]]