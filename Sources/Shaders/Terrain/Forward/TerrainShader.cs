#version 420 core

layout (vertices = 4) out;

in VS_OUT
{
    vec2 textCoord;
} tcs_in[];

out TCS_OUT
{
    vec2 textCoord;
} tcs_out[];

uniform mat4 modelViewProjectionMatrix;

const float scale = 16.f;

float CalculateTessLvl(vec4 p1, vec4 p2)
{
    return length(p1.xy - p2.xy) * scale + 1.0;
}

void main(void)
{
    if (gl_InvocationID == 0)
    {
        vec4 p0 = modelViewProjectionMatrix * gl_in[0].gl_Position;
        vec4 p1 = modelViewProjectionMatrix * gl_in[1].gl_Position;
        vec4 p2 = modelViewProjectionMatrix * gl_in[2].gl_Position;
        vec4 p3 = modelViewProjectionMatrix * gl_in[3].gl_Position;

        p0 /= p0.w;
        p1 /= p1.w;
        p2 /= p2.w;
        p3 /= p3.w;
      
        float distanceToCamera_p0 = length(gl_in[0].gl_Position.xz);
        float distanceToCamera_p1 = length(gl_in[1].gl_Position.xz);
        float distanceToCamera_p2 = length(gl_in[2].gl_Position.xz);
        float distanceToCamera_p3 = length(gl_in[3].gl_Position.xz);

        if (p0.z <= 0.0 ||
            p1.z <= 0.0 ||
            p2.z <= 0.0 ||
            p3.z <= 0.0)
         {
              gl_TessLevelOuter[0] = 0.0;
              gl_TessLevelOuter[1] = 0.0;
              gl_TessLevelOuter[2] = 0.0;
              gl_TessLevelOuter[3] = 0.0;
         }
         else
         {
            float l0 = CalculateTessLvl(p2, p0);
            float l1 = CalculateTessLvl(p3, p2);
            float l2 = CalculateTessLvl(p3, p1);
            float l3 = CalculateTessLvl(p1, p0);


            gl_TessLevelOuter[0] = l0;
            gl_TessLevelOuter[1] = l1;
            gl_TessLevelOuter[2] = l2;
            gl_TessLevelOuter[3] = l3;
            gl_TessLevelInner[0] = min(l1, l3);
            gl_TessLevelInner[1] = min(l0, l2);
        }
    }
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tcs_out[gl_InvocationID].textCoord = tcs_in[gl_InvocationID].textCoord;
}
