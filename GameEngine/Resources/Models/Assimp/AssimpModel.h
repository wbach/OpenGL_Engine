#pragma once
#include "../Model.h"
#include "../../TextureLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 

class CAssimModel : public CModel
{
public:
	CAssimModel(CTextureLoader& texture_lodaer);
	virtual ~CAssimModel() override;
	virtual void	InitModel(const std::string& file_name) override;
	static void		ReadCollisions(std::string file_name, std::vector<float>& postions, std::vector<float>& normals, std::vector<uint>& indices);

private:
	static void RecursiveProcess(const aiScene *scene, aiNode *node, std::vector<float>& postions, std::vector<float>& normals, std::vector<uint>& indices);
	void ProcessMesh(std::string file_path, aiMesh* mesh, const aiScene* scene);
	void RecursiveProcess(std::string file_path, aiNode *node, const aiScene *scene);
	
	static glm::mat4 ToGlmMatrix(const aiMatrix4x4& assimp_atrix);
	static glm::mat4 ToGlmMatrix(const aiMatrix3x3& assimp_atrix);
	
	virtual void SetTime(const float& time) override;
	virtual void Update(float delta_time) override;
	virtual const std::vector<glm::mat4>* GetBonesTransforms(uint mesh_id) override;

	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
	uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

	void BoneTransform(uint num_bones, std::vector<SBoneInfo>& bone_info, std::map<std::string, uint>& bone_mapping, float TimeInSeconds, std::vector<glm::mat4>& Transforms);
	void ReadNodeHeirarchy(std::map<std::string, uint>& bone_mapping, std::vector<SBoneInfo>& bone_info, float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

private:
    CTextureLoader&	textureLodaer;
    float currentTime;
    const aiScene* scene;
    bool isInit;

    std::vector<SBonesInfo> bonesInfo;
    glm::mat4 globalInverseTransform;
    std::vector<glm::mat4> boneTransformMatrixes[50];

    Assimp::Importer importer;
};
