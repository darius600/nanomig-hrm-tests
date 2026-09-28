# appF-cia-system: two 8520 CIAs -- timer PB outputs, CRA/CRB bit layout,
# the phi2 clock. See SPECIFICATION.md.

TEST_CPP := test_appF_cia_system.cpp
PROGS    := p1_ciaa_ta p1_ciaa_tb p1_ciab_ta p1_ciab_tb \
            p1_ciab_tb_pre11 p1_ciab_tb_pre12 p1_ciab_tb_pre13 p1_ciab_tb_pre14 \
            p1_ciaa_ta_pbon0 p1_ciaa_tb_pbon0 p1_ciab_ta_pbon0 p1_ciab_tb_pbon0 \
            p1_ciab_tb_lp p1_ciab_tb_otherbits \
            p2_ciaa_ta p2_ciaa_tb p2_ciab_ta p2_ciab_tb \
            p2_ciab_tb_pre21 p2_ciab_tb_pre22 p2_ciab_tb_pre23 p2_ciab_tb_pre24 \
            p2_ciab_tb_contrast p2_ciab_tb_legh0 \
            ep1_contrast ep2_contrast ep3_contrast \
            p7_ciaa_start p7_ciab_start p8_ciaa_start p8_ciab_start \
            p7_ciab_start_stpre p7_ciab_start_contrast p8_ciab_start_stpre p8_ciab_start_contrast \
            p7_ciaa_run p7_ciab_run p7_ciab_run_pad p8_ciaa_run p8_ciab_run p8_ciab_run_pad \
            p7_ciaa_load p7_ciab_load p7_ciab_load_ld0 p7_ciab_load_run01 \
            p8_ciaa_load p8_ciab_load p8_ciab_load_ld0 p8_ciab_load_run01 \
            p7_ciaa_unused81 p7_ciaa_unused01 p7_ciab_unused81 p7_ciab_unused01 \
            p7_ciab_unused_c1contrast p7_ciab_unused_wcontrast \
            p7_ciaa_inmode p7_ciaa_inmode_phi2 p7_ciab_inmode p7_ciab_inmode_phi2 \
            p8_ciaa_inmode01 p8_ciaa_inmode01_phi2 p8_ciab_inmode01 p8_ciab_inmode01_phi2 \
            kp_a kp_b \
            p76_ciab p76_ciab_contrast p76_ciab_pre \
            p865_ciaa_mode10 p865_ciab_mode10 p865_ciab_mode10_contrast \
            p865_ciaa_mode11 \
            p87_ciab p87_ciaa p87_ciab_nolsb \
            p5a_ciaa_ta_d1 p5a_ciaa_ta_d2 p5a_ciaa_tb_d1 p5a_ciaa_tb_d2 \
            p5a_ciab_ta_d1 p5a_ciab_ta_d2 p5a_ciab_tb_d1 p5a_ciab_tb_d2 \
            p5a_ciab_tb_d2_pre51 p5a_ciab_tb_d1_pre52 p5a_ciab_tb_d1_cnt p5a_ciab_tb_d2_cnt \
            p5b_ciaa_ta p5b_ciaa_tb p5b_ciab_ta p5b_ciab_tb \
            p6_k123 \
            p3_ciaa_ta p3_ciaa_tb p3_ciab_ta p3_ciab_tb \
            p3_ciab_tb_l2 p3_ciab_tb_nostart p3_ciab_tb_pulse p3_ciab_tb_pbon0 \
            p4_ciaa_ta_l20 p4_ciaa_ta_l21 p4_ciaa_ta_l22 p4_ciaa_ta_l23 p4_ciaa_ta_l24 p4_ciaa_ta_l25 \
            p4_ciaa_tb_l20 p4_ciaa_tb_l21 p4_ciaa_tb_l22 p4_ciaa_tb_l23 p4_ciaa_tb_l24 p4_ciaa_tb_l25 \
            p4_ciab_ta_l20 p4_ciab_ta_l21 p4_ciab_ta_l22 p4_ciab_ta_l23 p4_ciab_ta_l24 p4_ciab_ta_l25 \
            p4_ciab_tb_l20 p4_ciab_tb_l21 p4_ciab_tb_l22 p4_ciab_tb_l23 p4_ciab_tb_l24 p4_ciab_tb_l25 \
            p4_ciab_tb_l20_toggle p4_ciab_tb_l20_nostart \
            p9_legw_l40 p9_legw_l100 \
            p9_legf_ciab_tb_l40 p9_legf_ciab_tb_l41 p9_legf_ciab_tb_l64 p9_legf_ciab_tb_l100

include ../harness/nanomig.mk
