#include "TreeShader.h"
#include "GameEngine/Objects/RenderAble/Terrain/TerrainTexturesTypes.h"

#define GetLocation(X) uniformLocations[UniformLocation::X] = GetUniformLocation(#X)

namespace GameEngine
{
	TreeShader::TreeShader(IGraphicsApiPtr graphicsApi)
		: CShaderProgram(graphicsApi)
	{
		SetFiles
		({
			{ "Tree/TreeShader.vert", ShaderType::VERTEX_SHADER },
			{ "Tree/TreeShader.frag", ShaderType::FRAGMENT_SHADER },
		});
	}

	void TreeShader::LoadTransforms(const std::vector<mat4> transforms) const
	{
		uint32 i = 0;
		for (const auto& mat : transforms)
			LoadValue(transformsLocations[i++], mat);
	}

	void TreeShader::GetAllUniformLocations()
	{
		GetLocation(ViewMatrix);
		GetLocation(ProjectionMatrix);
		GetLocation(DiffuseTexture);
		GetLocation(NormalizationMatrix);
		GetLocation(UseShading);
		GetLocation(PositionMap);
		GetLocation(PositionMapSize);
		GetLocation(CameraPosition);
		
		uniformLocations[UniformLocation::ModelMaterial_Ambient] = GetUniformLocation("ModelMaterial.ambient");
		uniformLocations[UniformLocation::ModelMaterial_Diffuse] = GetUniformLocation("ModelMaterial.diffuse");
		uniformLocations[UniformLocation::ModelMaterial_Specular] = GetUniformLocation("ModelMaterial.specular");
		uniformLocations[UniformLocation::ModelMaterial_ShineDumper] = GetUniformLocation("ModelMaterial.shineDamper");

		for (int x = 0; x < MAX_TRANSFORMS; x++)
			transformsLocations[x] = GetUniformLocation("Transforms[" + std::to_string(x) + "]");
	}
	void TreeShader::BindAttributes()
	{
		BindAttribute(0, "Position");
		BindAttribute(1, "TexCoord");
		BindAttribute(2, "Normal");
	}

#define ConnectTerrainTexture(X, Y) LoadValue(uniformLocations.at(UniformLocation::X), Y);

	void TreeShader::ConnectTextureUnits() const
	{
		ConnectTerrainTexture(PositionMap, 0);
		ConnectTerrainTexture(DiffuseTexture, 1);
	}
}
