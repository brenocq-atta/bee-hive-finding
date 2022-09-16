//--------------------------------------------------
// Bee Hive Finding
// hiveComponent.cpp
// Date: 2022-09-16
// By Breno Cunha Queiroz
//--------------------------------------------------
#include "hiveComponent.h"

namespace cmp = atta::component;

template <>
cmp::ComponentDescription& cmp::TypedComponentRegistry<HiveComponent>::getDescription() {
    static cmp::ComponentDescription desc = {
        "Hive",
        {
            // Type                  Offset                            Name       Min   Max   Step
            {AttributeType::FLOAT32, offsetof(HiveComponent, quality), "quality", 0.0f, 1.0f, 0.01f},
        },
        // Max instances
        1024,
    };

    return desc;
}
