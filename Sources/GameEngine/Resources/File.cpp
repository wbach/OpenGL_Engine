#include "File.h"

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <algorithm>
#include <filesystem>
#include <Logger/Log.h>

#include "GameEngine/Engine/Configuration.h"

namespace GameEngine
{
File::File()
    : fp_{nullptr}
    , fileSize_(0)
{
}

File::File(const std::string &input)
    : initValue_(input)
    , fp_{nullptr}
    , fileSize_(0)
{
    Change(input);
}
File::File(const char *input)
    : File(std::string(input))
{
}

void File::Change(const std::string &input)
{
    initValue_ = input;

    if (input.empty())
        return;

    if (Utils::IsAbsolutePath(initValue_))
        AbsoultePath(initValue_);
    else if (IsProjectRelativePath(initValue_))
        ProjectRelative(initValue_);
    else
        DataRelative(initValue_);

    ClearSpecialCharacters();
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
                /// std::filesystem::create_directories(parentPath);
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
    auto absoultePath    = std::filesystem::path(absolutePath_).replace_extension(extension).string();
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

const std::string &File::GetAbsolutePath() const
{
    return absolutePath_;
}

std::string File::GetAbsolutePathWithDifferentExtension(const std::string &extension) const
{
    return std::filesystem::path(absolutePath_).replace_extension(extension).string();
}

const std::string &File::GetInitValue() const
{
    return initValue_;
}

std::string File::GetBaseName() const
{
    return std::filesystem::path(absolutePath_).stem().string();
}

std::string File::GetExtension() const
{
    auto str = std::filesystem::path(absolutePath_).extension().string();
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

std::string File::GetFilename() const
{
    return std::filesystem::path(absolutePath_).filename().string();
}

std::string File::GetParentDir() const
{
    return Utils::GetParent(absolutePath_);
}

File File::CreateFileWithExtension(const std::string &extension) const
{
    File f(*this);
    f.ChangeExtension(extension);
    return f;
}

void File::ChangeFileName(const std::string &filename)
{
    auto absoultePath    = std::filesystem::path(absolutePath_).replace_filename(filename).string();
    auto dataRelative    = std::filesystem::path(dataRelative_).replace_filename(filename).string();
    auto projectRelative = std::filesystem::path(projectRelative_).replace_filename(filename).string();
    ConvertSlashes(absoultePath, dataRelative, projectRelative);
}

void File::ChangeBaseName(const std::string &basename)
{
    auto filenameWithExtension = basename + GetExtension();
    auto absoultePath          = std::filesystem::path(absolutePath_).replace_filename(filenameWithExtension).string();
    auto dataRelative          = std::filesystem::path(dataRelative_).replace_filename(filenameWithExtension).string();
    auto projectRelative       = std::filesystem::path(projectRelative_).replace_filename(filenameWithExtension).string();
    ConvertSlashes(absoultePath, dataRelative, projectRelative);
}

void File::AddSuffixToBaseName(const std::string &suffix)
{
    auto newBaseName = GetBaseName() + suffix;
    ChangeBaseName(newBaseName);
}

std::string getExtensionToCompare(std::string input)
{
    if (input[0] != '.')
        input = '.' + input;
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    return input;
}

bool File::IsExtension(const std::string &extension) const
{
    if (extension.empty())
        return true;

    return GetExtension() == getExtensionToCompare(extension);
}

bool File::IsExtension(const std::vector<std::string> &extensions) const
{
    auto ext = GetExtension();

    return std::any_of(extensions.begin(), extensions.end(), [&ext](const auto &s) { return getExtensionToCompare(s) == ext; });
}

bool File::operator==(const File &f) const
{
    return absolutePath_ == f.absolutePath_;
}
File &File::operator=(const File &f)
{
    if (&f != this)
    {
        Change(f.initValue_);
    }
    return *this;
}
File &File::operator=(const char *str)
{
    Change(str);
    return *this;
}
File &File::operator=(const std::string &str)
{
    Change(str);
    return *this;
}
File::operator bool() const
{
    try
    {
        return not absolutePath_.empty() and std::filesystem::exists(absolutePath_);
    }
    catch (...)
    {
        return false;
    }
}
bool File::empty() const
{
    return initValue_.empty() or absolutePath_.empty() or dataRelative_.empty() or projectRelative_.empty();
}

bool File::exist() const
{
    return not empty() and std::filesystem::exists(GetAbsolutePath());
}
bool File::openToWrite()
{
    if (fp_)
    {
        ERROR_LOG("Can not write to openned file!.");
        return false;
    }

    fp_ = fopen(absolutePath_.c_str(), "wb");

    if (not fp_)
    {
        ERROR_LOG("cannot open file : " + absolutePath_);
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

    if (not std::filesystem::exists(absolutePath_))
    {
        ERROR_LOG("file not exist! " + absolutePath_);
        return false;
    }

    fp_ = fopen(absolutePath_.c_str(), "rb");

    if (not fp_)
    {
        ERROR_LOG("cannot open file : " + absolutePath_);
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
void File::ConvertSlashes(const std::string &absoultePath, const std::string &dataRelative, const std::string &projectRelative)
{
    absolutePath_    = Utils::ReplaceSlash(absoultePath);
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
void File::ClearSpecialCharacters()
{
    const std::string notAllowed{"<>:\"|?*"};

    bool changeNeeded{false};
    auto fname = GetFilename();
    auto new_end =
        std::remove_if(fname.begin(), fname.end(),
                       [notAllowed, this, &changeNeeded](std::string::value_type c)
                       {
                           auto result = notAllowed.find(c) != std::string::npos;
                           if (result)
                           {
                               changeNeeded = true;
                               DEBUG_LOG(std::string("Remove notAllowed character \"") + c + "\" from file : " + initValue_);
                           }
                           return result;
                       });

    if (changeNeeded)
    {
        fname.erase(new_end, fname.end());
        ChangeFileName(fname);
    }
}
}  // namespace GameEngine

std::ostream& operator<<(std::ostream& os, const GameEngine::File& file)
{
    os << "File{";
    if (!file.GetAbsolutePath().empty())
        os << "absoultePath: " << file.GetAbsolutePath();
    else
        os << "initValue: " << file.GetInitValue();

    os << "}";
    return os;
}
