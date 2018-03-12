#include "Collada.h"
#include "Xml/MainReader.h"
#include "Logger/Log.h"
#include "Utils/GLM/GLMUtils.h"
#include "GameEngine/Resources/TextureLoader.h"
#include <algorithm>
#include <numeric>

static GameEngine::Collada::Node staticNode;
static mat4 correction_matrix = Utils::CreateTransformationMatrix(vec3(0), vec3(-90, 0, 0), vec3(1));

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
		FillAnimationData();
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
	void ColladaDae::FillAnimationData()
	{
		for (const auto& vs : data_.libraryVisualScenes_.visualScenes_)
		{
			for (const auto& node : vs.second.nodes_)
			{
				if (GetType(node.second.name_) != GameEngine::Collada::ObjectType::LIBRARY_GOMETRIES && node.second.instanceController_)
				{
					if (idToMeshMap_.count(node.second.name_) == 0)
						continue;

					auto mesh = idToMeshMap_[node.second.name_];
					uint32 i = 0;

					const auto& ic = node.second.instanceController_.constValue();
					auto skinSource = GameEngine::Collada::GetSource(ic.url_);
					auto& skin = data_.libraryControllers_.controllers_[skinSource].skins_.begin()->second;

					for (const auto& in : skin.vertexWeights_.inputs_)
					{
						if (in.semantic == "WEIGHT")
						{
							auto source = GameEngine::Collada::GetSource(in.sourceId);
							GameEngine::Collada::GetFloatsFromString(skin.sources_[source].dataArray.data, mesh->bonesWeights);
						}
					}

					uint32 vbId = 0;
					for (auto vc : skin.vertexWeights_.vcount_)
					{
						std::vector<std::pair<int32, float>> wjoints;

						for (uint32 x = 0; x < vc; ++x)
						{
							uint32 sumOffsets = 0;
							
							wjoints.emplace_back();
							for (const auto& in : skin.vertexWeights_.inputs_)
							{
								auto offset = in.offset;
								sumOffsets += offset;
								auto value = skin.vertexWeights_.v_[i + offset];

								if (in.semantic == "JOINT")
								{
									wjoints.back().first = value;
								}
								if (in.semantic == "WEIGHT")
								{
									auto weightValue = mesh->bonesWeights[value];
									wjoints.back().second = weightValue;
								}
							}
							i += skin.vertexWeights_.inputs_.size();
						}

						std::sort(wjoints.begin(), wjoints.end(), [](const std::pair<int32, float>& a, const std::pair<int32, float>& b) -> bool 
						{
							return a.second > b.second;
						});


						vec3 weight(0);
						vec3i joints;

						std::pair<int32, float> sum;
						int x = 0;
						for (const auto& p : wjoints)
						{
							if (x > 2)
								break;
							sum.first += p.first;
							sum.second += p.second;

							switch (x)
							{
							case 0: joints.x = p.first; weight.x = p.second; break;
							case 1: joints.y = p.first; weight.y = p.second; break;
							case 2: joints.z = p.first; weight.z = p.second; break;
							}

							++x;
						}

						weight /= sum.second;

						for (const auto& ii : mesh->vertexPlacesInVertexBuffer_[vbId])
						{
							mesh->vertexBuffer[ii].jointIds = joints;
							mesh->vertexBuffer[ii].weights = weight;
						}

						++vbId;
					}
					const auto& skeleton = node.second.instanceController_.constValue().skeleton_;
					const auto& node = GetNode(GameEngine::Collada::GetSource(skeleton));
					mesh->bonesWeights.clear();//!n
					std::unordered_map<std::string, uint32> joints;
					for (const auto& input : skin.joitns_.inputs_)
					{
						if (input.semantic == "JOINT")
						{
							const auto& data = skin.sources_[GameEngine::Collada::GetSource(input.sourceId)].dataArray;
							if (data.type == "Name_array")
							{
								joints = Utils::SplitStringWithId(data.data, ' ');
							}
							break;
						}
					}
					CreateSkeleton(node, mesh->rootJoint_, joints);
					FillAnimator(mesh->animator_, joints);
				}

			}
		}
	}
	void ColladaDae::NewGeometry(const GameEngine::Collada::Geometry& geomtery)
	{
		objects.emplace_back();
		objects.back().transformMatrix = correction_matrix;
		for (const auto& mesh : geomtery.meshes_)
		{
			NewMesh(mesh, geomtery.name_);
		}
	}
	void ColladaDae::NewMesh(const GameEngine::Collada::Mesh& mesh, const std::string& geometryName)
	{
		for (const auto& pair : mesh.polylist_)
		{
			auto& polyList = pair.second;

			objects.back().meshes.emplace_back();
			auto& newMesh = objects.back().meshes.back();
			auto meshSource = GameEngine::Collada::GetSource(polyList.materialId_);
			ApplyMaterials(newMesh.material, meshSource);
			idToMeshMap_[geometryName] = &newMesh;

			for (const auto& input : polyList.inputs_)
			{
				PrepareMeshData(mesh, newMesh, input);
			}

			uint32 i = 0;
			uint32 vbId = 0;
			for (auto vc : polyList.vcount_)
			{
				for (uint32 x = 0; x < vc; ++x)
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
							newMesh.vertexPlacesInVertexBuffer_[value].push_back(vbId);
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
					++vbId;
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

	void ConvertJoint(const GameEngine::Collada::Node & node, GameEngine::Animation::Joint& joint)
	{
		joint.name = node.name_;
		if (node.matrix_)
		{
			joint.transform = node.matrix_.constValue().matrix_;
			//joint.invTransform = glm::inverse(joint.transform);
		}
	}
	void ColladaDae::CreateSkeleton(const GameEngine::Collada::Node & node, GameEngine::Animation::Joint & joint, const std::unordered_map<std::string, uint32>& joints)
	{
		if (joints.count(node.sid_) != 0)
			joint.id = joints.at(node.sid_);

		joint.name = node.sid_;
		if (node.matrix_)
		{
			joint.transform = node.matrix_.constValue().matrix_;
			//joint.invTransform = glm::inverse(joint.transform);
		}

		for (const auto& pair : node.children_)
		{
			if (pair.second == nullptr)
				continue;

			auto& child = *pair.second;
			joint.children.emplace_back();
			CreateSkeleton(child, joint.children.back(), joints);
		}
	}

	bool ContainJoint(const std::string& str1, const std::string& str2)
	{
		for (uint16 x = 0; x < str1.size(); ++x)
		{
			uint16 y = x;
			bool is = true;
			for (auto c2 : str2)
			{
				if (y >= str1.size())
					return false;

				if (str1[y++] != c2)
				{
					is = false;
					break;
				}
			}
			if (is)
				return true;
		}
		return false;
	}

	void ColladaDae::FillAnimator(GameEngine::Animation::Animator& animator, const std::unordered_map<std::string, uint32 >& joints)
	{
		std::vector<GameEngine::Animation::KeyFrame> frames;

		bool isSetTimeStamp = false;

		for (const auto& anim_pair : data_.libraryAnimations_.animations_)
		{
			auto& anim = anim_pair.second;
			auto& animName = anim_pair.first;

			for (const auto& joint : joints)
			{
				if (ContainJoint(animName, joint.first))
				{
					for (const auto& input : anim.sampler_.inputs_)
					{
						if (input.semantic == "INPUT" && !isSetTimeStamp)
						{
							auto sourceId = GameEngine::Collada::GetSource(input.sourceId);
							if (anim.sources_.count(sourceId) != 0)
							{
								auto& source = anim.sources_.at(sourceId);
								auto floatData = GameEngine::Collada::GetFloatsFromString(source.dataArray.data);

								uint16 frameId = 0;
								for (auto frameTime : floatData)
								{
									if (frames.size() <= frameId)
										frames.emplace_back();
									frames[frameId++].timeStamp = frameTime;
								}
								isSetTimeStamp = true;
							}
						}
						else if (input.semantic == "OUTPUT")
						{
							auto sourceId = GameEngine::Collada::GetSource(input.sourceId);
							if (anim.sources_.count(sourceId) != 0)
							{
								auto& source = anim.sources_.at(sourceId);
								auto matrixes = GameEngine::Collada::GetMatrixesFromString(source.dataArray.data);


								uint16 frameId = 0;
								for (auto& mat : matrixes)
								{
									if (frames.size() <= frameId)
										frames.emplace_back();

									vec3 position(mat[3][0], mat[3][1], mat[3][2]);
									auto rotation = glm::quat_cast(mat);

									frames[frameId].transforms[joint.first].position = position;
									frames[frameId].transforms[joint.first].rotation = rotation;

									frames[frameId].idTransforms_[joint.second].position = position;
									frames[frameId].idTransforms_[joint.second].rotation = rotation;
									++frameId;
								}
							}
						}
					}
					break;
				}
			}
		}

		for (const auto& frame : frames)
			animator.animations_["Animiation_0"].AddFrame(frame);
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
	GameEngine::Collada::ObjectType ColladaDae::GetType(const std::string& name)
	{
		if (data_.libraryGeometries_.geometries_.count(name) != 0)
			return GameEngine::Collada::ObjectType::LIBRARY_GOMETRIES;

		if (data_.libraryControllers_.controllers_.count(name) != 0)
			return GameEngine::Collada::ObjectType::LIBRARY_CONTROLLERS;

		return {};
	}
	const GameEngine::Collada::Node& ColladaDae::GetNode(const std::string& node_id)
	{
		for (const auto& vs : data_.libraryVisualScenes_.visualScenes_)
		{
			for (const auto& node : vs.second.nodes_)
			{
				if (node.second.id_ == node_id)
				{
					return node.second;
				}
				else
				{
					for (const auto& child : node.second.children_)
					{
						if (child.second->id_ == node_id)
							return *child.second;

						return GetNode( *(child.second), node_id);
					}
				}
			}
		}

		return staticNode;
	}
	const GameEngine::Collada::Node & ColladaDae::GetNode(const GameEngine::Collada::Node & node, const std::string & node_id)
	{
		if (node.children_.count(node_id) != 0)
		{
			return *node.children_.at(node_id);
		}

		for (const auto& p : node.children_)
		{
			return GetNode(*p.second, node_id);
		}

		return staticNode;
	}
}
