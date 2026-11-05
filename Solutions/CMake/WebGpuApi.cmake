if(BUILD_GAME_ENGINE AND BUILD_WEB_GPU_GAME)
    if(NOT EMSCRIPTEN)
    message(FATAL_ERROR
        "BUILD_WEB_GPU_GAME requires the Emscripten toolchain. "
        "Configure with emcmake."
    )
    endif()

    if(EMSCRIPTEN AND BUILD_WEB_GPU_GAME)
        include(External/FreeImage.cmake)
        include(External/SDL2.cmake)
        include(External/SDL2_ttf.cmake)
    endif()

    message(STATUS "Configuring WebGPU backend")

    # --------------------------------------------------
    # WebGPU API library
    # --------------------------------------------------

    include(${CMAKE_CURRENT_LIST_DIR}/Sources/WebGpuApiSources.cmake)

    add_library(WebGpuApiLib STATIC
        ${WebGpuApiSources}
    )

    set_target_properties(WebGpuApiLib PROPERTIES
        OSITION_INDEPENDENT_CODE ON
    )

    target_compile_definitions(WebGpuApiLib PRIVATE
        ENGINE_EXPORTS
    )

    target_compile_options(WebGpuApiLib PRIVATE
        ${ENGINE_COMPILE_FLAGS}
        --use-port=emdawnwebgpu
        -Wno-error=nontrivial-memcall
    )

    target_include_directories(WebGpuApiLib PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/../Sources/WebGpuApi
    )

    target_link_libraries(WebGpuApiLib PUBLIC
        GraphicsApiLib
        InputLib
        UtilsLib
    )

    # --------------------------------------------------
    # WebGPU game executable
    # --------------------------------------------------

    include(${CMAKE_CURRENT_LIST_DIR}/Sources/GameSources.cmake)

    add_executable(WebGpuGame
        ${GameSources}
    )

    set_target_properties(WebGpuGame PROPERTIES
        OUTPUT_NAME "Game"
        SUFFIX ".html"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/WebGpuGame"
    )

    target_compile_options(WebGpuGame PRIVATE
        ${ENGINE_COMPILE_FLAGS}
        -use-port=emdawnwebgpu
    )

    target_link_options(WebGpuGame PRIVATE
        --use-port=emdawnwebgpu
        -Wno-error=nontrivial-memcall
        -sALLOW_MEMORY_GROWTH=1
        "-sMAXIMUM_MEMORY=8GB"
        "-sFETCH=1"
        "-pthread"
        "-sPTHREAD_POOL_SIZE=6"
    )

    target_include_directories(WebGpuGame PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/../Sources/WebGpuApi
    )

    target_link_libraries(WebGpuGame PRIVATE
        GameEngineLib
        WebGpuApiLib
        GraphicsApiLib
        InputLib
        CommonLib
        UtilsLib
        ${BULLET_LIBS}
    )
    set(FFLATE_JS
        "${CMAKE_SOURCE_DIR}/../../Tools/common/fflate/fflate.js"
    )

    set(WEBGPU_PRELOAD_JS
        "${CMAKE_SOURCE_DIR}/WebGpuPreload.js"
    )

    if(NOT EXISTS "${FFLATE_JS}")
        message(FATAL_ERROR "Missing fflate.js: ${FFLATE_JS}")
    endif()

    target_link_options(WebGpuGame PRIVATE
        "--pre-js=${WEBGPU_PRELOAD_JS}"
        "-sFORCE_FILESYSTEM=1"
        "-sSTACK_SIZE=4194304"
    )
    set_property(
        TARGET WebGpuGame
        APPEND PROPERTY LINK_DEPENDS
        "${WEBGPU_PRELOAD_JS}"
    )

    add_custom_command(TARGET WebGpuGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${FFLATE_JS}"
            "$<TARGET_FILE_DIR:WebGpuGame>/fflate.js"
        VERBATIM
    )

    message(STATUS "WebGPU game target: WebGpuGame")
    message(STATUS "WebGPU output directory: ${CMAKE_BINARY_DIR}/WebGpuGame")
endif()
