#!/usr/bin/env bash

# OSS-fuzz has a limit of 2.5GB before treating an input as an Out-Of-Memory bug:
# https://google.github.io/oss-fuzz/faq/#how-do-you-handle-timeouts-and-ooms
MAX_MEMORY=2500

ASAN_OPTIONS="detect_leaks=false" ./fuzzer \
  -artifact_prefix="fuzz/results/" \
  -rss_limit_mb="$MAX_MEMORY" \
  -fork=4 \
  fuzz/corpus/ fuzz/seeds/
