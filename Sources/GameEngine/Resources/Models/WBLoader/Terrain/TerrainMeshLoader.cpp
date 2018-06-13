#include "TerrainMeshLoader.h"
#include "GameEngine/Resources/TextureLoader.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "GameEngine/Objects/RenderAble/Terrain/TerrainDef.h"
#include "GameEngine/Engine/Configuration.h"

namespace WBLoader
{
	TerrainMeshLoader::TerrainMeshLoader(CTextureLoader & textureLoader)
		: AbstractLoader(textureLoader.GetGraphicsApi(), textureLoader)
	{
	}
	void TerrainMeshLoader::ParseFile(const std::string& filename)
	{
		auto fullFilePath = EngineConf_GetFullDataPathAddToRequierd(filename);
		auto texture = textureLoader_.LoadHeightMap(fullFilePath);

		auto hm = static_cast<HeightMap*>(texture);
		heightMapResolution_ = static_cast<uint16>(hm->GetImage()->width);
		heights_ = &hm->GetImage()->floatData;

		vertices_.reserve(heightMapResolution_ * heightMapResolution_ * 3);
		normals_.reserve(heightMapResolution_ * heightMapResolution_ * 3);
		textureCoords_.reserve(heightMapResolution_ * heightMapResolution_ * 2);
		indices_.reserve(heightMapResolution_ * heightMapResolution_ * 2);

		CreateTerrainVertexes(0, 0, heightMapResolution_, heightMapResolution_);
		CreateMesh();
		Clear();
	}
	bool TerrainMeshLoader::CheckExtension(const std::string& filename)
	{
		auto ext = Utils::GetFileExtension(filename);
		return ext == "terrain";
	}
	std::unique_ptr<CModel> TerrainMeshLoader::Create()
	{
		auto model = std::make_unique<CModel>();

		SMaterial material;
		CMesh newMesh(textureLoader_.GetGraphicsApi(), material);
		newMesh.GetMeshDataRef().positions_ = vertices_;
		newMesh.GetMeshDataRef().textCoords_ = textureCoords_;
		newMesh.GetMeshDataRef().normals_ = normals_;
		newMesh.GetMeshDataRef().tangents_ = tangens_;
		newMesh.GetMeshDataRef().indices_ = indices_;
		model->AddMesh(newMesh);

		//Normals stay
		indices_.clear();
		vertices_.clear();
		tangens_.clear();
		textureCoords_.clear();

		return model;
	}
	void TerrainMeshLoader::CreateTerrainVertexes(uint16 x_start, uint16 y_start, uint16 width, uint16 height)
	{
		for (uint16 i = y_start; i < height; i++)
		{
			for (uint16 j = x_start; j < width; j++)
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
	vec3 TerrainMeshLoader::CalculateNormalMap(uint16 x, uint16 z)
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
	float TerrainMeshLoader::GetHeight(uint16 x, uint16 y) const
	{
		return (*heights_)[x + y * heightMapResolution_];
	}
	void TerrainMeshLoader::CreateMesh()
	{
		//Triaagnle strip
		for (uint16 gz = 0; gz < heightMapResolution_ - 1; gz++)
		{
			if ((gz & 1) == 0)
			{ // even rows
				for (uint16 gx = 0; gx < heightMapResolution_; gx++)
				{
					indices_.push_back(gx + gz * heightMapResolution_);
					indices_.push_back(gx + (gz + 1) * heightMapResolution_);
				}
			}
			else
			{ // odd rows
				for (uint16 gx = heightMapResolution_ - 1; gx > 0; gx--)
				{
					indices_.push_back(gx + (gz + 1) * heightMapResolution_);
					indices_.push_back(gx - 1 + +gz * heightMapResolution_);
				}
			}
		}
	}
	void TerrainMeshLoader::Clear()
	{
		heightMapResolution_ = 0;
		indices_.clear();
		vertices_.clear();
		normals_.clear();
		tangens_.clear();
		textureCoords_.clear();
		heights_->clear();
	}
} // WBLoader