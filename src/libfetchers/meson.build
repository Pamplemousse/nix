# libfetchers build file
#============================================================================

libfetchers_inc = []

# dependencies
#============================================================================

_libfetchers_external_dep_list = [
    libbz2_dep,
    libcurl_dep,
    libdl_dep,
    libseccomp_dep,
    libsodium_dep,
    #nlohmann_json_dep,
    pthread_dep,
    sqlite3_dep,
]

# Allow some "meta-programming" to generate derived versions of this library for fuzzing.
_libfetchers_nix_dep_list_names = [
    'libutil_dep',
    'libstore_dep',
]
_libfetchers_nix_dep_list = []
foreach d : _libfetchers_nix_dep_list_names
    _libfetchers_nix_dep_list += get_variable(d)
endforeach

libfetchers_dep_list = _libfetchers_external_dep_list + _libfetchers_nix_dep_list

# src files
#============================================================================

libfetchers_src = files(
    'attrs.cc',
    'cache.cc',
    'fetchers.cc',
    'git.cc',
    'github.cc',
    'indirect.cc',
    'mercurial.cc',
    'path.cc',
    'registry.cc',
    'tarball.cc')

libfetchers_headers = files(
    'attrs.hh',
    'cache.hh',
    'fetchers.hh',
    'registry.hh')


# include directories
#========================================================================

# include current dir
#---------------------------------------------------
libfetchers_inc += include_directories('.')


# targets
#============================================================================


# build
#============================================================================

# set build args
#---------------------------------------------------
libfetchers_cxx_args = []

libfetchers_link_args = []


# build library
#---------------------------------------------------
libfetchers_lib = library(
    'nixfetchers',
    sources : libfetchers_src,
    install : true,
    install_mode : 'rwxr-xr-x',
    install_dir : libdir,
    include_directories : [
        libfetchers_inc,
        proj_inc],
    cpp_args : libfetchers_cxx_args,
    link_args : libfetchers_link_args,
    dependencies : libfetchers_dep_list)


# install headers
#---------------------------------------------------
install_headers(
    libfetchers_headers,
    install_dir : join_paths(includedir, 'nix'))


# generate pkg-config
#---------------------------------------------------
libfetchers_config = pkg.generate(
    libfetchers_lib,
    libraries : [
        libfetchers_lib],
    version : meson.project_version(),
    name : 'Nix',
    subdirs : ['nix/'],
    filebase : 'nix-store',
    extra_cflags : '-std=c++17',
    description : 'Nix Package Manager.')


# declare dependency
#---------------------------------------------------
libfetchers_dep = declare_dependency(
    link_with : libfetchers_lib,
    include_directories : libfetchers_inc)
