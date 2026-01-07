#pragma once

#include <bitsery/adapter/stream.h>
#include <bitsery/bitsery.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>
#include "MeshRawData.h"

namespace bitsery
{
template <typename S>
void serialize(S& s, glm::mat4& m)
{
    float* ptr = glm::value_ptr(m);
    for (int i = 0; i < 16; ++i)
        s.value4b(ptr[i]);
}
}  // namespace bitsery

namespace GraphicsApi
{
template <typename S>
void serialize(S& s, MeshRawData& m)
{
    uint16_t version = 1;
    s.value2b(version);

    constexpr size_t MAX_FLOAT_VEC = 1000000;
    constexpr size_t MAX_INDICES   = 1000000;
    constexpr size_t MAX_MATRICES  = 10000;

    s.container4b(m.positions_, MAX_FLOAT_VEC);
    s.container4b(m.textCoords_, MAX_FLOAT_VEC);
    s.container4b(m.normals_, MAX_FLOAT_VEC);
    s.container4b(m.tangents_, MAX_FLOAT_VEC);
    s.container4b(m.bitangents_, MAX_FLOAT_VEC);

    s.container4b(m.bonesWeights_, MAX_FLOAT_VEC);
    s.container4b(m.joinIds_, MAX_INDICES);

    s.container4b(m.indices_, MAX_INDICES);

    s.container(m.instancedMatrixes_, MAX_MATRICES,
                [](auto& s2, glm::mat4& mtx)
                {
                    float* ptr = glm::value_ptr(mtx);
                    for (int i = 0; i < 16; ++i)
                        s2.value4b(ptr[i]);
                });
}
}  // namespace GraphicsApi
