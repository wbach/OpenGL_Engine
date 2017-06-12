#pragma once
#include <string.h>
#include <math.h>
#include <cfloat>
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include "../../Debug_/Log.h"

const int NUM_BONES_PER_VEREX = 4;

struct SVertexBoneData
{
	uint32 ids[NUM_BONES_PER_VEREX];
	float weights[NUM_BONES_PER_VEREX];

	SVertexBoneData()
	{
		memset(ids, 0, sizeof(ids));
		memset(weights, 0, sizeof(weights));
	}

	void AddBoneData(uint32 bone_id, float weight)
	{
		for (uint32 i = 0; i < NUM_BONES_PER_VEREX; i++)
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
    glm::mat4 boneOffset = glm::mat4(0.f);
    glm::mat4 finalTransformation = glm::mat4(0.f);
};

struct SBonesInfo
{
    std::vector<SBoneInfo>		 boneInfo;
	std::vector<SVertexBoneData> bones;
    std::map<std::string, uint32>  boneMapping; // maps a bone name to its index
    uint32 numBones = 0;
};
