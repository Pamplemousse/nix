#!/usr/bin/env bash

export BUILD_DEBUG=1

./bootstrap.sh
./configure $configureFlags --prefix="$(pwd)/outputs/out" --enable-gc=no

# Build our shared library to do leak-free and GC-free memory management.
make memory

# * `shared-libasan` necessary because of `-Wl,-z,defs` in `mk/libraries.mk`.
#   See https://github.com/google/sanitizers/wiki/AddressSanitizer#faq ;
# * `AddressSanitizerUseAfterScope` seems to raise a lot of false positive;
make fuzzer \
  CXXFLAGS="-O0 -fsanitize=address -fno-sanitize-address-use-after-scope" \
  LDFLAGS="-fsanitize=address -shared-libasan" \
  OPTIMIZE=0
