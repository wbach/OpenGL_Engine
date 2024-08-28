#pragma once
#include "Container.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "Rigidbody.h"

namespace GameEngine
{
namespace Physics
{
namespace Bullet
{
class Rigidbodies
{
public:
    using IsStatic = bool;

    Rigidbodies();

    Rigidbody* get(const RigidbodyId&);
    RigidbodyId insert(Rigidbody, IsStatic);
    void clear();
    void foreach (std::function<void(Rigidbody&)>);
    void erase(IdType);
    std::optional<IsStatic> isStatic(IdType) const;

private:
    Utils::IdPool idPool;
    std::unordered_map<IdType, IsStatic> rigidbodiesStatic;

public:
    Container<Rigidbody> dynamic_;
    Container<Rigidbody> static_;
};

}  // namespace Bullet
}  // namespace Physics
}  // namespace GameEngine
