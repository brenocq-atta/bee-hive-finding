//--------------------------------------------------
// Global Hive Finding
// globalComponent.cpp
// Date: 2022-09-16
// By Breno Cunha Queiroz
//--------------------------------------------------
#include "globalComponent.h"

template <>
cmp::ComponentDescription& cmp::TypedComponentRegistry<GlobalComponent>::getDescription() {
    static cmp::ComponentDescription desc = {
        "Global",
        {
            {AttributeType::VECTOR_FLOAT32, offsetof(GlobalComponent, dancingPerc), "dancingPerc"},
            {AttributeType::FLOAT32, offsetof(GlobalComponent, interestDecay), "interestDecay"},
            {AttributeType::FLOAT32, offsetof(GlobalComponent, followChance), "followChance"},
            {AttributeType::FLOAT32, offsetof(GlobalComponent, exploreChance), "exploreChance"},
            {AttributeType::FLOAT32, offsetof(GlobalComponent, maxRotation), "maxRotation"},
            {AttributeType::FLOAT32, offsetof(GlobalComponent, maxSpeed), "maxSpeed"},
        },
        // Max instances
        1,
    };

    return desc;
}
