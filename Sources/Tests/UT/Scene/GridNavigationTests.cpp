#include <gtest/gtest.h>

#include "GameEngine/Scene/Navigation/GridNavigation.h"

using namespace GameEngine;

class GridNavigationTest : public ::testing::Test
{
protected:
    const int width      = 10;
    const int height     = 10;
    const float cellSize = 1.0f;
    const vec3 orgin{vec3(0)};
    GridNavigation* nav;

    void SetUp() override
    {
        nav = new GridNavigation(orgin, width, height, cellSize);
    }

    void TearDown() override
    {
        delete nav;
    }

    void DumpGrid(const std::vector<vec3>& path = {}, vec3 start = {0, 0, 0}, vec3 end = {0, 0, 0})
    {
        const auto& nodes = nav->GetNodes();

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
            int x = static_cast<int>(std::floor((p.x - orgin.x) / cellSize));
            int z = static_cast<int>(std::floor((p.z - orgin.z) / cellSize));
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
            "\n--- GRID VISUALIZATION (Origin: " + std::to_string(orgin.x) + "," + std::to_string(orgin.z) + ") ---\n";

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
    vec3 pos(5.5f, 0.0f, 5.5f);
    auto path = nav->CalculatePath(pos, pos);
    EXPECT_TRUE(path.empty() || path.size() == 1);
}

TEST_F(GridNavigationTest, OutOfBoundsReturnsEmpty)
{
    auto path = nav->CalculatePath({1.0f, 0.0f, 1.0f}, {50.0f, 0.0f, 50.0f});
    EXPECT_TRUE(path.empty());
}

TEST_F(GridNavigationTest, SimpleStraightPath)
{
    vec3 start(0.5f, 0.0f, 0.5f);
    vec3 end(0.5f, 0.0f, 3.5f);

    auto path = nav->CalculatePath(start, end);

    DumpGrid(path, start, end);

    ASSERT_FALSE(path.empty());
    EXPECT_NEAR(path.back().z, 3.5f, 0.1f);
    EXPECT_EQ(path.back().x, 0.5f);
}

TEST_F(GridNavigationTest, ObstacleAvoidance)
{
    nav->SetWalkable(1, 0, false);
    nav->SetWalkable(1, 1, false);
    nav->SetWalkable(1, 2, false);

    glm::vec3 start(0.5f, 0.0f, 1.5f);
    glm::vec3 end(2.5f, 0.0f, 1.5f);

    auto path = nav->CalculatePath(start, end);

    DumpGrid(path, start, end);
    ASSERT_FALSE(path.empty());

    for (const auto& point : path)
    {
        EXPECT_FALSE(point.x == 1.5f && point.z >= 0.0f && point.z <= 2.0f);
    }
}

TEST_F(GridNavigationTest, NoPathFound)
{
    nav->SetWalkable(0, 1, false);
    nav->SetWalkable(1, 1, false);
    nav->SetWalkable(1, 0, false);

    glm::vec3 start(0.5f, 0.0f, 0.5f);
    glm::vec3 end(5.5f, 0.0f, 5.5f);

    auto path = nav->CalculatePath(start, end);

    DumpGrid(path, start, end);

    EXPECT_TRUE(path.empty());
}

TEST_F(GridNavigationTest, CustomPath)
{
    vec3 start(8.5f, 0.0f, 8.5f);
    vec3 end(0.5f, 0.0f, 3.5f);

    for (int x = 2; x < 8; ++x)
    {
        nav->SetWalkable(7, x, false);
    }

    for (int x = 0; x < 8; ++x)
    {
        nav->SetWalkable(x, 7, false);
    }

    auto path = nav->CalculatePath(start, end);

    DumpGrid(path, start, end);

    ASSERT_FALSE(path.empty());
    EXPECT_NEAR(path.back().z, 3.5f, 0.1f);
    EXPECT_EQ(path.back().x, 0.5f);
}

TEST_F(GridNavigationTest, OffsetOriginMapping)
{
    vec3 origin(100.0f, 0.0f, 100.0f);
    GridNavigation navOffset(origin, 10, 10, 1.0f);

    const vec3 start(100.5f, 0.0f, 100.5f);  // Indeks [0,0]
    const vec3 end(103.5f, 0.0f, 103.5f);    // Indeks [3,3]

    auto path = navOffset.CalculatePath(start, end);

    ASSERT_FALSE(path.empty());

    EXPECT_NEAR(path.front().x, 101.5f, 0.1f);
    EXPECT_NEAR(path.front().z, 101.5f, 0.1f);

    EXPECT_NEAR(path.back().x, 103.5f, 0.1f);
    EXPECT_NEAR(path.back().z, 103.5f, 0.1f);
}

TEST_F(GridNavigationTest, OutOfBoundsWithOrigin)
{
    vec3 origin(-50.0f, 0.0f, -50.0f);
    GridNavigation navOffset(origin, 10, 10, 1.0f);
    const vec3 start{-60.0f, 0, -60.0f};
    const vec3 end{-45.0f, 0, -45.0f};
    auto path = navOffset.CalculatePath(start, end);

    DumpGrid(path, start, end);

    EXPECT_TRUE(path.empty());
}