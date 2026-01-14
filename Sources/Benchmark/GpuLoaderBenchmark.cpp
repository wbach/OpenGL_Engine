#include <benchmark/benchmark.h>
#include <memory>
#include <vector>
#include "GameEngine/Resources/GpuResourceLoader.h"

// --------------------------------------------
// Minimalny fake GpuObject do benchmarku
// --------------------------------------------
class FakeGpuObject : public GameEngine::GpuObject
{
public:
    FakeGpuObject(int id)
        : id_(id)
    {}

    IdType GetGpuObjectId() const override { return id_; }
    const GraphicsApi::ID& GetGraphicsObjectId() const override { return graphicsId_; }

    void GpuLoadingPass() override {}
    void UpdateGpuPass() override {}
    void ReleaseGpuPass() override {}

private:
    IdType id_;
    GraphicsApi::ID graphicsId_;
};

// --------------------------------------------
// Benchmark masowego dodania do release
// --------------------------------------------
static void BM_AddObjectsToRelease(benchmark::State& state)
{
    const int count = state.range(0);

    for (auto _ : state)
    {
        GameEngine::GpuResourceLoader loader;
        std::vector<std::unique_ptr<GameEngine::GpuObject>> objs;
        objs.reserve(count);

        for (int i = 0; i < count; ++i)
        {
            objs.push_back(std::make_unique<FakeGpuObject>(i));
        }

        // Mierzymy dodanie paczki obiektów
        loader.AddObjectsToRelease(std::move(objs));
    }
}

BENCHMARK(BM_AddObjectsToRelease)->Arg(256)->Arg(512)->Arg(1024)->Arg(2048);

// --------------------------------------------
// Benchmark masowego release (RuntimeReleaseObjectGpu)
// --------------------------------------------
static void BM_RuntimeRelease(benchmark::State& state)
{
    const int count = state.range(0);

    for (auto _ : state)
    {
        GameEngine::GpuResourceLoader loader;

        for (int i = 0; i < count; ++i)
        {
            loader.AddObjectToRelease(std::make_unique<FakeGpuObject>(i));
        }

        // Mierzymy proces czyszczenia
        loader.RuntimeGpuTasks();
    }
}

BENCHMARK(BM_RuntimeRelease)->Arg(256)->Arg(512)->Arg(1024)->Arg(2048);

// --------------------------------------------


// static void BM_AddObjectsToReleaseVector(benchmark::State& state)
// {
//     const int count = state.range(0);

//     for (auto _ : state)
//     {
//         // 1. Utworzenie loadera
//         GameEngine::GpuResourceLoader loader;

//         // 2. Przygotowanie paczki obiektów
//         std::vector<std::unique_ptr<GameEngine::GpuObject>> objs;
//         objs.reserve(count);

//         for (int i = 0; i < count; ++i)
//         {
//             objs.push_back(std::make_unique<FakeGpuObject>(i));
//         }

//         // 3. Benchmarkowana operacja
//         loader.AddObjectsToRelease(std::move(objs));
//     }
// }

BENCHMARK(BM_AddObjectsToRelease)
    ->Arg(256)
    ->Arg(512)
    ->Arg(1024)
    ->Arg(2048)
    ->Arg(4096);
