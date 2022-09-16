//--------------------------------------------------
// Bee Hive Finding
// beeComponent.cpp
// Date: 2022-09-16
// By Breno Cunha Queiroz
//--------------------------------------------------
#include "beeComponent.h"

template <>
cmp::ComponentDescription& cmp::TypedComponentRegistry<BeeComponent>::getDescription() {
    static cmp::ComponentDescription desc = {
        "Bee",
        {
            {AttributeType::VECTOR_FLOAT32, offsetof(BeeComponent, home), "home"},
            {AttributeType::UINT8, offsetof(BeeComponent, task), "task"},
            {AttributeType::INT32, offsetof(BeeComponent, targetHive), "targetHive"},
            {AttributeType::FLOAT32, offsetof(BeeComponent, hiveInterest), "hiveInterest"},
        },
        // Max instances
        1024,
    };

    return desc;
}
