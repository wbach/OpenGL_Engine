#include <gtest/gtest.h>

#include <memory>

#include "GameEngine/Scene/Navigation/GridNavigation.h"

using namespace GameEngine;

class GridNavigationTest : public ::testing::Test
{
protected:
    int width      = 10;
    int height     = 10;
    float cellSize = 1.0f;
    vec3 origin{vec3(0)};
    std::unique_ptr<GridNavigation> sut;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

    void createSut()
    {
        sut = std::make_unique<GridNavigation>(origin, width, height, cellSize);
    }

    void DumpGrid(const std::vector<vec3>& path = {}, vec3 start = {0, 0, 0}, vec3 end = {0, 0, 0})
    {
        const auto& nodes = sut->GetNodes();

        std::vector<std::string> display(height, std::string(width, '.'));

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                if (!nodes[y * width + x].isWalkable)
                {
                    display[y][x] = '#';
                }
            }
        }

        auto worldToLocalIdx = [&](const vec3& p) -> std::pair<int, int>
        {
            int x = static_cast<int>(std::floor((p.x - origin.x) / cellSize));
            int z = static_cast<int>(std::floor((p.z - origin.z) / cellSize));
            return {x, z};
        };

        for (const auto& p : path)
        {
            auto [x, z] = worldToLocalIdx(p);
            if (x >= 0 && x < width && z >= 0 && z < height)
            {
                display[z][x] = '*';
            }
        }

        auto [sx, sz] = worldToLocalIdx(start);
        if (sx >= 0 && sx < width && sz >= 0 && sz < height)
            display[sz][sx] = 'S';

        auto [ex, ez] = worldToLocalIdx(end);
        if (ex >= 0 && ex < width && ez >= 0 && ez < height)
            display[ez][ex] = 'E';

        std::string output =
            "\n--- GRID VISUALIZATION (Origin: " + std::to_string(origin.x) + "," + std::to_string(origin.z) + ") ---\n";

        for (int y = height - 1; y >= 0; --y)
        {
            output += display[y] + "\n";
        }
        output += "--------------------------";

        LOG_DEBUG << output;
    }
};

TEST_F(GridNavigationTest, WorldToGridConversion)
{
    createSut();
    vec3 pos(5.5f, 0.0f, 5.5f);
    auto path = sut->CalculatePath(pos, pos);
    EXPECT_TRUE(path.empty() || path.size() == 1);
}

TEST_F(GridNavigationTest, OutOfBoundsReturnsEmpty)
{
    createSut();
    auto path = sut->CalculatePath({1.0f, 0.0f, 1.0f}, {50.0f, 0.0f, 50.0f});
    EXPECT_TRUE(path.empty());
}

TEST_F(GridNavigationTest, SimpleStraightPath)
{
    createSut();
    vec3 start(0.5f, 0.0f, 0.5f);
    vec3 end(0.5f, 0.0f, 3.5f);

    auto path = sut->CalculatePath(start, end);

    DumpGrid(path, start, end);

    ASSERT_FALSE(path.empty());
    EXPECT_NEAR(path.back().z, 3.5f, 0.1f);
    EXPECT_EQ(path.back().x, 0.5f);
}

TEST_F(GridNavigationTest, ObstacleAvoidance)
{
    createSut();
    sut->SetWalkable(1, 0, false);
    sut->SetWalkable(1, 1, false);
    sut->SetWalkable(1, 2, false);

    glm::vec3 start(0.5f, 0.0f, 1.5f);
    glm::vec3 end(2.5f, 0.0f, 1.5f);

    auto path = sut->CalculatePath(start, end);

    DumpGrid(path, start, end);
    ASSERT_FALSE(path.empty());

    for (const auto& point : path)
    {
        EXPECT_FALSE(point.x == 1.5f && point.z >= 0.0f && point.z <= 2.0f);
    }
}

TEST_F(GridNavigationTest, NoPathFound)
{
    createSut();
    sut->SetWalkable(0, 1, false);
    sut->SetWalkable(1, 1, false);
    sut->SetWalkable(1, 0, false);

    glm::vec3 start(0.5f, 0.0f, 0.5f);
    glm::vec3 end(5.5f, 0.0f, 5.5f);

    auto path = sut->CalculatePath(start, end);

    DumpGrid(path, start, end);

    EXPECT_TRUE(path.empty());
}

TEST_F(GridNavigationTest, CustomPath)
{
    createSut();
    vec3 start(8.5f, 0.0f, 8.5f);
    vec3 end(0.5f, 0.0f, 3.5f);

    for (int x = 2; x < 8; ++x)
    {
        sut->SetWalkable(7, x, false);
    }

    for (int x = 0; x < 8; ++x)
    {
        sut->SetWalkable(x, 7, false);
    }

    auto path = sut->CalculatePath(start, end);

    DumpGrid(path, start, end);

    ASSERT_FALSE(path.empty());
    EXPECT_NEAR(path.back().z, 3.5f, 0.1f);
    EXPECT_EQ(path.back().x, 0.5f);
}

TEST_F(GridNavigationTest, OffsetOriginMapping)
{
    origin = vec3(100.0f, 0.0f, 100.0f);
    createSut();

    const vec3 start(100.5f, 0.0f, 100.5f);  // Indeks [0,0]
    const vec3 end(103.5f, 0.0f, 103.5f);    // Indeks [3,3]

    auto path = sut->CalculatePath(start, end);

    ASSERT_FALSE(path.empty());

    EXPECT_NEAR(path.front().x, 101.5f, 0.1f);
    EXPECT_NEAR(path.front().z, 101.5f, 0.1f);

    EXPECT_NEAR(path.back().x, 103.5f, 0.1f);
    EXPECT_NEAR(path.back().z, 103.5f, 0.1f);
}

TEST_F(GridNavigationTest, OutOfBoundsWithOrigin)
{
    origin = vec3(-50.0f, 0.0f, -50.0f);
    createSut();

    const vec3 start{-60.0f, 0, -60.0f};
    const vec3 end{-45.0f, 0, -45.0f};
    auto path = sut->CalculatePath(start, end);

    DumpGrid(path, start, end);

    EXPECT_TRUE(path.empty());
}

TEST_F(GridNavigationTest, MazeNavigationTest)
{
    origin = vec3(-10.0f, 0.0f, -10.0f);
    width = height = 20;
    cellSize       = 1.0f;
    createSut();

    vec3 startPos(-9.5f, 0.0f, -9.5f);
    vec3 endPos(9.5f, 0.0f, 9.5f);

    for (int y = 0; y < 15; ++y)
    {
        sut->SetWalkable(5, y, false);
    }

    for (int y = 5; y < 20; ++y)
    {
        sut->SetWalkable(10, y, false);
    }

    for (int y = 0; y < 15; ++y)
    {
        sut->SetWalkable(15, y, false);
    }

    auto path = sut->CalculatePath(startPos, endPos);

    DumpGrid(path, startPos, endPos);

    ASSERT_FALSE(path.empty()) << "Algorytm nie znalazł wyjścia z labiryntu!";

    EXPECT_NEAR(path.back().x, endPos.x, 0.1f);
    EXPECT_NEAR(path.back().z, endPos.z, 0.1f);

    LOG_DEBUG << "Maze path length: " << path.size() << " nodes.";
}