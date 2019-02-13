#include "WinApi.h"
#include "XInput/XInputManager.h"
#include <Windows.h>
#include <D3D11.h>
#include "DirectXApi/DirectXContext.h"

#undef CreateWindow

namespace DirectX
{
namespace
{
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (msg)
    {
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
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

    MSG message_;
    HINSTANCE hInstance_;
    LPSTR mainWindowClassName_ = (LPSTR) "Klasa Okienka";
    DirectXContext& directXContext_;

    void InitDirectx(HWND hwnd)
    {
        // create a struct to hold information about the swap chain
        DXGI_SWAP_CHAIN_DESC scd;

        // clear out the struct for use
        ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

        // fill the swap chain description struct
        scd.BufferCount = 1;                                    // one back buffer
        scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
        scd.OutputWindow = hwnd;                                // the window to be used
        scd.SampleDesc.Count = 1;                               // how many multisamples
        scd.SampleDesc.Quality = 0;
        scd.Windowed = TRUE;                                    // windowed/full-screen mode

        // create a device, device context and swap chain using the information in the scd struct
        D3D11CreateDeviceAndSwapChain(NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            NULL,
            NULL,
            NULL,
            D3D11_SDK_VERSION,
            &scd,
            &directXContext_.swapchain,
            &directXContext_.dev,
            NULL,
            &directXContext_.devcon);
    }

    void InitViewPort(uint32 width, uint32 height)
    {
        D3D11_VIEWPORT viewport;
        ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = width;
        viewport.Height = height;

        directXContext_.devcon->RSSetViewports(1, &viewport);
        directXContext_.viewPort.size = vec2ui(width, height);
    }
};

WinApi::WinApi(DirectXContext& directXContext)
{
    impl_ = std::make_unique<Pimpl>(directXContext);
}
WinApi::~WinApi()
{
}
void WinApi::CreateWindow(const std::string& window_name, uint32 width, uint32 height,
                          GraphicsApi::WindowType full_screen)
{
    RegiesterWindowClass();

    auto hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, impl_->mainWindowClassName_, window_name.c_str(), WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, impl_->hInstance_, NULL);

    if (hwnd == NULL)
    {
        MessageBox(NULL, "Can not create a window.", "Error", MB_ICONEXCLAMATION);
        return;
    }

    impl_->InitDirectx(hwnd);
    impl_->InitViewPort(width, height);
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);
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
}
std::shared_ptr<Input::InputManager> WinApi::CreateInput()
{
    return std::make_shared<XInputManager>();
}
double WinApi::GetTime()
{
    return 0.0;
}
void WinApi::SetCursorPosition(int x, int y)
{
}
uint32 WinApi::OpenFont(const std::string& filename, uint32 size)
{
    return uint32();
}
GraphicsApi::Surface WinApi::RenderFont(uint32 id, const std::string& text, const vec4& color)
{
    return GraphicsApi::Surface();
}
uint32 WinApi::CreateWindowFlags(GraphicsApi::WindowType type) const
{
    return uint32();
}
void WinApi::CreateSDLWindow(const std::string& window_name, const int& width, const int& height, uint32 flags)
{
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
}  // namespace DirectX
