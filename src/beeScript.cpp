//--------------------------------------------------
// Bee Hive Finding
// beeScript.h
// Date: 2022-09-16
// By Breno Cunha Queiroz
//--------------------------------------------------
#include "beeScript.h"
#include "common.h"
#include "hiveComponent.h"

void BeeScript::update(cmp::Entity entity, float dt) {
    BeeComponent* bee = entity.get<BeeComponent>();
    cmp::Transform* t = entity.get<cmp::Transform>();
    switch (bee->task) {
        case BeeComponent::WATCH_DANCE: {
            watchDance(bee, t, dt);
            break;
        }
        case BeeComponent::EXPLORE: {
            explore(bee, t, dt);
            break;
        }
        case BeeComponent::EVALUATE_HIVE: {
            evaluateHive(bee, t, dt);
            break;
        }
        case BeeComponent::GO_HOME: {
            goHome(bee, t, dt);
            break;
        }
        case BeeComponent::DANCE: {
            dance(bee, t, dt);
            break;
        }
        default:
            LOG_ERROR("BeeScript", "Bee $0 with invalid task $1", entity.getId(), bee->task);
    }
}

void BeeScript::move(cmp::Transform* t, float dt) {
    const float r0 = rand() / float(RAND_MAX);
    const float angle = t->orientation.get2DAngle();
    t->orientation.set2DAngle(angle + (r0 - 0.5f) * dt * 10);
    t->position.x += cos(angle) * dt * 0.03f;
    t->position.y += sin(angle) * dt * 0.03f;
}

void BeeScript::watchDance(BeeComponent* bee, cmp::Transform* t, float dt) {
    const atta::vec2 diff = bee->home - atta::vec2(t->position);
    const float distToHome = diff.length();
    const float inHome = 0.5f;

    // Go back to home if too far
    if (distToHome > inHome) {
        float angleToHome = atan2(diff.y, diff.x);
        t->orientation.set2DAngle(angleToHome);
    }
    move(t, dt);

    // Random change to start exploring
    if (rand() / float(RAND_MAX) < 0.000001f)
        bee->task = BeeComponent::EXPLORE;
}

void BeeScript::explore(BeeComponent* bee, cmp::Transform* t, float dt) {
    const float searchRadius = 10;
    const atta::vec2 pos = atta::vec2(t->position);
    const float distHome = pos.length();

    // Turn back home if too far away
    if (distHome > searchRadius)
        t->orientation.set2DAngle(atan2(-pos.y, -pos.x));

    // Check if found a hive
    int idx = 0;
    for (cmp::Entity hive : cmp::getFactory(hivePrototype)->getClones()) {
        float dist = (atta::vec2(hive.get<cmp::Transform>()->position) - pos).length();
        if (dist < hive.get<cmp::Transform>()->scale.x / 2.0f) {
            bee->targetHive = idx;
            bee->task = BeeComponent::EVALUATE_HIVE;
            break;
        }
        idx++;
    }

    move(t, dt);
}

void BeeScript::evaluateHive(BeeComponent* bee, cmp::Transform* t, float dt) {
    cmp::Entity hive = cmp::getFactory(hivePrototype)->getClones()[bee->targetHive];
    bee->hiveInterest = hive.get<HiveComponent>()->quality;
    bee->task = BeeComponent::GO_HOME;
}

void BeeScript::goHome(BeeComponent* bee, cmp::Transform* t, float dt) {
    atta::vec2 diff = bee->home - atta::vec2(t->position);
    const float distHome = t->position.length();
    const float inHome = 0.5f;

    // Check if arrived home
    if(distHome < inHome)
    {
        bee->task = BeeComponent::DANCE;
        return;
    }

    // Move to home
    float angleToHome = atan2(diff.y, diff.x);
    t->orientation.set2DAngle(angleToHome);
    move(t, dt);
}

void BeeScript::dance(BeeComponent* bee, cmp::Transform* t, float dt) {
    bee->task = BeeComponent::WATCH_DANCE;
    bee->targetHive = -1;
}
