#pragma once
#include <Types.h>

namespace GameEngine
{
class File
{
public:
    File() = default;
    File(const std::string&);
    File(const char*);
    File(const File&) = default;
    void DataRelative(const std::string&);
    void ProjectRelative(const std::string&);
    void AbsoultePath(const std::string&);
    void ChangeExtension(const std::string&);

    const std::string& GetDataRelativeDir() const;
    const std::string& GetProjectRelativeDir() const;
    const std::string& GetAbsoultePath() const;
    std::string GetAbsolutePathWithDifferentExtension(const std::string&) const;

    std::string GetBaseName() const;
    std::string GetExtension() const;
    std::string GetFilename() const;
    std::string GetParentDir() const;
    File CreateFileWithExtension(const std::string&) const;
    void ChangeFileName(const std::string&);
    void ChangeBaseName(const std::string&);
    void AddSuffixToBaseName(const std::string&);
    bool IsExtension(const std::string&) const;

    bool operator==(const File&) const;
    File operator=(const char*) const;
    operator bool() const;
    bool empty() const;

private:
    void ConvertSlashes(const std::string&, const std::string&, const std::string&);
    void ConvertSlashesAndAddToRequired(const std::string&, const std::string&, const std::string&);
    bool IsProjectRelativePath(const std::string&) const;

private:
    std::string absoultePath_;
    std::string dataRelative_;
    std::string projectRelative_;
};
}  // namespace GameEngine
