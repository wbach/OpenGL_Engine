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
        LOG_WARN << "Rigidbody not found id=" << rigidbodyId;
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
        LOG_WARN << "Invalid rigidbody: " << rigidbodyId;
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

    LOG_WARN << "Rigidbody not found " << rigidbodyId;
    return nullptr;
}

Rigidbody* Rigidbodies::get(std::function<bool(const std::pair<const IdType, Rigidbody>&)> predicate)
{
    auto maybeRigidBody = dynamic_.get(predicate);

    if (maybeRigidBody)
    {
        return &maybeRigidBody->second;
    }

    maybeRigidBody = static_.get(predicate);
    if (maybeRigidBody)
    {
        return &maybeRigidBody->second;
    }

    LOG_WARN << "Rigidbody not found with predicate.";
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
