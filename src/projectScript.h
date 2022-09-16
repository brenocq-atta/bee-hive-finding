//--------------------------------------------------
// Bee Hive Finding
// projectScript.h
// Date: 2022-09-14
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef PROJECT_SCRIPT_H
#define PROJECT_SCRIPT_H
#include <atta/script/projectScript.h>

class Project : public atta::script::ProjectScript {
  public:
    void onLoad() override;

    void onStart() override;
    void onStop() override;

    void onUpdateBefore(float dt) override;
    void onUpdateAfter(float dt) override;
    void onUIRender() override;

  private:
    void init();

    // Analysis data
    std::vector<std::vector<float>> _distribTask;// Number of bees in each task at each step
    std::vector<std::vector<float>> _distribInterest;// Number of bees interested in each hive at each step
};

ATTA_REGISTER_PROJECT_SCRIPT(Project)

#endif // PROJECT_SCRIPT_H
