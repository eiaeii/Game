#include "Singleton.hpp"
#include <string>
#include <ctime>

#ifndef _TimeManager_HPP
#define _TimeManager_HPP

class CTimeManager : public CSingleton<CTimeManager>
{
public:
	unsigned long GetYYYYMMDD();
	
	std::string GetYYYYMMDDString();
	
	std::string GetYYYYMMDDHHMMSSString();
};

#endif // _TimeManager_HPP

