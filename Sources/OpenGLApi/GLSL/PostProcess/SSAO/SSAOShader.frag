#version 450 core

layout (location = 0) out float FragColor;

in vec2 textureCoords;

// Tekstury z G-Buffera
layout (binding = 0) uniform sampler2D gPosition; // Pozycje w View Space
layout (binding = 1) uniform sampler2D gNormal;   // Normalne w View Space
layout (binding = 2) uniform sampler2D texNoise;  // Tekstura szumu 4x4

// Uniform Buffer Object - slot 6
layout (std140, binding = 6) uniform SsaoBuffer
{
    mat4 projection;
    vec4 params; // x: noiseScale.x, y: noiseScale.y, z: radius, w: bias
    vec4 samples[64];
} ssao;

void main()
{
    const float radius = ssao.params.z;
    const float bias   = ssao.params.w;
    const vec2 noiseScale = ssao.params.xy;

    // 1. Pobranie danych wejściowych
    vec3 fragPos   = texture(gPosition, textureCoords).xyz;
    vec3 normal    = normalize(texture(gNormal, textureCoords).rgb);

    // Pobranie szumu i skalowanie go do rozmiaru ekranu
    vec3 randomVec = normalize(texture(texNoise, textureCoords * ssao.noiseScale).xyz);

    // 2. Utworzenie macierzy TBN (Tangent-Bitangent-Normal)
    // Budujemy bazę wokół normalnej fragmentu, używając szumu do losowej rotacji
    vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN       = mat3(tangent, bitangent, normal);

    // 3. Obliczanie okluzji
    float occlusion = 0.0;

    for(int i = 0; i < 64; ++i)
    {
        // Pobieramy próbkę z kernela i przenosimy do View Space (za pomocą TBN)
        vec3 samplePos = TBN * ssao.samples[i].xyz;
        samplePos = fragPos + samplePos * ssao.radius;

        // 4. Projekcja próbki na współrzędne ekranowe (UV)
        vec4 offset = vec4(samplePos, 1.0);
        offset = ssao.projection * offset;    // Transformacja do Clip Space
        offset.xyz /= offset.w;               // Perspective divide [-1, 1]
        offset.xyz = offset.xyz * 0.5 + 0.5;  // Transformacja do [0, 1] (UV)

        // 5. Pobranie rzeczywistej głębokości (Z) z G-Buffera w punkcie próbki
        float sampleDepth = texture(gPosition, offset.xy).z;

        // 6. Test okluzji
        // Sprawdzamy, czy rzeczywista geometria jest "bliżej" kamery niż nasza próbka
        // Range Check: wygasza okluzję, jeśli różnica głębi jest zbyt duża (zapobiega artefaktom na krawędziach tła)
        float rangeCheck = smoothstep(0.0, 1.0, ssao.radius / abs(fragPos.z - sampleDepth));

        // Jeśli sampleDepth jest "większe" (bliżej 0, bo Z jest ujemne), to mamy zasłonięcie
        occlusion += (sampleDepth >= samplePos.z + ssao.bias ? 1.0 : 0.0) * rangeCheck;
    }

    // Normalizacja wyniku: 0.0 (pełna okluzja) do 1.0 (brak okluzji)
    occlusion = 1.0 - (occlusion / 64.0);
    FragColor = occlusion;
}