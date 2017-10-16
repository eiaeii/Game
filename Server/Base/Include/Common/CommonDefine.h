#ifndef _CommonDefine_H
#define _CommonDefine_H

#ifndef SAFE_DELETE
#define SAFE_DELETE(x)	do { delete (x); (x) = nullptr; } while(false)
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) do { delete[] (x); (x) = nullptr; } while(false)
#endif

#ifdef __cplusplus
#ifdef _WIN32
#define SERVER_API extern "C" __declspec(dllexport)
#else
#define SERVER_API extern "C"
#endif // _WIN32
#else
#ifdef _WIN32
#define SERVER_API __declspec(dllexport)
#else
#define SERVER_API
#endif // _WIN32
#endif // _cplusplus

#endif // _CommonDefine_H

