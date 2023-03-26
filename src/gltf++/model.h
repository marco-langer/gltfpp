#pragma once

#include <cstddef>
#include <string>
#include <optional>
#include <unordered_map>
#include <string>
#include <vector>

namespace gltf {

struct Asset
{
    std::optional<std::string> generator;
    std::optional<std::string> copyright;
};

struct Scene
{
    std::string name;
    std::vector<std::size_t> nodes;
};

struct Node
{
    std::size_t mesh;
};

struct Primitive
{
    std::unordered_map<std::string, int> attributes;
    std::size_t material;
    std::size_t indices;
};

struct Mesh
{
    std::vector<Primitive> primitives;
};

enum class BufferTarget
{
    Array = 34962,
    ElementArray = 34963
};

struct Buffer
{
    std::vector<std::byte> data;
};

struct BufferView
{
    std::size_t buffer;
    std::size_t byte_offset;
    std::size_t byte_length;
    BufferTarget target;
};

enum class ComponentType
{
    Byte = 5120,
    UnsignedByte = 5121,
    Short = 5122,
    UnsignedShort = 5123,
    Int = 5124,
    UnsignedInt = 5125,
    Float = 5126,
    Double = 5130
};

enum class AccessorType
{
    Vec2,
    Vec3,
    Vec4,
    Mat2,
    Mat3,
    Mat4,
    Scalar,
    Vector,
    Matrix
};

struct Accessor
{
    std::size_t bufferview;
    std::size_t byte_offset;
    ComponentType component_type;
    std::size_t count;
    AccessorType type;
    std::vector<double> min_values;
    std::vector<double> max_values;
};

struct Model
{
    Asset asset;
    std::optional<std::size_t> default_scene;
    std::vector<Scene> scenes;
    std::vector<Node> nodes;
    std::vector<Mesh> meshes;
    std::vector<Buffer> buffers;
    std::vector<BufferView> bufferviews;
    std::vector<Accessor> accessors;
};

}