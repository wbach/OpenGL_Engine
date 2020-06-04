#pragma once
#include <Types.h>
#include <fstream>

namespace GameEngine
{
class File
{
public:
    File();
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

    bool openToWrite();
    bool openToRead();
    void close();

    template <typename T>
    void addVectorToFile(const std::vector<T>& data)
    {
        if (fp_)
        {
            uint32 dataSize = data.size();
            fwrite(&dataSize, sizeof(uint32), 1, fp_);

            if (not data.empty())
            {
                fwrite(&data[0], sizeof(T), dataSize, fp_);
            }
        }
    }

    template <class T>
    void readVectorFromFile(std::vector<T>& data)
    {
        if (fp_)
        {
            uint32 dataSize = 0;

            auto currentPosition = ftell(fp_);
            if (currentPosition < sizeof(uint32))
            {
                data.clear();
                fread(&dataSize, sizeof(uint32), 1, fp_);
            }
            else
            {
                printError("Reading file error, to many bits requested");
            }

            currentPosition = ftell(fp_);
            if (currentPosition < sizeof(T) * dataSize)
            {
                data.resize(dataSize);
                fread(&data[0], sizeof(T), dataSize, fp_);
            }
            else
            {
                printError("Reading file error, to many bits requested");
            }
        }
    }

private:
    void ConvertSlashes(const std::string&, const std::string&, const std::string&);
    void ConvertSlashesAndAddToRequired(const std::string&, const std::string&, const std::string&);
    bool IsProjectRelativePath(const std::string&) const;
    void printError(const std::string&) const;

private:
    std::string absoultePath_;
    std::string dataRelative_;
    std::string projectRelative_;

    FILE* fp_;
    long fileSize_;
};
}  // namespace GameEngine
