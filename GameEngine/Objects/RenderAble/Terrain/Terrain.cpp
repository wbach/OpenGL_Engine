#include "Terrain.h"
#include "../../../Utils/GLM/GLMUtils.h"
#include "../../../Resources/Models/Model.h"
#include "../../../Resources/Textures/Image.h"
#include "../../../Resources/Textures/Texture.h"

STerrain::STerrain()
{	
    for (auto& texture : textures)
	{
		texture = nullptr;
	}
}

wb::optional<glm::vec3> STerrain::CollisionDetection(const glm::vec3 & v)
{
    wb::optional<glm::vec3> position;
	auto height = GetHeightofTerrain(v.x, v.z);

	if (height)
	{
        glm::vec3 p = v;
        p.y = height.GetValue();
        position = p;
	}
	return position;
}

void STerrain::SetHeight(int x, int y, float value)
{
    heights[x + y*heightMapResolution] = value;
}

wb::optional<float> STerrain::GetHeightofTerrain(glm::vec2 posXZ) const
{
	return GetHeightofTerrain(posXZ.x, posXZ.y);
}

wb::optional<float> STerrain::GetHeightofTerrain(float worldX, float worldZ) const
{
	wb::optional<float> result;

    float terrain_x = worldX - worldTransform.GetPositionXZ().x;
    float terrain_z = worldZ - worldTransform.GetPositionXZ().y;

    float grid_squere_size = TERRAIN_SIZE / ((float) heightMapResolution - 1);
	int grid_x = (int)floor(terrain_x / grid_squere_size);
	int grid_z = (int)floor(terrain_z / grid_squere_size);

    if (grid_x >= heightMapResolution - 1 || grid_z >= heightMapResolution - 1 || grid_x < 0 || grid_z < 0)
		return result;

	float x_coord = (fmod(terrain_x, grid_squere_size)) / grid_squere_size;
	float z_coord = (fmod(terrain_z, grid_squere_size)) / grid_squere_size;

	if (x_coord <= (1 - z_coord))
	{
		result = Utils::BarryCentric(glm::vec3(0, GetHeight(grid_x, grid_z), 0), glm::vec3(1, GetHeight(grid_x + 1, grid_z), 0),
			glm::vec3(0, GetHeight(grid_x, grid_z + 1), 1), glm::vec2(x_coord, z_coord));

	}
	else
	{
		result = Utils::BarryCentric(glm::vec3(1, GetHeight(grid_x + 1, grid_z), 0), glm::vec3(1, GetHeight(grid_x + 1, grid_z + 1), 1),
			glm::vec3(0, GetHeight(grid_x, grid_z + 1), 1), glm::vec2(x_coord, z_coord));
	}

	return result;
}

void STerrain::InitHeights(int x, int y)
{
    heights.resize(x*y);
}

void STerrain::LoadHeight(const SImage &height_map)
{
    if (height_map.data == nullptr)
	{
		Log("Loading terrain heights faild. Height map is nullptr.");
		return;
	}

    heightMapResolution = height_map.height;

    auto h = height_map.height;
    auto w = height_map.width;

	InitHeights(w, h);
	
	//bgr2rgb
	for (uint j = 0; j<w*h; j++)
	{
        float r = static_cast<float>(height_map.data[j * 4 + 0]);
        //float g = static_cast<float>(height_map.m_Data[j *4 + 1]);
        //float b = static_cast<float>(height_map.m_Data[j * 4 + 2]);
        //float a = static_cast<float>(height_map.m_Data[j * 4 + 3]);
		float height = r;
		auto max = 255.f;
		height /= max;
		height *= 2.f;
		height -= 1.f;
		height *= 25.f;
        heights[j] = height;
	}
}

void STerrain::SetTexture(CTexture *texture, Terrain::TexturesTypes type)
{
    if (type == Terrain::TexturesTypes::count)
        return;

    textures[type] = texture;
}

float STerrain::GetHeight(int x, int y) const
{
    if(heights.empty())
        return 0;
    return heights[x + y*heightMapResolution];
}

void STerrain::Render()
{
    quad.Render(GL_PATCHES);
}
