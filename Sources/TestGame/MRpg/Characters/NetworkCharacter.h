#pragma once
#include <unordered_map>
#include "Common/Controllers/ControllersTypes.h"
#include "Common/Hero/HeroClassesTypes.h"
#include "Common/Hero/Stats.h"
#include "GameEngine/Objects/GameObject.h"

namespace common
{
namespace Hero
{
struct CommonStats;
}  // namespace Hero

namespace Controllers
{
class IController;
}  // namespace Controllers
}  // namespace common

namespace MmmoRpg
{

typedef std::unordered_map<common::Controllers::Types, std::shared_ptr<common::Controllers::IController>>
    ControllersMap;

class NetworkCharacter
{
public:
    NetworkCharacter(uint32 id, const common::Hero::CommonStats&, GameEngine::GameObject& gameObject);
    std::shared_ptr<common::Controllers::IController> GetControllerByType(common::Controllers::Types type);
    void UpdateControllers(float dt);
    uint32 GetId();
    GameEngine::GameObject& GetGameObject();

private:
    uint32 id_;
    common::Hero::CommonStats stats_;
    GameEngine::GameObject& gameObject_;
    ControllersMap controllers_;
};
}  // namespace MmmoRpg