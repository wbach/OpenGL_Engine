#include "Bone.h"
#include <cfloat>
#include <math.h>
#include "Logger/Log.h"

namespace GameEngine
{
VertexBoneData::VertexBoneData()
{
    //memset(ids, 0, sizeof(ids));
    //memset(weights, 0, sizeof(weights));
}

void VertexBoneData::AddBoneData(uint32 bone_id, float weight)
{
    for (uint32 i = 0; i < NUM_BONES_PER_VEREX; i++)
    {
        if (abs(weights[i]) < FLT_MIN)
        {
            ids[i]     = bone_id;
            weights[i] = weight;
            return;
        }
    }
    /* LOG TO FIX*/  LOG_ERROR << ("To many bones per vertex. Current limit : " + std::to_string(NUM_BONES_PER_VEREX));
}
}  // namespace GameEngine
