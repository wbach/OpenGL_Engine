#include "MainReader.h"
#include "GeometryReader.h"
#include "libraryEffectsReader.h"
#include "LibraryImagesReader.h"
#include "MaterialReader.h"
#include "XML/XMLUtils.h"
#include <rapidxml.hpp>
#include "Logger/Log.h"
#include "Utils.h"

namespace GameEngine
{
	namespace Collada
	{
		void ReadCollada(const std::string & filename, ColladaData& colladaData)
		{
			auto fileData = Utils::ReadFile(filename);

			if (fileData.empty())
				return;

			rapidxml::xml_document<> document;

			try
			{
				document.parse<0>(const_cast<char*>(fileData.c_str()));
			}
			catch (...)
			{
				Error("Can not parse file " + filename);
				return;
			}
			auto root = document.first_node();
			Utils::ForEachSubNode(root, [&](const Utils::RapidNodeData & data, XMLNode* node)
			{
				if (data.name == "library_geometries")
				{
					LibraryGeometriesReader(colladaData.libraryGeometries_).readGeometries(node);
				}
				else if (data.name == "library_materials")
				{
					LibraryMaterialReader(colladaData.libraryMaterials_).read(node);
				}
				else if (data.name == "library_effects")
				{
					LibraryEffectsReader(colladaData.libraryEffects_).read(node);
				}
				else if (data.name == "library_images")
				{
					LibraryImagesReader(colladaData.libraryImages_).read(node);
				}
			});
		}
	} // Collada
} // GameEngine