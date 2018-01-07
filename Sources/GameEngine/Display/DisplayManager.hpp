#pragma once
#include "../Api/SDL2/SDLOpenGL.h"
#include "TimeMeasurer.h"
#include "Utils.h"
#include <memory>
#include <string>

class CApi;
class CInput;

class CDisplayManager
{
public:
	CDisplayManager();
	CDisplayManager(const std::string& window_name, const int& w, const int& h, bool full_screen);
        CDisplayManager(std::unique_ptr<CApi> api, const std::string& window_name, const int& w, const int& h, bool full_screen);
        ~CDisplayManager();
	ApiMessages::Type PeekApiMessage();
	void Update();

	void SetRefreshRate(const int& rate);
	void SetFullScreen(bool full_screen);

	const int GetFps();
    const int& GetFPSCap() { return fpsCap; }

	const float GetCurrentTime();
    const double GetDeltaTime() const { return timeMeasurer.GetDeltaTime(); }

	const wb::vec2i& GetWindowSize();

	void ShowCoursor(bool show);
	bool CheckActiveWindow();

	void SetApi(std::unique_ptr<CApi>& api);
	void SetInput(std::unique_ptr<CInput>&);

    void EnableTime() { time = true; }
    void DisableTime() { time = false; }

    bool& GetSync() { return sync; }

private:
	void PrintFps();

private:
    std::unique_ptr<CApi> api;
	GameEngine::Time::CTimeMeasurer timeMeasurer;

    bool time = true; 

    int fpsCap;
    bool isFullScreen;
    wb::vec2i windowsSize;

    bool sync = true;
};
