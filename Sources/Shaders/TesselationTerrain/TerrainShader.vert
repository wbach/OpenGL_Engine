#version 410 core                                                                               
                                                                                                
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
                                                                                                
uniform mat4 TransformationMatrix;                                                                            
                                                                                                
out vec3 WorldPos_CS_in;                                                                        
out vec2 TexCoord_CS_in;                                                                        
out vec3 Normal_CS_in;                                                                          
                                                                                                
void main()                                                                                     
{                                                                                               
    WorldPos_CS_in = (TransformationMatrix * vec4(Position, 1.0)).xyz;                                  
    TexCoord_CS_in = TexCoord;                                                            
    Normal_CS_in   = (TransformationMatrix * vec4(Normal, 0.0)).xyz;                                    
}
