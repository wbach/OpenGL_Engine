#include <gtest/gtest.h>

#include "GameEngine/Scene/Navigation/GridNavigation.h"

using namespace GameEngine;

class GridNavigationTest : public ::testing::Test
{
protected:
    const int width      = 10;
    const int height     = 10;
    const float cellSize = 1.0f;
    GridNavigation* nav;

    void SetUp() override
    {
        nav = new GridNavigation(width, height, cellSize);
    }

    void TearDown() override
    {
        delete nav;
    }

    void DumpGrid(const std::vector<vec3>& path = {}, vec3 start = {0, 0, 0}, vec3 end = {0, 0, 0})
    {
        const auto& nodes = nav->GetNodes();
        // Kopia siatki do wizualizacji
        std::vector<std::string> display(height, std::string(width, '.'));

        // 1. Naniesienie przeszkód
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

        // 2. Naniesienie ścieżki
        for (const auto& p : path)
        {
            int x = static_cast<int>(p.x / cellSize);
            int z = static_cast<int>(p.z / cellSize);
            if (x >= 0 && x < width && z >= 0 && z < height)
            {
                display[z][x] = '*';
            }
        }

        // 3. Naniesienie Startu i Końca
        int sx = static_cast<int>(start.x / cellSize);
        int sz = static_cast<int>(start.z / cellSize);
        if (sx >= 0 && sx < width && sz >= 0 && sz < height)
            display[sz][sx] = 'S';

        int ex = static_cast<int>(end.x / cellSize);
        int ez = static_cast<int>(end.z / cellSize);
        if (ex >= 0 && ex < width && ez >= 0 && ez < height)
            display[ez][ex] = 'E';

        // 4. Wypisanie do loga
        std::string output = "\n--- GRID VISUALIZATION ---\n";
        for (int y = 0; y < height; ++y)
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
