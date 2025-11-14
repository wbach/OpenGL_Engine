#pragma once
#include <memory>

#include "Types.h"

namespace GraphicsApi
{
class IGraphicsApi;
}

namespace GameEngine
{
class Model;
class Mesh;
class MeshWaterFactory
{
public:
    MeshWaterFactory()  = default;
    ~MeshWaterFactory() = default;

    std::unique_ptr<Model> CreateAsSingleTile(GraphicsApi::IGraphicsApi&, uint32 resolution);

private:
    void ReserveMeshData(Mesh&, uint32 size);
    void CreateVertexes(Mesh&, uint32 x_start, uint32 y_start, uint32 width, uint32 height);
    void CreateIndicies(Mesh&, IndicesDataType size);
};
}  // namespace GameEngine
