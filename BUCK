# A list of available rules and their signatures can be found here: https://buck2.build/docs/prelude/globals/

cxx_library(
    name = "fmt",
    srcs = glob([
        "third_party/fmt/*.cc",
        "third_party/fmt/*.cpp",
        "third_party/fmt/src/*.cc", 
        "third_party/fmt/src/*.cpp", 
    ]),
    headers = glob(["third_party/fmt/include/fmt/*.h"]),
    include_directories = ["third_party/fmt/include"],
    visibility = ["PUBLIC"],
)

cxx_library(
    name = "array",
    headers = ["array.hpp"],
    deps = [":fmt"],
    visibility = ["PUBLIC"],
)

cxx_library(
    name = "catch2",
    srcs = glob([
        "third_party/catch2/src/*.cpp",
        "third_party/catch2/src/catch2/*.cpp",
        "third_party/catch2/src/catch2/**/*.cpp",
    ]),
    headers = glob([
        "third_party/catch2/src/*.hpp",
        "third_party/catch2/src/catch2/*.hpp",
        "third_party/catch2/src/catch2/**/*.hpp",
    ]),
    include_directories = [
        "third_party/catch2/src",
    ],
    compiler_flags = [
        "-fPIC",
        "-std=c++17",
    ],
    visibility = ["PUBLIC"],
)

cxx_binary(
    name = "array_test",
    srcs = ["test/array_tests.cpp"],
    deps = [
        ":array",
        ":catch2",
        ":fmt",
    ],
    include_directories = [
        ".",
        "third_party/catch2/src",
    ],
)

cxx_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [
        ":array",
        ":fmt",
    ],
)