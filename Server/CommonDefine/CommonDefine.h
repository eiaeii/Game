#ifndef _CommonDefine_H
#define _CommonDefine_H

#ifndef SAFE_DELETE
#define SAFE_DELETE(x)	do { delete (x); (x) = nullptr; } while(false)
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) do { delete[] (x); (x) = nullptr; } while(false)
#endif

#ifdef _WIN32
	#define SERVER_API __declspec(dllexport)
#else
	#define SERVER_API
#endif _WIN32

#endif // _CommonDefine_H

