#!/usr/bin/env bash

#
# Setup
#

# Adding test expressions to the initial seeds.
if [[ $(find fuzz/seeds/ -type l | wc -l) -lt 1 ]]; then
  for EXPRESSION in $(ls tests/lang/ | grep -E ".nix$"); do
    ln -s "../../tests/lang/${EXPRESSION}" "fuzz/seeds/${EXPRESSION}"
  done
fi

# Minimize the initial seeds into the corpus.
ASAN_OPTIONS=detect_leaks=false ./buildir/fuzz/parse_eval-fuzasan \
  -artifact_prefix="fuzz/results/" \
  -merge=1 \
  fuzz/corpus/ fuzz/seeds/

DICTIONARY=$(mktemp)
cat fuzz/dictionaries/* > "$DICTIONARY"

#
# Run the fuzzer(s)
#

# OSS-fuzz has a limit of 2.5GB before treating an input as an Out-Of-Memory bug:
# https://google.github.io/oss-fuzz/faq/#how-do-you-handle-timeouts-and-ooms
MAX_MEMORY=2500

ASAN_OPTIONS="detect_leaks=false" ./buildir/fuzz/parse_eval-fuzasan \
  -artifact_prefix="fuzz/results/" \
  -dict="$DICTIONARY" \
  -rss_limit_mb="$MAX_MEMORY" \
  -fork=4 \
  fuzz/corpus/

#
# Teardown / Clean
#

rm "$DICTIONARY"
