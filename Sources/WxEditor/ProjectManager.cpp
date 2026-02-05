#include "ProjectManager.h"

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <filesystem>

#include "Engine/Configuration.h"
#include "Engine/ConfigurationReader.h"
#include "Logger/Log.h"

ProjectManager& ProjectManager::GetInstance()
{
    static ProjectManager instance;
    return instance;
}

void ProjectManager::SetProjectPath(const std::filesystem::path& path)
{
    projectPath                  = path;
    lastOpenedPath               = path;
    projectEditorConfigFilePath  = GameEngine::getGlobalConfigDirPath() / "editorConfig.json";
    projectEditorFilePath        = projectPath / "editor.json";
    projectLocalConfigPath       = projectPath / "config.xml";
    projectScenesFactoryFilePath = path / "scenes.json";
    projectScenesDirPath         = path / "Scenes";
    projectDataDirPath           = path / "Data";
    projectComponentsDirPath     = projectDataDirPath / "Components";

    CreateDirectories();

    LOG_DEBUG << "projectEditorConfigFilePath : " << projectEditorConfigFilePath;
    LOG_DEBUG << "projectEditorFilePath : " << projectEditorFilePath;

    GameEngine::ReadConfigFromFile(EngineLocalConf, projectLocalConfigPath);
    EngineLocalConf.files.setProjectPath(projectPath);

    bool needUpdate{false};

    if (EngineLocalConf.files.getDataPath() != projectDataDirPath)
    {
        EngineLocalConf.files.setDataPath(projectDataDirPath);
        needUpdate = true;
    }

    if (needUpdate)
    {
        GameEngine::WriteConfigurationToFile(EngineConf);
    }

    if (std::filesystem::exists(projectEditorConfigFilePath))
    {
        ReadEditorConfig();
        EngineLocalConf.files.setShaderPath(engineIncludesDir / "Sources");
    }
    else
    {
        wxDirDialog dirDlg(nullptr, "Shaders files and components compilation needs sources from engine. Choose engine folder:",
                           projectPath.string(), wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

        if (dirDlg.ShowModal() == wxID_OK)
        {
            engineIncludesDir = dirDlg.GetPath().ToStdString();
            auto path         = engineIncludesDir / "Sources";
            EngineLocalConf.files.setShaderPath(path);
            LOG_DEBUG << "Path :" << path;
        }
        else
        {
            wxMessageBox(
                "Shaders files location is set by default in project dir. EngineIncludesDir is not set, comilation of "
                "components will not work. You can change it in options frame.",
                "Information", wxOK | wxICON_INFORMATION);
        }

        SaveEditorConfig();
    }

    if (std::filesystem::exists(projectEditorFilePath))
    {
        ReadEditorFile();
    }
    else
    {
        SaveEditor();
    }
}

void ProjectManager::CreateDirectories()
{
    Utils::CreateDirectories(projectPath);
    Utils::CreateDirectories(projectScenesDirPath);
    Utils::CreateDirectories(projectDataDirPath);
    Utils::CreateDirectories(projectComponentsDirPath);
}

const std::filesystem::path& ProjectManager::GetProjectPath() const
{
    return projectPath;
}
const std::filesystem::path& ProjectManager::GetComponentsDirPath() const
{
    return projectComponentsDirPath;
}

void ProjectManager::SetProjectName(const std::string& name)
{
    projectName = name;
}
const std::string& ProjectManager::GetProjectName() const
{
    return projectName;
}

const std::filesystem::path& ProjectManager::GetLastOpenedPath() const
{
    return lastOpenedPath;
}

void ProjectManager::AddScene(const std::string& scene, const std::string& file)
{
    scenes.insert({scene, file});
    SaveSceneFiles();
}

void ProjectManager::SetSenePath(const std::string& scene, const std::filesystem::path& file)
{
    scenes[scene] = file;
    SaveSceneFiles();
}

void ProjectManager::RemoveScene(const std::string& scene)
{
    scenes.erase(scene);
    SaveSceneFiles();
}

const std::unordered_map<std::string, std::filesystem::path>& ProjectManager::GetScenes() const
{
    return scenes;
}

void ProjectManager::Clear()
{
    projectPath.clear();
    projectName.clear();
    scenes.clear();
}

const std::filesystem::path& ProjectManager::GetLocalConfigFile() const
{
    return projectLocalConfigPath;
}
const std::filesystem::path& ProjectManager::GetScenesFactoryFile() const
{
    return projectScenesFactoryFilePath;
}
const std::filesystem::path& ProjectManager::GetScenesDir() const
{
    return projectScenesDirPath;
}
const std::filesystem::path& ProjectManager::GetDataDir() const
{
    return projectDataDirPath;
}
const std::filesystem::path& ProjectManager::GetEngineIncludesDir() const
{
    return engineIncludesDir;
}
std::filesystem::path& ProjectManager::GetEngineIncludesDir()
{
    return engineIncludesDir;
}
void ProjectManager::SetEngineIncludesDir(const std::filesystem::path& dir)
{
    engineIncludesDir = dir;
}
void ProjectManager::SetStartupScene(const std::string& name)
{
    startupscene = name;
    SaveSceneFiles();
}
const std::string& ProjectManager::GetStartupScene() const
{
    return startupscene;
}
void ProjectManager::SaveSceneFiles()
{
    GameEngine::createScenesFile(projectScenesFactoryFilePath, scenes, startupscene);
}
void ProjectManager::SaveEditorConfig()
{
    try
    {
        TreeNode node("editorConfig");
        node.addChild("engineIncludesDir", engineIncludesDir);
        Utils::Json::Write(projectEditorConfigFilePath.string(), node);
        LOG_DEBUG << "projectEditorConfigFilePath saved";
    }
    catch (...)
    {
        LOG_DEBUG << "SaveEditorConfig error. lastOpenedSceneFile =" << lastOpenedSceneFile
                  << ". projectEditorConfigFilePath= " << projectEditorConfigFilePath;
    }
}
void ProjectManager::ReadEditorConfig()
{
    Utils::JsonReader json;
    json.Read(projectEditorConfigFilePath.string());
    if (auto engineIncludesDirNode = json.Get("engineIncludesDir"))
    {
        engineIncludesDir = engineIncludesDirNode->value_;
    }
}
std::vector<ProjectManager::RecentProject> ProjectManager::GetRecentProjects()
{
    wxConfig config("MyEditor");
    std::vector<RecentProject> result;

    long count = config.ReadLong("Recent/Count", 0);
    for (long i = 0; i < count; i++)
    {
        wxString key;
        key.Printf("Recent/Project%ld", i);

        wxString wxPath = config.Read(key, "");
        if (!wxPath.IsEmpty())
        {
            std::string path = wxPath.ToStdString();
            if (!path.empty() && (path.back() == '/' || path.back() == '\\'))
                path.pop_back();

            size_t lastSlash = path.find_last_of("/\\");
            std::string name = (lastSlash != std::string::npos) ? path.substr(lastSlash + 1) : path;

            result.push_back({name, std::filesystem::path(path).make_preferred().string()});
        }
    }
    return result;
}
void ProjectManager::SaveRecentProject(const std::string& path)
{
    wxConfig config("MyEditor");
    auto projects = GetRecentProjects();
    projects.erase(std::remove_if(projects.begin(), projects.end(), [&path](const RecentProject& p) { return p.path == path; }),
                   projects.end());
    projects.insert(projects.begin(), {"", path});
    if (projects.size() > 10)
        projects.resize(10);

    config.Write("Recent/Count", (long)projects.size());
    for (size_t i = 0; i < projects.size(); i++)
    {
        wxString key;
        key.Printf("Recent/Project%zu", i);
        config.Write(key, wxString(projects[i].path));
    }
    config.Flush();
}
void ProjectManager::RemoveRecentProject(const std::string& path)
{
    wxConfig config("MyEditor");
    auto recents = GetRecentProjects();
    recents.erase(std::remove_if(recents.begin(), recents.end(), [&path](const RecentProject& p) { return p.path == path; }),
                  recents.end());

    config.Write("Recent/Count", (long)recents.size());
    for (size_t i = 0; i < recents.size(); i++)
    {
        wxString key;
        key.Printf("Recent/Project%zu", i);
        config.Write(key, wxString(recents[i].path));
    }
    config.Flush();
}
const std::filesystem::path& ProjectManager::GetLastOpenedScene() const
{
    return lastOpenedSceneFile;
}
void ProjectManager::SetLastOpenedSceneFile(const std::filesystem::path& file)
{
    lastOpenedSceneFile = file;
    SaveEditor();
}
void ProjectManager::ReadEditorFile()
{
    LOG_DEBUG << "ReadEditorFile : " << projectEditorFilePath;
    Utils::JsonReader json;
    json.Read(projectEditorFilePath.string());
    if (auto lastOpenedSceneFileNode = json.Get("lastOpenedSceneFile"))
    {
        lastOpenedSceneFile = lastOpenedSceneFileNode->value_;
    }
}
void ProjectManager::SaveEditor()
{
    try
    {
        TreeNode node("editorConfig");
        node.addChild("lastOpenedSceneFile",
                      lastOpenedSceneFile.empty() ? "" : std::filesystem::absolute(lastOpenedSceneFile).lexically_normal());
        Utils::Json::Write(projectEditorFilePath.string(), node);
        LOG_DEBUG << "projectEditorFilePath saved";
    }
    catch (...)
    {
        LOG_DEBUG << "SaveEditorConfig error. lastOpenedSceneFile =" << lastOpenedSceneFile
                  << ". projectEditorFilePath= " << projectEditorFilePath;
    }
}
void ProjectManager::SaveLocalConfigFile()
{
    GameEngine::WriteConfigurationToFile(EngineLocalConf, projectLocalConfigPath);
}
