local ffi = require "ffi"

function SaveDebugLog(strLog)
	local msg = debug.traceback()
	ffi.C.SaveDebugLog_Lua(strLog .. "\n" .. msg)
end

function SaveAssertLog(strLog)
	local msg = debug.traceback()
	ffi.C.SaveAssertLog_Lua(strLog .. "\n" .. msg)
end

-------------------------------------------------------------------
-- 把一个字符串转换成一个表
-- 例如: "{1,2,3}" => {1,2,3}
--       "{a=1,b=2,c=3}" => {a=1,b=2,c=3}
-- 备注: TableToString的反向过程
-------------------------------------------------------------------
function StringToTable( str )
    if str == nil or type(str) ~= "string" then
        return
    end
    
    return loadstring("return " .. str)()
end

-- table是否为空
function IsTableEmpty(t)
	return t == nil or _G.next(t) == nil
end

-------------------------------------------------------------------
-- 把一个table转成一个字符串
-- 例如: {1,2,3} => "{1,2,3}"
--       {a=1,b=2,c=3} => "{a=1,b=2,c=3}"
-- 备注: 该函数功能类似table.concat 
-------------------------------------------------------------------
function TostringEx(value)
	if type(value)=='table' then
	   return TableToString(value)
	elseif type(value)=='string' then
		return "\""..value.."\""
	else
	   return tostring(value)
	end
end

function TableToString(t)
	if IsTableEmpty(t) then
		return "{}"
	end

	local retstr= "{"

	local i = 1
	for key,value in pairs(t) do
	    local signal = ","
	    if i==1 then
          signal = ""
		end

		if key==i then
			-- 不加键值

			retstr = retstr..signal..TostringEx(value)
		else
			if type(key)=='number' then
				retstr = retstr..signal..'['..key.."]="..TostringEx(value)
			else
				retstr = retstr..signal..key.."="..TostringEx(value)
			end
		end

		i = i+1
	end

 	retstr = retstr.."}"
 	return retstr
end

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
