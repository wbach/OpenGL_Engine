#pragma once
#include <GL/glew.h>
#include <vector>
#include "../Types.h"

namespace Utils
{
	static const GLuint s_GLuint_zero = 0;

	void BindFrameBuffer(const int & frameBuffer, const int & width, const int & height);

	int CreateFrameBuffer();

	int CreateDepthBufferAttachment(const int & width, const int & height);

	int CreateVao();

	GLuint BindIndicesBuffer(const std::vector<unsigned short>& indices);

	GLuint StoreDataInAttributesList(const int & attributeNumber, const int & coordinateSize, const std::vector<float>& data);
	void EnableVao(uint vao, const std::vector<uint>& attributes);
	void DisableVao(const std::vector<uint>& attributes);
	void UnbindVao();

	void EnableCulling();

	void DisableCulling();

	void SimpleRenderVao(const GLuint & vao, int indices, int attributes = 1, GLenum mode = GL_TRIANGLES);

	void CreateQuad(GLuint & vao, GLuint & vbo_indices, GLuint & vbo_vertex, GLuint & vbo_text_coord, int & indices_size);

	void DeleteQuad(GLuint & vao, GLuint & vbo_indices, GLuint & vbo_vertex, GLuint & vbo_text_coord);

}