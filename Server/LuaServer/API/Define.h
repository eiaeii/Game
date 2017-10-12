#ifndef _LUASERVER_DEFINE_H
#define _LUASERVER_DEFINE_H

#ifdef _WIN32
	#define LUA_API extern "C" __declspec(dllexport)
#else
	#define LUA_API
#endif // _WIN32

#endif // !_LUASERVER_DEFINE_H

