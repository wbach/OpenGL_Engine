#pragma once
#include <string>

#include "ISceneFactory.h"

namespace GameEngine
{
class ISceneManager
{
public:
    virtual ~ISceneManager() = default;

    virtual Scene* GetActiveScene()                 = 0;
    virtual void Update()                           = 0;
    virtual void SetActiveScene(const std::string&) = 0;
    virtual void Reset()                            = 0;
    virtual void Stop()                             = 0;
    virtual const IdMap& GetAvaiableScenes() const  = 0;
};
}  // namespace GameEngine
