#include "VulkanContext.hpp"

namespace GraphicsApi::Vulkan
{
void VulkanContext::ClearResources()
{
    if (device != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(device);

        for (auto& [_, program] : programs)
        {
            if (program.pipeline != VK_NULL_HANDLE)
            {
                vkDestroyPipeline(device, program.pipeline, nullptr);
                program.pipeline = VK_NULL_HANDLE;
            }

            if (program.layout != VK_NULL_HANDLE)
            {
                vkDestroyPipelineLayout(device, program.layout, nullptr);
                program.layout = VK_NULL_HANDLE;
            }

            if (program.descriptorSetLayout != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorSetLayout(device, program.descriptorSetLayout, nullptr);
                program.descriptorSetLayout = VK_NULL_HANDLE;
            }
        }
        programs.clear();

        if (imageAvailableSemaphore != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
            imageAvailableSemaphore = VK_NULL_HANDLE;
        }

        if (renderFinishedSemaphore != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
            renderFinishedSemaphore = VK_NULL_HANDLE;
        }

        if (commandPool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(device, commandPool, nullptr);
            commandPool = VK_NULL_HANDLE;
        }

        for (VkFramebuffer framebuffer : framebuffers)
        {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }
        framebuffers.clear();

        if (renderPass != VK_NULL_HANDLE)
        {
            vkDestroyRenderPass(device, renderPass, nullptr);
            renderPass = VK_NULL_HANDLE;
        }

        for (VkImageView imageView : swapChainImageViews)
        {
            vkDestroyImageView(device, imageView, nullptr);
        }
        swapChainImageViews.clear();
        swapChainImages.clear();

        vkDestroyImageView(device, depthImageView, nullptr);
        vkDestroyImage(device, depthImage, nullptr);
        vkFreeMemory(device, depthImageMemory, nullptr);

        if (swapChain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(device, swapChain, nullptr);
            swapChain = VK_NULL_HANDLE;
        }

        vkDestroyDevice(device, nullptr);
        device = VK_NULL_HANDLE;
    }

    if (surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(instance, surface, nullptr);
        surface = VK_NULL_HANDLE;
    }

    if (instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(instance, nullptr);
        instance = VK_NULL_HANDLE;
    }
}
}  // namespace GraphicsApi::Vulkan
