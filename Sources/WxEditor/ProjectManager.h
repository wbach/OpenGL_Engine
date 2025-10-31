#pragma once

#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Engine/ConfigurationReader.h>
#include <GameEngine/Engine/ConfigurationWriter.h>
#include <GameEngine/Scene/SceneUtils.h>
#include <Utils/Json/JsonReader.h>
#include <Utils/Json/JsonWriter.h>
#include <Utils/TreeNode.h>
#include <wx/config.h>
#include <wx/wx.h>

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

class ProjectManager
{
public:
    struct RecentProject
    {
        std::string name;
        std::string path;
    };

    static ProjectManager& GetInstance();

    void SetProjectName(const std::string&);
    void SetEngineIncludesDir(const std::filesystem::path&);
    void SetProjectPath(const std::filesystem::path&);
    void CreateDirectories();

    void AddScene(const std::string& scene, const std::string&);
    void SetSenePath(const std::string& scene, const std::filesystem::path&);
    void RemoveScene(const std::string& scene);
    const std::unordered_map<std::string, std::filesystem::path>& GetScenes() const;

    void Clear();

    const std::string& GetProjectName() const;
    const std::filesystem::path& GetProjectPath() const;
    const std::filesystem::path& GetComponentsDirPath() const;
    const std::filesystem::path& GetLastOpenedPath() const;
    const std::filesystem::path& GetConfigFile() const;
    const std::filesystem::path& GetScenesFactoryFile() const;
    const std::filesystem::path& GetScenesDir() const;
    const std::filesystem::path& GetDataDir() const;
    const std::filesystem::path& GetEngineIncludesDir() const;
    std::filesystem::path& GetEngineIncludesDir();

    std::vector<RecentProject> GetRecentProjects();
    void SaveRecentProject(const std::string& path);
    void RemoveRecentProject(const std::string& path);

    void SetStartupScene(const std::string& name);
    const std::string& GetStartupScene() const;

    void SaveSceneFiles();
    void SaveEditorConfig();
    void ReadEditorConfig();

private:
    ProjectManager()  = default;
    ~ProjectManager() = default;

    ProjectManager(const ProjectManager&)            = delete;
    ProjectManager& operator=(const ProjectManager&) = delete;

    void createDirIfNotExist(const std::filesystem::path&);

private:
    std::filesystem::path projectPath;
    std::filesystem::path projectConfigPath;
    std::filesystem::path projectScenesFactoryFilePath;
    std::filesystem::path projectCachePath;
    std::filesystem::path projectScenesDirPath;
    std::filesystem::path projectDataDirPath;
    std::filesystem::path projectComponentsDirPath;
    std::filesystem::path projectEditorConfigFilePath;
    std::filesystem::path lastOpenedPath;
    std::string projectName;
    std::string startupscene;
    std::filesystem::path engineIncludesDir;
    std::unordered_map<std::string, std::filesystem::path> scenes;
};
