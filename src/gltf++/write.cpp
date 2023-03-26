#include "write.h"

#include <cpp-base64/base64.h>

#include <boost/json.hpp>

#include <array>
#include <fstream>

namespace json = boost::json;

namespace {

constexpr auto accessor_type_strs = std::array{
    "VEC2", "VEC3", "VEC4", "MAT2", "MAT3", "MAT4",
    "SCALAR", "VECTOR", "MATRIX"
};

auto append_default_scene(json::object& root, std::optional<std::size_t> const& default_scene)
{
    if (default_scene.has_value()) {
        root.insert(std::pair{ "scene", *default_scene });
    }
}

auto append_scene(json::object& scene_obj, gltf::Scene const& scene) -> void
{
    scene_obj.insert(std::pair{ "name", json::string_view{ scene.name } });
    auto& nodes_arr = scene_obj.insert(std::pair{ "nodes", json::array{} }).first->value().as_array();
    nodes_arr.reserve(scene.nodes.size());
    for (auto const& node : scene.nodes) {
        nodes_arr.push_back(node);
    }
}

auto append_scenes(json::object& root, std::vector<gltf::Scene> const& scenes) -> void
{
    auto& scenes_arr = root.insert(std::pair{ "scenes", json::array{} }).first->value().as_array();
    scenes_arr.reserve(scenes.size());
    for (auto const& scene : scenes) {
        append_scene(scenes_arr.emplace_back(json::object{}).as_object(), scene);
    }
}

auto append_asset(json::object& root, gltf::Asset const& asset) -> void
{
    auto& asset_obj = root.insert(std::pair{ "asset", json::object{} }).first->value().as_object();
    asset_obj.insert(std::pair{ "version", "2.0" });

    if (asset.generator.has_value()) {
        asset_obj.insert(std::pair{ "generator", json::string_view{ *asset.generator } });
    }
    if (asset.copyright.has_value()) {
        asset_obj.insert(std::pair{ "copyright", json::string_view{ *asset.copyright } });
    }
}

auto append_node(json::object& node_obj, gltf::Node const& node) -> void
{
    node_obj.insert(std::pair{ "mesh", node.mesh });
}

auto append_nodes(json::object& root, std::vector<gltf::Node> const& nodes) -> void
{
    auto& nodes_arr = root.insert(std::pair{ "nodes", json::array{} }).first->value().as_array();
    nodes_arr.reserve(nodes.size());
    for (auto const& node : nodes) {
        append_node(nodes_arr.emplace_back(json::object{}).as_object(), node);
    }
}

auto append_primitive(json::object& primitive_obj, gltf::Primitive const& primitive) -> void
{
    primitive_obj.insert(std::pair{ "material", primitive.material });
    primitive_obj.insert(std::pair{ "indices", primitive.indices });

    if (!primitive.attributes.empty()) { // TODO is empty allowed?
        auto& attributes_obj = primitive_obj.insert(std::pair{ "attributes", json::object{} }).first->value().as_object();
        for (auto const& [key, value] : primitive.attributes) {
            attributes_obj.insert(std::pair{ json::string_view{ key }, value });
        }
    }
}

auto append_primitives(json::object& mesh_obj, std::vector<gltf::Primitive> const& primitives) -> void
{
    auto& primitives_arr = mesh_obj.insert(std::pair{ "primitives", json::array{} }).first->value().as_array();
    primitives_arr.reserve(primitives.size());
    for (auto const& primitive : primitives) {
        append_primitive(primitives_arr.emplace_back(json::object{}).as_object(), primitive);
    }
}

auto append_mesh(json::object& mesh_obj, gltf::Mesh const& mesh) -> void
{
    append_primitives(mesh_obj, mesh.primitives);
}

auto append_meshes(json::object& root, std::vector<gltf::Mesh> const& meshes) -> void
{
    auto& meshes_arr = root.insert(std::pair{ "meshes", json::array{} }).first->value().as_array();
    meshes_arr.reserve(meshes.size());
    for (auto const& mesh : meshes) {
        append_mesh(meshes_arr.emplace_back(json::object{}).as_object(), mesh);
    }
}

auto append_buffer(json::object& buffer_obj, gltf::Buffer const& buffer) -> void
{
    buffer_obj.insert(std::pair{ "byteLength", buffer.data.size() });

    // TODO replace this base64 implementation with a faster/vectorized one
    constexpr auto * buffer_uri = "data:application/octet-stream;base64,";
    const auto encoded = base64_encode(reinterpret_cast<unsigned char const*>(buffer.data.data()), buffer.data.size());
    auto& uri = buffer_obj.insert(std::pair{ "uri", buffer_uri }).first->value().as_string();
    uri.reserve(uri.size() + encoded.size());
    uri.append(json::string_view{ encoded });
}

auto append_buffers(json::object& root, std::vector<gltf::Buffer> const& buffers) -> void
{
    auto& buffer_arr = root.insert(std::pair{ "buffers", json::array{} }).first->value().as_array();
    buffer_arr.reserve(buffers.size());
    for (auto const& buffer : buffers) {
        append_buffer(buffer_arr.emplace_back(json::object{}).as_object(), buffer);
    }
}

auto append_bufferview(json::object& bufferview_obj, gltf::BufferView const& bufferview) -> void
{
    bufferview_obj.insert(std::pair{ "buffer", bufferview.buffer });
    bufferview_obj.insert(std::pair{ "byteOffset", bufferview.byte_offset });
    bufferview_obj.insert(std::pair{ "byteLength", bufferview.byte_length });
    bufferview_obj.insert(std::pair{ "target", static_cast<int>(bufferview.target) });
}

auto append_bufferviews(json::object& root, std::vector<gltf::BufferView> const& bufferviews) -> void
{
    auto& bufferviews_arr = root.insert(std::pair{ "bufferViews", json::array{} }).first->value().as_array();
    bufferviews_arr.reserve(bufferviews.size());
    for (auto const& bufferview : bufferviews) {
        append_bufferview(bufferviews_arr.emplace_back(json::object{}).as_object(), bufferview);
    }
}

auto append_accessor(json::object& accessor_obj, gltf::Accessor const& accessor) -> void
{
    accessor_obj.insert(std::pair{ "bufferView", accessor.bufferview });
    accessor_obj.insert(std::pair{ "byteOffset", accessor.byte_offset });
    accessor_obj.insert(std::pair{ "componentType", static_cast<int>(accessor.component_type) });
    accessor_obj.insert(std::pair{ "count", accessor.count });
    accessor_obj.insert(std::pair{ "type", accessor_type_strs[static_cast<int>(accessor.type)]});
    
    // TODO insert via iterators ?
    auto& max_arr = accessor_obj.insert(std::pair{ "max", json::array{} }).first->value().as_array();
    max_arr.reserve(accessor.max_values.size());
    for (auto const& max_value : accessor.max_values) {
        max_arr.push_back(max_value);
    }
    auto& min_arr = accessor_obj.insert(std::pair{ "min", json::array{} }).first->value().as_array();
    min_arr.reserve(accessor.min_values.size());
    for (auto const& min_value : accessor.min_values) {
        min_arr.push_back(min_value);
    }
}

auto append_accessors(json::object& root, std::vector<gltf::Accessor> const& accessors) -> void
{
    auto& accessors_arr = root.insert(std::pair{ "accessors", json::array{} }).first->value().as_array();
    accessors_arr.reserve(accessors.size());
    for (auto const& accessor : accessors) {
        append_accessor(accessors_arr.emplace_back(json::object{}).as_object(), accessor);
    }
}

}

namespace gltf {

auto write_model(Model const& model, std::filesystem::path const& filepath) -> void
{
    std::ofstream out_stream{ filepath, std::ios::out | std::ios::binary };
    write_model(model, out_stream);
}
auto write_model(Model const& model, std::ostream& out_stream) -> void
{
    boost::json::value value;
    auto& root = value.emplace_object();

    append_default_scene(root, model.default_scene);
    append_scenes(root, model.scenes);
    append_nodes(root, model.nodes);
    append_meshes(root, model.meshes);
    append_buffers(root, model.buffers);
    append_bufferviews(root, model.bufferviews);
    append_accessors(root, model.accessors);
    append_asset(root, model.asset);

    out_stream << value;
}

}