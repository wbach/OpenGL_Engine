#pragma once
#include <bitsery/adapter/buffer.h>
#include <bitsery/bitsery.h>
#include <bitsery/ext/std_variant.h>
#include <bitsery/traits/vector.h>

#include "Image.h"
#include "Logger/Log.h"
#include "Types.h"

namespace Utils
{
const uint32 MAX_CONTAINERSIZE = 100000000;

template <typename S>
void serialize(S& s, Image& obj)
{
    s.value4b(obj.width);
    s.value4b(obj.height);
    s.value1b(obj.channels_);

    s.ext(obj.data_, bitsery::ext::StdVariant{[](S& s, std::monostate&) {},
                                              [](S& s, std::vector<uint8_t>& v) { s.container1b(v, MAX_CONTAINERSIZE); },
                                              [](S& s, std::vector<float>& v) { s.container4b(v, MAX_CONTAINERSIZE); }});
}

}  // namespace Utils