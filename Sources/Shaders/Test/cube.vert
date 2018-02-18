#version 420 core

const vec4 vertices[] = vec4[](
	vec4( 0.5f, 0.5f,-0.5f, 1.0f),          // Top Right Of The Quad (Top)
	vec4(-0.5f, 0.5f,-0.5f, 1.0f),          // Top Left Of The Quad (Top)
	vec4(-0.5f, 0.5f, 0.5f, 1.0f),          // Bottom Left Of The Quad (Top)
	vec4( 0.5f, 0.5f, 0.5f, 1.0f),          // Bottom Right Of The Quad (Top)

	vec4( 0.5f,-0.5f, 0.5f, 1.0f),          // Top Right Of The Quad (Bottom)
	vec4(-0.5f,-0.5f, 0.5f, 1.0f),          // Top Left Of The Quad (Bottom)
	vec4(-0.5f,-0.5f,-0.5f, 1.0f),          // Bottom Left Of The Quad (Bottom)
	vec4( 0.5f,-0.5f,-0.5f, 1.0f),          // Bottom Right Of The Quad (Bottom)

	vec4( 0.5f, 0.5f, 0.5f, 1.0f),          // Top Right Of The Quad (Front)
	vec4(-0.5f, 0.5f, 0.5f, 1.0f),          // Top Left Of The Quad (Front)
	vec4(-0.5f,-0.5f, 0.5f, 1.0f),          // Bottom Left Of The Quad (Front)
	vec4( 0.5f,-0.5f, 0.5f, 1.0f),          // Bottom Right Of The Quad (Front)

	vec4( 0.5f,-0.5f,-0.5f, 1.0f),          // Bottom Left Of The Quad (Back)
	vec4(-0.5f,-0.5f,-0.5f, 1.0f),          // Bottom Right Of The Quad (Back)
	vec4(-0.5f, 0.5f,-0.5f, 1.0f),          // Top Right Of The Quad (Back)
	vec4( 0.5f, 0.5f,-0.5f, 1.0f),          // Top Left Of The Quad (Back)

	vec4(-0.5f, 0.5f, 0.5f, 1.0f),          // Top Right Of The Quad (Left)
	vec4(-0.5f, 0.5f,-0.5f, 1.0f),          // Top Left Of The Quad (Left)
	vec4(-0.5f,-0.5f,-0.5f, 1.0f),          // Bottom Left Of The Quad (Left)
	vec4(-0.5f,-0.5f, 0.5f, 1.0f),          // Bottom Right Of The Quad (Left

	vec4( 0.5f, 0.5f,-0.5f, 1.0f),          // Top Right Of The Quad (Right)
	vec4( 0.5f, 0.5f, 0.5f, 1.0f),          // Top Left Of The Quad (Right)
	vec4( 0.5f,-0.5f, 0.5f, 1.0f),          // Bottom Left Of The Quad (Right)
	vec4( 0.5f,-0.5f,-0.5f, 1.0f)           // Bottom Right Of The Quad (Right)
);

void main(void)
{
    gl_Position = vertices[gl_VertexID];
}