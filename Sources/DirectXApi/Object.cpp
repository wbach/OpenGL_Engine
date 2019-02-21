#include "Object.h"
#include <xnamath.h>
namespace DirectX
{
Object::Object(const DirectXContext& dxCondext, const Vao& vao)
    : stride_(sizeof(Vertex))
    , offset_(0)
    , dxCondext_(dxCondext)
    , vao_(vao)
{
    CreateVertexBuffer(vao.vertexes_);
    CreateIndiciesBuffer(vao.indices_);
}
void Object::Bind() const
{
    dxCondext_.devcon->IASetIndexBuffer(indiciesBuffer_.ptr, DXGI_FORMAT_R32_UINT, 0);
}
void Object::BindVertexBuffer() const
{
      dxCondext_.devcon->IASetVertexBuffers(0, 1, &vertexBuffer_.ptr, &stride_, &offset_);
}
void Object::Draw() const
{
    dxCondext_.devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    if (indieciesSize_ > 0)
    {
        BindVertexBuffer();
        Bind();
        dxCondext_.devcon->DrawIndexed(indieciesSize_, 0, 0);
    }
    else
    {
        BindVertexBuffer();
        dxCondext_.devcon->Draw(vao_.vertexes_.size(), 0);
    }
}
void Object::CreateVertexBuffer(const std::vector<Vertex>& vertexes)
{
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage          = D3D11_USAGE_DEFAULT;
    bd.ByteWidth      = sizeof(Vertex) * vertexes.size();
    bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = &vertexes[0];

    vertexBuffer_.bindLocation = 0;

    auto hr = dxCondext_.dev->CreateBuffer(&bd, &InitData, &vertexBuffer_.ptr);

    if (FAILED(hr))
    {
        MessageBox(NULL, "Vertex buffer creation error.", "Error", MB_OK);
    }
    BindVertexBuffer();
}
void Object::CreateIndiciesBuffer(const std::vector<Index>& indicies)
{
    indieciesSize_ = indicies.size();

    if (indicies.empty())
    {
        return;
    }
 
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));

    bd.Usage          = D3D11_USAGE_DEFAULT;
    bd.ByteWidth      = sizeof(Index) * indieciesSize_;
    bd.BindFlags      = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem  = &indicies[0];

    auto hr = dxCondext_.dev->CreateBuffer(&bd, &InitData, &indiciesBuffer_.ptr);
    if (FAILED(hr))
    {
        MessageBox(NULL, "Indicies buffer creation error.", "Error", MB_OK);
    }
    Bind();
}
}  // namespace DirectX
