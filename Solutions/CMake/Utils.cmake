if(BUILD_UTILS)
    include(./Sources/UtilsSources.cmake)
    add_library(UtilsLib ${UtilsSources})
    if(BUILD_I386)
        set_target_properties(UtilsLib PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif()
endif()