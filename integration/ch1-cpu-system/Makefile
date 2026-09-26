# ch1-cpu-system: see SPECIFICATION.md.
#
# VASMFLAGS=-m68010: only p5_vbr.s/p5_mutb.s use MOVEC (68010+); vasm
# defaults to plain 68000 and rejects it otherwise. -m68010 only widens
# which instructions vasm accepts -- it does not change how any 68000
# opcode used elsewhere in this package is encoded.

VASMFLAGS := -m68010

TEST_CPP := test_ch1_cpu_system.cpp
PROGS    := p1 p1_move p1_muta p1_mutb p1_mutc \
            p1_mutc1 p1_mutc2 p1_mutc3 p1_mutc4 \
            p2 p2_low p2_muta p2_mutb p2_mutc \
            p3 p3_mutnop p3_mutc p3_mutd \
            p5_68000 p5_vbr p5_muta p5_mutb p5_mutc \
            p6 p6_mute

include ../harness/nanomig.mk
