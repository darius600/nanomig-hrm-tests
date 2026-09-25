#!/bin/bash
# Every test in this repository: the HRM suite, the AA suite and the
# integration tests.
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

# The AA packages live in their own tree and this script -- the one whose first
# line says "every test in this repository" -- did not run them. hrm/run_all.sh
# grew --in on 21.09.2026 for exactly that reason and the repair stopped one
# level short, so three packages and 15 assertions stayed invisible to the
# repository's own entry point on the day they were written. That is the same
# failure the header above describes from 07.09.2026, one tree over.
if [ -d aa ]; then
    echo
    echo "===== AA suite ====="
    ./hrm/run_all.sh --in ../aa "$@" || rc=1
fi

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
# The apparatus for tests at nanomig.v's own ports: both CPU cores must pass
# every apparatus check, and each core's control must fail its colour check.
# Its Makefile exits non-zero if either does not hold.
out=$(make $MAKE_ARGS -C integration/apparatus-cpu-to-video test 2>&1)
if [ $? -eq 0 ]; then
    echo "PASS  apparatus-cpu-to-video"
else
    echo "FAIL  apparatus-cpu-to-video"
    echo "$out" | grep -E '^%Error|^FAIL|CONTROL|normal run had' | head -5
    rc=1
fi
# The apparatus for 68000 programs assembled with vasm: loaded where the
# harness says, started through the reset vectors, results read back, and a
# control whose value check must fail. Needs vasmm68k_mot on the PATH.
out=$(make $MAKE_ARGS -C integration/apparatus-vasm-program test 2>&1)
if [ $? -eq 0 ]; then
    echo "PASS  apparatus-vasm-program"
else
    echo "FAIL  apparatus-vasm-program"
    echo "$out" | grep -E '^%Error|^FAIL|vasmm68k_mot|not found' | head -5
    rc=1
fi

exit $rc
