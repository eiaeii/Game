local ffi = require "ffi"

function MyTest()
	ffi.C.MyAPITest() 
	local msg = ffi.new("struct Msg_Goods_WEB2GB")
	msg.nGoodsID = 10086
	msg.nNum = 123
	ffi.C.SendMsgToGBServer(msg)
	
end