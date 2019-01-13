#include "OpenGLApi.h"
#include <GL/glew.h>
#include <optional>
#include "Font.h"
#include "GameEngine/Api/SDL2/SDLOpenGL.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/Models/MeshRawData.h"
#include "Logger/Log.h"
#include "OpenGL/OpenGLUtils.h"
#include "glm/gtc/type_ptr.hpp"

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
wb::optional<GLuint> CreateShaderProgram()
{
    auto id = glCreateProgram();
    if (id == 0)
    {
        Error("[Error] Error creating shader program.");
        return wb::optional<GLuint>();
    }
    return id;
}

OpenGLMesh Convert(const Utils::Vao& v)
{
    OpenGLMesh mesh;
    mesh.vao         = v.vao;
    mesh.vbos        = v.vbos;
    mesh.attributes  = v.attributes;
    mesh.vertexCount = v.size;
    return mesh;
}
OpenGLApi::OpenGLApi()
    : OpenGLApi(std::make_shared<SdlOpenGlApi>())
{
}
OpenGLApi::OpenGLApi(IWindowApiPtr windowApi)
    : activeBuffer_(0)
    , objectId_(1)
    , windowApi_(windowApi)
    , usedShader(0)
    , bgColor_(0)
    , quad_()
    , quadTs_(true)
    , polygonMode(true)
{
    idToGlId_.insert({0, 0});

    shaderTypeMap_    = {{ShaderType::VERTEX_SHADER, GL_VERTEX_SHADER},
                      {ShaderType::FRAGMENT_SHADER, GL_FRAGMENT_SHADER},
                      {ShaderType::GEOMETRY_SHADER, GL_GEOMETRY_SHADER},
                      {ShaderType::TESS_CONTROL_SHADER, GL_TESS_CONTROL_SHADER},
                      {ShaderType::TESS_EVALUATION_SHADER, GL_TESS_EVALUATION_SHADER}};
    textureFilterMap_ = {{TextureFilter::LINEAR, GL_LINEAR}, {TextureFilter::NEAREST, GL_NEAREST}};

    bufferAtachmentMap_ = {{BufferAtachment::COLOR_1, GL_COLOR_ATTACHMENT0},
                           {BufferAtachment::COLOR_2, GL_COLOR_ATTACHMENT1},
                           {BufferAtachment::COLOR_3, GL_COLOR_ATTACHMENT2},
                           {BufferAtachment::COLOR_4, GL_COLOR_ATTACHMENT3},
                           {BufferAtachment::DEPTH, GL_DEPTH_ATTACHMENT}};

    bufferTypeMap_ = {{BufferType::COLOR, GL_COLOR_BUFFER_BIT}, {BufferType::DEPTH, GL_DEPTH_BUFFER_BIT}};
}

void OpenGLApi::Init()
{
    GLint glew_init_result = glewInit();
    if (glew_init_result == GLEW_OK)
    {
        quad_.Init();
        quadTs_.Init();
        return;
    }

    std::string err(reinterpret_cast<char const*>(glewGetErrorString(glew_init_result)));
    Log("[Error] Glew init error : " + err);
}
void OpenGLApi::SetShaderQuaility(ShaderQuaility q)
{
    shaderQuality_ = q;
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
    std::string ver(reinterpret_cast<char const*>(glGetString(GL_VERSION)));
    Log("GL version: " + ver);

    std::string glslver(reinterpret_cast<char const*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
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

    auto code = glGetError();
    if (code != GL_NO_ERROR)
    {
        auto errString = gluErrorString(code);
        std::string error((char*)errString);
        Error(error);
    }
}
void OpenGLApi::SetDefaultTarget()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
void OpenGLApi::SetBackgroundColor(const vec3& color)
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
        return 0;

    auto programId = program.constValue();

    if (functions.count(GraphicFunctionType::SHADER_SET_ID) != 0)
        functions.at(GraphicFunctionType::SHADER_SET_ID)(programId);

    shaderPrograms_.insert({programId, OpenGLShaderProgram()});
    shaderPrograms_.at(programId).id   = programId;
    shaderPrograms_.at(programId).name = files.begin()->first;

    for (const auto& p : files)
    {
        if (!AddShader(programId, p.first, p.second))
            return false;
    }

    if (!FinalizeShader(programId, functions))
        return false;

    auto rid = ConvertAndRememberId(programId);
    createdObjectIds.insert({rid, ObjectType::SHADER_PROGRAM});
    return rid;
}
void OpenGLApi::DeleteMesh(uint32 id)
{
    if (openGlMeshes_.count(id) == 0)
        return;

    auto& mesh = openGlMeshes_.at(id);

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
    auto oid = objectId_;
    ++objectId_;

    idToGlId_.insert({oid, id});
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
    usedShader = idToGlId_.at(id);
    glUseProgram(usedShader);
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
        Error("[Error] Error creating shader type " + shaderTypeMap_.at(mode));
        return false;
    }

    shaderPrograms_.at(programId).shaderObjectsList.push_back(id);

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
        functions.at(GraphicFunctionType::SHADER_BIND_ATTRIBUTES)(0);

    glLinkProgram(programId);

    GLint Success         = 0;
    GLchar ErrorLog[1024] = {0};

    glGetProgramiv(programId, GL_LINK_STATUS, &Success);
    if (Success == 0)
    {
        glGetProgramInfoLog(programId, sizeof(ErrorLog), NULL, ErrorLog);
        Error("Error linking shader program: " + std::to_string(programId) + " : " + std::string(ErrorLog));
        return false;
    }

    glUseProgram(programId);

    if (functions.count(GraphicFunctionType::SHADER_VARIABLES_LOCATION) != 0)
        functions.at(GraphicFunctionType::SHADER_VARIABLES_LOCATION)(0);

    if (functions.count(GraphicFunctionType::SHADER_CONNECT_TEXTURES) != 0)
        functions.at(GraphicFunctionType::SHADER_CONNECT_TEXTURES)(0);

    glValidateProgram(programId);
    glGetProgramiv(programId, GL_VALIDATE_STATUS, &Success);

    if (!Success)
    {
        glGetProgramInfoLog(programId, sizeof(ErrorLog), NULL, ErrorLog);
        Error("Invalid shader program : " + std::to_string(programId) + " : " + std::string(ErrorLog));
        return false;
    }

    for (auto& id : shaderPrograms_.at(programId).shaderObjectsList)
        glDeleteShader(id);

    shaderPrograms_.at(programId).shaderObjectsList.clear();

    auto glError = glGetError();
    if (glError != GL_NO_ERROR)
    {
        glGetProgramInfoLog(programId, sizeof(ErrorLog), NULL, ErrorLog);
        std::string error(ErrorLog);
        Error("GlError ShaderprogramID: : " + shaderPrograms_.at(programId).name +
              " error code : " + std::to_string(glError) + " : ");
    }

    glUseProgram(0);

    return true;
}
ShadersFiles OpenGLApi::GetShaderFiles(Shaders shaderType)
{
    switch (shaderType)
    {
        case Shaders::Loading:
            return {{"Game/loadingShader.vert", ShaderType::VERTEX_SHADER},
                    {"Game/loadingShader.frag", ShaderType::FRAGMENT_SHADER}};
        case Shaders::Entity:
            return {{"Entity/EntityGeometryPassShader.vert", ShaderType::VERTEX_SHADER},
                    {"Entity/EntityGeometryPassShader.frag", ShaderType::FRAGMENT_SHADER}};
        case Shaders::Grass:
            return {{"Grass/GrassShader.vert", ShaderType::VERTEX_SHADER},
                    {"Grass/GrassShader.frag", ShaderType::FRAGMENT_SHADER},
                    {"Grass/GrassShader.geom", ShaderType::GEOMETRY_SHADER}};
        case Shaders::Particles:
            return {{"Particles/ParticlesShader.vert", ShaderType::VERTEX_SHADER},
                    {"Particles/ParticlesShader.frag", ShaderType::FRAGMENT_SHADER}};
        case Shaders::AnimatedParticles:
            return {{"Particles/AnimatedParticlesShader.vert", ShaderType::VERTEX_SHADER},
                    {"Particles/AnimatedParticlesShader.frag", ShaderType::FRAGMENT_SHADER}};
        case Shaders::Plants:
            return {{"Terrain/TerrainShader.vert", ShaderType::VERTEX_SHADER},
                    {"Terrain/TerrainShader.frag", ShaderType::FRAGMENT_SHADER},
                    {"Terrain/TerrainShader.cs", ShaderType::TESS_CONTROL_SHADER},
                    {"Terrain/TerrainShader.es", ShaderType::TESS_EVALUATION_SHADER}};
        case Shaders::Shadows:
            return {{"Shadows/ShadowVertexShader.vert", ShaderType::VERTEX_SHADER},
                    {"Shadows/ShadowFragmentShader.frag", ShaderType::FRAGMENT_SHADER}};
        case Shaders::SkyBox:
            return {{"SkyBox/SkyboxVertexShader.vert", GameEngine::ShaderType::VERTEX_SHADER},
                    {"SkyBox/SkyboxFragmentShader.frag", GameEngine::ShaderType::FRAGMENT_SHADER}};
        case Shaders::Terrain:
            return {{"Terrain/TerrainShader.vert", ShaderType::VERTEX_SHADER},
                    {"Terrain/TerrainShader.frag", ShaderType::FRAGMENT_SHADER},
                    {"Terrain/TerrainShader.cs", ShaderType::TESS_CONTROL_SHADER},
                    {"Terrain/TerrainShader.es", ShaderType::TESS_EVALUATION_SHADER}};
        case Shaders::Tree:
            return {{"Tree/TreeShader.vert", ShaderType::VERTEX_SHADER},
                    {"Tree/TreeShader.frag", ShaderType::FRAGMENT_SHADER}};
        case Shaders::Water:
            return {{"Terrain/TerrainShader.vert", ShaderType::VERTEX_SHADER},
                    {"Terrain/TerrainShader.frag", ShaderType::FRAGMENT_SHADER},
                    {"Terrain/TerrainShader.cs", ShaderType::TESS_CONTROL_SHADER},
                    {"Terrain/TerrainShader.es", ShaderType::TESS_EVALUATION_SHADER}};
        case Shaders::Blur:
            return {{"PostProcess/Blur/Blur.vert", GameEngine::ShaderType::VERTEX_SHADER},
                    {"PostProcess/Blur/Blur.frag", GameEngine::ShaderType::FRAGMENT_SHADER}};
        case Shaders::ColorFilper:
            return {{"PostProcess/ColorFliper/ColorFliper.vert", GameEngine::ShaderType::VERTEX_SHADER},
                    {"PostProcess/ColorFliper/ColorFliper.frag", GameEngine::ShaderType::FRAGMENT_SHADER}};
        case Shaders::Deffered:
            return {{"Light/LightPassShader.vert", ShaderType::VERTEX_SHADER},
                    {"Light/LightPassShader.frag", ShaderType::FRAGMENT_SHADER}};
        case Shaders::SSAO:
            return {{"PostProcess/SSAO/SSAOShader.vert", ShaderType::VERTEX_SHADER},
                    {"PostProcess/SSAO/SSAOShader.frag", ShaderType::FRAGMENT_SHADER}};
        case Shaders::Font:
            return {{"Font/fontVertex.vert", GameEngine::ShaderType::VERTEX_SHADER},
                    {"Font/fontFragment.frag", GameEngine::ShaderType::FRAGMENT_SHADER}};
        case Shaders::Texture:
            return {{"Gui/guiVertexShader.vert", ShaderType::VERTEX_SHADER},
                    {"Gui/guiFragmentShader.frag", ShaderType::FRAGMENT_SHADER}};
    }
    return {};
}
uint32 OpenGLApi::GetShaderVariableLocation(uint32 id, const std::string& varname)
{
    auto i = glGetUniformLocation(id, varname.c_str());

    if (i < 0)
    {
        Log("Variable \"" + varname + "\" not found " + " in : " + shaderPrograms_.at(id).name);
    }

    return static_cast<uint32>(i);
}
void OpenGLApi::BindAttribute(uint32 programId, uint32 attribute, const std::string& variableName)
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
void OpenGLApi::LoadValueToShader(uint32 loacation, const vec2& value)
{
    glUniform2fv(loacation, 1, glm::value_ptr(value));
}
void OpenGLApi::LoadValueToShader(uint32 loacation, const vec3& value)
{
    glUniform3fv(loacation, 1, glm::value_ptr(value));
}
void OpenGLApi::LoadValueToShader(uint32 loacation, const vec4& value)
{
    glUniform4fv(loacation, 1, glm::value_ptr(value));
}
void OpenGLApi::LoadValueToShader(uint32 loacation, const mat3& value)
{
    glUniformMatrix3fv(loacation, 1, GL_FALSE, glm::value_ptr(value));
}
void OpenGLApi::LoadValueToShader(uint32 loacation, const mat4& value)
{
    glUniformMatrix4fv(loacation, 1, GL_FALSE, glm::value_ptr(value));
}

void OpenGLApi::LoadValueToShader(uint32 loacation, const std::vector<float>& v)
{
    glUniform3fv(loacation, v.size(), (const GLfloat*)&v[0]);
}

void OpenGLApi::LoadValueToShader(uint32 loacation, const std::vector<vec3>& v)
{
    glUniform3fv(loacation, v.size(), (const GLfloat*)&v[0]);
}

void OpenGLApi::LoadValueToShader(uint32 loacation, const std::vector<mat4>& v)
{
    glUniformMatrix4fv(loacation, v.size(), GL_FALSE, (const GLfloat*)&v[0]);
}

uint32 OpenGLApi::CreateTexture(TextureType type, TextureFilter filter, TextureMipmap mimpamp,
                                BufferAtachment atachment, vec2ui size, void* data)
{
    uint32 textureType   = GL_TEXTURE_2D;
    GLint dataType       = GL_FLOAT;
    GLint internalFormat = GL_RGBA;
    GLint format         = GL_RGBA;

    if (type == TextureType::FLOAT_BUFFER_2D || type == TextureType::FLOAT_TEXTURE_2D ||
        type == TextureType::DEPTH_BUFFER_2D)
    {
        dataType = GL_FLOAT;
        if (type == TextureType::DEPTH_BUFFER_2D)
        {
            internalFormat = GL_DEPTH_COMPONENT32;
            format         = GL_DEPTH_COMPONENT;
        }
        else
        {
            internalFormat = GL_RGBA16F;
        }
    }
    else if (type == TextureType::FLOAT_TEXTURE_1C)
    {
        format         = GL_RED;
        dataType       = GL_FLOAT;
        internalFormat = GL_R32F;
    }
    else if (type == TextureType::U8_RGBA)
    {
        format         = GL_RGBA;
        dataType       = GL_UNSIGNED_BYTE;
        internalFormat = GL_RGBA;
    }
    else if (type == TextureType::FLOAT_TEXTURE_3C)
    {
        format         = GL_RGB;
        dataType       = GL_FLOAT;
        internalFormat = GL_RGB32F;
    }
    else if (type == TextureType::FLOAT_TEXTURE_4C)
    {
        format         = GL_RGBA;
        dataType       = GL_FLOAT;
        internalFormat = GL_RGBA32F;
    }
    GLuint texture;
    glGenTextures(1, &texture);

    GLenum hubo_error = glGetError();

    if (hubo_error)
    {
        auto errInfo = gluErrorString(hubo_error);
        Log("" + (char*)errInfo);
        return 0;
    }

    glBindTexture(textureType, texture);
    glTexImage2D(textureType, 0, internalFormat, size.x, size.y, 0, format, dataType, data);
    glTexParameterf(textureType, GL_TEXTURE_MIN_FILTER, (GLfloat)textureFilterMap_[filter]);
    glTexParameterf(textureType, GL_TEXTURE_MAG_FILTER, (GLfloat)textureFilterMap_[filter]);

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
    createdObjectIds.insert({rid, ObjectType::TEXTURE_2D});
    return rid;
}

uint32 OpenGLApi::CreateCubMapTexture(vec2ui size, std::vector<void*> data)
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

    auto rid              = ConvertAndRememberId(id);
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
    auto openGLId = idToGlId_.at(id);

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
    auto rid              = ConvertAndRememberId(id);
    createdObjectIds[rid] = ObjectType::FRAME_BUFFER;
    return rid;
}

void OpenGLApi::BindBuffer(BindType type, uint32 id)
{
    if (activeBuffer_ == id or idToGlId_.count(id) == 0)
        return;

    activeBuffer_ = id;

    auto openGLId = idToGlId_.at(id);

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
    if (id == 0 or idToGlId_.count(id) == 0)
        return;

    auto openGLId = idToGlId_.at(id);

    switch (createdObjectIds[id])
    {
        case ObjectType::SHADER_PROGRAM:
            DeleteShader(openGLId);
            break;
        case ObjectType::TEXTURE_2D:
            glDeleteTextures(1, &openGLId);
            break;
        case ObjectType::TEXTURE_CUBE_MAP:
            glDeleteTextures(1, &openGLId);
            break;
        case ObjectType::RENDER_BUFFER:
            glDeleteRenderbuffers(1, &openGLId);
            break;
        case ObjectType::FRAME_BUFFER:
            glDeleteFramebuffers(1, &openGLId);
            break;
        case ObjectType::MESH:
            DeleteMesh(id);
            break;
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

    auto rid                          = objectId_;
    createdObjectIds.insert({ rid, ObjectType::MESH });
    openGlMeshes_.insert({ rid, {} });
    openGlMeshes_.at(rid).instancesCount = count;
    openGlMeshes_.at(rid).patches        = patch;
    ++objectId_;
    return rid;
}

uint32 OpenGLApi::CreateMesh(const MeshRawData& meshRawData)
{
    auto rid              = objectId_;
    createdObjectIds.insert({ rid, ObjectType::MESH });
    ++objectId_;

    openGlMeshes_.insert({ rid, {} });
    auto& mesh = openGlMeshes_.at(rid);

    Utils::VaoCreator vaoCreator;
    vaoCreator.AddIndicesBuffer(meshRawData.indices_);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::POSITION, 3, meshRawData.positions_);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::TEXT_COORD, 2, meshRawData.textCoords_);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::NORMAL, 3, meshRawData.normals_);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::TANGENT, 3, meshRawData.tangents_);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::WEIGHTS, 3, meshRawData.bonesWeights_);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::JOINTS, 3, meshRawData.joinIds_);
    mesh = Convert(vaoCreator.Get());
    return rid;
}

uint32 OpenGLApi::CreateParticle()
{
    auto rid              = objectId_;
    createdObjectIds.insert({ rid, ObjectType::MESH });
    objectId_++;

    openGlMeshes_.insert({ rid, {} });
    auto& mesh = openGlMeshes_.at(rid);

    std::vector<float> vertex      = {-1, 1, 0, -1, -1, 0, 1, -1, 0, 1, 1, 0};
    std::vector<float> text_coords = {0, 0, 0, 1, 1, 1, 1, 0};
    std::vector<uint16> indices    = {0, 1, 3, 3, 1, 2};

    Utils::VaoCreator vaoCreator;
    vaoCreator.AddIndicesBuffer(indices);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::POSITION, 3, vertex);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::TEXT_COORD, 2, text_coords);
    vaoCreator.AllocateDynamicAttribute(VertexBufferObjects::TRANSFORM_MATRIX, 4, sizeof(mat4));
    mesh = Convert(vaoCreator.Get());
    return rid;
}
uint32 OpenGLApi::CreateAnimatedParticle()
{
    auto rid              = objectId_;
    createdObjectIds.insert({ rid, ObjectType::MESH });
    objectId_++;

    openGlMeshes_.insert({ rid, {} });
    auto& mesh = openGlMeshes_.at(rid);

    std::vector<float> vertex      = {-1, 1, 0, -1, -1, 0, 1, -1, 0, 1, 1, 0};
    std::vector<float> text_coords = {0, 0, 0, 1, 1, 1, 1, 0};
    std::vector<uint16> indices    = {0, 1, 3, 3, 1, 2};

    Utils::VaoCreator vaoCreator;
    vaoCreator.AddIndicesBuffer(indices);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::POSITION, 3, vertex);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::TEXT_COORD, 2, text_coords);
    vaoCreator.AllocateDynamicAttribute(VertexBufferObjects::TRANSFORM_MATRIX, 4, sizeof(mat4));
    vaoCreator.AllocateDynamicAttribute(VertexBufferObjects::TEXTURE_OFFSET, 4, sizeof(vec4));
    vaoCreator.AllocateDynamicAttribute(VertexBufferObjects::BLEND_FACTOR, 1, sizeof(float));
    mesh = Convert(vaoCreator.Get());
    return rid;
}
struct FloatBufferMatrix
{
    std::vector<float> rows[4];
};

void pushRow(const mat4& matrix, uint32 row, std::vector<float>& dest)
{
    dest.push_back(matrix[row][0]);
    dest.push_back(matrix[row][1]);
    dest.push_back(matrix[row][2]);
    dest.push_back(matrix[row][3]);
}

FloatBufferMatrix GetFloatBufferMatrix(const std::vector<mat4>& matrixes)
{
    FloatBufferMatrix result;
    result.rows[0].reserve(matrixes.size() * 4);
    result.rows[1].reserve(matrixes.size() * 4);
    result.rows[2].reserve(matrixes.size() * 4);
    result.rows[3].reserve(matrixes.size() * 4);
    for (const auto& mat : matrixes)
    {
        for (uint8 i = 0; i < 4; ++i)
        {
            pushRow(mat, i, result.rows[i]);
        }
    }
    return result;
}

void OpenGLApi::UpdateMatrixes(uint32 objectId, const std::vector<mat4>& mat)
{
    auto& obj = openGlMeshes_.at(objectId);

    glBindBuffer(GL_ARRAY_BUFFER, obj.vbos[VertexBufferObjects::TRANSFORM_MATRIX]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * mat.size(), &mat[0], GL_STREAM_DRAW);
}
void OpenGLApi::UpdateOffset(uint32 objectId, const std::vector<vec4>& offset)
{
    auto& obj = openGlMeshes_.at(objectId);

    glBindBuffer(GL_ARRAY_BUFFER, obj.vbos[VertexBufferObjects::TEXTURE_OFFSET]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * offset.size(), &offset[0], GL_STREAM_DRAW);
}
void OpenGLApi::UpdateBlend(uint32 objectId, const std::vector<float>& blendFactor)
{
    auto& obj = openGlMeshes_.at(objectId);

    glBindBuffer(GL_ARRAY_BUFFER, obj.vbos[VertexBufferObjects::BLEND_FACTOR]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * blendFactor.size(), &blendFactor[0], GL_STREAM_DRAW);
}
uint32 OpenGLApi::CloneImage(uint32 objectId)
{
    // auto& obj = idToGlId_.at(objectId];

    Log("Not implementet");
    // glCopyTexImage2D();
    return uint32();
}
void OpenGLApi::RenderPurePatchedMeshInstances(uint32 id)
{
    if (openGlMeshes_.count(id) == 0)
        return;

    const auto& obj = openGlMeshes_.at(id);
    glBindVertexArray(obj.vao);
    glDrawArraysInstanced(GL_PATCHES, 0, obj.patches, obj.instancesCount);
}

void OpenGLApi::RenderMesh(uint32 id)
{
    if (openGlMeshes_.count(id) == 0)
        return;

    auto& mesh = openGlMeshes_.at(id);

    glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLES, mesh.vertexCount, GL_UNSIGNED_SHORT, 0);
}

void OpenGLApi::RenderMeshInstanced(uint32 id, uint32 istanced)
{
    if (openGlMeshes_.count(id) == 0)
        return;

    auto& mesh = openGlMeshes_.at(id);

    glBindVertexArray(mesh.vao);
    glDrawElementsInstanced(GL_TRIANGLES, mesh.vertexCount, GL_UNSIGNED_SHORT, 0, istanced);
}

void OpenGLApi::RenderPoints(uint32 id)
{
    if (openGlMeshes_.count(id) == 0)
        return;

    auto& mesh = openGlMeshes_.at(id);
    glBindVertexArray(mesh.vao);
    glDrawArrays(GL_POINTS, 0, mesh.vertexCount);
}

void OpenGLApi::RenderQuad()
{
    quad_.Render(2);
}

void OpenGLApi::RenderQuadTs()
{
    quadTs_.Render(2);
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
    glBindTexture(GL_TEXTURE_2D, idToGlId_.at(id));
}

uint32 OpenGLApi::CreateShadowMap(uint32 sizex, uint32 sizey)
{
    auto shadowMapId = Utils::CreateDepthBufferAttachment(sizex, sizey);
    auto rid         = ConvertAndRememberId(shadowMapId);
    createdObjectIds.insert({rid, ObjectType::TEXTURE_2D});
    return rid;
}

void OpenGLApi::PolygonModeRender()
{
    if (polygonMode)
        return;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    polygonMode = true;
}

void OpenGLApi::LineModeRender()
{
    if (not polygonMode)
        return;

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    polygonMode = false;
}

void OpenGLApi::SetBlendFunction(BlendFunctionType type)
{
    switch (type)
    {
        case BlendFunctionType::ALPHA_ONE_MINUS_ALPHA:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case BlendFunctionType::ONE:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;
        case BlendFunctionType::ZERO:
            glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
            break;
        case BlendFunctionType::SRC_COLOR:
            glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);
            break;
        case BlendFunctionType::ONE_MINUS_SRC_COLOR:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
            break;
        case BlendFunctionType::DST_COLOR:
            glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);
            break;
        case BlendFunctionType::SRC_ALPHA:
            glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
            break;
        case BlendFunctionType::ONE_MINUS_DST_COLOR:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_COLOR);
            break;
    }
}

void OpenGLApi::CreateFont(const std::string& filename)
{
    font.Init(filename);
}

void OpenGLApi::PrintText(const std::string& text, const vec2i& position)
{
    glPushMatrix();
    glLoadIdentity();
    font.Print((int)position.x, (int)position.y, text.c_str());
    glPopMatrix();
}

}  // namespace GameEngine
