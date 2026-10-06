# appF-cia-system: two 8520 CIAs -- timer PB outputs, CRA/CRB bit layout,
# the phi2 clock. See SPECIFICATION.md.

TEST_CPP := test_appF_cia_system.cpp
PROGS    := p1_ciaa_ta p1_ciaa_tb p1_ciab_ta p1_ciab_tb p1_ciab_tb_pre11 p1_ciab_tb_pre12 p1_ciab_tb_pre13 \
            p1_ciab_tb_pre14 p1_ciaa_ta_pbon0 p1_ciaa_tb_pbon0 p1_ciab_ta_pbon0 p1_ciab_tb_pbon0 p1_ciab_tb_lp \
            p1_ciab_tb_otherbits p2_ciaa_ta p2_ciaa_tb p2_ciab_ta p2_ciab_tb p2_ciab_tb_pre22 p2_ciab_tb_pre23 \
            p2_ciab_tb_pre24 ep1_contrast ep2_contrast p7_ciaa_start p7_ciab_start p8_ciaa_start p8_ciab_start \
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
            p8_ciab_inmode01_short p7_ciaa_inmode_ctl_nostart1 p8_ciaa_inmode01_ctl p865_ciaa_mode11_short \
            p865_ciaa_mode11_w2300 p9_legf_ciab_tb_l40_nostart p87_ciab_x p87_ciab_y p87_ciab_x_nolsb p87_ciab_x_short \
            p87_ciaa_x p87_ciaa_y p1_ciab_tb_pbon0_pre11 p1_ciab_tb_pbon0_pre12 p1_ciab_tb_pbon0_pre13

include ../harness/nanomig.mk
