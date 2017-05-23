#version 400 core
                                                                           
in vec2 TexCoordOut;                                                                
uniform sampler2D modelTexture;                                                       
uniform float alphaBlend ;         

		
out vec4 FragColor;                                                                 
                                                                                    
void main()                                                                         
{    
	vec4 textureColor = texture(modelTexture,TexCoordOut)  ;	
	if(textureColor.a < 0.5)
		{   
			discard ;
		}	
    FragColor = mix(vec4(0.0),textureColor, 1.0f);
//	FragColor = vec4(1.0,0,0,1);                                                        
}