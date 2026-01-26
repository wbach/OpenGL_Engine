if(BUILD_UTILS)
    include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/UtilsSources.cmake)

    if(MSVC)
       add_library(UtilsLib STATIC ${UtilsSources})
    else()
            add_library(UtilsLib SHARED ${UtilsSources})
    endif()


    set_target_properties(UtilsLib PROPERTIES POSITION_INDEPENDENT_CODE ON)


    target_compile_options(UtilsLib PRIVATE ${ENGINE_COMPILE_FLAGS})

    target_include_directories(UtilsLib
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Sources/Utils
    )


    if(MSVC)
        target_link_libraries(UtilsLib
            PUBLIC
                ${LINK_LIBS}
        )
    else()
        target_link_libraries(UtilsLib
            PUBLIC
                stdc++fs
                ${LINK_LIBS}
        )
    endif()

    if(WIN32)
        copy_dll_to_build_dir(UtilsLib)
    endif()

endif()
