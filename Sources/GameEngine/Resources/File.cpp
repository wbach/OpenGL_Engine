#include "File.h"

#include <Logger/Log.h>
#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <filesystem>

#include "GameEngine/Engine/Configuration.h"

namespace GameEngine
{
File::File()
    : fp_{nullptr}
    , fileSize_(0)
{
}

File::File(const std::string &input)
    : fp_{nullptr}
    , fileSize_(0)
{
    if (Utils::IsAbsolutePath(input))
        AbsoultePath(input);
    else if (IsProjectRelativePath(input))
        ProjectRelative(input);
    else
        DataRelative(input);
}
File::File(const char *input)
    : File(std::string(input))
{
}
void File::DataRelative(const std::string &filename)
{
    auto dataRelative    = filename;
    auto projectRelative = EngineConf_GetFullDataPath(filename);

    std::string absoultePath;

    if (std::filesystem::exists(projectRelative))
    {
        absoultePath = Utils::GetAbsolutePath(projectRelative);
    }
    else
    {
        auto parentPath = Utils::GetAbsolutePath(Utils::GetParent(projectRelative));

        if (not std::filesystem::exists(parentPath))
        {
            try
            {
                std::filesystem::create_directories(parentPath);
            }
            catch (const std::exception &e)
            {
                ERROR_LOG(e.what());
            }
        }

        absoultePath = parentPath + "/" + std::filesystem::path(filename).filename().string();
    }
    ConvertSlashesAndAddToRequired(absoultePath, dataRelative, absoultePath);
}

void File::ProjectRelative(const std::string &filename)
{
    auto dataRelative    = GetRelativeDataPath(filename);
    auto projectRelative = filename;
    auto absoultePath    = Utils::GetAbsolutePath(projectRelative);
    ConvertSlashesAndAddToRequired(absoultePath, dataRelative, absoultePath);
}

void File::AbsoultePath(const std::string &filename)
{
    auto absoultePath    = filename;
    auto dataRelative    = GetRelativeDataPath(filename);
    auto projectRelative = EngineConf_GetFullDataPath(dataRelative_);
    ConvertSlashesAndAddToRequired(absoultePath, dataRelative, absoultePath);
}

void File::ChangeExtension(const std::string &extension)
{
    auto absoultePath    = std::filesystem::path(absoultePath_).replace_extension(extension).string();
    auto dataRelative    = std::filesystem::path(dataRelative_).replace_extension(extension).string();
    auto projectRelative = std::filesystem::path(projectRelative_).replace_extension(extension).string();
    ConvertSlashes(absoultePath, dataRelative, projectRelative);
}

const std::string &File::GetDataRelativeDir() const
{
    return dataRelative_;
}

const std::string &File::GetProjectRelativeDir() const
{
    return projectRelative_;
}

const std::string &File::GetAbsoultePath() const
{
    return absoultePath_;
}

std::string File::GetAbsolutePathWithDifferentExtension(const std::string &extension) const
{
    return std::filesystem::path(absoultePath_).replace_extension(extension).string();
}

std::string File::GetBaseName() const
{
    return std::filesystem::path(dataRelative_).stem().string();
}

std::string File::GetExtension() const
{
    return std::filesystem::path(dataRelative_).extension().string();
}

std::string File::GetFilename() const
{
    return std::filesystem::path(dataRelative_).filename().string();
}

std::string File::GetParentDir() const
{
    return Utils::GetParent(absoultePath_);
}

File File::CreateFileWithExtension(const std::string &extension) const
{
    File f(*this);
    f.ChangeExtension(extension);
    return f;
}

void File::ChangeFileName(const std::string &filename)
{
    auto absoultePath    = std::filesystem::path(absoultePath_).replace_filename(filename).string();
    auto dataRelative    = std::filesystem::path(dataRelative_).replace_filename(filename).string();
    auto projectRelative = std::filesystem::path(projectRelative_).replace_filename(filename).string();
    ConvertSlashes(absoultePath, dataRelative, projectRelative);
}

void File::ChangeBaseName(const std::string &basename)
{
    auto filenameWithExtension = basename + GetExtension();
    auto absoultePath          = std::filesystem::path(absoultePath_).replace_filename(filenameWithExtension).string();
    auto dataRelative          = std::filesystem::path(dataRelative_).replace_filename(filenameWithExtension).string();
    auto projectRelative = std::filesystem::path(projectRelative_).replace_filename(filenameWithExtension).string();
    ConvertSlashes(absoultePath, dataRelative, projectRelative);
}

void File::AddSuffixToBaseName(const std::string &suffix)
{
    auto newBaseName = GetBaseName() + suffix;
    ChangeBaseName(newBaseName);
}

bool File::IsExtension(const std::string &extension) const
{
    return GetExtension() == extension;
}

bool File::operator==(const File &f) const
{
    return absoultePath_ == f.absoultePath_;
}
File File::operator=(const char *str) const
{
    return File(str);
}
File::operator bool() const
{
    return not absoultePath_.empty() and std::filesystem::exists(absoultePath_);
}
bool File::empty() const
{
    return absoultePath_.empty() or dataRelative_.empty() or projectRelative_.empty();
}
bool File::openToWrite()
{
    if (fp_)
    {
        ERROR_LOG("Can not write to openned file!.");
        return false;
    }

    fp_ = fopen(absoultePath_.c_str(), "wb");

    if (not fp_)
    {
        ERROR_LOG("cannot open file : " + absoultePath_);
        return false;
    }
    return true;
}
bool File::openToRead()
{
    if (fp_)
    {
        ERROR_LOG("Can not open to openned file!.");
        return false;
    }

    if (not std::filesystem::exists(absoultePath_))
    {
        ERROR_LOG("file not exist! " + absoultePath_);
        return false;
    }

    fp_ = fopen(absoultePath_.c_str(), "rb");

    if (not fp_)
    {
        ERROR_LOG("cannot open file : " + absoultePath_);
        return false;
    }

    fseek(fp_, 0L, SEEK_END);
    fileSize_ = ftell(fp_);
    fseek(fp_, 0, SEEK_SET);

    return true;
}
void File::close()
{
    if (fp_)
    {
        fclose(fp_);
    }

    fp_ = nullptr;
}
void File::ConvertSlashes(const std::string &absoultePath, const std::string &dataRelative,
                          const std::string &projectRelative)
{
    absoultePath_    = Utils::ReplaceSlash(absoultePath);
    dataRelative_    = Utils::ReplaceSlash(dataRelative);
    projectRelative_ = Utils::ReplaceSlash(projectRelative);
}
void File::ConvertSlashesAndAddToRequired(const std::string &absoultePath, const std::string &dataRelative,
                                          const std::string &projectRelative)
{
    ConvertSlashes(absoultePath, dataRelative, projectRelative);
    EngineConf_AddRequiredFile(dataRelative_);
}
bool File::IsProjectRelativePath(const std::string &inputpath) const
{
    try
    {
        return (std::filesystem::exists(inputpath) or inputpath.find(EngineConf.files.data) != std::string::npos);
    }
    catch (...)
    {
        return false;
    }
}

void File::printError(const std::string &str) const
{
    ERROR_LOG(str);
}
}  // namespace GameEngine
