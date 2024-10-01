# A list of available rules and their signatures can be found here: https://buck2.build/docs/prelude/globals/

cxx_library(
    name = "fmt",
    srcs = [
        "lib/fmt/src/format.cc",
    ],
    include_directories = [
        "lib/fmt/include",
    ],
    compiler_flags = [
        "-lfmt",
    ],
    visibility = ["PUBLIC"],
)

cxx_library(
    name = "array",
    headers = glob(["include/*.hpp"]),
    deps = [":fmt"],
    visibility = ["PUBLIC"],
    compiler_flags = [
        "-std=c++20",
        "-lfmt",
    ]
)

cxx_library(
    name = "catch2",
    srcs = glob([
        "lib/catch2/src/*.cpp",
        "lib/catch2/src/catch2/*.cpp",
        "lib/catch2/src/catch2/**/*.cpp",
    ]),
    headers = glob([
        "lib/catch2/src/*.hpp",
        "lib/catch2/src/catch2/*.hpp",
        "lib/catch2/src/catch2/**/*.hpp",
    ]),
    include_directories = [
        "lib/catch2/src",
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
        "lib/catch2/src",
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