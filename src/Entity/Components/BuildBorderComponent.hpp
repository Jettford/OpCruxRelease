#ifndef __REPLICA__COMPONENTS__BUILD_BORDER_COMPONENT_HPP__
#define __REPLICA__COMPONENTS__BUILD_BORDER_COMPONENT_HPP__

#include "Entity/Components/Interface/IEntityComponent.hpp"

class BuildBorderComponent : public IEntityComponent {
public:

	BuildBorderComponent(std::int32_t componentID) : IEntityComponent(componentID) {}

	// IMAGINATION HELMET LOT
	const std::uint32_t thinkingHatLOT = 6086;

	static constexpr int GetTypeID() { return 114; }

	void OnRequestUse(Entity::GameObject* sender, GM::RequestUse& msg);

	void OnSetBuildMode(Entity::GameObject* sender, GM::SetBuildMode& msg);

	void OnStartBuildingWithItem(Entity::GameObject* sender, GM::StartBuildingWithItem& msg);

};

#endif