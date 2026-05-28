#pragma once
#include "CustomStructure.h"

namespace GameEngine
{
namespace Components
{
struct VectorOfCustomStructure
{
    std::function<CustomStructure()> createElement;
    std::function<void(std::vector<CustomStructure>&)> initElements;
    std::vector<CustomStructure> structures;

    void init()
    {
        if (initElements)
        {
            initElements(structures);
        }
    }

    size_t size() const;
    void emplace_back();
    void resize(size_t);
    bool empty() const;
    void push_back();
    void push_back(const CustomStructure&);
    void clear();
    void erase(size_t);

    using ConstIterator = std::vector<CustomStructure>::const_iterator;
    ConstIterator erase(ConstIterator);

    CustomStructure& operator[](size_t);
    const CustomStructure& operator[](size_t) const;

    ConstIterator begin();
    ConstIterator end();
    ConstIterator begin() const;
    ConstIterator end() const;
};
}  // namespace Components
}  // namespace GameEngine
