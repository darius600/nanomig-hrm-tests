# ch6-blitter-cpu-sharing: see SPECIFICATION.md.

TEST_CPP := test_blitter_cpu_sharing.cpp
PROGS    := prog_fixture prog_fixture_mut \
            prog_pointer prog_pointer_mutant prog_pointer_mutant_a prog_pointer_mutant_b \
            prog_pointer_mutant_c prog_pointer_mutant_d \
            prog_sharing prog_sharing_share prog_sharing_nasty prog_sharing_tiny \
            prog_sharing_aligned prog_sharing_nasty_tiny \
            prog_earlycheck prog_earlycheck_large prog_earlycheck_reordered \
            prog_bitplane prog_bitplane_hires prog_bitplane_planes6 prog_bitplane_planes5 \
            prog_bitplane_aligned prog_bitplane_planes6_aligned \
            prog_bitplane_aligned_p1 prog_bitplane_aligned_p2 prog_bitplane_aligned_p3 \
            prog_copyloop \
            prog_tas_share prog_tas_span prog_tas_span_mut \
            prog_descblit prog_descblit_offby1 prog_descblit_offby2 \
            prog_fastrun prog_fastrun_long prog_fastrun_chipcode prog_fastrun_long_chipcode \
            prog_fastrun_lockout prog_fastrun_lockout_chipcode prog_fastrun_lockout_2plane \
            prog_bbusy_lockout prog_bbusy_lockout_2plane prog_bbusy_lockout_intena

include ../harness/nanomig.mk
