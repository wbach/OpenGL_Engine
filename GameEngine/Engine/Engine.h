#pragma once
#include "Projection.h"
#include "../Display/DisplayManager.hpp"
#include "../Input/InputManager.h"
#include "../Resources/ResourceManager.h"
#include "../Renderers/LodingScreenRenderer.h"

class CScene;
class CLoadingScreenRenderer;

class CEngine
{
public:
    CEngine();
	~CEngine();
	void Init();
	void GameLoop();
	void OpenGLLoadingPass(std::thread& loading_thread);
	void PreperaScene();
	void LoadScene();
	void SetIsLoading(bool is);
	bool GetIsLoading();

public:
    CProjection projection;
    CDisplayManager displayManager;
    CInputManager	inputManager;
    std::unique_ptr<CScene> scene;
    std::vector<std::unique_ptr<CRenderer>> renderers;

private:	
    std::unique_ptr<CLoadingScreenRenderer> loadingScreenRenderer;
	
	//Global items, loading screen etc...
    CResourceManager resorceManager;

    bool isLoading;
    std::mutex loadingMutex;
};
