#include "Bone.h"
#include "Logger/Log.h"

SVertexBoneData::SVertexBoneData()
{
	memset(ids, 0, sizeof(ids));
	memset(weights, 0, sizeof(weights));
}

void SVertexBoneData::AddBoneData(uint32 bone_id, float weight)
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
