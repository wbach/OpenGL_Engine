#pragma once
#include "../Types.h"
#include "VaoEnableController.hpp"
#include <vector>
#include <GL/glew.h>

namespace Utils
{
static const GLuint s_GLuint_zero = 0;

void BindFrameBuffer(const int& frameBuffer, const int& width, const int& height);

int CreateFrameBuffer();

int CreateDepthBufferAttachment(const int& width, const int& height);

int CreateVao();

GLuint BindIndicesBuffer(const std::vector<uint16>& indices);

GLuint StoreDataInAttributesList(const int& attributeNumber, const int& coordinateSize, const std::vector<float>& data);
GLuint StoreDataInAttributesList(const int& attributeNumber, const int& coordinateSize, const std::vector<int32>& data);
//void EnableVao(uint32 vao, const std::vector<uint32>& attributes);
//void DisableVao(const std::vector<uint32>& attributes);
void UnbindVao();

void EnableCulling();

void DisableCulling();

void SimpleRenderVao(const GLuint& vao, int indices, int attributes = 1, GLenum mode = GL_TRIANGLES);

void CreateQuad(GLuint& vao, GLuint& vbo_indices, GLuint& vbo_vertex, GLuint& vbo_text_coord, int& indices_size);

void DeleteQuad(GLuint& vao, GLuint& vbo_indices, GLuint& vbo_vertex, GLuint& vbo_text_coord);

void ActiveBindTexture(int i, int id);
}
