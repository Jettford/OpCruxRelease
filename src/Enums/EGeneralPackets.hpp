#ifndef __ENUMS__EGENERALPACKETS_HPP__
#define __ENUMS__EGENERALPACKETS_HPP__

#include <memory>

namespace Enums {

	/*
		Used to identify disconect reasons easily.
	*/
	enum class EDisconnectReason : uint32_t {
		UNKNOWN_SERVER_ERROR = 0x00,
		DUPLICATE_LOGIN = 0x04,
		SERVER_SHUTDOWN = 0x05,
		SERVER_UNABLE_TO_LOAD_MAP = 0x06,
		INVALID_SESSION_KEY = 0x07,
		ACCOUNT_WAS_NOT_IN_PENDING_LIST = 0x08,
		CHARACTER_NOT_FOUND = 0x09,
		CHARACTER_CORRUPTION = 0x0a,
		KICK = 0x0b,
		FREE_TRIAL_EXPIRED = 0x0d,
		PLAY_SCHEDULE_TIME_DONE = 0x0e
	};
}

#endif // !__ENUMS__EGENERALPACKETS_HPP__
