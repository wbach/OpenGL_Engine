#include "TesselationTerrainShader.h"

CTesselationTerrainShader::CTesselationTerrainShader()
{
	for (uint32 x = 0; x < UniformLocation::Count; x++)
		uniformLocations[x] = -1;
}

void CTesselationTerrainShader::Init()
{
	SetFiles
	({
		{"Terrain/TerrainShader.vert", GL_VERTEX_SHADER},
		{"Terrain/TerrainShader.frag", GL_FRAGMENT_SHADER},
		{"Terrain/TerrainShader.cs", GL_TESS_CONTROL_SHADER},
		{"Terrain/TerrainShader.es", GL_TESS_EVALUATION_SHADER}
	});

	CShaderProgram::Init();
}

void CTesselationTerrainShader::GetAllUniformLocations()
{
	uniformLocations[UniformLocation::TransformMatrix] = GetUniformLocation("TransformationMatrix");
	uniformLocations[UniformLocation::ProjectionMatrix] = GetUniformLocation("ProjectionMatrix");
	uniformLocations[UniformLocation::ViewMatrix] = GetUniformLocation("ViewMatrix");


	uniformLocations[UniformLocation::ScreenSize] = GetUniformLocation("CameraPosition");
	uniformLocations[UniformLocation::LodFactor] = GetUniformLocation("LodFactor");
	uniformLocations[UniformLocation::HeightFactor] = GetUniformLocation("heightFactor");
	uniformLocations[UniformLocation::Viewport] = GetUniformLocation("viewport");
	uniformLocations[UniformLocation::DisplacementMap] = GetUniformLocation("gDisplacementMap");
	uniformLocations[UniformLocation::ViewDistance] = GetUniformLocation("ViewDistance");

	//Get location of samplers 2d
	uniformLocations[UniformLocation::BlendMap] = GetUniformLocation("BlendMap");

	uniformLocations[UniformLocation::BackgroundTexture] = GetUniformLocation("BackgroundTexture");
	uniformLocations[UniformLocation::rTexture] = GetUniformLocation("rTexture");
	uniformLocations[UniformLocation::bTexture] = GetUniformLocation("bTexture");
	uniformLocations[UniformLocation::gTexture] = GetUniformLocation("gTexture");

	uniformLocations[UniformLocation::BackgroundNormalTexture] = GetUniformLocation("BackgroundTextureNormal");
	uniformLocations[UniformLocation::rNormalTexture] = GetUniformLocation("rTextureNormal");
	uniformLocations[UniformLocation::bNormalTexture] = GetUniformLocation("bTextureNormal");
	uniformLocations[UniformLocation::gNormalTexture] = GetUniformLocation("gTextureNormal");

	uniformLocations[UniformLocation::RockTexture] = GetUniformLocation("RockTexture");
	uniformLocations[UniformLocation::RockNormalTexture] = GetUniformLocation("RockTextureNormal");
	uniformLocations[UniformLocation::DisplacementMap] = GetUniformLocation("gDisplacementMap");

	uniformLocations[UniformLocation::distanceDev] = GetUniformLocation("distanceDev");
	uniformLocations[UniformLocation::minTessLevelOuter] = GetUniformLocation("minTessLevelOuter");

    uniformLocations[UniformLocation::shadowMap] = GetUniformLocation("shadowMap");

	for (uint32 x = 0; x < UniformLocation::Count; x++)
		if (uniformLocations[x] == -1)
			Log("CTerrainShader::GetAllUniformLocations() : UniformLocation " + std::to_string(x) + " not set!");
}

void CTesselationTerrainShader::BindAttributes()
{
	BindAttribute(0, "Position");
	BindAttribute(1, "TexCoord");
	BindAttribute(2, "Normal");
}

void CTesselationTerrainShader::ConnectTextureUnits() const
{
	LoadValue(uniformLocations[UniformLocation::BackgroundTexture], 0);
	LoadValue(uniformLocations[UniformLocation::rTexture], 1);
	LoadValue(uniformLocations[UniformLocation::gTexture], 2);
	LoadValue(uniformLocations[UniformLocation::bTexture], 3);
	LoadValue(uniformLocations[UniformLocation::BlendMap], 4);
	LoadValue(uniformLocations[UniformLocation::BackgroundNormalTexture], 5);
	LoadValue(uniformLocations[UniformLocation::rNormalTexture], 6);
	LoadValue(uniformLocations[UniformLocation::gNormalTexture], 7);
	LoadValue(uniformLocations[UniformLocation::bNormalTexture], 8);
	LoadValue(uniformLocations[UniformLocation::RockTexture], 9);
	LoadValue(uniformLocations[UniformLocation::RockNormalTexture], 10);
	LoadValue(uniformLocations[UniformLocation::DisplacementMap], 11);
    LoadValue(uniformLocations[UniformLocation::shadowMap], 12);
}
