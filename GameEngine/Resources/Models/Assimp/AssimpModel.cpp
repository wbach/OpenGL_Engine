#include "AssimpModel.h"
#include "../../../Utils/GLM/GLMUtils.h"

CAssimModel::CAssimModel(CTextureLoader & texture_lodaer)
	: CModel()
	, m_TextureLodaer(texture_lodaer)
	, m_CurrentTime(0)
	, m_Scene(nullptr)
	, m_IsInit(false)
{
}

void CAssimModel::InitModel(const std::string& file_name)
{
	m_Filename = file_name;
	std::replace(m_Filename.begin(), m_Filename.end(), '\\', '/');
	std::string path = m_Filename.substr(0, m_Filename.find_last_of('/'));

	if (!Utils::CheckFileExist(file_name))
	{
		std::string error = "[Error] The file " + m_Filename + " wasnt successfuly opened";
		Log(error);
		throw std::runtime_error(error.c_str());
	}

	uint flags =
		aiProcess_Triangulate | aiProcess_CalcTangentSpace
		| aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices
		| aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes;
	//if(normals == "flat" ) flags |= aiProcess_GenNormals ;
	//if(normals == "smooth" ) flags |= aiProcess_GenSmoothNormals ;
	m_Scene = importer.ReadFile(m_Filename.c_str(), flags);// aiImportFile(m_Filename.c_str(), flags);
	if (m_Scene)
	{
		if (m_Scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !m_Scene->mRootNode)
		{
			std::string error = "[Error] The file " + m_Filename + " wasnt successfuly opened - AI_SCENE_FLAGS_INCOMPLETE";
			throw std::runtime_error(error.c_str());
		}
		aiMatrix4x4 m = m_Scene->mRootNode->mTransformation;
		m_GlobalInverseTransform = ToGlmMatrix(m);
		RecursiveProcess("../Data/Textures/", m_Scene->mRootNode, m_Scene);
		m_IsInit = true;
		//aiReleaseImport(m_Scene);
	}
	else
	{
		std::string error = "Error parsing " + file_name +importer.GetErrorString();
		Log(error);
		throw std::runtime_error(error.c_str());
	}
	CModel::InitModel(file_name);
}

void CAssimModel::ReadCollisions(std::string file_name, std::vector<float>& postions, std::vector<float>& normals, std::vector<uint>& indices)
{
	if (!Utils::CheckFileExist(file_name))
	{
		std::string error = "[Error] The file " + file_name + " wasnt successfuly opened";
		Log(error);
		throw std::runtime_error(error.c_str());
	}

	//Assimp::Importer importer;
	uint flags = aiProcess_Triangulate;

	const aiScene *scene = aiImportFile(file_name.c_str(), flags);
	if (scene)
	{
		if (scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::string error = "[Error] The file " + file_name + " wasnt successfuly opened - AI_SCENE_FLAGS_INCOMPLETE";
			throw std::runtime_error(error.c_str());
		}
		RecursiveProcess(scene, scene->mRootNode, postions, normals, indices);
	}
	else
	{
		std::string error = "Error parsing " + file_name;
		Log(error);
		throw std::runtime_error(error.c_str());
	}
	aiReleaseImport(scene);
}

CAssimModel::~CAssimModel()
{
	if (!m_IsInit)
		return;

	if (m_Scene != nullptr)
	{
		///aiReleaseImport(m_Scene);
	}
	Log("Destructor assimp model : " + m_Filename);
}
void CAssimModel::RecursiveProcess(const aiScene *scene, aiNode * node, std::vector<float>& postions, std::vector<float>& normals, std::vector<uint>& indices)
{
	//proces
	for (uint i = 0; i<node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

		for (uint i = 0; i< mesh->mNumVertices; i++)
		{
			postions.push_back(mesh->mVertices[i].x);
			postions.push_back(mesh->mVertices[i].y);
			postions.push_back(mesh->mVertices[i].z);

			normals.push_back(mesh->mNormals[i].x);
			normals.push_back(mesh->mNormals[i].y);
			normals.push_back(mesh->mNormals[i].z);
		}

		int offset = indices.size();
		for (unsigned short i = 0; i<mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned short j = 0; j<face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j] + offset);
			}
		}
	}
	//r
	for (uint i = 0; i <node->mNumChildren; i++)
	{
		RecursiveProcess(scene, node->mChildren[i], postions, normals, indices);
	}
}
void CAssimModel::ProcessMesh(std::string file_path, aiMesh* mesh, const aiScene* scene)
{
	std::vector<float> postions;
	std::vector<float> text_coords;
	std::vector<float> normals;
	std::vector<float> tangents;
	std::vector<unsigned short> indices;

	m_BonesInfo.push_back(SBonesInfo());
	SBonesInfo& bones_info = m_BonesInfo.back();
	bones_info.bones.resize(mesh->mNumVertices);

	aiColor4D diff;
	aiColor4D amb;
	aiColor4D spec;
	float shine_damper;
	float transparent;
	float reflectivity;
	aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
	aiString name;
	aiGetMaterialString(mat, AI_MATKEY_NAME, &name);
	aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diff);
	aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &amb);
	aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &spec);
	aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &shine_damper);
	aiGetMaterialFloat(mat, AI_MATKEY_REFLECTIVITY, &reflectivity);
	aiGetMaterialFloat(mat, AI_MATKEY_OPACITY, &transparent);

	glm::vec3 defaultDiff(diff.r, diff.g, diff.b);
	glm::vec3 defaultSpec(spec.r, spec.g, spec.b);
	glm::vec3 defaultAmb(amb.r, amb.g, amb.b);

	for (uint i = 0; i< mesh->mNumVertices; i++)
	{

		postions.push_back(mesh->mVertices[i].x);
		postions.push_back(mesh->mVertices[i].y);
		postions.push_back(mesh->mVertices[i].z);


		normals.push_back(mesh->mNormals[i].x);
		normals.push_back(mesh->mNormals[i].y);
		normals.push_back(mesh->mNormals[i].z);


		if (mesh->mTangents)
		{
			tangents.push_back(mesh->mTangents[i].x);
			tangents.push_back(mesh->mTangents[i].y);
			tangents.push_back(mesh->mTangents[i].z);

		}
		else
		{
			tangents.push_back(1.0f);
			tangents.push_back(0.0f);
			tangents.push_back(0.0f);
		}

		if (mesh->mColors[0])
		{
			defaultDiff.x = mesh->mColors[0][i].r;
			defaultDiff.y = mesh->mColors[0][i].g;
			defaultDiff.z = mesh->mColors[0][i].b;
		}

		if (mesh->mTextureCoords[0])
		{
			text_coords.push_back(mesh->mTextureCoords[0][i].x);
			text_coords.push_back(mesh->mTextureCoords[0][i].y);
		}
		else
		{
			text_coords.push_back(0);
			text_coords.push_back(0);
		}
	}

	for (unsigned short i = 0; i<mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned short j = 0; j<face.mNumIndices; j++)
		{
			indices.push_back(static_cast<unsigned short>(face.mIndices[j]));
		}
	}
	if (mesh->mNumBones < 1)
		bones_info.bones.clear();

	for (uint i = 0; i < mesh->mNumBones; i++)
	{
		uint BoneIndex = 0;
		std::string BoneName(mesh->mBones[i]->mName.data);
		//cout << BoneName << endl;
		if (bones_info.bone_mapping.find(BoneName) == bones_info.bone_mapping.end())
		{
			// Allocate an index for a new bone
			BoneIndex = bones_info.num_bones;
			bones_info.num_bones++;
			SBoneInfo bi;
			bones_info.bone_info.push_back(bi);
			bones_info.bone_info[BoneIndex].BoneOffset = ToGlmMatrix(mesh->mBones[i]->mOffsetMatrix);
			bones_info.bone_mapping[BoneName] = BoneIndex;
		}
		else {
			BoneIndex = bones_info.bone_mapping[BoneName];
		}

		for (uint j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{
			uint vertexid = mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;
			bones_info.bones[vertexid].AddBoneData(BoneIndex, weight);
		}
	}
	//for (SVertexBoneData& data : bones_info.bones)
	//{
	//	cout << data.ids[0] << endl;
	//	cout << data.ids[1] << endl;
	//	cout << data.ids[2] << endl;
	//	cout << data.ids[3] << endl;
	//	cout << "*************************" << endl;
	//}
	SMaterial material;
	material.diffuse = defaultDiff;
	material.specular = defaultSpec;
	material.shineDamper = shine_damper;
	material.reflectivity = reflectivity;

	for (uint i = 0; i <mat->GetTextureCount(aiTextureType_DIFFUSE); i++)
	{
		aiString str;
		mat->GetTexture(aiTextureType_DIFFUSE, i, &str);
		std::string s(str.C_Str());
		s = Utils::ConvertToRelativePath(s);
		s = s.substr(s.find_last_of('/') + 1);

		try
		{
			auto texture = m_TextureLodaer.LoadTexture(file_path + s.c_str(), true, TextureType::MATERIAL);
			material.m_DiffuseTexture = texture;
		}
		catch (const std::runtime_error& e)
		{
			throw e;
		}

	}
	for (uint i = 0; i <mat->GetTextureCount(aiTextureType_HEIGHT); i++)
	{
		aiString str;
		mat->GetTexture(aiTextureType_HEIGHT, i, &str);
		try
		{
			auto texture = m_TextureLodaer.LoadTexture(file_path + str.C_Str(), true, TextureType::MATERIAL);
			material.m_NormalTexture = texture;
		}
		catch (const std::runtime_error& e)
		{
			throw e;
		}
	}

	AddMesh(material, postions, text_coords, normals, tangents, indices, bones_info.bones);
}

void CAssimModel::RecursiveProcess(std::string file_path, aiNode *node, const aiScene *scene)
{
	//proces
	for (uint i = 0; i<node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		try
		{
			ProcessMesh(file_path, mesh, scene);
		}
		catch (const std::runtime_error& e)
		{
			throw e;
		}
	}
	//r
	for (uint i = 0; i <node->mNumChildren; i++)
	{
		RecursiveProcess(file_path, node->mChildren[i], scene);
	}
}

glm::mat4 CAssimModel::ToGlmMatrix(const aiMatrix4x4 & assimp_atrix)
{
	glm::mat4 m;
	m[0][0] = assimp_atrix.a1; m[0][1] = assimp_atrix.b1; m[0][2] = assimp_atrix.c1; m[0][3] = assimp_atrix.d1;
	m[1][0] = assimp_atrix.a2; m[1][1] = assimp_atrix.b2; m[1][2] = assimp_atrix.c2; m[1][3] = assimp_atrix.d2;
	m[2][0] = assimp_atrix.a3; m[2][1] = assimp_atrix.b3; m[2][2] = assimp_atrix.c3; m[2][3] = assimp_atrix.d3;
	m[3][0] = assimp_atrix.a4; m[3][1] = assimp_atrix.b4; m[3][2] = assimp_atrix.c4; m[3][3] = assimp_atrix.d4;
	return m;
}

glm::mat4 CAssimModel::ToGlmMatrix(const aiMatrix3x3 & assimp_atrix)
{
	glm::mat4 m;
	m[0][0] = assimp_atrix.a1; m[0][1] = assimp_atrix.b1; m[0][2] = assimp_atrix.c1; m[0][3] = 0;
	m[1][0] = assimp_atrix.a2; m[1][1] = assimp_atrix.b2; m[1][2] = assimp_atrix.c2; m[1][3] = 0;
	m[2][0] = assimp_atrix.a3; m[2][1] = assimp_atrix.b3; m[2][2] = assimp_atrix.c3; m[2][3] = 0;
	m[3][0] = 0;			   m[3][1] = 0;				  m[3][2] = 0;				 m[3][3] = 1;
	return m;
}

void CAssimModel::SetTime(const float & time) { m_CurrentTime = time; }

void CAssimModel::Update(float delta_time)
{
	m_CurrentTime += delta_time;
	//if (m_CurrentTime > 1)
	//	m_CurrentTime = 0;
	int x = 0;
	for (SBonesInfo& info : m_BonesInfo)
	{
		BoneTransform(info.num_bones, info.bone_info, info.bone_mapping, m_CurrentTime, m_BoneTransformMatrixes[x++]);
	}
}

const std::vector<glm::mat4>* CAssimModel::GetBonesTransforms(uint mesh_id) { return  &m_BoneTransformMatrixes[mesh_id]; }

void CAssimModel::BoneTransform(uint num_bones, std::vector<SBoneInfo>& bone_info, std::map<std::string, uint>& bone_mapping, float TimeInSeconds, std::vector<glm::mat4>& Transforms)
{
	glm::mat4 Identity(1.0f);

	float TicksPerSecond = (float)m_Scene->mAnimations[0]->mTicksPerSecond != 0 ?
		(float)m_Scene->mAnimations[0]->mTicksPerSecond : 25.0f;
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = (float)fmod(TimeInTicks, m_Scene->mAnimations[0]->mDuration);

	ReadNodeHeirarchy(bone_mapping, bone_info, AnimationTime, m_Scene->mRootNode, Identity);

	//	Transforms.resize(num_bones);

	for (uint i = 0; i < num_bones; i++)
	{
		if (Transforms.size() < i + 1) Transforms.push_back(glm::mat4(1.f));
		Transforms[i] = bone_info[i].FinalTransformation;
	}
}

void CAssimModel::ReadNodeHeirarchy(std::map<std::string, uint>& bone_mapping, std::vector<SBoneInfo>& bone_info, float AnimationTime, const aiNode * pNode, const glm::mat4 & ParentTransform)
{
	std::string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = m_Scene->mAnimations[0];

	glm::mat4 NodeTransformation = ToGlmMatrix(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim)
	{
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		glm::mat4 ScalingM(1.f);
		ScalingM = glm::scale(Scaling.x, Scaling.y, Scaling.z);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		glm::mat4 RotationM = glm::mat4(1.f);//glm::mat4(RotationQ.GetMatrix());
		RotationM = ToGlmMatrix(RotationQ.GetMatrix());
		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation(1.f);
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		glm::mat4 TranslationM(1.f);
		TranslationM = glm::translate(glm::vec3(Translation.x, Translation.y, Translation.z));

		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}

	glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

	if (bone_mapping.find(NodeName) != bone_mapping.end())
	{
		uint BoneIndex = bone_mapping[NodeName];
		bone_info[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation *
			bone_info[BoneIndex].BoneOffset;
	}

	for (uint i = 0; i < pNode->mNumChildren; i++)
	{
		ReadNodeHeirarchy(bone_mapping, bone_info, AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}

void CAssimModel::CalcInterpolatedPosition(aiVector3D & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	unsigned NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void CAssimModel::CalcInterpolatedRotation(aiQuaternion & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	uint NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}

void CAssimModel::CalcInterpolatedScaling(aiVector3D & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	uint NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

const aiNodeAnim * CAssimModel::FindNodeAnim(const aiAnimation * pAnimation, const std::string NodeName)
{
	for (uint i = 0; i < pAnimation->mNumChannels; i++)
	{
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == NodeName)
		{
			return pNodeAnim;
		}
	}
	return nullptr;
}

uint CAssimModel::FindPosition(float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
	{
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
		{
			return i;
		}
	}
	assert(0);
	return 0;
}

uint CAssimModel::FindRotation(float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);
	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
	{
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
		{
			return i;
		}
	}

	assert(0);
	return 0;
}

uint CAssimModel::FindScaling(float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
	{
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
		{
			return i;
		}
	}
	assert(0);
	return 0;
}
