#version 450

layout (std140, align=16, binding=6) uniform SkyBuffer
{
    mat4 invProj;
    mat4 invViewRot;
    vec4 screenSize;

    vec4 sunDirection; 
    vec4 sunColor;
} skyBuffer;

uniform sampler2D gDepth;

out vec4 outSkyColor;

// --- KONFIGURACJA SŁOŃCA ---
const float SUN_SIZE       = 0.009;  // promień tarczy słońca w radianach (~0.5°)
const float SUN_INTENSITY  = 1.0;    // globalna jasność tarczy + halo
const float HALO_INTENSITY = 0.7;    // intensywność halo wokół słońca

// ---- HELPERS ----
float smoothSunDisk(float cosAngle, float angularRadius)
{
    float inner = cos(angularRadius);
    float outer = cos(angularRadius * 1.1);
    return smoothstep(outer, inner, cosAngle);
}

float haloFalloff(float cosAngle)
{
    return pow(max(cosAngle, 0.0), 128.0);
}

vec3 applyRayleigh(vec3 worldRay, vec3 sunDir)
{
    // dot = kąt między promieniem a słońcem
    float cosAngleSun = dot(worldRay, sunDir); // 1 = patrzymy w stronę słońca, 0 = bok, -1 = przeciwnie
    cosAngleSun = clamp(cosAngleSun, 0.0, 1.0);

    // Kolory w zależności od kąta względem słońca
    vec3 rayleighHorizon = vec3(1.0, 0.5, 0.2); // ciepłe przy horyzoncie i niskim słońcu
    vec3 rayleighZenit   = vec3(0.5, 0.7, 1.0); // niebieskie w zenicie

    // Interpolacja między kolorem horyzontu a nieba, zależnie od słońca
    float t = pow(cosAngleSun, 1.5); // mocniejsze przejście przy patrzeniu w stronę słońca
    vec3 rayleigh = mix(rayleighHorizon, rayleighZenit, t);

    // Skalowanie jasności
    rayleigh *= 2.0;

    return rayleigh;
}


vec3 applyMie(vec3 worldRay, vec3 sunDir, vec3 sunCol)
{
    float mu = dot(worldRay, sunDir);
    return sunCol * pow(max(mu, 0.0), 12.0);
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

    // --- Rekonstrukcja kierunku promienia w world space ---
    vec4 ndc = vec4(uv * 2.0 - 1.0, 1.0, 1.0);
    vec4 viewRay = skyBuffer.invProj * ndc;
    vec3 worldRay = normalize((skyBuffer.invViewRot * vec4(viewRay.xyz, 0.0)).xyz);
    worldRay *= -1.0; // dopasowanie do cieni w light passie

    // --- Gradient nieba ---
    float t = max(worldRay.y * 0.5 + 0.5, 0.0);
    vec3 topColor    = vec3(0.1, 0.3, 0.8);
    vec3 bottomColor = vec3(0.6, 0.8, 1.0);
    vec3 sky = mix(bottomColor, topColor, t);

    // --- Słońce ---
    vec3 sunDir = normalize(skyBuffer.sunDirection.xyz);
    vec3 sunCol = skyBuffer.sunColor.rgb;

    float cosAng = dot(worldRay, sunDir);
    float sunDisk = smoothSunDisk(cosAng, SUN_SIZE);
    float sunHalo = haloFalloff(cosAng);

    // modulacja koloru w zależności od wysokości słońca (poranek/zenit)
    float elevation = clamp(dot(sunDir, vec3(0,1,0)), 0.0, 1.0);
    vec3 sunColMod = mix(vec3(1.0,0.5,0.2), sunCol, elevation);

    vec3 sun =  sunColMod * SUN_INTENSITY * (sunDisk + sunHalo * HALO_INTENSITY);

    // // --- Rozproszenie atmosferyczne ---
    // vec3 rayleigh = applyRayleigh(worldRay, sunDir);
    // vec3 mie      = applyMie(worldRay, sunDir, sunCol);

    // --- Finalny kolor ---
    //vec3 finalSky = sky + rayleigh + mie + sun;
    //finalSky = rayleigh ;

    vec3 finalSky = sky + sun;

    outSkyColor = vec4(finalSky, 1.0);
}
