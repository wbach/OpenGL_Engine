#include "ShadowShader.h"

#define GetLocation(X) uniformLocations[UniformLocation::X].push_back(GetUniformLocation(#X))

CShadowShader::CShadowShader(GameEngine::IGraphicsApiPtr graphicsApi)
	: CShaderProgram(graphicsApi)
{
}

void CShadowShader::Init()
{
    SetFiles
    ({
        {"Shadows/ShadowVertexShader.vert", GameEngine::ShaderType::VERTEX_SHADER},
        {"Shadows/ShadowFragmentShader.frag", GameEngine::ShaderType::FRAGMENT_SHADER}
    });

    CShaderProgram::Init();
}

void CShadowShader::GetAllUniformLocations()
{
	GetLocation(TransformationMatrix);
	GetLocation(ProjectionViewMatrix);
	GetLocation(UseBoneTransform);

	for (int x = 0; x < MAX_BONES; x++)
		uniformLocations[UniformLocation::BonesTransforms].push_back(GetUniformLocation("BonesTransforms[" + std::to_string(x) + "]"));

	GetLocation(NumberOfRows);
	GetLocation(TextureOffset);
	GetLocation(ModelTexture);
}

void CShadowShader::BindAttributes()
{
    BindAttribute(0, "Position");
    BindAttribute(1, "TexCoord");
    BindAttribute(4, "Weights");
	BindAttribute(5, "BoneIds");
}
void CShadowShader::ConnectTextureUnits() const
{
	if (uniformLocations.count(ModelTexture) == 0 ||  uniformLocations.at(ModelTexture).empty())
	{
		return;
	}
	LoadValue(uniformLocations.at(UniformLocation::ModelTexture)[0], 0);
}
