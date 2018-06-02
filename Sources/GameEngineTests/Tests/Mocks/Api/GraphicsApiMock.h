#pragma once
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Resources/Models/MeshRawData.h"
#include <gmock/gmock.h>

namespace GameEngine
{
	struct GraphicsApiMock : public IGraphicsApi
	{
		MOCK_METHOD0(GetWindowApi, IWindowApiPtr());
		MOCK_METHOD0(Init, void());
		MOCK_METHOD0(CreateContext, void());
		MOCK_METHOD0(DeleteContext, void());
		MOCK_METHOD0(PrintVersion, void());
		MOCK_METHOD0(SetDefaultTarget, void());
		MOCK_METHOD1(SetBackgroundColor, void(const vec3&));
		MOCK_METHOD0(EnableDepthTest, void());
		MOCK_METHOD0(DisableDepthTest, void());
		MOCK_METHOD0(PrepareFrame, void());
		MOCK_METHOD2(CreateShader, uint32(const ShadersFiles&, GraphicsApiFunctions));
		MOCK_METHOD2(GetShaderVariableLocation, uint32(uint32, const std::string&));
		MOCK_METHOD1(UseShader, void(uint32));
		MOCK_METHOD3(BindAttribute, void(uint32, uint32, const std::string&));
		MOCK_METHOD2(LoadValueToShader, void(uint32, int));
		MOCK_METHOD2(LoadValueToShader, void(uint32, uint32));
		MOCK_METHOD2(LoadValueToShader, void(uint32, float));
		MOCK_METHOD2(LoadValueToShader, void(uint32, const vec2&));
		MOCK_METHOD2(LoadValueToShader, void(uint32, const vec3&));
		MOCK_METHOD2(LoadValueToShader, void(uint32, const vec4&));
		MOCK_METHOD2(LoadValueToShader, void(uint32, const mat3&));
		MOCK_METHOD2(LoadValueToShader, void(uint32, const mat4&));
		MOCK_METHOD6(CreateTexture, uint32(TextureType, TextureFilter, TextureMipmap, BufferAtachment, vec2ui, void* data));
		MOCK_METHOD2(CreateCubMapTexture, uint32(vec2ui, std::vector<void*>));
		MOCK_METHOD1(SetBuffers, void(const std::vector<BufferAtachment>&));
		MOCK_METHOD1(ClearBuffer, void(BufferType));
		MOCK_METHOD1(ClearBuffers, void(const std::vector<BufferType>&));
		MOCK_METHOD0(EnableBlend, void());
		MOCK_METHOD0(DisableBlend, void());
		MOCK_METHOD0(EnableDepthMask, void());
		MOCK_METHOD0(DisableDepthMask, void());
		MOCK_METHOD2(ActiveTexture, void(uint32, uint32));
		MOCK_METHOD1(ActiveTexture, void(uint32));
		MOCK_METHOD0(CreateBuffer, uint32());
		MOCK_METHOD2(BindBuffer, void(BindType, uint32));
		MOCK_METHOD1(DeleteObject, void(uint32));
		MOCK_METHOD0(GetBufferStatus, std::string());
		MOCK_METHOD2(CreatePurePatchMeshInstanced, uint32(uint32, uint32));
		MOCK_METHOD1(CreateMesh, uint32(const MeshRawData&));
		MOCK_METHOD0(CreateParticle, uint32());
		MOCK_METHOD0(CreateAnimatedParticle, uint32());
		MOCK_METHOD1(RenderPurePatchedMeshInstances, void(uint32));
		MOCK_METHOD1(RenderMesh, void(uint32));
		MOCK_METHOD2(RenderMeshInstanced, void(uint32, uint32));
		MOCK_METHOD1(RenderPoints, void(uint32));
		MOCK_METHOD0(RenderQuad, void());
		MOCK_METHOD0(RenderQuadTs, void());
		MOCK_METHOD0(EnableCulling, void());
		MOCK_METHOD0(DisableCulling, void());
		MOCK_METHOD4(SetViewPort, void(uint32, uint32, uint32, uint32));
		MOCK_METHOD1(BindTexture, void(uint32));
		MOCK_METHOD2(CreateShadowMap, uint32(uint32, uint32));
		MOCK_METHOD0(PolygonModeRender, void());
		MOCK_METHOD0(LineModeRender, void());
		MOCK_METHOD1(SetBlendFunction, void(BlendFunctionType));
		MOCK_METHOD2(UpdateMatrixes, void(uint32, const std::vector<mat4>&));
		MOCK_METHOD2(UpdateOffset, void(uint32, const std::vector<vec4>&));
		MOCK_METHOD2(UpdateBlend, void(uint32, const std::vector<float>&));
		MOCK_METHOD1(CloneImage, uint32(uint32));

		//temp
		MOCK_METHOD1(CreateFont, void(const std::string&));
		MOCK_METHOD2(PrintText, void(const std::string&, const vec2i&));
	};
	typedef std::shared_ptr<GraphicsApiMock> GraphicsApiMockPtr;
}
