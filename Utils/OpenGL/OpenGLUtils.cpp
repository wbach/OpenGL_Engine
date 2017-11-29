#include "OpenGLUtils.h"

void Utils::BindFrameBuffer(const int & frameBuffer, const int & width, const int & height)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, width, height);
}

int Utils::CreateFrameBuffer()
{
	GLuint frame_buffer;
	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	glDrawBuffer(GL_NONE);
	return frame_buffer;
}

int Utils::CreateDepthBufferAttachment(const int & width, const int & height) {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
	// Disable writes to the color buffer
	glDrawBuffer(GL_NONE);

	// Disable reads from the color buffer
	glReadBuffer(GL_NONE);
	return texture;
}

int Utils::CreateVao()
{
	GLuint vao_id;
	glGenVertexArrays(1, &vao_id);
	glBindVertexArray(vao_id);
	return vao_id;
}

GLuint Utils::BindIndicesBuffer(const std::vector<uint16>& indices)
{
	GLuint vbo_id;
	glGenBuffers(1, &vbo_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16), &indices[0], GL_STATIC_DRAW);
	return vbo_id;
}

GLuint Utils::StoreDataInAttributesList(const int & attributeNumber, const int & coordinateSize, const std::vector<float>& data)
{
	GLuint vbo_id;
	glGenBuffers(1, &vbo_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
	glVertexAttribPointer(attributeNumber, coordinateSize, GL_FLOAT, false, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return vbo_id;
}

void Utils::EnableVao(uint32 vao, const std::vector<uint32>& attributes)
{
	glBindVertexArray(vao);
	for (const auto& i : attributes)
		glEnableVertexAttribArray(i);
}

void Utils::DisableVao(const std::vector<uint32>& attributes)
{
	for (const auto& i : attributes)
		glDisableVertexAttribArray(i);
	glBindVertexArray(0);
}

void Utils::UnbindVao()
{
	glBindVertexArray(0);
}

void Utils::EnableCulling()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void Utils::DisableCulling()
{
	glDisable(GL_CULL_FACE);
}

void Utils::SimpleRenderVao(const GLuint & vao, int indices, int attributes, GLenum mode)
{
	glBindVertexArray(vao);
	for (int x = 0; x < attributes; x++)
		glEnableVertexAttribArray(x);

	glDrawElements(mode, indices, GL_UNSIGNED_SHORT, 0);

	for (int x = attributes; x > 0; x--)
		glDisableVertexAttribArray(x);
	glBindVertexArray(0);
}

void Utils::CreateQuad(GLuint & vao, GLuint & vbo_indices, GLuint & vbo_vertex, GLuint & vbo_text_coord, int & indices_size)
{
	//std::vector<float> vertex = { -0.5, 0.5, 0, -0.5, -0.5, 0, 0.5, -0.5, 0, 0.5, 0.5, 0 };
	std::vector<float> vertex = {
		-1,  1, 0,
		-1, -1, 0,
		1, -1, 0,
		1,  1, 0 };
	std::vector<float> text_coords = {
		0, 0,
		0, 1,
		1, 1,
		1, 0 };
	std::vector<uint16> indices = { 0, 1, 3, 3, 1, 2 };
	indices_size = indices.size();
	vao = CreateVao();
	vbo_indices = BindIndicesBuffer(indices);
	vbo_vertex = StoreDataInAttributesList(0, 3, vertex);
	vbo_text_coord = StoreDataInAttributesList(1, 2, text_coords);
	UnbindVao();
}

void Utils::DeleteQuad(GLuint & vao, GLuint & vbo_indices, GLuint & vbo_vertex, GLuint & vbo_text_coord)
{
	glDeleteBuffers(1, &vbo_indices);
	glDeleteBuffers(1, &vbo_vertex);
	glDeleteBuffers(1, &vbo_text_coord);
    glDeleteVertexArrays(1, &vao);
}
void Utils::EnableVertexAttribArrays(const std::vector<GLuint> & v)
{
    for(auto i : v)
        glEnableVertexAttribArray(i);
}

void Utils::DisableVertexAttribArrays(const std::vector<GLuint> & v)
{
    for(auto iter = v.end(); iter != v.begin(); iter--)
        glDisableVertexAttribArray(*iter);
}

void Utils::ActiveBindTexture(int i, int id)
{
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, id);
}
