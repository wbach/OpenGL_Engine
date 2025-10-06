#include <GameEngine/Camera/Camera.h>
#include <GameEngine/Camera/Frustrum.h>
#include <GameEngine/Renderers/GUI/Text/GuiTextElement.h>
#include <GameEngine/Renderers/Projection.h>
#include <Types.h>
#include <Utils/GLM/GLMUtils.h>
#include <gtest/gtest.h>

#include <GameEngine/Scene/Scene.hpp>
#include <Utils/math.hpp>
#include <iomanip>  // dla std::setw i std::setprecision

#include "Logger/Log.h"

namespace GameEngine
{
const float OFFSET = 0.001f;

struct CameraFrustrumShould : public ::testing::Test
{
    void CalculatePlanes(const vec3& cameraPosition, const vec3& cameraLookAt)
    {
        projection_.CreateProjectionMatrix();
        camera_.SetPosition(cameraPosition);
        camera_.LookAt(cameraLookAt);
        camera_.UpdateMatrix();

        auto projectionViewMatrix = projection_.GetProjectionMatrix() * camera_.GetViewMatrix();
        sut_.prepareFrame(projectionViewMatrix);
        //DebugPrintPlanes("");
    }

    void DebugPrintPlanes(const std::string& tag) const
    {
        const auto& planes = sut_.getPlanes().back();

        LOG_DEBUG << "==== Frustum Planes Debug [" << tag << "] ====";
        for (size_t i = 0; i < planes.size(); ++i)
        {
            const auto& p = planes[i];
            LOG_DEBUG << "Plane[" << i << "]"
                      << " normal: (" << std::setw(9) << std::setprecision(5) << p.normal_.x << ", " << std::setw(9)
                      << std::setprecision(5) << p.normal_.y << ", " << std::setw(9) << std::setprecision(5) << p.normal_.z << ")"
                      << " d: " << std::setw(9) << std::setprecision(5) << p.d_;

            // obliczanie punktu na płaszczyźnie najbliższego origo
            vec3 pointOnPlane = -p.normal_ * p.d_;
            LOG_DEBUG << "  → passes near point: "
                      << "(" << pointOnPlane.x << ", " << pointOnPlane.y << ", " << pointOnPlane.z << ")";
        }
        LOG_DEBUG << "=========================================";
    }

    vec3 CalculatePointOnBorder(float z)
    {
        auto x1 = camera_.GetPosition().x;
        auto z1 = camera_.GetPosition().z;

        auto fi = (projection_.GetFoV() / 2.f);  // + camera_.GetRotation().y;
        auto x  = z * tanf(ToRadians(fi)) * projection_.GetAspectRatio();
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
    Camera camera_;
    Projection projection_;
};
// PointTest

TEST_F(CameraFrustrumShould, PointAtFront)
{
    CalculatePlanes(vec3(0, 0, -3), vec3(0, 0, 0));
    EXPECT_TRUE(sut_.intersection(vec3(0)));
}

TEST_F(CameraFrustrumShould, PointAtFront2)
{
    CalculatePlanes(vec3(0, 0, 0), vec3(0, 0, -3));
    EXPECT_TRUE(sut_.intersection(vec3(0, 0, -3)));
}

TEST_F(CameraFrustrumShould, PointOtherPosition)
{
    CalculatePlanes(vec3(2, -3, 1), vec3(-2, 4, 3));
    EXPECT_TRUE(sut_.intersection(vec3(-2, 4, 3)));
}

TEST_F(CameraFrustrumShould, PointBehind)
{
    CalculatePlanes(vec3(0, 0, -3), vec3(0, 0, 0));
    EXPECT_FALSE(sut_.intersection(vec3(0, 0, -4)));
}

TEST_F(CameraFrustrumShould, PointCloseToCamera)
{
    vec3 point(0, 0, -3.f + projection_.GetNear() + OFFSET);
    CalculatePlanes(vec3(0, 0, -3.f), vec3(0, 0, 0));
    EXPECT_TRUE(sut_.intersection(point));
}

TEST_F(CameraFrustrumShould, PointOutOfFarPlane)
{
    vec3 point(vec3(0, 0, projection_.GetFar() + 0.2f));
    CalculatePlanes(vec3(0, 0, 0), vec3(0, 0, 1));
    EXPECT_FALSE(sut_.intersection(point));
}

TEST_F(CameraFrustrumShould, PointBorderOutside)
{
    for (float z = projection_.GetNear() + OFFSET; z < projection_.GetFar(); z += 0.01f)
    {
        auto point           = CalculatePointOnBorder(z);
        float adaptiveOffset = glm::max(OFFSET, z * 0.001f);
        point.x += adaptiveOffset;
        CalculatePlanes(vec3(0, 0, 0), vec3(0, 0, 1));

        bool is = sut_.intersection(point);

        EXPECT_FALSE(is);

        if (is)
        {
            LOG_DEBUG << "Z: " << z;
            LOG_DEBUG << "Point border : " << (point - OFFSET);
            LOG_DEBUG << "Point border changed : " << point;
            break;
        }
    }
}

TEST_F(CameraFrustrumShould, PointBorderInside)
{
    for (float z = projection_.GetNear() + OFFSET; z < projection_.GetFar(); z += 0.01f)
    {
        vec3 forward = glm::normalize(camera_.GetDirection());
        vec3 right   = glm::normalize(glm::cross(forward, vec3(0, 1, 0)));

        float halfFovY  = glm::radians(projection_.GetFoV() / 2.f);
        float halfWidth = z * tanf(halfFovY) * projection_.GetAspectRatio();

        vec3 pointOnBorder = camera_.GetPosition() + forward * z + right * halfWidth;
        vec3 pointInside   = pointOnBorder - right * OFFSET;
        CalculatePlanes(camera_.GetPosition(), camera_.GetPosition() + forward);

        bool isInside = sut_.intersection(pointInside);

        EXPECT_TRUE(isInside);

        if (!isInside)
        {
            LOG_DEBUG << "Z: " << z;
            LOG_DEBUG << "Point border: " << pointOnBorder;
            LOG_DEBUG << "Point inside: " << pointInside;
            break;
        }
    }
}

// Sphere Tests
TEST_F(CameraFrustrumShould, SphereAtFront)
{
    vec3 sphereCenter(0, 0, 0);
    float radius(1.f);

    CalculatePlanes(vec3(0, 0, -3), vec3(0, 0, 0));
    EXPECT_TRUE(sut_.intersection(sphereCenter, radius));
}

TEST_F(CameraFrustrumShould, SphereAtFrontSwapPositions)
{
    vec3 sphereCenter(0, 0, -3);
    float radius(1.f);

    CalculatePlanes(vec3(0, 0, 0), vec3(0, 0, -3));
    EXPECT_TRUE(sut_.intersection(sphereCenter, radius));
}

TEST_F(CameraFrustrumShould, SphereAtFrontOtherPositions)
{
    vec3 sphereCenter(-2, 4, 3);
    float radius(1.f);

    CalculatePlanes(vec3(2, -3, 1), vec3(-2, 4, 3));
    EXPECT_TRUE(sut_.intersection(sphereCenter, radius));
}

TEST_F(CameraFrustrumShould, SphereBehind)
{
    vec3 sphereCenter(0, 0, -4);
    float radius(1.f);

    CalculatePlanes(vec3(0, 0, -3), vec3(0, 0, 0));
    EXPECT_FALSE(sut_.intersection(sphereCenter, radius));
}

TEST_F(CameraFrustrumShould, SphereBehindCloseToCamera)
{
    vec3 sphereCenter(0, 0, -4 + projection_.GetNear() + OFFSET);
    float radius(1.f);

    CalculatePlanes(vec3(0, 0, -3), vec3(0, 0, 0));
    EXPECT_TRUE(sut_.intersection(sphereCenter, radius));
}

TEST_F(CameraFrustrumShould, SphereAtFrontBehindFarPlane)
{
    float radius(1.f);
    vec3 sphereCenter(vec3(0, 0, projection_.GetFar() + radius + OFFSET));

    CalculatePlanes(vec3(0, 0, -3), sphereCenter);
    EXPECT_FALSE(sut_.intersection(sphereCenter, radius));
}

TEST_F(CameraFrustrumShould, SphereRightBorderOutside)
{
    auto distanceSphreToPlane = 1.0f + OFFSET;
    auto v3                   = CalculateSpherePositionInDistanceToSidePlane(distanceSphreToPlane);

    float radius(1.f);
    vec3 sphereCenter(v3);

    CalculatePlanes(vec3(0, 0, 0), vec3(0, 0, 10));
    EXPECT_FALSE(sut_.intersection(sphereCenter, radius));
}

TEST_F(CameraFrustrumShould, SphereRightBorderInside)
{
    auto distanceSphreToPlane = 1.0f - OFFSET;
    auto v3                   = CalculateSpherePositionInDistanceToSidePlane(distanceSphreToPlane);

    float radius(1.f);
    vec3 sphereCenter(v3);

    CalculatePlanes(vec3(0, 0, 0), vec3(0, 0, 10));
    EXPECT_TRUE(sut_.intersection(sphereCenter, radius));
}
// BoundingBox Tests
TEST_F(CameraFrustrumShould, BoundingBoxCompletelyInside)
{
    BoundingBox box(vec3(-1, -1, -1), vec3(1, 1, 1));
    CalculatePlanes(vec3(0, 0, -5), vec3(0, 0, 0));
    EXPECT_TRUE(sut_.intersection(box));
}

TEST_F(CameraFrustrumShould, BoundingBoxCompletelyOutside)
{
    BoundingBox box(vec3(100, 100, 100), vec3(102, 102, 102));
    CalculatePlanes(vec3(0, 0, -5), vec3(0, 0, 0));
    EXPECT_FALSE(sut_.intersection(box));
}

TEST_F(CameraFrustrumShould, BoundingBoxPartiallyLeft)
{
    BoundingBox box(vec3(-10, -1, -1), vec3(0.5f, 1, 1));
    CalculatePlanes(vec3(0, 0, -5), vec3(0, 0, 0));
    EXPECT_TRUE(sut_.intersection(box));
}

TEST_F(CameraFrustrumShould, BoundingBoxPartiallyRight)
{
    BoundingBox box(vec3(-0.5f, -1, -1), vec3(10, 1, 1));
    CalculatePlanes(vec3(0, 0, -5), vec3(0, 0, 0));
    EXPECT_TRUE(sut_.intersection(box));
}

TEST_F(CameraFrustrumShould, BoundingBoxTouchNearPlane)
{
    BoundingBox box(vec3(-1, -1, -5), vec3(1, 1, -5 + projection_.GetNear() * 1.1f));
    CalculatePlanes(vec3(0, 0, -5), vec3(0, 0, 0));
    EXPECT_TRUE(sut_.intersection(box));
}

TEST_F(CameraFrustrumShould, BoundingBoxBeforeNearPlane)
{
    BoundingBox box(vec3(-1, -1, -5.1f), vec3(1, 1, -5 + projection_.GetNear() * 0.5f));
    CalculatePlanes(vec3(0, 0, -5), vec3(0, 0, 0));
    EXPECT_FALSE(sut_.intersection(box));
}

TEST_F(CameraFrustrumShould, BoundingBoxBeyondFarPlane)
{
    BoundingBox box(vec3(-1, -1, projection_.GetFar() + 0.1f), vec3(1, 1, projection_.GetFar() + 1.0f));
    CalculatePlanes(vec3(0, 0, -5), vec3(0, 0, 0));
    EXPECT_FALSE(sut_.intersection(box));
}

TEST_F(CameraFrustrumShould, BoundingBoxVeryLargeCoordinates)
{
    BoundingBox box(vec3(-2000, -10, -3000), vec3(2300, 10, 2700));
    CalculatePlanes(vec3(0, 0, -5), vec3(0, 0, 0));
    EXPECT_TRUE(sut_.intersection(box));
}

TEST_F(CameraFrustrumShould, BoundingBoxTouchSidePlane)
{
    // Odległość BB od kamery wzdłuż osi Z
    float zDistance = -(-10.f - (-20.f)); // BB z = -10, kamera z = -20 → odległość = 10
    // Półszerokość frustum w tej odległości
    float halfWidthAtZ = zDistance * tanf(glm::radians(projection_.GetFoV() / 2.f)) * projection_.GetAspectRatio();

    // BoundingBox tuż przy prawej krawędzi
    BoundingBox box(
        vec3(halfWidthAtZ - 1.f, -1, -10), // min
        vec3(halfWidthAtZ + 1.f, 1, -8)    // max
    );

    LOG_DEBUG << box;

    CalculatePlanes(vec3(0, 0, -20), vec3(0, 0, 0));
    EXPECT_TRUE(sut_.intersection(box));
}

TEST_F(CameraFrustrumShould, BoundingBoxOutsideSidePlane)
{
    // Trochę poza prawą krawędzią frustum
    float halfWidth = projection_.GetFar() * tanf(glm::radians(projection_.GetFoV() / 2.f)) * projection_.GetAspectRatio();
    BoundingBox box(vec3(halfWidth + 0.5f, -1, -10), vec3(halfWidth + 2.f, 1, -8));
    CalculatePlanes(vec3(0, 0, -20), vec3(0, 0, 0));
    EXPECT_FALSE(sut_.intersection(box));
}

}  // namespace GameEngine
