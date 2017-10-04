#pragma once
#include <list>
#include <string>
#include "../Mesh.h"

namespace WBLoader
{
    class IMeshLoader
    {
    public:
        virtual void ParseFile(const std::string& filename)      = 0;
        virtual std::list<CMesh> CreateFinalMesh()               = 0;
        virtual bool CheckExtension(const std::string& filename) = 0;
    };
} // WBLoader
