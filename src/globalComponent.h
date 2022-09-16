//--------------------------------------------------
// Global Hive Finding
// globalComponent.h
// Date: 2022-09-16
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef GLOBAL_COMPONENT_H
#define GLOBAL_COMPONENT_H
#include <atta/component/interface.h>

namespace cmp = atta::component;

struct GlobalComponent final : public cmp::Component {
    static constexpr unsigned maxNumHives = 20; // Support up to 20 hives
    // Percentage of bees dancing for each hive
    float dancingPerc[maxNumHives + 1];
    float interestDecay; // Rate of decay of dancing time
    float followChance;  // Change of following another bee
    float exploreChance; // Change of start exploring
    float maxRotation;   // Bee maximum rotation
    float maxSpeed;   // Bee maximum speed
};
ATTA_REGISTER_COMPONENT(GlobalComponent);
template <>
cmp::ComponentDescription& cmp::TypedComponentRegistry<GlobalComponent>::getDescription();

#endif // GLOBAL_COMPONENT_H
