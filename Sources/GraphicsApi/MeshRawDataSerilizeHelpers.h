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
    constexpr size_t MAX_FLOAT_VEC = std::numeric_limits<size_t>::max();
    constexpr size_t MAX_INDICES   = std::numeric_limits<size_t>::max();
    constexpr size_t MAX_MATRICES  = std::numeric_limits<size_t>::max();

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
