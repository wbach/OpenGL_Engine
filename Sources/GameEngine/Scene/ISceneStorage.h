#pragma once
#include "GameEngine/Resources/File.h"

namespace GameEngine
{
class GameObject;
class ISceneStorage
{
public:
    virtual ~ISceneStorage() = default;

    virtual void store()                                            = 0;
    virtual void restore()                                          = 0;
    virtual void saveToFile(const File&)                            = 0;
    virtual void readFromFile(const File&)                          = 0;
    virtual GameObject* loadPrefab(const File&, const std::string&) = 0;
    virtual void createPrefab(const File&, const GameObject&)       = 0;
    virtual GameObject* clone(const GameObject&)                    = 0;
};
}  // namespace GameEngine
