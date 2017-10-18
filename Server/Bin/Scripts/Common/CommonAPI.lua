local ffi = require "ffi"
ffi.cdef[[
	void SaveDebugLog_Lua(const char *pszLog);
	void SaveAssertLog_Lua(const char *pszLog);
]]