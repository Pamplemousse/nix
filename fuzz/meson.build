# fuzz binaries build file
#============================================================================

fuzz_inc = [include_directories('.')]


# include subdirectories
#========================================================================

subdir('libraries')


# common flags for libraries for fuzzing
#============================================================================

library_cov_cxx_args = [
  '-fprofile-instr-generate',
  '-fcoverage-mapping'
]
library_cov_link_args = []

library_fuzasan_cxx_args = [
  '-fsanitize=fuzzer-no-link,address',
  # `shared-libasan` is necessary because of `-Wl,-z,defs` in `mk/libraries.mk`;
  # See https://github.com/google/sanitizers/wiki/AddressSanitizer#faq .
  '-shared-libasan',
  # `AddressSanitizerUseAfterScope` seems to raise a lot of false positive;
  '-fno-sanitize-address-use-after-scope',
  # Destructors called twice (probably because of static variables duplicated amongst libraries) cause double-frees.
  '-fno-c++-static-destructors',
]
library_fuzasan_link_args = [
  '-fsanitize=fuzzer-no-link,address',
  '-shared-libasan'
]

# Different "flavours" for fuzzing binaries and libraries, corresponding to the different instrumentations:
# coverage, fuzzer with ASan, etc.
#============================================================================
fuzzing_flavours = ['cov', 'fuzasan']


# Alter different libraries with different flavours of instrumentation;
# e.g. for every `libsomething`, produce `libcovsomething`, `libfuzasansomething`, etc.
#============================================================================

# Order matters, as the first ones are requirements for the last ones.
foreach l : [ 'libutil', 'libstore', 'libfetchers', 'libmain', 'libexpr', 'libcmd' ]
    foreach flavour : fuzzing_flavours

        foreach d : get_variable('_@0@_nix_dep_list_names'.format(l))
            # _lib<lib>_nix_<flavour>_dep_list
            set_variable(
                '_@0@_nix_@1@_dep_list'.format(l, flavour),
                get_variable('_@0@_nix_@1@_dep_list'.format(l, flavour), []) + [get_variable(d.replace('lib', 'lib@0@'.format(flavour)))],
            )
        endforeach

        # lib<flavour><lib>_cxx_args
        set_variable(
            'lib@0@@1@_cxx_args'.format(flavour, l.replace('lib', '')),
            get_variable('@0@_cxx_args'.format(l)) + get_variable('library_@0@_cxx_args'.format(flavour)),
        )

        # lib<flavour><lib>_link_args
        set_variable(
            'lib@0@@1@_link_args'.format(flavour, l.replace('lib', '')),
            get_variable('@0@_link_args'.format(l)) + get_variable('library_@0@_link_args'.format(flavour)),
        )

        # lib<flavour><lib>_dep_list
        set_variable(
            'lib@0@@1@_dep_list'.format(flavour, l.replace('lib', '')),
            get_variable('_@0@_external_dep_list'.format(l)) + get_variable('_@0@_nix_dep_list'.format(l)),
        )

        # lib<flavour><lib>_lib
        set_variable(
            'lib@0@@1@_lib'.format(flavour, l.replace('lib', '')),
            library(
                'nix@0@@1@'.format(flavour, l.replace('lib', '')),
                sources : get_variable('@0@_src'.format(l)),
                install : true,
                install_mode : 'rwxr-xr-x',
                install_dir : libdir,
                include_directories : [
                    get_variable('@0@_inc'.format(l)),
                    proj_inc],
                cpp_args : get_variable('lib@0@@1@_cxx_args'.format(flavour, l.replace('lib', ''))),
                link_args : get_variable('lib@0@@1@_link_args'.format(flavour, l.replace('lib', ''))),
                dependencies : get_variable('lib@0@@1@_dep_list'.format(flavour, l.replace('lib', '')))
            )
        )

        # lib<flavour><lib>_dep
        set_variable(
            'lib@0@@1@_dep'.format(flavour, l.replace('lib', '')),
            declare_dependency(
                link_with : get_variable('lib@0@@1@_lib'.format(flavour, l.replace('lib', ''))),
                include_directories : get_variable('@0@_inc'.format(l))
            )
        )
    endforeach
endforeach

# dependencies
#============================================================================

cov_dep_list = [
    libcovutil_dep,
    libcovstore_dep,
    libcovmain_dep,
    libcovexpr_dep,
    libcovfetchers_dep,
    libcovcmd_dep,

    fuzz_libmemory_dep,
]

fuzasan_dep_list = [
    libfuzasanutil_dep,
    libfuzasanstore_dep,
    libfuzasanmain_dep,
    libfuzasanexpr_dep,
    libfuzasanfetchers_dep,
    libfuzasancmd_dep,

    fuzz_libmemory_dep,
]

# common flags for fuzz targets for fuzz
#============================================================================

fuzz_target_cov_cxx_args = nix_cxx_args + [
    '-fsanitize=fuzzer',
    '-fprofile-instr-generate',
    '-fcoverage-mapping',
]

fuzz_target_cov_link_args = nix_link_args + [
    '-fsanitize=fuzzer',
    # TODO: those might not be necessary
    # '-fsanitize=fuzzer',
    # '-fprofile-instr-generate',
    # '-fcoverage-mapping',
]

fuzz_target_fuzasan_cxx_args = nix_cxx_args + [
    '-fsanitize=fuzzer,address',
    # `shared-libasan` is necessary because of `-Wl,-z,defs` in `mk/libraries.mk`;
    # See https://github.com/google/sanitizers/wiki/AddressSanitizer#faq .
    '-shared-libasan',
    # `AddressSanitizerUseAfterScope` seems to raise a lot of false positive;
    '-fno-sanitize-address-use-after-scope',
    # `-U_FORTIFY_SOURCE` is not supported by ASan;
    # See https://github.com/google/sanitizers/issues/247.
    '-U_FORTIFY_SOURCE',
]

fuzz_target_fuzasan_link_args = nix_link_args + [
    '-fsanitize=fuzzer,address',
    '-shared-libasan',
]


# build
#============================================================================

# build executables
#---------------------------------------------------

foreach e : ['parse', 'parse_eval']
    foreach flavour : fuzzing_flavours
        # <e>_<flavour>_bin
        set_variable(
            '@0@_@1@_bin'.format(e, flavour),
            executable(
                '@0@-@1@'.format(e, flavour),
                sources : [
                    files('target_@0@.cc'.format(e))
                ],
                install : true,
                install_mode : 'rwxr-xr-x',
                install_dir : bindir,
                include_directories : [fuzz_inc, proj_inc],
                cpp_args : get_variable('fuzz_target_@0@_cxx_args'.format(flavour)),
                link_args : get_variable('fuzz_target_@0@_link_args'.format(flavour)),
                dependencies : get_variable('@0@_dep_list'.format(flavour)),
            ),
        )
    endforeach
endforeach
