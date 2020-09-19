#ifndef __REPLICA__COMPONENTS__FACTION_TRIGGER_COMPONENT_HPP__
#define __REPLICA__COMPONENTS__FACTION_TRIGGER_COMPONENT_HPP__

#include "Entity/Components/Interface/IEntityComponent.hpp"

class FactionTriggerComponent : public IEntityComponent {
public:

	FactionTriggerComponent(std::int32_t componentID) : IEntityComponent(componentID) {}


	static constexpr std::int16_t GetTypeID() { return 72; }

};

#endif