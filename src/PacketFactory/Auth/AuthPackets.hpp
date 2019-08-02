#ifndef __PACKETFACTORY__AUTH__AUTHPACKETS_HPP__
#define __PACKETFACTORY__AUTH__AUTHPACKETS_HPP__

#include <memory>
#include <RakNet/BitStream.h>
#include <RakNet/Types.h>
#include <RakNet/RakNetTypes.h>
#include <RakNet/MessageIdentifiers.h>
#include <RakNet/RakPeerInterface.h>


#include "Enums/EPackets.hpp"
#include "Structs/Networking/General/StructPacketHeader.hpp"
#include "Enums/ERemoteConnection.hpp"
#include "Utils/ServerInfo.hpp"
#include "Utils/IPUtils.hpp"

#include "Enums/EAuthPackets.hpp"
#include "NetworkDataTypes/ByteBool.hpp"

namespace PacketFactory {

	namespace Auth {

		inline void doLoginResponse(RakPeerInterface * rakServer, SystemAddress client, ELoginReturnCode reason, std::wstring customErrorMessage=L"") {
			RakNet::BitStream returnBS;
			// Head
			LUPacketHeader returnBSHead;
			returnBSHead.protocolID = static_cast<uint8_t>(ID_USER_PACKET_ENUM);
			returnBSHead.remoteType = static_cast<uint16_t>(ERemoteConnection::CLIENT);
			returnBSHead.packetID = static_cast<uint32_t>(EAuthPacketID::LOGIN_REQUEST);
			returnBS.Write(returnBSHead);
			//Data
			returnBS.Write(reason);

			StringUtils::writeStringToBitStream(&returnBS, "Talk_Like_A_Pirate");
			StringUtils::writeStringToBitStream(&returnBS, "Sneak_Like_A_Ninja");
			StringUtils::FillZero(&returnBS, 6 * 33);
			uint16_t majorVersion; uint16_t currentVersion; uint16_t minorVersion; ServerInfo::numericGameVersion(&majorVersion, &currentVersion, &minorVersion);
			returnBS.Write(majorVersion); returnBS.Write(currentVersion); returnBS.Write(minorVersion);

			std::string cip = client.ToString(false);
			std::string ip = (IPUtils::isIPPublic(client.binaryAddress) ? rakServer->GetExternalID(UNASSIGNED_SYSTEM_ADDRESS).ToString(false) :
							 (IPUtils::isIPIntern(client.binaryAddress) ? rakServer->GetInternalID().ToString(false) : "127.0.0.1"));
			
			Logger::log("AUTH", "Redirecting " +  cip + " to " + ip);

			StringUtils::writeWstringToBitStream(&returnBS, L"fad1892aa57db9e0cf74d45445f71599", 33);
			StringUtils::writeStringToBitStream(&returnBS, ip);
			StringUtils::writeStringToBitStream(&returnBS, ip);
			returnBS.Write<uint16_t>(2001);
			returnBS.Write<uint16_t>(0);
			StringUtils::FillZero(&returnBS, 33);
			StringUtils::writeStringToBitStream(&returnBS, "00000000-0000-0000-0000-000000000000", 37);
			returnBS.Write<uint32_t>(0);
			StringUtils::writeStringToBitStream(&returnBS, SERVER_LANGUAGE, 3);
			returnBS.Write<ByteBool>(true); // first Subscription Logon
			returnBS.Write<ByteBool>(false); // is FTP
			returnBS.Write<uint64_t>(0);

			returnBS.Write<uint16_t>(customErrorMessage.length()&0xFFFF);
			StringUtils::writeWstringToBitStream(&returnBS, customErrorMessage, customErrorMessage.length());

			// Stamp stuff
			returnBS.Write<uint32_t>(0x0144); // Stamps
			returnBS.Write<uint64_t>(0x0000000000000000);	returnBS.Write<uint64_t>(0x000000004ee27a4c);
			returnBS.Write<uint64_t>(0x0000001c00000007);	returnBS.Write<uint64_t>(0x000000004ee27a4c);
			returnBS.Write<uint64_t>(0x0000000300000008);	returnBS.Write<uint64_t>(0x000000004ee27a4c);
			returnBS.Write<uint64_t>(0x0000000000000009);	returnBS.Write<uint64_t>(0x000000004ee27a4c);
			returnBS.Write<uint64_t>(0x000000000000000a);	returnBS.Write<uint64_t>(0x000000004ee27a4c);
			returnBS.Write<uint64_t>(0x000000010000000b);	returnBS.Write<uint64_t>(0x000000004ee27a4c);
			returnBS.Write<uint64_t>(0x000000010000000e);	returnBS.Write<uint64_t>(0x000000004ee27a4c);
			returnBS.Write<uint64_t>(0x000000000000000f);	returnBS.Write<uint64_t>(0x000000004ee27a4c);
			returnBS.Write<uint64_t>(0x0000000100000011);	returnBS.Write<uint64_t>(0x000000004ee27a4d);
			returnBS.Write<uint64_t>(0x0000000000000005);	returnBS.Write<uint64_t>(0x000000004ee27a4d);
			returnBS.Write<uint64_t>(0x0000000100000006);	returnBS.Write<uint64_t>(0x000000004ee27a4d);
			returnBS.Write<uint64_t>(0x0000000100000014);	returnBS.Write<uint64_t>(0x000000004ee27a4d);
			returnBS.Write<uint64_t>(0x000029ca00000013);	returnBS.Write<uint64_t>(0x000000004ee27a4d);
			returnBS.Write<uint64_t>(0x0000000000000015);	returnBS.Write<uint64_t>(0x000000004ee27a4c);
			returnBS.Write<uint64_t>(0x0000000000000016);	returnBS.Write<uint64_t>(0x000000004ee27a4c);
			returnBS.Write<uint64_t>(0x000029c400000017);	returnBS.Write<uint64_t>(0x000000004ee27a4d);
			returnBS.Write<uint64_t>(0x000029c40000001b);	returnBS.Write<uint64_t>(0x000000004ee27a4d);
			returnBS.Write<uint64_t>(0x000000010000001c);	returnBS.Write<uint64_t>(0x000000004ee27a4c);
			returnBS.Write<uint64_t>(0x000000000000001d);	returnBS.Write<uint64_t>(0x000000004ee27a4c);
			returnBS.Write<uint64_t>(0x000029ca0000001e);	returnBS.Write<uint64_t>(0x000000004ee27a4d);
			rakServer->Send(&returnBS, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, client, false);
		}

	};

};

#endif // !__PACKETFACTORY__AUTH__AUTHPACKETS_HPP__