# ch5-audio-output-scanrate: see SPECIFICATION.md and SPECIFICATION-min-period.md.

TEST_CPP := test_ch5_audio_all.cpp
PROGS    := prog_audio \
            prog_audio_a1mut prog_audio_a2mut prog_audio_a3mut prog_audio_a4mut prog_audio_fixturemut \
            prog_audio_fixwritemut prog_audio_a6mut prog_audio_p1lastmut prog_t2s_a6mut prog_nodone \
            prog_t2s prog_t2s_fixturemut prog_t2s_a2mut prog_t2s_fixwritemut \
            prog_adkcon prog_adkcon_p5mut prog_adkcon_c1mut prog_adkcon_c1dupmut prog_adkcon_fixturemut \
            prog_diskrate prog_diskrate_fastf0 prog_diskrate_fasts1 \
            prog_diskrate_b2 prog_diskrate_b3 prog_diskrate_f1mut \
            prog_diskrate_f1mut_ntsc prog_diskrate_fixturemut prog_diskrate_fixwritemut \
            prog_minper prog_minper_mc2dma prog_minper_mc2adk prog_minper_mc3len prog_minper_mc3oth prog_minper_mc3a prog_minper_mc3b prog_minper_mc3f4 prog_minper_mc4len \
            prog_minper_mc4cpu prog_minper_mc5mark prog_minper_mc5stop prog_minper_mp5ch1 prog_minper_mp6len prog_minper_fixmp

include ../harness/nanomig.mk

# Implementation-copy run: this package against a private copy of the NanoMig
# chip-set RTL, made by applying ntsc_long.patch to a copy of the pinned
# submodule's files. The submodule is never modified. The copy is made under
# obj_dir_lc/, each core builds in its own object directory
# (obj_dir_lc_<core>/) and writes its own output file (out_lc_<core>.txt), so
# obj_dir_<core>/ and out_<core>.txt stay those of the unmodified build.
#
#   lc: ntsc_long.patch -- the long NTSC lines are 228 counts long and the
#       others 227 (0 ... $E3 and 0 ... $E2 alternating). The record of
#       MP5-RATE (SPECIFICATION-min-period.md, MP5-RATE item 6).
#         NMH_ONLY=RATE make lc-test-fx68k
#         NMH_ONLY=RATE make lc-test-tg68k
# The directory of the copy is shared by its two cores and is removed and made
# again by lc-rtl: build the two cores one after the other (lc-test does),
# never in two shells at once.
LC_DIR   := $(PKG_DIR)/obj_dir_lc/minimig-aga
LC_FILES := $(patsubst $(MINIMIG_DIR)/%,$(LC_DIR)/%,$(MINIMIG_FILES))

.PHONY: lc-rtl lc-test lc-test-fx68k lc-test-tg68k

lc-rtl:
	rm -rf obj_dir_lc
	mkdir -p obj_dir_lc
	cp -r $(MINIMIG_DIR) obj_dir_lc/minimig-aga
	cd obj_dir_lc/minimig-aga && patch -p1 < $(PKG_DIR)/ntsc_long.patch

lc-test-fx68k: lc-rtl $(BINS)
	rm -rf obj_dir_lc_fx68k out_lc_fx68k.txt
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_lc_fx68k -cc --exe --build -j 0 \
		$(LC_FILES) $(FX68K_FILES) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_lc_fx68k/
	( cd obj_dir_lc_fx68k && ./Vnanomig $(PKG_DIR) ) > out_lc_fx68k.txt; rc=$$?; cat out_lc_fx68k.txt; exit $$rc

lc-test-tg68k: lc-rtl $(BINS)
	rm -rf obj_dir_lc_tg68k out_lc_tg68k.txt
	verilator $(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR \
		-I$(NANOMIG)/sim \
		--Mdir obj_dir_lc_tg68k -cc --exe --build -j 0 \
		$(call tg68k_files,$(LC_FILES)) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_lc_tg68k && ./Vnanomig $(PKG_DIR) ) > out_lc_tg68k.txt; rc=$$?; cat out_lc_tg68k.txt; exit $$rc

lc-test: lc-test-fx68k lc-test-tg68k
