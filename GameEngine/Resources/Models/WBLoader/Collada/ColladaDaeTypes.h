#pragma once
#include <vector>

namespace WBLoader
{
    namespace ColladaTypes
    {
        enum ArrayType
        {
            POSITIONS,
            NORMALS,
            TEXT_COORD,
            JOINT_NAMES,
            BIND_POSES_ARRAY,
            JOINT_WEIGHTS,
            UNKNOWN
        };

        struct source
        {
            std::vector<float> floatArray;
        };
    }
}
