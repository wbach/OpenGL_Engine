#pragma once
#include "GameEngine/Resources/File.h"

namespace GameEngine
{
class ISceneStorage
{
public:
    virtual ~ISceneStorage() = default;

    virtual void store()                   = 0;
    virtual void saveToFile(const File&)   = 0;
    virtual void readFromFile(const File&) = 0;
};
}  // namespace GameEngine
