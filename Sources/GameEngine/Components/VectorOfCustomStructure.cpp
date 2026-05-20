#include "VectorOfCustomStructure.h"

namespace GameEngine
{
namespace Components
{
size_t VectorOfCustomStructure::size() const
{
    return fields.size();
}
void VectorOfCustomStructure::emplace_back()
{
    if (init)
    {
        fields.push_back(CustomStructure{init()});
    }
    else
    {
        fields.push_back(CustomStructure{});
    }
}
void VectorOfCustomStructure::resize(size_t newSize)
{
    auto currentSize = fields.size();

    if (newSize < currentSize)
    {
        fields.resize(newSize);
    }
    else if (newSize > currentSize)
    {
        fields.reserve(newSize);

        size_t elementsToAdd = newSize - currentSize;
        for (size_t i = 0; i < elementsToAdd; ++i)
        {
            if (init)
            {
                fields.push_back(CustomStructure{init()});
            }
            else
            {
                fields.push_back(CustomStructure{});
            }
        }
    }
}
bool VectorOfCustomStructure::empty() const
{
    return fields.empty();
}
void VectorOfCustomStructure::push_back()
{
    if (init)
    {
        fields.push_back(CustomStructure{init()});
    }
}
void VectorOfCustomStructure::push_back(const CustomStructure& value)
{
    if (value.fields.empty() and init)
    {
        fields.push_back(CustomStructure{init()});
    }
    else
    {
        fields.push_back(value);
    }
}
void VectorOfCustomStructure::clear()
{
    fields.clear();
}
void VectorOfCustomStructure::erase(size_t index)
{
    if (index < fields.size())
    {
        fields.erase(fields.begin() + index);
    }
}
VectorOfCustomStructure::ConstIterator VectorOfCustomStructure::erase(VectorOfCustomStructure::ConstIterator position)
{
    return fields.erase(position);
}
CustomStructure& VectorOfCustomStructure::operator[](size_t index)
{
    return fields[index];
}
const CustomStructure& VectorOfCustomStructure::operator[](size_t index) const
{
    return fields[index];
}
VectorOfCustomStructure::ConstIterator VectorOfCustomStructure::begin()
{
    return fields.begin();
}
VectorOfCustomStructure::ConstIterator VectorOfCustomStructure::end()
{
    return fields.end();
}
VectorOfCustomStructure::ConstIterator VectorOfCustomStructure::begin() const
{
    return fields.begin();
}
VectorOfCustomStructure::ConstIterator VectorOfCustomStructure::end() const
{
    return fields.end();
}
}  // namespace Components
}  // namespace GameEngine
