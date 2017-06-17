#pragma once
#include "Types.h"
#include <string.h>
#include <iostream>
#include <cfloat> 
#include <math.h>
#include <vector>
#include <map>

const int NUM_BONES_PER_VEREX = 4;

struct SVertexBoneData
{
	uint32 ids[NUM_BONES_PER_VEREX];
	float weights[NUM_BONES_PER_VEREX];

	SVertexBoneData();

	void AddBoneData(uint32 bone_id, float weight);
};
struct SBoneInfo
{
    mat4 boneOffset = mat4(0.f);
    mat4 finalTransformation = mat4(0.f);
};

struct SBonesInfo
{
    std::vector<SBoneInfo>		 boneInfo;
	std::vector<SVertexBoneData> bones;
    std::map<std::string, uint32>  boneMapping; // maps a bone name to its index
    uint32 numBones = 0;
};
