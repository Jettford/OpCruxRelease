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
#include "WebInterface/WebInterface.hpp"
#include "GameCache/Interface/FastDatabase.hpp"
#include "DataTypes/AMF3.hpp"

#include "Utils/ServerInfo.hpp"
#include "Utils/StringUtils.hpp"
#include <chrono>

#include "Database/Database.hpp"

using namespace std::chrono;

std::vector<ILUServer *> virtualServerInstances;

enum class SERVERMODE : uint8_t { STANDALONE, MASTER, WORLD, AUTH } MODE_SERVER;

// Following Includes are for testing
#include "FileTypes/LUZFile/LUZone.hpp"
#include "Entity/GameObject.hpp"
#include "GameCache/WorldConfig.hpp"

//#include <mysql.h>


GameCache::Interface::FDB::Connection Cache;
BridgeMasterServer* masterServerBridge;

int main(int argc, char* argv[]) {
	std::string ipMaster = "127.0.0.1";

	Cache.Connect("./res/cdclient.fdb");
	
	using namespace Entity;

	GameObject * test = new GameObject();
	test->Test();
	RakNet::BitStream * testBs = new RakNet::BitStream();
	test->Serialize(testBs, ReplicaTypes::PacketTypes::CONSTRUCTION);

	//LUZone luz("./res/maps/01_live_maps/avant_gardens/nd_avant_gardens.luz");

	Database::Connect();
	//Database::DoATestQuery();


	MODE_SERVER = SERVERMODE::STANDALONE;
	
#ifdef LUR_PLATFORM_WIN32
	std::system("title OpCrux Server 3.0 (Standalone)");
#endif
	for (std::ptrdiff_t i = 0; i < argc; i++) {
		std::string arg = std::string(argv[i]);
		if (arg == "--master") {
			MODE_SERVER = SERVERMODE::MASTER;
#ifdef LUR_PLATFORM_WIN32
			std::system("title OpCrux Server 3.0 (Master only)");
#endif
		}
		if (arg == "--world") {
			MODE_SERVER = SERVERMODE::WORLD;
#ifdef LUR_PLATFORM_WIN32
			std::system("title OpCrux Server 3.0 (World only)");
#endif
		}
		if (arg == "--auth") {
			MODE_SERVER = SERVERMODE::AUTH;
#ifdef WIN32
			std::system("title OpCrux Server 3.0 (Auth only)");
#endif
		}
		else {
			continue;
		}

		if (argc >= 3) {
			ipMaster = argv[i + 1];
		}

		break;
	}

	ServerInfo::init();

	if (MODE_SERVER == SERVERMODE::STANDALONE || MODE_SERVER == SERVERMODE::MASTER) {
		std::thread mT([](MasterServer * ms) { ms = new MasterServer(); }, ServerInfo::masterServer);
		mT.detach();

		std::thread wiT([]() { WebInterface::WebInterfaceLoop(); });
		wiT.detach();
	}

	if (MODE_SERVER == SERVERMODE::STANDALONE || MODE_SERVER != SERVERMODE::MASTER) {
		masterServerBridge = new BridgeMasterServer(ipMaster);
		masterServerBridge->Connect();
		masterServerBridge->Listen();
	}

	if (MODE_SERVER == SERVERMODE::STANDALONE || MODE_SERVER == SERVERMODE::AUTH) {
		AuthServer * aS;
		std::thread aT([](AuthServer * as) { as = new AuthServer(); }, aS);
		aT.detach();
	}

	if (MODE_SERVER == SERVERMODE::STANDALONE || MODE_SERVER == SERVERMODE::WORLD) {
		WorldServer * charSelectWs;
		std::thread wT([](WorldServer * ws) { ws = new WorldServer(0,2001); }, charSelectWs);
		wT.detach();
	}

	while (ServerInfo::bRunning) RakSleep(30);

	std::cin.get();

}
