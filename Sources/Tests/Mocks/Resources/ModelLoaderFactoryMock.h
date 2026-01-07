#pragma once
#include <GameEngine/Resources/Models/AbstractLoader.h>
#include <GameEngine/Resources/Models/IModelLoaderFactory.h>
#include <gmock/gmock.h>

namespace GameEngine
{
class ModelLoaderFactoryMock : public IModelLoaderFactory
{
public:
    MOCK_METHOD(LoadersVector, createLoaders, (), (const, override));
};

class LoaderMock : public AbstractLoader
{
public:
    using AbstractLoader::AbstractLoader;

    MOCK_METHOD(bool, CheckExtension, (const File&), (override));
    MOCK_METHOD(bool, ParseFile, (const File&), (override));
    MOCK_METHOD(std::unique_ptr<Model>, Create, (), (override));
};
}  // namespace GameEngine
