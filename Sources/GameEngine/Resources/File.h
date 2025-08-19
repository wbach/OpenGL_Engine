#pragma once
#include <Logger/Log.h>
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
    void Change(const std::string&);
    void DataRelative(const std::string&);
    void ProjectRelative(const std::string&);
    void AbsoultePath(const std::string&);
    void ChangeExtension(const std::string&);

    const std::string& GetDataRelativeDir() const;
    const std::string& GetProjectRelativeDir() const;
    const std::string& GetAbsoultePath() const;
    std::string GetAbsolutePathWithDifferentExtension(const std::string&) const;

    const std::string& GetInitValue() const;
    std::string GetBaseName() const;
    std::string GetExtension() const;
    std::string GetFilename() const;
    std::string GetParentDir() const;
    File CreateFileWithExtension(const std::string&) const;
    void ChangeFileName(const std::string&);
    void ChangeBaseName(const std::string&);
    void AddSuffixToBaseName(const std::string&);
    bool IsExtension(const std::string&) const;
    bool IsExtension(const std::vector<std::string>&) const;

    bool operator==(const File&) const;
    File& operator=(const File&);
    File& operator=(const char*);
    File& operator=(const std::string&);
    operator bool() const;
    bool empty() const;
    bool exist() const;

    bool openToWrite();
    bool openToRead();
    void close();

    template <typename T>
    void addVectorToFile(const std::vector<T>& data)
    {
        if (fp_)
        {
            uint32 dataSize = static_cast<uint32>(data.size());
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
            data.clear();
            auto readedBytes = fread(&dataSize, sizeof(uint32), 1, fp_);
            if (readedBytes < sizeof(uint32))
            {
                ERROR_LOG("Read size error in " + absoultePath_);
            }
            data.resize(dataSize);
            auto dataBytes = fread(&data[0], sizeof(T), dataSize, fp_);

            if (dataBytes < sizeof(T) * dataSize)
            {
                ERROR_LOG("Read data error in " + absoultePath_);
            }
        }
    }

private:
    void ConvertSlashes(const std::string&, const std::string&, const std::string&);
    void ConvertSlashesAndAddToRequired(const std::string&, const std::string&, const std::string&);
    bool IsProjectRelativePath(const std::string&) const;
    void printError(const std::string&) const;
    void ClearSpecialCharacters();

private:
    std::string initValue_;
    std::string absoultePath_;
    std::string dataRelative_;
    std::string projectRelative_;

    FILE* fp_;
    long fileSize_;
};
}  // namespace GameEngine

std::ostream& operator<<(std::ostream&, const GameEngine::File&);
