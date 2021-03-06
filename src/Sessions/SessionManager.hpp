#ifndef __SESSIONS__SESSIONMANAGER_HPP__
#define __SESSIONS__SESSIONMANAGER_HPP__
/// NOTE:
// This is attached to the client server

#include "ClientSession.hpp"
#include <vector>
#include <stdlib.h>
#include "DataTypes/LWOOBJID.hpp"

class SessionManager {
	std::vector<ClientSession> clients;
public:

	std::vector<ClientSession> GetClients() {
		return clients;
	}

	ClientSession * GetSession(SystemAddress systemAddress) {
		for (int i = 0; i < clients.size(); ++i) {
			ClientSession * session = &clients[i];
			if ((session->systemAddress) != systemAddress) continue;
			return session;
		}
		return nullptr;
	}

	ClientSession* GetSession(DataTypes::LWOOBJID object) {
		for (int i = 0; i < clients.size(); ++i) {
			ClientSession* session = &clients[i];
			if ((session->actorID) != object) continue;
			return session;
		}
		return nullptr;
	}

	ClientSession* GetSession(std::uint32_t accountID) {
		for (int i = 0; i < clients.size(); ++i) {
			ClientSession* session = &clients[i];
			if ((session->accountID) != accountID) continue;
			return session;
		}
		return nullptr;
	}

	void AddSession(ClientSession csCopy) {
		clients.push_back(csCopy);
	}
	void RemoveSession(ClientSession * session) {
		for (int i = 0; i < clients.size(); ++i) {
			if (&clients[i] == session) {
				clients.erase(clients.begin() + i);
				return;
			}
		}
	}
};

#endif // !__SESSIONS__SESSIONMANAGER_HPP__
