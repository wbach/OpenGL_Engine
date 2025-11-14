#include <Logger/Log.h>
#include <gtest/gtest.h>

#include <cmath>

#include "DebugTools/Painter/Interpolation.h"

using namespace GameEngine;

constexpr float EPS = 1e-4f;

// -----------------------------------------------------------------------------
// Hard interpolation
// -----------------------------------------------------------------------------
TEST(InterpolationTest, HardInterpolation)
{
    auto interp      = makeInterpolation(InterpolationType::Hard);
    uint32 brushSize = 10;

    EXPECT_NEAR(interp(vec2i(0, 0), vec2i(0, 0), brushSize), 1.0f, EPS);
    EXPECT_NEAR(interp(vec2i(5, 0), vec2i(0, 0), brushSize), 1.0f, EPS);
    EXPECT_NEAR(interp(vec2i(10, 0), vec2i(0, 0), brushSize), 1.0f, EPS);
    EXPECT_NEAR(interp(vec2i(15, 0), vec2i(0, 0), brushSize), 0.0f, EPS);
}

// -----------------------------------------------------------------------------
// Linear interpolation
// -----------------------------------------------------------------------------
TEST(InterpolationTest, LinearInterpolation)
{
    auto interp      = makeInterpolation(InterpolationType::Linear);
    uint32 brushSize = 10;

    EXPECT_NEAR(interp(vec2i(0, 0), vec2i(0, 0), brushSize), 1.0f, EPS);
    EXPECT_NEAR(interp(vec2i(5, 0), vec2i(0, 0), brushSize), 0.5f, 0.05f);
    EXPECT_NEAR(interp(vec2i(10, 0), vec2i(0, 0), brushSize), 0.0f, EPS);
    EXPECT_NEAR(interp(vec2i(20, 0), vec2i(0, 0), brushSize), 0.0f, EPS);
}

TEST(InterpolationTest, LinearInterpolationProgressive)
{
    auto interp      = makeInterpolation(InterpolationType::Linear);
    uint32 brushSize = 10;

    // Wartość w centrum powinna być 1
    float prev = interp(vec2i(0, 0), vec2i(0, 0), brushSize);
    EXPECT_NEAR(prev, 1.0f, EPS);

    // Sprawdzamy wartości co 1 jednostkę od środka do krawędzi
    for (uint32 d = 1; d <= 10; ++d)
    {
        float val = interp(vec2i(d, 0), vec2i(0, 0), brushSize);
        LOG_DEBUG << "dist=" << d << " -> " << val;

        // powinno monotonicznie maleć
        EXPECT_LE(val, prev + EPS) << "Value increased at dist=" << d;
        EXPECT_GE(val, 0.0f - EPS);
        EXPECT_LE(val, 1.0f + EPS);

        prev = val;
    }

    // Poza zasięgiem brushSize wartość powinna być ~0
    float beyond = interp(vec2i(15, 0), vec2i(0, 0), brushSize);
    EXPECT_NEAR(beyond, 0.0f, EPS);
}

// -----------------------------------------------------------------------------
// Smooth interpolation (smoothstep)
// -----------------------------------------------------------------------------
TEST(InterpolationTest, SmoothInterpolation)
{
    auto interp      = makeInterpolation(InterpolationType::Smooth);
    uint32 brushSize = 10;

    float center = interp(vec2i(0, 0), vec2i(0, 0), brushSize);
    float mid    = interp(vec2i(5, 0), vec2i(0, 0), brushSize);
    float edge   = interp(vec2i(10, 0), vec2i(0, 0), brushSize);

    EXPECT_NEAR(center, 1.0f, EPS);
    EXPECT_GT(mid, 0.0f);
    EXPECT_LT(mid, 1.0f);
    EXPECT_NEAR(edge, 0.0f, EPS);
}

TEST(InterpolationTest, SmoothInterpolationProgressive)
{
    auto interp      = makeInterpolation(InterpolationType::Smooth);
    uint32 brushSize = 10;

    float prev = interp(vec2i(0, 0), vec2i(0, 0), brushSize);
    EXPECT_NEAR(prev, 1.0f, EPS);

    std::vector<float> values;
    values.reserve(brushSize + 1);

    for (uint32 d = 0; d <= brushSize; ++d)
    {
        float val = interp(vec2i(d, 0), vec2i(0, 0), brushSize);
        values.push_back(val);
        LOG_DEBUG << "dist=" << d << " -> " << val;

        // monotonicznie malejące, w zakresie [0,1]
        if (d > 0)
        {
            EXPECT_LE(val, prev + EPS) << "Value increased at dist=" << d;
        }
        EXPECT_GE(val, 0.0f - EPS);
        EXPECT_LE(val, 1.0f + EPS);
        prev = val;
    }

    // Sprawdzenie kilku orientacyjnych relacji charakterystycznych dla smoothstep
    // (wolniejszy spadek na początku, szybszy przy krawędzi)
    EXPECT_GT(values[1], 0.9f);  // blisko środka, bardzo wysoka wartość
    EXPECT_GT(values[3], 0.6f);
    EXPECT_LT(values[7], 0.3f);
    EXPECT_NEAR(values[10], 0.0f, EPS);

    // Poza zasięgiem brushSize -> 0
    float beyond = interp(vec2i(15, 0), vec2i(0, 0), brushSize);
    EXPECT_NEAR(beyond, 0.0f, EPS);
}

// -----------------------------------------------------------------------------
// Gaussian interpolation
// -----------------------------------------------------------------------------
TEST(InterpolationTest, GaussianInterpolation)
{
    auto interp      = makeInterpolation(InterpolationType::Gaussian);
    uint32 brushSize = 10;

    float center = interp(vec2i(0, 0), vec2i(0, 0), brushSize);
    float mid    = interp(vec2i(5, 0), vec2i(0, 0), brushSize);
    float edge   = interp(vec2i(10, 0), vec2i(0, 0), brushSize);

    EXPECT_NEAR(center, 1.0f, EPS);
    EXPECT_GT(mid, 0.0f);
    EXPECT_LT(mid, 1.0f);
    EXPECT_GT(center, mid);
    EXPECT_GT(mid, edge);
}

TEST(InterpolationTest, GaussianInterpolationProgressive)
{
    auto interp      = makeInterpolation(InterpolationType::Gaussian);
    uint32 brushSize = 10;

    float prev = interp(vec2i(0, 0), vec2i(0, 0), brushSize);
    EXPECT_NEAR(prev, 1.0f, EPS);

    std::vector<float> values;
    values.reserve(brushSize + 1);

    for (uint32 d = 0; d <= brushSize; ++d)
    {
        float val = interp(vec2i(d, 0), vec2i(0, 0), brushSize);
        values.push_back(val);
        LOG_DEBUG << "dist=" << d << " -> " << val;

        if (d > 0)
        {
            EXPECT_LE(val, prev + EPS);
        }
        EXPECT_GE(val, 0.0f - EPS);
        EXPECT_LE(val, 1.0f + EPS);
        prev = val;
    }

    // Dostosowane oczekiwania do sigma = brushSize / 2 (czyli sigma=5)
    EXPECT_GT(values[1], 0.95f);
    EXPECT_NEAR(values[5], 0.606f, 0.01f);
    EXPECT_NEAR(values[10], 0.135f, 0.01f);

    // Poza zakresem: bardzo małe, ale nie 0
    float beyond = interp(vec2i(20, 0), vec2i(0, 0), brushSize);
    EXPECT_LT(beyond, 0.02f);
}

TEST(InterpolationTest, LinearInterpolationNegative)
{
    auto interp      = makeInterpolation(InterpolationType::Linear);
    uint32 brushSize = 10;
    vec2i center(0, 0);

    // punkty w różnych kwadrantach
    EXPECT_NEAR(interp(vec2i(-5, 0), center, brushSize), 0.5f, 0.05f);
    EXPECT_NEAR(interp(vec2i(0, -5), center, brushSize), 0.5f, 0.05f);
    EXPECT_NEAR(interp(vec2i(-5, -5), center, brushSize), 1.0f - std::sqrt(25 + 25) / 10.0f, 0.05f);
}

TEST(InterpolationTest, SmoothInterpolationNegative)
{
    auto interp      = makeInterpolation(InterpolationType::Smooth);
    uint32 brushSize = 10;
    vec2i center(0, 0);

    float val1 = interp(vec2i(-5, 0), center, brushSize);
    float val2 = interp(vec2i(5, 0), center, brushSize);

    EXPECT_NEAR(val1, val2, 1e-4f);  // symetria po obu stronach osi X

    val1 = interp(vec2i(0, -5), center, brushSize);
    val2 = interp(vec2i(0, 5), center, brushSize);
    EXPECT_NEAR(val1, val2, 1e-4f);  // symetria po osi Y
}

TEST(InterpolationTest, GaussianInterpolationNegative)
{
    auto interp      = makeInterpolation(InterpolationType::Gaussian);
    uint32 brushSize = 10;
    vec2i center(0, 0);

    float centerVal = interp(vec2i(0, 0), center, brushSize);
    float negVal    = interp(vec2i(-5, -5), center, brushSize);
    float posVal    = interp(vec2i(5, 5), center, brushSize);

    EXPECT_NEAR(centerVal, 1.0f, 1e-4f);
    EXPECT_NEAR(negVal, posVal, 1e-4f);  // symetria po obu kwadrantach
    EXPECT_GT(centerVal, negVal);
}

TEST(InterpolationTest, LinearInterpolationProgressiveNegative)
{
    auto interp      = makeInterpolation(InterpolationType::Linear);
    uint32 brushSize = 10;
    vec2i center(0, 0);

    // Testujemy progresywnie wzdłuż X w przedziale -brushSize .. +brushSize
    for (int x = -static_cast<int>(brushSize); x <= static_cast<int>(brushSize); ++x)
    {
        float val      = interp(vec2i(x, 0), center, brushSize);
        float expected = 1.0f - std::clamp(std::abs(x) / static_cast<float>(brushSize), 0.0f, 1.0f);
        EXPECT_NEAR(val, expected, 0.01f);

        LOG_DEBUG << "x=" << x << " val=" << val;
    }
}

TEST(InterpolationTest, SmoothInterpolationProgressiveNegative)
{
    auto interp      = makeInterpolation(InterpolationType::Smooth);
    uint32 brushSize = 10;
    vec2i center(0, 0);

    for (int x = -static_cast<int>(brushSize); x <= static_cast<int>(brushSize); ++x)
    {
        float val = interp(vec2i(x, 0), center, brushSize);

        float t        = std::clamp(std::abs(x) / static_cast<float>(brushSize), 0.0f, 1.0f);
        float expected = 1.0f - t * t * (3.0f - 2.0f * t);  // smoothstep

        EXPECT_NEAR(val, expected, 0.01f);

        LOG_DEBUG << "x=" << x << " val=" << val;
    }
}

TEST(InterpolationTest, GaussianInterpolationProgressiveNegative)
{
    auto interp      = makeInterpolation(InterpolationType::Gaussian);
    uint32 brushSize = 10;
    vec2i center(0, 0);
    float sigma = static_cast<float>(brushSize) / 2.0f;

    for (int x = -static_cast<int>(brushSize); x <= static_cast<int>(brushSize); ++x)
    {
        float val      = interp(vec2i(x, 0), center, brushSize);
        float expected = std::exp(-(x * x) / (2.0f * sigma * sigma));

        EXPECT_NEAR(val, expected, 0.01f);

        LOG_DEBUG << "x=" << x << " val=" << val;
    }
}

// -----------------------------------------------------------------------------
// ConstValue interpolation
// -----------------------------------------------------------------------------
TEST(InterpolationTest, ConstValueInterpolation)
{
    auto interp      = makeInterpolation(InterpolationType::ConstValue);
    uint32 brushSize = 10;

    EXPECT_NEAR(interp(vec2i(0, 0), vec2i(0, 0), brushSize), 1.0f, EPS);
    EXPECT_NEAR(interp(vec2i(5, 5), vec2i(0, 0), brushSize), 1.0f, EPS);
    EXPECT_NEAR(interp(vec2i(100, 100), vec2i(0, 0), brushSize), 1.0f, EPS);
}

// -----------------------------------------------------------------------------
// Edge cases
// -----------------------------------------------------------------------------
TEST(InterpolationTest, EdgeCasesAll)
{
    using Type = InterpolationType;

    const vec2i center(0, 0);
    const vec2i farPoint(5, 0);

    std::vector<Type> allTypes = {Type::ConstValue, Type::Hard, Type::Linear, Type::Smooth, Type::Gaussian};

    for (auto type : allTypes)
    {
        auto interp = makeInterpolation(type);

        // ------------------------------
        // brushSize = 0
        // ------------------------------
        float val_zero = interp(center, center, 0);
        EXPECT_FALSE(std::isnan(val_zero)) << "NaN for type=" << (int)type;
        EXPECT_NEAR(val_zero, 1.0f, 1e-4f) << "Expected 1.0 when brushSize=0 for type=" << (int)type;

        // ------------------------------
        // dystans > brushSize (np. 5 > 1)
        // ------------------------------
        float val_far = interp(farPoint, center, 1);
        EXPECT_FALSE(std::isnan(val_far)) << "NaN for type=" << (int)type;
        EXPECT_GE(val_far, 0.0f);
        EXPECT_LE(val_far, 1.0f);

        // ------------------------------
        // bardzo duży brushSize (powinno być ~1)
        // ------------------------------
        float val_large = interp(farPoint, center, 1000);
        EXPECT_FALSE(std::isnan(val_large)) << "NaN for type=" << (int)type;
        EXPECT_NEAR(val_large, 1.0f, 0.01f) << "Expected ~1.0 for large brush for type=" << (int)type;
    }
}
