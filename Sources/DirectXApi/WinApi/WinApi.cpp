
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
std::string WideToUtf8(const std::wstring& wideStr)
{
    if (wideStr.empty())
        return std::string();
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, &wideStr[0], (int)wideStr.size(), NULL, 0, NULL, NULL);
    std::string strTo(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wideStr[0], (int)wideStr.size(), &strTo[0], sizeNeeded, NULL, NULL);
    return strTo;
}
}  // namespace
class WinApi::Pimpl
{
public:
    Pimpl(DirectXContext& directXContext)
        : directXContext_(directXContext)
    {
    }

    RECT rect_;
    MSG message_;
    HINSTANCE hInstance_;
    LPCWSTR mainWindowClassName_ = L"DirectXWindowClass";
    DirectXContext& directXContext_;

    void Init()
    {
        RegiesterWindowClass();
        dispalyModes_ = {{640, 480, 60, 0}, {1280, 800, 60, 0}, {1366, 768, 60, 0}, {1920, 1080, 60, 0}};
    }

    void CreateGameWindow(const std::string& window_name, uint32 width, uint32 height, GraphicsApi::WindowType full_screen)
    {
        RECT r{0, 0, width, height};
        windowSize = {width, height};
        rect_      = r;
        std::wstring wideTitle(window_name.begin(), window_name.end());

        AdjustWindowRect(&rect_, WS_OVERLAPPEDWINDOW, FALSE);
        directXContext_.mainWindow =
            CreateWindowEx(WS_EX_CLIENTEDGE, mainWindowClassName_, wideTitle.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                           CW_USEDEFAULT, rect_.right - rect_.left, rect_.bottom - rect_.top, nullptr, nullptr, hInstance_, this);

        if (directXContext_.mainWindow != nullptr)
        {
            inputManager_ = std::make_unique<XInputManager>(directXContext_.mainWindow, vec2ui(rect_.right, rect_.bottom));
            ::ShowWindow(directXContext_.mainWindow, SW_SHOWDEFAULT);
        }
        else
        {
            MessageBoxA(NULL, "Can not create a window.", "Error", MB_ICONEXCLAMATION);
        }
    }
    void SetWindowSize(const vec2ui& size)
    {
        RECT wr = {0, 0, static_cast<LONG>(size.x), static_cast<LONG>(size.y)};
        AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

        int windowWidth  = wr.right - wr.left;
        int windowHeight = wr.bottom - wr.top;

        ::SetWindowPos(directXContext_.mainWindow, nullptr, 0, 0, windowWidth, windowHeight,
                       SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

        windowSize = size;
    }

    vec2ui GetWindowSize() const
    {
        return windowSize;
    }

    void UpdateWindow()
    {
        directXContext_.swapchain->Present(0, 0);
    }

    IdType SubscribeForEvent(std::function<void(const GraphicsApi::IWindowApi::Event&)> f)
    {
        std::lock_guard<std::mutex> lk(eventSubscribersMutex_);
        auto id = eventSubscribersEventsPool_.getId();
        eventsSubscribers_.insert({id, f});
        return id;
    }
    void UnsubscribeForEvent(IdType id)
    {
        std::lock_guard<std::mutex> lk(eventSubscribersMutex_);
        auto iter = eventsSubscribers_.find(id);
        if (iter != eventsSubscribers_.end())
        {
            eventSubscribersEventsPool_.releaseId(id);
            eventsSubscribers_.erase(iter);
        }
    }

    const std::vector<GraphicsApi::DisplayMode>& GetDisplayModes() const
    {
        return dispalyModes_;
    }

    Input::InputManager& GetInputManager()
    {
        return *inputManager_;
    }

    bool RegiesterWindowClass();
    void MessageLoop();

private:
    static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    Utils::IdPool eventSubscribersEventsPool_;
    std::mutex eventSubscribersMutex_;
    std::unordered_map<IdType, std::function<void(const GraphicsApi::IWindowApi::Event&)>> eventsSubscribers_;

    uint32 startTime{0};
    bool fullScreenActive{false};
    vec2ui windowSize{0, 0};

    std::function<void(uint32, uint32)> addKeyEvent_;
    std::vector<GraphicsApi::DisplayMode> dispalyModes_;
    std::unique_ptr<Input::InputManager> inputManager_;
};

LRESULT CALLBACK WinApi::Pimpl::StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    WinApi::Pimpl* pThis = nullptr;

    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis                 = (WinApi::Pimpl*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
    }
    else
    {
        pThis = (WinApi::Pimpl*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis)
    {
        return pThis->HandleMessage(hwnd, msg, wParam, lParam);
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT WinApi::Pimpl::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
            HDROP hDrop = reinterpret_cast<HDROP>(wParam);

            UINT fileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);

            for (UINT i = 0; i < fileCount; ++i)
            {
                UINT bufferSize = DragQueryFileW(hDrop, i, nullptr, 0) + 1;
                std::vector<wchar_t> pathBuffer(bufferSize);

                if (DragQueryFileW(hDrop, i, pathBuffer.data(), bufferSize))
                {
                    std::wstring widePath(pathBuffer.data());
                    std::string finalPath = WideToUtf8(widePath);

                    LOG_INFO << "File dropped: " << finalPath;

                    std::lock_guard<std::mutex> lk(eventSubscribersMutex_);
                    for (const auto& [_, subscriber] : eventsSubscribers_)
                    {
                        subscriber(GraphicsApi::DropFileEvent{finalPath});
                    }
                }
            }

            DragFinish(hDrop);
            break;
        }
        break;
        case WM_MOUSEWHEEL:
        {
            short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (wheelDelta > 0)
            {
                inputManager_->AddKeyEvent(WM_KEYDOWN, VK_MBUTTON);
            }
            else if (wheelDelta < 0)
            {
                inputManager_->AddKeyEvent(WM_KEYUP, VK_MBUTTON);
            }
            break;
        }
        case WM_KEYDOWN:
            inputManager_->AddKeyEvent(WM_KEYDOWN, wParam);
            break;
        case WM_KEYUP:
            inputManager_->AddKeyEvent(WM_KEYUP, wParam);
            break;
        case WM_LBUTTONDOWN:
            inputManager_->AddKeyEvent(WM_KEYDOWN, VK_LBUTTON);
            break;
        case WM_LBUTTONUP:
            inputManager_->AddKeyEvent(WM_KEYUP, VK_LBUTTON);
            break;
        case WM_RBUTTONDOWN:
            inputManager_->AddKeyEvent(WM_KEYDOWN, VK_RBUTTON);
            break;
        case WM_RBUTTONUP:
            inputManager_->AddKeyEvent(WM_KEYUP, VK_RBUTTON);
            break;
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            break;
        case WM_SIZE:
            if (wParam != SIZE_MINIMIZED)
            {
            }
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
        case WM_SETCURSOR:
            if (not inputManager_->IsCursorVisible())
            {
                ::SetCursor(NULL);
                return TRUE;
            }
            return DefWindowProc(hwnd, msg, wParam, lParam);
        case WM_DESTROY:
            // CleanD3D();
            PostQuitMessage(0);
            break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool WinApi::Pimpl::RegiesterWindowClass()
{
    hInstance_ = GetModuleHandle(0);

    WNDCLASSEXW wc{.cbSize        = sizeof(WNDCLASSEXW),
                   .style         = CS_HREDRAW | CS_VREDRAW,
                   .lpfnWndProc   = StaticWindowProc,
                   .cbClsExtra    = 0,
                   .cbWndExtra    = 0,
                   .hInstance     = hInstance_,
                   .hIcon         = LoadIcon(NULL, IDI_APPLICATION),
                   .hCursor       = LoadCursor(NULL, IDC_ARROW),
                   .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
                   .lpszMenuName  = NULL,
                   .lpszClassName = mainWindowClassName_,
                   .hIconSm       = LoadIcon(NULL, IDI_APPLICATION)};

    if (not RegisterClassExW(&wc))
    {
        MessageBoxW(NULL, L"RegisterClassEx failed", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    return true;
}
void WinApi::Pimpl::MessageLoop()
{
    while (PeekMessage(&message_, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message_);
        DispatchMessage(&message_);

        if (message_.message == WM_QUIT)
        {
            exit(0);
        }
    }
}

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
    impl_->Init();
}
void WinApi::CreateGameWindow(const std::string& windowName, uint32 width, uint32 height, GraphicsApi::WindowType fullScreen)
{
    impl_->CreateGameWindow(windowName, width, height, fullScreen);
}
void WinApi::SetWindowSize(const vec2ui& size)
{
    impl_->SetWindowSize(size);
}
vec2ui WinApi::GetWindowSize() const
{
    return impl_->GetWindowSize();
}
void WinApi::CreateContext()
{
}
void WinApi::DeleteContext()
{
}
void WinApi::ProcessEvents()
{
    impl_->MessageLoop();
}
void WinApi::UpdateWindow()
{
    impl_->UpdateWindow();
}
IdType WinApi::SubscribeForEvent(std::function<void(const GraphicsApi::IWindowApi::Event&)> f)
{
    return impl_->SubscribeForEvent(f);
}
void WinApi::UnsubscribeForEvent(IdType id)
{
    impl_->UnsubscribeForEvent(id);
}
void WinApi::SetFullScreen(bool full_screen)
{
    if (impl_->directXContext_.swapchain)
    {
        HRESULT hr = impl_->directXContext_.swapchain->SetFullscreenState(full_screen ? TRUE : FALSE, nullptr);

        if (FAILED(hr))
        {
            LOG_ERROR << "Failed to set fullscreen state. HRESULT: " << std::hex << hr;
        }
    }
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
    impl_->GetInputManager().ShowCursor(show);

    if (show)
    {
        while (::ShowCursor(TRUE) < 0)
            ;
    }
    else
    {
        while (::ShowCursor(FALSE) >= 0)
            ;
    }
}
Input::InputManager& WinApi::GetInputManager()
{
    return impl_->GetInputManager();
}
double WinApi::GetTime()
{
    return 0.0;
}
void WinApi::SetCursorPosition(int x, int y)
{
    POINT pt = {x, y};

    if (::ClientToScreen(impl_->directXContext_.mainWindow, &pt))
    {
        ::SetCursorPos(pt.x, pt.y);
    }
}
void WinApi::ShowMessageBox(const std::string& title, const std::string& msg) const
{
    MessageBoxA(NULL, msg.c_str(), title.c_str(), MB_ICONEXCLAMATION | MB_OK);
}
void WinApi::ShowMessageBox(const std::string& title, const std::string& msg, std::function<void(bool)> func) const
{
    int msgboxID = MessageBoxA(NULL, msg.c_str(), title.c_str(), MB_ICONEXCLAMATION | MB_YESNO);
    func(msgboxID == IDYES);
}
const std::vector<GraphicsApi::DisplayMode>& WinApi::GetDisplayModes() const
{
    return impl_->GetDisplayModes();
}
}  // namespace DirectX
