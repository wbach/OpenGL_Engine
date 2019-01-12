#pragma once
#include <GL/glew.h>
#include <vector>
#include "../Types.h"
#include "GameEngine/Resources/VboTypes.h"

namespace Utils
{
struct Vao
{
    uint32 vao  = 0;
    uint32 size = 0;
    GameEngine::VboMap vbos;
    GameEngine::VboMap attributes;
};

class VaoCreator
{
public:
    VaoCreator();
    ~VaoCreator();
    void AddIndicesBuffer(const std::vector<uint16>& indices);
    void AddStaticAttribute(GameEngine::VertexBufferObjects type, uint32 coordinateSize, const std::vector<float>& data);
    void AddStaticAttribute(GameEngine::VertexBufferObjects type, uint32 coordinateSize, const std::vector<int32>& data);
    void AllocateDynamicAttribute(GameEngine::VertexBufferObjects type, uint32 coordinateSize, uint32 dataLength);
    const Vao& Get();

private:
    void CreateVao();
    void AllocateDynamicArray(uint32 attributeNumber, uint32 dataSize, uint32 dataLength, uint32 offset);

private:
    Vao vao_;
    uint32 attributeNumber_;
};

static const GLuint s_GLuint_zero = 0;

void BindFrameBuffer(const int& frameBuffer, const int& width, const int& height);

int CreateFrameBuffer();

int CreateDepthBufferAttachment(const int& width, const int& height);

int CreateVao();

GLuint BindIndicesBuffer(const std::vector<uint16>& indices);

GLuint StoreDataInAttributesList(const int& attributeNumber, const int& coordinateSize, const std::vector<float>& data);
GLuint StoreDataInAttributesList(const int& attributeNumber, const int& coordinateSize, const std::vector<int32>& data);
GLuint StoreDataInAttributesListInstanced(uint32 attributeNumber, uint32 dataSize, uint32 dataLength, uint32 offset);
// void EnableVao(uint32 vao, const std::vector<uint32>& attributes);
// void DisableVao(const std::vector<uint32>& attributes);
void UnbindVao();

void EnableCulling();

void DisableCulling();

void SimpleRenderVao(const GLuint& vao, int indices, int attributes = 1, GLenum mode = GL_TRIANGLES);

void CreateQuad(GLuint& vao, GLuint& vbo_indices, GLuint& vbo_vertex, GLuint& vbo_text_coord, int& indices_size);
void CreateQuadTS05(GLuint& vao, GLuint& vbo_indices, GLuint& vbo_vertex, GLuint& vbo_text_coord, int& indices_size);

void DeleteQuad(GLuint& vao, GLuint& vbo_indices, GLuint& vbo_vertex, GLuint& vbo_text_coord);

void ActiveBindTexture(int i, int id);
}
