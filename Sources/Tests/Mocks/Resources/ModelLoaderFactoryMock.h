#pragma once
#include <GameEngine/Resources/Models/WBLoader/AbstractLoader.h>
#include <GameEngine/Resources/Models/WBLoader/IModelLoaderFactory.h>
#include <gmock/gmock.h>

namespace GameEngine
{
class ModelLoaderFactoryMock : public IModelLoaderFactory
{
public:
    MOCK_METHOD(LoadersVector, createLoaders, (), (const, override));
};

class LoaderMock : public WBLoader::AbstractLoader
{
public:
    using WBLoader::AbstractLoader::AbstractLoader;

    MOCK_METHOD(bool, CheckExtension, (const File&), (override));
    MOCK_METHOD(bool, ParseFile, (const File&), (override));
};
}  // namespace GameEngine
