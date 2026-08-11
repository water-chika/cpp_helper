#pragma once

namespace cpp_helper_build {

void add(build::builder& builder, std::filesystem::path path, std::filesystem::path build_dir) {
    builder.add_library("cpp_helper", build_dir / "cpp_helper", absolute(path).string());
}

}
