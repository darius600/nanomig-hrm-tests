#!/bin/bash
# Every test in this repository: the HRM suite and the integration tests.
#
#   ./run_all.sh            rebuild everything (make -B) and run
#   ./run_all.sh --fast     reuse existing binaries where possible
#
# Why this exists alongside hrm/run_all.sh: that one only looks at hrm/. On
# 07.09.2026 an upstream change broke the integration test at build time -
# PINNOTFOUND, not a single line ran - while the HRM summary still read 38/8/0.
# The failure was noticed only because the test happened to be run by hand.
# That should not be necessary again.
cd "$(dirname "$0")" || exit 2

if [ ! -e nanomig/src/minimig-aga/agnus_copper.v ]; then
    echo "The NanoMig submodule is missing. Once:"
    echo "    git submodule update --init"
    echo "Or point at an existing checkout:"
    echo "    make -C hrm/<test> NANOMIG=/path/to/NanoMig"
    exit 2
fi

rc=0

echo "===== HRM suite ====="
./hrm/run_all.sh "$@" || rc=1

echo
echo "===== Integration ====="
MAKE_ARGS="-B"
[ "$1" = "--fast" ] && MAKE_ARGS=""
for target in test_minimig_system test_copper; do
    out=$(make $MAKE_ARGS -C integration/minimig_system "$target" 2>&1)
    if [ $? -eq 0 ] && echo "$out" | grep -q 'All tests passed'; then
        echo "PASS  $target"
    else
        echo "FAIL  $target"
        echo "$out" | grep -E '^%Error|FAIL' | head -5
        rc=1
    fi
done

exit $rc
