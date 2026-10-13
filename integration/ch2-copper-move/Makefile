# ch2-copper-move: see SPECIFICATION.md.
#
# Every program assembles with vasm's default 68000 mode. Every program is
# assembled a second time with a listing (<name>.lst): the test reads the
# listings to show that the CPU program executes no data access to a region
# another channel uses (SPECIFICATION.md Section 6). Each listing is checked
# against its image byte for byte before it is used.

VASMFLAGS :=

TEST_CPP := test_ch2_copper_move.cpp
PROGS    := prog_nodone \
            prog_p1 prog_p1_a prog_p1_b prog_p1_attr \
            prog_p2 prog_p2_attr \
            prog_p3 prog_p3_b prog_p3_dest prog_p3_fast prog_p3_attr \
            prog_p4 prog_p4_b prog_p4_attr \
            prog_p5_r1 prog_p5_r1x prog_p5_r1_a prog_p5_r1_b prog_p5_r1_attr \
            prog_p5_r2 prog_p5_r2x prog_p5_r2_a prog_p5_r2_b prog_p5_r2_attr \
            prog_p5_r2c prog_p5_r2cx \
            prog_p6 prog_p6_value prog_p6_nopre prog_p6_probe2 \
            prog_p7 prog_p7_a prog_p7_b prog_p7_c prog_p7_attr prog_p7_lf0 \
            prog_p8 prog_p8_c prog_p8_len prog_p8_c_nolc prog_p8_noaud prog_p8_early prog_p8_attr \
            prog_p8_c_noaud prog_p8_c_early prog_p8_c_attr

include ../harness/nanomig.mk

LSTS := $(addsuffix .lst,$(PROGS))

%.lst: %.s $(HARNESS_DIR)/nanomig.i ch2move.i
	$(VASM) -quiet -Fbin -I$(HARNESS_DIR) $(VASMFLAGS) -L $@ -o /dev/null $<

test-fx68k test-tg68k: $(LSTS)
