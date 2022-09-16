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

  private:
    void init();
};

ATTA_REGISTER_PROJECT_SCRIPT(Project)

#endif // PROJECT_SCRIPT_H
