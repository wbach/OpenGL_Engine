if(BUILD_WX_EDITOR)
    option(BUILD_UNIT_TESTS "" OFF)

    set(wxUSE_GRAPHICS_DIRECT2D OFF CACHE BOOL "" FORCE)
    set(wxUSE_DIRECT2D OFF CACHE BOOL "" FORCE)
    set(wxUSE_MANIFEST ON CACHE BOOL "" FORCE)

    #find_package(wxWidgets REQUIRED gl core base)
    #include(${wxWidgets_USE_FILE})

    include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/WxEditorSources.cmake)

    set(WX_WIDGETS_DLL_DIR "${CMAKE_CURRENT_BINARY_DIR}/wxWidgets/lib/gcc_x64_dll")
    add_subdirectory(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/wxWidgets/
        ${CMAKE_CURRENT_BINARY_DIR}/wxWidgets
    )

    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/common/wxWidgets/include
         ${CMAKE_CURRENT_BINARY_DIR}/wxWidgets/lib/gcc_x64_dll/mswu
    )

    add_executable(WxEditorExe ${WxEditorSources})

    if (WIN32 AND MINGW)
        target_sources(WxEditorExe PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/app.manifest)
    endif()

    target_compile_options(WxEditorExe PRIVATE ${ENGINE_COMPILE_FLAGS})

    target_include_directories(WxEditorExe
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine
            ${CMAKE_CURRENT_SOURCE_DIR}/Sources/WxEditor
    )

    target_link_libraries(WxEditorExe
        PUBLIC
            GameEngineLib
            OpenGLApiLib
            GraphicsApiLib
            InputLib
            UtilsNetworkLib
            CommonLib
            UtilsLib
            wx::base
            wx::core
            wx::gl
            wx::aui
            wx::propgrid
           # ${wxWidgets_LIBRARIES}
            ${BULLET_LIBS}
            ${LINK_LIBS}
    )

    if(WIN32)
        copy_all_dlls_to_target("${WX_WIDGETS_DLL_DIR}" WxEditorExe)
    endif()
endif()
