#include "DisplayManager.hpp"
#include "../Input/Input.h"
#include "../Debug_/Log.h"

CDisplayManager::CDisplayManager()
{}

CDisplayManager::CDisplayManager(const std::string& window_name, const int& w, const int& h, bool full_screen)
	: CDisplayManager(std::make_unique<CSdlOpenGlApi>(), window_name, w, h, full_screen)
{
}

CDisplayManager::CDisplayManager(std::unique_ptr<CApi> api, const std::string& window_name, const int& w, const int& h, bool full_screen)
	: m_Api(std::move(api)) 
	, m_FPS_CAP(60)
	, m_LastFrameTime(0)
	, m_WindowsSize({w, h})
{
	if (m_Api == nullptr) 
	{
		Log("[Error] API not set!.Press any key.");
		return;
	}
		
	m_Api->CreateOpenGLWindow(window_name, w, h, full_screen);
}

ApiMessages::Type CDisplayManager::PeekMessage()
{
	if (m_Api != nullptr)
		return	m_Api->PeekMessages();
	return ApiMessages::NONE;
}

void CDisplayManager::Update()
{
	CalculateFPS();
	double current_frame_time = GetCurrentTime();
	m_Delta = (current_frame_time - m_LastFrameTime);
	m_LastFrameTime = current_frame_time;

	if (m_Api != nullptr)
		m_Api->UpdateWindow();

	if (m_Api != nullptr && m_Sync)
		m_Api->LockFps((float)m_FPS_CAP);
}

void CDisplayManager::SetRefreshRate(const int & rate)
{
	m_FPS_CAP = rate;
}

void CDisplayManager::SetFullScreen(bool full_screen)
{
	if (m_Api != nullptr)
		m_Api->SetFullScreen(m_IsFullScreen);
}

void CDisplayManager::CalculateFPS()
{
	m_FrameCount++;

	m_CurrentTime = static_cast<float>(m_Api->GetTime());

	int time_interval = (int)(m_CurrentTime - m_PreviousTime);

	if (time_interval > 1)
	{
		m_Fps = m_FrameCount / (time_interval);
		m_PreviousTime = m_CurrentTime;
		m_FrameCount = 0;
	}
}

const int CDisplayManager::GetFps()
{
	return (int)m_Fps;
}

const float CDisplayManager::GetCurrentTime()
{
	if (m_Api != nullptr && m_Time)
		return (float)m_Api->GetTime();
	return 0.f;
}

const wb::vec2i & CDisplayManager::GetWindowSize()
{
	return m_WindowsSize;
}

void CDisplayManager::ShowCoursor(bool show)
{
	if (m_Api != nullptr)
		m_Api->ShowCursor(show);
}

bool CDisplayManager::CheckActiveWindow()
{
	if (m_Api != nullptr)
		return m_Api->CheckActiveWindow();
	return false;
}

void CDisplayManager::SetApi(std::unique_ptr<CApi>& api)
{
	m_Api = std::move(api);
}

void CDisplayManager::SetInput(std::unique_ptr<CInput>& input)
{
	if (m_Api != nullptr)
		m_Api->SetInput(input);
}
