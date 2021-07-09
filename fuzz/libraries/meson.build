fuzz_libmemory_dir = meson.current_source_dir()
fuzz_libmemory_inc = [include_directories('.')]


# src files
#============================================================================

fuzz_libmemory_src = files(
    join_paths(fuzz_libmemory_dir, 'memory.cc'),
)

fuzz_libmemory_headers = files(
    join_paths(fuzz_libmemory_dir, 'memory.hh'),
)


# build
#============================================================================

# build library
#---------------------------------------------------

fuzz_libmemory_lib = shared_library(
    'fuzzmemory',
    sources : fuzz_libmemory_src,
    include_directories : [
        fuzz_libmemory_inc,
        proj_inc,
    ],
)

# install headers
#---------------------------------------------------

install_headers(
    fuzz_libmemory_headers,
    install_dir : join_paths(includedir, 'fuzz'))


# declare dependency
#---------------------------------------------------
fuzz_libmemory_dep = declare_dependency(
    link_with : fuzz_libmemory_lib,
)
