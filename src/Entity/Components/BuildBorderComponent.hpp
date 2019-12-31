#ifndef __REPLICA__COMPONENTS__BUILD_BORDER_COMPONENT_HPP__
#define __REPLICA__COMPONENTS__BUILD_BORDER_COMPONENT_HPP__

#include "Entity/Components/Interface/IEntityComponent.hpp"

class BuildBorderComponent : public IEntityComponent {
public:

	BuildBorderComponent(std::int32_t componentID) : IEntityComponent(componentID) {}


	static constexpr int GetTypeID() { return 114; }

};

#endif