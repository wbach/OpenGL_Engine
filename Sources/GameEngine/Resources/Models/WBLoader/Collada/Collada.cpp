#include "Collada.h"
#include "Xml/MainReader.h"
#include "Logger/Log.h"
#include "Utils/GLM/GLMUtils.h"
#include "GameEngine/Resources/TextureLoader.h"

namespace WBLoader
{
	ColladaDae::ColladaDae(CTextureLoader& textureLodaer)
		: textureLodaer_(textureLodaer)
	{

	}
	void ColladaDae::ParseFile(const std::string & filename)
	{
		GameEngine::Collada::ReadCollada(filename, data_);
		ConstructModel();
	}
	bool ColladaDae::CheckExtension(const std::string & filename)
	{
		auto ext = Utils::GetFileExtension(filename);
		return ext == "dae" || ext == "DAE" || ext == "Dae";
	}
	void ColladaDae::ConstructModel()
	{
		for (const auto& geometry : data_.libraryGeometries_.geometries_)
		{
			NewGeometry(geometry.second);
		}
	}
	void ColladaDae::NewGeometry(const GameEngine::Collada::Geometry& geomtery)
	{
		objects.emplace_back();
		objects.back().transformMatrix = Utils::CreateTransformationMatrix(vec3(0), vec3(-90, 0,0), vec3(1));
		for (const auto& mesh : geomtery.meshes_)
		{
			NewMesh(mesh);
		}
	}
	void ColladaDae::NewMesh(const GameEngine::Collada::Mesh& mesh)
	{
		for (const auto& pair : mesh.polylist_)
		{
			auto& polyList = pair.second;

			objects.back().meshes.emplace_back();
			auto& newMesh = objects.back().meshes.back();

			ApplyMaterials(newMesh.material, GameEngine::Collada::GetSource(polyList.materialId_));

			for (const auto& input : polyList.inputs_)
			{
				PrepareMeshData(mesh, newMesh, input);
			}


			uint32 i = 0;
			for (auto vc : polyList.vcount_)
			{
				for (int x = 0; x < vc; ++x)
				{
					WBLoader::VertexBuffer vb;
					uint32 sumOffsets = 0;
					for (const auto& input : polyList.inputs_)
					{
						auto offset = input.offset;
						sumOffsets += offset;
						auto value = polyList.p_[i + offset];

						if (input.semantic == "VERTEX")
						{
							vb.position = newMesh.vertex[value];
							vb.indexes.x = value;
						}
						if (input.semantic == "NORMAL")
						{
							vb.normal = newMesh.normals[value];
							vb.indexes.y = value;
						}
						if (input.semantic == "TEXCOORD")
						{
							vb.uvs = newMesh.text_coords[value];
							vb.indexes.z = value;
						}
						if (input.semantic == "COLOR")
						{

						}
					}
					newMesh.vertexBuffer.push_back(vb);
					i += polyList.inputs_.size();
				}
			}
		}
	}
	void ColladaDae::GetVec2ArrayFromDataSource(const GameEngine::Collada::Mesh & mesh, const std::string & sourceId, std::vector<vec2>& v)
	{
		if (mesh.sources_.count(sourceId) == 0)
			return;

		const auto& datasource = mesh.sources_.at(sourceId);
		const auto& params = datasource.technique.accessor.params;

		if (params.empty())
			return;

		v.reserve(datasource.dataArray.count);
		GameEngine::Collada::GetVectors2dFromString(datasource.dataArray.data, v);
	}
	void ColladaDae::GetVec3ArrayFromDataSource(const GameEngine::Collada::Mesh& mesh, const std::string& sourceId, std::vector<vec3>& v)
	{
		if (mesh.sources_.count(sourceId) == 0)
			return;

		const auto& datasource = mesh.sources_.at(sourceId);
		const auto& params = datasource.technique.accessor.params;

		if (params.empty())
			return;

		v.reserve(datasource.dataArray.count);
		GameEngine::Collada::GetVectors3dFromString(datasource.dataArray.data, v);
	}
	void ColladaDae::PrepareMeshData(const GameEngine::Collada::Mesh& colladaMesh, Mesh & newMesh, const GameEngine::Collada::Input & input)
	{
		auto source = GameEngine::Collada::GetSource(input.sourceId);

		if (input.semantic == "VERTEX")
		{
			if (colladaMesh.vertices_.count(source) != 0)
			{
				for (const auto& vertInput : colladaMesh.vertices_.at(source).inputs_)
				{
					if (vertInput.semantic == "POSITION")
					{
						auto vertSourceId = GameEngine::Collada::GetSource(vertInput.sourceId);
						if (colladaMesh.sources_.count(vertSourceId) != 0)
						{
							GetVec3ArrayFromDataSource(colladaMesh, vertSourceId, newMesh.vertex);
						}
						else
						{
							Log("POSITION : Undefined reference to : " + vertInput.sourceId);
						}
					}
				}
			}
			else
			{
				GetVec3ArrayFromDataSource(colladaMesh, source, newMesh.vertex);
			}
		}
		else if (input.semantic == "NORMAL")
		{
			GetVec3ArrayFromDataSource(colladaMesh, source, newMesh.normals);
		}
		else if (input.semantic == "TEXCOORD")
		{
			GetVec2ArrayFromDataSource(colladaMesh, source, newMesh.text_coords);
		}
		else if (input.semantic == "COLOR")
		{

		}
	}
	GameEngine::Collada::DataTypes ColladaDae::GetSourceType(const GameEngine::Collada::Source& source) const
	{
		const auto& params = source.technique.accessor.params;

		if (params.empty())
			return GameEngine::Collada::DataTypes();

		//expect all as floats param
		uint8 type = 0;
		if (params.front().type == "float" && params.size() == 3)
			return GameEngine::Collada::DataTypes::VEC3;

		if (params.front().type == "float" && params.size() == 2)
			return GameEngine::Collada::DataTypes::VEC2;

		return GameEngine::Collada::DataTypes();
	}
	void ColladaDae::ApplyMaterials(SMaterial& material, const std::string& materialId)
	{
		if (materialId.empty())
			return;

		const auto& m = data_.libraryMaterials_.materials_[materialId];
		auto& effect = data_.libraryEffects_.effects_[GameEngine::Collada::GetSource(m.instanceEffect)];

		material.ambient = GameEngine::Collada::GetVector3(effect.ambient.value);
		material.diffuse = GameEngine::Collada::GetVector3(effect.diffuse.value);
		material.specular = GameEngine::Collada::GetVector3(effect.specular.value);

		if (!effect.diffuse.texture.empty())
		{
			auto& param = effect.newParams_[effect.diffuse.texture];
			if (param.sampler_)
			{
				const auto& surface = effect.newParams_[param.sampler_.constValue().value_].surface_;

				if (surface)
				{
					const auto& textureFileName = data_.libraryImages_.images_[surface.constValue().initfrom_].initFrom_;
					material.diffuseTexture = textureLodaer_.LoadTexture("Textures/" + textureFileName, true, true, TextureType::MATERIAL);
				}

			}
			if (param.surface_)
			{

			}
		}

		//material.diffuseTexture 
	}

}
