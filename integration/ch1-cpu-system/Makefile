# ch1-cpu-system: see SPECIFICATION.md.
#
# Every program assembles with vasm's default 68000 mode. Only p5_vbr.s and
# the images built from it use MOVEC (68010+ only), so only those get
# -m68010. vasm's -m68010 mode rewrites every MOVE.x #0,<ea> as CLR.x <ea>
# (the two differ on the bus: CLR reads first, MOVE does not), which would
# silently turn P1's own MOVE contrast, and every program's CLEAR_OVL
# move.b #0,CIAAPRA, into CLR -- so the default stays plain 68000 elsewhere.
#
# The P5 images (P5.b's static check compares two of them byte for byte, so
# all of them share one assembly setting) add -no-opt: without it,
# p5_vbr.bin's own CLEAR_OVL would still shrink to CLR under -m68010 even
# with the flag above, and -no-opt on only some of the images would make
# vasm's other size choices (branch/addressing shrinking) differ between
# them for no reason P5 cares about.
#
# Every program is assembled a second time with a listing (<name>.lst). The
# test reads the listings: P3.c decodes every instruction's effective-address
# path from them, and P5.b locates step 3 in them. Each listing is checked
# against its image byte for byte before it is used.
VASMFLAGS :=

P5_68K := p5_68000 p5_muta p5_mutc \
          p5_mutc1 p5_mutc2 p5_mutc3 p5_mutc4 p5_mutnodone p5_mutpeek
P5_VBR := p5_vbr p5_mutb p5_mutspan \
          p5_vbr_mutc1 p5_vbr_mutc2 p5_vbr_mutc3 p5_vbr_mutc4 \
          p5_vbr_mutnodone p5_vbr_mutpeek

$(addsuffix .bin,$(P5_68K)) $(addsuffix .lst,$(P5_68K)): VASMFLAGS := -no-opt
$(addsuffix .bin,$(P5_VBR)) $(addsuffix .lst,$(P5_VBR)): VASMFLAGS := -m68010 -no-opt

TEST_CPP := test_ch1_cpu_system.cpp
PROGS    := p1 p1_move p1_move_mutc p1_muta p1_mutb p1_mutc \
            p1_mutc1 p1_mutc2 p1_mutc3 p1_mutc3f p1_mutc4 p1_mutc4b \
            p1_mutord p1_mutnodone p1_mutpeek \
            p2 p2_low p2_muta p2_mutb p2_mutc \
            p2_mutc1 p2_mutc2 p2_mutc3 p2_mutc4 p2_mutnodone p2_mutpeek \
            p3 p3_muta p3_mutord p3_mutb p3_mutc p3_mutcpc p3_mutcreg p3_mutd \
            p3_mutwin p3_mutcal \
            p3_mutc1 p3_mutc2 p3_mutc3 p3_mutc4 p3_mutnodone p3_mutpeek \
            $(P5_68K) $(P5_VBR) \
            p6 p6_muta p6_mutb p6_mutc p6_mutd p6_mute p6_mutalign \
            p6_mutc1 p6_mutc2 p6_mutc3 p6_mutc4 p6_mutnodone p6_mutpeek

include ../harness/nanomig.mk

LSTS := $(addsuffix .lst,$(PROGS))

%.lst: %.s $(HARNESS_DIR)/nanomig.i ch1.i
	$(VASM) -quiet -Fbin -I$(HARNESS_DIR) $(VASMFLAGS) -L $@ -o /dev/null $<

test-fx68k test-tg68k: $(LSTS)
