#version 130

in vec2 TexCoordOut;

uniform sampler2D modelTexture;

void main()
{
    vec4 textureColor = texture(modelTexture, TexCoordOut);
    if(textureColor.a < 0.5)
    {
        discard ;
    }
    gl_FragColor = mix(vec4(0.0), textureColor, 1.0f);
}