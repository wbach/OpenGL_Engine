#version 110
uniform sampler2D texture;
uniform vec3 color;

void main()
{
    gl_FragColor = texture2D(texture, gl_TexCoord[0].xy) * vec4(color, 1.0);
}