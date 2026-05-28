#include "VectorOfCustomStructure.h"

namespace GameEngine
{
namespace Components
{
size_t VectorOfCustomStructure::size() const
{
    return structures.size();
}
void VectorOfCustomStructure::emplace_back()
{
    if (createElement)
    {
        structures.push_back(CustomStructure{createElement()});
    }
    else
    {
        structures.push_back(CustomStructure{});
    }
}
void VectorOfCustomStructure::resize(size_t newSize)
{
    auto currentSize = structures.size();

    if (newSize < currentSize)
    {
        structures.resize(newSize);
    }
    else if (newSize > currentSize)
    {
        structures.reserve(newSize);

        size_t elementsToAdd = newSize - currentSize;
        for (size_t i = 0; i < elementsToAdd; ++i)
        {
            if (createElement)
            {
                structures.push_back(CustomStructure{createElement()});
            }
            else
            {
                structures.push_back(CustomStructure{});
            }
        }
    }
}
bool VectorOfCustomStructure::empty() const
{
    return structures.empty();
}
void VectorOfCustomStructure::push_back()
{
    if (createElement)
    {
        structures.push_back(CustomStructure{createElement()});
    }
}
void VectorOfCustomStructure::push_back(const CustomStructure& value)
{
    if (value.fields.empty() and createElement)
    {
        structures.push_back(CustomStructure{createElement()});
    }
    else
    {
        structures.push_back(value);
    }
}
void VectorOfCustomStructure::clear()
{
    structures.clear();
}
void VectorOfCustomStructure::erase(size_t index)
{
    if (index < structures.size())
    {
        structures.erase(structures.begin() + index);
    }
}
VectorOfCustomStructure::ConstIterator VectorOfCustomStructure::erase(VectorOfCustomStructure::ConstIterator position)
{
    return structures.erase(position);
}
CustomStructure& VectorOfCustomStructure::operator[](size_t index)
{
    return structures[index];
}
const CustomStructure& VectorOfCustomStructure::operator[](size_t index) const
{
    return structures[index];
}
VectorOfCustomStructure::ConstIterator VectorOfCustomStructure::begin()
{
    return structures.begin();
}
VectorOfCustomStructure::ConstIterator VectorOfCustomStructure::end()
{
    return structures.end();
}
VectorOfCustomStructure::ConstIterator VectorOfCustomStructure::begin() const
{
    return structures.begin();
}
VectorOfCustomStructure::ConstIterator VectorOfCustomStructure::end() const
{
    return structures.end();
}
}  // namespace Components
}  // namespace GameEngine
