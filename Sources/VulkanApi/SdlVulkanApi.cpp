#include "SdlVulkanApi.h"

#include <Logger/Log.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <cassert>
#include <set>

#include "InputSDL.h"
namespace GraphicsApi::Vulkan
{
namespace
{
struct QueueFamilyIndices
{
    uint32 graphicsFamily = 0xFFFFFFFF;
    uint32 presentFamily  = 0xFFFFFFFF;
};

bool CreateVulkanInstance(VulkanContext& vkContext, SDL_Window* window)
{
    uint32 extensionCount = 0;
    if (SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr) == SDL_FALSE)
    {
        LOG_ERROR << "Error: Unable to query Vulkan instance extensions from SDL2\n";
        return false;
    }

    std::vector<const char*> extensions(extensionCount);
    SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions.data());

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
        LOG_ERROR << "Error: Failed to create Vulkan instance\n";
        return false;
    }

    return true;
}

bool CreateVulkanSurface(VulkanContext& vkContext, SDL_Window* window)
{
    if (SDL_Vulkan_CreateSurface(window, vkContext.instance, &vkContext.surface) == SDL_FALSE)
    {
        LOG_ERROR << "Error: SDL2 failed to create a Vulkan surface\n";
        return false;
    }

    return true;
}

QueueFamilyIndices FindQueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    QueueFamilyIndices indices;

    uint32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    for (uint32 i = 0; i < queueFamilyCount; ++i)
    {
        if ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
        if (presentSupport == VK_TRUE)
        {
            indices.presentFamily = i;
        }

        if (indices.graphicsFamily != 0xFFFFFFFF and indices.presentFamily != 0xFFFFFFFF)
        {
            break;
        }
    }

    return indices;
}

bool CreateLogicalDevice(VulkanContext& vkContext, const QueueFamilyIndices& indices)
{
    std::set<uint32> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
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

    VkPhysicalDeviceFeatures deviceFeatures{};
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos    = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures     = &deviceFeatures;

    const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    deviceCreateInfo.enabledExtensionCount   = static_cast<uint32>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceCreateInfo.enabledLayerCount       = 0;

    if (vkCreateDevice(vkContext.physicalDevice, &deviceCreateInfo, nullptr, &vkContext.device) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to create logical Vulkan device (VkDevice).\n";
        return false;
    }

    vkGetDeviceQueue(vkContext.device, indices.graphicsFamily, 0, &vkContext.graphicsQueue);
    vkGetDeviceQueue(vkContext.device, indices.presentFamily, 0, &vkContext.presentQueue);
    vkContext.graphicsFamilyIndex = indices.graphicsFamily;
    vkContext.presentFamilyIndex  = indices.presentFamily;

    return true;
}

VkSurfaceFormatKHR ChooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
{
    for (const auto& availableFormat : formats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB and
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return formats.empty() ? VkSurfaceFormatKHR{} : formats.front();
}

VkPresentModeKHR ChooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& presentModes)
{
    for (const auto& availablePresentMode : presentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D ChooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities, const vec2ui& windowSize)
{
    if (capabilities.currentExtent.width != 0xFFFFFFFF)
    {
        return capabilities.currentExtent;
    }

    return VkExtent2D{
        std::clamp(windowSize.x, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp(windowSize.y, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)};
}

bool CreateSwapChainResources(VulkanContext& vkContext, const vec2ui& windowSize)
{
    VkSurfaceCapabilitiesKHR capabilities{};
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

    const VkSurfaceFormatKHR surfaceFormat = ChooseSwapChainSurfaceFormat(formats);
    const VkPresentModeKHR presentMode     = ChooseSwapChainPresentMode(presentModes);
    const VkExtent2D extent                = ChooseSwapChainExtent(capabilities, windowSize);

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
    swapChainCreateInfo.presentMode    = presentMode;
    swapChainCreateInfo.clipped        = VK_TRUE;
    swapChainCreateInfo.oldSwapchain   = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(vkContext.device, &swapChainCreateInfo, nullptr, &vkContext.swapChain) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to create swapchain!\n";
        return false;
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
            LOG_ERROR << "Error: Failed to create VkImageView for swapchain image at index " << i << "\n";
            return false;
        }
    }

    return true;
}

bool CreateSyncObjects(VulkanContext& vkContext)
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(vkContext.device, &semaphoreInfo, nullptr, &vkContext.imageAvailableSemaphore) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to create semaphore!\n";
        return false;
    }

    if (vkCreateSemaphore(vkContext.device, &semaphoreInfo, nullptr, &vkContext.renderFinishedSemaphore) != VK_SUCCESS)
    {
        LOG_ERROR << "Error: Failed to create renderFinishedSemaphore!\n";
        return false;
    }

    return true;
}
}  // namespace


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

SdlVulkanApi::SdlVulkanApi(VulkanContext& context)
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
    if (!CreateVulkanInstance(vkContext, impl->window))
    {
        return;
    }

    if (!CreateVulkanSurface(vkContext, impl->window))
    {
        vkDestroyInstance(vkContext.instance, nullptr);
        vkContext.instance = VK_NULL_HANDLE;
        return;
    }

    uint32 deviceCount = 0;
    vkEnumeratePhysicalDevices(vkContext.instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        LOG_ERROR << "Error: No Vulkan-capable graphics device was found\n";
        vkDestroySurfaceKHR(vkContext.instance, vkContext.surface, nullptr);
        vkDestroyInstance(vkContext.instance, nullptr);
        vkContext.surface = VK_NULL_HANDLE;
        vkContext.instance = VK_NULL_HANDLE;
        return;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(vkContext.instance, &deviceCount, devices.data());
    vkContext.physicalDevice = devices[0];

    const QueueFamilyIndices queueFamilyIndices = FindQueueFamilyIndices(vkContext.physicalDevice, vkContext.surface);
    if (queueFamilyIndices.graphicsFamily == 0xFFFFFFFF or queueFamilyIndices.presentFamily == 0xFFFFFFFF)
    {
        LOG_ERROR << "Error: Suitable graphics or presentation queue families were not found.\n";
        return;
    }

    if (!CreateLogicalDevice(vkContext, queueFamilyIndices))
    {
        return;
    }

    if (!CreateSwapChainResources(vkContext, windowSize_))
    {
        return;
    }

    if (!CreateSyncObjects(vkContext))
    {
        return;
    }

    LOG_DEBUG << "Success: Vulkan initialized, window and surface created!\n";
    LOG_DEBUG << "Success: Created VkDevice and acquired Graphics and Present queues!";
    LOG_DEBUG << "Success: Created swapchain and " << vkContext.swapChainImageViews.size() << " image views!";
    LOG_DEBUG << "Success: Created swapchain and semaphores!";
}
void SdlVulkanApi::DeleteContext()
{
    vkDestroySemaphore(vkContext.device, vkContext.imageAvailableSemaphore, nullptr);

    if (vkContext.renderFinishedSemaphore != VK_NULL_HANDLE)
    {
        vkDestroySemaphore(vkContext.device, vkContext.renderFinishedSemaphore, nullptr);
        vkContext.renderFinishedSemaphore = VK_NULL_HANDLE;
    }

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
        LOG_ERROR << "Error: Failed to recreate swapchain!\n";
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
            LOG_ERROR << "Error: Failed to recreate VkImageView at index " << i << "\n";
            return;
        }
    }

    LOG_DEBUG << "Success: Swapchain recreated successfully! New size: " << extent.width << "x" << extent.height;
}
}  // namespace GraphicsApi::Vulkan
