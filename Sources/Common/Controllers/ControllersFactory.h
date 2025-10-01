#pragma once
#include <memory>

namespace Hero
{
class CommonStats;
};

namespace common
{
class Transform;

namespace Controllers
{
class IController;
std::shared_ptr<IController> CreateCharacterController(Transform& attachedTranform, const Hero::CommonStats& stats);
}  // namespace Controllers
}  // namespace common
