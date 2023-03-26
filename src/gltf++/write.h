#pragma once

#include "model.h"

#include <filesystem>
#include <fstream>

namespace gltf {

auto write_model(Model const& model, std::filesystem::path const& filepath) -> void;
auto write_model(Model const& model, std::ostream& out_stream) -> void;

}