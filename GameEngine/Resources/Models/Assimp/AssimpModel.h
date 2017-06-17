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
	static void		ReadCollisions(std::string file_name, std::vector<float>& postions, std::vector<float>& normals, std::vector<uint32>& indices);

private:
	static void RecursiveProcess(const aiScene *scene, aiNode *node, std::vector<float>& postions, std::vector<float>& normals, std::vector<uint32>& indices);
	void ProcessMesh(std::string file_path, aiMesh* mesh, const aiScene* scene);
	void RecursiveProcess(std::string file_path, aiNode *node, const aiScene *scene);
	
	static mat4 ToGlmMatrix(const aiMatrix4x4& assimp_atrix);
	static mat4 ToGlmMatrix(const aiMatrix3x3& assimp_atrix);
	
	virtual void SetTime(const float& time) override;
	virtual void Update(float delta_time) override;
	virtual const std::vector<mat4>* GetBonesTransforms(uint32 mesh_id) override;

	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
	uint32 FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint32 FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint32 FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

	void BoneTransform(uint32 num_bones, std::vector<SBoneInfo>& bone_info, std::map<std::string, uint32>& bone_mapping, float TimeInSeconds, std::vector<mat4>& Transforms);
	void ReadNodeHeirarchy(std::map<std::string, uint32>& bone_mapping, std::vector<SBoneInfo>& bone_info, float AnimationTime, const aiNode* pNode, const mat4& ParentTransform);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

private:
    CTextureLoader&	textureLodaer;
    float currentTime;
    const aiScene* scene;
    bool isInit;

    std::vector<SBonesInfo> bonesInfo;
    mat4 globalInverseTransform;
    std::vector<mat4> boneTransformMatrixes[50];

    Assimp::Importer importer;
};
