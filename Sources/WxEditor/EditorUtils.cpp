#include <Utils/XML/XMLUtils.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

#include "EditorUitls.h"
#include "Logger/Log.h"
#include "ProjectManager.h"

wxImage TrimMargins(const wxImage& img)
{
    if (!img.IsOk())
        return img;

    if (!img.HasAlpha())
        return img;  // bez alfy nie wiemy co jest tlem - mozna dodac heurystyke

    int minX = img.GetWidth(), minY = img.GetHeight();
    int maxX = 0, maxY = 0;

    for (int y = 0; y < img.GetHeight(); ++y)
    {
        for (int x = 0; x < img.GetWidth(); ++x)
        {
            if (img.GetAlpha(x, y) > 0)  // piksel widoczny
            {
                if (x < minX)
                    minX = x;
                if (y < minY)
                    minY = y;
                if (x > maxX)
                    maxX = x;
                if (y > maxY)
                    maxY = y;
            }
        }
    }

    if (minX > maxX || minY > maxY)
        return img;  // wszystko przezroczyste, nic nie tniemy

    return img.GetSubImage(wxRect(minX, minY, maxX - minX + 1, maxY - minY + 1));
}

wxImage createImage(const unsigned char* data, unsigned int len, const wxSize& size)
{
    auto newBmp = wxBitmap::NewFromPNGData(data, len);
    wxImage img = newBmp.ConvertToImage();
    img         = TrimMargins(img);
    img.Rescale(size.x, size.y, wxIMAGE_QUALITY_HIGH);
    return img;
}

bool is3dModelFile(const std::string& p)
{
    return is3dModelFile(GameEngine::File(p));
}

bool is3dModelFile(const std::filesystem::path& p)
{
    return is3dModelFile(GameEngine::File(p));
}
bool is3dModelFile(const GameEngine::File& file)
{
    return file.IsFormat({"AMF", "3DS",      "AC",      "ASE", "ASSBIN", "B3D",  "BVH",   "COLLADA", "DXF", "CSM",
                          "DAE", "HMP",      "IRRMESH", "IRR", "LWO",    "LWS",  "MD2",   "MD3",     "MD5", "MD5MESH",
                          "MDC", "MDL",      "NFF",     "NDO", "OFF",    "OBJ",  "OGRE",  "OPENGEX", "PLY", "MS3D",
                          "COB", "BLEND",    "IFC",     "XGL", "FBX",    "Q3D",  "Q3BSP", "RAW",     "SIB", "SMD",
                          "STL", "TERRAGEN", "3D",      "X",   "X3D",    "GLTF", "3MF",   "MMD",     "STEP"});
}

bool isPrefab(const GameEngine::File& file)
{
    return file.IsFormat("prefab") or Utils::CheckXmlObjectType(file.GetAbsolutePath().string(), "prefab");
}

wxString GetParentPath(const wxString& currentFolderPath)
{
    wxFileName fn(currentFolderPath);
    fn.Normalize();
    return fn.GetPath();
}

void runAnimationViewer(const std::string& extraParam)
{
    std::string cmd = "\"" + wxStandardPaths::Get().GetExecutablePath().ToStdString() + "\" --animationViewer " +
                      "--projectPath " + ProjectManager::GetInstance().GetProjectPath() + " " + extraParam;

    long pid = wxExecute(cmd, wxEXEC_ASYNC | wxEXEC_NOHIDE | wxEXEC_NODISABLE);
    if (pid == 0)
    {
        wxLogError("Run AnimationViewer error!");
        return;
    }
    LOG_DEBUG << "AnimationViewer started PID=" << pid;
}