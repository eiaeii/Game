local ffi = require "ffi"

ffi.cdef[[

struct Msg_Goods_WEB2GB
{
	long long nGoodsID;
	int nNum;
};

]]