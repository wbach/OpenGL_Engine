#include "TerrainMeshLoader.h"
#include "GameEngine/Resources/TextureLoader.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "GameEngine/Objects/RenderAble/Terrain/TerrainDef.h"

namespace WBLoader
{
	TerrainMeshLoader::TerrainMeshLoader(CTextureLoader & textureLoader)
		: textureLoader_(textureLoader)
	{
	}
	void TerrainMeshLoader::ParseFile(const std::string& filename)
	{
		auto texture = textureLoader_.LoadHeightMap(filename);

		auto hm = static_cast<HeightMap*>(texture);
		heightMapResolution_ = hm->GetImage()->width;
		heights_ = &hm->GetImage()->floatData;

		vertices_.reserve(heightMapResolution_ * heightMapResolution_ * 3);
		normals_.reserve(heightMapResolution_ * heightMapResolution_ * 3);
		textureCoords_.reserve(heightMapResolution_ * heightMapResolution_ * 2);
		indices_.reserve(heightMapResolution_ * heightMapResolution_ * 2);

		CreateTerrainVertexes(0, 0, heightMapResolution_, heightMapResolution_);
		CreateMesh();
	}
	bool TerrainMeshLoader::CheckExtension(const std::string& filename)
	{
		auto ext = Utils::GetFileExtension(filename);
		return ext == "terrain";
	}
	std::list<CMesh> TerrainMeshLoader::CreateFinalMesh()
	{
		std::list<CMesh> output;

		SMaterial material;
		output.emplace_back(material, vertices_, textureCoords_, normals_, tangens_, indices_);

		//Normals stay
		indices_.clear();
		vertices_.clear();
		tangens_.clear();
		textureCoords_.clear();

		return output;
	}
	void TerrainMeshLoader::CreateTerrainVertexes(int x_start, int y_start, int width, int height)
	{
		int i = 0;
		for (i = y_start; i < height; i++)
		{
			for (int j = x_start; j < width; j++)
			{
				float height = GetHeight(j, i);
				vertices_.push_back(static_cast<float>(j) / (static_cast<float>(heightMapResolution_ - 1)) * Terrain::SIZE * Terrain::VSCALE * Terrain::PART_SIZE);
				vertices_.push_back(height);
				vertices_.push_back(static_cast<float>(i) / (static_cast<float>(heightMapResolution_) - 1) * Terrain::SIZE * Terrain::VSCALE * Terrain::PART_SIZE);

				glm::vec3 normal = CalculateNormalMap(j, i);

				normals_.push_back(normal.x);
				normals_.push_back(normal.y);
				normals_.push_back(normal.z);

				textureCoords_.push_back(static_cast<float>(j) / static_cast<float>(heightMapResolution_ - 1));
				textureCoords_.push_back(static_cast<float>(i) / static_cast<float>(heightMapResolution_ - 1));
			}
		}
	}
	vec3 TerrainMeshLoader::CalculateNormalMap(int x, int z)
	{
		int lx = x - 1; if (lx < 0) lx = 0;
		int rx = x + 1; if (rx > heightMapResolution_ - 1) rx = heightMapResolution_ - 1;
		int dz = z - 1; if (dz < 0) dz = 0;
		int uz = z + 1; if (uz > heightMapResolution_ - 1) uz = heightMapResolution_ - 1;
		float heightL = GetHeight(lx, z);
		float heightR = GetHeight(rx, z);
		float heightD = GetHeight(x, dz);
		float heightU = GetHeight(x, uz);
		glm::vec3 normal(heightL - heightR, 2.0f, heightD - heightU);
		glm::normalize(normal);
		return normal;
	}
	float TerrainMeshLoader::GetHeight(int x, int y) const
	{
		return (*heights_)[x + y * heightMapResolution_];
	}
	void TerrainMeshLoader::CreateMesh()
	{
		int pointer = 0;
		//Triaagnle strip
		for (unsigned short gz = 0; gz < heightMapResolution_ - 1; gz++)
		{
			if ((gz & 1) == 0)
			{ // even rows
				for (unsigned short gx = 0; gx < heightMapResolution_; gx++)
				{
					indices_.push_back(gx + gz * heightMapResolution_);
					indices_.push_back(gx + (gz + 1) * heightMapResolution_);
				}
			}
			else
			{ // odd rows
				for (unsigned short gx = heightMapResolution_ - 1; gx > 0; gx--)
				{
					indices_.push_back(gx + (gz + 1) * heightMapResolution_);
					indices_.push_back(gx - 1 + +gz * heightMapResolution_);
				}
			}
		}
	}
} // WBLoader