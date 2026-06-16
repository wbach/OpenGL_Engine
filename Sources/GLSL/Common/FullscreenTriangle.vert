
#version 450 core
layout(location = 0) out vec2 textureCoords;

#ifdef IS_VULKAN
    #define VERTEX_ID gl_VertexIndex
    #define UBO(set_, binding_) layout(std140, set = set_, binding = binding_)
#else
    #define VERTEX_ID gl_VertexID
    #define UBO(set_, binding_) layout(std140, binding = binding_)
#endif

void main()
{
    float x = -1.0 + float((VERTEX_ID & 1) << 2);
    float y = -1.0 + float((VERTEX_ID & 2) << 1);

    textureCoords = vec2((x + 1.0) * 0.5, (y + 1.0) * 0.5);

    gl_Position = vec4(x, y, 0.0, 1.0);
}
