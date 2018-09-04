#pragma once
#ifndef UTILS_SERVERINFO_HPP
#define UTILS_SERVERINFO_HPP

#include <stdio.h>
#include <string>
#include <time.h>
#include <stdint.h>
#include "Common/CrossPlatform.hpp"
class MasterServer;

class ServerInfo {
private:
	static clock_t appstart;
	static bool initDone;
public:
	static time_t StartupTime;
	static int processID;
	static bool bRunning;
	static const char* baseTitle;
	static void init();
	static long long uptime();
	static float uptime_normalized();
	static long long time_gone(long long uptime);
	static float CompareTimes(long long startTime, long long endTime);
	static std::string getComputerName();
	static std::string getOsName();
	static MasterServer * masterServer;
	static std::string gameVersion;
	static void numericGameVersion(uint16_t * major, uint16_t * current, uint16_t * minor);
};
#endif