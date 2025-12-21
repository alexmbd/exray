project('raylib', 'c', version: '5.5')

raylib_inc = include_directories('include')

raylib_lib = meson.get_compiler('c').find_library(
  'raylib',
  dirs: meson.current_source_dir() + '/lib',
  static: true
)

raylib_dep = declare_dependency(
  include_directories: raylib_inc,
  dependencies: raylib_lib
)
