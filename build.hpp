#pragma once

namespace cpp_helper_build {

void add(build::builder& builder, std::filesystem::path path) {
    builder.add_library("cpp_helper", absolute(path).string());
}

}
