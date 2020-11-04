#define HOST_ENDIAN_IS_BIG
#define BIG_ENDIAN

#include "Common/HardConfig.hpp"

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <thread>

#include "Common/CrossPlatform.hpp"

#include <RakNet/RakSleep.h>

#include "Server/MasterServer.hpp"
#include "Server/AuthServer.hpp"
#include "Server/WorldServer.hpp"
#include "Server/Bridges/BridgeMasterServer.hpp"
#include "GameCache/Interface/FastDatabase.hpp"
#include "DataTypes/AMF3.hpp"

#include "Utils/FileUtils.hpp"
#include "Utils/ServerInfo.hpp"
#include "Utils/StringUtils.hpp"
#include <chrono>

#include "Configuration/ConfigurationManager.hpp"
#include "Database/Database.hpp"
//#include "Server/UGCServer.hpp"

using namespace std::chrono;

std::vector<ILUServer *> virtualServerInstances;

enum class SERVERMODE : uint8_t { STANDALONE, MASTER, WORLD, AUTH, UGCOP } MODE_SERVER;

GameCache::Interface::FDB::Connection Cache;
BridgeMasterServer* masterServerBridge;

int givenWorldID = 2000;


// Following Includes are for testing
#include "FileTypes/LUZFile/LUZone.hpp"
#include "Entity/GameObject.hpp"
#include "GameCache/WorldConfig.hpp"
#include "Entity/Components/StatsComponent.hpp"
#include "DataTypes/LDF.hpp"
#include "Database/CacheImporter.hpp"

#include "Server/Manager/WorldInstanceManager.hpp"

#include <iostream>
#include <vector>

WorldServer* viewWs;
AuthServer* authServer = nullptr;


int main(int argc, char* argv[]) {
	FileUtils::ChangeDirectory();
	Configuration::ConfigurationManager::Load();
	ServerInfo::init();

	std::string ipMaster = Configuration::ConfigurationManager::generalConf.GetStringVal("Master", "MASTERIP");
	ServerInfo::SetAuthIP(Configuration::ConfigurationManager::generalConf.GetStringVal("Master", "AUTHIP"));

	Logger::log("MAIN", "Connecting to database...");
	Database::Connect();

	Logger::log("MAIN", "Connecting game cache...");
	Cache.Connect("./res/cdclient.fdb");

	using namespace Entity;

	MODE_SERVER = SERVERMODE::STANDALONE;

	Logger::log("MAIN", "Booting up server instances...");

#ifdef OPCRUX_PLATFORM_WIN32
	std::system("title OpCrux Server (Standalone)");
#endif
	for (std::ptrdiff_t i = 0; i < argc; i++) {
		std::string arg = std::string(argv[i]);
		if (arg == "--master") {
			MODE_SERVER = SERVERMODE::MASTER;
#ifdef OPCRUX_PLATFORM_WIN32
			std::system("title OpCrux Server (Master only)");
#endif
		}
		else if (arg == "--world") {
			MODE_SERVER = SERVERMODE::WORLD;
#ifdef OPCRUX_PLATFORM_WIN32
			std::system("title OpCrux Server (World only)");
#endif
		}
		else if (arg == "--auth") {
			MODE_SERVER = SERVERMODE::AUTH;
#ifdef OPCRUX_PLATFORM_WIN32
			std::system("title OpCrux Server (Auth only)");
#endif
		}
		else if (arg == "--worldID" && i < argc) {
			givenWorldID = std::stoi(argv[i + 1]);
		}

		if (argc >= 4) {
			ipMaster = argv[i + 1];
		}
	}

	if (MODE_SERVER == SERVERMODE::STANDALONE || MODE_SERVER == SERVERMODE::MASTER) {
		std::thread mT([](MasterServer* ms) { ms = new MasterServer(); }, ServerInfo::masterServer);
		mT.detach();
	}

	if (MODE_SERVER == SERVERMODE::STANDALONE || MODE_SERVER != SERVERMODE::MASTER) {
		masterServerBridge = new BridgeMasterServer(ipMaster);
		masterServerBridge->Connect();
		masterServerBridge->Listen();
	}

	if (MODE_SERVER == SERVERMODE::STANDALONE || MODE_SERVER == SERVERMODE::AUTH) {
		std::thread aT([](AuthServer ** as) { new AuthServer(); }, &authServer);
		aT.detach();
	}

	while (ServerInfo::bRunning) {RakSleep(30);}
	
	return 0;
}
