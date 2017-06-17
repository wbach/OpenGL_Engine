#pragma once
#include <vector>
#include <GL/glew.h>
#include "Types.h"

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
	const GLuint& GetTexture(const uint32& id);
	void BindTextures(int offset = 0);
	void BindToDraw();
	void Bind();
	void UnBind();
	void UnBindDraw();
	~CFrameBuffer();

protected:
    bool isInitialized = false;
    GLuint fbo;
    GLuint depthTexture;
    std::vector<GLuint> textures;
    bool depthStorage = false;
};
