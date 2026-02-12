#include <GameEngine/Objects/Prefab.h>
#include <GameEngine/Resources/File.h>
#include <Logger/Log.h>
#include <Utils/Json/JsonUtils.h>
#include <Utils/XML/XMLUtils.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

#include "WxEditor/ProjectManager.h"
#include "WxEditor/WxHelpers/EditorUitls.h"

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
    return file.IsFormat("prefab") or
           (file.IsFormat("xml") and Utils::CheckXmlObjectType(file.GetAbsolutePath().string(), "prefab"));
}

bool isMaterial(const GameEngine::File& file)
{
    return file.IsFormat("material") or
           (file.IsFormat("json") and Utils::isJsonFileOfType(file.GetAbsolutePath().string(), "Material"));
}

wxString GetParentPath(const wxString& currentFolderPath)
{
    wxFileName fn(currentFolderPath);
#if defined(_WIN32)
#define MY_PATH_NORMALIZE_FLAGS (wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE)
#else
#define MY_PATH_NORMALIZE_FLAGS (wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE | wxPATH_NORM_TILDE)
#endif

    fn.Normalize(MY_PATH_NORMALIZE_FLAGS);
    return fn.GetPath();
}

void runAnimationViewer(const std::string& extraParam)
{
    std::string cmd = "\"" + wxStandardPaths::Get().GetExecutablePath().ToStdString() + "\" --animationViewer " +
                      "--projectPath " + ProjectManager::GetInstance().GetProjectPath().string() + " " + extraParam;

    LOG_DEBUG << cmd;
    long pid = wxExecute(cmd, wxEXEC_ASYNC | wxEXEC_NOHIDE | wxEXEC_NODISABLE);
    if (pid == 0)
    {
        wxLogError("Run AnimationViewer error!");
        return;
    }
    LOG_DEBUG << "AnimationViewer started PID=" << pid;
}

void runMaterialEditor(const std::string& extraParam)
{
    std::string cmd = "\"" + wxStandardPaths::Get().GetExecutablePath().ToStdString() + "\" --materialEditor " +
                      "--projectPath " + ProjectManager::GetInstance().GetProjectPath().string() + " " + extraParam;

    LOG_DEBUG << cmd;
    long pid = wxExecute(cmd, wxEXEC_ASYNC | wxEXEC_NOHIDE | wxEXEC_NODISABLE);
    if (pid == 0)
    {
        wxLogError("Run Material editor error!");
        return;
    }
    LOG_DEBUG << "Material editor started PID=" << pid;
}

bool isGameObjectPrefab(const GameEngine::GameObject& go)
{
    std::function<bool(const GameEngine::GameObject&)> checkPrefab;
    checkPrefab = [&checkPrefab](const GameEngine::GameObject& obj) -> bool
    {
        if (dynamic_cast<const GameEngine::Prefab*>(&obj) != nullptr)
            return true;

        if (obj.GetParent() != nullptr)
            return checkPrefab(*obj.GetParent());

        return false;
    };

    return checkPrefab(go);
}

std::unique_ptr<wxTextEntryDialog> createEntryDialogWithSelectedText(wxWindow* parent, const wxString& message,
                                                                     const wxString& caption, const wxString& value, long style,
                                                                     const wxPoint& pos)
{
    auto dlg = std::make_unique<wxTextEntryDialog>(parent, message, caption, value, style, pos);

    dlg->Bind(wxEVT_INIT_DIALOG,
              [&](wxInitDialogEvent&)
              {
                  wxTextCtrl* txt = nullptr;
                  for (wxWindow* w : dlg->GetChildren())
                  {
                      if ((txt = wxDynamicCast(w, wxTextCtrl)))
                          break;
                  }
                  if (txt)
                  {
                      txt->SetSelection(-1, -1);
                      dlg->CallAfter([txt]() { txt->SetFocus(); });
                  }
              });

    return dlg;
}