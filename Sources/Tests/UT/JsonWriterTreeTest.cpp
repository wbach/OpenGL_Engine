#include <Logger/Log.h>
#include <Utils/Json/JsonReader.h>
#include <Utils/Json/JsonWriter.h>
#include <gtest/gtest.h>

class JsonWriterReaderTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }
    void TearDown() override
    {
    }

    Utils::JsonReader reader_;
};

TEST_F(JsonWriterReaderTest, TreeNodeRoundTrip)
{
    // 1. Tworzymy TreeNode
    TreeNode root("scene");

    root.attributes_["name"] = "TestSceneName";
    root.attributes_["time"] = "120";

    auto& cube               = root.addChild("cube");
    cube.attributes_["name"] = "Karton";

    auto& children                = cube.addChild("Children");
    auto& childCube               = children.addChild("childCube");
    childCube.attributes_["name"] = "KartonChild";

    // 2. Zapis do JSON
    std::string jsonStr = Utils::Json::Write(root);
    ASSERT_FALSE(jsonStr.empty());
    LOG_DEBUG << jsonStr;

    // 3. Odczyt JSON
    Utils::JsonReader newReader;
    ASSERT_TRUE(newReader.ReadJson(jsonStr));

    TreeNode* newRoot = newReader.Get();
    ASSERT_NE(newRoot, nullptr);
    EXPECT_EQ(newRoot->name(), "root");  // JsonWriter dodaje root jako główny węzeł

    // 4. Sprawdzenie atrybutów scene
    TreeNode* scene = newReader.Get("scene");
    ASSERT_NE(scene, nullptr);

    ASSERT_TRUE(scene->attributes_.find("name") != scene->attributes_.end());
    ASSERT_TRUE(scene->attributes_.find("time") != scene->attributes_.end());
    EXPECT_EQ(scene->attributes_.at("name"), "TestSceneName");
    EXPECT_EQ(scene->attributes_.at("time"), "120");

    // 5. Sprawdzenie cube
    TreeNode* cubeNode = newReader.Get("cube");
    ASSERT_NE(cubeNode, nullptr);

    ASSERT_TRUE(cubeNode->attributes_.find("name") != cubeNode->attributes_.end());
    EXPECT_EQ(cubeNode->attributes_.at("name"), "Karton");

    // 6. Sprawdzenie childCube
    TreeNode* childrenNode = newReader.Get("Children", cubeNode);
    ASSERT_NE(childrenNode, nullptr);

    TreeNode* childCubeNode = newReader.Get("childCube", childrenNode);
    ASSERT_NE(childCubeNode, nullptr);

    ASSERT_TRUE(childCubeNode->attributes_.find("name") != childCubeNode->attributes_.end());
    EXPECT_EQ(childCubeNode->attributes_.at("name"), "KartonChild");
}

TEST_F(JsonWriterReaderTest, ArrayRoundTrip)
{
    const char* jsonInput = R"(
    {
        "diffuseColor": [1, 0, 0, 1]
    })";

    // 2. Parsowanie
    ASSERT_TRUE(reader_.ReadJson(jsonInput));
    TreeNode* root = reader_.Get();
    ASSERT_NE(root, nullptr);

    TreeNode* diffuseColor = reader_.Get("diffuseColor", root);
    ASSERT_NE(diffuseColor, nullptr);

    // 3. Sprawdzenie elementów tablicy
    ASSERT_EQ(diffuseColor->getChildren().size(), 4);

    EXPECT_EQ(diffuseColor->getChildren()[0]->value_, "1");
    EXPECT_EQ(diffuseColor->getChildren()[1]->value_, "0");
    EXPECT_EQ(diffuseColor->getChildren()[2]->value_, "0");
    EXPECT_EQ(diffuseColor->getChildren()[3]->value_, "1");

    // 4. Serializacja z powrotem do JSON
    std::string outJson = Utils::Json::Write(*root);
    ASSERT_FALSE(outJson.empty());
    LOG_DEBUG << outJson;

    // 5. Ponowne parsowanie JSON-a wygenerowanego przez Writer
    Utils::JsonReader newReader;
    ASSERT_TRUE(newReader.ReadJson(outJson));

    TreeNode* newRoot = newReader.Get();
    ASSERT_NE(newRoot, nullptr);

    TreeNode* newDiffuseColor = newReader.Get("diffuseColor", newRoot);
    ASSERT_NE(newDiffuseColor, nullptr);

    ASSERT_EQ(newDiffuseColor->getChildren().size(), 4);
    EXPECT_EQ(newDiffuseColor->getChildren()[0]->value_, "1");
    EXPECT_EQ(newDiffuseColor->getChildren()[1]->value_, "0");
    EXPECT_EQ(newDiffuseColor->getChildren()[2]->value_, "0");
    EXPECT_EQ(newDiffuseColor->getChildren()[3]->value_, "1");
}