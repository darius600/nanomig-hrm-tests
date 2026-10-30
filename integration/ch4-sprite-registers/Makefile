# ch4-sprite-registers: see SPECIFICATION.md.

TEST_CPP := test_ch4_sprite_registers.cpp
PROGS    := prog_manual prog_manual_p4read prog_manual_p4write prog_manual_p4pre1 \
            prog_manual_p4pre2 prog_manual_p5prop prog_manual_p5ref prog_manual_p5pre1 \
            prog_manual_p5pre2 prog_manual_fixture \
            prog_auto prog_auto_p6prop prog_auto_p6ref prog_auto_p6pre1 prog_auto_p6pre2 \
            prog_auto_p7prop prog_auto_fixture prog_autoodd prog_autoodd_c1 \
            prog_cpuptr prog_cpuptr_prop prog_cpuptr_pre1 prog_cpuptr_pre2 prog_cpuptr_pre3 prog_cpuptr_fixture \
            prog_jump prog_jump_prop prog_jump_pre1 prog_jump_pre2 prog_jump_pre3 prog_jump_fixture \
            prog_blit prog_blit_prop prog_blit_pre1 prog_blit_pre2 prog_blit_pre3 prog_blit_fixture

# NODONE (a program that never writes DONE: the contrast of every RUN-* line and of the DONE clause
# of FIXTURE-CB, FIXTURE-BEAM and FIXTURE-SHR), LOWRD (a non-refresh read at a low address: the
# contrast of the R5 anchors), P4NOPOS (the MEASURED run), AUTOODD_PRE1
PROGS    += prog_manual_nodone prog_manual_lowrd prog_manual_p4nopos prog_auto_nodone prog_auto_lowrd \
            prog_autoodd_nodone prog_autoodd_pre1 prog_cpuptr_nodone prog_jump_nodone prog_blit_nodone \
            prog_cb_o2_nodone prog_beam_nodone prog_shr_nodone

# COPBLT-v (P12, P13): o = OCS, e = ECS; the next character is the COPCON value
# 2 = $0002, F = $FFFF, 0 = $0000, D = $FFFD.
PROGS    += prog_cb_o2 prog_cb_oF prog_cb_o0 prog_cb_oD \
            prog_cb_e2 prog_cb_eF prog_cb_e0 prog_cb_eD \
            prog_cb_o2_s81 prog_cb_e2_s81 prog_cb_oF_b0 prog_cb_eF_b0 prog_cb_o2_l1end prog_cb_e2_l1end \
            prog_cb_e2_sizh3 prog_cb_o2_nocop prog_cb_o2_noblt prog_cb_o2_cpuearly \
            prog_cb_e2_nocop prog_cb_e2_noblt prog_cb_e2_cpuearly prog_cb_oD_b1 \
            prog_cb_eD_b1 prog_cb_o0_cpublt80 prog_cb_e0_cpublt80 prog_cb_o0_cpuwr \
            prog_cb_e0_cpuwr prog_cb_o2_cpuwr prog_cb_e2_cpuwr prog_cb_o2_fix \
            prog_cb_e2_cpusizh prog_cb_e0_cpusizh

# BEAM (P14)
# (prog_beam_o*: the OCS run, which writes BEAMCON0 sixteen times)
PROGS    += prog_beam prog_beam_b0 prog_beam_nolace prog_beam_fix prog_beam_short prog_beam_lowrd \
            prog_beam_o prog_beam_o_nolace prog_beam_o_short prog_beam_o_lowrd prog_beam_o_color \
            prog_beam_earlypal

# SHR-SWEEP, SHR-LORES, BITS-OCS (P1, P2, P3)
PROGS    += prog_shr prog_shr_nosh1 prog_shr_sref79 prog_shr_g0h5 prog_shr_fix prog_shr_lowrd prog_shr_u2cl prog_shrl_sw \
            prog_shrl prog_shrl_sref79 prog_shrl_g0h5 prog_shrl_u2cl \
            prog_shro prog_shro_u1sh0 prog_shro_hires prog_shro_u4cl prog_shro_g0h5 prog_shro_sref79

# FASTPT (P7's necessity clause): the Copper's sprite-0 pointer names a fast-RAM address
PROGS    += prog_autofast prog_autofast_nodone prog_autofast_fix prog_autofast_same \
            prog_autofast_bad prog_autofast_pre1 prog_autofast_nos4

# NOPOS (P5-NOPOS): MANUAL with field B's SPR0POS write left out, and its contrasts
PROGS    += prog_manual_nopos_green prog_manual_nopos_nodone prog_manual_nopos_fix \
            prog_manual_nopos_pre1 prog_manual_nopos_pre2 prog_manual_nopos_pre3 prog_manual_nopos_lowrd \
            prog_manual_nopos_new prog_manual_nopos_dis

# SWEEP (P13): thirty-two COPCON values, one Copper field each; o = OCS, e = ECS
PROGS    += prog_cbsw_o prog_cbsw_e prog_cbsw_o_bitset prog_cbsw_e_bitset prog_cbsw_o_bitclr \
            prog_cbsw_o_ctrl prog_cbsw_o_nocop prog_cbsw_o_short prog_cbsw_o_cpudead prog_cbsw_o_cpuwr

include ../harness/nanomig.mk

# Implementation-copy runs: the same package against a private copy of the NanoMig chip-set RTL,
# made by applying a patch file of this package to a copy of the pinned submodule's files. The
# submodule is never modified; each copy is made under its own directory (obj_dir_sh/,
# obj_dir_bc/), and each core builds in its own object directory (obj_dir_<copy>_<core>/) and
# writes its own output file (out_<copy>_<core>.txt), so obj_dir_<core>/ and out_<core>.txt stay
# those of the unmodified build, which `make test` uses. NMH_MUTATE selects a line as for the
# unmodified build (the environment is passed through).
#
#   sh: shsh1.patch -- the records of P1 and P2 that name a private copy
#         make sh-test-fx68k      NMH_MUTATE=P2-PROP make sh-test-fx68k
#         make sh-test-tg68k
#   bc: ocs_beamcon0.patch -- the record of P14's OCS clause
#         make bc-test-fx68k
#         make bc-test-tg68k
# The directory of a private copy (obj_dir_<copy>/) is shared by its two cores and is removed and
# made again by <copy>-rtl: build the two cores of a copy one after the other (<copy>-test does),
# never in two shells at once.
SH_DIR   := $(PKG_DIR)/obj_dir_sh/minimig-aga
SH_FILES := $(patsubst $(MINIMIG_DIR)/%,$(SH_DIR)/%,$(MINIMIG_FILES))
BC_DIR   := $(PKG_DIR)/obj_dir_bc/minimig-aga
BC_FILES := $(patsubst $(MINIMIG_DIR)/%,$(BC_DIR)/%,$(MINIMIG_FILES))

.PHONY: sh-rtl sh-test sh-test-fx68k sh-test-tg68k bc-rtl bc-test bc-test-fx68k bc-test-tg68k

# shsh1.patch applies to the two files with their line ends as LF
sh-rtl:
	rm -rf obj_dir_sh
	mkdir -p obj_dir_sh
	cp -r $(MINIMIG_DIR) obj_dir_sh/minimig-aga
	cd obj_dir_sh/minimig-aga && for f in denise_sprites_shifter.v denise_sprites.v; do tr -d '\r' < $$f > $$f.lf && mv $$f.lf $$f; done
	cd obj_dir_sh/minimig-aga && patch -p1 < $(PKG_DIR)/shsh1.patch

sh-test-fx68k: sh-rtl $(BINS)
	rm -rf obj_dir_sh_fx68k out_sh_fx68k.txt
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_sh_fx68k -cc --exe --build -j 0 \
		$(SH_FILES) $(FX68K_FILES) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_sh_fx68k/
	( cd obj_dir_sh_fx68k && ./Vnanomig $(PKG_DIR) ) > out_sh_fx68k.txt; rc=$$?; cat out_sh_fx68k.txt; exit $$rc

sh-test-tg68k: sh-rtl $(BINS)
	rm -rf obj_dir_sh_tg68k out_sh_tg68k.txt
	verilator $(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR \
		-I$(NANOMIG)/sim \
		--Mdir obj_dir_sh_tg68k -cc --exe --build -j 0 \
		$(call tg68k_files,$(SH_FILES)) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_sh_tg68k && ./Vnanomig $(PKG_DIR) ) > out_sh_tg68k.txt; rc=$$?; cat out_sh_tg68k.txt; exit $$rc

sh-test: sh-test-fx68k sh-test-tg68k

bc-rtl:
	rm -rf obj_dir_bc
	mkdir -p obj_dir_bc
	cp -r $(MINIMIG_DIR) obj_dir_bc/minimig-aga
	cd obj_dir_bc/minimig-aga && patch -p1 < $(PKG_DIR)/ocs_beamcon0.patch

bc-test-fx68k: bc-rtl $(BINS)
	rm -rf obj_dir_bc_fx68k out_bc_fx68k.txt
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_bc_fx68k -cc --exe --build -j 0 \
		$(BC_FILES) $(FX68K_FILES) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_bc_fx68k/
	( cd obj_dir_bc_fx68k && ./Vnanomig $(PKG_DIR) ) > out_bc_fx68k.txt; rc=$$?; cat out_bc_fx68k.txt; exit $$rc

bc-test-tg68k: bc-rtl $(BINS)
	rm -rf obj_dir_bc_tg68k out_bc_tg68k.txt
	verilator $(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR \
		-I$(NANOMIG)/sim \
		--Mdir obj_dir_bc_tg68k -cc --exe --build -j 0 \
		$(call tg68k_files,$(BC_FILES)) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_bc_tg68k && ./Vnanomig $(PKG_DIR) ) > out_bc_tg68k.txt; rc=$$?; cat out_bc_tg68k.txt; exit $$rc

bc-test: bc-test-fx68k bc-test-tg68k
