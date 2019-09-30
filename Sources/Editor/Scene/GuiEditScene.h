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
    void AddStartupActions();
    void AddMenuButtonAction();
    void ReadGuiFile(const std::string&);
    void ReadMenu();
    void EnableAlginToFirstX();
    void EnableSetTheSameYDistance();
    void ShowCreateWindow(GameEngine::GuiElementTypes);
    void ReadLastOpenedLocation();
    void SaveLastOpenedLocation();

private:
    std::unique_ptr<FileExplorer> fileExplorer_;
    vec2 mousePosition_;
    std::vector<GameEngine::GuiElement*> guiElementsChoose_;
    std::vector<std::string> notCleanLayers_;
    std::string processingFilename_;
    std::string currentLayer_;
    bool multiSelect_;
    std::string lastOpenedLocation_;
};
}  // namespace Editor
