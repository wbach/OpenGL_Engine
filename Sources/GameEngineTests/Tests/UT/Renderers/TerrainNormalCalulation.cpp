#include <Types.h>
#include <gtest/gtest.h>
#include <vector>

namespace
{
struct HeightMapCalculatorShould : public ::testing::Test
{
    HeightMapCalculatorShould()
    {
        heights_.resize(heightMapResolution_);
    }

    void SetUp() override
    {

    }

    vec3 CalculateNormalMap(uint32 x, uint32 z)
    {
        auto lx = x - 1;
        if (x == 0)
            lx = 0;
        auto rx = x + 1;
        if (rx > heightMapResolution_ - 1)
            rx = heightMapResolution_ - 1;
        auto dz = z - 1;
        if (z == 0)
            dz = 0;
        auto uz = z + 1;
        if (uz > heightMapResolution_ - 1)
            uz = heightMapResolution_ - 1;
        float heightL = GetHeight(lx, z);
        float heightR = GetHeight(rx, z);
        float heightD = GetHeight(x, dz);
        float heightU = GetHeight(x, uz);
        glm::vec3 normal(heightL - heightR, 2.0f, heightD - heightU);
        return glm::normalize(normal);
    }

    float& GetHeight(uint32 x, uint32 y)
    {
        return heights_[x + y * heightMapResolution_];
    }

    uint32 heightMapResolution_ = 256;
    std::vector<float> heights_;

};

TEST_F(HeightMapCalculatorShould, a)
{
    for (uint32 y = 0; y < heightMapResolution_; ++y)
    {
        for (uint32 x = 0; x < heightMapResolution_; ++x)
        {
            auto& height = GetHeight(x, y);
            height = static_cast<float>(heightMapResolution_) - (static_cast<float>(x) * 0.25f);
        }
    }

    auto normal = CalculateNormalMap(128, 128);
    std::cout << std::to_string(normal) << std::endl;


    auto dotProduct = glm::dot(normal, vec3(1, 0, 0));
    std::cout << "Dot : " << dotProduct  << std::endl;

}
}  // namespace
