#version 150 core

#ifndef NUM_LIGHTS
#define NUM_LIGHTS 1
#endif

#ifndef MAX_BONES
#define MAX_BONES 100
#endif

#type vertex

#include <glr>
#include <light>

in vec3 in_Position;
in vec2 in_Texture;
in vec4 in_Color;
in vec3 in_Normal;
in ivec4 in_BoneIds;
in vec4 in_BoneWeights;

out vec2 textureCoord;
out vec3 normalDirection;
out vec3 lightDirection;
out vec4 color;
out float bug;

@bind Light
layout(std140) uniform Lights 
{
	Light lights[ NUM_LIGHTS ];
};

@bind Bone
layout(std140) uniform Bones 
{
	mat4 bones[ MAX_BONES ];
};

void main()
{
	// Calculate the transformation on the vertex position based on the bone weightings
	mat4 boneTransform = bones[ in_BoneIds[0] ] * in_BoneWeights[0];
    boneTransform     += bones[ in_BoneIds[1] ] * in_BoneWeights[1];
    boneTransform     += bones[ in_BoneIds[2] ] * in_BoneWeights[2];
    boneTransform     += bones[ in_BoneIds[3] ] * in_BoneWeights[3];
    
    // Temporary - this will cease all animation (and show just the model) - this works if you want to just show the model
    //mat4 tempM = mat4(1.0);
    //boneTransform = tempM;
    
    // This is for animating the model
    vec4 tempPosition = boneTransform * vec4(in_Position, 1.0);
	gl_Position = pvmMatrix * tempPosition;
	
	// Assign texture coordinates
	textureCoord = in_Texture;
	
	// Calculate normal
	vec4 normalDirTemp = boneTransform * vec4(in_Normal, 0.0);
	normalDirection = normalize(normalMatrix * normalDirTemp.xyz);
	//normalDirection = normalize(normalMatrix * in_Normal);
	
	// Calculate light direction
	vec4 lightDirTemp = viewMatrix * lights[0].direction;
	lightDirection = normalize(vec3(lightDirTemp));
	
	color = in_Color;
	
	
	// If we have any bugs, should highlight the vertex red or green
	bug = 0.0;
	float sum = in_BoneWeights[0] + in_BoneWeights[1] + in_BoneWeights[2] + in_BoneWeights[3];
	if (sum > 1.05f)
		bug = 1.0;
	else if (sum < 0.95f)
		bug = 2.0;
	//else if (in_BoneIds[0] > 32 || in_BoneIds[1] > 32 || in_BoneIds[2] > 32 || in_BoneIds[3] > 32)
	//	bug = 3.0;
	// disable bug highlighting
	//bug = 0.0;
	
	
	
	/*
	bvec4 result = equal( in_BoneIds, ivec4(0, 0, 0, 0) );
	if(result[0] && result[1] && result[2] && result[3])
		bug = 1.0;
	result = equal( in_BoneWeights, vec4(0.0, 0.0, 0.0, 0.0) );
	if(result[0] && result[1] && result[2] && result[3])
		bug = 2.0;
	*/
	
}
