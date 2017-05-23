#pragma once
//#define _CRT_SECURE_NO_WARNINGS
#include "../Utils/Utils.h"
#include <GL/glew.h>
#include "glm/glm.hpp"
#include <list>

const int MAX_BONES = 100;

class CShaderProgram
{
public:
	virtual ~CShaderProgram();

	bool CreateProgram();
	bool AddShader(const std::string& filename, GLenum mode);
	bool FinalizeShader();

	void Start() const;
	void Stop() const;
	void BindAttribute(int attribute, const std::string& variableName) const;
	int GetUniformLocation(const std::string& uniformName) const;

protected:
	virtual void GetAllUniformLocations() {}
	virtual void BindAttributes() {}
	virtual void ConnectTextureUnits() const {}
	void LoadValue(uint loacation, const int& value) const;
	void LoadValue(uint loacation, const float& value) const;
	void LoadValue(uint loacation, const glm::vec2& value) const;
	void LoadValue(uint loacation, const glm::vec3& value) const;
	void LoadValue(uint loacation, const glm::vec4& value) const;
	void LoadValue(uint loacation, const glm::mat3& value) const;
	void LoadValue(uint loacation, const glm::mat4& value) const;

private:
    int programID = 0;
    std::list<GLuint> shaderObjectsList;
	std::string name;
};
