#version 400                                                         
layout(vertices = 3) out;                                                                     

in Vertex
{
	vec3 position;
	vec2 textCoord;
	vec3 normal;
	vec2 winPosition;
	mat4 viewProjection;
} In[];

out Vertex
{
	vec3 position;
	vec2 textCoord;
	vec3 normal;
	mat4 viewProjection;	
} Out[];

//	[Debug]
uniform float distanceDev;
uniform int minTessLevelOuter;

//struct PatchData
//{
//	mat4 viewProjection;
//};

//patch out PatchData outPatch;

//patch out Patch
//{
//	mat4 viewProjection;
//} OutPatch;
                                                    													
void main()
{
	if( gl_InvocationID == 0 )
    {
    	//OutPatch.viewProjection = In[gl_InvocationID].viewProjection;
		//outPatch.viewProjection = In[gl_InvocationID].viewProjection;
       // gl_TessLevelOuter[0] = 2;
       // gl_TessLevelOuter[1] = 2;
       // gl_TessLevelOuter[2] = 2;
       // gl_TessLevelInner[0] = 2;
	    float d01 = distance( In[0].winPosition, In[1].winPosition );
		float d12 = distance( In[1].winPosition, In[2].winPosition );
		float d20 = distance( In[2].winPosition, In[0].winPosition );

		int a = minTessLevelOuter;
		float dist = distanceDev;
		dist = 5;
		
		int minLvl = minTessLevelOuter;
		minLvl = 8;
		gl_TessLevelOuter[0] = min( max( 1.0, d12 / dist ), minLvl );
		gl_TessLevelOuter[1] = min( max( 1.0, d20 / dist ), minLvl );
		gl_TessLevelOuter[2] = min( max( 1.0, d01 / dist ), minLvl );//gl_MaxTessGenLevel
		gl_TessLevelInner[0] = (gl_TessLevelOuter[0] + gl_TessLevelOuter[1] + gl_TessLevelOuter[2]) / 3.0;
    }

    // kopiowanie wybranego standardowego atrybutu wierzchołka
    Out[gl_InvocationID].viewProjection = In[gl_InvocationID].viewProjection;
	Out[gl_InvocationID].textCoord = In[gl_InvocationID].textCoord;
	Out[gl_InvocationID].normal = In[gl_InvocationID].normal;
	Out[gl_InvocationID].position = In[gl_InvocationID].position;

    //gl_out[gl_InvocationID].gl_Position = In[gl_InvocationID].position;
}                                                                                              