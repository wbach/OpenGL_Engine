#pragma once
#include <memory>
#include "FileExplorer.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
class TreeView;
} // namespace GameEngine

namespace Editor
{
struct Context;

class GuiEditScene : public GameEngine::Scene
{
public:
    GuiEditScene(Context& context);
    ~GuiEditScene() override;
    int Initialize() override;
    void PostInitialize() override;
    int Update(float) override;

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
    void FillGuiElementInTreeObject(const std::unique_ptr<GameEngine::GuiElement>&, std::optional<uint32> parent = {});

private:
    Context& context_;
    std::unique_ptr<FileExplorer> fileExplorer_;
    vec2 mousePosition_;
    std::vector<GameEngine::GuiElement*> guiElementsChoose_;
    std::vector<std::string> notCleanLayers_;
    std::string processingFilename_;
    std::string currentLayer_;
    bool multiSelect_;
    std::string lastOpenedLocation_;
    GameEngine::TreeView* objectTree_;
};
}  // namespace Editor
