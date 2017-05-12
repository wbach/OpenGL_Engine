#pragma once
#include "../Display/DisplayManager.hpp"
#include "../Input/InputManager.h"
#include "../Scene/Scene.hpp"
#include "../Renderers/LodingScreenRenderer.h"
#include "../Resources/ResourceManager.h"
#include "Projection.h"
#include "Configuration.h"

class CEngine
{
public:
    CEngine();
	int  ReadConfiguration(const std::string& file_name);
	void Init();
	void GameLoop();
	void OpenGLLoadingPass(std::thread& loading_thread);
	void PreperaScene();
	void LoadScene();
	void SetIsLoading(bool is);
	bool GetIsLoading();

    SConfiguration m_Configuration;
	SProjection m_Projection;
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
