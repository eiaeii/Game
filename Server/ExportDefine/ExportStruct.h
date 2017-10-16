/******************************************************************************
* 注意：由于 Lua ffi 不支持只支持 C++，为了方便导出到Lua，需要在Lua中使用的结构体和函数做一些规范
*
* 1、所有需要在Lua中使用的结构体采用 【C语言】 的风格
*	如：
*
*	struct tagName
*	{
*		int a;			// 正确
*		int a = 0;		// 错误的
*		tagName() {}	// 错误的
*		tagName *p;		// 错误的
*	};
*
*	在C++中使用时注意初始化
*	如：
*	tagName a = { 0 };
*
* 2、所有需要在Lua中使用的函数，声明时在最前面加上 LUA_API，结构体前要加 struct
*	如：
*
*	LUA_API void MyFunction(struct tagName a);
*
*******************************************************************************/

#ifndef _EXPORTSTRUCT_H
#define _EXPORTSTRUCT_H

// Export Begin

struct Msg_Goods_WEB2GB
{
	long long nGoodsID;
	int nNum;
};

// Export End

#endif // !_EXPORTSTRUCT_H
