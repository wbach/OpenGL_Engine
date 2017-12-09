#include "DisplayManager.hpp"
#include "../Input/Input.h"
#include "Logger/Log.h"

CDisplayManager::CDisplayManager()
{
}

CDisplayManager::CDisplayManager(const std::string& window_name, const int& w, const int& h, bool full_screen)
    : CDisplayManager(std::make_unique<CSdlOpenGlApi>(), window_name, w, h, full_screen)
{
}

CDisplayManager::CDisplayManager(std::unique_ptr<CApi> capi, const std::string& window_name, const int& w, const int& h, bool full_screen)
    : api(std::move(capi))
	, lastFrameTime(0)
	, fpsCap(60)
	, windowsSize({w, h})
{
	if (api == nullptr)
	{
		Log("[Error] API not set!.Press any key.");
		return;
	}

    api->CreateOpenGLWindow(window_name, w, h, full_screen);
}

CDisplayManager::~CDisplayManager()
{
   Log(__func__);
}

ApiMessages::Type CDisplayManager::PeekApiMessage()
{
	if (api != nullptr)
		return	api->PeekMessages();
	return ApiMessages::NONE;
}

void CDisplayManager::Update()
{
	CalculateFPS();
	double current_frame_time = GetCurrentTime();
	deltaTime = (current_frame_time - lastFrameTime);
	lastFrameTime = current_frame_time;

	if (api != nullptr)
		api->UpdateWindow();

	if (api != nullptr && sync)
		api->LockFps((float) fpsCap);
}

void CDisplayManager::SetRefreshRate(const int & rate)
{
	fpsCap = rate;
}

void CDisplayManager::SetFullScreen(bool full_screen)
{
	if (api != nullptr)
		api->SetFullScreen(isFullScreen);
}

void CDisplayManager::CalculateFPS()
{
	frameCount++;
	auto time_interval = CalculateFpsTimeInterval();
	CheckFpsTimeElapsed(time_interval);
}

void CDisplayManager::CheckFpsTimeElapsed(int time_interval)
{
	if (time_interval < 1)
		return;

	fps = frameCount / (time_interval);
	previousTime = currentTime;
	frameCount = 0;
	Log("Fps : " + std::to_string(fps));
}

int CDisplayManager::CalculateFpsTimeInterval()
{
	currentTime = static_cast<float>(api->GetTime());
	return static_cast<int>(currentTime - previousTime);
}

const int CDisplayManager::GetFps()
{
	return static_cast<int>(fps);
}

const float CDisplayManager::GetCurrentTime()
{
	if (api != nullptr && time)
		return static_cast<float>(api->GetTime());
	return 0.f;
}

const wb::vec2i & CDisplayManager::GetWindowSize()
{
	return windowsSize;
}

void CDisplayManager::ShowCoursor(bool show)
{
	if (api != nullptr)
		api->ShowCursor(show);
}

bool CDisplayManager::CheckActiveWindow()
{
	if (api != nullptr)
		return api->CheckActiveWindow();
	return false;
}

void CDisplayManager::SetApi(std::unique_ptr<CApi>& api)
{
	api = std::move(api);
}

void CDisplayManager::SetInput(std::unique_ptr<CInput>& input)
{
	if (api != nullptr)
		api->SetInput(input);
}
