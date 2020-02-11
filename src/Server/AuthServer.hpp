#ifndef __SERVER__AUTHSERVER_HPP__
#define __SERVER__AUTHSERVER_HPP__

#include "Interfaces/ILUServer.hpp"

class AuthServer : public ILUServer {
public:
	AuthServer();
	~AuthServer();
private:
	void RequestMasterUserAuthConfirmation(SystemAddress systemAddress, std::uint64_t accountID);
	void handlePacket(RakPeerInterface * rakServer, LUPacket * packet);
};

#endif // !__SERVER__AUTHSERVER_HPP__
