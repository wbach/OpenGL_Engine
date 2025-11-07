#include <Utils/Image/Image.h>
#include <Utils/Image/ImageUtils.h>
#include <benchmark/benchmark.h>

#include <random>

using namespace Utils;

// Pomocnicza funkcja – generuje losowy obraz float
static Image GenerateImageFloat(uint32 width, uint32 height, uint8 channels)
{
    Image img;
    img.width = width;
    img.height = height;
    img.setChannels(channels);

    std::vector<float> data(width * height * channels);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    for (auto& v : data)
        v = dist(rng);

    img.moveData(std::move(data));
    return img;
}

// Pomocnicza funkcja – pusty obraz uint8 (cel)
static Image GenerateImageUint8(uint32 width, uint32 height, uint8 channels)
{
    Image img;
    img.width = width;
    img.height = height;
    img.setChannels(channels);

    std::vector<uint8> data(width * height * channels, 0);
    img.moveData(std::move(data));
    return img;
}

// Generuje wszystkie punkty w obrazie
static std::vector<vec2ui> GeneratePoints(uint32 width, uint32 height)
{
    std::vector<vec2ui> points;
    points.reserve(width * height);
    for (uint32 y = 0; y < height; ++y)
        for (uint32 x = 0; x < width; ++x)
            points.push_back({x, y});
    return points;
}

// Główny benchmark
static void BM_FastCopyPixels(benchmark::State& state)
{
    const uint32 width  = static_cast<uint32>(state.range(0));
    const uint32 height = static_cast<uint32>(state.range(0));

    auto srcImg = GenerateImageFloat(width, height, 4);
    auto dstImg = GenerateImageUint8(width, height, 4);
    auto points = GeneratePoints(width, height);

    const size_t multiCoreThreshold = 10000;

    for (auto _ : state)
    {
        FastCopyPixels(srcImg, dstImg, points, multiCoreThreshold);
        benchmark::ClobberMemory();
    }

    // Informacja o ilości przetworzonych bajtów
    double bytesCopied = static_cast<double>(width) * height * 4 * sizeof(uint8);
    state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(bytesCopied));
}

// Benchmark dla różnych rozmiarów obrazów
BENCHMARK(BM_FastCopyPixels)
    ->Arg(256)    // 256x256
    ->Arg(512)    // 512x512
    ->Arg(1024)   // 1024x1024
    ->Arg(2048)  // 2048x2048
    ->Arg(4096);  // 2048x2048