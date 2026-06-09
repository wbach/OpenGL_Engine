#pragma once
#include <d3d11.h>

#include <vector>

#include "Buffer.h"
#include "DirectXContext.h"
#include "Types.h"
#include "Vao.h"
#include "Vertex.h"

namespace GraphicsApi::Dx11
{
class Object
{
public:
    Object(const DirectXContext& dxCondext, const Vao& vao);
    void Draw() const;
    void DrawInstanced(uint32) const;
    void Release();

public:
    Buffer indiciesBuffer_;
    Buffer vertexBuffer_;

private:
    void Bind() const;
    void BindVertexBuffer() const;
    void CreateVertexBuffer(const std::vector<Vertex>& vertexes);
    void CreateIndiciesBuffer(const std::vector<Index>& v);

private:
    UINT stride_;
    UINT offset_;
    uint32 indieciesSize_;
    const DirectXContext& dxCondext_;
    Vao vao_;
};
}  // namespace GraphicsApi::Dx11
