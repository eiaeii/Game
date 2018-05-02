local ffi = require "ffi"
ffi.cdef[[
	void SaveDebugLog_Lua(const char *pszLog);
	
	void SaveAssertLog_Lua(const char *pszLog);
	
	// ���Խṹ��
	struct TestStruct API_TestStruct(struct TestStruct stIn, struct TestStruct stOut);
	
	// ����ָ��
	struct TestStruct API_TestArray(struct TestStruct *stIn, int nIn, struct TestStruct *stOut, int *nOut);
	int API_TestPointer(struct TestStruct *stIn, int nIn);
]]