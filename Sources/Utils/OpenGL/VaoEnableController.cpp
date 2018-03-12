#include "VaoEnableController.hpp"
#include <GL/glew.h>
#include <iostream>
#include <algorithm>

Utils::EnableVao::EnableVao(const uint32 vao, const VboMap &vertexArrays)
{
	v.reserve(vertexArrays.size());
	for (const auto& va : vertexArrays)
		v.push_back(va.second);
	std::sort(v.begin(), v.end());
    glBindVertexArray(vao);
    Enable();
}

Utils::EnableVao::EnableVao(const uint32 vao, const VboMap& vertexArrays, const std::vector<VertexBufferObjects>& ignored)
{
	v.reserve(vertexArrays.size() - ignored.size());
	uint8 i = 0;
	for (const auto& va : vertexArrays)
	{
		if (va.first != ignored[i++])
			v.push_back(va.second);
	}
	std::sort(v.begin(), v.end());
	glBindVertexArray(vao);
	Enable();
}

Utils::EnableVao::~EnableVao()
{
    Disable();
    glBindVertexArray(0);
}

void Utils::EnableVao::Enable()
{
    for(auto i : v)
        glEnableVertexAttribArray(i);
}

void Utils::EnableVao::Disable()
{
    for(auto iter = v.rbegin(); iter != v.rend(); ++iter)
        glDisableVertexAttribArray(*iter);

}

