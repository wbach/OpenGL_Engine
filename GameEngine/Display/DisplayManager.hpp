#pragma once
#include "../Api/Api.hpp"
#include "../Api/SDL2/SDLOpenGL.h"
#include "../Utils/Utils.h"
#include <memory>
#include <string>
#include <glm/glm.hpp>

class CDisplayManager
{
public:
	CDisplayManager(const std::string& window_name, const int& w, const int& h, bool full_screen);
	CDisplayManager(std::unique_ptr<CApi> api, const std::string& window_name, const int& w, const int& h, bool full_screen);
	ApiMessages::Type PeekMessage();
	void Update();

	void SetRefreshRate(const int& rate);
	void SetFullScreen(bool full_screen);
	void CalculateFPS();
	const int GetFps();
	const int& GetFPSCap() { return m_FPS_CAP; }

	const float GetCurrentTime();
	const double GetDeltaTime() const { return m_Delta; }

	const wb::vec2i& GetWindowSize();

	void ShowCoursor(bool show);
	bool CheckActiveWindow();

	void SetApi(std::unique_ptr<CApi>& api);
	void SetInput(std::unique_ptr<CInput>&);

	void EnableTime() { m_Time = true; }
	void DisableTime() { m_Time = false; }

	bool& GetSync() { return m_Sync; }
private:
	std::unique_ptr<CApi> m_Api;

	bool m_Time = true;

	double m_LastFrameTime;
	double m_Delta;
	float m_CurrentTime, m_PreviousTime;
	float m_FrameCount, m_Fps;

	int m_FPS_CAP;
	bool m_IsFullScreen;
	wb::vec2i m_WindowsSize;

	bool m_Sync = true;
};
