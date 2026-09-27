# ch1-cpu-system: see SPECIFICATION.md.
#
# Every program assembles with vasm's default 68000 mode. Only p5_vbr.s and
# p5_mutb.s use MOVEC (68010+ only), so only those two targets get
# -m68010. vasm's -m68010 mode rewrites every MOVE.x #0,<ea> as CLR.x <ea>
# (the two differ on the bus: CLR reads first, MOVE does not), which would
# silently turn P1's own MOVE contrast, and every program's CLEAR_OVL
# move.b #0,CIAAPRA, into CLR -- so the default stays plain 68000 elsewhere.
#
# The five P5 images (P5.b's static check compares two of them byte for
# byte, so all five share one assembly setting) add -no-opt: without it,
# p5_vbr.bin/p5_mutb.bin's own CLEAR_OVL would still shrink to CLR under
# -m68010 even with the flag above, and -no-opt on only some of the five
# would make vasm's other size choices (branch/addressing shrinking) differ
# between them for no reason P5 cares about.
VASMFLAGS :=

p5_68000.bin p5_muta.bin p5_mutc.bin: VASMFLAGS := -no-opt
p5_vbr.bin p5_mutb.bin:               VASMFLAGS := -m68010 -no-opt

TEST_CPP := test_ch1_cpu_system.cpp
PROGS    := p1 p1_move p1_muta p1_mutb p1_mutc \
            p1_mutc1 p1_mutc2 p1_mutc3 p1_mutc4 \
            p2 p2_low p2_muta p2_mutb p2_mutc \
            p3 p3_mutnop p3_mutc p3_mutd \
            p5_68000 p5_vbr p5_muta p5_mutb p5_mutc \
            p6 p6_mute p6_mutalign

include ../harness/nanomig.mk
