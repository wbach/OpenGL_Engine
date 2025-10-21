#include <Logger/Log.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <numeric>
#include <ranges>

#include "DebugTools/Painter/CircleBrush.h"

using namespace GameEngine;

constexpr float EPS = 1e-4f;

void visualizeBrushValues(const CircleBrush& brush)
{
    const auto& influance = brush.getInfluence();

    if (influance.empty())
    {
        LOG_DEBUG << "No influence points to visualize";
        return;
    }

    // Znajdź minimalne i maksymalne x,y
    int minX = 0, maxX = 0, minY = 0, maxY = 0;
    for (const auto& p : influance)
    {
        minX = std::min(minX, static_cast<int>(p.point.x));
        maxX = std::max(maxX, static_cast<int>(p.point.x));
        minY = std::min(minY, static_cast<int>(p.point.y));
        maxY = std::max(maxY, static_cast<int>(p.point.y));
    }

    // Tworzymy mapę floatów
    std::vector<std::vector<float>> map(maxY - minY + 1, std::vector<float>(maxX - minX + 1, 0.0f));

    for (const auto& p : influance)
    {
        int row       = maxY - static_cast<int>(p.point.y);
        int col       = static_cast<int>(p.point.x) - minX;
        map[row][col] = p.influance;
    }

    // Wypisujemy wartości w konsoli
    for (const auto& row : map)
    {
        std::ostringstream line;
        for (float val : row)
        {
            line << std::fixed << std::setprecision(2) << val << " ";
        }
        LOG_DEBUG_RAW << line.str();
    }
}

TEST(CircleBrushTest, InfluencePointsLinear)
{
    // brush radius = 2 w world space
    WorldSpaceBrushRadius worldRadius{2.0f};

    // używamy gotowej funkcji makeInterpolation
    Interpolation linearInterp = makeInterpolation(InterpolationType::Linear);

    CircleBrush brush(linearInterp, worldRadius, 1.f);

    // Krok mapy 1:1, więc localBrushSize = 2
    brush.createInfluance(false, 1.0f, 1.0f);

    const auto& influance = brush.getInfluence();

    // Sprawdzamy liczbę punktów w kole o radius=2, zerowe punkty sa ucinane
    EXPECT_EQ(influance.size(), 9u);

    // Sprawdzenie wartości interpolacji (Linear)
    for (const auto& p : influance)
    {
        float dx       = static_cast<float>(p.point.x);
        float dy       = static_cast<float>(p.point.y);
        float dist     = std::sqrt(dx * dx + dy * dy);
        float expected = 1.0f - std::clamp(dist / 2.0f, 0.0f, 1.0f);  // radius=2
        EXPECT_NEAR(p.influance, expected, EPS);
    }

    // Środek koła
    auto it = std::find_if(influance.begin(), influance.end(),
                           [](const Influance& inf) { return inf.point.x == 0 && inf.point.y == 0; });
    ASSERT_NE(it, influance.end());
    EXPECT_NEAR(it->influance, 1.0f, EPS);

    visualizeBrushValues(brush);
}

TEST(CircleBrushTest, InfluencePointsLinearForEvenSize)
{
    WorldSpaceBrushRadius worldRadius{4.0f};
    Interpolation linearInterp = makeInterpolation(InterpolationType::Linear);
    CircleBrush brush(linearInterp, worldRadius, 1.f);

    brush.createInfluance(true, 1.0f, 1.0f);
    const auto& influance = brush.getInfluence();

    // liczba punktów i zakres wartości
    EXPECT_EQ(influance.size(), 60u);
    for (const auto& p : influance)
    {
        EXPECT_GE(p.influance, 0.0f);
        EXPECT_LE(p.influance, 1.0f);
    }

    // maksimum w środku
    float maxVal = 0.0f;
    glm::ivec2 maxPt{};
    for (const auto& p : influance)
        if (p.influance > maxVal)
        {
            maxVal = p.influance;
            maxPt  = p.point;
        }
    EXPECT_NEAR(maxVal, 1.0f, 1e-3f);
    EXPECT_TRUE(maxPt.x >= -1 && maxPt.x <= 1 && maxPt.y >= -1 && maxPt.y <= 1);

    // sprawdzenie trendu malejącego na poziomie pierścieni
    std::map<int, std::vector<float>> rings;
    for (const auto& p : influance)
    {
        float dist = std::sqrt(float(p.point.x * p.point.x + p.point.y * p.point.y));
        rings[int(dist)].push_back(p.influance);
    }

    float prevAvg = 1.1f;
    for (const auto& [r, vals] : rings)
    {
        float avg = std::accumulate(vals.begin(), vals.end(), 0.0f) / vals.size();
        EXPECT_LT(avg, prevAvg + 0.1f) << "avg influence should decrease with distance";
        prevAvg = avg;
    }

    visualizeBrushValues(brush);
}

TEST(CircleBrushTest, LocalBrushSize_HalfStep)
{
    WorldSpaceBrushRadius worldRadius{2.5f};
    Interpolation linearInterp = makeInterpolation(InterpolationType::Linear);
    CircleBrush brush(linearInterp, worldRadius, 1.f);

    brush.createInfluance(false, 0.5f, 0.5f);
    EXPECT_EQ(brush.getLocalBrushSize().value, 5u);
}

TEST(CircleBrushTest, LocalBrushSize_DoubleStep)
{
    WorldSpaceBrushRadius worldRadius{2.5f};
    Interpolation linearInterp = makeInterpolation(InterpolationType::Linear);
    CircleBrush brush(linearInterp, worldRadius, 1.f);

    brush.createInfluance(false, 2.0f, 2.0f);
    EXPECT_EQ(brush.getLocalBrushSize().value, 1u);
}

TEST(CircleBrushTest, LocalBrushSize_UnitStep)
{
    WorldSpaceBrushRadius worldRadius{2.5f};
    Interpolation linearInterp = makeInterpolation(InterpolationType::Linear);
    CircleBrush brush(linearInterp, worldRadius, 1.f);

    brush.createInfluance(false, 1.0f, 1.0f);
    EXPECT_EQ(brush.getLocalBrushSize().value, 3u);
}

TEST(CircleBrushTest, LocalBrushSize_DifferentSteps)
{
    WorldSpaceBrushRadius worldRadius{2.5f};
    Interpolation linearInterp = makeInterpolation(InterpolationType::Linear);
    CircleBrush brush(linearInterp, worldRadius, 1.f);

    brush.createInfluance(false, 0.5f, 1.0f);
    EXPECT_EQ(brush.getLocalBrushSize().value, 3u);
}

TEST(CircleBrushTest, InfluanceBelowZero)
{
    WorldSpaceBrushRadius worldRadius{2.5f};
    Interpolation linearInterp = makeInterpolation(InterpolationType::Linear);
    CircleBrush brush(linearInterp, worldRadius, -1.f);

    brush.createInfluance(false, 0.5f, 1.0f);

    const auto& influances = brush.getInfluence();

    auto it = std::min_element(influances.begin(), influances.end(),
                               [](const auto& a, const auto& b) { return a.influance < b.influance; });

    EXPECT_NE(it, influances.end());
    EXPECT_NEAR(-1.f, it->influance, 0.01f);
}

TEST(CircleBrushTest, InfluancZero)
{
    WorldSpaceBrushRadius worldRadius{2.5f};
    Interpolation linearInterp = makeInterpolation(InterpolationType::Linear);
    CircleBrush brush(linearInterp, worldRadius, 0.f);

    brush.createInfluance(false, 0.5f, 1.0f);

    const auto& influances = brush.getInfluence();

    EXPECT_TRUE(influances.empty());
}