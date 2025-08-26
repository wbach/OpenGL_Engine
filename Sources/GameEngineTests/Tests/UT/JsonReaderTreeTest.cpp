#include <Utils/Json/JsonReader.h>
#include <gtest/gtest.h>

class JsonReaderTreeTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}

    Utils::JsonReader reader_;
};

TEST_F(JsonReaderTreeTest, SceneTreeStructure)
{
    const char* jsonData = R"json(
    {
        "scene": {
            "attributes": {
                "name": "TestSceneName",
                "time": 120
            },
            "gameObjects": {
                "cube": {
                    "attributes": { "name": "Karton" },
                    "transform": {
                        "position": { "x": 1.2, "y": 2.4, "z": 0.6 },
                        "rotation": { "x": 32.1, "y": 45.8, "z": 21.3 },
                        "scale": { "x": 2.1, "y": 3.8, "z": 1.3 }
                    },
                    "Components": {
                        "attributes": { "count": "1" },
                        "RendererComponent": { "file": "testFile.obj" }
                    },
                    "Children": {
                        "childCube": {
                            "attributes": { "name": "KartonChild" },
                            "transform": {
                                "position": { "x": 5.2, "y": 3.4, "z": 6.6 },
                                "rotation": { "x": 2.1, "y": 12.8, "z": 33.3 },
                                "scale": { "x": 5.1, "y": 4.8, "z": 1.7 }
                            },
                            "Components": {
                                "attributes": { "count": "1" },
                                "RendererComponent": { "file": "testFile2.obj" }
                            },
                            "Children": {}
                        }
                    }
                }
            }
        }
    }
    )json";

    // Parsowanie JSON
    ASSERT_TRUE(reader_.ReadJson(jsonData));
    TreeNode* root = reader_.Get();
    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->name(), "root");

    // Scene node
    TreeNode* scene = reader_.Get("scene", root);
    ASSERT_NE(scene, nullptr);

    // Sprawdzamy atrybuty sceny
    ASSERT_EQ(scene->attributes_.size(), 2);
    EXPECT_EQ(scene->attributes_.at("name"), "TestSceneName");
    float timeVal = std::stof(scene->attributes_.at("time"));
    EXPECT_FLOAT_EQ(timeVal, 120.0f);

    // GameObjects
    TreeNode* gameObjects = reader_.Get("gameObjects", scene);
    ASSERT_NE(gameObjects, nullptr);

    TreeNode* cube = reader_.Get("cube", gameObjects);
    ASSERT_NE(cube, nullptr);

    // Atrybuty cube
    ASSERT_EQ(cube->attributes_.size(), 1);
    EXPECT_EQ(cube->attributes_.at("name"), "Karton");

    // Cube Components
    TreeNode* components = reader_.Get("Components", cube);
    ASSERT_NE(components, nullptr);
    ASSERT_EQ(components->attributes_.size(), 1);
    EXPECT_EQ(components->attributes_.at("count"), "1");

    TreeNode* renderer = reader_.Get("RendererComponent", components);
    ASSERT_NE(renderer, nullptr);
    EXPECT_EQ(renderer->attributes_.size(), 0);
    EXPECT_EQ(renderer->value_, "");

    TreeNode* fileNode = reader_.Get("file", renderer);
    ASSERT_NE(fileNode, nullptr);
    EXPECT_EQ(fileNode->value_, "testFile.obj");

    // Cube Children
    TreeNode* children = reader_.Get("Children", cube);
    ASSERT_NE(children, nullptr);

    TreeNode* childCube = reader_.Get("childCube", children);
    ASSERT_NE(childCube, nullptr);

    // Atrybuty childCube
    ASSERT_EQ(childCube->attributes_.size(), 1);
    EXPECT_EQ(childCube->attributes_.at("name"), "KartonChild");

    // ChildCube Components
    TreeNode* childComponents = reader_.Get("Components", childCube);
    ASSERT_NE(childComponents, nullptr);
    ASSERT_EQ(childComponents->attributes_.size(), 1);
    EXPECT_EQ(childComponents->attributes_.at("count"), "1");

    TreeNode* childRenderer = reader_.Get("RendererComponent", childComponents);
    ASSERT_NE(childRenderer, nullptr);
    EXPECT_EQ(childRenderer->attributes_.size(), 0);
    EXPECT_EQ(childRenderer->value_, "");

    TreeNode* fileNode2 = reader_.Get("file", childRenderer);
    ASSERT_NE(fileNode2, nullptr);
    EXPECT_EQ(fileNode2->value_, "testFile2.obj");
}
