#version 430

layout (location = 0) in vec2 position0;

uniform vec3 cameraPosition;
uniform mat4 localMatrix;
uniform mat4 worldMatrix;
uniform float scaleY;
uniform int lod;
uniform vec2 index;
uniform float gap;
uniform vec2 location;
uniform int lod_morph_area_1;
uniform int lod_morph_area_2;
uniform int lod_morph_area_3;
uniform int lod_morph_area_4;
uniform int lod_morph_area_5;
uniform int lod_morph_area_6;
uniform int lod_morph_area_7;
uniform int lod_morph_area_8;
uniform sampler2D heightmap;

out vec2 mapCoord_TC;

float morphLatitude(vec2 position) {

    vec2 frac = position - location;

    if (index == vec2(0,0)){
        float morph = frac.x - frac.y;
        if (morph > 0)
            return morph;
    }
    if (index == vec2(1,0)){
        float morph = gap - frac.x - frac.y;
        if (morph > 0)
            return morph;
    }
    if (index == vec2(0,1)){
        float morph = frac.x + frac.y - gap;
        if (morph > 0)
            return -morph;
    }
    if (index == vec2(1,1)){
        float morph = frac.y - frac.x;
        if (morph > 0)
            return -morph;
    }
    return 0;
}

float morphLongitude(vec2 position) {

    vec2 frac = position - location;

    if (index == vec2(0,0)){
        float morph = frac.y - frac.x;
        if (morph > 0)
            return -morph;
    }
    if (index == vec2(1,0)){
        float morph = frac.y - (gap - frac.x);
        if (morph > 0)
            return morph;
    }
    if (index == vec2(0,1)){
        float morph = gap - frac.y - frac.x;
        if (morph > 0)
            return -morph;
    }
    if (index == vec2(1,1)){
        float morph = frac.x - frac.y;
        if (morph > 0)
            return morph;
    }
    return 0;
}

vec2 morph(vec2 localPosition, int morph_area){

    vec2 morphing = vec2(0,0);

    vec2 fixPointLatitude = vec2(0,0);
    vec2 fixPointLongitude = vec2(0,0);
    float distLatitude;
    float distLongitude;

    if (index == vec2(0,0)) {
        fixPointLatitude = location + vec2(gap,0);
        fixPointLongitude = location + vec2(0,gap);
    }
    else if (index == vec2(1,0)) {
        fixPointLatitude = location;
        fixPointLongitude = location + vec2(gap,gap);
    }
    else if (index == vec2(0,1)) {
        fixPointLatitude = location + vec2(gap,gap);
        fixPointLongitude = location;
    }
    else if (index == vec2(1,1)) {
        fixPointLatitude = location + vec2(0,gap);
        fixPointLongitude = location + vec2(gap,0);
    }

    float planarFactor = 0;
    if (cameraPosition.y > abs(scaleY))
        planarFactor = 1;
    else
        planarFactor = cameraPosition.y/ abs(scaleY);

    distLatitude = length(cameraPosition - (worldMatrix *
                    vec4(fixPointLatitude.x,planarFactor,fixPointLatitude.y,1)).xyz);
    distLongitude = length(cameraPosition - (worldMatrix *
                    vec4(fixPointLongitude.x,planarFactor,fixPointLongitude.y,1)).xyz);

    if (distLatitude > morph_area)
        morphing.x = morphLatitude(localPosition.xy);
    if (distLongitude > morph_area)
        morphing.y = morphLongitude(localPosition.xy);

    return morphing;
}

void main()
{
    int lod_morph_area[8];
    lod_morph_area[0] = lod_morph_area_1;
    lod_morph_area[1] = lod_morph_area_2;
    lod_morph_area[2] = lod_morph_area_3;
    lod_morph_area[3] = lod_morph_area_4;
    lod_morph_area[4] = lod_morph_area_5;
    lod_morph_area[5] = lod_morph_area_6;
    lod_morph_area[6] = lod_morph_area_7;
    lod_morph_area[7] = lod_morph_area_8;

    vec2 localPosition = (localMatrix * vec4(position0.x,0,position0.y,1)).xz;

    if (lod > 0)
        localPosition += morph(localPosition, lod_morph_area[lod-1]);

    mapCoord_TC = localPosition;
    gl_Position = worldMatrix * vec4(localPosition.x, texture(heightmap, localPosition).r, localPosition.y, 1);
}