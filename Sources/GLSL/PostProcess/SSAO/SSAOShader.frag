#version 450 core

layout (std140, align=16, binding=6) uniform SsaoBuffer
{
    mat4 projection;
    vec4 params; // x: noiseScale.x, y: noiseScale.y, z: radius, w: bias
    vec4 samples[64];
} ssao;

layout (binding = 0) uniform sampler2D gDepth;
layout (binding = 1) uniform sampler2D gNormal;
layout (binding = 2) uniform sampler2D texNoise;

in vec2 textureCoords;

layout (location = 0) out float FragColor;

vec3 getPosViewSpace(vec2 uv)
{
    float depth = texture(gDepth, uv).r;
    vec4 clipSpacePos = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    mat4 invProj = inverse(ssao.projection);
    vec4 viewSpacePos = invProj * clipSpacePos;
    return viewSpacePos.xyz / viewSpacePos.w;
}

void main()
{
    const float radius = ssao.params.z;
    const float bias   = ssao.params.w;
    const vec2 noiseScale = ssao.params.xy;

    vec3 fragPos = getPosViewSpace(textureCoords);
    vec3 normal    = normalize(texture(gNormal, textureCoords).rgb);

    // Pobranie szumu i skalowanie go do rozmiaru ekranu
    vec3 randomVec = normalize(texture(texNoise, textureCoords * noiseScale).xyz);

    // Utworzenie macierzy TBN (Tangent-Bitangent-Normal)
    // Budujemy bazę wokół normalnej fragmentu, używając szumu do losowej rotacji
    vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN       = mat3(tangent, bitangent, normal);

    // Obliczanie okluzji
    float occlusion = 0.0;

    for(int i = 0; i < 64; ++i)
    {
        // Pobieramy próbkę z kernela i przenosimy do View Space (za pomocą TBN)
        vec3 samplePos = TBN * ssao.samples[i].xyz;
        samplePos = fragPos + samplePos * radius;

        // Projekcja próbki na współrzędne ekranowe (UV)
        vec4 offset = vec4(samplePos, 1.0);
        offset = ssao.projection * offset;    // Transformacja do Clip Space
        offset.xyz /= offset.w;               // Perspective divide [-1, 1]
        offset.xyz = offset.xyz * 0.5 + 0.5;  // Transformacja do [0, 1] (UV)

        // Pobranie rzeczywistej głębokości (Z) z G-Buffera w punkcie próbki
        float sampleDepth = getPosViewSpace(offset.xy).z;

        // Test okluzji
        // Sprawdzamy, czy rzeczywista geometria jest "bliżej" kamery niż nasza próbka
        // Range Check: wygasza okluzję, jeśli różnica głębi jest zbyt duża (zapobiega artefaktom na krawędziach tła)
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));

        // Jeśli sampleDepth jest "większe" (bliżej 0, bo Z jest ujemne), to mamy zasłonięcie
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    // Normalizacja wyniku: 0.0 (pełna okluzja) do 1.0 (brak okluzji)
    occlusion = 1.0 - (occlusion / 64.0);

    FragColor = occlusion;
}
