//--------------------------------------------------
// Bee Hive Finding
// beeScript.h
// Date: 2022-09-16
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef BEE_SCRIPT_H
#define BEE_SCRIPT_H
#include "beeComponent.h"
#include <atta/component/components/transform.h>
#include <atta/script/script.h>

namespace cmp = atta::component;
namespace scr = atta::script;

class BeeScript : public scr::Script {
  public:
    void update(cmp::Entity entity, float dt) override;

  private:
    void move(cmp::Transform* t, float dt);
    // Tasks
    void watchDance(BeeComponent* bee, cmp::Transform* t, float dt);
    void explore(BeeComponent* bee, cmp::Transform* t, float dt);
    void evaluateHive(BeeComponent* bee, cmp::Transform* t, float dt);
    void goHome(BeeComponent* bee, cmp::Transform* t, float dt);
    void dance(BeeComponent* bee, cmp::Transform* t, float dt);
};

ATTA_REGISTER_SCRIPT(BeeScript)

#endif // BEE_SCRIPT_H
