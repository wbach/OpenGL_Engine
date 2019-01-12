#include "OpenGLUtils.h"

using namespace GameEngine;

namespace Utils
{
VaoCreator::VaoCreator()
    : attributeNumber_(0)
{
    CreateVao();
}
VaoCreator::~VaoCreator()
{
    glBindVertexArray(0);
}
void VaoCreator::AddIndicesBuffer(const std::vector<uint16>& indices)
{
    if (indices.empty())
    {
        return;
    }

    auto& vbo_id = vao_.vbos[VertexBufferObjects::INDICES];
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16), &indices[0], GL_STATIC_DRAW);
    vao_.size = indices.size();
}

void VaoCreator::AddStaticAttribute(VertexBufferObjects type, uint32 coordinateSize, const std::vector<int32>& data)
{
    if (data.empty())
    {
        return;
    }

    auto& vbo_id = vao_.vbos[type];
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(int32), &data[0], GL_STATIC_DRAW);
    glVertexAttribIPointer(attributeNumber_, coordinateSize, GL_INT, 0, 0);
    glEnableVertexAttribArray(attributeNumber_);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    vao_.attributes[type] = attributeNumber_++;
}

void VaoCreator::AddStaticAttribute(VertexBufferObjects type, uint32 coordinateSize, const std::vector<float>& data)
{
    if (data.empty())
    {
        return;
    }

    auto& vbo_id = vao_.vbos[type];
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
    glVertexAttribPointer(attributeNumber_, coordinateSize, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(attributeNumber_);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    vao_.attributes[type] = attributeNumber_++;

    if (type == VertexBufferObjects::POSITION && vao_.size == 0)
    {
        vao_.size = data.size() / 3;
    }
}

void VaoCreator::AllocateDynamicAttribute(VertexBufferObjects type, uint32 coordinateSize, uint32 dataLength)
{
    auto& vboId = vao_.vbos[type];
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glEnableVertexAttribArray(attributeNumber_);
    vao_.attributes[type] = attributeNumber_;

    if (type == VertexBufferObjects::TRANSFORM_MATRIX)
    {
        AllocateDynamicArray(attributeNumber_++, coordinateSize, dataLength, 0);
        AllocateDynamicArray(attributeNumber_++, coordinateSize, dataLength, 4);
        AllocateDynamicArray(attributeNumber_++, coordinateSize, dataLength, 8);
        AllocateDynamicArray(attributeNumber_++, coordinateSize, dataLength, 12);
    }
    else
    {
        AllocateDynamicArray(attributeNumber_++, coordinateSize, dataLength, 0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

const Vao& VaoCreator::Get()
{
    return vao_;
}

void VaoCreator::CreateVao()
{
    glGenVertexArrays(1, &vao_.vao);
    glBindVertexArray(vao_.vao);
}

void VaoCreator::AllocateDynamicArray(uint32 attributeNumber, uint32 dataSize, uint32 dataLength, uint32 offset)
{
    glEnableVertexAttribArray(attributeNumber);
    glVertexAttribPointer(attributeNumber, dataSize, GL_FLOAT, GL_FALSE, dataLength,
                          (const GLvoid*)(offset * sizeof(GLfloat)));
    glVertexAttribDivisor(attributeNumber, 1);
}

void BindFrameBuffer(const int& frameBuffer, const int& width, const int& height)
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
    glViewport(0, 0, width, height);
}

int CreateFrameBuffer()
{
    GLuint frame_buffer;
    glGenFramebuffers(1, &frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    glDrawBuffer(GL_NONE);
    return frame_buffer;
}

int CreateDepthBufferAttachment(const int& width, const int& height)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);
    // glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
    // Disable writes to the color buffer
    glDrawBuffer(GL_NONE);

    // Disable reads from the color buffer
    glReadBuffer(GL_NONE);
    return texture;
}

int CreateVao()
{
    GLuint vao_id;
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);
    return vao_id;
}

GLuint BindIndicesBuffer(const std::vector<uint16>& indices)
{
    GLuint vbo_id;
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16), &indices[0], GL_STATIC_DRAW);
    return vbo_id;
}

GLuint StoreDataInAttributesList(const int& attributeNumber, const int& coordinateSize, const std::vector<float>& data)
{
    GLuint vbo_id;
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
    glVertexAttribPointer(attributeNumber, coordinateSize, GL_FLOAT, false, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return vbo_id;
}
GLuint StoreDataInAttributesList(const int& attributeNumber, const int& coordinateSize, const std::vector<int32>& data)
{
    GLuint vbo_id;
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(int32), &data[0], GL_STATIC_DRAW);
    glVertexAttribIPointer(attributeNumber, coordinateSize, GL_INT, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return vbo_id;
}
GLuint StoreDataInAttributesListInstanced(uint32 attributeNumber, uint32 dataSize, uint32 dataLength, uint32 offset)
{
    glEnableVertexAttribArray(attributeNumber);
    glVertexAttribPointer(attributeNumber, dataSize, GL_FLOAT, GL_FALSE, dataLength,
                          (const GLvoid*)(offset * sizeof(GLfloat)));
    glVertexAttribDivisor(attributeNumber, 1);
    return 0;
}

// void EnableVao(uint32 vao, const std::vector<uint32>& attributes)
//{
//	glBindVertexArray(vao);
//	for (const auto& i : attributes)
//		glEnableVertexAttribArray(i);
//}

// void DisableVao(const std::vector<uint32>& attributes)
//{
//	for (const auto& i : attributes)
//		glDisableVertexAttribArray(i);
//	glBindVertexArray(0);
//}

void UnbindVao()
{
    glBindVertexArray(0);
}

void EnableCulling()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void DisableCulling()
{
    glDisable(GL_CULL_FACE);
}

void SimpleRenderVao(const GLuint& vao, int indices, int attributes, GLenum mode)
{
    glBindVertexArray(vao);
    for (int x = 0; x < attributes; x++)
        glEnableVertexAttribArray(x);

    glDrawElements(mode, indices, GL_UNSIGNED_SHORT, 0);

    for (int x = attributes; x > 0; x--)
        glDisableVertexAttribArray(x);
    glBindVertexArray(0);
}

void CreateQuad(GLuint& vao, GLuint& vbo_indices, GLuint& vbo_vertex, GLuint& vbo_text_coord, int& indices_size)
{
    // std::vector<float> vertex = { -0.5, 0.5, 0, -0.5, -0.5, 0, 0.5, -0.5, 0, 0.5, 0.5, 0 };
    std::vector<float> vertex      = {-1, 1, 0, -1, -1, 0, 1, -1, 0, 1, 1, 0};
    std::vector<float> text_coords = {0, 0, 0, 1, 1, 1, 1, 0};
    std::vector<uint16> indices    = {0, 1, 3, 3, 1, 2};
    indices_size                   = indices.size();
    vao                            = CreateVao();
    vbo_indices                    = BindIndicesBuffer(indices);
    vbo_vertex                     = StoreDataInAttributesList(0, 3, vertex);
    vbo_text_coord                 = StoreDataInAttributesList(1, 2, text_coords);
    UnbindVao();
}

void CreateQuadTS05(GLuint& vao, GLuint& vbo_indices, GLuint& vbo_vertex, GLuint& vbo_text_coord, int& indices_size)
{
    std::vector<float> vertex = {-0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f};

    std::vector<float> text_coords = {0, 0, 0, 1, 1, 1, 1, 0};
    std::vector<uint16> indices    = {0, 1, 3, 3, 1, 2};
    indices_size                   = indices.size();
    vao                            = CreateVao();
    vbo_indices                    = BindIndicesBuffer(indices);
    vbo_vertex                     = StoreDataInAttributesList(0, 3, vertex);
    vbo_text_coord                 = StoreDataInAttributesList(1, 2, text_coords);
    UnbindVao();
}

void DeleteQuad(GLuint& vao, GLuint& vbo_indices, GLuint& vbo_vertex, GLuint& vbo_text_coord)
{
    glDeleteBuffers(1, &vbo_indices);
    glDeleteBuffers(1, &vbo_vertex);
    glDeleteBuffers(1, &vbo_text_coord);
    glDeleteVertexArrays(1, &vao);
}

void ActiveBindTexture(int i, int id)
{
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, id);
}
}  // Utils
