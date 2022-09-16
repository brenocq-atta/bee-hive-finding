//--------------------------------------------------
// Bee Hive Finding
// projectScript.cpp
// Date: 2022-09-14
// By Breno Cunha Queiroz
//--------------------------------------------------
#include "projectScript.h"
#include "beeComponent.h"
#include "common.h"
#include "hiveComponent.h"
#include <atta/component/components/material.h>
#include <atta/component/components/prototype.h>
#include <atta/component/components/transform.h>
#include <atta/resource/interface.h>
#include <atta/resource/resources/image.h>
#include <atta/resource/resources/material.h>

namespace rsc = atta::resource;

void Project::onLoad() {}

void Project::onStart() { init(); }

void Project::onStop() {
    // Destroy hive materials to avoid saving the created materials to the .atta file
    for (int i = 0; i < hivePrototype.get<cmp::Prototype>()->maxClones; i++)
        rsc::destroy<rsc::Material>("Hive" + std::to_string(i));
}

void Project::onUpdateBefore(float dt) {}

void getHeatMapColor(float value, float* red, float* green, float* blue) {
    // Thanks to https://www.andrewnoske.com/wiki/Code_-_heatmaps_and_color_gradients
    const int NUM_COLORS = 4;
    static float color[NUM_COLORS][3] = {{0, 0, 1}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0}};
    int idx1;
    int idx2;
    float fractBetween = 0;
    if (value <= 0)
        idx1 = idx2 = 0;
    else if (value >= 1)
        idx1 = idx2 = NUM_COLORS - 1;
    else {
        value = value * (NUM_COLORS - 1);
        idx1 = floor(value);
        idx2 = idx1 + 1;
        fractBetween = value - float(idx1);
    }
    *red = (color[idx2][0] - color[idx1][0]) * fractBetween + color[idx1][0];
    *green = (color[idx2][1] - color[idx1][1]) * fractBetween + color[idx1][1];
    *blue = (color[idx2][2] - color[idx1][2]) * fractBetween + color[idx1][2];
}

void Project::init() {
    // Initialize bees
    for (cmp::Entity bee : cmp::getFactory(beePrototype)->getClones()) {
        float r = rand() / float(RAND_MAX);
        float angle = rand() / float(RAND_MAX) * 2 * M_PI;
        bee.get<cmp::Transform>()->position = atta::vec3(r * cos(angle), r * sin(angle), 0);
        bee.get<cmp::Transform>()->orientation.set2DAngle(rand() / float(RAND_MAX) * 2 * M_PI);
        bee.get<BeeComponent>()->targetHive = -1;
    }

    // Initialize hives
    for (cmp::Entity hive : cmp::getFactory(hivePrototype)->getClones()) {
        // Random position between two squares
        const float extSize = 5;   // External square size
        const float intSize = 1.5; // Internal square size
        float r0 = (rand() / float(RAND_MAX) - 0.5f) * 2;
        float r1 = (rand() / float(RAND_MAX) - 0.5f) * 2;
        float x = intSize + (extSize - intSize) * std::abs(r0);
        float y = intSize + (extSize - intSize) * std::abs(r1);
        if (r0 < 0)
            x *= -1;
        if (r1 < 0)
            y *= -1;
        hive.get<cmp::Transform>()->position = atta::vec3(x, y, -1);
        float q = rand() / float(RAND_MAX);
        hive.get<HiveComponent>()->quality = q;

        // Create one material per hive and set material
        std::string materialName = "Hive" + std::to_string(hive.getCloneId());
        atta::vec3& color = rsc::create<rsc::Material>(materialName)->color;
        getHeatMapColor(q, &color.x, &color.y, &color.z);
        hive.get<cmp::Material>()->set(materialName);
    }
}
