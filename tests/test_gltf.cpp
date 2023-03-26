#include "gltf++/gltf.h"

#include <fmt/core.h>

#include <gtest/gtest.h>

#include <filesystem>
#include <sstream>

class AssetFixture : public ::testing::Test
{
public:
    AssetFixture()
    {
        model.default_scene = 1;
        model.scenes = {
            gltf::Scene{
                .name = "test-scene1",
                .nodes = {0}
            }
        };
        model.nodes = {
            gltf::Node{
                .mesh = 0
            }
        };
        model.asset.generator = "test-writer";
        model.asset.copyright = "test-copyright";
    }

protected:
    gltf::Model model;
};

TEST_F(AssetFixture, write_model)
{
    std::stringstream ss{ std::ios::in | std::ios::out | std::ios::binary };
    EXPECT_NO_THROW(gltf::write_model(model, ss));
    fmt::print("{}", ss.str());
}
