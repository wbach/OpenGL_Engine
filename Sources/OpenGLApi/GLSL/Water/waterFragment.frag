#version 330
layout (location = 0) out vec3 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 MaterialSpecular;

in vec2 TexCoord0;
in vec3 Normal0;
in vec3 WorldPos0;
in vec4 ClipSpace;
in vec3 toCameraVector;

uniform sampler2D reflectionTexture ;
uniform sampler2D refractionTexture ;
uniform sampler2D normalMap ;
uniform sampler2D depthMap;
uniform sampler2D dudvMap ;
uniform float Quality;

uniform float moveFactor ;
uniform vec4 WaterColor;

const float waveStrength = 0.02 ;
const float shineDamper = 20.0f;
const float reflectivity = 0.6f;

void main(void)
{
        vec2 ndc = (ClipSpace.xy/ClipSpace.w) /2.0 + 0.5;
        vec2 refractTexCoords = vec2(ndc.x,ndc.y);
        vec2 reflectTexCoords = vec2(ndc.x,-ndc.y);

        float tiledValue = 2;

         vec2 distortedTexCoords = texture(dudvMap, vec2(TexCoord0.x *tiledValue + moveFactor, TexCoord0.y *tiledValue)).rg*0.1;
        distortedTexCoords = TexCoord0 *tiledValue + vec2(distortedTexCoords.x, distortedTexCoords.y+moveFactor);
        vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength ;//* edgesFactor;

        vec4 normalMapColour = texture(normalMap, distortedTexCoords) ;
        vec3 normal = vec3(normalMapColour.r*2.0f - 1.0f, normalMapColour.b *1.0, normalMapColour.g*2.0f - 1.0f);
        normal = normalize(normal);

        if (Quality < 0.5f)
        {
                WorldPosOut      = WorldPos0;
                DiffuseOut       = vec4(WaterColor.xyz, 1.f) * normalMapColour * 0.45f;
                NormalOut        = vec4(normal, 1.f);
                MaterialSpecular = vec4(vec3(1.f), 20.f);
                return;
        }

        float near = 0.1 ;
        float far = 1000.0 ;
        float depth = texture(depthMap, refractTexCoords).r ;
        float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

        depth = gl_FragCoord.z ;
        float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
        float waterDepth = floorDistance - waterDistance ;
        float edgesFactor = clamp(waterDepth/2.0f,0.0f,1.0f) ;

        totalDistortion = totalDistortion * edgesFactor;

        reflectTexCoords+=totalDistortion;
        reflectTexCoords.x = clamp(reflectTexCoords.x,0.001, 0.999);
        reflectTexCoords.y = clamp(reflectTexCoords.y,-0.999, -0.001);

        refractTexCoords+=totalDistortion;
        refractTexCoords = clamp(refractTexCoords,0.001, 0.999);

        vec4 reflectColour =  texture(reflectionTexture, reflectTexCoords);

        vec3 viewVactor = normalize(toCameraVector);
        float refractiveFactor = dot(viewVactor, normal);
        refractiveFactor = pow(refractiveFactor, 0.5);
        refractiveFactor = clamp(refractiveFactor,0.0f,1.0f);

        vec4 refractColour =  texture(refractionTexture, refractTexCoords);
        vec4 out_Color = mix(reflectColour, refractColour, refractiveFactor);

        WorldPosOut      = WorldPos0;
        DiffuseOut       = mix(out_Color, vec4(WaterColor.xyz, 1.f), WaterColor.w);
        DiffuseOut.a = edgesFactor ;
        NormalOut        = vec4(normal, 1.f);
        MaterialSpecular = vec4(vec3(1.f), 20.f);
}