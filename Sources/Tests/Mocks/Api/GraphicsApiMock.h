#pragma once
#include <gmock/gmock.h>

#include "GraphicsApi/IGraphicsApi.h"
#include "GraphicsApi/MeshRawData.h"
#include "Logger/Log.h"

namespace GraphicsApi
{
class GraphicsApiMock : public IGraphicsApi
{
public:
    ~GraphicsApiMock() override
    {
        LOG_DEBUG << "~GraphicsApiMock";
    }

    MOCK_METHOD(IWindowApi&, GetWindowApi, (), (override));
    MOCK_METHOD(void, Init, (), (override));
    MOCK_METHOD(void, SetShadersFilesLocations, (const std::string&), (override));
    MOCK_METHOD(void, SetShaderQuaility, (ShaderQuaility), (override));
    MOCK_METHOD(void, CreateContext, (), (override));
    MOCK_METHOD(void, DeleteContext, (), (override));
    MOCK_METHOD(void, PrintVersion, (), (override));
    MOCK_METHOD(bool, IsTesselationSupported, (), (const, override));
    MOCK_METHOD(std::vector<RendererType>, GetSupportedRenderers, (), (const, override));
    MOCK_METHOD(void, SetDefaultTarget, (), (override));
    MOCK_METHOD(void, SetBackgroundColor, (const Color&), (override));
    MOCK_METHOD(const Color&, GetBackgroundColor, (), (const, override));
    MOCK_METHOD(void, EnableDepthTest, (), (override));
    MOCK_METHOD(void, DisableDepthTest, (), (override));
    MOCK_METHOD(void, PrepareFrame, (), (override));
    MOCK_METHOD(ID, CreateShader, (ShaderProgramType), (override));
    MOCK_METHOD(ID, CreateShaderBuffer, (uint32 bindLocation, uint32 size), (override));
    MOCK_METHOD(void, UpdateShaderBuffer, (uint32 id, const void* buffer), (override));
    MOCK_METHOD(uint32, BindShaderBuffer, (uint32), (override));
    MOCK_METHOD(void, UseShader, (uint32), (override));
    MOCK_METHOD(ID, CreateTexture, (const Utils::Image&, TextureFilter, TextureMipmap), (override));
    MOCK_METHOD(ID, CreateTextureStorage, (TextureType, TextureFilter, int32), (override));
    MOCK_METHOD(ID, CreateCubMapTexture, (const CubemapImages&), (override));
    MOCK_METHOD(void, UpdateTexture, (uint32, const vec2ui&, const Utils::Image&), (override));
    MOCK_METHOD(void, UpdateTexture, (uint32, const Utils::Image&), (override));
    MOCK_METHOD(void, ClearTexture, (uint32, const Color&), (override));
    MOCK_METHOD(void, EnableBlend, (), (override));
    MOCK_METHOD(void, DisableBlend, (), (override));
    MOCK_METHOD(void, EnableDepthMask, (), (override));
    MOCK_METHOD(void, DisableDepthMask, (), (override));
    MOCK_METHOD(void, ActiveTexture, (uint32, uint32), (override));
    MOCK_METHOD(void, ActiveTexture, (uint32), (override));
    MOCK_METHOD(void, DeleteObject, (uint32), (override));
    MOCK_METHOD(void, DeleteShaderBuffer, (uint32), (override));
    MOCK_METHOD(std::string, GetBufferStatus, (), (override));
    MOCK_METHOD(ID, CreatePatchMesh, (const std::vector<float>&), (override));
    MOCK_METHOD(ID, CreatePurePatchMeshInstanced, (uint32, uint32), (override));
    MOCK_METHOD(ID, CreateMesh, (const MeshRawData&, RenderType), (override));
    MOCK_METHOD(ID, CreateDynamicLineMesh, (), (override));
    MOCK_METHOD(ID, CreateParticle, (), (override));
    MOCK_METHOD(ID, CreateAnimatedParticle, (), (override));
    MOCK_METHOD(void, Compute, (uint32, uint32, uint32), (override));
    MOCK_METHOD(void, RenderPurePatchedMeshInstances, (uint32), (override));
    MOCK_METHOD(void, RenderMesh, (uint32), (override));
    MOCK_METHOD(void, RenderDebugNormals, (uint32), (override));
    MOCK_METHOD(void, RenderTriangleStripMesh, (uint32), (override));
    MOCK_METHOD(void, RenderMeshInstanced, (uint32, uint32), (override));
    MOCK_METHOD(void, RenderPoints, (uint32), (override));
    MOCK_METHOD(void, RenderQuad, (), (override));
    MOCK_METHOD(void, RenderQuadTs, (), (override));
    MOCK_METHOD(void, EnableClipingPlane, (uint32), (override));
    MOCK_METHOD(void, DisableCliping, (uint32), (override));
    MOCK_METHOD(void, EnableCulling, (), (override));
    MOCK_METHOD(void, DisableCulling, (), (override));
    MOCK_METHOD(void, SetViewPort, (uint32, uint32, uint32, uint32), (override));
    MOCK_METHOD(void, BindTexture, (uint32), (const, override));
    MOCK_METHOD(void, BindImageTexture, (uint32, TextureAccess), (override));
    MOCK_METHOD(ID, CreateShadowMap, (uint32, uint32), (override));
    MOCK_METHOD(void, PolygonModeRender, (), (override));
    MOCK_METHOD(void, LineModeRender, (), (override));
    MOCK_METHOD(void, SetBlendFunction, (BlendFunctionType), (override));
    MOCK_METHOD(void, UpdateMatrixes, (uint32, const std::vector<mat4>&), (override));
    MOCK_METHOD(void, UpdateMesh, (uint32, const MeshRawData&, const std::set<VertexBufferObjects>&), (override));
    MOCK_METHOD(void, UpdateLineMesh, (uint32, const LineMesh&), (override));
    MOCK_METHOD(void, UpdateOffset, (uint32, const std::vector<vec4>&), (override));
    MOCK_METHOD(void, UpdateBlend, (uint32, const std::vector<float>&), (override));
    MOCK_METHOD(void, LoadViewMatrix, (const mat4&), (override));
    MOCK_METHOD(void, LoadProjectionMatrix, (const mat4&), (override));
    MOCK_METHOD(void, DrawLine, (const vec3& color, const vec3& from, const vec3& to), (override));
    MOCK_METHOD(mat4, PrepareMatrixToLoad, (const mat4&), (override));
    MOCK_METHOD(std::vector<uint8>, GetTextureData, (uint32), (const, override));
    MOCK_METHOD(const TextureInfo&, GetTextureInfo, (uint32), (const, override));
    MOCK_METHOD(void, TakeSnapshoot, (const std::string& path), (const, override));
    MOCK_METHOD(void, BindDefaultFrameBuffer, (), (override));
    MOCK_METHOD(IFrameBuffer&, CreateFrameBuffer, (const std::vector<FrameBuffer::Attachment>&), (override));
    MOCK_METHOD(void, DeleteFrameBuffer, (IFrameBuffer&), (override));
};

}  // namespace GraphicsApi
