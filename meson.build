project(
    'prefetch-test',
    ['cpp'],
    default_options: [
        'buildtype=release',
        'optimization=3',
        'debug=true',
        'cpp_std=c++20',
        'warning_level=3',
    ],
)

cpp_args = [
]
add_project_arguments(cpp_args, language: 'cpp')

dependencies = [
#     dependency('absl_container', default_options: ['debug=true']),
]

srcs = [
  'main.cpp',
]

include_directories = [
  'wyhash/',
]

executable(
  'prefetch-test',
  srcs,
  dependencies: dependencies,
  include_directories: include_directories,
)