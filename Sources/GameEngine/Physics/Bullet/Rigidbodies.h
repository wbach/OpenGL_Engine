#pragma once
#include "GameEngine/Physics/PhysicsApiTypes.h"
#include "Container.h"
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
    Rigidbody* get(std::function<bool(const std::pair<const IdType, Rigidbody>&)>);
    RigidbodyId insert(Rigidbody, IsStatic);
    void clear();
    void find_if();
    void foreach (std::function<void(IdType, Rigidbody&)>);
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
