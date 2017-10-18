local ffi = require "ffi"

function SplitString(szFullString, szSeparator)
	local nFindStartIndex = 1  
	local nSplitIndex = 1  
	local nSplitArray = {}  
	while true do  
		local nFindLastIndex = string.find(szFullString, szSeparator, nFindStartIndex)  
		if not nFindLastIndex then  
			nSplitArray[nSplitIndex] = string.sub(szFullString, nFindStartIndex, string.len(szFullString))  
			break  
		end  
		nSplitArray[nSplitIndex] = string.sub(szFullString, nFindStartIndex, nFindLastIndex - 1)  
		nFindStartIndex = nFindLastIndex + string.len(szSeparator)  
		nSplitIndex = nSplitIndex + 1  
	end  
	return nSplitArray  
end

function SaveDebugLog(strLog)
	local msg = debug.traceback()
	ffi.C.SaveDebugLog_Lua(strLog .. "\n" .. msg)
end

function SaveAssertLog(strLog)
	local msg = debug.traceback()
	ffi.C.SaveAssertLog_Lua(strLog .. "\n" .. msg)
end