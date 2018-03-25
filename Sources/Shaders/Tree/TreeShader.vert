#version 330 core
const int MAX_BONES = 100;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;

out vec4 position;
out vec2 textureCoords;
out vec3 normal;

uniform sampler2D PositionMap;
uniform uint PositionMapSize;
uniform vec3 CameraPosition;

uniform mat4 NormalizationMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

void main(void)
{
	int s = int(PositionMapSize);
	int x = gl_InstanceID % 20 - 10;
    int y = gl_InstanceID / 20 - 10;

	float d = 1.f / PositionMapSize;
	vec2 posCoord = CameraPosition.xz * d / 10.f;
	posCoord =  posCoord + vec2(x, y) * d ;//+ vec2(0.5);

	if (posCoord.x > 1.f || posCoord.x < 0.f || posCoord.y > 1.f || posCoord.y < 0.f)
		return;

	vec4 treePosition = texture(PositionMap, posCoord);
	if (treePosition.a < 0.5f)
		return;

	mat4 translationMat = mat4(1.f);
	translationMat[3].xyz = treePosition.rgb;

	mat4 transform = translationMat * NormalizationMatrix;
    position =  ProjectionMatrix * ViewMatrix * transform  * vec4(Position, 1.0);
    textureCoords = TexCoord;
    normal = (transform * vec4(Normal, 0.f)).xyz;

	gl_Position = position;
}