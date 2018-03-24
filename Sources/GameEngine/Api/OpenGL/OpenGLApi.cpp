#include "OpenGLApi.h"
#include "Logger/Log.h"
#include "GameEngine/Api/SDL2/SDLOpenGL.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/Models/MeshRawData.h"
#include "OpenGL/OpenGLUtils.h"
#include "OpenGL/VaoEnableController.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Font.h"
#include <GL/glew.h>

enum class ObjectType
{
	TEXTURE_2D,
	TEXTURE_CUBE_MAP,
	FRAME_BUFFER,
	RENDER_BUFFER,
	SHADER_PROGRAM,
	MESH
};

CFont font;
std::unordered_map<uint32, ObjectType> createdObjectIds;

namespace GameEngine
{
	OpenGLApi::OpenGLApi()
		: OpenGLApi(std::make_shared<SdlOpenGlApi>())
	{
	}
	OpenGLApi::OpenGLApi(IWindowApiPtr windowApi)
		: windowApi_(windowApi)
		, usedShader(0)
		, objectId(1)
	{
		shaderTypeMap_ = 
		{
			{ ShaderType::VERTEX_SHADER, GL_VERTEX_SHADER },
			{ ShaderType::FRAGMENT_SHADER, GL_FRAGMENT_SHADER},
			{ ShaderType::GEOMETRY_SHADER, GL_GEOMETRY_SHADER },
			{ ShaderType::TESS_CONTROL_SHADER, GL_TESS_CONTROL_SHADER },
			{ ShaderType::TESS_EVALUATION_SHADER, GL_TESS_EVALUATION_SHADER }
		};
		textureFilterMap_ = 
		{
			{ TextureFilter::LINEAR, GL_LINEAR },
			{ TextureFilter::NEAREST, GL_NEAREST }
		};

		bufferAtachmentMap_ = 
		{
			{ BufferAtachment::COLOR_1, GL_COLOR_ATTACHMENT0 },
			{ BufferAtachment::COLOR_2, GL_COLOR_ATTACHMENT1 },
			{ BufferAtachment::COLOR_3, GL_COLOR_ATTACHMENT2 },
			{ BufferAtachment::COLOR_4, GL_COLOR_ATTACHMENT3 },
			{ BufferAtachment::DEPTH, GL_DEPTH_ATTACHMENT }
		};

		bufferTypeMap_ = 
		{
			{ BufferType::COLOR, GL_COLOR_BUFFER_BIT },
			{ BufferType::DEPTH, GL_DEPTH_BUFFER_BIT }
		};
	}
	void OpenGLApi::Init()
	{
		GLint glew_init_result = glewInit();
		if (glew_init_result == GLEW_OK)
		{
			quad.Init();
			return;
		}

		std::string err(reinterpret_cast<char const *>(glewGetErrorString(glew_init_result)));
		Log("[Error] Glew init error : " + err);
	}
	void OpenGLApi::CreateContext()
	{
		windowApi_->CreateContext();
	}
	void OpenGLApi::DeleteContext()
	{
		windowApi_->DeleteContext();
	}
	void OpenGLApi::PrintVersion()
	{
		std::string ver(reinterpret_cast<char const *>(glGetString(GL_VERSION)));
		Log("GL version: " + ver);

		std::string glslver(reinterpret_cast<char const *>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
		Log("GLSL version: " + glslver);

		GLint MaxPatchVertices = 0;
		glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
		Log("Max supported patch vertices :" + std::to_string(MaxPatchVertices));
		glPatchParameteri(GL_PATCH_VERTICES, 3);
	}
	IWindowApiPtr OpenGLApi::GetWindowApi()
	{
		return windowApi_;
	}
	void OpenGLApi::PrepareFrame()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(bgColor_.x, bgColor_.y, bgColor_.z, 1.f);
	}
	void OpenGLApi::SetDefaultTarget()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}
	void OpenGLApi::SetBackgroundColor(const vec3& color )
	{
		bgColor_ = color;
	}
	void OpenGLApi::EnableDepthTest()
	{
		glEnable(GL_DEPTH_TEST);
	}
	void OpenGLApi::DisableDepthTest()
	{
		glDisable(GL_DEPTH_TEST);
	}
	uint32 OpenGLApi::CreateShader(const ShadersFiles& files, GraphicsApiFunctions functions)
	{
		auto program = CreateShaderProgram();
		if (!program)
			return false;

		auto programId = program.constValue();

		if (functions.count(GraphicFunctionType::SHADER_SET_ID) != 0)
			functions[GraphicFunctionType::SHADER_SET_ID](programId);

		shaderPrograms_[programId].id = programId;

		for (const auto& p : files)
		{
			if (!AddShader(programId, p.first, p.second))
				return false;
		}

		if (!FinalizeShader(programId, functions))
			return false;

		auto rid = ConvertAndRememberId(programId);
		createdObjectIds[rid] = ObjectType::SHADER_PROGRAM;
		return rid;
	}
	void OpenGLApi::DeleteMesh(uint32 id)
	{
		if (openGlMeshes_.count(id) == 0)
			return;

		auto& mesh = openGlMeshes_[id];

		for (auto& vbo : mesh.vbos)
		{
			if (vbo.second != 0)
				glDeleteBuffers(1, &vbo.second);
		}

		glDeleteVertexArrays(1, &mesh.vao);

		openGlMeshes_.erase(id);
	}
	uint32 OpenGLApi::ConvertAndRememberId(uint32 id)
	{
		auto oid = objectId;
		++objectId;

		idToGlId_[oid] = id;
		return oid;
	}
	void OpenGLApi::DeleteShader(uint32 programId)
	{
		if (usedShader == programId)
			glUseProgram(0);

		for (auto& id : shaderPrograms_[programId].shaderObjectsList)
		{
			glDetachShader(programId, id);
			glDeleteShader(id);
		}

		if (programId != 0)
			glDeleteProgram(programId);
	}
	void OpenGLApi::UseShader(uint32 id)
	{
		usedShader = idToGlId_[id];
		glUseProgram(usedShader);
	}
	wb::optional<uint32> OpenGLApi::CreateShaderProgram()
	{
		auto id = glCreateProgram();
		if (id == 0)
		{
			Error("[Error] Error creating shader program.");
			return wb::optional<uint32>();
		}
		return id;
	}
	bool OpenGLApi::AddShader(uint32 programId, const std::string& filename, GameEngine::ShaderType mode)
	{
		auto full_path = EngineConf_GetFullShaderPath(filename);
		EngineConf_AddRequiredFile(full_path);

		std::string source = Utils::ReadFile(full_path);

		uint32 id;
		id = glCreateShader(shaderTypeMap_[mode]);

		if (id == 0)
		{
			Error("[Error] Error creating shader type " + shaderTypeMap_[mode]);
			return false;
		}

		shaderPrograms_[programId].shaderObjectsList.push_back(id);

		const char* csource = source.c_str();

		glShaderSource(id, 1, &csource, NULL);

		glCompileShader(id);

		GLint compiled = GL_FALSE;
		glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);

		if (!compiled)
		{
			char err[1000];
			int length = 0;
			glGetShaderInfoLog(id, 1000, &length, err);
			Log("[Error] ERRORS in Shader! \nFile name:\t" + filename + "\nCompile status: \n\n" + err);
			return false;
		}
		if (id == GL_FALSE)
		{
		}
		glAttachShader(programId, id);

		return true;
	}

	bool OpenGLApi::FinalizeShader(uint32 programId, GraphicsApiFunctions functions)
	{
		if (functions.count(GraphicFunctionType::SHADER_BIND_ATTRIBUTES) != 0)
			functions[GraphicFunctionType::SHADER_BIND_ATTRIBUTES](0);

		glLinkProgram(programId);

		GLint Success = 0;
		GLchar ErrorLog[1024] = { 0 };

		glGetProgramiv(programId, GL_LINK_STATUS, &Success);
		if (Success == 0)
		{
			glGetProgramInfoLog(programId, sizeof(ErrorLog), NULL, ErrorLog);
			Error("[Error] Error linking shader program: " + std::to_string(programId) + " : " + std::string(ErrorLog));
			return false;
		}

		glUseProgram(programId);

		if (functions.count(GraphicFunctionType::SHADER_VARIABLES_LOCATION) != 0)
			functions[GraphicFunctionType::SHADER_VARIABLES_LOCATION](0);

		if (functions.count(GraphicFunctionType::SHADER_CONNECT_TEXTURES) != 0)
			functions[GraphicFunctionType::SHADER_CONNECT_TEXTURES](0);

		glValidateProgram(programId);
		glGetProgramiv(programId, GL_VALIDATE_STATUS, &Success);

		if (!Success)
		{
			glGetProgramInfoLog(programId, sizeof(ErrorLog), NULL, ErrorLog);
			Error("[Error] Invalid shader program : " + std::to_string(programId) + " : " + std::string(ErrorLog));
			return false;
		}

		for (auto& id : shaderPrograms_[programId].shaderObjectsList)
			glDeleteShader(id);

		shaderPrograms_[programId].shaderObjectsList.clear();

		if (glGetError() != GL_NO_ERROR)
		{
			glGetProgramInfoLog(programId, sizeof(ErrorLog), NULL, ErrorLog);
			std::string error(ErrorLog);
			Error("[Error] Invalid shader program. " + std::to_string(programId) + " : " + error);
			return false;
		}

		glUseProgram(0);

		return true;
	}
	uint32 OpenGLApi::GetShaderVariableLocation(uint32 id, const std::string & varname)
	{
		return glGetUniformLocation(id, varname.c_str());;
	}
	void OpenGLApi::BindAttribute(uint32 programId, uint32 attribute, const std::string & variableName)
	{
		glBindAttribLocation(programId, attribute, variableName.c_str());
	}
	void OpenGLApi::LoadValueToShader(uint32 loacation, int value)
	{
		glUniform1i(loacation, value);
	}
	void OpenGLApi::LoadValueToShader(uint32 loacation, float value)
	{
		glUniform1f(loacation, value);
	}
	void OpenGLApi::LoadValueToShader(uint32 loacation, uint32 value)
	{
		glUniform1ui(loacation, value);
	}
	void OpenGLApi::LoadValueToShader(uint32 loacation, const vec2 & value)
	{
		glUniform2fv(loacation, 1, glm::value_ptr(value));
	}
	void OpenGLApi::LoadValueToShader(uint32 loacation, const vec3 & value)
	{
		glUniform3fv(loacation, 1, glm::value_ptr(value));
	}
	void OpenGLApi::LoadValueToShader(uint32 loacation, const vec4 & value)
	{
		glUniform4fv(loacation, 1, glm::value_ptr(value));
	}
	void OpenGLApi::LoadValueToShader(uint32 loacation, const mat3 & value)
	{
		glUniformMatrix3fv(loacation, 1, GL_FALSE, glm::value_ptr(value));
	}
	void OpenGLApi::LoadValueToShader(uint32 loacation, const mat4& value)
	{
		glUniformMatrix4fv(loacation, 1, GL_FALSE, glm::value_ptr(value));
	}

	uint32 OpenGLApi::CreateTexture(TextureType type, TextureFilter filter, TextureMipmap mimpamp, BufferAtachment atachment, vec2i size, void* data)
	{
		uint32 textureType = GL_TEXTURE_2D;
		GLint dataType = GL_FLOAT;
		GLint internalFormat = GL_RGBA;
		GLint format = GL_RGBA;

		if (type == TextureType::FLOAT_BUFFER_2D || type == TextureType::FLOAT_TEXTURE_2D || type == TextureType::DEPTH_BUFFER_2D)
		{
			dataType = GL_FLOAT;
			if (type == TextureType::DEPTH_BUFFER_2D)
			{
				internalFormat = GL_DEPTH_COMPONENT32;
				format = GL_DEPTH_COMPONENT;
			}
			else
			{
				internalFormat = GL_RGBA16F;
			}
		}
		else if (type == TextureType::FLOAT_TEXTURE_1C)
		{
			format = GL_RED;
			dataType = GL_FLOAT;
			internalFormat = GL_R32F;
		}
		else if (type == TextureType::U8_RGBA)
		{
			format = GL_RGBA;
			dataType = GL_UNSIGNED_BYTE;
			internalFormat = GL_RGBA;
		}
		else if (type == TextureType::FLOAT_TEXTURE_4C)
		{
			format = GL_RGBA;
			dataType = GL_FLOAT;
			internalFormat = GL_RGBA32F;
		}
		GLuint texture;
		glGenTextures(1, &texture);

		GLenum hubo_error = glGetError();

		if (hubo_error)
		{
			return 0;
		}

		glBindTexture(textureType, texture);
		glTexImage2D(textureType, 0, internalFormat, size.x, size.y, 0, format, dataType, data);
		glTexParameterf(textureType, GL_TEXTURE_MIN_FILTER, (GLfloat) textureFilterMap_[filter]);
		glTexParameterf(textureType, GL_TEXTURE_MAG_FILTER, (GLfloat) textureFilterMap_[filter]);

		if (type == TextureType::FLOAT_BUFFER_2D || type == TextureType::DEPTH_BUFFER_2D)
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, bufferAtachmentMap_[atachment], textureType, texture, 0);

		if (mimpamp == TextureMipmap::LINEAR)
		{
			glGenerateMipmap(textureType);
			glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(textureType, GL_TEXTURE_LOD_BIAS, 0);
		}

		glBindTexture(textureType, 0);

		auto rid = ConvertAndRememberId(texture);
		createdObjectIds[rid] = ObjectType::TEXTURE_2D;
		return rid;
	}

	uint32 OpenGLApi::CreateCubMapTexture(vec2i size, std::vector<void*> data)
	{
		if (data.size() != 6)
			return 0;

		uint32 id;
		glGenTextures(1, &id);
		GLenum hubo_error = glGetError();

		if (hubo_error)
		{
			return 0;
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		for (int x = 0; x < 6; x++)
		{
			auto ptr = data[x];
		    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptr);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		auto rid = ConvertAndRememberId(id);
		createdObjectIds[rid] = ObjectType::TEXTURE_CUBE_MAP;
		return rid;
	}

	void OpenGLApi::SetBuffers(const std::vector<BufferAtachment>& attachemnts)
	{
		GLenum* drawBuffers;

		drawBuffers = new GLenum[attachemnts.size()];

		uint32 i = 0;
		for (auto at : attachemnts)
		{
			drawBuffers[i++] = bufferAtachmentMap_[at];
		}
		glDrawBuffers(attachemnts.size(), drawBuffers);

		delete[] drawBuffers;
	}

	void OpenGLApi::ClearBuffer(BufferType type)
	{
		glClear(bufferTypeMap_[type]);
	}

	void OpenGLApi::ClearBuffers(const std::vector<BufferType>& buffers)
	{
		if (buffers.empty())
			return;

		GLbitfield mask = bufferTypeMap_[buffers.front()];

		bool first = true;
		for (auto buffer : buffers)
		{
			if (first)
			{
				first = false;
				continue;
			}
			mask |= bufferTypeMap_[buffer];
		}

		glClear(mask);
	}

	void OpenGLApi::EnableBlend()
	{
		glEnable(GL_BLEND);
	}

	void OpenGLApi::DisableBlend()
	{
		glDisable(GL_BLEND);
	}

	void OpenGLApi::EnableDepthMask()
	{
		glDepthMask(GL_TRUE);
	}

	void OpenGLApi::DisableDepthMask()
	{
		glDepthMask(GL_FALSE);
	}

	void OpenGLApi::ActiveTexture(uint32 nr)
	{
		glActiveTexture(GL_TEXTURE0 + nr);
	}

	void OpenGLApi::ActiveTexture(uint32 nr, uint32 id)
	{
		auto openGLId = idToGlId_[id];

		glActiveTexture(GL_TEXTURE0 + nr);

		if (createdObjectIds[id] == ObjectType::TEXTURE_CUBE_MAP)
			glBindTexture(GL_TEXTURE_CUBE_MAP, openGLId);
		else
			glBindTexture(GL_TEXTURE_2D, openGLId);
	}

	uint32 OpenGLApi::CreateBuffer()
	{
		uint32 id;
		glGenFramebuffers(1, &id);
		auto rid = ConvertAndRememberId(id);
		createdObjectIds[rid] = ObjectType::FRAME_BUFFER;
		return rid;
	}

	void OpenGLApi::BindBuffer(BindType type, uint32 id)
	{
		auto openGLId = idToGlId_[id];

		switch (type)
		{
		case BindType::DEFAULT:
			glBindFramebuffer(GL_FRAMEBUFFER, openGLId);
			break;
		case BindType::DRAW:
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, openGLId);
			break;
		}
	}

	void OpenGLApi::DeleteObject(uint32 id)
	{
		if (id == 0)
			return;

		auto openGLId = idToGlId_[id];

		switch (createdObjectIds[id])
		{
		case ObjectType::SHADER_PROGRAM: DeleteShader(openGLId); break;
		case ObjectType::TEXTURE_2D: glDeleteTextures(1, &openGLId);  break;
		case ObjectType::TEXTURE_CUBE_MAP: glDeleteTextures(1, &openGLId);  break;
		case ObjectType::RENDER_BUFFER: glDeleteRenderbuffers(1, &openGLId);  break;
		case ObjectType::FRAME_BUFFER: glDeleteFramebuffers(1, &openGLId);  break;
		case ObjectType::MESH: DeleteMesh(id); break;
		}
	}

	std::string OpenGLApi::GetBufferStatus()
	{
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			Log("[Error] FB error, status: " + std::to_string(status));
			return std::to_string(status);
		}

		return std::string();
	}

	uint32 OpenGLApi::CreatePurePatchMeshInstanced(uint32 patch, uint32 count)
	{
		uint32 vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glPatchParameteri(GL_PATCH_VERTICES, patch);
		glBindVertexArray(0);

		auto rid = objectId;
		createdObjectIds[rid] = ObjectType::MESH;
		openGlMeshes_[rid].instancesCount = count;
		openGlMeshes_[rid].patches = patch;
		objectId++;
		return rid;
	}

	uint32 OpenGLApi::CreateMesh(const MeshRawData& meshRawData)
	{
		auto rid = objectId;
		createdObjectIds[rid] = ObjectType::MESH;
		objectId++;

		auto& mesh = openGlMeshes_[rid];
		mesh.vao = Utils::CreateVao();

		mesh.vertexCount = meshRawData.indices_.size() > 0 ? meshRawData.indices_.size() : meshRawData.positions_.size() / 3;

		if (meshRawData.indices_.size() > 0)
		{
			GLuint vboId = Utils::BindIndicesBuffer(meshRawData.indices_);
			mesh.vbos[VertexBufferObjects::INDICES] = vboId;
		}

		if (meshRawData.positions_.size() > 0)
		{
			GLuint vboId = Utils::StoreDataInAttributesList(0, 3, meshRawData.positions_);
			mesh.vbos[VertexBufferObjects::POSITION] = vboId;
			mesh.attributes[VertexBufferObjects::POSITION] = 0;
		}
		if (meshRawData.textCoords_.size() > 0)
		{
			GLuint vboId = Utils::StoreDataInAttributesList(1, 2, meshRawData.textCoords_);
			mesh.vbos[VertexBufferObjects::TEXT_COORD] = vboId;
			mesh.attributes[VertexBufferObjects::TEXT_COORD] = 1;
		}
		if (meshRawData.normals_.size() > 0)
		{
			GLuint vboId = Utils::StoreDataInAttributesList(2, 3, meshRawData.normals_);
			mesh.vbos[VertexBufferObjects::NORMAL] = vboId;
			mesh.attributes[VertexBufferObjects::NORMAL] = 2;
		}
		if (meshRawData.tangents_.size() > 0)
		{
			GLuint vboId = Utils::StoreDataInAttributesList(3, 3, meshRawData.tangents_);
			mesh.vbos[VertexBufferObjects::TANGENT] = vboId;
			mesh.attributes[VertexBufferObjects::TANGENT] = 3;
		}
		if (meshRawData.bonesWeights_.size() > 0)
		{
			GLuint vboId = Utils::StoreDataInAttributesList(4, 3, meshRawData.bonesWeights_);
			mesh.vbos[VertexBufferObjects::WEIGHTS] = vboId;
			mesh.attributes[VertexBufferObjects::WEIGHTS] = 4;
		}
		if (meshRawData.joinIds_.size() > 0)
		{
			GLuint vboId = Utils::StoreDataInAttributesList(5, 3, meshRawData.joinIds_);
			mesh.vbos[VertexBufferObjects::JOINTS] = vboId;
			mesh.attributes[VertexBufferObjects::JOINTS] = 5;
		}
		Utils::UnbindVao();
		return rid;
	}

	void OpenGLApi::RenderPurePatchedMeshInstances(uint32 id)
	{
		const auto& obj = openGlMeshes_[id];
		glBindVertexArray(obj.vao);
		glDrawArraysInstanced(GL_PATCHES, 0, obj.patches, obj.instancesCount);
		glBindVertexArray(0);
	}

	void OpenGLApi::RenderMesh(uint32 id)
	{
		if (openGlMeshes_.count(id) == 0)
			return;

		auto& mesh = openGlMeshes_[id];

		Utils::EnableVao ev(mesh.vao, mesh.attributes);
		glDrawElements(GL_TRIANGLES, mesh.vertexCount, GL_UNSIGNED_SHORT, 0);
	}

	void OpenGLApi::RenderMeshInstanced(uint32 id, uint32 istanced)
	{
		if (openGlMeshes_.count(id) == 0)
			return;

		auto& mesh = openGlMeshes_[id];

		Utils::EnableVao ev(mesh.vao, mesh.attributes);
		glDrawElementsInstanced(GL_TRIANGLES, mesh.vertexCount, GL_UNSIGNED_SHORT, 0, istanced);
	}

	void OpenGLApi::RenderPoints(uint32 id)
	{
		if (openGlMeshes_.count(id) == 0)
			return;

		auto& mesh = openGlMeshes_[id];
		Utils::EnableVao ev(mesh.vao, { { VertexBufferObjects::POSITION, 0 } });
		glDrawArrays(GL_POINTS, 0, mesh.vertexCount);
	}

	void OpenGLApi::RenderQuad()
	{
		Utils::SimpleRenderVao(quad.vao, quad.indicesSize, 2);
	}

	void OpenGLApi::EnableCulling()
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	void OpenGLApi::DisableCulling()
	{
		glDisable(GL_CULL_FACE);
	}

	void OpenGLApi::SetViewPort(uint32 x, uint32 y, uint32 width, uint32 height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLApi::BindTexture(uint32 id)
	{
		glBindTexture(GL_TEXTURE_2D, idToGlId_[id]);
	}

	uint32 OpenGLApi::CreateShadowMap(uint32 sizex, uint32 sizey)
	{
		auto shadowMapId = Utils::CreateDepthBufferAttachment(sizex, sizey);
		auto rid = ConvertAndRememberId(shadowMapId);
		createdObjectIds[rid] = ObjectType::TEXTURE_2D;
		return rid;
	}

	void OpenGLApi::PolygonModeRender()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void OpenGLApi::LineModeRender()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	void OpenGLApi::CreateFont(const std::string& filename)
	{
		font.Init(filename);
	}
	
	void OpenGLApi::PrintText(const std::string& text, const vec2i& position)
	{
		glPushMatrix();
		glLoadIdentity();
		font.Print((int) position.x, (int) position.y, text.c_str());
		glPopMatrix();
	}

} // GameEngine
