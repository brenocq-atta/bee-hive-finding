//--------------------------------------------------
// Bee Hive Finding
// beeScript.h
// Date: 2022-09-16
// By Breno Cunha Queiroz
//--------------------------------------------------
#include "beeScript.h"
#include "common.h"
#include "globalComponent.h"
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
    static GlobalComponent* g = global.get<GlobalComponent>();
    const float r0 = rand() / float(RAND_MAX);
    const float angle = t->orientation.get2DAngle();
    t->orientation.set2DAngle(angle + (r0 - 0.5f) * dt * g->maxRotation);
    t->position.x += cos(angle) * dt * g->maxSpeed;
    t->position.y += sin(angle) * dt * g->maxSpeed;
}

void BeeScript::watchDance(BeeComponent* bee, cmp::Transform* t, float dt) {
    const atta::vec2 diff = bee->home - atta::vec2(t->position);
    const float distToHome = diff.length();
    const float inHome = 0.5f;
    static GlobalComponent* g = global.get<GlobalComponent>();

    // Go back to home if too far
    if (distToHome > inHome) {
        float angleToHome = atan2(diff.y, diff.x);
        t->orientation.set2DAngle(angleToHome);
    }
    move(t, dt);

    // Random chance to follow dancing bee
    if (rand() / float(RAND_MAX) < (g->followChance * dt)) {
        auto dancingPerc = global.get<GlobalComponent>()->dancingPerc;

        // Roulette to choose which hive to evaluate
        float currPerc = 0;
        float r = rand() / float(RAND_MAX);
        for (int i = 0; dancingPerc[i] != -1.0f; i++) {
            currPerc += dancingPerc[i];
            if (r < currPerc) {
                bee->targetHive = i;
                bee->task = BeeComponent::EVALUATE_HIVE;
                break;
            }
        }
    }

    // Random chance to start exploring
    if (rand() / float(RAND_MAX) < (g->exploreChance * dt))
        bee->task = BeeComponent::EXPLORE;
}

void BeeScript::explore(BeeComponent* bee, cmp::Transform* t, float dt) {
    const float searchLength = 5;
    const atta::vec2 pos = atta::vec2(t->position);

    // Turn back if too far away
    if (std::abs(pos.x) > searchLength || std::abs(pos.y) > searchLength)
        t->orientation.set2DAngle(atan2(-pos.y, -pos.x));

    // Check if found a hive
    int idx = 0;
    for (cmp::Entity hive : cmp::getFactory(hivePrototype)->getClones()) {
        float dist = (atta::vec2(hive.get<cmp::Transform>()->position) - pos).length();
        // Check if close enough from hive
        if (dist < hive.get<cmp::Transform>()->scale.x / 2.0f) {
            // Ignore if hive is home
            if (atta::vec2(hive.get<cmp::Transform>()->position) == bee->home)
                continue;
            bee->targetHive = idx;
            bee->task = BeeComponent::EVALUATE_HIVE;
            break;
        }
        idx++;
    }

    move(t, dt);
}

void BeeScript::evaluateHive(BeeComponent* bee, cmp::Transform* t, float dt) {
    const cmp::Entity hive = cmp::getFactory(hivePrototype)->getClones()[bee->targetHive];
    const atta::vec2 pos = atta::vec2(t->position);
    const atta::vec2 hiveVec = atta::vec2(hive.get<cmp::Transform>()->position) - pos;
    const float distHive = hiveVec.length();
    if (distHive > hive.get<cmp::Transform>()->scale.x / 2.0f) {
        t->orientation.set2DAngle(atan2(hiveVec.y, hiveVec.x));
        move(t, dt);
    } else {
        bee->hiveInterest = hive.get<HiveComponent>()->quality;
        bee->task = BeeComponent::GO_HOME;
    }
}

void BeeScript::goHome(BeeComponent* bee, cmp::Transform* t, float dt) {
    atta::vec2 diff = bee->home - atta::vec2(t->position);
    const float distHome = diff.length();
    const float inHome = 0.5f;

    // Check if arrived home
    if (distHome < inHome) {
        bee->task = BeeComponent::DANCE;
        return;
    }

    // Move to home
    float angleToHome = atan2(diff.y, diff.x);
    t->orientation.set2DAngle(angleToHome);
    move(t, dt);
}

void BeeScript::dance(BeeComponent* bee, cmp::Transform* t, float dt) {
    static GlobalComponent* g = global.get<GlobalComponent>();

    // Shake shake
    t->orientation.set2DAngle(t->orientation.get2DAngle() + dt * (rand() / float(RAND_MAX) - 0.5f));

    // Stop dancing after a while
    bee->hiveInterest -= dt * g->interestDecay;
    if (bee->hiveInterest < 0) {
        bee->task = BeeComponent::WATCH_DANCE;
        bee->targetHive = -1;
    }
}
