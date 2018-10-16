#pragma once
#include "GameEngine/Shaders/ShaderProgram.h"
#include "GameEngine/Resources/Models/Material.h"
#include "Logger/Log.h"
#include <unordered_map>

namespace GameEngine
{
	class TreeShader : public CShaderProgram
	{
	public:
		enum UniformLocation
		{
			ViewMatrix,
			NormalizationMatrix,
			ProjectionMatrix,
			UseShading,
			ModelMaterial_Ambient,
			ModelMaterial_Diffuse,
			ModelMaterial_Specular,
			ModelMaterial_ShineDumper,
			DiffuseTexture,
			CameraPosition,
			PositionMap,
			PositionMapSize
		};

		TreeShader(IGraphicsApiPtr graphicsApi);
		template <class T>
		void Load(UniformLocation location, const T& value) const
		{
			if (uniformLocations.count(location) == 0)
			{
				Log("CTerrainShader : Try load to shader not set variable : " + std::to_string(location));
				return;
			}
			LoadValue(uniformLocations.at(location), value);
		}

	private:
		void GetAllUniformLocations();
		void BindAttributes() override;
		void ConnectTextureUnits() const override;

	protected:
		std::unordered_map<UniformLocation, uint32> uniformLocations;
	};
} // GameEngine
