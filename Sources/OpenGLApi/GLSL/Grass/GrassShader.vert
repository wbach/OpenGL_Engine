#version 440 core

struct GrassInstance
{
    vec4 position;      // xyz: pos, w: scale
    vec4 rotation;    // x: sizeX, y: sizeY, z: rotation, w: unused
    vec4 normal;        // xyz: normal
    vec4 colorAndSizeRandomness;         // rgb: color
};

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
    vec4 fogData; // xyz - color, w - gradient
} perApp;

layout (std140, align=16, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
    vec4 projection;
} perFrame;

layout(std430, binding = 5) buffer GrassBuffer
{
    GrassInstance instances[];
};

layout (std140, align=16, binding=6) uniform GrassShaderBuffer
{
    vec4 windParams;
    vec4 variables; // x = viewDistance, y - globalTime, z,w -dir
} grassShaderBuffer;


out VS_OUT
{
    vec2 texCoord;
    vec4 worldPos;
    vec3 normal;
    vec3 color;
} vs_out;

vec3 rotate_vector(vec4 quat, vec3 vec)
{
    vec4 qv = vec4(
        quat.w * vec.x + quat.y * vec.z - quat.z * vec.y,
        quat.w * vec.y + quat.z * vec.x - quat.x * vec.z,
        quat.w * vec.z + quat.x * vec.y - quat.y * vec.x,
        -quat.x * vec.x - quat.y * vec.y - quat.z * vec.z
    );
    return vec3(
        qv.x * quat.w + qv.w * -quat.x + qv.y * -quat.z - qv.z * -quat.y,
        qv.y * quat.w + qv.w * -quat.y + qv.z * -quat.x - qv.x * -quat.z,
        qv.z * quat.w + qv.w * -quat.z + qv.x * -quat.y - qv.y * -quat.x
    );
}

vec4 CreateQuaternion(vec3 normal)
{
    vec3 up = vec3(0, 1, 0);
    if(abs(dot(normal, up)) > 0.999f) return vec4(0, 0, 0, 1);
    vec3 a = cross(up, normal);
    float w = sqrt(1.0 + dot(up, normal));
    return normalize(vec4(a.x, a.y, a.z, w));
}

vec3 ApplyWind(vec3 offset, vec3 worldBasePos, float heightFactor)
{
    float t = 6.f * grassShaderBuffer.variables.y;
    vec3 windDir = normalize(vec3(grassShaderBuffer.variables.z, 0.f, grassShaderBuffer.variables.w));
    float phase = dot(worldBasePos.xz, vec2(0.21, 0.17));

    float windWaves = sin(t * 1.3 + phase) * 0.6 + sin(t * 0.7 + phase * 2.2) * 0.3 + sin(t * 3.1 + phase * 0.9) * 0.1;
    float hFactor = pow(clamp(heightFactor, 0.0, 1.0), 2.5);
    
    vec3 windOffset = windDir * windWaves * grassShaderBuffer.windParams.y * hFactor * grassShaderBuffer.windParams.x * 2.f;
    return offset + windOffset;
}

void main()
{
    int globalVertexIdx = gl_VertexID;
    int instanceID = globalVertexIdx / 96; 
    int vertIdx = globalVertexIdx % 96;

    int quadIdx = vertIdx / 24;        
    int segIdx  = (vertIdx % 24) / 6;    
    int triIdx  = vertIdx % 6;           

    GrassInstance instance = instances[instanceID];
    vec3 baseWorldPos = instance.position.xyz;
    
    float dist = length(baseWorldPos - perFrame.cameraPosition);
    if (dist > grassShaderBuffer.variables.x)
    {
        gl_Position = vec4(0, 0, 0, 0); 
        return; 
    }

    if (quadIdx >= 2 && dist > (grassShaderBuffer.variables.x / 3.0))
    {
        gl_Position = vec4(0, 0, 0, 0); 
        return;
    }

    vec3 c0, c1, c2, c3;
    float w = 0.5;
    if (quadIdx == 0)      { c0 = vec3(-w,0,0); c1 = vec3(-w,1,0); c2 = vec3(w,0,0); c3 = vec3(w,1,0); }
    else if (quadIdx == 1) { c0 = vec3(0,0,-w); c1 = vec3(0,1,-w); c2 = vec3(0,0,w); c3 = vec3(0,1,w); }
    else if (quadIdx == 2) { c0 = vec3(-0.4,0,-0.4); c1 = vec3(-0.4,1,-0.4); c2 = vec3(0.4,0,0.4); c3 = vec3(0.4,1,0.4); }
    else                   { c0 = vec3(0.4,0,-0.4); c1 = vec3(0.4,1,-0.4); c2 = vec3(-0.4,0,0.4); c3 = vec3(-0.4,1,0.4); }

    float y0 = float(segIdx) / 4.0;
    float y1 = float(segIdx + 1) / 4.0;

    int indices[6] = int[](0, 1, 2, 0, 2, 3);
    int localV = indices[triIdx];

    vec3 pos;
    vec2 uv;
    float currentY;

    if (localV == 0)      { pos = mix(c0, c1, y0); uv = vec2(0, y0); currentY = y0; }
    else if (localV == 1) { pos = mix(c0, c1, y1); uv = vec2(0, y1); currentY = y1; }
    else if (localV == 2) { pos = mix(c2, c3, y1); uv = vec2(1, y1); currentY = y1; }
    else                  { pos = mix(c2, c3, y0); uv = vec2(1, y0); currentY = y0; }

    pos.xyz *= instance.colorAndSizeRandomness.w;

    pos = ApplyWind(pos, baseWorldPos, currentY);
    
    vec4 quat = CreateQuaternion(normalize(instance.normal.xyz));
    vec3 finalOffset = rotate_vector(quat, pos);

    vec4 worldPos = vec4(baseWorldPos + finalOffset, 1.0);
    gl_Position = perFrame.projectionViewMatrix * worldPos;

    vs_out.texCoord = uv;
    vs_out.worldPos = worldPos;
    vs_out.normal = instance.normal.xyz;
    vs_out.color = instance.colorAndSizeRandomness.xyz;
}