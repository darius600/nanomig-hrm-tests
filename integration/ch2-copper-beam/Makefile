# ch2-copper-beam: see SPECIFICATION.md.

TEST_CPP := test_ch2_copper_beam.cpp
PROGS    := prog_a prog_a_anchor prog_a_dup prog_a_earlycop prog_a_few \
            prog_a_longpad prog_a_noe0 prog_a_nocop prog_a_nodone prog_a_nofix \
            prog_a_noe1_ntsc prog_a_ntsc prog_a_p2waited prog_a_p7early \
            prog_a_slow prog_a_tailshift prog_a_tailshift_ntsc prog_b prog_b_b254 \
            prog_b_bchain prog_b_be4 prog_b_bend prog_b_bfeoff prog_b_blate prog_b_bwaited \
            prog_b_ntsc prog_c prog_c_clate prog_c_cnext prog_c_cr1line prog_c_cshift \
            prog_c_cshiftend prog_c_ntsc prog_ddrop prog_dflathi prog_dflatlo prog_dhi \
            prog_dhi_ntsc prog_dlate prog_dlo prog_dlo_ntsc prog_dshift prog_dsweep \
            prog_dsweep_ntsc prog_e prog_e_ntsc prog_ebrdblnk prog_eblackgap prog_enobpl \
            prog_ewidefetch prog_ewidefetch_ntsc prog_ewidewin

include ../harness/nanomig.mk

# Implementation-copy runs: the same package against a private copy of the
# NanoMig chip-set RTL, made by applying a patch file of this package to a
# copy of the pinned submodule's files. The submodule is never modified; each
# copy is made under its own directory (obj_dir_op/, obj_dir_rn/), and each
# core builds in its own object directory (obj_dir_<copy>_<core>/) and writes
# its own output file (out_<copy>_<core>.txt), so obj_dir_<core>/ and
# out_<core>.txt stay those of the unmodified build, which `make test` uses.
#
#   op: odd_pal.patch -- the odd lines of a PAL field are 228 counts long
#       (0 ... $E3). The records of P2-PAL and P3-PAL (SPECIFICATION.md 8.6).
#         NMH_ONLY=A make op-test-fx68k     runs section A on the fx68k build
#         NMH_ONLY=A make op-test-tg68k     runs section A on the TG68K build
#   rn: restart_nonzero.patch -- every line starts the horizontal count at 1
#       in place of 0 and ends it at $E3 in place of $E2 (still 227 counts).
#       The record of P1-ORIGIN (SPECIFICATION.md 8.6).
#         NMH_ONLY=A make rn-test-fx68k
#         NMH_ONLY=A make rn-test-tg68k
OP_DIR   := $(PKG_DIR)/obj_dir_op/minimig-aga
OP_FILES := $(patsubst $(MINIMIG_DIR)/%,$(OP_DIR)/%,$(MINIMIG_FILES))
RN_DIR   := $(PKG_DIR)/obj_dir_rn/minimig-aga
RN_FILES := $(patsubst $(MINIMIG_DIR)/%,$(RN_DIR)/%,$(MINIMIG_FILES))

.PHONY: op-rtl op-test op-test-fx68k op-test-tg68k rn-rtl rn-test rn-test-fx68k rn-test-tg68k
op-rtl:
	rm -rf obj_dir_op
	mkdir -p obj_dir_op
	cp -r $(MINIMIG_DIR) obj_dir_op/minimig-aga
	cd obj_dir_op/minimig-aga && patch -p1 < $(PKG_DIR)/odd_pal.patch

op-test-fx68k: op-rtl $(BINS)
	rm -rf obj_dir_op_fx68k out_op_fx68k.txt
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_op_fx68k -cc --exe --build -j 0 \
		$(OP_FILES) $(FX68K_FILES) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_op_fx68k/
	( cd obj_dir_op_fx68k && ./Vnanomig $(PKG_DIR) ) > out_op_fx68k.txt; rc=$$?; cat out_op_fx68k.txt; exit $$rc

op-test-tg68k: op-rtl $(BINS)
	rm -rf obj_dir_op_tg68k out_op_tg68k.txt
	verilator $(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR \
		-I$(NANOMIG)/sim \
		--Mdir obj_dir_op_tg68k -cc --exe --build -j 0 \
		$(OP_FILES) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_op_tg68k && ./Vnanomig $(PKG_DIR) ) > out_op_tg68k.txt; rc=$$?; cat out_op_tg68k.txt; exit $$rc

op-test: op-test-fx68k op-test-tg68k

rn-rtl:
	rm -rf obj_dir_rn
	mkdir -p obj_dir_rn
	cp -r $(MINIMIG_DIR) obj_dir_rn/minimig-aga
	cd obj_dir_rn/minimig-aga && patch -p1 < $(PKG_DIR)/restart_nonzero.patch

rn-test-fx68k: rn-rtl $(BINS)
	rm -rf obj_dir_rn_fx68k out_rn_fx68k.txt
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_rn_fx68k -cc --exe --build -j 0 \
		$(RN_FILES) $(FX68K_FILES) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_rn_fx68k/
	( cd obj_dir_rn_fx68k && ./Vnanomig $(PKG_DIR) ) > out_rn_fx68k.txt; rc=$$?; cat out_rn_fx68k.txt; exit $$rc

rn-test-tg68k: rn-rtl $(BINS)
	rm -rf obj_dir_rn_tg68k out_rn_tg68k.txt
	verilator $(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR \
		-I$(NANOMIG)/sim \
		--Mdir obj_dir_rn_tg68k -cc --exe --build -j 0 \
		$(RN_FILES) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_rn_tg68k && ./Vnanomig $(PKG_DIR) ) > out_rn_tg68k.txt; rc=$$?; cat out_rn_tg68k.txt; exit $$rc

rn-test: rn-test-fx68k rn-test-tg68k
