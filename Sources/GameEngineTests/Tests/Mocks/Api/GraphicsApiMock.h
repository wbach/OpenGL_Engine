#pragma once
#include <gmock/gmock.h>
#include "GraphicsApi/IGraphicsApi.h"
#include "GraphicsApi/MeshRawData.h"

namespace GraphicsApi
{
class GraphicsApiMock : public IGraphicsApi
{
public:
    MOCK_METHOD0(GetWindowApi, IWindowApi&());
    MOCK_METHOD0(Init, void());
    MOCK_METHOD1(SetShadersFilesLocations, void(const std::string&));
    MOCK_METHOD1(SetShaderQuaility, void(ShaderQuaility));
    MOCK_METHOD0(CreateContext, void());
    MOCK_METHOD0(DeleteContext, void());
    MOCK_METHOD0(PrintVersion, void());
    MOCK_CONST_METHOD0(IsTesselationSupported, bool());
    MOCK_CONST_METHOD0(GetSupportedRenderers, std::vector<RendererType>());
    MOCK_METHOD0(SetDefaultTarget, void());
    MOCK_METHOD1(SetBackgroundColor, void(const Color&));
    MOCK_CONST_METHOD0(GetBackgroundColor, const Color&());
    MOCK_METHOD0(EnableDepthTest, void());
    MOCK_METHOD0(DisableDepthTest, void());
    MOCK_METHOD0(PrepareFrame, void());
    MOCK_METHOD1(CreateShader, ID(ShaderProgramType));
    MOCK_METHOD2(CreateShaderBuffer, ID(uint32 bindLocation, uint32 size));
    MOCK_METHOD2(UpdateShaderBuffer, void(uint32 id, void const* buffer));
    MOCK_METHOD1(BindShaderBuffer, uint32(uint32));
    MOCK_METHOD1(UseShader, void(uint32));
    MOCK_METHOD5(CreateTexture, ID(TextureType, TextureFilter, TextureMipmap, const vec2ui&, const void* data));
    MOCK_METHOD3(CreateTextureStorage, ID(TextureType, TextureFilter, int32 N));
    MOCK_METHOD2(CreateCubMapTexture, ID(vec2ui, std::vector<void*>));
    MOCK_METHOD4(UpdateTexture, void(uint32, const vec2ui&, const vec2ui&, const void* data));
    MOCK_METHOD3(UpdateTexture, void(uint32, const vec2ui&, const void* data));
    MOCK_METHOD2(ClearTexture, void(uint32, const Color&));
    MOCK_METHOD0(EnableBlend, void());
    MOCK_METHOD0(DisableBlend, void());
    MOCK_METHOD0(EnableDepthMask, void());
    MOCK_METHOD0(DisableDepthMask, void());
    MOCK_METHOD2(ActiveTexture, void(uint32, uint32));
    MOCK_METHOD1(ActiveTexture, void(uint32));
    MOCK_METHOD1(DeleteObject, void(uint32));
    MOCK_METHOD1(DeleteShaderBuffer, void(uint32));
    MOCK_METHOD0(GetBufferStatus, std::string());
    MOCK_METHOD1(CreatePatchMesh, ID(const std::vector<float>&));
    MOCK_METHOD2(CreatePurePatchMeshInstanced, ID(uint32, uint32));
    MOCK_METHOD2(CreateMesh, ID(const MeshRawData&, RenderType));
    MOCK_METHOD0(CreateDynamicLineMesh, ID());
    MOCK_METHOD0(CreateParticle, ID());
    MOCK_METHOD0(CreateAnimatedParticle, ID());
    MOCK_METHOD3(Compute, void(uint32, uint32, uint32));
    MOCK_METHOD1(RenderPurePatchedMeshInstances, void(uint32));
    MOCK_METHOD1(RenderMesh, void(uint32));
    MOCK_METHOD1(RenderTriangleStripMesh, void(uint32));
    MOCK_METHOD2(RenderMeshInstanced, void(uint32, uint32));
    MOCK_METHOD1(RenderPoints, void(uint32));
    MOCK_METHOD0(RenderQuad, void());
    MOCK_METHOD0(RenderQuadTs, void());
    MOCK_METHOD0(EnableCulling, void());
    MOCK_METHOD0(DisableCulling, void());
    MOCK_METHOD4(SetViewPort, void(uint32, uint32, uint32, uint32));
    MOCK_CONST_METHOD1(BindTexture, void(uint32));
    MOCK_METHOD2(BindImageTexture, void(uint32, TextureAccess));
    MOCK_METHOD2(CreateShadowMap, ID(uint32, uint32));
    MOCK_METHOD0(PolygonModeRender, void());
    MOCK_METHOD0(LineModeRender, void());
    MOCK_METHOD1(SetBlendFunction, void(BlendFunctionType));
    MOCK_METHOD2(UpdateMatrixes, void(uint32, const std::vector<mat4>&));
    MOCK_METHOD3(UpdateMesh, void(uint32, const MeshRawData&, const std::set<VertexBufferObjects>&));
    MOCK_METHOD2(UpdateLineMesh, void(uint32, const LineMesh&));
    MOCK_METHOD2(UpdateOffset, void(uint32, const std::vector<vec4>&));
    MOCK_METHOD2(UpdateBlend, void(uint32, const std::vector<float>&));
    MOCK_METHOD1(LoadViewMatrix, void(const mat4&));
    MOCK_METHOD1(LoadProjectionMatrix, void(const mat4&));
    MOCK_METHOD3(DrawLine, void(const vec3& color, const vec3& from, const vec3& to));
    MOCK_METHOD1(PrepareMatrixToLoad, mat4(const mat4&));
    MOCK_CONST_METHOD1(GetTextureData, std::vector<uint8>(uint32));
    MOCK_CONST_METHOD1(GetTextureInfo, const TextureInfo&(uint32));
    MOCK_CONST_METHOD1(TakeSnapshoot, void(const std::string& path));
    MOCK_METHOD0(BindDefaultFrameBuffer, void());
    MOCK_METHOD1(CreateFrameBuffer, IFrameBuffer&(const std::vector<FrameBuffer::Attachment>&));
    MOCK_METHOD1(DeleteFrameBuffer, void(IFrameBuffer&));
};

}  // namespace GraphicsApi
