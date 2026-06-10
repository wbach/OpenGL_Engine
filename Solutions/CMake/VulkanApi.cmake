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
        # 1. Wymuszamy na CMake respektowanie zmiennej <PackageName>_ROOT
        cmake_policy(SET CMP0074 NEW)

        set(LOCAL_VULKAN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/Windows/Vulkan")
        set(Vulkan_ROOT "${LOCAL_VULKAN_DIR}")

        # 2. Starsze wersje modułu FindVulkan używały VULKAN_SDK zamiast Vulkan_ROOT
        set(ENV{VULKAN_SDK} "${LOCAL_VULKAN_DIR}")
        set(VULKAN_SDK "${LOCAL_VULKAN_DIR}")

        set(Vulkan_FIND_REGISTRY NEVER)

        # ZABEZPIECZENIE DLA CROSS-KOMPILACJI
        set(OLD_FIND_LIBRARY_MODE ${CMAKE_FIND_ROOT_PATH_MODE_LIBRARY})
        set(OLD_FIND_INCLUDE_MODE ${CMAKE_FIND_ROOT_PATH_MODE_INCLUDE})

        set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
        set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)

        find_library(SHADERC_LIB
            NAMES shaderc_shared.dll.a shaderc_shared shaderc_shared.lib shaderc
            HINTS "${LOCAL_VULKAN_DIR}/lib"
            NO_DEFAULT_PATH
        )

        target_include_directories(VulkanApiLib PRIVATE "${LOCAL_VULKAN_DIR}/Include")
    else()
        find_library(SHADERC_LIB shaderc_shared)
    endif()

    # Teraz w pełni bezpieczne i wymuszone wyszukiwanie
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
        set(VULKAN_WIN64_BIN_DIR "${CMAKE_SOURCE_DIR}/Tools/Dlls/Vulkan")
        copy_all_dlls_to_target("${VULKAN_WIN64_BIN_DIR}" VulkanApiLib)
    endif()

endif()