# appF-cia-system: two 8520 CIAs -- timer PB outputs, CRA/CRB bit layout,
# the phi2 clock. See SPECIFICATION.md.

TEST_CPP := test_appF_cia_system.cpp
PROGS    := p1_ciaa_ta p1_ciaa_tb p1_ciab_ta p1_ciab_tb p1_ciab_tb_pre11 p1_ciab_tb_pre12 p1_ciab_tb_pre13 \
            p1_ciab_tb_pre14 p1_ciaa_ta_pbon0 p1_ciaa_tb_pbon0 p1_ciab_ta_pbon0 p1_ciab_tb_pbon0 p1_ciab_tb_lp \
            p1_ciab_tb_otherbits p2_ciaa_ta p2_ciaa_tb p2_ciab_ta p2_ciab_tb p2_ciab_tb_pre22 p2_ciab_tb_pre23 \
            p2_ciab_tb_pre24 p2_ciab_tb_pre21 ep1_contrast ep2_contrast p7_ciaa_start p7_ciab_start p8_ciaa_start p8_ciab_start \
            p7_ciab_start_stpre p7_ciab_start_contrast p8_ciab_start_stpre p8_ciab_start_contrast p7_ciaa_run \
            p7_ciab_run p7_ciab_run_pad p8_ciaa_run p8_ciab_run p8_ciab_run_pad p7_ciaa_load p7_ciab_load \
            p7_ciab_load_ld0 p7_ciab_load_run01 p8_ciaa_load p8_ciab_load p8_ciab_load_ld0 p8_ciab_load_run01 \
            p7_ciaa_unused81 p7_ciaa_unused01 p7_ciab_unused81 p7_ciab_unused01 p7_ciab_unused_c1contrast \
            p7_ciab_unused81_startoff p7_ciaa_inmode p7_ciaa_inmode_phi2 p7_ciab_inmode \
            p7_ciab_inmode_phi2 p7_ciab_inmode_nostart p8_ciaa_inmode01 p8_ciaa_inmode01_phi2 p8_ciab_inmode01 \
            p8_ciab_inmode01_phi2 p8_ciab_inmode01_nostart kp_a kp_b p76_ciab p76_ciab_contrast p76_ciab_pre \
            p76_ciaa p865_ciaa_mode10 p865_ciab_mode10 p865_ciab_mode10_contrast p865_ciab_mode10_shortlatch \
            p865_ciaa_mode11 p5a_ciaa_ta_d1 p5a_ciaa_ta_d2 p5a_ciaa_tb_d1 \
            p5a_ciaa_tb_d2 p5a_ciab_ta_d1 p5a_ciab_ta_d2 p5a_ciab_tb_d1 p5a_ciab_tb_d2 p5a_ciab_tb_d2_pre51 \
            p5a_ciab_tb_d1_pre52 p5a_ciab_tb_d1_cnt p5a_ciab_tb_d2_cnt p5b_ciaa_ta p5b_ciaa_tb p5b_ciab_ta \
            p5b_ciab_tb p6_k123 p3_ciaa_ta p3_ciaa_tb p3_ciab_ta p3_ciab_tb p3_ciab_tb_l2 p3_ciab_tb_nostart \
            p3_ciab_tb_cputoggle p3_ciab_tb_cputoggle_low p4_ciaa_ta_l20 p4_ciaa_ta_l21 p4_ciaa_ta_l22 \
            p4_ciaa_ta_l23 p4_ciaa_ta_l24 p4_ciaa_ta_l25 p4_ciaa_tb_l20 p4_ciaa_tb_l21 p4_ciaa_tb_l22 \
            p4_ciaa_tb_l23 p4_ciaa_tb_l24 p4_ciaa_tb_l25 p4_ciab_ta_l20 p4_ciab_ta_l21 p4_ciab_ta_l22 \
            p4_ciab_ta_l23 p4_ciab_ta_l24 p4_ciab_ta_l25 p4_ciab_tb_l20 p4_ciab_tb_l21 p4_ciab_tb_l22 \
            p4_ciab_tb_l23 p4_ciab_tb_l24 p4_ciab_tb_l25 p4c_ciab_ta_l40 p4c_ciab_ta_l41 p4c_ciab_ta_l42 \
            p4c_ciab_ta_l43 p4c_ciab_ta_l44 p4c_ciab_ta_l45 p4c_ciab_tb_l40 p4c_ciab_tb_l41 p4c_ciab_tb_l42 \
            p4c_ciab_tb_l43 p4c_ciab_tb_l44 p4c_ciab_tb_l45 p4c_ciab_tb_l40_two p9_legw_l40 p9_legw_l100 \
            p9_legw_l40_nouf p9_legw_l40_lvl1 p9_legw_ctl_l40 p9_legw_ctl_l100 p9_legw_ctl_l20 \
            p9_legw_ctl_l40_nouf p9_legw_ctl_l40_lvl1 p9_legf_ciab_tb_cputoggle p9_legf_ciab_tb_cputoggle_skip \
            p9_legf_ciab_tb_l40 p9_legf_ciab_tb_l41 p9_legf_ciab_tb_l64 p9_legf_ciab_tb_l100 p1_ciaa_ta_fixture \
            p1_ciaa_ta_ep3 p1_ciaa_ta_f1 p1_ciaa_tb_ep1 p1_ciaa_tb_ep2 p1_ciaa_tb_ep3 p1_ciaa_tb_f1 \
            ep2_contrast_low p1_ciab_tb_pbon0_b0 p2c_ciab_ta p2c_ciab_tb p2c_ciab_tb_legr p2c_ciab_tb_k4 \
            p2c_ciab_tb_h1 p2c_ciab_tb_pre22 p2c_ciab_tb_pre23 p2c_ciab_tb_pre24 p4_ciab_tb_l21_nostart \
            p4_ciab_tb_l22_nostart p4_ciab_tb_l24_nostart p9_legf_ciab_tb_l41_m \
            p5a_ciab_tb_d2_nostart p5b_ciab_tb_nostart p5b_ciab_tb_chip p5b_ciab_tb_short p6_k123_short \
            p6_k123_k1chip p6_k123_k3mix p6_k123_n3 p6_k123_n5 p4_ciab_tb_l20_yspread p4_ciab_tb_l20_xspread \
            p9_legw_l40_chip p7_ciaa_inmode_ctl p76_ciaa_in p7_ciab_load_nostart p7_ciab_run_short \
            p7_ciab_unused81_delay p7_ciaa_inmode_short p7_ciab_inmode_short \
            p7_ciab_start_nostart p8_ciab_start_nostart p8_ciab_load_nostart p8_ciab_run_short p8_ciaa_inmode01_short \
            p8_ciab_inmode01_short p8_ciaa_inmode01_ctl p865_ciaa_mode11_short \
            p865_ciaa_mode11_w2300 p9_legf_ciab_tb_l40_nostart p87_ciab_x p87_ciab_y p87_ciab_x_nolsb p87_ciab_x_short \
            p87_ciaa_x p87_ciaa_y p1_ciab_tb_pbon0_pre11 p1_ciab_tb_pbon0_pre12 p1_ciab_tb_pbon0_pre13

include ../harness/nanomig.mk

# Implementation-copy runs: the same package against a private copy of the chip set, made by applying a patch
# file of this package to a copy of the pinned submodule's chip-set sources. The submodule is never modified.
# Each copy and each core has its own directories (obj_dir_<copy>_rtl_<core>/ holds the patched files,
# obj_dir_<copy>_<core>/ the build) and its own output file (out_<copy>_<core>.txt); obj_dir_<core>/ and
# out_<core>.txt stay those of the unmodified build, which `make test` uses.
#
#   conf: conf.patch -- a CNT input for both CIAs that follows pp. 348-351 and 357-358: CIA-A's CNT is the
#         keyboard clock KCLK, CIA-B's CNT is POUT (undriven, pulled up). The implementation records of
#         P5-CONTRAST, P7.5, P7.5-CIAA-INMODE, P7.5-CIAB-GREENCTL, P8.65-MODE01 and P8.65-MODE01-CIAB-GREENCTL.
#           NMH_MUTATE=P75-GREENCTL,P865M01-GREENCTL,P5-CONTRAST make conf-test-fx68k
#           NMH_MUTATE=P75-GREENCTL,P865M01-GREENCTL,P5-CONTRAST make conf-test-tg68k
#   sp:   sp.patch -- NOT a conforming copy: a key code does not set CIA-A's SP flag (a fault against p. 345).
#         The implementation-red record of P7.6-CIAA.
#           make sp-test-fx68k
#           make sp-test-tg68k
# The selectors of a record are passed in NMH_MUTATE (the environment of make reaches the test).
CONF_FX68K_DIR   := $(PKG_DIR)/obj_dir_conf_rtl_fx68k/minimig-aga
CONF_FX68K_FILES := $(patsubst $(MINIMIG_DIR)/%,$(CONF_FX68K_DIR)/%,$(MINIMIG_FILES))
CONF_TG68K_DIR   := $(PKG_DIR)/obj_dir_conf_rtl_tg68k/minimig-aga
CONF_TG68K_FILES := $(patsubst $(MINIMIG_DIR)/%,$(CONF_TG68K_DIR)/%,$(MINIMIG_FILES))
SP_FX68K_DIR   := $(PKG_DIR)/obj_dir_sp_rtl_fx68k/minimig-aga
SP_FX68K_FILES := $(patsubst $(MINIMIG_DIR)/%,$(SP_FX68K_DIR)/%,$(MINIMIG_FILES))
SP_TG68K_DIR   := $(PKG_DIR)/obj_dir_sp_rtl_tg68k/minimig-aga
SP_TG68K_FILES := $(patsubst $(MINIMIG_DIR)/%,$(SP_TG68K_DIR)/%,$(MINIMIG_FILES))

.PHONY: conf-rtl-fx68k conf-test-fx68k conf-rtl-tg68k conf-test-tg68k sp-rtl-fx68k sp-test-fx68k sp-rtl-tg68k sp-test-tg68k conf-test sp-test

conf-rtl-fx68k:
	rm -rf obj_dir_conf_rtl_fx68k
	mkdir -p obj_dir_conf_rtl_fx68k
	cp -r $(MINIMIG_DIR) obj_dir_conf_rtl_fx68k/minimig-aga
	cd obj_dir_conf_rtl_fx68k/minimig-aga && patch -p1 < $(PKG_DIR)/conf.patch

conf-test-fx68k: conf-rtl-fx68k $(BINS)
	rm -rf obj_dir_conf_fx68k out_conf_fx68k.txt
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_conf_fx68k -cc --exe --build -j 0 \
		$(CONF_FX68K_FILES) $(FX68K_FILES) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_conf_fx68k/
	( cd obj_dir_conf_fx68k && ./Vnanomig $(PKG_DIR) ) > out_conf_fx68k.txt; rc=$$?; cat out_conf_fx68k.txt; exit $$rc

conf-rtl-tg68k:
	rm -rf obj_dir_conf_rtl_tg68k
	mkdir -p obj_dir_conf_rtl_tg68k
	cp -r $(MINIMIG_DIR) obj_dir_conf_rtl_tg68k/minimig-aga
	cd obj_dir_conf_rtl_tg68k/minimig-aga && patch -p1 < $(PKG_DIR)/conf.patch

conf-test-tg68k: conf-rtl-tg68k $(BINS)
	rm -rf obj_dir_conf_tg68k out_conf_tg68k.txt
	verilator $(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR -I$(NANOMIG)/sim \
		--Mdir obj_dir_conf_tg68k -cc --exe --build -j 0 \
		$(call tg68k_files,$(CONF_TG68K_FILES)) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_conf_tg68k && ./Vnanomig $(PKG_DIR) ) > out_conf_tg68k.txt; rc=$$?; cat out_conf_tg68k.txt; exit $$rc

sp-rtl-fx68k:
	rm -rf obj_dir_sp_rtl_fx68k
	mkdir -p obj_dir_sp_rtl_fx68k
	cp -r $(MINIMIG_DIR) obj_dir_sp_rtl_fx68k/minimig-aga
	cd obj_dir_sp_rtl_fx68k/minimig-aga && patch -p1 < $(PKG_DIR)/sp.patch

sp-test-fx68k: sp-rtl-fx68k $(BINS)
	rm -rf obj_dir_sp_fx68k out_sp_fx68k.txt
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_sp_fx68k -cc --exe --build -j 0 \
		$(SP_FX68K_FILES) $(FX68K_FILES) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_sp_fx68k/
	( cd obj_dir_sp_fx68k && ./Vnanomig $(PKG_DIR) ) > out_sp_fx68k.txt; rc=$$?; cat out_sp_fx68k.txt; exit $$rc

sp-rtl-tg68k:
	rm -rf obj_dir_sp_rtl_tg68k
	mkdir -p obj_dir_sp_rtl_tg68k
	cp -r $(MINIMIG_DIR) obj_dir_sp_rtl_tg68k/minimig-aga
	cd obj_dir_sp_rtl_tg68k/minimig-aga && patch -p1 < $(PKG_DIR)/sp.patch

sp-test-tg68k: sp-rtl-tg68k $(BINS)
	rm -rf obj_dir_sp_tg68k out_sp_tg68k.txt
	verilator $(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR -I$(NANOMIG)/sim \
		--Mdir obj_dir_sp_tg68k -cc --exe --build -j 0 \
		$(call tg68k_files,$(SP_TG68K_FILES)) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_sp_tg68k && ./Vnanomig $(PKG_DIR) ) > out_sp_tg68k.txt; rc=$$?; cat out_sp_tg68k.txt; exit $$rc

conf-test: conf-test-fx68k conf-test-tg68k
sp-test: sp-test-fx68k sp-test-tg68k
