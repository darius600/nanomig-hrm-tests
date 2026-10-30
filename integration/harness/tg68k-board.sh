#!/bin/sh
# tg68k-board.sh -- the TG68K the board synthesizes, as a file Verilator can compile.
#
#   tg68k-board.sh dir      [NANOMIG [MINIMIG_DIR]]   print the build directory, change nothing
#   tg68k-board.sh generate [NANOMIG [MINIMIG_DIR]]   make it if it is not there, then print it
#
# NANOMIG defaults to the repository's nanomig/ submodule, MINIMIG_DIR to $NANOMIG/src/minimig-aga.
#
# nanomig/sim/TG68KdotC_Kernel.v is a Verilog netlist of an older revision of the kernel. The boards synthesize
# $NANOMIG/src/tg68k/TG68KdotC_Kernel.vhd (with TG68K_Pack.vhd and TG68K_ALU.vhd) instead, and
# cpu_wrapper.v connects the kernel's skipFetch output in the board instance only. This script produces, for the
# NanoMig tree it is given and without touching that tree:
#
#   TG68KdotC_Kernel.v   the three VHDL files translated by GHDL's synthesis (--synth --out=verilog) with the
#                        generics of the board instance (cpu_wrapper.v, the parameter list under `ifndef VERILATOR),
#                        the ports of module TG68KdotC_Kernel lower-cased as the `ifdef VERILATOR instance names them
#   cpu_wrapper.v        a copy of $MINIMIG_DIR/cpu_wrapper.v that differs by one added line: .skipfetch(skip_fetch_i)
#                        in the `ifdef VERILATOR instance, the connection the board instance makes
#                        (line endings of the copy are LF whatever the checkout has)
#
# They are written to $TG68K_BUILD_ROOT/<key>/ (default: build/tg68k/ at the repository root). <key> is a hash of
# the three VHDL files, cpu_wrapper.v, the generics, the GHDL version and this script, so two NanoMig trees or two
# GHDL versions never share a directory. A directory is complete as soon as it exists: generation runs under a lock,
# in a temporary directory that is renamed into place, so packages that build in parallel generate once.
#
# GHDL names the tool to call (default: ghdl); only POSIX sh, sed, awk, diff and flock are used besides it.
set -eu

HERE=$(cd "$(dirname "$0")" && pwd)
REPO=$(cd "$HERE/../.." && pwd)
GHDL=${GHDL:-ghdl}
ROOT=${TG68K_BUILD_ROOT:-$REPO/build/tg68k}

# The generics of the board instance, which this script also checks against cpu_wrapper.v.
GENERICS="SR_Read=2 VBR_Stackframe=2 extAddr_Mode=2 MUL_Mode=2 DIV_Mode=2 BitField=2 MUL_Hardware=1"

die() { echo "tg68k-board.sh: $*" >&2; exit 1; }

mode=${1:-}
case "$mode" in dir|generate) ;; *) die "usage: tg68k-board.sh dir|generate [NANOMIG [MINIMIG_DIR]]" ;; esac
NM=${2:-$REPO/nanomig}
MM=${3:-$NM/src/minimig-aga}
NM=$(cd "$NM" 2>/dev/null && pwd) || die "no such NanoMig tree: $NM"
MM=$(cd "$MM" 2>/dev/null && pwd) || die "no such directory: $MM"
VHD="$NM/src/tg68k/TG68K_Pack.vhd $NM/src/tg68k/TG68K_ALU.vhd $NM/src/tg68k/TG68KdotC_Kernel.vhd"
WRAP="$MM/cpu_wrapper.v"
for f in $VHD "$WRAP"; do
    [ -f "$f" ] || die "missing input: $f"
done

command -v "$GHDL" >/dev/null 2>&1 ||
    die "GHDL not found ('$GHDL'). The TG68K builds translate the board's VHDL with GHDL (last verified with 6.0.0, mcode build). Install it (see README, Requires), set GHDL=/path/to/ghdl, or build with TG68K_KERNEL=shipped."
ghdl_version=$("$GHDL" --version | sed -n 1p) || die "'$GHDL --version' failed"

key=$({ cat $VHD "$WRAP" "$0"; echo "$GENERICS"; echo "$ghdl_version"; } | sha256sum | cut -c1-16)
OUT="$ROOT/$key"

if [ "$mode" = dir ]; then
    echo "$OUT"
    exit 0
fi
if [ -f "$OUT/TG68KdotC_Kernel.v" ] && [ -f "$OUT/cpu_wrapper.v" ]; then
    echo "$OUT"
    exit 0
fi

# The generics this script passes must be the ones of the board instance: the parameter list between
# `ifndef VERILATOR and its `endif.
have=$(awk '
    { sub(/\r$/, "") }
    /^[ \t]*`ifndef[ \t]+VERILATOR([ \t]|$)/ { n++; inb = 1; next }
    inb && /^[ \t]*`endif/          { inb = 0; next }
    inb {
        line = $0; sub(/\/\/.*/, "", line)
        while (match(line, /\.[A-Za-z_][A-Za-z0-9_]*[ \t]*\([^)]*\)/)) {
            t = substr(line, RSTART + 1, RLENGTH - 1); line = substr(line, RSTART + RLENGTH)
            name = t; sub(/[ \t]*\(.*/, "", name)
            val = t; sub(/^[^(]*\([ \t]*/, "", val); sub(/[ \t]*\)$/, "", val)
            print name "=" val
        }
    }
    END { if (n != 1) exit 3 }
' "$WRAP") || die "$WRAP: expected exactly one \`ifndef VERILATOR (the board instance's parameter list)"
have=$(printf '%s\n' "$have" | sort)
want=$(printf '%s\n' $GENERICS | sort)
if [ "$want" != "$have" ]; then
    echo "tg68k-board.sh: the generics passed to GHDL differ from the board instance in $WRAP:" >&2
    echo "  passed:" >&2; printf '%s\n' "$want" | sed 's/^/    /' >&2
    echo "  board instance:" >&2; printf '%s\n' "$have" | sed 's/^/    /' >&2
    exit 1
fi

mkdir -p "$ROOT"
exec 9> "$ROOT/.lock"
flock -w 3600 9 || die "could not take $ROOT/.lock"
if [ -f "$OUT/TG68KdotC_Kernel.v" ] && [ -f "$OUT/cpu_wrapper.v" ]; then
    echo "$OUT"
    exit 0
fi

# The work is done in a scratch directory of the local temp area (on a mounted Windows drive it takes two
# minutes instead of one second); the finished files are copied next to their final place and renamed into it.
TMP="$ROOT/$key.tmp.$$"
WORK=$(mktemp -d "${TMPDIR:-/tmp}/tg68k-board.XXXXXX") || die "cannot make a scratch directory"
trap 'rm -rf "$TMP" "$WORK"' EXIT
rm -rf "$TMP"
mkdir "$TMP"
cd "$WORK"

# a) the kernel. The VHDL is copied into the working directory first: GHDL writes each source file's path
# into the Verilog as a comment, and the translation should not depend on where the tree sits.
mkdir vhd
cp $VHD vhd/
gflags=""
for g in $GENERICS; do gflags="$gflags -g$g"; done
# shellcheck disable=SC2086
"$GHDL" --synth --std=08 -fsynopsys -frelaxed --latches --out=verilog $gflags \
    vhd/TG68K_Pack.vhd vhd/TG68K_ALU.vhd vhd/TG68KdotC_Kernel.vhd -e TG68KdotC_Kernel \
    > raw.v 2> ghdl.log || { cat ghdl.log >&2; die "GHDL failed on $NM/src/tg68k"; }
grep -q '^module TG68KdotC_Kernel' raw.v ||
    die "GHDL output has no module TG68KdotC_Kernel"

# The ports of module TG68KdotC_Kernel, from its header (the text up to the first ");").
awk '
    /^module[ \t]+TG68KdotC_Kernel([^A-Za-z0-9_]|$)/ { inm = 1 }
    inm { hdr = hdr $0 "\n"; if ($0 ~ /\);/) exit }
    END {
        while (match(hdr, /(input|output|inout)[ \t\n]+(\[[^]]*\][ \t\n]*)?[A-Za-z_][A-Za-z0-9_]*/)) {
            s = substr(hdr, RSTART, RLENGTH); hdr = substr(hdr, RSTART + RLENGTH)
            sub(/^.*[ \t\n\]]/, "", s); print s
        }
    }
' raw.v > ports.txt
[ -s ports.txt ] || die "found no ports in module TG68KdotC_Kernel"

# Lower-case, inside that module, every port name that is not lower case already.
awk '
    function lc(line,   out, tok) {
        out = ""
        while (match(line, /[A-Za-z_][A-Za-z0-9_]*/)) {
            tok = substr(line, RSTART, RLENGTH)
            out = out substr(line, 1, RSTART - 1) ((tok in up) ? tolower(tok) : tok)
            line = substr(line, RSTART + RLENGTH)
        }
        return out line
    }
    NR == FNR { if ($0 != tolower($0)) up[$0] = 1; next }
    /^module[ \t]+TG68KdotC_Kernel([^A-Za-z0-9_]|$)/ { inm = 1 }
    inm { print lc($0); if ($0 ~ /endmodule/) inm = 0; next }
    { print }
' ports.txt raw.v > TG68KdotC_Kernel.v
[ -s TG68KdotC_Kernel.v ] || die "kernel translation is empty"

# b) the wrapper: one added line in the `ifdef VERILATOR branch.
awk '
    { sub(/\r$/, "") }
    /^[ \t]*`ifdef[ \t]+VERILATOR([ \t]|$)/ { nb++; inb = 1; depth = 0; print; next }
    inb {
        if (tolower($0) ~ /skipfetch/) bad++
        if ($0 ~ /^[ \t]*`if(n?def)?[ \t]/)            depth++
        else if ($0 ~ /^[ \t]*`endif/)  { if (depth > 0) depth--; else inb = 0 }
        else if ($0 ~ /^[ \t]*`(else|elsif)/ && depth == 0) inb = 0
        if (inb && $0 ~ /^[ \t]*\.cpu\(cpucfg\),[ \t]*$/) {
            ind = $0; sub(/\..*$/, "", ind)
            print; print ind ".skipfetch(skip_fetch_i),"; na++; next
        }
    }
    { print }
    END {
        if (nb != 1)  { print "expected exactly one `ifdef VERILATOR, found " nb + 0 > "/dev/stderr"; exit 1 }
        if (bad > 0)  { print "the `ifdef VERILATOR branch already names skipFetch" > "/dev/stderr"; exit 1 }
        if (na != 1)  { print "expected exactly one \".cpu(cpucfg),\" in that branch, found " na + 0 > "/dev/stderr"; exit 1 }
    }
' "$WRAP" > cpu_wrapper.v || die "cannot connect skipFetch in $WRAP"
tr -d '\r' < "$WRAP" > cpu_wrapper.orig.v
d=$(diff cpu_wrapper.orig.v cpu_wrapper.v || true)
[ "$(printf '%s\n' "$d" | grep -c '^[<>]')" = 1 ] && printf '%s\n' "$d" | grep -q '^> [[:space:]]*\.skipfetch(skip_fetch_i),$' ||
    die "the wrapper copy must differ from $WRAP by exactly the added .skipfetch line"

cp TG68KdotC_Kernel.v cpu_wrapper.v ghdl.log "$TMP/"
cd /
rm -rf "$OUT"
mv "$TMP" "$OUT"
trap - EXIT
echo "$OUT"
