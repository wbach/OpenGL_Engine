#pragma once
#include "GameEngine/Shaders/ShaderProgram.h"
#include "Logger/Log.h"
#include <unordered_map>

namespace GameEngine
{
	class ParticlesShader : public CShaderProgram
	{
	public:
		enum UniformLocation
		{

		};
		ParticlesShader(IGraphicsApiPtr graphicsApi);
		template <class T>
		void Load(UniformLocation location, T value) const
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
