#pragma once
#include <vector>
#include <GL/glew.h>
#include "../../Utils/Types.h"

class CFrameBuffer
{
public:
	virtual void Init(const wb::vec2i& size) = 0;
	virtual void Clean() {}
	void CreateFrameBuffer();
	void AddTexture(GLuint& texture);
	void SetDepthTexture(const GLuint& texture);
	int CheckStatus();
	const GLuint& GetFbo();
	const GLuint& GetDepthTexture();
	const GLuint& GetTexture(const uint& id);
	void BindTextures(int offset = 0);
	void BindToDraw();
	void Bind();
	void UnBind();
	void UnBindDraw();
	~CFrameBuffer();
protected:
	bool m_IsInitialized = false;
	GLuint m_Fbo;
	GLuint m_DepthTexture;
	std::vector<GLuint> m_Textures;
	bool m_DepthStorage = false;
};