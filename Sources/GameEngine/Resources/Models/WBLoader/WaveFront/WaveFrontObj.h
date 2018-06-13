#pragma once
#include "../AbstractLoader.h"
#include <list>

class CTextureLoader;

namespace WBLoader
{
	class WaveFrontObjLoader : public AbstractLoader
	{
	public:
		WaveFrontObjLoader(CTextureLoader& textureLodaer);
		virtual void ParseFile(const std::string& filename) override;
		virtual bool CheckExtension(const std::string& filename) override;

	private:
		void GetFileData(const std::string& file_name);
		void ProcessFileData();
		void ReadMaterialFile(const std::string&file);
		void ProcesVertex(WBLoader::Object* object, const std::string& value);
		void ProcesNormal(WBLoader::Object* object, const std::string& value);
		void ProcesUVs(WBLoader::Object* object, const std::string& value);
		void ProcesFace(WBLoader::Object* object, WBLoader::Mesh* mesh, const std::string& value);
		void ProcesObject(WBLoader::Object*& object, const std::string& value);
		void ProcesMaterial(WBLoader::Object* object, WBLoader::Mesh*& mesh, const std::string& value);
		void Clear();

	private:
		std::list<std::string> fileData;
		std::string path;
		std::string filename;
		
		std::list<SMaterial> materials;
		std::vector<vec3> vertex;
		std::vector<vec2> text_coords;
		std::vector<vec3> normals;
	};
}