#include "SdlVulkanApi.h"

#include <Logger/Log.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <cassert>
#include <iostream>

#include "InputSDL.h"

namespace VulkanApi
{

#ifndef USE_GNU
const SDL_MessageBoxButtonData buttons[] = {{0, 0, "no"}, {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "yes"}};
#else
const SDL_MessageBoxButtonData buttons[] = {{SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "yes"}, {0, 0, "no"}};
#endif

struct SdlVulkanApi::Pimpl
{
    SDL_Window* window = nullptr;
    SDL_Event event;
};

SdlVulkanApi::SdlVulkanApi(GraphicsApi::VulkanContext& context)
    : vkContext(context)
{
    SDL_setenv("SDL_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR", "0", 1);
    impl = std::make_unique<Pimpl>();
}

SdlVulkanApi::~SdlVulkanApi()
{
    if (impl->window)
    {
        SDL_DestroyWindow(impl->window);
    }
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
}

void SdlVulkanApi::Init()
{
    SDL_Init(SDL_INIT_VIDEO);
    FillDisplayModes();
}

void SdlVulkanApi::CreateGameWindow(const std::string& window_name, uint32 width, uint32 height,
                                    GraphicsApi::WindowType windowType)
{
    uint32 flags = CreateWindowFlags(windowType);
    CreateSDLWindow(window_name, width, height, flags);

    int w, h;
    SDL_Vulkan_GetDrawableSize(impl->window, &w, &h);
    windowSize_ = vec2ui(static_cast<uint32>(w), static_cast<uint32>(h));

    //    windowSize_ = vec2ui(width, height);
    //  windowSize_ = GetWindowSize();
}

void SdlVulkanApi::SetWindowSize(const vec2ui& size)
{
    SDL_SetWindowSize(impl->window, static_cast<int>(size.x), static_cast<int>(size.y));
    SDL_SetWindowPosition(impl->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

vec2ui SdlVulkanApi::GetWindowSize() const
{
    if (impl->window)
    {
        vec2i createdSize(0);
        SDL_GetWindowSize(impl->window, &(createdSize.x), &(createdSize.y));
        return vec2ui{static_cast<uint32>(createdSize.x), static_cast<uint32>(createdSize.y)};
    }
    return windowSize_;
}

IdType SdlVulkanApi::SubscribeForEvent(std::function<void(const GraphicsApi::IWindowApi::Event&)> f)
{
    std::lock_guard<std::mutex> lk(eventSubscribersMutex_);
    // Pobierz ID ze swojej puli (uproszczone generowanie ID na potrzeby szablonu)
    IdType id = eventsSubscribers_.size() + 1;
    eventsSubscribers_.insert({id, f});
    return id;
}

void SdlVulkanApi::UnsubscribeForEvent(IdType id)
{
    std::lock_guard<std::mutex> lk(eventSubscribersMutex_);
    typename std::unordered_map<IdType, std::function<void(const GraphicsApi::IWindowApi::Event&)>>::iterator iter =
        eventsSubscribers_.find(id);
    if (iter != eventsSubscribers_.end())
    {
        eventsSubscribers_.erase(iter);
    }
}

void SdlVulkanApi::SetFullScreen(bool full_screen)
{
    if (full_screen)
        SDL_SetWindowFullscreen(impl->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    else
        SDL_SetWindowFullscreen(impl->window, 0);
}

bool SdlVulkanApi::CheckActiveWindow()
{
    return (SDL_GetWindowFlags(impl->window) & SDL_WINDOW_INPUT_FOCUS) != 0;
}

void SdlVulkanApi::ShowCursor(bool show)
{
    SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
}

Input::InputManager& SdlVulkanApi::GetInputManager()
{
    assert(inputManager_);
    return *inputManager_;
}

double SdlVulkanApi::GetTime()
{
    return static_cast<double>(SDL_GetTicks() / 1000.0);
}

void SdlVulkanApi::SetCursorPosition(int x, int y)
{
    SDL_WarpMouseInWindow(impl->window, x, y);
}

void SdlVulkanApi::ShowMessageBox(const std::string& title, const std::string& message) const
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title.c_str(), message.c_str(), nullptr);
}

void SdlVulkanApi::ShowMessageBox(const std::string& title, const std::string& msg, std::function<void(bool)> selectedFunc) const
{
    const SDL_MessageBoxData messageboxdata = {SDL_MESSAGEBOX_INFORMATION, impl->window, title.c_str(), msg.c_str(),
                                               SDL_arraysize(buttons),     buttons,      nullptr};
    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0)
    {
        return;
    }

    selectedFunc(buttonid == 1);
}

const std::vector<GraphicsApi::DisplayMode>& SdlVulkanApi::GetDisplayModes() const
{
    return displayModes_;
}

void SdlVulkanApi::FillDisplayModes()
{
    int displayCount = SDL_GetNumVideoDisplays();

    for (int displayIndex = 0; displayIndex < displayCount; ++displayIndex)
    {
        SDL_DisplayMode mode = {SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0};
        for (int modeIndex = 0; modeIndex < SDL_GetNumDisplayModes(displayIndex); ++modeIndex)
        {
            if (SDL_GetDisplayMode(displayIndex, modeIndex, &mode) != 0)
            {
                continue;
            }
            displayModes_.push_back({mode.w, mode.h, mode.refresh_rate, displayIndex});
        }
    }
}

uint32 SdlVulkanApi::CreateWindowFlags(GraphicsApi::WindowType type) const
{
    uint32 flags = SDL_WINDOW_VULKAN;
    switch (type)
    {
        case GraphicsApi::WindowType::WINDOW_BORDERLESS:
            flags |= SDL_WINDOW_BORDERLESS;
            break;
        case GraphicsApi::WindowType::FULL_SCREEN:
            flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
            break;
        case GraphicsApi::WindowType::WINDOW:
            break;
    }
    return flags;
}

void SdlVulkanApi::CreateSDLWindow(const std::string& window_name, int width, int height, uint32 flags)
{
    impl->window = SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);

    inputManager_ = std::make_unique<InputSDL>(impl->window);

    if (impl->window)
        return;

    exit(-1);
}

void SdlVulkanApi::BeginFrame()
{
    startTime = SDL_GetTicks();
}

void SdlVulkanApi::ProcessEvents()
{
    BeginFrame();

    if (inputManager_)
    {
        inputManager_->ApplyPendingChanges();
    }

    while (SDL_PollEvent(&impl->event))
    {
        ProcessSdlEvent();
    }
}

void SdlVulkanApi::ProcessSdlEvent()
{
    switch (impl->event.type)
    {
        case SDL_QUIT:
        {
            std::lock_guard<std::mutex> lk(eventSubscribersMutex_);
            for (const auto& [_, subscriber] : eventsSubscribers_)
            {
                subscriber(GraphicsApi::QuitEvent{});
            }
        }
        break;
        case SDL_MOUSEBUTTONDOWN:
            inputManager_->AddKeyEvent(SDL_KEYDOWN, impl->event.button.button);
            break;
        case SDL_MOUSEBUTTONUP:
            inputManager_->AddKeyEvent(SDL_KEYUP, impl->event.button.button);
            break;
        case SDL_MOUSEWHEEL:
            if (impl->event.wheel.y == -1)
            {
                inputManager_->AddKeyEvent(SDL_KEYUP, SDL_BUTTON_MIDDLE);
            }
            else if (impl->event.wheel.y == 1)
            {
                inputManager_->AddKeyEvent(SDL_KEYDOWN, SDL_BUTTON_MIDDLE);
            }
            break;
        case SDL_KEYDOWN:
            ProccesSdlKeyDown(SDL_KEYDOWN);
            break;
        case SDL_KEYUP:
            ProccesSdlKeyDown(SDL_KEYUP);
            break;
        case SDL_FINGERDOWN:
            break;
        case SDL_DROPFILE:
        {
            std::lock_guard<std::mutex> lk(eventSubscribersMutex_);
            for (const auto& [_, subscriber] : eventsSubscribers_)
            {
                subscriber(GraphicsApi::DropFileEvent{impl->event.drop.file});
            }
        }
        break;
    }
}

void SdlVulkanApi::ProccesSdlKeyDown(uint32 type) const
{
    if (impl->event.key.repeat != 0)
        return;

    auto key      = impl->event.key.keysym.sym;
    auto scanCode = SDL_GetScancodeFromKey(key);

    inputManager_->AddKeyEvent(type, scanCode);
}

void SdlVulkanApi::CreateContext()
{
    uint32 extensionCount = 0;
    if (SDL_Vulkan_GetInstanceExtensions(impl->window, &extensionCount, nullptr) == SDL_FALSE)
    {
        LOG_ERROR << "Blad: Nie mozna pobrac liczby rozszerzen Vulkan z SDL2\n";
        return;
    }

    std::vector<const char*> extensions(extensionCount);
    SDL_Vulkan_GetInstanceExtensions(impl->window, &extensionCount, extensions.data());

    VkApplicationInfo appInfo{};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = "Game Engine Vulkan";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName        = "CustomEngine";
    appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion         = VK_API_VERSION_1_2;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo        = &appInfo;
    createInfo.enabledExtensionCount   = static_cast<uint32>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount       = 0;

    if (vkCreateInstance(&createInfo, nullptr, &vkContext.instance) != VK_SUCCESS)
    {
        LOG_ERROR << "Blad: Nie udalo sie utworzyc instancji Vulkana\n";
        return;
    }

    if (SDL_Vulkan_CreateSurface(impl->window, vkContext.instance, &vkContext.surface) == SDL_FALSE)
    {
        LOG_ERROR << "Blad: SDL2 nie moglo utworzyc powierzchni Vulkan\n";
        vkDestroyInstance(vkContext.instance, nullptr);
        vkContext.instance = VK_NULL_HANDLE;
        return;
    }

    uint32 deviceCount = 0;
    vkEnumeratePhysicalDevices(vkContext.instance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        LOG_ERROR << "Blad: Brak karty graficznej ze wsparciem dla Vulkana\n";
        vkDestroyInstance(vkContext.instance, nullptr);
        vkContext.instance = VK_NULL_HANDLE;
        return;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(vkContext.instance, &deviceCount, devices.data());

    vkContext.physicalDevice = devices[0];

    LOG_DEBUG << "Sukces: Vulkan zainicjalizowany, utworzono okno i surface!\n";

    uint32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vkContext.physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkContext.physicalDevice, &queueFamilyCount, queueFamilies.data());

    // Szukamy indeksów rodzin kolejek wspierających grafikę i prezentację obrazu
    for (uint32 i = 0; i < queueFamilyCount; ++i)
    {
        // Sprawdzenie wsparcia dla grafiki
        if (queueFamilies[i].queueFlags and VK_QUEUE_GRAPHICS_BIT)
        {
            vkContext.graphicsFamilyIndex = i;
        }

        // Sprawdzenie wsparcia dla prezentacji okna SDL2
        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(vkContext.physicalDevice, i, vkContext.surface, &presentSupport);
        if (presentSupport)
        {
            vkContext.presentFamilyIndex = i;
        }

        // Jeśli znaleźliśmy obie (mogą mieć ten sam indeks), przerywamy szukanie
        if (vkContext.graphicsFamilyIndex != 0xFFFFFFFF and vkContext.presentFamilyIndex != 0xFFFFFFFF)
        {
            break;
        }
    }

    if (vkContext.graphicsFamilyIndex == 0xFFFFFFFF or vkContext.presentFamilyIndex == 0xFFFFFFFF)
    {
        LOG_ERROR << "Blad: Nie znaleziono odpowiednich kolejek graficznych lub prezentacji.\n";
        return;
    }

    // --- Tworzenie Logicznego Urządzenia (VkDevice) ---
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32> uniqueQueueFamilies = {vkContext.graphicsFamilyIndex, vkContext.presentFamilyIndex};

    float queuePriority = 1.0f;
    for (uint32 queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount       = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // Definiujemy cechy urządzenia (na razie puste, później dodamy np. Anisotropic Filtering)
    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos    = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures     = &deviceFeatures;

    // Wymagane rozszerzenie dla urządzenia: SWAPCHAIN (umożliwia rysowanie do okna)
    const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    deviceCreateInfo.enabledExtensionCount          = static_cast<uint32>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames        = deviceExtensions.data();
    deviceCreateInfo.enabledLayerCount              = 0;

    if (vkCreateDevice(vkContext.physicalDevice, &deviceCreateInfo, nullptr, &vkContext.device) != VK_SUCCESS)
    {
        LOG_ERROR << "Blad: Nie udalo sie utworzyc urzadzenia logicznego (VkDevice).\n";
        return;
    }

    // Pobranie uchwytów do kolejek z utworzonego urządzenia
    vkGetDeviceQueue(vkContext.device, vkContext.graphicsFamilyIndex, 0, &vkContext.graphicsQueue);
    vkGetDeviceQueue(vkContext.device, vkContext.presentFamilyIndex, 0, &vkContext.presentQueue);

    LOG_DEBUG << "Sukces: Utworzono VkDevice oraz pobrano kolejki Graphics i Present!";

    // --- Nowy kod: Konfiguracja Swapchaina ---

    // 1. Sprawdzanie możliwości karty graficznej w odniesieniu do powierzchni okna
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkContext.physicalDevice, vkContext.surface, &capabilities);

    uint32 formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkContext.physicalDevice, vkContext.surface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkContext.physicalDevice, vkContext.surface, &formatCount, formats.data());

    uint32 presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(vkContext.physicalDevice, vkContext.surface, &presentModeCount, nullptr);
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(vkContext.physicalDevice, vkContext.surface, &presentModeCount,
                                              presentModes.data());

    // 2. Wybór najlepszego formatu (Szukamy 8-bitowego BGRA z przestrzenią sRGB)
    VkSurfaceFormatKHR surfaceFormat = formats[0];
    for (const auto& availableFormat : formats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB and availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            surfaceFormat = availableFormat;
            break;
        }
    }

    // 3. Wybór trybu prezentacji (Szukamy Mailbox / Triple Buffering, jeśli brak - bierzemy klasyczny V-Sync: FIFO)
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (const auto& availablePresentMode : presentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            presentMode = availablePresentMode;
            break;
        }
    }

    // 4. Dopasowanie rozdzielczości do okna
    VkExtent2D extent = capabilities.currentExtent;
    if (extent.width == 0xFFFFFFFF)  // Jeśli system pozwala na dowolność, bierzemy rozmiar z SDL
    {
        extent.width  = std::clamp(windowSize_.x, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        extent.height = std::clamp(windowSize_.y, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }

    // 5. Ustalenie liczby obrazów w Swapchainie (Zazwyczaj 3 dla Triple Buffering)
    uint32 imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 and imageCount > capabilities.maxImageCount)
    {
        imageCount = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapChainCreateInfo{};
    swapChainCreateInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface          = vkContext.surface;
    swapChainCreateInfo.minImageCount    = imageCount;
    swapChainCreateInfo.imageFormat      = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace  = surfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent      = extent;
    swapChainCreateInfo.imageArrayLayers = 1;                                    // 1, chyba że robimy grę VR (stereoskopia)
    swapChainCreateInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;  // Będziemy tu bezpośrednio rysować kolory

    // Jak zarządzać obrazami, jeśli kolejka graficzna i prezentacji to inne urządzenia?
    uint32 queueFamilyIndices[] = {vkContext.graphicsFamilyIndex, vkContext.presentFamilyIndex};

    if (vkContext.graphicsFamilyIndex != vkContext.presentFamilyIndex)
    {
        swapChainCreateInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices   = queueFamilyIndices;
    }
    else
    {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    swapChainCreateInfo.preTransform   = capabilities.currentTransform;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.presentMode    = presentMode;
    swapChainCreateInfo.clipped        = VK_TRUE;
    swapChainCreateInfo.oldSwapchain   = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(vkContext.device, &swapChainCreateInfo, nullptr, &vkContext.swapChain) != VK_SUCCESS)
    {
        LOG_ERROR << "Blad: Nie udalo sie utworzyc Swapchaina!\n";
        return;
    }

    // Zapisujemy format i rozmiar do kontekstu – będą krytyczne przy tworzeniu Shaders/RenderPass
    vkContext.swapChainImageFormat = surfaceFormat.format;
    vkContext.swapChainExtent      = extent;

    // 6. Pobieranie uchwytów do wygenerowanych obrazów przez Swapchain
    vkGetSwapchainImagesKHR(vkContext.device, vkContext.swapChain, &imageCount, nullptr);
    vkContext.swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(vkContext.device, vkContext.swapChain, &imageCount, vkContext.swapChainImages.data());

    // 7. Tworzenie Widoków Obrazu (VkImageView)
    // Sam VkImage to tylko surowa pamięć. Aby Vulkan mógł go użyć jako celu renderowania, potrzebuje "widoku".
    vkContext.swapChainImageViews.resize(vkContext.swapChainImages.size());

    for (size_t i = 0; i < vkContext.swapChainImages.size(); ++i)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image    = vkContext.swapChainImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format   = vkContext.swapChainImageFormat;

        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel   = 0;
        viewInfo.subresourceRange.levelCount     = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount     = 1;

        if (vkCreateImageView(vkContext.device, &viewInfo, nullptr, &vkContext.swapChainImageViews[i]) != VK_SUCCESS)
        {
            LOG_ERROR << "Blad: Nie udalo sie utworzyc VkImageView dla obrazu Swapchaina o indeksie " << i << "\n";
            return;
        }
    }

    LOG_DEBUG << "Sukces: Utworzono Swapchain oraz " << vkContext.swapChainImageViews.size() << " obiektow ImageView!";

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(vkContext.device, &semaphoreInfo, nullptr, &vkContext.imageAvailableSemaphore) != VK_SUCCESS)
    {
        LOG_ERROR << "Blad: Nie udalo sie utworzyc semafora!\n";
        return;
    }

    LOG_DEBUG << "Sukces: Utworzono Swapchain oraz semafory!";
}
void SdlVulkanApi::DeleteContext()
{
    vkDestroySemaphore(vkContext.device, vkContext.imageAvailableSemaphore, nullptr);

    if (vkContext.device != VK_NULL_HANDLE)
    {
        if (vkContext.commandPool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(vkContext.device, vkContext.commandPool, nullptr);
            vkContext.commandPool = VK_NULL_HANDLE;
        }

        // 2. Niszczymy Framebuffers
        for (VkFramebuffer framebuffer : vkContext.framebuffers)
        {
            vkDestroyFramebuffer(vkContext.device, framebuffer, nullptr);
        }
        vkContext.framebuffers.clear();

        // 3. Niszczymy Render Pass
        if (vkContext.renderPass != VK_NULL_HANDLE)
        {
            vkDestroyRenderPass(vkContext.device, vkContext.renderPass, nullptr);
            vkContext.renderPass = VK_NULL_HANDLE;
        }

        for (auto imageView : vkContext.swapChainImageViews)
        {
            vkDestroyImageView(vkContext.device, imageView, nullptr);
        }
        vkContext.swapChainImageViews.clear();
        vkContext.swapChainImages.clear();

        if (vkContext.swapChain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(vkContext.device, vkContext.swapChain, nullptr);
            vkContext.swapChain = VK_NULL_HANDLE;
        }

        vkDestroyDevice(vkContext.device, nullptr);
        vkContext.device = VK_NULL_HANDLE;
    }

    if (vkContext.surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(vkContext.instance, vkContext.surface, nullptr);
        vkContext.surface = VK_NULL_HANDLE;
    }

    if (vkContext.instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(vkContext.instance, nullptr);
        vkContext.instance = VK_NULL_HANDLE;
    }
}
void SdlVulkanApi::UpdateWindow()
{
    if (vkContext.swapChain == VK_NULL_HANDLE)
        return;

    uint32 imageIndex = 0;

    // 1. Pobranie indeksu wolnego obrazu ze Swapchaina
    VkResult result = vkAcquireNextImageKHR(vkContext.device, vkContext.swapChain, UINT64_MAX, vkContext.imageAvailableSemaphore,
                                            VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS and result != VK_SUBOPTIMAL_KHR)
    {
        LOG_ERROR << "Blad podczas pobierania obrazu ze Swapchaina! Kod bledu: " << result;
        return;
    }

    // 2. Nagrywanie komendy czyszczenia ekranu na pobranym obrazie
    VkCommandBuffer commandBuffer = vkContext.commandBuffers[imageIndex];

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        LOG_ERROR << "Blad: Nie udalo sie rozpoczac nagrywania Command Buffera!\n";
        return;
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass        = vkContext.renderPass;
    renderPassInfo.framebuffer       = vkContext.framebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = vkContext.swapChainExtent;

    // Tutaj definiujesz kolor tła (R, G, B, A) w zakresie 0.0f - 1.0f
    VkClearValue clearColor        = {{{vkContext.backgroundColor[0], vkContext.backgroundColor[1], vkContext.backgroundColor[2],
                                        vkContext.backgroundColor[3]}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues    = &clearColor;

    // Rozpoczęcie Render Passu automatycznie wyczyści ekran zdefiniowanym kolorem
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        LOG_ERROR << "Blad: Nie udalo sie zakonczyc nagrywania Command Buffera!\n";
        return;
    }

    // 3. Wysłanie nagranych poleceń do wykonania przez GPU (Kolejka graficzna)
    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    if (vkQueueSubmit(vkContext.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        LOG_ERROR << "Blad podczas wysyłania komend renderowania do kolejki (vkQueueSubmit)!\n";
        return;
    }

    // 4. Przygotowanie prezentacji przetworzonego obrazu na ekranie
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &vkContext.imageAvailableSemaphore;

    VkSwapchainKHR swapChains[] = {vkContext.swapChain};
    presentInfo.swapchainCount  = 1;
    presentInfo.pSwapchains     = swapChains;
    presentInfo.pImageIndices   = &imageIndex;

    result = vkQueuePresentKHR(vkContext.presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR or result == VK_SUBOPTIMAL_KHR)
    {
        RecreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        LOG_ERROR << "Blad prezentacji obrazu!\n";
    }

    // Tymczasowa pełna synchronizacja - czekamy, aż GPU skończy całą pracę,
    // aby bezpiecznie przejść do kolejnej klatki bez ryzyka nadpisania buforów.
    vkQueueWaitIdle(vkContext.presentQueue);
}
void SdlVulkanApi::RecreateSwapChain()
{
    // 1. Czekamy, aż GPU będzie bezczynne
    vkDeviceWaitIdle(vkContext.device);

    // 2. Czyścimy stare zasoby Swapchaina
    for (VkImageView imageView : vkContext.swapChainImageViews)
    {
        vkDestroyImageView(vkContext.device, imageView, nullptr);
    }
    vkContext.swapChainImageViews.clear();
    vkContext.swapChainImages.clear();

    if (vkContext.swapChain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(vkContext.device, vkContext.swapChain, nullptr);
        vkContext.swapChain = VK_NULL_HANDLE;
    }

    // 3. Pobieramy absolutnie świeży, realny rozmiar okna z SDL
    int w = 0;
    int h = 0;
    SDL_Vulkan_GetDrawableSize(impl->window, &w, &h);

    // Jeśli okno jest zminimalizowane (rozmiar 0,0), stopujemy pętlę, dopóki się nie pojawi
    while (w == 0 or h == 0)
    {
        SDL_Vulkan_GetDrawableSize(impl->window, &w, &h);
        SDL_WaitEvent(&impl->event);  // Czekamy na jakikolwiek event systemowy (np. przywrócenie okna)
    }

    windowSize_ = vec2ui(static_cast<uint32>(w), static_cast<uint32>(h));

    // 4. Ponowne uruchomienie logiki tworzenia Swapchaina
    // Pobieramy capabilities dla nowego rozmiaru okna
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkContext.physicalDevice, vkContext.surface, &capabilities);

    uint32 formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkContext.physicalDevice, vkContext.surface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkContext.physicalDevice, vkContext.surface, &formatCount, formats.data());

    VkSurfaceFormatKHR surfaceFormat = formats[0];
    for (const auto& availableFormat : formats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB and availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            surfaceFormat = availableFormat;
            break;
        }
    }

    VkExtent2D extent = capabilities.currentExtent;
    if (extent.width == 0xFFFFFFFF)
    {
        extent.width  = std::clamp(windowSize_.x, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        extent.height = std::clamp(windowSize_.y, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }

    uint32 imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 and imageCount > capabilities.maxImageCount)
    {
        imageCount = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapChainCreateInfo{};
    swapChainCreateInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface          = vkContext.surface;
    swapChainCreateInfo.minImageCount    = imageCount;
    swapChainCreateInfo.imageFormat      = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace  = surfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent      = extent;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32 queueFamilyIndices[] = {vkContext.graphicsFamilyIndex, vkContext.presentFamilyIndex};
    if (vkContext.graphicsFamilyIndex != vkContext.presentFamilyIndex)
    {
        swapChainCreateInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices   = queueFamilyIndices;
    }
    else
    {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    swapChainCreateInfo.preTransform   = capabilities.currentTransform;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.presentMode    = VK_PRESENT_MODE_FIFO_KHR;  // Bezpieczny fallback na V-Sync
    swapChainCreateInfo.clipped        = VK_TRUE;
    swapChainCreateInfo.oldSwapchain   = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(vkContext.device, &swapChainCreateInfo, nullptr, &vkContext.swapChain) != VK_SUCCESS)
    {
        LOG_ERROR << "Blad: Nie udalo sie zrekonstruowac Swapchaina!\n";
        return;
    }

    vkContext.swapChainImageFormat = surfaceFormat.format;
    vkContext.swapChainExtent      = extent;

    vkGetSwapchainImagesKHR(vkContext.device, vkContext.swapChain, &imageCount, nullptr);
    vkContext.swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(vkContext.device, vkContext.swapChain, &imageCount, vkContext.swapChainImages.data());

    vkContext.swapChainImageViews.resize(vkContext.swapChainImages.size());
    for (size_t i = 0; i < vkContext.swapChainImages.size(); ++i)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image                           = vkContext.swapChainImages[i];
        viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format                          = vkContext.swapChainImageFormat;
        viewInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel   = 0;
        viewInfo.subresourceRange.levelCount     = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount     = 1;

        if (vkCreateImageView(vkContext.device, &viewInfo, nullptr, &vkContext.swapChainImageViews[i]) != VK_SUCCESS)
        {
            LOG_ERROR << "Blad: Nie udalo sie zrekonstruowac VkImageView o indeksie " << i << "\n";
            return;
        }
    }

    LOG_DEBUG << "Sukces: Swapchain zrekonstruowany pomyślnie! Nowy rozmiar: " << extent.width << "x" << extent.height;
}
}  // namespace VulkanApi
