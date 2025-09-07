#pragma once
#include <GameEngine/Scene/SceneUtils.h>
#include <Utils/Json/JsonWriter.h>
#include <wx/config.h>

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "Engine/Configuration.h"
#include "Engine/ConfigurationReader.h"
#include "Engine/ConfigurationWriter.h"
#include "Json/JsonReader.h"
#include "Json/JsonWriter.h"
#include "TreeNode.h"

class ProjectManager
{
public:
    struct RecentProject
    {
        std::string name;
        std::string path;
    };

    // Singleton pattern (optional, if you want a single global manager)
    static ProjectManager& GetInstance()
    {
        static ProjectManager instance;
        return instance;
    }

    // Set current project directory
    void SetProjectPath(const std::string& path)
    {
        projectPath                  = path;
        lastOpenedPath               = path;
        projectEditorConfigFilePath  = path + "/editorConfig.json";
        projectConfigPath            = path + "/config.xml";
        projectScenesFactoryFilePath = path + "/scenes.json";
        projectScenesDirPath         = path + "/Scenes/";
        projectDataDirPath           = path + "/Data/";
        projectComponentsDirPath     = projectDataDirPath + "Components/";
        projectCachePath             = path + "/.cache/";

        CreateDirectories();

        GameEngine::ConfigurationReader reader(projectConfigPath);

        bool needUpdate{false};
        if (EngineConf.files.data != projectDataDirPath)
        {
            EngineConf.files.data = projectDataDirPath;
            needUpdate            = true;
        }
        if (EngineConf.files.cache != projectCachePath)
        {
            EngineConf.files.cache = projectCachePath;
            needUpdate             = true;
        }

        if (needUpdate)
        {
            GameEngine::WriteConfigurationToFile(EngineConf);
        }

        if (std::filesystem::exists(projectEditorConfigFilePath))
        {
            ReadEditorConfig();
        }
        else
        {
            SaveEditorConfig();
        }
    }

    void CreateDirectories()
    {
        createDirIfNotExist(projectPath);
        createDirIfNotExist(projectScenesDirPath);
        createDirIfNotExist(projectDataDirPath);
        createDirIfNotExist(projectComponentsDirPath);
        createDirIfNotExist(projectCachePath);
    }

    // Get current project directory
    const std::string& GetProjectPath() const
    {
        return projectPath;
    }

    const std::string& GetComponentsDirPath() const
    {
        return projectComponentsDirPath;
    }

    // Set project name
    void SetProjectName(const std::string& name)
    {
        projectName = name;
    }

    // Get project name
    const std::string& GetProjectName() const
    {
        return projectName;
    }

    // Get last opened path
    const std::string& GetLastOpenedPath() const
    {
        return lastOpenedPath;
    }

    // Add scene to the project
    void AddScene(const std::string& scene, const std::string& file)
    {
        scenes.insert({scene, file});
        SaveSceneFiles();
    }

    void SetSenePath(const std::string& scene, const std::string& file)
    {
        scenes[scene] = file;
        SaveSceneFiles();
    }

    void RemoveScene(const std::string& scene)
    {
        scenes.erase(scene);
        SaveSceneFiles();
    }

    // Get all scenes
    const std::unordered_map<std::string, std::string>& GetScenes() const
    {
        return scenes;
    }

    // Clear all project data (for example, when closing project)
    void Clear()
    {
        projectPath.clear();
        projectName.clear();
        scenes.clear();
    }

    const std::string& GetConfigFile() const
    {
        return projectConfigPath;
    }

    const std::string& GetScenesFactoryFile() const
    {
        return projectScenesFactoryFilePath;
    }

    const std::string& GetScenesDir() const
    {
        return projectScenesDirPath;
    }

    const std::string& GetEngineIncludesDir() const
    {
        return engineIncludesDir;
    }

    std::string& GetEngineIncludesDir()
    {
        return engineIncludesDir;
    }

    void SetEngineIncludesDir(const std::string& dir)
    {
        engineIncludesDir = dir;
    }

    std::vector<RecentProject> GetRecentProjects()
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

                if (not path.empty() && (path.back() == '/' or path.back() == '\\'))
                    path.pop_back();

                // wyciągamy nazwę projektu z folderu
                size_t lastSlash = path.find_last_of("/\\");
                std::string name = (lastSlash != std::string::npos) ? path.substr(lastSlash + 1) : path;

                result.push_back({name, path});
            }
        }
        return result;
    }

    void SaveRecentProject(const std::string& path)
    {
        wxConfig config("MyEditor");

        // Pobieramy ostatnie projekty
        std::vector<RecentProject> projects = GetRecentProjects();

        // Usuń duplikaty po ścieżce
        projects.erase(
            std::remove_if(projects.begin(), projects.end(), [&path](const RecentProject& p) { return p.path == path; }),
            projects.end());

        // Dodaj nowy projekt na początek
        projects.insert(projects.begin(), {"", path});

        // Zachowaj tylko 10 ostatnich
        if (projects.size() > 10)
            projects.resize(10);

        // Zapisz do wxConfig
        config.Write("Recent/Count", (long)projects.size());
        for (size_t i = 0; i < projects.size(); i++)
        {
            wxString key;
            key.Printf("Recent/Project%zu", i);
            config.Write(key, wxString(projects[i].path));
        }

        config.Flush();
    }

    void RemoveRecentProject(const std::string& path)
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

    void SetStartupScene(const std::string& name)
    {
        startupscene = name;
        SaveSceneFiles();
    }

    const std::string& GetStartupScene() const
    {
        return startupscene;
    }

    void SaveSceneFiles()
    {
        GameEngine::createScenesFile(projectScenesFactoryFilePath, scenes, startupscene);
    }

    void SaveEditorConfig()
    {
        TreeNode node("editorConfig");
        node.addChild("engineIncludesDir", engineIncludesDir);
        Utils::Json::Write(projectEditorConfigFilePath, node);
    }

    void ReadEditorConfig()
    {
        Utils::JsonReader json;
        json.Read(projectEditorConfigFilePath);
        if (auto engineIncludesDirNode = json.Get("engineIncludesDir"))
        {
            engineIncludesDir = engineIncludesDirNode->value_;
        }
    }

private:
    ProjectManager()  = default;
    ~ProjectManager() = default;

    // Prevent copying
    ProjectManager(const ProjectManager&)            = delete;
    ProjectManager& operator=(const ProjectManager&) = delete;

    void createDirIfNotExist(const std::string& dir)
    {
        if (not std::filesystem::exists(dir))
        {
            std::filesystem::create_directory(dir);
        }
    }

private:
    std::string projectPath;
    std::string projectConfigPath;
    std::string projectScenesFactoryFilePath;
    std::string projectCachePath;
    std::string projectScenesDirPath;
    std::string projectDataDirPath;
    std::string projectComponentsDirPath;
    std::string projectEditorConfigFilePath;
    std::string lastOpenedPath;
    std::string projectName;
    std::string startupscene;
    std::string engineIncludesDir;
    std::unordered_map<std::string, std::string> scenes;
};
