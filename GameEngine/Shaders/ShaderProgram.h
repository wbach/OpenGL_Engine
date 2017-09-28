#pragma once
//#define _CRT_SECURE_NO_WARNINGS
#include "Utils.h"
#include "Types.h"
#include <GL/glew.h>
#include <list>
#include <unordered_map>

const int MAX_BONES = 100;

typedef std::unordered_map<std::string, GLenum> ShadersFiles;

class CShaderProgram
{
public:

	virtual ~CShaderProgram();
	
	bool Init();
	void Reload();
	void SetFiles(const ShadersFiles&);
	
	bool IsReady() const;
	bool IsReadyToLoad() const;
	
	void Start() const;
	void Stop() const;
	
	int GetUniformLocation(const std::string& uniformName) const;

protected:
	void Clear();
	void BindAttribute(int attribute, const std::string& variableName) const;
	bool CreateProgram();
	bool AddShader(const std::string& filename, GLenum mode);
	bool FinalizeShader();
	virtual void GetAllUniformLocations() {}
	virtual void BindAttributes() {}
	virtual void ConnectTextureUnits() const {}
	void LoadValue(uint32 loacation, const int& value) const;
	void LoadValue(uint32 loacation, const float& value) const;
	void LoadValue(uint32 loacation, const vec2& value) const;
	void LoadValue(uint32 loacation, const vec3& value) const;
	void LoadValue(uint32 loacation, const vec4& value) const;
	void LoadValue(uint32 loacation, const mat3& value) const;
	void LoadValue(uint32 loacation, const mat4& value) const;

private:
    int programID = 0;
	ShadersFiles shaderFiles;
    std::list<GLuint> shaderObjectsList;
	std::string name;
};
