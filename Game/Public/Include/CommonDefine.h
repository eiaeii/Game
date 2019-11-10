#ifndef _CommonDefine_H
#define _CommonDefine_H

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
using HANDLE = void*;

#endif // _WINDOWS

#endif // _CommonDefine_H

