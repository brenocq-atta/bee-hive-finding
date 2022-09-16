//--------------------------------------------------
// Bee Hive Finding
// hiveComponent.h
// Date: 2022-09-16
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef HIVE_COMPONENT_H
#define HIVE_COMPONENT_H
#include <atta/component/interface.h>

namespace cmp = atta::component;

struct HiveComponent final : public cmp::Component {
    float quality; // Hive quality from 0 to 1
};
ATTA_REGISTER_COMPONENT(HiveComponent);
template <>
cmp::ComponentDescription& cmp::TypedComponentRegistry<HiveComponent>::getDescription();

#endif // HIVE_COMPONENT_H
