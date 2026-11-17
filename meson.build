project(
  'uprov',
  'c',
  version: '0.0.1',
  license: 'MIT',
  meson_version: '>=1.3.1',
  default_options: [
    'warning_level=2',
    'werror=true',
    'prefix=/usr/'
  ],
)

pargs = []
cc = meson.get_compiler('c')
project_name = meson.project_name()
so_version = meson.project_version()
src_root_dir = meson.current_source_dir()

file_offset_bits = get_option('file-offset-bits')

inc = include_directories('include')

pargs += [
  '-DUPROV_VERSION_MAJOR=@0@'.format(so_version.split('.')[0]),
  '-DUPROV_VERSION_MINOR=@0@'.format(so_version.split('.')[1]),
  '-DUPROV_VERSION_PATCH=@0@'.format(so_version.split('.')[2]),
  '-D_FILE_OFFSET_BITS=@0@'.format(file_offset_bits)
]

subdir('include')
subdir('src')

libuprov = library(
  project_name, fs,
  version: so_version,
  include_directories: inc,
  dependencies: libuprov_deps,
  c_args: pargs,
  install: true
)

libuprov_a = static_library(
  project_name, fs,
  include_directories: inc,
  dependencies: libuprov_deps,
  c_args: pargs,
  install: true
)

executable(project_name, exec_fs,
           link_with: libuprov,
           include_directories: inc,
           dependencies: [libudo],
           c_args: pargs,
           install: true)

pkg = import('pkgconfig')

pkgsubdirs = []

pkglibs_shared = [
  libuprov,
  libuprov_deps
]

pkglibs_private = [
  libuprov_a,
  libuprov_deps
]

pkg.generate(libraries: pkglibs_shared,
             libraries_private: pkglibs_private,
             subdirs : pkgsubdirs,
             version : so_version,
             name : project_name,
             filebase : project_name,
             description : 'Disk manipulation libfdisk wrapper library')

if get_option('tests')
  subdir('tests')
endif

if get_option('docs')
  docs_dir = src_root_dir + '/docs'
  docs_build_dir = build_root_dir + '/docs'
  sphinx = find_program('sphinx-build-5', 'sphinx-build', required: true)

  if sphinx.found()
    subdir('docs')
    custom_target('docs',
                  command: [sphinx, '-W', '-b', 'html', docs_dir, '@OUTPUT@'],
                  input: docs_src, output: 'docs', install: false, install_dir: '',
                  build_by_default: false, build_always_stale: true)
  endif
endif
