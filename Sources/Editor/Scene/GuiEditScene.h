#pragma once
#include "GameEngine/Scene/Scene.hpp"
#include "FileExplorer.h"
#include <memory>

namespace Editor
{
class GuiEditScene : public GameEngine::Scene
{
public:
    GuiEditScene();
    virtual ~GuiEditScene() override;
    virtual int Initialize();
    virtual void PostInitialize()
    {
    }
    virtual int Update(float) override;

private:
    std::unique_ptr<FileExplorer> fileExplorer_;
};
}  // namespace Editor
