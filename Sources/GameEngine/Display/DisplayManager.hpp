#pragma once
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Time/Time.h"
#include "Time/TimeMeasurer.h"
#include "Utils.h"
#include <memory>
#include <string>

namespace GameEngine
{
	class CApi;

	class CDisplayManager
	{
	public:
		CDisplayManager(IGraphicsApiPtr api, const std::string& window_name, const int& w, const int& h, WindowType type);
		~CDisplayManager();
		void ProcessEvents();
		void Update();
		void ShowCoursor(bool show);
		bool CheckActiveWindow();
		std::shared_ptr<InputManager> CreateInput();
		void EnableTime() { time = true; }
		void DisableTime() { time = false; }

		void SetRefreshRate(const int& rate);
		void SetFullScreen(bool full_screen);

		inline const Time& GetTime();

		const int GetFps();
		const wb::vec2i& GetWindowSize();

	private:
		IGraphicsApiPtr api;
		Utils::Time::CTimeMeasurer timeMeasurer;

		Time time_;
		bool sync;
		bool time;
		bool isFullScreen;
		wb::vec2i windowsSize;
	};

	inline const Time& CDisplayManager::GetTime()
	{
		return time_;
	}
} // GameEngine
