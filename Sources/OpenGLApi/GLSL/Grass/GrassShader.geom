#version 150
layout (points) in;
layout (triangle_strip, max_vertices = 16) out;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

uniform float GlobalTime; // grass movement

//Shadows
uniform mat4 ToShadowMapSpace;
uniform vec3 ShadowVariables;

out vec4 ShadowCoords;
out float UseShadows;
out float ShadowMapSize;

out vec2 TexCoord;
out vec3 WorldPos;

uniform float ViewDistance;

const float size = 0.35f;

int CreateVertex(vec3 offset, vec2 text_coord)
{
    if( offset.y > 0.f)
    {
        //offset.x += sin(GlobalTime);
        //offset.z += cos(GlobalTime);
    }
    vec4 actual_offset = vec4(offset * size, .0f);
    actual_offset.y += size;
    vec4 world_position = (gl_in[0].gl_Position + actual_offset);
    vec4 model_view_position  = ViewMatrix * world_position;    
    
    float Distance = length(model_view_position.xyz) ;
    //if (Distance > ViewDistance + 50.f) return -1;
    
    gl_Position = ProjectionMatrix * model_view_position;
    TexCoord = vec2((text_coord.x+1.0)/2.0, 1 - (-text_coord.y+1.0)/2.0);
    WorldPos = world_position.xyz;
    WorldPos.y += size;
    
    if (ShadowVariables.x > 0.5f)
    {
        ShadowMapSize = ShadowVariables.z;

        float shadow_distance           = ShadowVariables.y;
        const float transition_distance = 10.f;

        float distance_to_cam   = Distance;
        ShadowCoords            = ToShadowMapSpace * world_position; 
        distance_to_cam         = distance_to_cam - (shadow_distance - transition_distance);
        distance_to_cam         = distance_to_cam / shadow_distance;
        ShadowCoords.w          = clamp(1.f - distance_to_cam, 0.f, 1.f);   
    }   
    EmitVertex();
    return 0;
}

void main(void)
{
    UseShadows = ShadowVariables.x;

    vec2 quad_text_coord[4];
    quad_text_coord[0] = vec2(-1,1);
    quad_text_coord[1] = vec2(-1,-1);
    quad_text_coord[2] = vec2(1,1);
    quad_text_coord[3] = vec2(1,-1);

    //x - axis quad
    int pass = 0;
    pass += CreateVertex(vec3(-1.f, 1.f, 0.f), quad_text_coord[0] );
    if( pass < 0) return;
    CreateVertex(vec3(-1.f, -1.f, 0.f), quad_text_coord[1] );
    CreateVertex(vec3(1.f, 1.f, 0.f), quad_text_coord[2] );
    CreateVertex(vec3(1.f, -1.f, 0.f), quad_text_coord[3] );
    EndPrimitive();

    //z - axis quad
    CreateVertex(vec3(0.f, 1.f, -1.f), quad_text_coord[0] );
    CreateVertex(vec3(0.f, -1.f, -1.f), quad_text_coord[1] );
    CreateVertex(vec3(0.f, 1.f, 1.f), quad_text_coord[2] );
    CreateVertex(vec3(0.f, -1.f, 1.f), quad_text_coord[3] );
    EndPrimitive();

    //xz - axis quad
    CreateVertex(vec3(-1.f, 1.f, -1.f), quad_text_coord[0] );
    CreateVertex(vec3(-1.f, -1.f, -1.f), quad_text_coord[1] );
    CreateVertex(vec3(1.f, 1.f, 1.f), quad_text_coord[2] );
    CreateVertex(vec3(1.f, -1.f, 1.f), quad_text_coord[3] );
    EndPrimitive();

    //xz - axis quad
    CreateVertex(vec3(1.f, 1.f, -1.f), quad_text_coord[0] );
    CreateVertex(vec3(1.f, -1.f, -1.f), quad_text_coord[1] );
    CreateVertex(vec3(-1.f, 1.f, 1.f), quad_text_coord[2] );
    CreateVertex(vec3(-1.f, -1.f, 1.f), quad_text_coord[3] );
    EndPrimitive();
}