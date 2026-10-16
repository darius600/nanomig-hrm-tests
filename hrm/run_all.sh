#!/bin/bash
# Rebuild and run every test in a package tree, one summary line per test.
#
#   ./run_all.sh             rebuild from scratch (make -B) and run hrm/
#   ./run_all.sh --fast      use existing binaries where possible
#   ./run_all.sh --in ../aa  run a different tree, such as the AA packages
#   ./run_all.sh --jobs 8    build and run up to 8 packages at a time
#
# Exit code 0 if every test passed, 1 otherwise. A build failure is reported as
# its own outcome (BUILD) and never counted as a passing or failing test.
#
# --in exists because this runner iterates `*/` relative to its own directory:
# without it the AA packages would never be built or counted, and a red in one
# of them would never surface while hrm/ reported clean beside it. Without --in
# nothing about this script's behaviour changes, so the published hrm/ figures
# are the same figures.
#
# --jobs runs the packages in parallel and prints the same lines, in the same
# order, once all of them have finished. Every package builds into its own
# directory, so the packages do not share anything a parallel run could race on.
cd "$(dirname "$0")" || exit 2

MAKE_ARGS="-B"
TREE="."
JOBS=1
while [ $# -gt 0 ]; do
    case "$1" in
        --fast) MAKE_ARGS=""; shift ;;
        --in)   TREE="$2"; shift 2 ;;
        --jobs) JOBS="$2"; shift 2 ;;
        *)      echo "unknown argument: $1" >&2; exit 2 ;;
    esac
done
[ -d "$TREE" ] || { echo "no such tree: $TREE" >&2; exit 2; }
cd "$TREE" || exit 2
case "$JOBS" in ''|*[!0-9]*|0) echo "--jobs needs a positive number" >&2; exit 2 ;; esac

pass=0; fail=0; build=0
declare -a failed_list
declare -a build_list

# One package's line, from the output of its make and make's exit code.
report() {
    local name="$1" out="$2" rc="$3" counts
    # Every summary the package printed, summed: a package may build several
    # programs and each declares its own, so keeping only the last one showed
    # a red package as "5 passed, 0 failed". A summary is a line that IS one --
    # at most a label and a colon before the count, at most a qualifier after
    # it -- so a record quoting another package's figures in mid-sentence is
    # not added in. The same rule as the pipeline's graded_lines.SUMMARY_RES.
    counts=$(echo "$out" \
        | grep -iE '^[[:space:]]*=*[[:space:]]*([=[:space:]]*[[:alnum:]_.-]+[[:space:]]*:[[:space:]]*)?[0-9]+[[:space:]]+passed,[[:space:]]*[0-9]+[[:space:]]+failed(,[[:space:]]*[0-9]+[[:space:]]+[a-z]+)*([[:space:]]*\([^()]{0,120}\))?\.?[[:space:]]*=*[[:space:]]*$' \
        | grep -oiE '[0-9]+[[:space:]]+passed,[[:space:]]*[0-9]+[[:space:]]+failed' \
        | awk '{ gsub(/[^0-9]+/, " "); p += $1; f += $2; n++ } END { if (n) printf "%d passed, %d failed", p, f }')
    # A build error means the binary never ran. Detect that by the absence of
    # any evidence of execution - not by the absence of a "N passed" summary,
    # since the tests do not share one output format. (An earlier revision used
    # that weaker check and mislabelled three genuine, known-red tests as build
    # errors.)
    if ! echo "$out" | grep -qE '^(PASS|FAIL)[: ]|^\./obj|Assertions|passed,'; then
        echo "BUILD $name"
        build=$((build+1)); build_list+=("$name"); return
    fi
    if [ "$rc" -eq 0 ]; then
        echo "PASS  $name   ${counts:-(no assertion summary)}"
        pass=$((pass+1))
    else
        echo "FAIL  $name   ${counts:-(no assertion summary)}"
        fail=$((fail+1)); failed_list+=("$name")
    fi
}

packages=()
for dir in */; do
    name="${dir%/}"
    [ -f "$name/Makefile" ] && packages+=("$name")
done

if [ "$JOBS" -eq 1 ]; then
    for name in "${packages[@]}"; do
        out=$(make $MAKE_ARGS -C "$name" 2>&1)
        rc=$?
        report "$name" "$out" "$rc"
    done
else
    work=$(mktemp -d) || exit 2
    trap 'rm -rf "$work"' EXIT
    for name in "${packages[@]}"; do
        while [ "$(jobs -rp | wc -l)" -ge "$JOBS" ]; do wait -n; done
        ( make $MAKE_ARGS -C "$name" > "$work/$name.out" 2>&1; echo $? > "$work/$name.rc" ) &
    done
    wait
    for name in "${packages[@]}"; do
        report "$name" "$(cat "$work/$name.out")" "$(cat "$work/$name.rc")"
    done
fi

echo
echo "tests: $pass passed, $fail failed, $build build errors"
[ ${#failed_list[@]} -gt 0 ] && echo "failing: ${failed_list[*]}"
[ ${#build_list[@]} -gt 0 ] && echo "build errors: ${build_list[*]}"
[ $fail -eq 0 ] && [ $build -eq 0 ]
