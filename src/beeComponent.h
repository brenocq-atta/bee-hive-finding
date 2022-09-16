//--------------------------------------------------
// Bee Hive Finding
// beeComponent.h
// Date: 2022-09-16
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef BEE_COMPONENT_H
#define BEE_COMPONENT_H
#include <atta/component/interface.h>

namespace cmp = atta::component;

struct BeeComponent final : public cmp::Component {
    enum Task : uint8_t {
        WATCH_DANCE = 0, // Stay at home watching other bees dancing
        EXPLORE,         // Fly around trying to find new hives
        EVALUATE_HIVE,   // Fly to hive to evaluate it
        GO_HOME,         // Fly back to home
        DANCE,           // Dance to tell other bees about the hive
    };
    atta::vec2 home;    // Home position
    Task task;          // Current bee task
    int targetHive;     // The hive of preference of this bee bee, -1 if no preference
    float hiveInterest; // How good this bee bee thinks the hive is
};
ATTA_REGISTER_COMPONENT(BeeComponent);
template <>
cmp::ComponentDescription& cmp::TypedComponentRegistry<BeeComponent>::getDescription();

#endif // BEE_COMPONENT_H
