#pragma once
//#define _CRT_SECURE_NO_WARNINGS
#include "Utils.h"
#include "Types.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include <list>
#include <unordered_map>

const int MAX_BONES = 50;

class CShaderProgram
{
public:
	CShaderProgram(GameEngine::IGraphicsApiPtr graphicsApi);
	virtual ~CShaderProgram();
	
	bool Init();
	void Reload();
	void SetFiles(const GameEngine::ShadersFiles&);
	
	bool IsReady() const;
	bool IsReadyToLoad() const;
	
	void Start() const;
	void Stop() const;
	
	uint32 GetUniformLocation(const std::string& uniformName) const;

protected:
	virtual void GetAllUniformLocations() {}
	virtual void BindAttributes() {}
	virtual void ConnectTextureUnits() const {}
	void SetProgramId(uint32);
	void GetAllUniformLocationsFunction(uint32);
	void BindAttributesFunction(uint32);
	void ConnectTextureUnitsFunction(uint32);

	void Clear();
	void BindAttribute(int attribute, const std::string& variableName) const;
	void LoadValue(uint32 loacation, int value) const;
	void LoadValue(uint32 loacation, uint32 value) const;
	void LoadValue(uint32 loacation, float value) const;
	void LoadValue(uint32 loacation, const vec2ui& value) const;
	void LoadValue(uint32 loacation, const vec2& value) const;
	void LoadValue(uint32 loacation, const vec3& value) const;
	void LoadValue(uint32 loacation, const vec4& value) const;
	void LoadValue(uint32 loacation, const mat3& value) const;
	void LoadValue(uint32 loacation, const mat4& value) const;
	void LoadValue(uint32 loacation, const std::vector<float>& value) const;
	void LoadValue(uint32 loacation, const std::vector<vec3>& value) const;

private:
	GameEngine::IGraphicsApiPtr graphicsApi_;
    uint32 programID_;
	std::string name_;
	GameEngine::ShadersFiles shaderFiles_;
    std::list<uint32> shaderObjectsList_;
};
