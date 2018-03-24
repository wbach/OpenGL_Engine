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

uniform mat4 Transforms[100];
uniform mat4 NormalizationMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

void main(void)
{
	vec2 posCoord = vec2(1.f) / vec2(PositionMapSize);
	float d = 1.f / PositionMapSize;
	vec4 treePosition = texture(PositionMap, vec2( gl_InstanceID * d, 0.f) );
	if (treePosition.a < 0.5f)
		return;

	//mat4 transform = Transforms[gl_InstanceID] * NormalizationMatrix;
	int x = gl_InstanceID & 10;
    int y = gl_InstanceID >> 3;

	vec3 translation = vec3(395 + 5 * x, 0 , 575 + 5 * y);

	mat4 testPos = mat4(1.f);
	testPos[3].xyz = treePosition.rgb * 1000.f;

	mat4 transform = testPos * NormalizationMatrix;
    position =  ProjectionMatrix * ViewMatrix * transform  * vec4(Position, 1.0);
    textureCoords = TexCoord;
    normal = (transform * vec4(Normal, 0.f)).xyz;

	gl_Position = position;
}