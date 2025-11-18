#version 450

layout (std140, align=16, binding=6) uniform SkyBuffer
{
    mat4 invProj;
    mat4 invViewRot;
    vec4 screenSize;

    vec4 sunDirection; // xyz = direction, w = unused
    vec4 sunColor;     // rgb = sun/moon color, a = isNight (0 day, 1 night)
} skyBuffer;

uniform sampler2D gDepth;

out vec4 outSkyColor;

// --- CONFIG ---
const float SUN_SIZE       = 0.009;
const float SUN_INTENSITY  = 1.0;
const float HALO_INTENSITY = 0.7;

// --- HELPERS ---
float smoothSunDisk(float cosAngle, float angularRadius)
{
    float inner = cos(angularRadius);
    float outer = cos(angularRadius * 1.1);
    return smoothstep(outer, inner, cosAngle);
}

void main()
{
    vec2 uv = gl_FragCoord.xy / skyBuffer.screenSize.xy;

    float depth = texture(gDepth, uv).r;
    if (depth < 1.0)
    {
        outSkyColor = vec4(0.0);
        return;
    }

    // --- RECONSTRUCT WORLD RAY ---
    vec4 ndc = vec4(uv * 2.0 - 1.0, 1.0, 1.0);
    vec4 viewRay = skyBuffer.invProj * ndc;
    vec3 worldRay = normalize((skyBuffer.invViewRot * vec4(viewRay.xyz, 0.0)).xyz);
    //worldRay *= -1.0;

    // --- SUN / MOON DIR & COLORS ---
    vec3 sunDir = normalize(-skyBuffer.sunDirection.xyz); // NOTE: minus!
    vec3 sunCol = skyBuffer.sunColor.rgb;
    float isNight = skyBuffer.sunColor.a; // 0 = day, 1 = night
    float dayFactor = 1.0 - isNight;

    // sun elevation
    float elevation = clamp(dot(sunDir, vec3(0,1,0)), 0.0, 1.0);

    // --- SKY GRADIENT ---
    float t = clamp(worldRay.y * 0.5 + 0.5, 0.0, 1.0);
    float phase = elevation * dayFactor;

    // zenit
    vec3 zenithNight = vec3(0.02, 0.05, 0.10);
    vec3 zenithDay   = vec3(0.10, 0.35, 0.95);
    vec3 zenith = mix(zenithNight, zenithDay, phase);

    // horyzont
    vec3 horizonNight = vec3(0.03, 0.04, 0.08);
    vec3 horizonDay   = vec3(0.6, 0.8, 1.0);
    vec3 horizon = mix(horizonNight, horizonDay, phase);

    // wschód/zachód — tylko gdy dzień i tylko przy niskim słońcu
    float sunsetFactor = (1.0 - elevation) * dayFactor;
    horizon = mix(horizon, sunCol * vec3(1.4, 1.0, 0.7), sunsetFactor);

    vec3 sky = mix(horizon, zenith, t);

    // --- SUN / MOON RENDER ---
    float cosAng = dot(worldRay, sunDir);

    float sunDisk = smoothSunDisk(cosAng, SUN_SIZE);

    // halo (różne dla słońca i księżyca)
    float haloPow = mix(32.0, 128.0, dayFactor);      // day = ostre
    float haloStrength = mix(0.2, 0.7, dayFactor);    // day = mocniejsze
    float sunHalo = pow(max(cosAng, 0.0), haloPow);

    // kolor słońca (ciepły przy horyzoncie)
    vec3 sunDayWarm = vec3(1.0, 0.6, 0.25);
    vec3 sunDayColor = mix(sunDayWarm, sunCol, elevation);

    // kolor księżyca
    vec3 moonColor = vec3(0.7, 0.75, 1.0);

    // finalny kolor tarczy
    vec3 sunOrMoonColor = mix(sunDayColor, moonColor, isNight);

    vec3 sunFinal = sunOrMoonColor * SUN_INTENSITY * 
                    (sunDisk + sunHalo * haloStrength);

    // --- FINAL COLOR ---
    vec3 finalSky = sky + sunFinal;

    outSkyColor = vec4(finalSky, 1.0);
}
