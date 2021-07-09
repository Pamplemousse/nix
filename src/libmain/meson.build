# libmain build file
#============================================================================

libmain_inc = []


# dependencies
#============================================================================

_libmain_external_dep_list = [
    pthread_dep,
    openssl_dep,
    libsodium_dep,
]

# Allow some "meta-programming" to generate derived versions of this library for fuzzing.
_libmain_nix_dep_list_names = [
    'libutil_dep',
    'libstore_dep',
]
_libmain_nix_dep_list = []
foreach d : _libmain_nix_dep_list_names
    _libmain_nix_dep_list += get_variable(d)
endforeach

libmain_dep_list = _libmain_external_dep_list + _libmain_nix_dep_list


# src files
#============================================================================

libmain_src = files(
    'common-args.cc',
    'loggers.cc',
    'progress-bar.cc',
    'shared.cc',
    'stack.cc')


libmain_headers = files(
    'common-args.hh',
    'loggers.hh',
    'progress-bar.hh',
    'shared.hh')


# include directories
#========================================================================

# include current dir
#---------------------------------------------------
libmain_inc += include_directories('.')


# targets
#============================================================================


# build
#============================================================================

# set build args
#---------------------------------------------------
libmain_cxx_args = []

libmain_link_args = []


# build library
#---------------------------------------------------
libmain_lib = library(
    'nixmain',
    sources : libmain_src,
    install : true,
    install_mode : 'rwxr-xr-x',
    install_dir : libdir,
    include_directories : [
        libmain_inc,
        proj_inc],
    cpp_args : libmain_cxx_args,
    link_args : libmain_link_args,
    dependencies : libmain_dep_list)


# install headers
#---------------------------------------------------
install_headers(
    libmain_headers,
    install_dir : join_paths(includedir, 'nix'))

# declare dependency
#---------------------------------------------------
libmain_dep = declare_dependency(
    link_with : libmain_lib,
    include_directories : libmain_inc)
