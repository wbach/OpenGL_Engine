#include "MergeTool.h"
#include <rapidxml.hpp>
#include "GameEngine/Engine/Configuration.h"
#include "Logger/Log.h"
#include "Utils/XML/XMLUtils.h"

using namespace rapidxml;

// std::vector<xml_document<char>> colldadaDocuments;

void GameEngine::ColladaMergeAnimationClips(const std::vector<std::string>& files)
{
    // colldadaDocuments.reserve(files.size());

    for (const auto& filename : files)
    {
        // colldadaDocuments.emplace_back();
        xml_document<char> document;  // = colldadaDocuments.back();

        auto fullFilePath = EngineConf_GetFullDataPathAddToRequierd(filename);
        auto fileData     = Utils::ReadFile(fullFilePath);

        if (fileData.empty())
            return;

        try
        {
            document.parse<0>(const_cast<char*>(fileData.c_str()));
        }
        catch (...)
        {
            Error("Can not parse file " + filename);
            return;
        }
        // auto root = document.first_node();
    }
}
