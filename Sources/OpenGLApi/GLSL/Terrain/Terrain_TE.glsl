#version 430
#define HEIGHT_FACTOR 800
#define HEIGHT_MAP_SIZE 513.0

layout(quads, fractional_odd_spacing, cw) in;

uniform sampler2D heightmap;
uniform sampler2D normalmap;
uniform vec3 cameraPosition;

in vec2 mapCoord_TE[];
out vec2 mapCoord_GS;
out vec3 normal_GS;


vec3 calculateNormal(vec3 position, vec2 texCoord)
{
    if (distance(position, cameraPosition) > 50000)
    {
        return vec3(0, 1, 0);
    }

    // z0 -- z1 -- z2
    // |     |     |
    // z3 -- h  -- z4
    // |     |     |
    // z5 -- z6 -- z7

    float texelSize = 1.0/ HEIGHT_MAP_SIZE;

    float z0 = texture(heightmap, texCoord + vec2(-texelSize,-texelSize)).r;
    float z1 = texture(heightmap, texCoord + vec2(0,-texelSize)).r;
    float z2 = texture(heightmap, texCoord + vec2(texelSize,-texelSize)).r;
    float z3 = texture(heightmap, texCoord + vec2(-texelSize,0)).r;
    float z4 = texture(heightmap, texCoord + vec2(texelSize,0)).r;
    float z5 = texture(heightmap, texCoord + vec2(-texelSize,texelSize)).r;
    float z6 = texture(heightmap, texCoord + vec2(0,texelSize)).r;
    float z7 = texture(heightmap, texCoord + vec2(texelSize,texelSize)).r;

    vec3 normal;
    float normalStrength = 1.0;
    float yoffset = 0.0f;
    // Sobel Filter
    normal.z = 1.0/normalStrength;
    normal.x = z0 + 2*z3 + z5 - z2 - 2*z4 - z7;
    normal.y = z0 + 2*z1 + z2 -z5 - 2*z6 - z7 + yoffset;

    return (normalize(normal)+1.0)/2.0;
}

void main(){

    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // world position
    vec4 position =
    ((1 - u) * (1 - v) * gl_in[12].gl_Position +
    u * (1 - v) * gl_in[0].gl_Position +
    u * v * gl_in[3].gl_Position +
    (1 - u) * v * gl_in[15].gl_Position);

    vec2 mapCoord =
    ((1 - u) * (1 - v) * mapCoord_TE[12] +
    u * (1 - v) * mapCoord_TE[0] +
    u * v * mapCoord_TE[3] +
    (1 - u) * v * mapCoord_TE[15]);

    float height = texture(heightmap, mapCoord).r;
    height *= HEIGHT_FACTOR;
    height -= HEIGHT_FACTOR /2.f;
    height += 19.f;
    position.y = height;
    mapCoord_GS = mapCoord;
    //normal_GS = calculateNormal(position.xyz, mapCoord);
    gl_Position = position;
}