if(BUILD_BENCHMARK)
   # --- Include directories ---
    set(BENCHMARK_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Sources/GameEngine
        ${CMAKE_CURRENT_SOURCE_DIR}/Sources/Benchmark
    )

    # --- Google Benchmark ---
    include(FetchContent)
    FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.8.4 # możesz zmienić na nowszą wersję jeśli chcesz
    )

    # Wyłącz wewnętrzne testy Google Benchmark (nie są potrzebne w Twoim projekcie)
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark internal tests" FORCE)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "Disable benchmark GTest integration" FORCE)

    FetchContent_MakeAvailable(benchmark)

    # --- Źródła benchmarków ---
    include(${CMAKE_CURRENT_SOURCE_DIR}/Sources/BenchmarkSources.cmake)

    # --- Executable benchmarkowy ---
    add_executable(Benchmark ${BenchmarkSources})

    # Flagi kompilacji
    target_compile_options(Benchmark PRIVATE ${ENGINE_COMPILE_FLAGS})

    # Include directories
    target_include_directories(Benchmark PRIVATE ${BENCHMARK_INCLUDE_DIRS})

    # Linkowanie bibliotek i targetów
    target_link_libraries(Benchmark
        PRIVATE
            GameEngineLib
            OpenGLApiLib
            GraphicsApiLib
            InputLib
            UtilsNetworkLib
            CommonLib
            UtilsLib
            benchmark::benchmark
            ${UNIX_LINK_LIBS}
            ${BULLET_LIBS}
    )

endif()
