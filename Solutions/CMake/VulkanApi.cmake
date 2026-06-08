if(BUILD_GAME_ENGINE OR BUILD_GAME)

    # 1. Dołączenie listy plików źródłowych
    include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/VulkanApiSources.cmake)

    # 2. Tworzenie biblioteki (STATIC/SHARED)
    if(MSVC)
        add_library(VulkanApiLib STATIC ${VulkanApiSources})
    else()
        add_library(VulkanApiLib SHARED ${VulkanApiSources})
    endif()

    set_target_properties(VulkanApiLib PROPERTIES POSITION_INDEPENDENT_CODE ON)

    target_compile_definitions(VulkanApiLib PRIVATE ENGINE_EXPORTS)
    target_compile_options(VulkanApiLib PRIVATE ${ENGINE_COMPILE_FLAGS})

    # 3. Poprawiona ścieżka do nagłówków (zmieniono z VulkanApiSources na katalog z nagłówkami)
    target_include_directories(VulkanApiLib
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Sources/VulkanApi
    )

    if(WIN32)
        set(LOCAL_VULKAN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/Windows/Vulkan")
        set(Vulkan_ROOT "${LOCAL_VULKAN_DIR}")
        set(Vulkan_FIND_REGISTRY NEVER)
    endif()

    # 4. Wyszukiwanie Vulkana (wymaga CMake 3.7+)
    find_package(Vulkan REQUIRED)

    # 5. Linkowanie z użyciem oficjalnego targetu nowoczesnego CMake
    target_link_libraries(VulkanApiLib
        PUBLIC
            Vulkan::Vulkan
            InputLib
            UtilsLib
            GraphicsApiLib
    )

    if(WIN32)
        copy_dll_to_build_dir(VulkanApiLib)
    endif()

endif()