#version 400 core

in vec2 TexCoord;

out vec4 OutColor;

const int MAX_KERNEL_SIZE = 128;
uniform vec3 Kernel[MAX_KERNEL_SIZE];
uniform sampler2D PositionTexture;
uniform float SampleRadius;
uniform mat4 ProjectionMatrix;

void main(void)
{    
	vec4 pos = texture(PositionTexture, TexCoord);

	float AO = 0.0;

    for (int i = 0 ; i < MAX_KERNEL_SIZE ; i++)
    {
        vec3 samplePos = pos.xyz + Kernel[i]; // generate a random point
        vec4 offset = vec4(samplePos, 1.0); // make it a 4-vector
        offset = ProjectionMatrix * offset; // project on the near clipping plane
        offset.xy /= offset.w; // perform perspective divide
        offset.xy = offset.xy * 0.5 + vec2(0.5); // transform to (0,1) range

        float sampleDepth = texture(PositionTexture, offset.xy).b;

        if (abs(pos.z - sampleDepth) < SampleRadius)
        {
            AO += step(sampleDepth,samplePos.z);
        }
    }

    AO = 1.0 - AO / 128.0;

    OutColor = vec4(pow(AO, 2.0));
}