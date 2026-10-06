# ch4-sprite-registers: see SPECIFICATION.md.

TEST_CPP := test_ch4_sprite_registers.cpp
PROGS    := prog_manual prog_manual_p4read prog_manual_p4write prog_manual_p4pre1 \
            prog_manual_p4pre2 prog_manual_p5prop prog_manual_p5ref prog_manual_p5pre1 \
            prog_manual_p5pre2 prog_manual_fixture \
            prog_auto prog_auto_p6prop prog_auto_p6ref prog_auto_p6pre1 prog_auto_p6pre2 \
            prog_auto_p7prop prog_auto_fixture prog_autoodd prog_autoodd_c1 \
            prog_cpuptr prog_cpuptr_prop prog_cpuptr_pre1 prog_cpuptr_pre2 prog_cpuptr_pre3 prog_cpuptr_fixture \
            prog_jump prog_jump_prop prog_jump_pre1 prog_jump_pre2 prog_jump_pre3 prog_jump_fixture \
            prog_blit prog_blit_prop prog_blit_pre1 prog_blit_pre3 prog_blit_fixture

# COPBLT-v (P12, P13): o = OCS, e = ECS; the next character is the COPCON value
# 2 = $0002, F = $FFFF, 0 = $0000, D = $FFFD.
PROGS    += prog_cb_o2 prog_cb_oF prog_cb_o0 prog_cb_oD \
            prog_cb_e2 prog_cb_eF prog_cb_e0 prog_cb_eD \
            prog_cb_o2_s81 prog_cb_e2_s81 prog_cb_oF_s81 prog_cb_eF_s81 \
            prog_cb_e2_sizh3 prog_cb_o2_nocop prog_cb_o2_noblt prog_cb_o2_cpuearly \
            prog_cb_e2_nocop prog_cb_e2_noblt prog_cb_e2_cpuearly prog_cb_oD_cpublt80 \
            prog_cb_eD_cpublt80 prog_cb_o0_cpublt80 prog_cb_e0_cpublt80 prog_cb_o0_cpuwr \
            prog_cb_e0_cpuwr prog_cb_o2_cpuwr prog_cb_e2_cpuwr prog_cb_o2_fix

# BEAM (P14)
PROGS    += prog_beam prog_beam_b0 prog_beam_lace1 prog_beam_nolace prog_beam_fix prog_beam_short prog_beam_lowrd

# SHR-SWEEP, SHR-LORES, BITS-OCS (P1, P2, P3)
PROGS    += prog_shr prog_shr_nosh1 prog_shr_nospr prog_shr_g0h5 prog_shr_fix prog_shr_lowrd prog_shr_u2cl prog_shrl_sw \
            prog_shrl prog_shrl_nospr prog_shrl_g0h5 prog_shrl_u2cl \
            prog_shro prog_shro_u1sh0 prog_shro_shres prog_shro_u4cl prog_shro_g0h5 prog_shro_nospr

include ../harness/nanomig.mk
