# ch5-audio-attach-modulation: see SPECIFICATION.md.

TEST_CPP := test_ch5_audio_attach_modulation.cpp
PROGS    := prog_fixture prog_fixture_mut prog_fixture_noplay \
            prog_rmod prog_rmod_pc6 prog_rmod_pc7 prog_rmod_pc7frame \
            prog_rmod_nodma prog_rmod_len7 prog_rmod_per6000 prog_rmod_vol0 \
            prog_rdacp4 prog_rdacvp prog_rdac3v prog_rdac3p \
            prog_rvol1 prog_rvol1_mut prog_rvol2 prog_rvol3 \
            prog_rper1 prog_rper1_mut prog_rper2 prog_rper3 \
            prog_rramp \
            prog_s_av prog_s_av_noattach prog_s_av_per6000 prog_s_av_per1write \
            prog_s_av_vol1write prog_s_av_shift \
            prog_s_av_swap prog_s_av_ch3 prog_s_av_stop prog_s_av_zero prog_s_av_ch1bytes \
            prog_s_ap prog_s_ap_per6000 prog_s_ap_swap prog_s_ap_shift prog_s_ap_pulse \
            prog_s_both prog_s_both_per6000 prog_s_both_volonly prog_s_both_volonly_per \
            prog_s_both_swap prog_s_both_v32 prog_s_both_perconst prog_s_both_shift \
            prog_s_both_per4050 \
            prog_s_p2 prog_s_p2_swap prog_s_p2_per1write prog_s_p2_ramp prog_s_p2_w129 \
            prog_s_pair0 prog_s_pair0_swap prog_s_pair0_tgtper prog_s_pair0_attc \
            prog_s_pair1 prog_s_pair1_swap prog_s_pair1_attc \
            prog_s_pair2 prog_s_pair2_swap prog_s_pair2_attc \
            prog_s_pair4 prog_s_pair4_swap prog_s_pair4_tgtvol prog_s_pair4_attc prog_s_pair4_attd \
            prog_s_pair5 prog_s_pair5_swap prog_s_pair5_attc prog_s_pair5_attd \
            prog_s_pair6 prog_s_pair6_swap prog_s_pair6_attc prog_s_pair6_attd \
            prog_s_ch3v prog_s_ch3v_att \
            prog_s_ch3p prog_s_ch3p_att \
            prog_s_dac3v prog_s_dac3p \
            prog_s_lena prog_s_lena_len10 prog_s_lena_swap prog_s_lena_ch1len2 \
            prog_s_lena_per1write prog_s_lena_ch1len1000 \
            prog_s_lenb

include ../harness/nanomig.mk

# Page-model run: the same package against a private copy of the NanoMig audio
# RTL in which the attach bits of ADKCON are implemented as pages 149-151 and
# 164-165 describe them (page_model.patch). The pinned submodule is never
# modified; the copy is made under obj_dir_$(PM_NAME)/. The page-model builds use
# their own object directories and output files (obj_dir_$(PM_NAME)_<core>/,
# out_$(PM_NAME)_<core>.txt), so they never replace the ordinary run's.
#   make pm-test-fx68k    runs the fx68k build against the copy
#   make pm-test-tg68k    runs the TG68K build against the copy
# PM_PATCH and PM_NAME name another reading of the pages (another patch file and
# its own directories) for a comparison run; the defaults are the page model.
PM_PATCH ?= $(PKG_DIR)/page_model.patch
PM_NAME  ?= pm
PM_RTL   := $(PKG_DIR)/obj_dir_$(PM_NAME)/minimig-aga

.PHONY: pm-rtl pm-test pm-test-fx68k pm-test-tg68k pm-run-fx68k pm-run-tg68k
pm-rtl:
	rm -rf obj_dir_$(PM_NAME)
	mkdir -p obj_dir_$(PM_NAME)
	cp -r $(NANOMIG)/src/minimig-aga obj_dir_$(PM_NAME)/minimig-aga
	cd obj_dir_$(PM_NAME)/minimig-aga && patch -p1 < $(PM_PATCH)

pm-test-fx68k: pm-rtl
	$(MAKE) -B pm-run-fx68k MINIMIG_DIR=$(PM_RTL) PM_NAME=$(PM_NAME)

pm-test-tg68k: pm-rtl
	$(MAKE) -B pm-run-tg68k MINIMIG_DIR=$(PM_RTL) PM_NAME=$(PM_NAME)

pm-test: pm-test-fx68k pm-test-tg68k

# The two recipes below are test-fx68k and test-tg68k of nanomig.mk with the
# object directory and the output file renamed.
pm-run-fx68k: $(BINS)
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_$(PM_NAME)_fx68k -cc --exe --build -j 0 \
		$(MINIMIG_FILES) $(FX68K_FILES) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_$(PM_NAME)_fx68k/
	( cd obj_dir_$(PM_NAME)_fx68k && ./Vnanomig $(PKG_DIR) ) > out_$(PM_NAME)_fx68k.txt; rc=$$?; cat out_$(PM_NAME)_fx68k.txt; exit $$rc

pm-run-tg68k: $(BINS)
	verilator $(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR \
		-I$(NANOMIG)/sim \
		--Mdir obj_dir_$(PM_NAME)_tg68k -cc --exe --build -j 0 \
		$(call tg68k_files,$(MINIMIG_FILES)) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_$(PM_NAME)_tg68k && ./Vnanomig $(PKG_DIR) ) > out_$(PM_NAME)_tg68k.txt; rc=$$?; cat out_$(PM_NAME)_tg68k.txt; exit $$rc
