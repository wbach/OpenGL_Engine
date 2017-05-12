#pragma once
#include <string.h>
#include <math.h>
#include <cfloat>
#include <iostream>
#include "glm/glm.hpp"
#include <vector>
#include <map>
#include "../../Debug_/Log.h"

const int NUM_BONES_PER_VEREX = 4;

struct SVertexBoneData
{
	uint ids[NUM_BONES_PER_VEREX];
	float weights[NUM_BONES_PER_VEREX];

	SVertexBoneData()
	{
		memset(ids, 0, sizeof(ids));
		memset(weights, 0, sizeof(weights));
	}

	void AddBoneData(uint bone_id, float weight)
	{
		for (uint i = 0; i < NUM_BONES_PER_VEREX; i++)
		{
			if (abs(weights[i]) < FLT_MIN)
			{
				ids[i] = bone_id;
				weights[i] = weight;
				return;
			}
		}
		Log("[Error] To many bones per vertex. Current limit : " + std::to_string(NUM_BONES_PER_VEREX));
	}
};
struct SBoneInfo
{
	glm::mat4 BoneOffset = glm::mat4(0.f);
	glm::mat4 FinalTransformation = glm::mat4(0.f);
};

struct SBonesInfo
{
	std::vector<SBoneInfo>		 bone_info;
	std::vector<SVertexBoneData> bones;
	std::map<std::string, uint> bone_mapping; // maps a bone name to its index
	uint num_bones = 0;	
};