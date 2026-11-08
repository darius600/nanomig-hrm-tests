# Integration package aa-integ-s1: see SPECIFICATION.md.

TEST_CPP := test_aa_integ_s1.cpp
PROGS    := prog_bars prog_bars_nofix prog_bars_midrun prog_bars_midrun_nofix prog_regs prog_regs_nofix prog_lpen prog_lpen_c prog_lpen_late prog_lpen_var prog_lpen_nofix prog_joy prog_joy_nofix prog_uart prog_uart_k prog_uart_early prog_uart_nofix prog_disk prog_disk_nofix prog_dual prog_dual_cpu prog_dual_nofix prog_dual_pad121 prog_dual_sync prog_uhres prog_uhres_nofix prog_uhres_slow prog_dual_sync24 prog_dual_sync70

include ../harness/nanomig.mk

# The implementation record (SPECIFICATION.md section 4, Mutations): a private copy of the chip set in which the BPLCON1
# register resets to $0000 (private/bplcon1reset.patch), built on both cores in obj_dir_private_bplcon1reset/ and run
# there with NMH_MUTATE=P5-NONE. The outputs are out_private_bplcon1reset_<core>.txt.
#   make private-bplcon1reset
.PHONY: private-bplcon1reset
private-bplcon1reset: private/bplcon1reset.patch
	rm -rf obj_dir_private_bplcon1reset
	mkdir -p obj_dir_private_bplcon1reset/nanomig obj_dir_private_bplcon1reset/integration/aa-integ-s1
	cp -r $(NANOMIG)/src $(NANOMIG)/sim obj_dir_private_bplcon1reset/nanomig/
	cp -r ../harness obj_dir_private_bplcon1reset/integration/
	cp Makefile *.s *.i $(TEST_CPP) obj_dir_private_bplcon1reset/integration/aa-integ-s1/
	patch -s -p1 -d obj_dir_private_bplcon1reset/nanomig < private/bplcon1reset.patch
	NMH_MUTATE=P5-NONE $(MAKE) -C obj_dir_private_bplcon1reset/integration/aa-integ-s1 -k -B test NANOMIG=$(abspath obj_dir_private_bplcon1reset/nanomig); \
	rc=$$?; \
	cp obj_dir_private_bplcon1reset/integration/aa-integ-s1/out_fx68k.txt out_private_bplcon1reset_fx68k.txt; \
	cp obj_dir_private_bplcon1reset/integration/aa-integ-s1/out_tg68k.txt out_private_bplcon1reset_tg68k.txt; \
	exit $$rc
