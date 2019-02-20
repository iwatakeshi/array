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
    ':array',
  ],
)