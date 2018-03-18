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
-- ��һ���ַ���ת����һ����
-- ����: "{1,2,3}" => {1,2,3}
--       "{a=1,b=2,c=3}" => {a=1,b=2,c=3}
-- ��ע: TableToString�ķ������
-------------------------------------------------------------------
function StringToTable( str )
    if str == nil or type(str) ~= "string" then
        return
    end
    
    return loadstring("return " .. str)()
end

-- table�Ƿ�Ϊ��
function IsTableEmpty(t)
	return t == nil or _G.next(t) == nil
end

-------------------------------------------------------------------
-- ��һ��tableת��һ���ַ���
-- ����: {1,2,3} => "{1,2,3}"
--       {a=1,b=2,c=3} => "{a=1,b=2,c=3}"
-- ��ע: �ú�����������table.concat 
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
			-- ���Ӽ�ֵ

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
