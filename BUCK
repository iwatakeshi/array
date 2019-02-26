load('//:buckaroo_macros.bzl', 'buckaroo_deps')

prebuilt_cxx_library(
  name = 'array',
  header_only = True,
  header_namespace = '',
  exported_headers = [
    'array.hpp',
  ],
  licenses = [
    'LICENSE',
  ], 
  visibility = [
    'PUBLIC',
  ],
)

cxx_binary(
  name = 'main',
  srcs = [
    'main.cpp',
  ],
  deps = [
    '//:array',
  ] + buckaroo_deps(),
)

cxx_test(
  name = 'test',
  srcs = [
    'test/main.cpp'
  ],
  deps = [
    '//:array',
  ] + buckaroo_deps()
)