#pragma once
#include "EntityShader.h"
#include "../../../Resources/Models/Material.h"

class CEntityGeometryPassShader : public CEntityShader
{
public:
	CEntityGeometryPassShader(GameEngine::IGraphicsApiPtr graphicsApi);
	void GetAllUniformLocations() override;
	void ConnectTextureUnits() const override;
	void BindAttributes() override;

	void Init();

	void LoadViewDistance(const float& distance) const;
	void LoadUseFakeLight(const float& use) const;


	void LoadUseNormalMap(const float& use) const;
	void LoadMeshMaterial(const SMaterial& material) const;

	void LoadToShadowSpaceMatrix(const mat4& matrix) const;
	void LoadShadowValues(const float& is, const float& distance, const float& shadow_map_size) const;

	void LoadClipPlane(const vec4 clip_plane = vec4(0, 1, 0, 100000)) const;

	void LoadUseTexture(const float& is) const;

private:
	//Textures locations
	int location_UseTexture;
	int location_ModelTexture;
	int location_UseNormalMap;
	int location_NormalMap;

	//Shadows Variables
	int location_ShadowVariables;
	int location_ShadowMap;
	int location_ToShadowMapSpace;

	//Material Locations
	int location_MaterialAmbient;
	int location_MaterialDiffuse;
	int location_MaterialSpecular;

	int location_ClipPlane;
	int location_ViewDistance;
	int location_UseFakeLighting;
};
