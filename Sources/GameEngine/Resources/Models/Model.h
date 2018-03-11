#pragma once
#include "Mesh.h"
#include "../OpenGLObject.h"
#include <list>
#include <memory>

class CModel : public COpenGLObject
{
public:
	CModel();
    CModel(const CModel& ) = delete;

	virtual ~CModel();
	virtual void InitModel(const std::string&  file_name);
	virtual void OpenGLLoadingPass() override;

	CMesh* AddMesh(CMesh& mesh);

	inline bool operator==(const CModel &q) const;
	inline bool operator==(const std::string &file) const;

	inline const std::string& GetFileName() const;
	inline const std::list<CMesh>& GetMeshes() const;

    std::list<CMesh> meshes;
protected:
    std::string filename;
};

typedef CModel* ModelRawPtr;
typedef std::shared_ptr<CModel> ModelPtr;

const std::list<CMesh>& CModel::GetMeshes() const
{ 
	return meshes;
}
bool CModel::operator==(const CModel &q) const
{ 
	return filename.compare(q.filename) == 0;
}
bool CModel::operator==(const std::string &file) const
{ 
	return filename.compare(file) == 0;
}
const std::string& CModel::GetFileName() const
{ 
	return filename;
}