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
	void Init();
	void GameLoop();
	void OpenGLLoadingPass(std::thread& loading_thread);
	void PreperaScene();
	void LoadScene();
	void SetIsLoading(bool is);
	bool GetIsLoading();

	CProjection m_Projection;
	CDisplayManager m_DisplayManager;
	CInputManager	m_InputManager;
	std::unique_ptr<CScene> m_Scene;
	std::vector<std::unique_ptr<CRenderer>> m_Renderers;
private:	
	std::unique_ptr<CLoadingScreenRenderer> m_LoadingScreenRenderer;	
	
	//Global items, loading screen etc...
	CResourceManager m_ResorceManager;

	bool m_IsLoading;
	std::mutex m_LoadingMutex;
};
