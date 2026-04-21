#pragma once
#include "GameEngine/Resources/File.h"
#include "GameObject.h"

namespace GameEngine
{
class Prefab : public GameObject
{
public:
    using GameObject::GameObject;

    void setFile(const File& f);
    const File& getFile() const;
    GameObject* getObject();

private:
    File file;
};
}  // namespace GameEngine
