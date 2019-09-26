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
    virtual void PostInitialize();
    virtual int Update(float) override;

private:
    void KeyOperations();
    void AddMenuButtonAction();
    void ReadGuiFile(const std::string&);
    void ReadMenu();
    void EnableAlginToFirstX();
    void EnableSetTheSameYDistance();

private:
    std::unique_ptr<FileExplorer> fileExplorer_;
    vec2 mousePosition_;
    std::vector<GameEngine::GuiElement*> guiElementsChoose_;
    std::vector<std::string> notCleanLayers_;
    std::string processingFilename_;
    bool multiSelect_;
};
}  // namespace Editor
