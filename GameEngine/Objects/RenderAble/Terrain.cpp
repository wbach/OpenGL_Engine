#include "Terrain.h"
#include "../../Utils/GLM/GLMUtils.h"
#include "../../Resources/Models/Model.h"
#include "../../Resources/Textures/Image.h"
#include "../../Resources/Textures/Texture.h"

STerrain::STerrain()
{	
	for (auto& texture : m_Textures)
	{
		texture = nullptr;
	}
}

wb::optional<glm::vec3> STerrain::CollisionDetection(const glm::vec3 & v)
{
	glm::vec3 position;
	auto height = GetHeightofTerrain(v.x, v.z);

	if (height)
	{
		position = v;
		position.y = height.GetValue();
	}
	return position;
}

void STerrain::SetHeight(int x, int y, float value)
{
	m_Heights[x + y*m_HeightMapResolution] = value;
}

wb::optional<float> STerrain::GetHeightofTerrain(glm::vec2 posXZ) const
{
	return GetHeightofTerrain(posXZ.x, posXZ.y);
}

wb::optional<float> STerrain::GetHeightofTerrain(float worldX, float worldZ) const
{
	wb::optional<float> result;

	float terrain_x = worldX - m_WorldTransform.GetPositionXZ().x;
	float terrain_z = worldZ - m_WorldTransform.GetPositionXZ().y;

	float grid_squere_size = TERRAIN_SIZE / ((float) m_HeightMapResolution - 1);
	int grid_x = (int)floor(terrain_x / grid_squere_size);
	int grid_z = (int)floor(terrain_z / grid_squere_size);

	if (grid_x >= m_HeightMapResolution - 1 || grid_z >= m_HeightMapResolution - 1 || grid_x < 0 || grid_z < 0)
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
	m_Heights.resize(x*y);
}

void STerrain::LoadHeight(SImage& height_map)
{
	if (height_map.m_Data == nullptr)
	{
		Log("Loading terrain heights faild. Height map is nullptr.");
		return;
	}

	m_HeightMapResolution = height_map.m_Height;

	auto h = height_map.m_Height;
	auto w = height_map.m_Width;

	InitHeights(w, h);
	
	//bgr2rgb
	for (uint j = 0; j<w*h; j++)
	{
		float r = static_cast<float>(height_map.m_Data[j * 4 + 0]);
		float g = static_cast<float>(height_map.m_Data[j *4 + 1]);
		float b = static_cast<float>(height_map.m_Data[j * 4 + 2]);
		float a = static_cast<float>(height_map.m_Data[j * 4 + 3]);
		float height = r;
		auto max = 255.f;
		height /= max;
		height *= 2.f;
		height -= 1.f;
		height *= 25.f;
		m_Heights[j] = height;
	}
}

float STerrain::GetHeight(int x, int y) const
{
	return m_Heights[x + y*m_HeightMapResolution];
}

void STerrain::Render()
{
	m_Quad.Render(GL_PATCHES);
}
