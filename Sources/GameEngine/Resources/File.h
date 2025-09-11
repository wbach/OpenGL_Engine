#pragma once
#include <Logger/Log.h>
#include <Types.h>

#include <filesystem>
#include <string>

namespace GameEngine
{
class File
{
public:
    File();
    File(const char*);
    File(const std::string&);
    File(const std::filesystem::path&);
    File(const File&) = default;

    void Init(const std::string&);
    void Init(const std::filesystem::path&);

    void DataRelative(const std::filesystem::path&);
    void AbsoultePath(const std::filesystem::path&);
    void ChangeExtension(const std::string&);

    const std::filesystem::path& GetDataRelativePath() const;
    const std::filesystem::path& GetAbsolutePath() const;

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
    File& operator=(const std::filesystem::path&);
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
                LOG_ERROR << "Read size error in " << absolutePath_;
            }
            data.resize(dataSize);
            auto dataBytes = fread(&data[0], sizeof(T), dataSize, fp_);

            if (dataBytes < sizeof(T) * dataSize)
            {
                LOG_ERROR << "Read data error in " << absolutePath_;
            }
        }
    }

private:
    void ClearSpecialCharacters();

private:
    std::filesystem::path absolutePath_;
    std::filesystem::path dataRelative_;

    FILE* fp_;
    long fileSize_;
};
}  // namespace GameEngine

std::ostream& operator<<(std::ostream&, const GameEngine::File&);
