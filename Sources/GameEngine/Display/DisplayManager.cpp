#include "DisplayManager.hpp"
#include "Logger/Log.h"

namespace GameEngine
{
	CDisplayManager::CDisplayManager()
	{
	}

	CDisplayManager::CDisplayManager(const std::string& window_name, const int& w, const int& h, bool full_screen)
		: CDisplayManager(std::make_unique<CSdlOpenGlApi>(), window_name, w, h, full_screen)
	{
	}

	CDisplayManager::CDisplayManager(std::unique_ptr<CApi> capi, const std::string& window_name, const int& w, const int& h, bool full_screen)
		: api(std::move(capi))
		, fpsCap(60)
		, windowsSize({ w, h })
	{
		if (api == nullptr)
		{
			Log("[Error] API not set!.Press any key.");
			return;
		}

		api->CreateOpenGLWindow(window_name, w, h, full_screen);
		timeMeasurer.AddOnTickCallback(std::bind(&CDisplayManager::PrintFps, this));
	}

	CDisplayManager::~CDisplayManager()
	{
		Log(__func__);
	}

	ApiMessages::Type CDisplayManager::PeekApiMessage()
	{
		timeMeasurer.CalculateAndLock();

		if (api != nullptr)
			return	api->PeekMessages();
		return ApiMessages::NONE;
	}

	void CDisplayManager::PrintFps()
	{
		std::string msg = "Thread id : renderer, fps : " + std::to_string(timeMeasurer.GetFps());
		Log(msg);
	}

	void CDisplayManager::Update()
	{
		if (api != nullptr)
			api->UpdateWindow();

		if (api != nullptr && sync)
			api->LockFps((float)fpsCap);

		timeMeasurer.EndFrame();
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

	const int CDisplayManager::GetFps()
	{
		return static_cast<int>(timeMeasurer.GetFps());
	}

	const wb::vec2i& CDisplayManager::GetWindowSize()
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
} // GameEngine
