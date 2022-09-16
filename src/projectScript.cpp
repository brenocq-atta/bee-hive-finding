//--------------------------------------------------
// Bee Hive Finding
// projectScript.cpp
// Date: 2022-09-14
// By Breno Cunha Queiroz
//--------------------------------------------------
#include "projectScript.h"
#include "common.h"
#include "imgui.h"
#include "implot.h"
#include <atta/component/components/material.h>
#include <atta/component/components/prototype.h>
#include <atta/component/components/transform.h>
#include <atta/resource/interface.h>
#include <atta/resource/resources/image.h>
#include <atta/resource/resources/material.h>

#include "beeComponent.h"
#include "globalComponent.h"
#include "hiveComponent.h"

namespace rsc = atta::resource;

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

void Project::onLoad() {
    GlobalComponent* g = global.get<GlobalComponent>();
#ifdef WEB_BUILD
    g->interestDecay = 0.01;
    g->followChance = 0.05f;
    g->exploreChance = 0.005f;
    g->maxRotation = 1.0f;
    g->maxSpeed = 0.3f;
#else
    g->interestDecay = 0.00005;
    g->followChance = 0.0005f;
    g->exploreChance = 0.00001f;
    g->maxRotation = 0.05f;
    g->maxSpeed = 0.002f;
#endif
}

void Project::onStart() {
    init();
    _distribTask.resize(int(BeeComponent::NUM_TASKS));
    _distribInterest.resize(hivePrototype.get<cmp::Prototype>()->maxClones);
}

void Project::onStop() {
    // Destroy hive materials to avoid saving the created materials to the .atta file
    for (int i = 0; i < hivePrototype.get<cmp::Prototype>()->maxClones; i++)
        rsc::destroy<rsc::Material>("Hive " + std::to_string(i));
    _distribTask.clear();
    _distribInterest.clear();
}

void Project::onUpdateBefore(float dt) {
    // Before bees are run, update the global data with information about which bees are dancing
    const unsigned numHives = hivePrototype.get<cmp::Prototype>()->maxClones;
    if (numHives > GlobalComponent::maxNumHives) {
        LOG_ERROR("Project",
                  "There are [w]$0[] hives, but only a maximum of [w]$1[] is supported. Change [w]GlobalComponent[] if you need more hives",
                  GlobalComponent::maxNumHives);
        return;
    }

    // Count number of bees dancing
    unsigned numDancing = 0;
    std::vector<unsigned> hiveDancingNum(numHives);
    for (cmp::Entity bee : cmp::getFactory(beePrototype)->getClones()) {
        BeeComponent* b = bee.get<BeeComponent>();
        if (b->task == BeeComponent::DANCE) {
            numDancing++;
            hiveDancingNum[b->targetHive]++;
        }
    }

    // Update dancing perc
    GlobalComponent* g = global.get<GlobalComponent>();
    if (numDancing > 0) {
        for (int i = 0; i < numHives; i++)
            g->dancingPerc[i] = hiveDancingNum[i] / float(numDancing);
        g->dancingPerc[numHives] = -1.0f;
    } else
        g->dancingPerc[0] = -1.0f;

    // Check if all bees should migrate
    const float percDecide = 0.75;
    if (numDancing >= beePrototype.get<cmp::Prototype>()->maxClones * percDecide) {
        for (int i = 0; i < numHives; i++)
            if (g->dancingPerc[i] >= percDecide) {
                // Migrate bees
                for (cmp::Entity bee : cmp::getFactory(beePrototype)->getClones()) {
                    BeeComponent* b = bee.get<BeeComponent>();
                    b->home = atta::vec2(cmp::getFactory(hivePrototype)->getClones()[i].get<cmp::Transform>()->position);
                    b->task = BeeComponent::WATCH_DANCE;
                    b->targetHive = -1;
                    b->hiveInterest = 0.0f;

                    // Reset hive colors
                    for (int j = 0; j < numHives; j++) {
                        std::string materialName = "Hive " + std::to_string(j);
                        if (j != i) {
                            float q = cmp::getFactory(hivePrototype)->getClones()[j].get<HiveComponent>()->quality;
                            atta::vec3& color = rsc::create<rsc::Material>(materialName)->color;
                            getHeatMapColor(q, &color.x, &color.y, &color.z);
                        } else {
                            // Set home as gray
                            rsc::get<rsc::Material>(materialName)->color = atta::vec3(0.3, 0.3, 0.3);
                        }
                    }
                }
                break;
            }
    }
}

void Project::onUpdateAfter(float dt) {
    static auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    int time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Collect data for the UI every 10ms
    if (time > 10) {
        start = end;
        // Update distribution vector for plotting
        for (auto& task : _distribTask)
            task.push_back(0);
        for (auto& interest : _distribInterest)
            interest.push_back(0);

        for (cmp::Entity bee : cmp::getFactory(beePrototype)->getClones()) {
            BeeComponent* b = bee.get<BeeComponent>();
            _distribTask[int(b->task)].back()++;
            if (b->targetHive >= 0)
                _distribInterest[b->targetHive].back()++;
        }
    }
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
        std::string materialName = "Hive " + std::to_string(hive.getCloneId());
        atta::vec3& color = rsc::create<rsc::Material>(materialName)->color;
        getHeatMapColor(q, &color.x, &color.y, &color.z);
        hive.get<cmp::Material>()->set(materialName);
    }
}

void Project::onUIRender() {
    static const char* taskNames[] = {"WATCH_DANCE", "EXPLORE", "EVALUATE_HIVE", "GO_HOME", "DANCE"};
    static bool firstTime = true;

    if (_distribTask.empty() || _distribTask[0].empty())
        return;

    if (firstTime) {
        ImGui::SetNextWindowSize(ImVec2(400, 550));
        firstTime = false;
    }

    ImGui::Begin("Analysis");
    {
        std::vector<float> xs(_distribTask[0].size());
        for (unsigned i = 0; i < xs.size(); i++)
            xs[i] = i;

        if (ImPlot::BeginPlot("Distribution of tasks", ImVec2(-1, 250))) {
            ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
            for (unsigned i = 0; i < _distribTask.size(); i++)
                ImPlot::PlotLine(taskNames[i], xs.data(), _distribTask[i].data(), xs.size());
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("Distribution of interest", ImVec2(-1, 250))) {
            ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
            for (unsigned i = 0; i < _distribInterest.size(); i++) {
                std::string name = "Hive " + std::to_string(i);
                atta::vec3 c = rsc::get<rsc::Material>(name)->color;
                ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(c.x, c.y, c.z, 1));
                ImPlot::PlotLine(name.c_str(), xs.data(), _distribInterest[i].data(), xs.size());
                ImPlot::PopStyleColor();
            }
            ImPlot::EndPlot();
        }
    }
    ImGui::End();
}
