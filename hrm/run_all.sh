#!/bin/bash
# Rebuild and run every HRM test under hrm/, one summary line per test.
#
#   ./run_all.sh            rebuild from scratch (make -B) and run
#   ./run_all.sh --fast     use existing binaries where possible
#
# Exit code 0 if every test passed, 1 otherwise. A build failure is reported as
# its own outcome (BUILD) and never counted as a passing or failing test.
cd "$(dirname "$0")" || exit 2

MAKE_ARGS="-B"
[ "$1" = "--fast" ] && MAKE_ARGS=""

pass=0; fail=0; build=0
declare -a failed_list
declare -a build_list

for dir in */; do
    name="${dir%/}"
    [ -f "$name/Makefile" ] || continue
    out=$(make $MAKE_ARGS -C "$name" 2>&1)
    rc=$?
    counts=$(echo "$out" | grep -oE '[0-9]+ passed, [0-9]+ failed' | tail -1)
    # A build error means the binary never ran. Detect that by the absence of
    # any evidence of execution - not by the absence of a "N passed" summary,
    # since the tests do not share one output format. (An earlier revision used
    # that weaker check and mislabelled three genuine, known-red tests as build
    # errors.)
    if ! echo "$out" | grep -qE '^(PASS|FAIL)[: ]|^\./obj|Assertions|passed,'; then
        echo "BUILD $name"
        build=$((build+1)); build_list+=("$name"); continue
    fi
    if [ $rc -eq 0 ]; then
        echo "PASS  $name   ${counts:-(no assertion summary)}"
        pass=$((pass+1))
    else
        echo "FAIL  $name   ${counts:-(no assertion summary)}"
        fail=$((fail+1)); failed_list+=("$name")
    fi
done

echo
echo "tests: $pass passed, $fail failed, $build build errors"
[ ${#failed_list[@]} -gt 0 ] && echo "failing: ${failed_list[*]}"
[ ${#build_list[@]} -gt 0 ] && echo "build errors: ${build_list[*]}"
[ $fail -eq 0 ] && [ $build -eq 0 ]
