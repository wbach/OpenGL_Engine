#version 440 core
const vec3 luma = vec3(0.299, 0.587, 0.114);
// http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf
uniform sampler2D FilterTexture;

layout (std140, align=16, binding=6) uniform Fxaa
{
    vec4 fxaaVariables;
    vec2 invertedScreenSize;
} fxaaInput;

in vec2 textureCoords;
out vec4 FragColor;

void main()
{
    // FragColor = texture(FilterTexture, textureCoords);
    // return;
    float FxaaSpanMax   = fxaaInput.fxaaVariables.x;
    float FxaaReduceMin = fxaaInput.fxaaVariables.y;
    float FxaaReduceMul = fxaaInput.fxaaVariables.z;
    vec2 texCoordOffset = fxaaInput.invertedScreenSize;

    float lumaTL = dot(luma, texture(FilterTexture, textureCoords + (vec2(-1.0, -1.0) * texCoordOffset)).xyz);
    float lumaTR = dot(luma, texture(FilterTexture, textureCoords + (vec2(1.0, -1.0) * texCoordOffset)).xyz);
    float lumaBL = dot(luma, texture(FilterTexture, textureCoords + (vec2(-1.0, 1.0) * texCoordOffset)).xyz);
    float lumaBR = dot(luma, texture(FilterTexture, textureCoords + (vec2(1.0, 1.0) * texCoordOffset)).xyz);
    float lumaM  = dot(luma, texture(FilterTexture, textureCoords).xyz);

    vec2 dir;
    dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
    dir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));

    float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (FxaaReduceMul * 0.25), FxaaReduceMin);
    float inverseDirAdjustment = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir = min(vec2(FxaaSpanMax, FxaaSpanMax),
        max(vec2(-FxaaSpanMax, -FxaaSpanMax), dir * inverseDirAdjustment)) * texCoordOffset;

    vec3 result1 = (1.0/2.0) * (
        texture(FilterTexture, textureCoords.xy + (dir * vec2(1.0/3.0 - 0.5))).xyz +
        texture(FilterTexture, textureCoords.xy + (dir * vec2(2.0/3.0 - 0.5))).xyz);

    vec3 result2 = result1 * (1.0/2.0) + (1.0/4.0) * (
        texture(FilterTexture, textureCoords.xy + (dir * vec2(0.0/3.0 - 0.5))).xyz +
        texture(FilterTexture, textureCoords.xy + (dir * vec2(3.0/3.0 - 0.5))).xyz);

    float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
    float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
    float lumaResult2 = dot(luma, result2);

    if(lumaResult2 < lumaMin || lumaResult2 > lumaMax)
        FragColor = vec4(result1, 1.0);
    else
        FragColor = vec4(result2, 1.0);
}