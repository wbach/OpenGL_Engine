#include "ArrowController.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
ArrowController::ArrowController(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(ArrowController).hash_code(), componentContext, gameObject)
    , direction(VECTOR_FORWARD)
{
}

ArrowController &ArrowController::setDirection(const vec3 & dir)
{
    direction = dir;
    return *this;
}

void ArrowController::CleanUp()
{
}

void ArrowController::ReqisterFunctions()
{
    RegisterFunction(FunctionType::OnStart,
                     [this]()
                     {
                         rigidbody = thisObject_.GetComponent<Rigidbody>();
                         if (rigidbody)
                         {
                             rigidbody->InputParams().angularFactor_ = vec3(0);
                             rigidbody->ApplyImpulse(direction * 10.f);
                         }
                     });
    RegisterFunction(FunctionType::Update, std::bind(&ArrowController::update, this));
}

void ArrowController::update()
{
    // DEBUG_LOG("update");
    if (not rigidbody)
        return;

//    const auto moveSpeed = vec3(1.0f);

//    auto targetVelocity = rigidbody->GetRotation() * vec3(VECTOR_UP) * moveSpeed;
//    rigidbody->SetVelocity(targetVelocity);
//    //DEBUG_LOG("targetVelocity : " + std::to_string(targetVelocity));
}

void ArrowController::registerReadFunctions()
{
}

void ArrowController::write(TreeNode&) const
{
}

}  // namespace Components
}  // namespace GameEngine
