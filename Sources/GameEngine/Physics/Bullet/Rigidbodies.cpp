#include "Rigidbodies.h"

#include <Logger/Log.h>

namespace GameEngine
{
namespace Physics
{
namespace Bullet
{
Rigidbodies::Rigidbodies()
    : dynamic_{idPool}
    , static_{idPool}
{
}
void Rigidbodies::clear()
{
    dynamic_.clear();
    static_.clear();
}
void Rigidbodies::foreach (std::function<void(IdType, Rigidbody&)> f)
{
    dynamic_.foreach (f);
    static_.foreach (f);
}
void Rigidbodies::erase(IdType rigidbodyId)
{
    auto it = rigidbodiesStatic.find(rigidbodyId);
    if (it != rigidbodiesStatic.end())
    {
        auto& [_, isStatic] = *it;
        if (isStatic)
        {
            static_.erase(rigidbodyId);
        }
        else
        {
            dynamic_.erase(rigidbodyId);
        }
        rigidbodiesStatic.erase(rigidbodyId);
    }
    else
    {
        ERROR_LOG("Rigidbody not found id=" + std::to_string(rigidbodyId));
    }
}
std::optional<Rigidbodies::IsStatic> Rigidbodies::isStatic(IdType rigidbodyId) const
{
    if (auto pairPtr = Utils::get(rigidbodiesStatic, rigidbodyId))
    {
        auto [_, is] = *pairPtr;
        return is;
    }
    
    return std::nullopt;
}
Rigidbody* Rigidbodies::get(const RigidbodyId& rigidbodyId)
{
    if (not rigidbodyId)
    {
        ERROR_LOG("Ivalid rigidbody");
        return nullptr;
    }

    auto maybeRigidBody = dynamic_.get(*rigidbodyId);
    if (maybeRigidBody)
    {
        return maybeRigidBody;
    }

    maybeRigidBody = static_.get(*rigidbodyId);
    if (maybeRigidBody)
    {
        return maybeRigidBody;
    }

    ERROR_LOG("Rigidbody not found " + std::to_string(*rigidbodyId));
    return nullptr;
}
RigidbodyId Rigidbodies::insert(Rigidbody newBody, Rigidbodies::IsStatic isStatic)
{
    auto btRigidbody = newBody.btRigidbody_.get();

    auto& target     = isStatic ? static_ : dynamic_;
    auto rigidBodyId = target.insert(std::move(newBody));
    btRigidbody->setUserIndex(static_cast<int>(rigidBodyId));
    rigidbodiesStatic.insert({rigidBodyId, isStatic});
    return rigidBodyId;
}
}  // namespace Bullet
}  // namespace Physics
}  // namespace GameEngine
