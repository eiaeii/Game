/******************************************************************************
* ע�⣺���� Lua ffi ��֧��ֻ֧�� C++��Ϊ�˷��㵼����Lua����Ҫ��Lua��ʹ�õĽṹ��ͺ�����һЩ�淶
*
* 1��������Ҫ��Lua��ʹ�õĽṹ����� ��C���ԡ� �ķ��
*	�磺
*
*	struct tagName
*	{
*		int a;			// ��ȷ
*		int a = 0;		// �����
*		tagName() {}	// �����
*		tagName *p;		// �����
*	};
*
*	��C++��ʹ��ʱע���ʼ��
*	�磺
*	tagName a = { 0 };
*
* 2��������Ҫ��Lua��ʹ�õĺ���������ʱ����ǰ����� LUA_API���ṹ��ǰҪ�� struct
*	�磺
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
