#ifndef _CommonDefine_H
#define _CommonDefine_H

static_assert(sizeof(void*) == 8, "该架构只支持64位平台");

#ifndef SAFE_DELETE
#define SAFE_DELETE(x)	do { delete (x); (x) = nullptr; } while(false)
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) do { delete[] (x); (x) = nullptr; } while(false)
#endif

#ifdef _WINDOWS
#define SERVER_API extern "C" __declspec(dllexport)
#include <Windows.h>

#else
#define SERVER_API extern "C" __attribute__((visibility("default")))
#endif // _WINDOWS

#endif // _CommonDefine_H

