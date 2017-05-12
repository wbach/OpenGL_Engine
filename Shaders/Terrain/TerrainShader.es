#version 400
layout( triangles, equal_spacing, ccw ) in;

uniform sampler2D gDisplacementMap;
uniform float heightFactor;

in Vertex
{
	vec3 position;
	vec2 textCoord;
	vec3 normal;	
} In[];

out Vertex
{
	vec3 position;
	vec2 textCoord;
	vec3 normal;	
} Out;

patch in Patch
{
	mat4 viewProjection;
} InPatch;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
   	return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
   	return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main()
{
	Out.normal = interpolate3D(In[0].normal, In[1].normal, In[2].normal);
   	Out.normal = normalize(Out.normal);
				  
    Out.position	= interpolate3D(In[0].position, In[1].position, In[2].position);
	Out.normal	 	= vec3(0.f, 1.f, 0.f);
	Out.textCoord	= interpolate2D(In[0].textCoord, In[1].textCoord, In[2].textCoord);
	
	float displacement = texture(gDisplacementMap, Out.textCoord.xy).x;
	
	Out.position += Out.normal * displacement * heightFactor;
	
	gl_Position = InPatch.viewProjection * vec4(Out.position, 1.f);
}