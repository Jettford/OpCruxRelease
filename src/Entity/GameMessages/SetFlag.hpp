#ifndef __ENTITY__GM__SetFlag_HPP__
#define __ENTITY__GM__SetFlag_HPP__
#include "Entity/GameMessages.hpp"
#include "Entity/Components/CharacterComponent.hpp"

namespace GM {
	struct SetFlag : GMBase {

		inline constexpr static Enums::EGameMessageID GetID() {
			return Enums::EGameMessageID::CLIENT_SET_FLAG;
		}

		bool bFlag;
		std::int32_t iFlagID;

		SetFlag() : GMBase() {}

		void Deserialize(RakNet::BitStream* bs) {
			GM_VAR_DESERIALIZE(bs, bFlag);
			GM_VAR_DESERIALIZE(bs, iFlagID);
		}

		void TriggerEvent(Entity::GameObject* sender, Entity::GameObject* target) {
			auto playerComp = target->GetComponent<CharacterComponent>();
			if (playerComp != nullptr) {
				playerComp->SetFlag(iFlagID, bFlag);
				MissionManager::LaunchTaskEvent(Enums::EMissionTask::FLAG, sender, target->GetObjectID(), iFlagID);
			}
		}
	};
}

#endif