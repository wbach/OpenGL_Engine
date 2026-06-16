#version 450

layout(location = 0) out vec2 textureCoords;

#ifdef IS_VULKAN
    #define VERTEX_ID gl_VertexIndex
    #define UBO(set_, binding_) layout(std140, set = set_, binding = binding_)
#else
    #define VERTEX_ID gl_VertexID
    #define UBO(set_, binding_) layout(std140, binding = binding_)
#endif

UBO(0, 3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

void main()
{
    const vec2 positions[6] = vec2[]
    (
        vec2(-1.0,  1.0), 
        vec2(-1.0, -1.0), 
        vec2( 1.0,  1.0),
        
        vec2( 1.0,  1.0), 
        vec2(-1.0, -1.0),
        vec2( 1.0, -1.0)  
    );

    vec2 position = positions[VERTEX_ID];
    gl_Position = perObjectUpdate.transformationMatrix * vec4(position, 0.0, 1.0);
    textureCoords = vec2((position.x + 1.0) / 2.0, 1.0 - (position.y + 1.0) / 2.0);
}