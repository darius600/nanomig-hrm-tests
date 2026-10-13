# ch5-audio-registers-dma: see SPECIFICATION.md.

TEST_CPP := test_ch5_audio_registers_dma.cpp
PROGS    := prog_fixture prog_fixture_mut prog_fixture_mutr \
            prog_p1 prog_p1_c1 prog_p1_c2 prog_p1_c3 \
            prog_p1_g1 prog_p1_g2 prog_p1_g3 prog_p1_g4 prog_p1_g5 \
            prog_p2 prog_p2_omit prog_p2_g1 \
            prog_p3 prog_p3_c1 prog_p3_g1 prog_p3_g2 prog_p3_g3 prog_p3_g4 prog_p3_g5 \
            prog_p4 prog_p4_c1 prog_p4_g1 prog_p4_g2 prog_p4_g3 prog_p4_g4 prog_p4_g5 \
            prog_p5 prog_p5_c1 prog_p5_g1 prog_p5_g2 prog_p5_g3 prog_p5_g3in prog_p5_g4 prog_p5_g5 \
            prog_p6 prog_p6_c1 prog_p6_g1 prog_p6_g2 prog_p6_g3 prog_p6_g4 prog_p6_g5 \
            prog_p7 prog_p7_c1 prog_p7_gearly prog_p7_donly prog_p7_donly_c1 prog_p7_donly_g1 prog_p7_donly_g2 prog_p7_donly_g3 prog_p7_donly_gearly prog_p7_g1 prog_p7_g2 prog_p7_g3 \
            prog_p8 prog_p8_c1 prog_p8_g1 prog_p8_g4 prog_p8_gbltpri \
            prog_p9 prog_p9_c1 prog_p9_g1 prog_p9_g2 prog_p9_gnowait prog_p9_gdwrite prog_p9_gwrites \
            prog_p10 prog_p10_cdang prog_p10_g1 prog_p10_g2k prog_p10_g2w prog_p10_g3 prog_p10_g3l \
            prog_p10_g4 prog_p10_g5 prog_p10_g_nolist \
            prog_p11 prog_p11_g1 prog_p11_g2 prog_p11_g3 prog_p11_g3q prog_p11_g4 prog_p11_g5 prog_p11_g_nolist

include ../harness/nanomig.mk

# Implementation-copy run: the same package against a private copy of the
# design in which DMACONR bit 14 reads 0 once a blit has run for 1500 clocks,
# while the blit is still running (private_copy.patch; see the P8 record in
# the test). The pinned submodule is never modified; the copy is made under
# obj_dir_priv/, and each core builds in its own object directory and writes
# its own output file (obj_dir_priv_<core>/, out_priv_<core>.txt), so
# obj_dir_<core>/ and out_<core>.txt stay those of the unmodified build.
#   NMH_ONLY=P8 make priv-test-fx68k    runs the fx68k build against the copy
#   NMH_ONLY=P8 make priv-test-tg68k    runs the TG68K build against the copy
# NMH_ONLY=P2,P8,P9 selects the DMACONR aggregate with its three halves.
PRIV_DIR   := $(PKG_DIR)/obj_dir_priv/minimig-aga
PRIV_FILES := $(patsubst $(MINIMIG_DIR)/%,$(PRIV_DIR)/%,$(MINIMIG_FILES))

.PHONY: priv-rtl priv-test priv-test-fx68k priv-test-tg68k
priv-rtl:
	rm -rf obj_dir_priv
	mkdir -p obj_dir_priv
	cp -r $(MINIMIG_DIR) obj_dir_priv/minimig-aga
	cd obj_dir_priv/minimig-aga && patch -p1 < $(PKG_DIR)/private_copy.patch

priv-test-fx68k: $(BINS) priv-rtl
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_priv_fx68k -cc --exe --build -j 0 \
		$(PRIV_FILES) $(FX68K_FILES) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_priv_fx68k/
	( cd obj_dir_priv_fx68k && ./Vnanomig $(PKG_DIR) ) > out_priv_fx68k.txt; rc=$$?; cat out_priv_fx68k.txt; exit $$rc

priv-test-tg68k: $(BINS) priv-rtl
	verilator $(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR \
		-I$(NANOMIG)/sim \
		--Mdir obj_dir_priv_tg68k -cc --exe --build -j 0 \
		$(PRIV_FILES) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_priv_tg68k && ./Vnanomig $(PKG_DIR) ) > out_priv_tg68k.txt; rc=$$?; cat out_priv_tg68k.txt; exit $$rc

priv-test: priv-test-fx68k priv-test-tg68k
