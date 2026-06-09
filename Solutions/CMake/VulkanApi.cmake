if(BUILD_GAME_ENGINE OR BUILD_GAME)

    include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/VulkanApiSources.cmake)

    if(MSVC)
        add_library(VulkanApiLib STATIC ${VulkanApiSources})
    else()
        add_library(VulkanApiLib SHARED ${VulkanApiSources})
    endif()

    set_target_properties(VulkanApiLib PROPERTIES POSITION_INDEPENDENT_CODE ON)

    target_compile_definitions(VulkanApiLib PRIVATE ENGINE_EXPORTS)
    target_compile_options(VulkanApiLib PRIVATE ${ENGINE_COMPILE_FLAGS})

    target_include_directories(VulkanApiLib
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Sources/VulkanApi
    )

    if(WIN32)
        set(LOCAL_VULKAN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/Windows/Vulkan")
        set(Vulkan_ROOT "${LOCAL_VULKAN_DIR}")
        set(Vulkan_FIND_REGISTRY NEVER)
        find_library(SHADERC_LIB shaderc_shared HINTS "${LOCAL_VULKAN_DIR}/Lib")
        target_include_directories(VulkanApiLib PRIVATE "${LOCAL_VULKAN_DIR}/Include")
    else()
        find_library(SHADERC_LIB shaderc_shared)
    endif()

    find_package(Vulkan REQUIRED)

    target_link_libraries(VulkanApiLib
        PUBLIC
            Vulkan::Vulkan
            InputLib
            UtilsLib
            GraphicsApiLib
        PRIVATE
            ${SHADERC_LIB}
    )

    if(WIN32)
        copy_dll_to_build_dir(VulkanApiLib)
    endif()

endif()