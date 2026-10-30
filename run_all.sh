#!/bin/bash
# Every test in this repository: the HRM suite, the AA suite and the
# integration tests.
#
#   ./run_all.sh            rebuild everything (make -B) and run
#   ./run_all.sh --fast     reuse existing binaries where possible
#   ./run_all.sh --jobs 8   run up to 8 packages of each tree at a time
#
# Why this exists alongside hrm/run_all.sh: that one only looks at one tree. An
# upstream change can break the integration tests at build time, so that not a
# single line of them runs, while the HRM summary still reads as before. Every
# layer reports here, so such a break shows without anyone running a test by
# hand.
cd "$(dirname "$0")" || exit 2

if [ ! -e nanomig/src/minimig-aga/agnus_copper.v ]; then
    echo "The NanoMig submodule is missing. Once:"
    echo "    git submodule update --init"
    echo "Or point at an existing checkout:"
    echo "    make -C hrm/<test> NANOMIG=/path/to/NanoMig"
    exit 2
fi

# The TG68K builds compile a GHDL translation of the board's VHDL and a copy of
# cpu_wrapper.v with the board's skipFetch connection (integration/harness/
# tg68k.mk). Make them once here, before any package starts: a missing GHDL
# stops the run with one message instead of failing every TG68K build.
# TG68K_KERNEL=shipped builds with the submodule's own files and needs no GHDL.
if [ "${TG68K_KERNEL:-board}" = board ]; then
    sh integration/harness/tg68k-board.sh generate "${NANOMIG:-}" > /dev/null || exit 2
fi

rc=0

echo "===== HRM suite ====="
./hrm/run_all.sh "$@" || rc=1

# The AA packages live in their own tree; hrm/run_all.sh reaches them only
# through --in, so this entry point names them explicitly.
if [ -d aa ]; then
    echo
    echo "===== AA suite ====="
    ./hrm/run_all.sh --in ../aa "$@" || rc=1
fi

echo
echo "===== Integration ====="
MAKE_ARGS="-B"
JOBS=1
prev=""
for arg in "$@"; do
    [ "$arg" = "--fast" ] && MAKE_ARGS=""
    [ "$prev" = "--jobs" ] && JOBS="$arg"
    prev="$arg"
done
case "$JOBS" in ''|*[!0-9]*|0) JOBS=1 ;; esac
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
# Graded integration packages: 68000 programs at nanomig.v's ports, graded
# against the printed page like hrm/ and aa/, on both CPU cores. -k keeps
# make going to the second core when the first has a red. One line per core,
# in the form of the package lines above; a core that printed no summary is a
# build error. A package with no Makefile has a specification and no test yet:
# one SPEC line, neither pass nor fail. --jobs builds and runs up to that many
# packages at a time, as hrm/run_all.sh does, and prints the same lines in the
# same order once all have finished; each package builds into its own
# directory.
graded=()
for dir in integration/*/; do
    pkg=$(basename "$dir")
    case "$pkg" in harness|minimig_system|apparatus-*) continue ;; esac
    [ -f "integration/$pkg/Makefile" ] && graded+=("$pkg")
done
for pkg in "${graded[@]}"; do
    while [ "$(jobs -rp | wc -l)" -ge "$JOBS" ]; do wait -n; done
    ( rm -f "integration/$pkg"/out_*.txt
      make $MAKE_ARGS -k -C "integration/$pkg" test > /dev/null 2>&1 ) &
done
wait
for dir in integration/*/; do
    pkg=$(basename "$dir")
    case "$pkg" in harness|minimig_system|apparatus-*) continue ;; esac
    if [ ! -f "integration/$pkg/Makefile" ]; then
        echo "SPEC  $pkg   specification only, not built"
        continue
    fi
    for core in fx68k tg68k; do
        line=$(grep -E '^[0-9]+ passed, [0-9]+ failed' "integration/$pkg/out_$core.txt" 2>/dev/null | tail -1)
        if [ -z "$line" ]; then
            echo "FAIL  $pkg/$core   no summary: build error"
            rc=1
        elif echo "$line" | grep -q ' 0 failed'; then
            echo "PASS  $pkg/$core   $line"
        else
            echo "FAIL  $pkg/$core   $line"
            rc=1
        fi
    done
done

exit $rc
