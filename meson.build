project(
  'exray',
  'cpp',
  meson_version: '>=1.10.0',
  default_options: [
    'cpp_std=c++23',
    'buildtype=release'
  ],
  subproject_dir: 'external',
  version: '1.0.0',
  license: 'Zlib'
)

src = [
  'src/main.cpp',
  'src/core/app.cpp',
  'src/core/canvas.cpp'
]

inc = ['src']

raylib_dep = subproject('rl').get_variable('raylib_dep')

thorvg_proj = subproject(
  'thorvg',
  default_options: [
    'default_library=static',
    'buildtype=release',
    'prefix=' + meson.current_source_dir() + '/external/thorvg',
    'threads=false',
    'extra=' # Disable Lottie Expressions support (maybe enabled in the future)
  ]
)
thorvg_dep = thorvg_proj.get_variable('thorvg_dep')

sys_deps = []
cpp = meson.get_compiler('cpp')
if host_machine.system() == 'windows'
  add_project_link_arguments('-lstdc++exp', '-static', '-static-libgcc', '-static-libstdc++', language: 'cpp') # For MinGW
  sys_deps += [
    cpp.find_library('opengl32'),
    cpp.find_library('gdi32'),
    cpp.find_library('winmm'),
    cpp.find_library('kernel32'),
    cpp.find_library('user32'),
    cpp.find_library('shell32')
  ]
endif

deps = declare_dependency(
  dependencies: [raylib_dep, thorvg_dep] + sys_deps
)

executable(
  'exray',
  src,
  include_directories: inc,
  dependencies: deps,
  build_subdir: 'dist'
)