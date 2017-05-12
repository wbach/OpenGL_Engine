#version 330
// http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf
uniform sampler2D FilterTexture;
uniform vec2	  ScreenSize;

uniform float FxaaSpanMax;
uniform float FxaaReduceMin;
uniform float FxaaReduceMul;

out vec4 FragColor;

vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / ScreenSize;
}

void main()
{

	vec2 texCoord0  = CalcTexCoord();
	vec2 texCoordOffset = vec2(1.f/ScreenSize.x, 1.f/ScreenSize.y); 
	
	vec3 luma = vec3(0.299, 0.587, 0.114);	
	float lumaTL = dot(luma, texture2D(FilterTexture, texCoord0.xy + (vec2(-1.0, -1.0) * texCoordOffset)).xyz);
	float lumaTR = dot(luma, texture2D(FilterTexture, texCoord0.xy + (vec2(1.0, -1.0) * texCoordOffset)).xyz);
	float lumaBL = dot(luma, texture2D(FilterTexture, texCoord0.xy + (vec2(-1.0, 1.0) * texCoordOffset)).xyz);
	float lumaBR = dot(luma, texture2D(FilterTexture, texCoord0.xy + (vec2(1.0, 1.0) * texCoordOffset)).xyz);
	float lumaM  = dot(luma, texture2D(FilterTexture, texCoord0.xy).xyz);

	vec2 dir;
	dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
	dir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));
	
	float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (FxaaReduceMul * 0.25), FxaaReduceMin);
	float inverseDirAdjustment = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
	
	dir = min(vec2(FxaaSpanMax, FxaaSpanMax), 
		max(vec2(-FxaaSpanMax, -FxaaSpanMax), dir * inverseDirAdjustment)) * texCoordOffset;

	vec3 result1 = (1.0/2.0) * (
		texture2D(FilterTexture, texCoord0.xy + (dir * vec2(1.0/3.0 - 0.5))).xyz +
		texture2D(FilterTexture, texCoord0.xy + (dir * vec2(2.0/3.0 - 0.5))).xyz);

	vec3 result2 = result1 * (1.0/2.0) + (1.0/4.0) * (
		texture2D(FilterTexture, texCoord0.xy + (dir * vec2(0.0/3.0 - 0.5))).xyz +
		texture2D(FilterTexture, texCoord0.xy + (dir * vec2(3.0/3.0 - 0.5))).xyz);

	float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
	float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
	float lumaResult2 = dot(luma, result2);
	
	if(lumaResult2 < lumaMin || lumaResult2 > lumaMax)
		FragColor = vec4(result1, 1.0);
	else
		FragColor = vec4(result2, 1.0);
		
		
		
		
}