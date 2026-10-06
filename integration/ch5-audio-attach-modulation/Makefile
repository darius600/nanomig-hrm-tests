# ch5-audio-attach-modulation: see SPECIFICATION.md.

TEST_CPP := test_ch5_audio_attach_modulation.cpp
PROGS    := prog_fixture prog_fixture_mut prog_fixture_noplay \
            prog_rmod prog_rmod_pc6 prog_rmod_pc7 prog_rmod_pc7frame \
            prog_rmod_nodma prog_rmod_noaud prog_rmod_vol0 \
            prog_rdacp4 prog_rdacvp prog_rdac3v prog_rdac3p \
            prog_rvol1 prog_rvol1_mut prog_rvol2 prog_rvol3 \
            prog_rper1 prog_rper1_mut prog_rper2 prog_rper3 \
            prog_rramp \
            prog_s_av prog_s_av_noattach prog_s_av_per6000 prog_s_av_mv33 prog_s_av_per1write \
            prog_s_ap prog_s_ap_per6000 \
            prog_s_both prog_s_both_per6000 prog_s_both_volonly prog_s_both_volonly_per \
            prog_s_p2 prog_s_p2_noattach \
            prog_s_pair0 prog_s_pair0_noattach prog_s_pair0_extra prog_s_pair0_tgtper \
            prog_s_pair1 prog_s_pair1_noattach \
            prog_s_pair2 prog_s_pair2_noattach prog_s_pair2_extra \
            prog_s_pair4 prog_s_pair4_noattach prog_s_pair4_extra prog_s_pair4_tgtvol \
            prog_s_pair5 prog_s_pair5_noattach prog_s_pair5_extra \
            prog_s_pair6 prog_s_pair6_noattach prog_s_pair6_extra \
            prog_s_ch3v prog_s_ch3v_extra \
            prog_s_ch3p prog_s_ch3p_extra \
            prog_s_dac3v prog_s_dac3p \
            prog_s_lena prog_s_lena_len4 prog_s_lena_ch1len2 \
            prog_s_lenb

include ../harness/nanomig.mk

# Page-model run: the same package against a private copy of the NanoMig audio
# RTL in which the attach bits of ADKCON are implemented as pages 149-151 and
# 164-165 describe them (page_model.patch). The pinned submodule is never
# modified; the copy is made under obj_dir_pm/.
#   make pm-test-fx68k    runs the fx68k build against the copy
#   make pm-test-tg68k    runs the TG68K build against the copy
.PHONY: pm-rtl pm-test pm-test-fx68k pm-test-tg68k
pm-rtl:
	rm -rf obj_dir_pm
	mkdir -p obj_dir_pm
	cp -r $(NANOMIG)/src/minimig-aga obj_dir_pm/minimig-aga
	cd obj_dir_pm/minimig-aga && patch -p1 < $(PKG_DIR)/page_model.patch

pm-test-fx68k: pm-rtl
	$(MAKE) -B test-fx68k MINIMIG_DIR=$(PKG_DIR)/obj_dir_pm/minimig-aga

pm-test-tg68k: pm-rtl
	$(MAKE) -B test-tg68k MINIMIG_DIR=$(PKG_DIR)/obj_dir_pm/minimig-aga

pm-test: pm-test-fx68k pm-test-tg68k
