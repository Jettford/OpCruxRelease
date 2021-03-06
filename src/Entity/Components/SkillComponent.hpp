#ifndef __REPLICA__COMPONENTS__SKILL_COMPONENT_HPP__
#define __REPLICA__COMPONENTS__SKILL_COMPONENT_HPP__

#include <mutex>

#include "Entity/Components/Interface/IEntityComponent.hpp"

#include "Entity/GameMessages/StartSkill.hpp"
#include "Entity/GameMessages/SyncSkill.hpp"

#include "GameCache/BehaviorParameter.hpp"
#include "GameCache/BehaviorTemplate.hpp"
#include "GameCache/BehaviorTemplateName.hpp"
#include "GameCache/SkillBehavior.hpp"


using namespace DataTypes;

struct SkillStackParameters {
	bool bUsedMouse = false;
	std::float_t fCasterLatency = 0.0f;
	std::int32_t iCastType = 0;
	DataTypes::Vector3 lastClickedPosit = DataTypes::Vector3::zero();
	DataTypes::LWOOBJID optionalOriginatorID = 0ULL;
	DataTypes::LWOOBJID optionalTargetID = 0ULL;
	DataTypes::Quaternion originatorRot = DataTypes::Quaternion();
	std::uint32_t uiBehvaiorHandle = 0;
	std::uint32_t uiSkillHandle = 0;
	DataTypes::LWOOBJID currentTarget = 0ULL;
	bool bDone = false;
};

class SkillComponent : public IEntityComponent {
private:
	bool _isDirtyFlag = false;

	SkillStackParameters parameters;

	std::uint32_t currentHandle = 0;
	std::uint32_t currentSkill = 0;

	void UnCast(const std::string sBitStream, long behaviorID = 0);

	std::unordered_map<std::uint32_t /*behaviorHandle*/, std::int32_t /*behaviorAction*/> behaviorHandles;
	std::mutex mutex_behaviorHandles;
public:
	std::uint32_t currentStackDepth = 0;

public:

	SkillStackParameters GetParameters() {
		return parameters;
	}

	void SetTarget(DataTypes::LWOOBJID target) {
		parameters.currentTarget = target;
	}

	void DoDamageOnTarget(std::uint32_t damage);

	SkillComponent(std::int32_t componentID) : IEntityComponent(componentID) {}

	static constexpr int GetTypeID() { return 9; }

	void Awake() {
		
	}

	void Serialize(RakNet::BitStream * factory, ReplicaTypes::PacketTypes packetType) {
		/* TODO: Skill Component Serialization */
		if (packetType == ReplicaTypes::PacketTypes::CONSTRUCTION) {
			factory->Write(true);
			if (true) {
				factory->Write<std::uint32_t>(0);
			}
		}
	}

	inline void OnStartSkill(Entity::GameObject * sender, GM::StartSkill & msg) {
		parameters = SkillStackParameters();

		currentSkill = msg.skillID;
		currentHandle = msg.uiSkillHandle;

		parameters.bUsedMouse = msg.bUsedMouse;
		parameters.fCasterLatency = msg.fCasterLatency;
		parameters.iCastType = msg.iCastType;
		parameters.lastClickedPosit = msg.lastClickedPosit;
		parameters.optionalOriginatorID = msg.optionalOriginatorID;
		parameters.optionalTargetID = msg.optionalTargetID;
		parameters.originatorRot = msg.originatorRot;
		parameters.uiSkillHandle = msg.uiSkillHandle;

		GM::EchoStartSkill echoGM; {
			echoGM.bUsedMouse = msg.bUsedMouse;
			echoGM.fCasterLatency = msg.fCasterLatency;
			echoGM.iCastType = msg.iCastType;
			echoGM.lastClickedPosit = msg.lastClickedPosit;
			echoGM.optionalOriginatorID = msg.optionalOriginatorID;
			echoGM.optionalTargetID = msg.optionalTargetID;
			echoGM.originatorRot = msg.originatorRot;
			echoGM.sBitStream = msg.sBitStream;
			echoGM.skillID = msg.skillID;
			echoGM.uiSkillHandle = msg.uiSkillHandle;
		}
		GameMessages::Broadcast(this->owner, echoGM, true);

		UnCast(msg.sBitStream);
	}

	inline void OnSyncSkill(Entity::GameObject* sender, GM::SyncSkill & msg) {
		parameters.bDone = msg.bDone;
		parameters.uiBehvaiorHandle = msg.uiBehaviorHandle;
		parameters.uiSkillHandle = msg.uiSkillHandle;

		mutex_behaviorHandles.lock();
		auto behaviorIDIt = behaviorHandles.find(msg.uiBehaviorHandle);
		std::int32_t behaviorID = 0;
		if (behaviorIDIt != behaviorHandles.end()) {
			behaviorID = behaviorIDIt->second;
			behaviorHandles.erase(behaviorIDIt);
		}
		mutex_behaviorHandles.unlock();

		GM::EchoSyncSkill echoGM; {
			echoGM.bDone = msg.bDone;
			echoGM.sBitStream = msg.sBitStream;
			echoGM.uiBehaviorHandle = msg.uiBehaviorHandle;
			echoGM.uiSkillHandle = msg.uiSkillHandle;
		}
		GameMessages::Broadcast(this->owner, echoGM, true);

		if (behaviorID <= 0) return;
		UnCast(msg.sBitStream, behaviorID);
	}

	inline void AddBehaviorHandle(std::uint32_t behaviorHandle, std::int32_t behaviorAction) {
		mutex_behaviorHandles.lock();
		behaviorHandles.insert({ behaviorHandle, behaviorAction });
		mutex_behaviorHandles.unlock();
	}

};

#include "Entity/Components/SkillComponent/AbstractAggregateBehavior.hpp"


void SkillComponent::UnCast(const std::string sBitStream, long behaviorID) {
	currentStackDepth = 0;
	RakNet::BitStream bs = RakNet::BitStream(reinterpret_cast<unsigned char*>(const_cast<char*>(sBitStream.c_str())), sBitStream.size(), false);
	if(behaviorID <= 0) behaviorID = CacheSkillBehavior::GetBehaviorID(currentSkill);
	AbstractAggregateBehavior::StartUnCast(this, behaviorID, &bs);
}

#include "Entity/Components/DestructibleComponent.hpp"
void SkillComponent::DoDamageOnTarget(std::uint32_t damage) {
	// Make sure objectID is not empty
	if (parameters.currentTarget == 0ULL) {
		Logger::log("WRLD", "Unable to do damage on empty objectID", LogType::WARN);
		return;
	}

	// Get object
	Entity::GameObject* target = this->owner->GetZoneInstance()->objectsManager->GetObjectByID(parameters.currentTarget);

	// Make sure object exists
	if (target == nullptr) {
		Logger::log("WRLD", "Unable to do damage on missing object", LogType::ERR);
		return;
	}

	// Get destructible component
	DestructibleComponent* targetDestComp = target->GetComponent<DestructibleComponent>();

	// make sure target has component
	if (targetDestComp == nullptr) {
		Logger::log("WRLD", "Unable to do damage on object without Destructible Component!", LogType::ERR);
		return;
	}

	// Do damage
	targetDestComp->PerformDamageRequest(this->owner, damage);
}

#endif