function MyTest()
	--ServerAPI.MyAPITest()
	local msg = ffi.new("struct Msg_Goods_WEB2GB")
	msg.nGoodsID = 10086
	msg.nNum = 123
	ServerAPI.SendMsgToGBServer(msg)
	 
end