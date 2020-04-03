#include <Camera/Frustrum.h>
#include <GameEngine/Camera/Camera.h>
#include <GameEngine/Renderers/GUI/Text/GuiTextElement.h>
#include <GameEngine/Renderers/Projection.h>
#include <Utils/GLM/GLMUtils.h>
#include <gtest/gtest.h>
#include <Utils/math.hpp>
#include "Scene/Scene.hpp"

namespace GameEngine
{
const float OFFSET = 0.001f;

struct FrustumCorners
{
    vec3 nw;
    vec3 ne;
    vec3 se;
    vec3 sw;
};

struct CameraFrustrumShould : public ::testing::Test
{
    void CalculatePlanes(const vec3& cameraPosition, const vec3& cameraLookAt)
    {
        projection_.CreateProjectionMatrix();
        camera_.SetPosition(cameraPosition);
        camera_.LookAt(cameraLookAt);
        camera_.UpdateMatrix();

        auto projectionViewMatrix = projection_.GetProjectionMatrix() * camera_.GetViewMatrix();
        sut_.CalculatePlanes(glm::transpose(projectionViewMatrix));
    }

    vec3 CalculatePointOnBorder(float z)
    {
        auto x1 = camera_.GetPosition().x;
        auto z1 = camera_.GetPosition().z;

        auto fi = (projection_.GetFoV() / 2.f);  // + camera_.GetRotation().y;
        auto x  = z * tanf(Utils::ToRadians(fi)) * projection_.GetAspectRatio();
        auto x2 = x1 + x;
        auto z2 = z1 + z;

        return vec3(x2, 0, z2);
    }

    vec3 CalculateSpherePositionInDistanceToSidePlane(float distanceSphreToPlane, float z = 10)
    {
        auto x1 = camera_.GetPosition().x;
        auto y1 = camera_.GetPosition().z;

        auto pointOnBorder = CalculatePointOnBorder(z);
        auto x2            = pointOnBorder.x;
        auto y2            = pointOnBorder.z;

        auto x3 = ((x2 * x2) - (x1 * x2) - (2 * y1 * y2) + (y1 * y1) + (y2 * y2)) / (x2 - x1);
        auto l2 = glm::length(vec2(x3 - x2, y1 - y2));

        vec3 v2(x3 - x2, 0, y1 - y2);

        vec3 p2(x2, 0, y2);
        return p2 + v2 * (distanceSphreToPlane / l2);
    }

    Frustrum sut_;
    BaseCamera camera_;
    Projection projection_;
    FrustumCorners viewSpaceCorners_;
};

// PointTest

TEST_F(CameraFrustrumShould, PointAtFront)
{
    CalculatePlanes(vec3(0, 0, -3), vec3(0, 0, 0));
    EXPECT_TRUE(sut_.PointIntersection(vec3(0)));
}

TEST_F(CameraFrustrumShould, PointAtFront2)
{
    CalculatePlanes(vec3(0, 0, 0), vec3(0, 0, -3));
    EXPECT_TRUE(sut_.PointIntersection(vec3(0, 0, -3)));
}

TEST_F(CameraFrustrumShould, PointOtherPosition)
{
    CalculatePlanes(vec3(2, -3, 1), vec3(-2, 4, 3));
    EXPECT_TRUE(sut_.PointIntersection(vec3(-2, 4, 3)));
}

TEST_F(CameraFrustrumShould, PointBehind)
{
    CalculatePlanes(vec3(0, 0, -3), vec3(0, 0, 0));
    EXPECT_FALSE(sut_.PointIntersection(vec3(0, 0, -4)));
}

TEST_F(CameraFrustrumShould, PointCloseToCamera)
{
    vec3 point(0, 0, -3 + projection_.GetNear() + OFFSET);
    CalculatePlanes(vec3(0, 0, -3), vec3(0, 0, 0));
    EXPECT_TRUE(sut_.PointIntersection(point));
}

TEST_F(CameraFrustrumShould, PointOutOfFarPlane)
{
    vec3 point(vec3(0, 0, projection_.GetFar() + 0.2f));
    CalculatePlanes(vec3(0, 0, 0), vec3(0, 0, 1));
    EXPECT_FALSE(sut_.PointIntersection(point));
}

TEST_F(CameraFrustrumShould, PointBorderOutside)
{
    for (float z = projection_.GetNear() + OFFSET; z < projection_.GetFar(); z += 0.01f)
    {
        auto point = CalculatePointOnBorder(z);
        point.x += OFFSET;
        CalculatePlanes(vec3(0, 0, 0), vec3(0, 0, 1));

        bool is = sut_.PointIntersection(point);

        EXPECT_FALSE(is);

        if (is)
        {
            DEBUG_LOG("Z: " + std::to_string(z));
            DEBUG_LOG("Point border : " + std::to_string(point - OFFSET));
            DEBUG_LOG("Point border changed : " + std::to_string(point));
            break;
        }
    }
}

TEST_F(CameraFrustrumShould, PointBorderInside)
{
    for (float z = projection_.GetNear() + OFFSET; z < projection_.GetFar(); z += 0.01f)
    {
        auto point = CalculatePointOnBorder(z);
        point.x -= OFFSET;
        CalculatePlanes(vec3(0, 0, 0), vec3(0, 0, 1));

        bool is = sut_.PointIntersection(point);

        EXPECT_TRUE(is);

        if (not is)
        {
            DEBUG_LOG("Z: " + std::to_string(z));
            DEBUG_LOG("Point border : " + std::to_string(point - OFFSET));
            DEBUG_LOG("Point border changed : " + std::to_string(point));
            break;
        }
    }
}

// Sphere Tests

// TEST_F(CameraFrustrumShould, Matrix_SphereAtFront)
//{
//    camera_.SetPosition(vec3(0, 0, -3));
//    camera_.LookAt(vec3(0, 0, 0));
//    camera_.UpdateMatrix();

//    vec3 sphereCenter(0, 0, 0);
//    float radius(1.f);

//    CalculatePlanes();
//    EXPECT_TRUE(sut_.SphereIntersection(sphereCenter, radius));
//}

// TEST_F(CameraFrustrumShould, Matrix_SphereAtFrontSwapPositions)
//{
//    camera_.SetPosition(vec3(0, 0, 0));
//    camera_.LookAt(vec3(0, 0, -3));
//    camera_.UpdateMatrix();

//    vec3 sphereCenter(0, 0, -3);
//    float radius(1.f);

//    CalculatePlanes();
//    EXPECT_TRUE(sut_.SphereIntersection(sphereCenter, radius));
//}

// TEST_F(CameraFrustrumShould, Matrix_SphereAtFrontOtherPositions)
//{
//    camera_.SetPosition(vec3(2, -3, 1));
//    camera_.LookAt(vec3(-2, 4, 3));
//    camera_.UpdateMatrix();

//    vec3 sphereCenter(-2, 4, 3);
//    float radius(1.f);

//    CalculatePlanes();
//    EXPECT_TRUE(sut_.SphereIntersection(sphereCenter, radius));
//}

// TEST_F(CameraFrustrumShould, Matrix_SphereBehind)
//{
//    camera_.SetPosition(vec3(0, 0, -3));
//    camera_.LookAt(vec3(0, 0, 0));
//    camera_.UpdateMatrix();

//    vec3 sphereCenter(0, 0, -4);
//    float radius(1.f);

//    CalculatePlanes();
//    EXPECT_FALSE(sut_.SphereIntersection(sphereCenter, radius));
//}

// TEST_F(CameraFrustrumShould, Matrix_SphereBehindCloseToCamera)
//{
//    camera_.SetPosition(vec3(0, 0, -3));
//    camera_.LookAt(vec3(0, 0, 0));
//    camera_.UpdateMatrix();

//    vec3 sphereCenter(0, 0, -4 + projection_.GetNear() + OFFSET);
//    float radius(1.f);

//    CalculatePlanes();
//    EXPECT_TRUE(sut_.SphereIntersection(sphereCenter, radius));
//}

// TEST_F(CameraFrustrumShould, Matrix_SphereAtFrontBehindFarPlane)
//{
//    float radius(1.f);
//    vec3 sphereCenter(vec3(0, 0, projection_.GetFar() + 1 + OFFSET));

//    camera_.SetPosition(vec3(0, 0, -3));
//    camera_.LookAt(sphereCenter);
//    camera_.UpdateMatrix();

//    CalculatePlanes();
//    EXPECT_FALSE(sut_.SphereIntersection(sphereCenter, radius));
//}

// TEST_F(CameraFrustrumShould, Matrix_SphereRightBorderOutside)
//{
//    camera_.SetPosition(vec3(0, 0, 0));
//    camera_.LookAt(vec3(0, 0, 10));
//    camera_.UpdateMatrix();

//    auto distanceSphreToPlane = 1.0f + OFFSET;
//    auto v3                   = CalculateSpherePositionInDistanceToSidePlane(distanceSphreToPlane);

//    float radius(1.f);
//    vec3 sphereCenter(v3);

//    CalculatePlanes();
//    EXPECT_FALSE(sut_.SphereIntersection(sphereCenter, radius));
//}

// TEST_F(CameraFrustrumShould, Matrix_SphereRightBorderInside)
//{
//    camera_.SetPosition(vec3(0, 0, 0));
//    camera_.LookAt(vec3(0, 0, 10));
//    camera_.UpdateMatrix();

//    auto distanceSphreToPlane = 1.0f - OFFSET;
//    auto v3                   = CalculateSpherePositionInDistanceToSidePlane(distanceSphreToPlane);

//    float radius(1.f);
//    vec3 sphereCenter(v3);

//    CalculatePlanes();
//    EXPECT_TRUE(sut_.SphereIntersection(sphereCenter, radius));
//}
}  // namespace GameEngine
