#pragma once
#include "../../../../Shaders/ShaderProgram.h"
#include "glm/gtc/matrix_transform.hpp"
#include "../../../../Debug_/Log.h"

class CTerrainShader : public CShaderProgram
{
public:
	enum UniformLocation
	{
		TransformMatrix,
		ProjectionMatrix,
		ViewMatrix,
		Viewport,
		ScreenSize,
		LodFactor,
		HeightFactor,
		ViewDistance,
		BlendMap,
		BackgroundTexture,
		rTexture,
		bTexture,
		gTexture,
		RockTexture,
		BackgroundNormalTexture,
		rNormalTexture,
		bNormalTexture,
		gNormalTexture,
		RockNormalTexture,
		DisplacementMap,
		Count
	};


	CTerrainShader();
	virtual void Init();
	void GetAllUniformLocations();
	void BindAttributes() override;
	void ConnectTextureUnits() const override;

	template<class T>
	void Load(UniformLocation location, T value)
	{
		if(uniformLocations[location] < 0)
			Error("CTerrainShader : Try load to shader not set variable : " + std::to_string(location));
		LoadValue(uniformLocations[location], value);
	}
protected:
	int uniformLocations[UniformLocation::Count];
	
};
