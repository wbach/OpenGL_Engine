#version 450

layout (std140, align=16, binding=6) uniform SkyBuffer
{
    mat4 invProj;
    mat4 invViewRot;
    vec4 screenSize;
} skyBuffer;


uniform sampler2D gDepth;
// uniform mat4 invProj;
// uniform mat4 invViewRot;  // view matrix without translation

out vec4 outSkyColor;

void main()
{
    // outSkyColor = vec4(1, 0, 0, 1);
    // return;
    
    vec2 uv = gl_FragCoord.xy / skyBuffer.screenSize.xy;

    float depth = texture(gDepth, uv).r;

    // Jeśli coś jest przed nami → brak nieba
    if (depth < 1.0) {
        outSkyColor = vec4(0.0);
        return;
    }

    // Ray direction reconstruction
    vec4 ndc = vec4(uv * 2.0 - 1.0, 1.0, 1.0);
    vec4 viewRay = skyBuffer.invProj * ndc;
    viewRay = vec4(viewRay.xy, -1.0, 0.0);   // z -1 patrzy w kierunku sceny
    vec3 worldRay = normalize((skyBuffer.invViewRot * viewRay).xyz);

    // Basic sky gradient
    float t = max(worldRay.y * 0.5 + 0.5, 0.0);  // 0..1
    vec4 topColor = vec4(0.1, 0.3, 0.8, 1.f);
    vec4 bottomColor = vec4(0.6, 0.8, 1.0, 1.f);

    outSkyColor = mix(bottomColor, topColor, t);
}
