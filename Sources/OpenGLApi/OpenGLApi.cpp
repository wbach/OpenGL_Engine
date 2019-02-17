#include "OpenGLApi.h"
#include <GL/glew.h>
#include <GLM/gtc/type_ptr.hpp>
#include <iostream>
#include <optional>
#include "Font.h"
#include "GameEngine/Engine/Configuration.h"
#include "GraphicsApi/MeshRawData.h"
#include "IdPool.h"
#include "Logger/Log.h"
#include "OpenGLUtils.h"
#include "SDL2/SDLOpenGL.h"

enum class ObjectType
{
    TEXTURE_2D,
    TEXTURE_CUBE_MAP,
    FRAME_BUFFER,
    RENDER_BUFFER,
    SHADER_PROGRAM,
    MESH
};

std::unordered_map<uint32, ObjectType> createdObjectIds;

namespace OpenGLApi
{
CFont font;

struct ShaderBuffer
{
    uint32 glId;
    bool isInGpu;
    uint32 bufferSize;
    uint32 bindLocation;
};

struct OpenGLApi::Pimpl
{
    Pimpl()
        : shaderManager_(idPool_)
    {
    }
    IdPool idPool_;
    ShaderManager shaderManager_;
    std::vector<ShaderBuffer> shaderBuffers_;
};

OpenGLMesh Convert(const Vao& v)
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
OpenGLApi::OpenGLApi(GraphicsApi::IWindowApiPtr windowApi)
    : activeBuffer_(0)
    , windowApi_(windowApi)
    , usedShader(0)
    , bgColor_(0)
    , quad_()
    , quadTs_(true)
    , polygonMode_(true)
    , useLowGLversion_(false)
{
    impl_ = std::make_unique<Pimpl>();

    textureFilterMap_ = {{GraphicsApi::TextureFilter::LINEAR, GL_LINEAR},
                         {GraphicsApi::TextureFilter::NEAREST, GL_NEAREST}};

    bufferAtachmentMap_ = {{GraphicsApi::BufferAtachment::COLOR_1, GL_COLOR_ATTACHMENT0},
                           {GraphicsApi::BufferAtachment::COLOR_2, GL_COLOR_ATTACHMENT1},
                           {GraphicsApi::BufferAtachment::COLOR_3, GL_COLOR_ATTACHMENT2},
                           {GraphicsApi::BufferAtachment::COLOR_4, GL_COLOR_ATTACHMENT3},
                           {GraphicsApi::BufferAtachment::DEPTH, GL_DEPTH_ATTACHMENT}};

    bufferTypeMap_ = {{GraphicsApi::BufferType::COLOR, GL_COLOR_BUFFER_BIT},
                      {GraphicsApi::BufferType::DEPTH, GL_DEPTH_BUFFER_BIT}};

    for (auto& b : bindedShaderBuffers_)
    {
        b = 0;
    }
}

OpenGLApi::~OpenGLApi()
{
    for (auto& buffer : impl_->shaderBuffers_)
    {
        if (buffer.isInGpu)
            glDeleteBuffers(1, &buffer.glId);
    }
}

void GetInfoAndPrint(const std::string& str, int i)
{
    int info;
    glGetIntegerv(i, &info);

    Log(str + " : " + std::to_string(info));
}

void OpenGLApi::Init()
{
    GLint glew_init_result = glewInit();

    if (glew_init_result != GLEW_OK)
    {
        std::string err(reinterpret_cast<char const*>(glewGetErrorString(glew_init_result)));
        Log("[Error] Glew init error : " + err);
        return;
    }

    quad_.Init();
    quadTs_.Init();

    glGetIntegerv(GL_MAJOR_VERSION, &openGLVersion_.x);
    glGetIntegerv(GL_MINOR_VERSION, &openGLVersion_.y);

    if (openGLVersion_.x < 4)
    {
        useLowGLversion_ = true;
        impl_->shaderManager_.UseDeprectedShaders();
    }
}
void OpenGLApi::SetShadersFilesLocations(const std::string& path)
{
    impl_->shaderManager_.SetShadersFilesLocations(path);
}
void OpenGLApi::SetShaderQuaility(GraphicsApi::ShaderQuaility q)
{
    impl_->shaderManager_.SetShaderQuaility(q);
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

    if (not useLowGLversion_)
    {
        GLint MaxPatchVertices = 0;
        glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
        Log("Max supported patch vertices :" + std::to_string(MaxPatchVertices));
        glPatchParameteri(GL_PATCH_VERTICES, 3);
    }

    GetInfoAndPrint("GL_MAX_GEOMETRY_UNIFORM_BLOCKS", GL_MAX_GEOMETRY_UNIFORM_BLOCKS);
    GetInfoAndPrint("GL_MAX_GEOMETRY_SHADER_INVOCATIONS", GL_MAX_GEOMETRY_SHADER_INVOCATIONS);
    GetInfoAndPrint("GL_MAX_UNIFORM_BUFFER_BINDINGS", GL_MAX_UNIFORM_BUFFER_BINDINGS);
    GetInfoAndPrint("GL_MAX_UNIFORM_BLOCK_SIZE", GL_MAX_UNIFORM_BLOCK_SIZE);
    GetInfoAndPrint("GL_MAX_SHADER_STORAGE_BLOCK_SIZE", GL_MAX_SHADER_STORAGE_BLOCK_SIZE);
}
GraphicsApi::IWindowApiPtr OpenGLApi::GetWindowApi()
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
uint32 OpenGLApi::CreateShader(GraphicsApi::Shaders shaderType, GraphicsApi::GraphicsApiFunctions functions)
{
    auto id = impl_->shaderManager_.Create(shaderType, functions);
    createdObjectIds.insert({id, ObjectType::SHADER_PROGRAM});
    return id;
}
void OpenGLApi::DeleteShader(uint32 id)
{
    impl_->shaderManager_.DeleteShader(id);
}
void OpenGLApi::LoadViewMatrix(const mat4& viewMatrix)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(&viewMatrix[0][0]);
}
void OpenGLApi::LoadProjectionMatrix(const mat4& projectionMatrix)
{
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(&projectionMatrix[0][0]);
}
void OpenGLApi::DrawLine(const vec3& color, const vec3& from, const vec3& to)
{
    glColor3f(color.x, color.y, color.z);
    glBegin(GL_LINES);
    glVertex3f(from.x, from.y, from.z);
    glVertex3f(to.x, to.y, to.z);
    glEnd();
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

void OpenGLApi::UseShader(uint32 id)
{
    impl_->shaderManager_.UseShader(id);
}

uint32 OpenGLApi::GetShaderVariableLocation(uint32 id, const std::string& varname)
{
    return impl_->shaderManager_.GetShaderVariableLocation(id, varname);
}

GraphicsApi::ID OpenGLApi::CreateShaderBuffer(uint32 bindLocation, uint32 size)
{
    uint32 buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    impl_->shaderBuffers_.push_back({buffer, true, size, bindLocation});
    auto id = impl_->shaderBuffers_.size() - 1;
    return id;
}

void OpenGLApi::UpdateShaderBuffer(uint32 id, void* buffer)
{
    const auto& b = impl_->shaderBuffers_[id];

    glBindBuffer(GL_UNIFORM_BUFFER, b.glId);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, b.bufferSize, buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

uint32 OpenGLApi::BindShaderBuffer(uint32 id)
{
    const auto& b = impl_->shaderBuffers_[id];
    uint32 result = bindedShaderBuffers_[b.bindLocation];

    bindedShaderBuffers_[b.bindLocation] = id;
    glBindBufferBase(GL_UNIFORM_BUFFER, b.bindLocation, b.glId);

    return result;
}

void OpenGLApi::BindAttribute(uint32 programId, uint32 attribute, const std::string& variableName)
{
    impl_->shaderManager_.BindAttribute(programId, attribute, variableName);
}

void OpenGLApi::LoadValueToShader(uint32 loacation, int value)
{
    impl_->shaderManager_.LoadValueToShader(loacation, value);
}

void OpenGLApi::LoadValueToShader(uint32 loacation, float value)
{
    impl_->shaderManager_.LoadValueToShader(loacation, value);
}

void OpenGLApi::LoadValueToShader(uint32 loacation, uint32 value)
{
    impl_->shaderManager_.LoadValueToShader(loacation, value);
}

void OpenGLApi::LoadValueToShader(uint32 loacation, const vec2& value)
{
    impl_->shaderManager_.LoadValueToShader(loacation, value);
}

void OpenGLApi::LoadValueToShader(uint32 loacation, const vec3& value)
{
    impl_->shaderManager_.LoadValueToShader(loacation, value);
}

void OpenGLApi::LoadValueToShader(uint32 loacation, const vec4& value)
{
    impl_->shaderManager_.LoadValueToShader(loacation, value);
}

void OpenGLApi::LoadValueToShader(uint32 loacation, const mat3& value)
{
    impl_->shaderManager_.LoadValueToShader(loacation, value);
}

void OpenGLApi::LoadValueToShader(uint32 loacation, const mat4& value)
{
    impl_->shaderManager_.LoadValueToShader(loacation, value);
}

void OpenGLApi::LoadValueToShader(uint32 loacation, const std::vector<float>& v)
{
    impl_->shaderManager_.LoadValueToShader(loacation, v);
}

void OpenGLApi::LoadValueToShader(uint32 loacation, const std::vector<vec3>& v)
{
    impl_->shaderManager_.LoadValueToShader(loacation, v);
}

void OpenGLApi::LoadValueToShader(uint32 loacation, const std::vector<mat4>& v)
{
    impl_->shaderManager_.LoadValueToShader(loacation, v);
}

uint32 OpenGLApi::CreateTexture(GraphicsApi::TextureType type, GraphicsApi::TextureFilter filter,
                                GraphicsApi::TextureMipmap mimpamp, GraphicsApi::BufferAtachment atachment, vec2ui size,
                                void* data)
{
    uint32 textureType   = GL_TEXTURE_2D;
    GLint dataType       = GL_FLOAT;
    GLint internalFormat = GL_RGBA;
    GLint format         = GL_RGBA;

    if (type == GraphicsApi::TextureType::FLOAT_BUFFER_2D || type == GraphicsApi::TextureType::FLOAT_TEXTURE_2D ||
        type == GraphicsApi::TextureType::DEPTH_BUFFER_2D)
    {
        dataType = GL_FLOAT;
        if (type == GraphicsApi::TextureType::DEPTH_BUFFER_2D)
        {
            internalFormat = GL_DEPTH_COMPONENT32;
            format         = GL_DEPTH_COMPONENT;
        }
        else
        {
            internalFormat = GL_RGBA16F;
        }
    }
    else if (type == GraphicsApi::TextureType::FLOAT_TEXTURE_1C)
    {
        format         = GL_RED;
        dataType       = GL_FLOAT;
        internalFormat = GL_R32F;
    }
    else if (type == GraphicsApi::TextureType::U8_RGBA)
    {
        format         = GL_RGBA;
        dataType       = GL_UNSIGNED_BYTE;
        internalFormat = GL_RGBA;
    }
    else if (type == GraphicsApi::TextureType::FLOAT_TEXTURE_3C)
    {
        format         = GL_RGB;
        dataType       = GL_FLOAT;
        internalFormat = GL_RGB32F;
    }
    else if (type == GraphicsApi::TextureType::FLOAT_TEXTURE_4C)
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

    if (type == GraphicsApi::TextureType::FLOAT_BUFFER_2D || type == GraphicsApi::TextureType::DEPTH_BUFFER_2D)
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, bufferAtachmentMap_[atachment], textureType, texture, 0);

    if (mimpamp == GraphicsApi::TextureMipmap::LINEAR)
    {
        glGenerateMipmap(textureType);
        glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(textureType, GL_TEXTURE_LOD_BIAS, 0);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(textureType, 0);

    auto rid = impl_->idPool_.ToUint(texture);
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

    auto rid              = impl_->idPool_.ToUint(id);
    createdObjectIds[rid] = ObjectType::TEXTURE_CUBE_MAP;
    return rid;
}

void OpenGLApi::SetBuffers(const std::vector<GraphicsApi::BufferAtachment>& attachemnts)
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

void OpenGLApi::ClearBuffer(GraphicsApi::BufferType type)
{
    glClear(bufferTypeMap_.at(type));
}

void OpenGLApi::ClearBuffers(const std::vector<GraphicsApi::BufferType>& buffers)
{
    if (buffers.empty())
        return;

    GLbitfield& mask = bufferTypeMap_[buffers.front()];

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
    if (id == 0)
    {
        return;
    }

    auto openGLId = impl_->idPool_.ToGL(id);

    glActiveTexture(GL_TEXTURE0 + nr);

    if (createdObjectIds.at(id) == ObjectType::TEXTURE_CUBE_MAP)
        glBindTexture(GL_TEXTURE_CUBE_MAP, openGLId);
    else
        glBindTexture(GL_TEXTURE_2D, openGLId);
}

uint32 OpenGLApi::CreateBuffer()
{
    GLuint glId;
    glGenFramebuffers(1, &glId);
    auto id = impl_->idPool_.ToUint(glId);
    createdObjectIds.insert({id, ObjectType::FRAME_BUFFER});
    return id;
}

void OpenGLApi::BindBuffer(GraphicsApi::BindType type, uint32 id)
{
    if (activeBuffer_ == id)
        return;

    activeBuffer_ = id;
    auto openGLId = impl_->idPool_.ToGL(id);

    switch (type)
    {
        case GraphicsApi::BindType::DEFAULT:
            glBindFramebuffer(GL_FRAMEBUFFER, openGLId);
            break;
        case GraphicsApi::BindType::DRAW:
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, openGLId);
            break;
    }
}

void OpenGLApi::DeleteObject(uint32 id)
{
    if (id == 0)
    {
        return;
    }

    if (createdObjectIds.count(id) == 0)
    {
        Error("Delete object error. Object with id : " + std::to_string(id) + " not created?");
        return;
    }

    auto openGLId = impl_->idPool_.ToGL(id);

    switch (createdObjectIds.at(id))
    {
        case ObjectType::SHADER_PROGRAM:
            DeleteShader(id);
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

void OpenGLApi::DeleteShaderBuffer(uint32 id)
{
    auto& bufferId   = impl_->shaderBuffers_[id];
    bufferId.isInGpu = false;
    glDeleteBuffers(1, &bufferId.glId);
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

    auto rid = impl_->idPool_.ToUint(vao);
    createdObjectIds.insert({rid, ObjectType::MESH});
    openGlMeshes_.insert({rid, {}});
    openGlMeshes_.at(rid).instancesCount = count;
    openGlMeshes_.at(rid).patches        = patch;
    return rid;
}

uint32 OpenGLApi::CreateMesh(const GraphicsApi::MeshRawData& meshRawData)
{
    auto rid = impl_->idPool_.ToUint(0);
    createdObjectIds.insert({rid, ObjectType::MESH});

    openGlMeshes_.insert({rid, {}});
    auto& mesh = openGlMeshes_.at(rid);

    VaoCreator vaoCreator;
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
    auto rid = impl_->idPool_.ToUint(0);
    createdObjectIds.insert({rid, ObjectType::MESH});

    openGlMeshes_.insert({rid, {}});
    auto& mesh = openGlMeshes_.at(rid);

    FloatAttributeVec vertex      = {-1, 1, 0, -1, -1, 0, 1, -1, 0, 1, 1, 0};
    FloatAttributeVec text_coords = {0, 0, 0, 1, 1, 1, 1, 0};
    IndicesVector indices         = {0, 1, 3, 3, 1, 2};

    VaoCreator vaoCreator;
    vaoCreator.AddIndicesBuffer(indices);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::POSITION, 3, vertex);
    vaoCreator.AddStaticAttribute(VertexBufferObjects::TEXT_COORD, 2, text_coords);
    vaoCreator.AllocateDynamicAttribute(VertexBufferObjects::TRANSFORM_MATRIX, 4, sizeof(mat4));
    mesh = Convert(vaoCreator.Get());
    return rid;
}
uint32 OpenGLApi::CreateAnimatedParticle()
{
    auto rid = impl_->idPool_.ToUint(0);
    createdObjectIds.insert({rid, ObjectType::MESH});

    openGlMeshes_.insert({rid, {}});
    auto& mesh = openGlMeshes_.at(rid);

    FloatAttributeVec vertex      = {-1, 1, 0, -1, -1, 0, 1, -1, 0, 1, 1, 0};
    FloatAttributeVec text_coords = {0, 0, 0, 1, 1, 1, 1, 0};
    IndicesVector indices         = {0, 1, 3, 3, 1, 2};

    VaoCreator vaoCreator;
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
    glDrawElements(GL_TRIANGLES, mesh.vertexCount, GL_UNSIGNED_INT, 0);
}

void OpenGLApi::RenderTriangleStripMesh(uint32 id)
{
    if (openGlMeshes_.count(id) == 0)
        return;

    auto& mesh = openGlMeshes_.at(id);

    glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLE_STRIP, mesh.vertexCount, GL_UNSIGNED_INT, 0);
}

void OpenGLApi::RenderMeshInstanced(uint32 id, uint32 istanced)
{
    if (openGlMeshes_.count(id) == 0)
        return;

    auto& mesh = openGlMeshes_.at(id);

    glBindVertexArray(mesh.vao);
    glDrawElementsInstanced(GL_TRIANGLES, mesh.vertexCount, GL_UNSIGNED_INT, 0, istanced);
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
    glBindTexture(GL_TEXTURE_2D, impl_->idPool_.ToGL(id));
}

uint32 OpenGLApi::CreateShadowMap(uint32 sizex, uint32 sizey)
{
    auto glId = CreateDepthBufferAttachment(sizex, sizey);
    auto rid  = impl_->idPool_.ToUint(glId);
    createdObjectIds.insert({rid, ObjectType::TEXTURE_2D});
    return rid;
}

void OpenGLApi::PolygonModeRender()
{
    if (polygonMode_)
        return;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    polygonMode_ = true;
}

void OpenGLApi::LineModeRender()
{
    if (not polygonMode_)
        return;

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    polygonMode_ = false;
}

void OpenGLApi::SetBlendFunction(GraphicsApi::BlendFunctionType type)
{
    switch (type)
    {
        case GraphicsApi::BlendFunctionType::ALPHA_ONE_MINUS_ALPHA:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case GraphicsApi::BlendFunctionType::ONE:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;
        case GraphicsApi::BlendFunctionType::ZERO:
            glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
            break;
        case GraphicsApi::BlendFunctionType::SRC_COLOR:
            glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);
            break;
        case GraphicsApi::BlendFunctionType::ONE_MINUS_SRC_COLOR:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
            break;
        case GraphicsApi::BlendFunctionType::DST_COLOR:
            glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);
            break;
        case GraphicsApi::BlendFunctionType::SRC_ALPHA:
            glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
            break;
        case GraphicsApi::BlendFunctionType::ONE_MINUS_DST_COLOR:
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

}  // namespace OpenGLApi
