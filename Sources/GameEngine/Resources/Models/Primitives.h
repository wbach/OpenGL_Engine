#pragma once
#include <GraphicsApi/MeshRawData.h>

namespace GameEngine
{
enum class PrimitiveType
{
    Cube,
    Sphere,
    Cylinder,
    Cone,
    Plane,
    Torus,
    Pyramid,
    IcoSphere,
    Triangle
};

GraphicsApi::MeshRawData GenerateCube(float size = 1.0f);
GraphicsApi::MeshRawData GenerateSphere(float radius = 1.0f, unsigned int latSegments = 16, unsigned int lonSegments = 16);
GraphicsApi::MeshRawData GenerateCylinder(float radius = 0.5f, float height = 1.0f, unsigned int segments = 16);
GraphicsApi::MeshRawData GenerateCone(float radius = 0.5f, float height = 1.0f, unsigned int segments = 16);
GraphicsApi::MeshRawData GeneratePlane(float size = 1.0f);
GraphicsApi::MeshRawData GenerateTorus(float radius = 0.5f, float tubeRadius = 0.2f, unsigned int radialSegs = 16,
                                       unsigned int tubularSegs = 12);
GraphicsApi::MeshRawData GeneratePyramid(float size = 1.0f, float height = 1.0f);
GraphicsApi::MeshRawData GenerateIcoSphere(float radius = 1.0f);
GraphicsApi::MeshRawData GenerateTriangle(float size = 1.0f);
}  // namespace GameEngine
