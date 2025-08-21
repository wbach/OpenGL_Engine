#pragma once
#include "GameObject.h"

namespace GameEngine
{
class Prefab : public GameObject
{
public:
    using GameObject::GameObject;

    void setFile(const File& f);
    const File & getFile() const;

private:
    File file;
};
}  // namespace GameEngine
