
#include "WinApi.h"

#include <D3D11.h>
#include <Logger/Log.h>
#include <Utils/IdPool.h>
#include <Windows.h>
#include <mutex>

#include "DirectXApi/DirectXContext.h"
#include "XInput/XInputManager.h"

#undef CreateWindow

namespace DirectX
{
namespace
{
Utils::IdPool eventSubscribersEventsPool_;
std::mutex eventSubscribersMutex_;
std::unordered_map<IdType, std::function<void(const GraphicsApi::IWindowApi::Event&)>> eventsSubscribers_;
Input::InputManager* inputManagerForProcPtr{nullptr};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (msg)
    {
        case WM_CREATE:
            DragAcceptFiles(hwnd, TRUE);
            break;

        case WM_DROPFILES:
        {
            HDROP drop           = (HDROP)wParam;
            auto filePathesCount = DragQueryFile(drop, 0xFFFF, (LPSTR)NULL, 0);
            wchar_t* fileName    = NULL;

            // If NULL as the third parameter: return the length of the path, not counting the trailing '0'
            UINT longestFileNameLength = 0;
            for (UINT i = 0; i < filePathesCount; ++i)
            {
                // If NULL as the third parameter: return the length of the path, not counting the trailing '0'
                UINT fileNameLength = DragQueryFileW(drop, i, NULL, 512) + 1;
                if (fileNameLength > longestFileNameLength)
                {
                    longestFileNameLength = fileNameLength;
                    fileName              = (wchar_t*)realloc(fileName, longestFileNameLength * sizeof(*fileName));
                }
                DragQueryFileW(drop, i, fileName, fileNameLength);
                LOG_ERROR << "fileName : " << reinterpret_cast<char*>(fileName);
            }

            std::lock_guard<std::mutex> lk(eventSubscribersMutex_);
            for (const auto& [_, subscriber] : eventsSubscribers_)
            {
                subscriber(GraphicsApi::DropFileEvent{reinterpret_cast<char*>(fileName)});
            }

            free(fileName);
            DragFinish(drop);
        }

        break;
        case WM_KEYDOWN:
            inputManagerForProcPtr->AddKeyEvent(WM_KEYDOWN, wParam);
            break;
        case WM_KEYUP:
            inputManagerForProcPtr->AddKeyEvent(WM_KEYUP, wParam);
            break;
        case WM_LBUTTONDOWN:
            inputManagerForProcPtr->AddKeyEvent(WM_KEYDOWN, VK_LBUTTON);
            break;
        case WM_LBUTTONUP:
            inputManagerForProcPtr->AddKeyEvent(WM_KEYUP, VK_LBUTTON);
            break;
        case WM_RBUTTONDOWN:
            inputManagerForProcPtr->AddKeyEvent(WM_KEYDOWN, VK_RBUTTON);
            break;
        case WM_RBUTTONUP:
            inputManagerForProcPtr->AddKeyEvent(WM_KEYUP, VK_RBUTTON);
            break;
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            break;

        case WM_CLOSE:
        {
            std::lock_guard<std::mutex> lk(eventSubscribersMutex_);
            for (const auto& [_, subscriber] : eventsSubscribers_)
            {
                subscriber(GraphicsApi::QuitEvent{});
            }

            DestroyWindow(hwnd);
        }
        break;

        case WM_DESTROY:
            // CleanD3D();
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
}  // namespace

struct WinApi::Pimpl
{
    Pimpl(DirectXContext& directXContext)
        : directXContext_(directXContext)
    {
    }

    RECT rect_;
    MSG message_;
    HINSTANCE hInstance_;
    LPSTR mainWindowClassName_ = (LPSTR) "Klasa Okienka";
    DirectXContext& directXContext_;
};

WinApi::WinApi(DirectXContext& directXContext)
{
    impl_ = std::make_unique<Pimpl>(directXContext);
}
WinApi::~WinApi()
{
    LOG_DEBUG << "destructor";
}
void WinApi::Init()
{
    RegiesterWindowClass();
    dispalyModes_ = {{640, 480, 60, 0}, {1280, 800, 60, 0}, {1366, 768, 60, 0}, {1920, 1080, 60, 0}};
}
void WinApi::CreateGameWindow(const std::string& window_name, uint32 width, uint32 height, GraphicsApi::WindowType full_screen)
{
    RECT r{0, 0, width, height};
    windowSize   = {width, height};
    impl_->rect_ = r;
    AdjustWindowRect(&impl_->rect_, WS_OVERLAPPEDWINDOW, FALSE);
    impl_->directXContext_.mainWindow = CreateWindowEx(
        WS_EX_CLIENTEDGE, impl_->mainWindowClassName_, window_name.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        impl_->rect_.right - impl_->rect_.left, impl_->rect_.bottom - impl_->rect_.top, NULL, NULL, impl_->hInstance_, NULL);

    if (impl_->directXContext_.mainWindow == NULL)
    {
        MessageBox(NULL, "Can not create a window.", "Error", MB_ICONEXCLAMATION);
        return;
    }

    inputManager_ =
        std::make_unique<XInputManager>(impl_->directXContext_.mainWindow, vec2ui(impl_->rect_.right, impl_->rect_.bottom));
    inputManagerForProcPtr = inputManager_.get();

    ::ShowWindow(impl_->directXContext_.mainWindow, SW_SHOWDEFAULT);
}
void WinApi::SetWindowSize(const vec2ui&)
{
}
vec2ui WinApi::GetWindowSize() const
{
    return windowSize;
}
void WinApi::CreateContext()
{
}
void WinApi::DeleteContext()
{
}
void WinApi::ProcessEvents()
{
    MessageLoop();
}
void WinApi::UpdateWindow()
{
    impl_->directXContext_.swapchain->Present(0, 0);
}
IdType WinApi::SubscribeForEvent(std::function<void(const GraphicsApi::IWindowApi::Event&)> f)
{
    std::lock_guard<std::mutex> lk(eventSubscribersMutex_);
    auto id = eventSubscribersEventsPool_.getId();
    eventsSubscribers_.insert({id, f});
    return id;
}
void WinApi::UnsubscribeForEvent(IdType id)
{
    std::lock_guard<std::mutex> lk(eventSubscribersMutex_);
    auto iter = eventsSubscribers_.find(id);
    if (iter != eventsSubscribers_.end())
    {
        eventSubscribersEventsPool_.releaseId(id);
        eventsSubscribers_.erase(iter);
    }
}
void WinApi::SetFullScreen(bool full_screen)
{
}
bool WinApi::CheckActiveWindow()
{
    return false;
}
void WinApi::BeginFrame()
{
}
void WinApi::ShowCursor(bool show)
{
    ::ShowCursor(show);
}
Input::InputManager& WinApi::GetInputManager()
{
    assert(inputManager_);
    return *inputManager_;
}
double WinApi::GetTime()
{
    return 0.0;
}
void WinApi::SetCursorPosition(int x, int y)
{
}
uint32 WinApi::CreateWindowFlags(GraphicsApi::WindowType type) const
{
    return uint32();
}
bool WinApi::RegiesterWindowClass()
{
    impl_->hInstance_ = GetModuleHandle(0);

    WNDCLASSEX wc;
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WindowProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = impl_->hInstance_;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = impl_->mainWindowClassName_;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "RegisterClassEx faild", "Error...", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    return true;
}
void WinApi::MessageLoop()
{
    if (PeekMessage(&impl_->message_, NULL, 0, 0, PM_REMOVE))
    {
        // translate keystroke messages into the right format
        TranslateMessage(&impl_->message_);

        // send the message to the WindowProc function
        DispatchMessage(&impl_->message_);

        // check to see if it's time to quit
        if (impl_->message_.message == WM_QUIT)
        {
            exit(0);
        }
    }
}
void WinApi::ProcessSdlEvent() const
{
}
void WinApi::ProccesSdlKeyDown(uint32 type) const
{
}
void WinApi::ShowMessageBox(const std::string& title, const std::string& msg) const
{
    MessageBox(NULL, msg.c_str(), title.c_str(), MB_ICONEXCLAMATION | MB_OK);
}
void WinApi::ShowMessageBox(const std::string& title, const std::string& msg, std::function<void(bool)> func) const
{
    int msgboxID = MessageBox(NULL, msg.c_str(), title.c_str(), MB_ICONEXCLAMATION | MB_YESNO);
    func(msgboxID == IDYES);
}
const std::vector<GraphicsApi::DisplayMode>& WinApi::GetDisplayModes() const
{
    return dispalyModes_;
}
}  // namespace DirectX
