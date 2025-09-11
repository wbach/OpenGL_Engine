#include "File.h"

#include <Logger/Log.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <algorithm>
#include <filesystem>

#include "GameEngine/Engine/Configuration.h"

namespace fs = std::filesystem;

namespace GameEngine
{
File::File()
    : fp_{nullptr}
    , fileSize_(0)
{
}

File::File(const std::string &input)
    : File(std::filesystem::path(input))
{
}

File::File(const char *input)
    : File(std::filesystem::path(input))
{
}

File::File(const std::filesystem::path &input)
    : fp_{nullptr}
    , fileSize_(0)
{
    Init(input);
}

void File::Init(const std::string &input)
{
    Init(std::filesystem::path{input});
}

void File::Init(const std::filesystem::path &input)
{
    if (input.empty())
        return;

    try
    {
        if (std::filesystem::path(input).is_absolute())
        {
            AbsoultePath(input);
        }
        else
        {
            DataRelative(input);
        }
    }
    catch (...)
    {
        LOG_ERROR << "File init error: " << input;
    }

    ClearSpecialCharacters();
}

void File::DataRelative(const std::filesystem::path &filename)
{
    dataRelative_ = filename;
    absolutePath_ = std::filesystem::absolute(std::filesystem::path(EngineConf.files.data) / dataRelative_);
}

void File::AbsoultePath(const std::filesystem::path &filename)
{
    absolutePath_ = filename;
    dataRelative_ = fs::relative(absolutePath_, EngineConf.files.data);
}

void File::ChangeExtension(const std::string &extension)
{
    absolutePath_ = std::filesystem::path(absolutePath_).replace_extension(extension).string();
    dataRelative_ = std::filesystem::path(dataRelative_).replace_extension(extension).string();
}

const std::filesystem::path &File::GetDataRelativePath() const
{
    return dataRelative_;
}

const std::filesystem::path &File::GetAbsolutePath() const
{
    return absolutePath_;
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
    absolutePath_ = std::filesystem::path(absolutePath_).replace_filename(filename).string();
    dataRelative_ = std::filesystem::path(dataRelative_).replace_filename(filename).string();
}

void File::ChangeBaseName(const std::string &basename)
{
    auto filenameWithExtension = basename + GetExtension();
    absolutePath_              = std::filesystem::path(absolutePath_).replace_filename(filenameWithExtension).string();
    dataRelative_              = std::filesystem::path(dataRelative_).replace_filename(filenameWithExtension).string();
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
        Init(f.absolutePath_);
    }
    return *this;
}
File &File::operator=(const char *str)
{
    Init(std::filesystem::path(str));
    return *this;
}
File &File::operator=(const std::string &str)
{
    Init(str);
    return *this;
}
File& File::operator=(const std::filesystem::path& path)
{
    Init(path);
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
    return absolutePath_.empty() or dataRelative_.empty();
}

bool File::exist() const
{
    return not empty() and std::filesystem::exists(absolutePath_);
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
        LOG_ERROR << "Cannot open file : " << absolutePath_;
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
        LOG_ERROR << "File not exist : " << absolutePath_;
        return false;
    }

    fp_ = fopen(absolutePath_.c_str(), "rb");

    if (not fp_)
    {
        LOG_ERROR << "Cannot open file : " << absolutePath_;
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
                               DEBUG_LOG(std::string("Remove notAllowed character \"") + c + "\" from file : " + GetFilename());
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

std::ostream &operator<<(std::ostream &os, const GameEngine::File &file)
{
    os << "File";
    os << "{";
    os << "absolutePath: " << file.GetAbsolutePath();
    os << "dataRelative: " << file.GetDataRelativePath();
    os << "}";
    return os;
}
