#pragma once
#include "../Display/DisplayManager.hpp"
#include "../Input/InputManager.h"
#include "../Resources/ResourceManager.h"
#include "Projection.h"

class CScene;
class CRenderer;
class CDisplayManager;
class CLoadingScreenRenderer;

class CEngine
{
public:
    CEngine();
    ~CEngine();
    void Init();
    void PreperaScene();
    void GameLoop();

public:
    CDisplayManager& GetDisplayManager();
    CProjection projection;
    CInputManager inputManager;
    std::unique_ptr<CScene> scene;
    std::vector<std::unique_ptr<CRenderer>> renderers;

private:
    void ReadConfigFile(const std::string& file_name);
    void SetDisplay();
    ApiMessages::Type MainLoop();
    void SetDefaultRenderer();
    void LoadScene();
    void InitRenderers();
    void RenderScene();
    void Render(CRenderer*);
    void LoadObjects();

    ApiMessages::Type ProccesScene();
    ApiMessages::Type CheckSceneMessages();
    ApiMessages::Type PrepareFrame();

private:
    // Global items, loading screen etc...
    CResourceManager resorceManager;
    std::unique_ptr<CDisplayManager> displayManager;
};
