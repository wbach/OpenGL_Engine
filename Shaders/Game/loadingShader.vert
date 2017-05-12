#version 400 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoords ; 
                                                                                     
uniform mat4 transformationMatrix;                                                                
                                                                                    
out vec2 TexCoordOut;                                                               
                                                                                    
void main()                                                                         
{                                                                                   
    gl_Position = transformationMatrix * vec4(position, 1.0);   
                                    
    TexCoordOut = textureCoords;                                                         
}