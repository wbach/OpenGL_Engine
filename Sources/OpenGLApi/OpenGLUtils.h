#pragma once
#include <GL/glew.h>
#include <vector>
#include "Types.h"

namespace OpenGLApi
{
struct Vao
{
    bool useIndicies = false;
    uint32 vao  = 0;
    uint32 size = 0;
    VboMap vbos;
    VboMap attributes;
};

class VaoCreator
{
public:
    VaoCreator();
    ~VaoCreator();
    void SetSize(uint32 size);
    // void AddIndicesBuffer(const std::vector<uint16>& indices);
    void AddIndicesBuffer(const IndicesVector& indices);
    void AddStaticAttributePatch(VertexBufferObjects type, uint32 coordinateSize, const std::vector<float>& data);
    void AddStaticAttribute(VertexBufferObjects type, uint32 coordinateSize, const std::vector<float>& data);
    void AddStaticAttribute(VertexBufferObjects type, uint32 coordinateSize, const std::vector<int32>& data);
    void AllocateDynamicAttribute(VertexBufferObjects type, uint32 coordinateSize, uint32 dataLength);
    const Vao& Get();

private:
    void CreateVao();
    void AllocateDynamicArray(uint32 attributeNumber, uint32 dataSize, uint32 dataLength, uint32 offset);

private:
    Vao vao_;
    uint32 attributeNumber_;
};

static const GLuint s_GLuint_zero = 0;

void BindFrameBuffer(int frameBuffer, int width, int height);
int CreateFrameBuffer();
int CreateDepthBufferAttachment(int width, int height);
int CreateVao();
GLuint BindIndicesBuffer(const std::vector<uint16>& indices);
GLuint StoreDataInAttributesList(int attributeNumber, int coordinateSize, const std::vector<float>& data);
GLuint StoreDataInAttributesList(int attributeNumber, int coordinateSize, const std::vector<int32>& data);
GLuint StoreDataInAttributesListInstanced(uint32 attributeNumber, uint32 dataSize, uint32 dataLength, uint32 offset);
void UnbindVao();
void EnableCulling();
void DisableCulling();
void SimpleRenderVao(const GLuint& vao, int indices, int attributes = 1, GLenum mode = GL_TRIANGLES);
void CreateQuad(GLuint& vao, GLuint& vbo_indices, GLuint& vbo_vertex, GLuint& vbo_text_coord, int& indices_size);
void CreateQuadTS05(GLuint& vao, GLuint& vbo_indices, GLuint& vbo_vertex, GLuint& vbo_text_coord, int& indices_size);
void DeleteQuad(GLuint& vao, GLuint& vbo_indices, GLuint& vbo_vertex, GLuint& vbo_text_coord);
void ActiveBindTexture(int i, int id);
}  // namespace OpenGLApi
