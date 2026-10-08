// test_ch2_copper_control.cpp -- ch2-copper-control. See SPECIFICATION.md
// for every assertion's proposition, program, observed value and expected
// value. This file only implements what SPECIFICATION.md already
// committed to.
//
// A plain `make test` sets no selector and runs every section. The normal
// run prints the fixtures, the preconditions and the assertions; a line
// whose preconditions did not hold prints "RECORDED: <label> NOT GRADED"
// and neither PASS nor FAIL.
//
// NMH_MUTATE is a comma-separated list of mutation selectors; NMH_ONLY (a
// comma-separated list of P1, P2, P3, P4, P6, P8, P10, FIXTURE) restricts a
// replay to those sections. A selector changes one stimulus, or swaps one
// capture into the slot a line grades, and leaves the comparator and the
// expected value alone. <label>-NODONE runs a program that never stores
// DONE in a fixture's place; <label>-EMPTY runs one that reaches DONE with
// no marker and no record. The selectors of one list act on different
// lines, so the record of each selector names its own line; the lists
// that were run are in SPECIFICATION.md 5.7.
//
// Five lines are red on every plain run, on both cores: P4, P4-Run2,
// P4-Run3, P4-Run3-reorder and P4-Run3-PAIR. Each compares the exact word
// address of the first Copper fetch after DMA is turned on with the first
// word of the list the page selects, and a green control (P4-CTRL,
// P4-CTRL-C) runs the same comparator on a live input. The mutation
// records of these five lines are `stimulus green`: a program or a capture
// for which the page predicts the selected list is graded through the same
// comparator and the line goes green.
//
// Each record below is one physical line starting `// MUTATION <label>`;
// the kind is stimulus, implementation or none and the result is red,
// green or impossible. A line whose result is `impossible` says why.
//
// Four selectors are not mutations. NMH_MUTATE=P6-C and NMH_MUTATE=P6-C2
// run the program that sets LACE (BPLCON0 bit 2), alone and with one
// bitplane fetched. P6c stays PASS under both on both cores: the four
// compared video spans measure equal (SPECIFICATION.md 5.6), so LACE is
// not an input that falsifies V1. P6c's own record uses BEAMCON0.
// NMH_MUTATE=P3-A runs Base without its second DSKLEN write, so region DSK
// is silent in Base: P3a prints that as a MEASURED line and P3-DSK and
// P3-ALL are not graded; no line is graded on it, so it has no record.
// NMH_MUTATE=P3-APP-I-REFPTR and NMH_MUTATE=P3-REFRESH-REFPTR run a program
// that writes REFPTR ($028); see the two `none impossible` records.
//
// MUTATION FIXTURE stimulus red NMH_MUTATE=FIXTURE: the program stores $DEAD instead of $BEEF -- measured FAIL on "FIXTURE", both cores
// MUTATION P1-FIX-DONE stimulus red NMH_MUTATE=P1-FIX-DONE-NODONE: prog_nodone.bin, which never stores DONE, in place of prog_p1.bin -- measured FAIL on "P1-FIX-DONE", both cores
// MUTATION P1-OFF-FIX-DONE stimulus red NMH_MUTATE=P1-OFF-FIX-DONE-NODONE: prog_nodone.bin in place of prog_p1_off.bin -- measured FAIL on "P1-OFF-FIX-DONE", both cores
// MUTATION P1-FIX-STRUCT stimulus red NMH_MUTATE=P1-FIX-STRUCT-EMPTY: prog_empty.bin, which reaches DONE with no marker and no record, in place of prog_p1.bin -- measured FAIL on "P1-FIX-STRUCT", both cores
// MUTATION P1-OFF-FIX-STRUCT stimulus red NMH_MUTATE=P1-OFF-FIX-STRUCT-EMPTY: prog_empty.bin in place of prog_p1_off.bin -- measured FAIL on "P1-OFF-FIX-STRUCT", both cores
// MUTATION P1a stimulus red NMH_MUTATE=P1-A: DMACON=$8280 written before COP1LC/COPJMP1 (before the manual's own step 3, p.30) -- measured FAIL on "P1a", both cores
// MUTATION P1b stimulus red NMH_MUTATE=P1-B: DMACON=$8080 (COPEN without DMAEN) written instead of $8280 at step 5 -- measured FAIL on "P1b", both cores
// MUTATION P1c stimulus red NMH_MUTATE=P1-C: a fixed delay inserted between samples, sparse enough to fail the density anchor B2/B3 -- measured FAIL on "P1c", both cores
// MUTATION P1-OFFa stimulus red NMH_MUTATE=P1-OFFA: in the COPEN-clear case, DMACON=$8280 written before COP1LC/COPJMP1 and COPEN cleared at step 5 -- measured FAIL on "P1-OFFa", both cores
// MUTATION P1-OFFb stimulus red NMH_MUTATE=P1-OFFB: in the COPEN-clear case, DMACON=$0000 at step 5, so DMAEN stays clear as well -- measured FAIL on "P1-OFFb", both cores
// MUTATION P1-OFFc stimulus red NMH_MUTATE=P1-OFFC: in the COPEN-clear case, a fixed delay inserted between samples -- measured FAIL on "P1-OFFc", both cores
// MUTATION P1 stimulus red NMH_MUTATE=P1-SWAP: prog_p1_off.bin's capture (COPEN clear) graded in place of prog_p1.bin's -- measured FAIL on "P1", both cores
// MUTATION P1-OFF stimulus red NMH_MUTATE=P1OFF-SWAP: prog_p1.bin's capture (COPEN set) graded in place of prog_p1_off.bin's -- measured FAIL on "P1-OFF", both cores
// MUTATION P1-ON-OFF stimulus red NMH_MUTATE=P1-SWAP: prog_p1_off.bin's capture graded in place of prog_p1.bin's, so the COPEN-set half fails -- measured FAIL on "P1-ON-OFF", both cores
// MUTATION P1-ON-OFF stimulus red NMH_MUTATE=P1OFF-SWAP: prog_p1.bin's capture graded in place of prog_p1_off.bin's, so the COPEN-clear half fails -- measured FAIL on "P1-ON-OFF", both cores
// MUTATION P2-FIX-DONE stimulus red NMH_MUTATE=P2-FIX-DONE-NODONE: prog_nodone.bin in place of prog_p2.bin -- measured FAIL on "P2-FIX-DONE", both cores
// MUTATION P2-OFF-FIX-DONE stimulus red NMH_MUTATE=P2-OFF-FIX-DONE-NODONE: prog_nodone.bin in place of prog_p2_off.bin -- measured FAIL on "P2-OFF-FIX-DONE", both cores
// MUTATION P2-FIX-STRUCT stimulus red NMH_MUTATE=P2-FIX-STRUCT-EMPTY: prog_empty.bin in place of prog_p2.bin -- measured FAIL on "P2-FIX-STRUCT", both cores
// MUTATION P2-OFF-FIX-STRUCT stimulus red NMH_MUTATE=P2-OFF-FIX-STRUCT-EMPTY: prog_empty.bin in place of prog_p2_off.bin -- measured FAIL on "P2-OFF-FIX-STRUCT", both cores
// MUTATION P2a stimulus red NMH_MUTATE=P2-A: L2's MOVE replaced by $0180,$0F00 (COLOR00), which never touches INTREQ -- measured FAIL on "P2a", both cores
// MUTATION P2b stimulus red NMH_MUTATE=P2-B: INTENA=$8010 (bit 4 set, the master enable bit 14 left clear) -- measured FAIL on "P2b", both cores
// MUTATION P2c stimulus red NMH_MUTATE=P2-C: a fixed delay inserted between phase-2 samples, sparse enough to fail the density anchor B2/B3 -- measured FAIL on "P2c", both cores
// MUTATION P2 stimulus red NMH_MUTATE=P2-SWAP: prog_p2_off.bin's capture (DMACON=$0000, fetches continue) graded in place of prog_p2.bin's -- measured FAIL on "P2", both cores
// MUTATION P2-OFF stimulus red NMH_MUTATE=P2OFF-SWAP: prog_p2.bin's capture (COPEN cleared, no fetch continues) graded in place of prog_p2_off.bin's -- measured FAIL on "P2-OFF", both cores
// P3-APP-I and P3-REFRESH are apparatus lines: no claim uid, no page sentence. Their program runs
// are the same ones P1 and P3 grade; the only input that reaches DONE and moves nothing is REFPTR:
// MUTATION P3-APP-I none impossible no observable effect at this boundary: p.282 lists REFPTR ($028) as writeable; NMH_MUTATE=P3-APP-I-REFPTR runs prog_p1_refptr.bin, which writes REFPTR=$8100 and reaches DONE, and every refresh-asserted read stays at word address 0 (the line stays PASS), measured on both cores
// MUTATION P3-REFRESH none impossible no observable effect at this boundary: p.282 lists REFPTR ($028) as writeable; NMH_MUTATE=P3-REFRESH-REFPTR runs prog_p3_refptr.bin, which writes REFPTR=$8100 (byte $10200, inside region BPL, if read as a word address) and reaches DONE, and no refresh-asserted access falls inside a region (the line stays PASS), measured on both cores
// MUTATION P3-FIX-Base stimulus red NMH_MUTATE=P3-FIX-Base-NODONE: prog_nodone.bin in place of Case Base's program -- measured FAIL on "P3-FIX-Base", both cores
// MUTATION P3-FIX-BPL stimulus red NMH_MUTATE=P3-FIX-BPL-NODONE: prog_nodone.bin in place of Case BPL's program -- measured FAIL on "P3-FIX-BPL", both cores
// MUTATION P3-FIX-SPR stimulus red NMH_MUTATE=P3-FIX-SPR-NODONE: prog_nodone.bin in place of Case SPR's program -- measured FAIL on "P3-FIX-SPR", both cores
// MUTATION P3-FIX-AUD0 stimulus red NMH_MUTATE=P3-FIX-AUD0-NODONE: prog_nodone.bin in place of Case AUD0's program -- measured FAIL on "P3-FIX-AUD0", both cores
// MUTATION P3-FIX-AUD1 stimulus red NMH_MUTATE=P3-FIX-AUD1-NODONE: prog_nodone.bin in place of Case AUD1's program -- measured FAIL on "P3-FIX-AUD1", both cores
// MUTATION P3-FIX-AUD2 stimulus red NMH_MUTATE=P3-FIX-AUD2-NODONE: prog_nodone.bin in place of Case AUD2's program -- measured FAIL on "P3-FIX-AUD2", both cores
// MUTATION P3-FIX-AUD3 stimulus red NMH_MUTATE=P3-FIX-AUD3-NODONE: prog_nodone.bin in place of Case AUD3's program -- measured FAIL on "P3-FIX-AUD3", both cores
// MUTATION P3-FIX-DSK stimulus red NMH_MUTATE=P3-FIX-DSK-NODONE: prog_nodone.bin in place of Case DSK's program -- measured FAIL on "P3-FIX-DSK", both cores
// MUTATION P3-FIX-BLT stimulus red NMH_MUTATE=P3-FIX-BLT-NODONE: prog_nodone.bin in place of Case BLT's program -- measured FAIL on "P3-FIX-BLT", both cores
// MUTATION P3-FIX-COP stimulus red NMH_MUTATE=P3-FIX-COP-NODONE: prog_nodone.bin in place of Case COP's program -- measured FAIL on "P3-FIX-COP", both cores
// MUTATION P3-FIX-M stimulus red NMH_MUTATE=P3-FIX-M-NODONE: prog_nodone.bin in place of Case M's program -- measured FAIL on "P3-FIX-M", both cores
// MUTATION P3b stimulus red NMH_MUTATE=P3-B: Base's DMACON=$83FE (AUD0EN left clear) graded against P=$3FF (prog_p3_mut_b.bin) -- measured FAIL on "P3b", both cores
// MUTATION P3c stimulus red NMH_MUTATE=P3-C: SPR1PT moved to AUD0's region start, AUD0LC 256 bytes above it (prog_p3_mut_c.bin) -- measured FAIL on "P3c", both cores
// MUTATION P3d stimulus red NMH_MUTATE=P3-D: Base's sampler replaced by the sparse sampler of 2.4 (prog_p3_mut_d.bin) -- measured FAIL on "P3d", both cores
// MUTATION P3-BASE stimulus red NMH_MUTATE=P3-BASE-SWAP: Case M's capture (DMAEN clear, no region has any access) graded in Base's place -- measured FAIL on "P3-BASE", both cores
// MUTATION P3-BPL stimulus red NMH_MUTATE=P3-BPL-SWAP: Base's capture graded in Case BPL's place -- measured FAIL on "P3-BPL", both cores
// MUTATION P3-SPR stimulus red NMH_MUTATE=P3-SPR-SWAP: Base's capture graded in Case SPR's place -- measured FAIL on "P3-SPR", both cores
// MUTATION P3-AUD0 stimulus red NMH_MUTATE=P3-AUD0-SWAP: Base's capture graded in Case AUD0's place -- measured FAIL on "P3-AUD0", both cores
// MUTATION P3-AUD1 stimulus red NMH_MUTATE=P3-AUD1-SWAP: Base's capture graded in Case AUD1's place -- measured FAIL on "P3-AUD1", both cores
// MUTATION P3-AUD2 stimulus red NMH_MUTATE=P3-AUD2-SWAP: Base's capture graded in Case AUD2's place -- measured FAIL on "P3-AUD2", both cores
// MUTATION P3-AUD3 stimulus red NMH_MUTATE=P3-AUD3-SWAP: Base's capture graded in Case AUD3's place -- measured FAIL on "P3-AUD3", both cores
// MUTATION P3-DSK stimulus red NMH_MUTATE=P3-DSK-SWAP: Base's capture graded in Case DSK's place -- measured FAIL on "P3-DSK", both cores
// MUTATION P3-BLT stimulus red NMH_MUTATE=P3-BLT-SWAP: Base's capture graded in Case BLT's place -- measured FAIL on "P3-BLT", both cores
// MUTATION P3-COP stimulus red NMH_MUTATE=P3-COP-SWAP: Base's capture graded in Case COP's place -- measured FAIL on "P3-COP", both cores
// MUTATION P3-M stimulus red NMH_MUTATE=P3-M-SWAP: Base's capture graded in Case M's place -- measured FAIL on "P3-M", both cores
// MUTATION P3-ALL stimulus red NMH_MUTATE=P3-BASE-SWAP: Case M's capture graded in Base's place, so the coverage of Base fails -- measured FAIL on "P3-ALL", both cores
// MUTATION P3-ALL stimulus red NMH_MUTATE=P3-BPL-SWAP: Base's capture graded in Case BPL's place, so one channel case fails -- measured FAIL on "P3-ALL", both cores
// MUTATION P3-DSKBYTR stimulus red NMH_MUTATE=P3-DSKBYTR-SWAP: Base's DSKBYTR value graded in the DSK case's and case M's places -- measured FAIL on "P3-DSKBYTR", both cores
// MUTATION P4-FIX-Run1 stimulus red NMH_MUTATE=P4-FIX-Run1-NODONE: prog_nodone.bin in place of prog_p4_run1.bin -- measured FAIL on "P4-FIX-Run1", both cores
// MUTATION P4-FIX-Run2 stimulus red NMH_MUTATE=P4-FIX-Run2-NODONE: prog_nodone.bin in place of prog_p4_run2.bin -- measured FAIL on "P4-FIX-Run2", both cores
// MUTATION P4-FIX-Run3 stimulus red NMH_MUTATE=P4-FIX-Run3-NODONE: prog_nodone.bin in place of prog_p4_run3.bin -- measured FAIL on "P4-FIX-Run3", both cores
// MUTATION P4-FIX-Run3-reorder stimulus red NMH_MUTATE=P4-FIX-Run3-reorder-NODONE: prog_nodone.bin in place of prog_p4_run3_reorder.bin -- measured FAIL on "P4-FIX-Run3-reorder", both cores
// MUTATION P4-FIX-CTRL stimulus red NMH_MUTATE=P4-FIX-CTRL-NODONE: prog_nodone.bin in place of prog_p4_ctrl_cop1b.bin -- measured FAIL on "P4-FIX-CTRL", both cores
// MUTATION P4-FIX-CTRL-C stimulus red NMH_MUTATE=P4-FIX-CTRL-C-NODONE: prog_nodone.bin in place of prog_p4_ctrl_cop1c.bin -- measured FAIL on "P4-FIX-CTRL-C", both cores
// MUTATION P4a-Run1 stimulus red NMH_MUTATE=P4-A-RUN1: DMACON=$8080 written before the DMACONR read (prog_p4_run1_mut_a.bin) -- measured FAIL on "P4a-Run1", both cores
// MUTATION P4a-Run2 stimulus red NMH_MUTATE=P4-A-RUN2: DMACON=$8080 written before the DMACONR read (prog_p4_run2_mut_a.bin) -- measured FAIL on "P4a-Run2", both cores
// MUTATION P4a-Run3 stimulus red NMH_MUTATE=P4-A-RUN3: DMACON=$8080 written before the DMACONR read (prog_p4_run3_mut_a.bin) -- measured FAIL on "P4a-Run3", both cores
// MUTATION P4a-Run3-reorder stimulus red NMH_MUTATE=P4-A-RUN3R: DMACON=$8080 written before the DMACONR read (prog_p4_run3_reorder_mut_a.bin) -- measured FAIL on "P4a-Run3-reorder", both cores
// MUTATION P4a-CTRL stimulus red NMH_MUTATE=P4-A-CTRL: DMACON=$8080 written before the DMACONR read (prog_p4_ctrl_cop1b_mut_a.bin) -- measured FAIL on "P4a-CTRL", both cores
// MUTATION P4a-CTRL-C stimulus red NMH_MUTATE=P4-A-CTRLC: DMACON=$8080 written before the DMACONR read (prog_p4_ctrl_cop1c_mut_a.bin) -- measured FAIL on "P4a-CTRL-C", both cores
// MUTATION P4b-Run1 stimulus red NMH_MUTATE=P4-B-RUN1: the whole Run 1 sequence, DMACON write included, run once before M1 (prog_p4_run1_mut_b.bin) -- measured FAIL on "P4b-Run1", both cores
// MUTATION P4b-Run2 stimulus red NMH_MUTATE=P4-B-RUN2: the whole Run 1 sequence, DMACON write included, run once before M1 (prog_p4_run2_mut_b.bin) -- measured FAIL on "P4b-Run2", both cores
// MUTATION P4b-Run3 stimulus red NMH_MUTATE=P4-B-RUN3: the whole Run 1 sequence, DMACON write included, run once before M1 (prog_p4_run3_mut_b.bin) -- measured FAIL on "P4b-Run3", both cores
// MUTATION P4b-Run3-reorder stimulus red NMH_MUTATE=P4-B-RUN3R: the whole Run 1 sequence, DMACON write included, run once before M1 (prog_p4_run3_reorder_mut_b.bin) -- measured FAIL on "P4b-Run3-reorder", both cores
// MUTATION P4b-CTRL stimulus red NMH_MUTATE=P4-B-CTRL: the whole Run 1 sequence, DMACON write included, run once before M1 (prog_p4_ctrl_cop1b_mut_b.bin) -- measured FAIL on "P4b-CTRL", both cores
// MUTATION P4b-CTRL-C stimulus red NMH_MUTATE=P4-B-CTRLC: the whole Run 1 sequence, DMACON write included, run once before M1 (prog_p4_ctrl_cop1c_mut_b.bin) -- measured FAIL on "P4b-CTRL-C", both cores
// MUTATION P4c-Run1 stimulus red NMH_MUTATE=P4-C-RUN1: a frame wrap forced between M1 and the DMACON write (prog_p4_run1_mut_c.bin) -- measured FAIL on "P4c-Run1", both cores
// MUTATION P4c-Run2 stimulus red NMH_MUTATE=P4-C-RUN2: a frame wrap forced between M1 and the DMACON write (prog_p4_run2_mut_c.bin) -- measured FAIL on "P4c-Run2", both cores
// MUTATION P4c-Run3 stimulus red NMH_MUTATE=P4-C-RUN3: a frame wrap forced between M1 and the DMACON write (prog_p4_run3_mut_c.bin) -- measured FAIL on "P4c-Run3", both cores
// MUTATION P4c-Run3-reorder stimulus red NMH_MUTATE=P4-C-RUN3R: a frame wrap forced between M1 and the DMACON write (prog_p4_run3_reorder_mut_c.bin) -- measured FAIL on "P4c-Run3-reorder", both cores
// MUTATION P4c-CTRL stimulus red NMH_MUTATE=P4-C-CTRL: a frame wrap forced between M1 and the DMACON write (prog_p4_ctrl_cop1b_mut_c.bin) -- measured FAIL on "P4c-CTRL", both cores
// MUTATION P4c-CTRL-C stimulus red NMH_MUTATE=P4-C-CTRLC: a frame wrap forced between M1 and the DMACON write (prog_p4_ctrl_cop1c_mut_c.bin) -- measured FAIL on "P4c-CTRL-C", both cores
// MUTATION P4 stimulus green NMH_MUTATE=P4-RUN2-CTRL: P4 is red on every plain run; P4-CTRL's program (COP1LC=B, COPJMP1, first fetch B) run in Run 2's place through the same comparator -- measured PASS on "P4", both cores
// MUTATION P4-Run2 stimulus green NMH_MUTATE=P4-RUN2-CTRL: P4-Run2 is red on every plain run; P4-CTRL's program run in Run 2's place through the same comparator -- measured PASS on "P4-Run2", both cores
// MUTATION P4-Run3 stimulus green NMH_MUTATE=P4-RUN3-CTRL: P4-Run3 is red on every plain run; P4-CTRL's program run in Run 3's place through the same comparator -- measured PASS on "P4-Run3", both cores
// MUTATION P4-Run3-reorder stimulus green NMH_MUTATE=P4-RUN3R-CTRL: P4-Run3-reorder is red on every plain run; P4-CTRL-C's program (COP1LC=C, COPJMP1, first fetch C) run in Run 3-reorder's place through the same comparator -- measured PASS on "P4-Run3-reorder", both cores
// MUTATION P4-Run3-PAIR stimulus green NMH_MUTATE=P4-RUN3-PAIR-A: P4-Run3-PAIR is red on every plain run; P4-CTRL's and P4-CTRL-C's captures graded in Run 3's and Run 3-reorder's places through the same paired comparator -- measured PASS on "P4-Run3-PAIR", both cores
// MUTATION P4-CTRL stimulus red NMH_MUTATE=P4-CTRL-RUN1: Run 1's program (first fetch A) run in the contrast's place -- measured FAIL on "P4-CTRL", both cores
// MUTATION P4-CTRL-C stimulus red NMH_MUTATE=P4-CTRLC-RUN1: Run 1's program (first fetch A) run in the C contrast's place -- measured FAIL on "P4-CTRL-C", both cores
// MUTATION P10-FIX-DONE stimulus red NMH_MUTATE=P10-FIX-DONE-NODONE: prog_nodone.bin in place of prog_p10_main.bin -- measured FAIL on "P10-FIX-DONE", both cores
// MUTATION P10-MAIN2-FIX-DONE stimulus red NMH_MUTATE=P10-MAIN2-FIX-DONE-NODONE: prog_nodone.bin in place of prog_p10_main2.bin -- measured FAIL on "P10-MAIN2-FIX-DONE", both cores
// MUTATION P10-FIX-LINES stimulus red NMH_MUTATE=P10-FIX-LINES-EMPTY: prog_empty.bin, which writes no record, in place of prog_p10_main.bin -- measured FAIL on "P10-FIX-LINES", both cores
// MUTATION P10a stimulus red NMH_MUTATE=P10-A: the second WAIT changed from $5001 to $5A01 -- measured FAIL on "P10a", both cores
// MUTATION P10b stimulus red NMH_MUTATE=P10-B: step 2 writes INTREQ=$8004 -- measured FAIL on "P10b", both cores
// MUTATION P10c stimulus red NMH_MUTATE=P10-C: INTENA=$C004 with a level-1 handler that clears bit 2 -- measured FAIL on "P10c", both cores
// MUTATION P10 stimulus red NMH_MUTATE=P10-SWAP: prog_p10_main2.bin's capture (2nd MOVE data $8010, 3rd MOVE data $8004) graded in place of prog_p10_main.bin's -- measured FAIL on "P10", both cores
// MUTATION P10-MAIN2 stimulus red NMH_MUTATE=P10-MAIN2-SWAP: prog_p10_main.bin's capture graded in place of prog_p10_main2.bin's -- measured FAIL on "P10-MAIN2", both cores
// MUTATION P8-COP-FIX-DONE stimulus red NMH_MUTATE=P8-COP-FIX-DONE-NODONE: prog_nodone.bin in place of prog_p8_cop.bin -- measured FAIL on "P8-COP-FIX-DONE", both cores
// MUTATION P8-VB-FIX-DONE stimulus red NMH_MUTATE=P8-VB-FIX-DONE-NODONE: prog_nodone.bin in place of prog_p8_vb.bin -- measured FAIL on "P8-VB-FIX-DONE", both cores
// MUTATION P8-BLIT-FIX-DONE stimulus red NMH_MUTATE=P8-BLIT-FIX-DONE-NODONE: prog_nodone.bin in place of prog_p8_blit.bin -- measured FAIL on "P8-BLIT-FIX-DONE", both cores
// MUTATION P8-COP-DEV-FIX-DONE stimulus red NMH_MUTATE=P8-COP-DEV-FIX-DONE-NODONE: prog_nodone.bin in place of prog_p8_cop_nomove.bin -- measured FAIL on "P8-COP-DEV-FIX-DONE", both cores
// MUTATION P8-COP-III-FIX-DONE stimulus red NMH_MUTATE=P8-COP-III-FIX-DONE-NODONE: prog_nodone.bin in place of prog_p8_cop_mut_iii.bin -- measured FAIL on "P8-COP-III-FIX-DONE", both cores
// MUTATION P8-VB-DEV-FIX-DONE stimulus red NMH_MUTATE=P8-VB-DEV-FIX-DONE-NODONE: prog_nodone.bin in place of prog_p8_vb_narrow.bin -- measured FAIL on "P8-VB-DEV-FIX-DONE", both cores
// MUTATION P8-BLIT-DEV-FIX-DONE stimulus red NMH_MUTATE=P8-BLIT-DEV-FIX-DONE-NODONE: prog_nodone.bin in place of prog_p8_blit_nosize.bin -- measured FAIL on "P8-BLIT-DEV-FIX-DONE", both cores
// MUTATION P8c-COP stimulus red NMH_MUTATE=P8-C: DMACON=$8200 (no COPEN): no IR2 fetch occurs (prog_p8_cop_mut_c.bin) -- measured FAIL on "P8c-COP", both cores
// MUTATION P8c-VB stimulus red NMH_MUTATE=P8-VB-C: the capture of prog_p8_vb_narrow.bin, which polls only V=$40..$F0 and so has no wrap after MC, graded in place of prog_p8_vb.bin's -- measured FAIL on "P8c-VB", both cores
// MUTATION P8c-BLIT stimulus red NMH_MUTATE=P8-BLIT-C: the capture of prog_p8_blit_nosize.bin, which writes no BLTSIZE and so has no write in region D, graded in place of prog_p8_blit.bin's -- measured FAIL on "P8c-BLIT", both cores
// MUTATION P8a-COP stimulus red NMH_MUTATE=P8-A: INTENA=$C020 with a level-3 handler that clears bit 5 (prog_p8_cop_mut_a.bin) -- measured FAIL on "P8a-COP", both cores
// MUTATION P8a-VB stimulus red NMH_MUTATE=P8-VB-A: INTENA=$8020 (bit 5 set, master enable clear) (prog_p8_vb_mut_a.bin) -- measured FAIL on "P8a-VB", both cores
// MUTATION P8a-BLIT stimulus red NMH_MUTATE=P8-BLIT-A: INTENA=$8040 (bit 6 set, master enable clear) (prog_p8_blit_mut_a.bin) -- measured FAIL on "P8a-BLIT", both cores
// MUTATION P8b-COP stimulus red NMH_MUTATE=P8-COP-B: WAIT V=$20 and no clear, so bit 4 reads 1 at the first record after MC (prog_p8_cop_mut_b.bin) -- measured FAIL on "P8b-COP", both cores
// MUTATION P8b-VB stimulus red NMH_MUTATE=P8-B: the VB case's clear skipped (prog_p8_vb_mut_b.bin) -- measured FAIL on "P8b-VB", both cores
// MUTATION P8b-BLIT stimulus red NMH_MUTATE=P8-BLIT-B: a whole blit runs and finishes before MC, so bit 6 reads 1 at the first record after MC (prog_p8_blit_mut_b.bin) -- measured FAIL on "P8b-BLIT", both cores
// MUTATION P8d-COP stimulus red NMH_MUTATE=P8-COP-D: a delay after MC (prog_p8_cop_mut_d.bin), so no poll record is stored before the Copper's MOVE executes; the event is present -- measured FAIL on "P8d-COP", both cores
// MUTATION P8d-VB stimulus red NMH_MUTATE=P8-VB-D: MC written at V >= $130 and no record stored until line 0 or 1 (prog_p8_vb_mut_d.bin), so no record lies between MC and the frame wrap; the wrap is present -- measured FAIL on "P8d-VB", both cores
// MUTATION P8d-BLIT stimulus red NMH_MUTATE=P8-BLIT-D: the program waits for bit 6 = 1 before its first record (prog_p8_blit_mut_d.bin), so no record is stored before the blit's last write; the writes are present -- measured FAIL on "P8d-BLIT", both cores
// MUTATION P8-VB-BOUNDARY stimulus red NMH_MUTATE=P8-VB-BOUNDARY-A: VERTB set from the program at V >= $130, before the wrap, so bit 5 reads 1 in the last record before the wrap (prog_p8_vb_mut_bnd_a.bin) -- measured FAIL on "P8-VB-BOUNDARY", both cores; P8-VB and P8 print NOT GRADED
// MUTATION P8-VB-BOUNDARY stimulus red NMH_MUTATE=P8-VB-BOUNDARY-B: INTREQ=$0020 written before every poll record, so no record at line 0 or 1 reads bit 5 = 1 (prog_p8_vb_mut_bnd_b.bin) -- measured FAIL on "P8-VB-BOUNDARY", both cores; P8-VB and P8 print NOT GRADED
// MUTATION P8-COP stimulus red NMH_MUTATE=P8-COP-SWAP: the capture of prog_p8_cop_nomove.bin (the MOVE replaced by COLOR00) graded in place of prog_p8_cop.bin's -- measured FAIL on "P8-COP", both cores
// MUTATION P8-VB stimulus red NMH_MUTATE=P8-VB-SWAP: the capture of prog_p8_vb_mut_iii.bin (INTREQ=$0020 written once a record reads bit 5 = 1; the wrap is present) graded in place of prog_p8_vb.bin's -- measured FAIL on "P8-VB", both cores
// MUTATION P8-BLIT stimulus red NMH_MUTATE=P8-BLIT-SWAP: the capture of prog_p8_blit_mut_iii.bin (INTREQ=$0040 written once a record reads bit 6 = 1; the writes in region D are present) graded in place of prog_p8_blit.bin's -- measured FAIL on "P8-BLIT", both cores
// MUTATION P8 stimulus red NMH_MUTATE=P8-COP-SWAP: the COP component fails (prog_p8_cop_nomove.bin's capture in its place) -- measured FAIL on "P8", both cores
// MUTATION P8 stimulus red NMH_MUTATE=P8-VB-SWAP: the VB component fails (prog_p8_vb_mut_iii.bin's capture in its place) -- measured FAIL on "P8", both cores
// MUTATION P8 stimulus red NMH_MUTATE=P8-BLIT-SWAP: the BLIT component fails (prog_p8_blit_mut_iii.bin's capture in its place) -- measured FAIL on "P8", both cores
// MUTATION P8-COP-DEV stimulus red NMH_MUTATE=P8-COP-DEV-SWAP: the capture of prog_p8_cop.bin (the MOVE present) graded in place of the contrast's -- measured FAIL on "P8-COP-DEV", both cores
// MUTATION P8-COP-III stimulus red NMH_MUTATE=P8-COP-III-SWAP: the capture of prog_p8_cop.bin (no second clear) graded in place of the contrast's -- measured FAIL on "P8-COP-III", both cores
// MUTATION P8-VB-DEV stimulus red NMH_MUTATE=P8-VB-DEV-SWAP: the capture of prog_p8_vb.bin graded in place of the contrast's -- measured FAIL on "P8-VB-DEV", both cores
// MUTATION P8-BLIT-DEV stimulus red NMH_MUTATE=P8-BLIT-DEV-SWAP: the capture of prog_p8_blit.bin graded in place of the contrast's -- measured FAIL on "P8-BLIT-DEV", both cores
// MUTATION P6-FIX-FRAME stimulus red NMH_MUTATE=P6-FIX-FRAME-NODONE: prog_nodone.bin in place of prog_p6_main.bin -- measured FAIL on "P6-FIX-FRAME", both cores
// MUTATION P6-FIX-ROWS stimulus red NMH_MUTATE=P6-FIX-ROWS-MUTA: prog_p6_mut_a.bin (the C3 MOVE after the final WAIT, so no row has C3) in place of prog_p6_main.bin -- measured FAIL on "P6-FIX-ROWS", both cores; P6a prints NOT GRADED
// MUTATION P6-i-FIX stimulus red NMH_MUTATE=P6-i-FIX-NODONE: prog_nodone.bin in place of prog_p6_wait2_c1.bin -- measured FAIL on "P6-i-FIX", both cores
// MUTATION P6-ii-FIX stimulus red NMH_MUTATE=P6-ii-FIX-NODONE: prog_nodone.bin in place of prog_p6_wait3_b0.bin -- measured FAIL on "P6-ii-FIX", both cores
// MUTATION P6a stimulus red NMH_MUTATE=P6-A: the C3 MOVE placed after the final WAIT (prog_p6_mut_a.bin) -- measured FAIL on "P6a", both cores
// MUTATION P6b stimulus red NMH_MUTATE=P6-B: DMACON=$8200 (COPEN clear) (prog_p6_mut_b.bin) -- measured FAIL on "P6b", both cores
// MUTATION P6c stimulus red NMH_MUTATE=P6-C3: BEAMCON0 ($1DC, p.293) written with its PAL bit 5 ("Programmable pal mode enable", p.305) clear a frame and a half after MJ, so later frames have the NTSC length (262/263 lines against PAL 312/313, p.24) and the frames differ in length (prog_p6_mut_c3.bin) -- measured FAIL on "P6c", both cores
// MUTATION P6-BELOW stimulus red NMH_MUTATE=P6-BELOW-SWAP: the values of C1 and C3 exchanged (prog_p6_mut_swap13.bin) -- measured FAIL on "P6-BELOW", both cores
// MUTATION P6-ORDER stimulus red NMH_MUTATE=P6-ORDER-SWAP: the values of C0 and C1 exchanged (prog_p6_mut_swap01.bin) -- measured FAIL on "P6-ORDER", both cores
// MUTATION P6-i stimulus red NMH_MUTATE=P6-I-SWAP: prog_p6_main.bin (second WAIT unchanged) in place of prog_p6_wait2_c1.bin -- measured FAIL on "P6-i", both cores
// MUTATION P6-ii stimulus red NMH_MUTATE=P6-II-SWAP: prog_p6_main.bin (third WAIT unchanged) in place of prog_p6_wait3_b0.bin -- measured FAIL on "P6-ii", both cores

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <initializer_list>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

// nanomig.i, restated.
constexpr uint32_t kResults   = 0x0C0000;
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
constexpr uint64_t kBootBudget = 6'000'000;

// copctl.i, restated: this package's own low-memory (post-CLEAR_OVL) and
// scratch layout.
constexpr uint32_t kList1   = 0x001000;
constexpr uint32_t kList2   = 0x001200;
constexpr uint32_t kList3   = 0x001400;
constexpr uint32_t kMarkBuf = 0x0C1000;
constexpr uint32_t kRecBuf  = 0x0C4000;
constexpr uint32_t kData1 = 0x002000; // copctl.i DATA1 (P8 BLIT case)

constexpr uint64_t kSamplerRunBudget = 24'000'000; // a handful of PAL frames

struct Checks {
    void expect(bool condition, const std::string& label) {
        std::printf("%s: %s\n", condition ? "PASS" : "FAIL", label.c_str());
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::printf("MEASURED: %s\n", msg.c_str()); }
    void record(const std::string& msg) { std::printf("RECORDED: %s\n", msg.c_str()); }
    // A line whose precondition did not hold is not graded: it prints
    // neither PASS nor FAIL.
    void skip(const std::string& id, const std::string& why) {
        std::printf("RECORDED: %s NOT GRADED: %s\n", id.c_str(), why.c_str());
    }
    int passed = 0, failed = 0;
};

std::string hexs(uint32_t v) {
    char b[16];
    std::snprintf(b, sizeof b, "0x%06X", v);
    return b;
}

// ---- capture: one simulation run, plus the RAM-port log and any words the
// program stored that a proposition wants to read back. ----------------
struct Capture {
    size_t   loaded = 0;
    bool     released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;
    std::vector<std::optional<uint16_t>> peeks;
};

Capture run_capture(const std::string& bin, uint64_t run_budget, const std::vector<uint32_t>& peek_addrs) {
    Capture c;
    nmh::Config cfg;
    nmh::NanomigHarness h(cfg);
    c.loaded = h.load_program(bin);
    if (!c.loaded) return c;
    c.released = h.boot(kBootBudget).cpu_released;
    c.done = h.run_until_write(kDoneAddr, kDoneValue, run_budget);
    c.log = h.ram_log();
    for (uint32_t a : peek_addrs) c.peeks.push_back(h.peek_logical(a));
    return c;
}

// ---- full run: keeps the harness alive so video() (P6) is available too,
// unlike Capture/run_capture above (log()/peek only). --------------------
struct FullRun {
    std::unique_ptr<nmh::NanomigHarness> h;
    bool loaded = false;
    bool done = false;
};

FullRun run_full(const std::string& bin, uint64_t run_budget) {
    FullRun r;
    nmh::Config cfg;
    r.h = std::make_unique<nmh::NanomigHarness>(cfg);
    if (!r.h->load_program(bin)) return r;
    r.loaded = true;
    r.h->boot(kBootBudget);
    r.done = r.h->run_until_write(kDoneAddr, kDoneValue, run_budget).has_value();
    return r;
}

// ---- SPECIFICATION.md 2.3: channel-fetch classification ---------------
//
// A read at a non-zero word address that is not a 68000 access. Word
// address 0 is memory refresh (measured: every refresh access reads word
// address 0, never any other address), and none of this package's
// list/data regions include word address 0. The 68000 never reads inside
// a list or data region (only writes it once, to construct it), so any
// read there, other than at word address 0, is unambiguously a channel
// fetch.
bool is_channel_read(const nmh::RamAccess& a, uint32_t byte_lo, uint32_t byte_hi) {
    if (a.is_write || a.word_addr == 0) return false;
    uint32_t b = a.word_addr * 2;
    return b >= byte_lo && b < byte_hi;
}

bool any_channel_read_in(const std::vector<nmh::RamAccess>& log, uint32_t byte_lo, uint32_t byte_hi,
                          uint64_t t_lo, uint64_t t_hi_excl) {
    for (const auto& a : log)
        if (is_channel_read(a, byte_lo, byte_hi) && a.cycle >= t_lo && a.cycle < t_hi_excl) return true;
    return false;
}

std::optional<uint64_t> first_write_cycle(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr) {
    uint32_t w = byte_addr / 2;
    for (const auto& a : log) if (a.is_write && a.word_addr == w) return a.cycle;
    return std::nullopt;
}

// ---- SPECIFICATION.md 2.4: reference frame B (beam records) -----------
//
// Each record is 4 consecutive word WRITES by the 68000 program at
// RECBUF+8*i, +2, +4, +6 (INTREQR, then VPOSR/VHPOSR as one long word, then
// VPOSR read again, per copctl.i's SAMPLE macro): found by their own store
// times at the port, exactly as 2.4 describes ("each record's stores are
// visible at the port with their times"), not guessed from buffer content.
// VPOSR/VHPOSR is two bus reads (p.216), so a record is kept only if the
// two VPOSR reads agree on V8 (bit 0): otherwise the pair straddled a wrap
// and the record is torn.
struct Rec { uint64_t t0, t1; uint16_t intreqr; int V, H; };

std::vector<Rec> decode_records(const std::vector<nmh::RamAccess>& log, uint32_t base_byte_addr, uint32_t count) {
    std::unordered_map<uint32_t, std::pair<uint64_t, uint16_t>> w;
    uint32_t base_word = base_byte_addr / 2;
    uint32_t hi_word = base_word + 4u * count;
    for (const auto& a : log) {
        if (!a.is_write || a.word_addr < base_word || a.word_addr >= hi_word) continue;
        w.emplace(a.word_addr, std::make_pair(a.cycle, a.data));
    }
    std::vector<Rec> out;
    for (uint32_t i = 0; i < count; ++i) {
        uint32_t a0 = base_word + 4 * i, a1 = a0 + 1, a2 = a0 + 2, a3 = a0 + 3;
        auto it0 = w.find(a0), it1 = w.find(a1), it2 = w.find(a2), it3 = w.find(a3);
        if (it0 == w.end() || it1 == w.end() || it2 == w.end() || it3 == w.end()) break;
        if ((it1->second.second & 1) != (it3->second.second & 1)) continue; // torn: V8 disagrees
        Rec r;
        r.t0 = it0->second.first;
        r.t1 = it3->second.first;
        r.intreqr = it0->second.second;
        uint32_t raw = (uint32_t(it1->second.second) << 16) | it2->second.second;
        r.V = (raw >> 8) & 0x1FF;
        r.H = raw & 0xFF;
        out.push_back(r);
    }
    return out;
}

struct Span { size_t begin, end; };

// Complete frames only: [wraps[k], wraps[k+1]) for consecutive wrap points.
// A wrap is a decrease from V >= 256 to V <= 1 (p.24: the vertical position
// "appears to wrap to 0" past line 255; p.219: line 0 is the start of
// vertical blank) -- not any V decrease, which a torn record could also
// produce were the tear filter above ever incomplete.
std::vector<Span> record_frames(const std::vector<Rec>& r) {
    std::vector<size_t> wraps;
    for (size_t i = 1; i < r.size(); ++i)
        if (r[i - 1].V >= 256 && r[i].V <= 1) wraps.push_back(i);
    std::vector<Span> out;
    for (size_t k = 0; k + 1 < wraps.size(); ++k) out.push_back({wraps[k], wraps[k + 1]});
    return out;
}

// "Line of a port event at time t" (2.4): a = last record stored before t,
// b = the record following the first record stored after t (so that b's
// own read, not just its store, is guaranteed after t).
std::optional<size_t> last_record_before(const std::vector<Rec>& r, uint64_t t) {
    std::optional<size_t> a;
    for (size_t i = 0; i < r.size(); ++i) {
        if (r[i].t1 < t) a = i; else break;
    }
    return a;
}
std::optional<size_t> first_record_after(const std::vector<Rec>& r, uint64_t t) {
    for (size_t i = 0; i < r.size(); ++i) if (r[i].t0 > t) return i;
    return std::nullopt;
}

// First complete frame whose own first record was stored strictly after t
// (e.g. after t(M2)): the frame-boundary analogue of "the window starts at
// t(M2)". Returns -1 if there is no such frame.
int first_frame_after(const std::vector<Rec>& r, const std::vector<Span>& frames, uint64_t t) {
    for (size_t k = 0; k < frames.size(); ++k)
        if (r[frames[k].begin].t0 > t) return (int)k;
    return -1;
}

// The time boundary between frame k-1 and frame k: the last record fully
// stored before the wrap record itself. A Copper restart's own fetch is
// causally tied to the wrap edge, so it is consistently measured a few
// hundred cycles before the wrap record's own store time; assigning that
// whole gap to the frame it starts (not the one it ends) is the right edge
// for a whole-frame membership question. Frame 0's own low boundary is 0.
uint64_t frame_lo_bound(const std::vector<Rec>& r, const std::vector<Span>& frames, size_t k) {
    if (k == 0) return 0;
    return r[frames[k].begin - 1].t1;
}

// A complete frame's own time range, built from frame_lo_bound on both
// ends so that adjacent frames' ranges share exactly one boundary point
// (disjoint, half-open) instead of each separately reaching into the gap
// around the wrap that lies between them.
std::pair<uint64_t, uint64_t> frame_time_range(const std::vector<Rec>& r, const std::vector<Span>& frames,
                                                size_t k) {
    uint64_t lo = frame_lo_bound(r, frames, k);
    uint64_t hi = (k + 1 < frames.size()) ? frame_lo_bound(r, frames, k + 1)
                  : ((frames[k].end < r.size()) ? r[frames[k].end].t0 : UINT64_MAX);
    return {lo, hi};
}

bool frame_has_read_at(const std::vector<Rec>& recs, const std::vector<Span>& frames, int frame_idx,
                        const std::vector<nmh::RamAccess>& log, uint32_t word_addr) {
    auto [lo, hi] = frame_time_range(recs, frames, (size_t)frame_idx);
    for (const auto& a : log) {
        if (a.is_write || a.word_addr != word_addr) continue;
        if (a.cycle >= lo && a.cycle < hi) return true;
    }
    return false;
}

// Anchor B1 (2.4): within a complete frame, V never decreases between
// consecutive records, checked over the graded line range [lo,hi] --
// vertical blank/sync itself is not a line any proposition in this package
// grades.
bool anchor_B1(const std::vector<Rec>& r, Span f, int lo, int hi) {
    for (size_t i = f.begin + 1; i < f.end; ++i) {
        if (r[i - 1].V < lo || r[i - 1].V > hi || r[i].V < lo || r[i].V > hi) continue;
        if (r[i].V < r[i - 1].V) return false;
    }
    return true;
}

// Anchors B2 (every line in [lo,hi] carries >= 7 records) and B3 (no line
// in [lo,hi] is missing): a density requirement of the test, not a page
// value (2.4). 7 is this package's own measured floor for the tear-checked
// SAMPLE/FAST_SAMPLE record (2.4): on both cores, an unrolled dense sampler
// reaches exactly 8 records on every graded line but one, and 7 on that
// one, in every captured frame.
std::pair<bool, bool> anchor_B2_B3(const std::vector<Rec>& r, Span f, int lo, int hi) {
    std::vector<int> cnt(hi - lo + 1, 0);
    for (size_t i = f.begin; i < f.end; ++i)
        if (r[i].V >= lo && r[i].V <= hi) ++cnt[r[i].V - lo];
    bool b2 = true, b3 = true;
    for (int c : cnt) { if (c == 0) b3 = false; if (c < 7) b2 = false; }
    return {b2, b3};
}

// A P1/P2-style capture: the run, its beam records and its frames. peeks[1]
// and peeks[2] hold the record count (RESULTS+8, RESULTS+12 high/low).
struct Timed {
    Capture cap;
    std::vector<Rec> recs;
    std::vector<Span> frames;
    std::optional<uint64_t> t_m1, t_m2;
    int f0 = -1;               // first complete frame after M2
    bool done = false;         // the program reached DONE
    bool two_frames = false;   // f0 and f0+1 are complete frames after M2
};

uint32_t rec_count(const Capture& c, size_t hi_idx) {
    return (c.peeks.size() > hi_idx + 1 && c.peeks[hi_idx] && c.peeks[hi_idx + 1])
               ? (uint32_t(*c.peeks[hi_idx]) << 16) | *c.peeks[hi_idx + 1] : 0;
}

Timed make_timed(Capture c) {
    Timed t;
    t.cap = std::move(c);
    t.done = t.cap.loaded && t.cap.done.has_value();
    if (!t.done) return t;
    t.t_m1 = first_write_cycle(t.cap.log, kMarkBuf);
    t.t_m2 = first_write_cycle(t.cap.log, kMarkBuf + 2);
    t.recs = decode_records(t.cap.log, kRecBuf, rec_count(t.cap, 1));
    t.frames = record_frames(t.recs);
    if (t.t_m2) {
        t.f0 = first_frame_after(t.recs, t.frames, *t.t_m2);
        t.two_frames = t.f0 >= 0 && (size_t)(t.f0 + 1) < t.frames.size();
    }
    return t;
}

// Anchors B1-B3 over the two complete frames after M2, on lines $10-$FF.
bool two_frame_anchors(const Timed& t) {
    if (!t.two_frames) return false;
    bool b1a = anchor_B1(t.recs, t.frames[t.f0], 0x10, 0xFF);
    bool b1b = anchor_B1(t.recs, t.frames[t.f0 + 1], 0x10, 0xFF);
    auto d1 = anchor_B2_B3(t.recs, t.frames[t.f0], 0x10, 0xFF);
    auto d2 = anchor_B2_B3(t.recs, t.frames[t.f0 + 1], 0x10, 0xFF);
    return b1a && b1b && d1.first && d1.second && d2.first && d2.second;
}

// ---- P4 --------------------------------------------------------------
//
// The first channel-fetch read (strictly) after time t, among several
// named 8-byte list regions: its time, the region it landed in, and its
// exact word address.
struct TaggedFetch { uint64_t cycle; char name; uint32_t word_addr; };
std::optional<TaggedFetch> first_fetch_among(const std::vector<nmh::RamAccess>& log, uint64_t t,
                                              const std::vector<std::pair<uint32_t, char>>& regions) {
    std::optional<TaggedFetch> best;
    for (const auto& a : log) {
        if (a.is_write || a.word_addr == 0 || a.cycle <= t) continue;
        uint32_t b = a.word_addr * 2;
        for (const auto& rb : regions) {
            if (b >= rb.first && b < rb.first + 8 && (!best || a.cycle < best->cycle)) {
                best = TaggedFetch{a.cycle, rb.second, a.word_addr};
            }
        }
    }
    return best;
}

// P4 precondition (c): the program samples nothing between t(MS) and the
// first fetch (SPECIFICATION.md P4 stimulus), so a check that only looks for a
// wrap or a low-V record strictly inside that stretch holds by
// construction, on every run, regardless of whether a vertical blank
// actually fell there. Bound the stretch instead by the records around it
// (2.4's own "line of a port event" construction, applied to t(MS) and the
// first fetch separately): a = the last record stored before t(MS); b =
// the record following the first record stored after the first fetch. No
// wrap lies between a and b, and both read V > 25 (p.219: vertical
// blanking ends at line 25 for PAL) -- so the stretch cannot have crossed
// a vertical blank even though the program itself samples nothing inside
// it.
bool bracket_holds(const std::vector<Rec>& r, uint64_t t_ms, uint64_t t_fetch) {
    auto a = last_record_before(r, t_ms);
    auto c = first_record_after(r, t_fetch);
    if (!a || !c) return false;
    size_t b = *c + 1;
    if (b >= r.size() || *a >= b) return false;
    for (size_t i = *a + 1; i <= b; ++i)
        if (r[i - 1].V >= 256 && r[i].V <= 1) return false; // a wrap lies between a and b
    return r[*a].V > 25 && r[b].V > 25;
}

// P4: one run's capture plus its decoded records and its first fetch (of
// A/B/C) after M1, reused both for the run's own assertion and for the
// preconditions, which all apply to this same style of run.
struct P4Result {
    Capture cap;
    bool ok = false;
    std::optional<uint64_t> t_ms;  // marker MS: precedes the first location-register write
    std::optional<uint64_t> t_m1;  // marker M1: precedes the DMACON write
    std::vector<Rec> recs;
    std::optional<TaggedFetch> fetch;   // first Copper fetch (A/B/C) after t(M1)
    bool precond_c = false;             // no wrap, no V<=25 record, between t(MS) and the fetch
    // The first fetch after M1 is at exactly this list's first word (IR1).
    bool first_word_is(uint32_t list_byte) const {
        return ok && fetch.has_value() && fetch->word_addr * 2 == list_byte;
    }
};

P4Result run_p4(const std::string& bin, uint64_t budget) {
    P4Result r;
    r.cap = run_capture(bin, budget, {kResults + 4, kResults + 8, kResults + 10});
    if (!r.cap.loaded || !r.cap.done.has_value()) return r;
    r.t_ms = first_write_cycle(r.cap.log, kMarkBuf + 4);
    r.t_m1 = first_write_cycle(r.cap.log, kMarkBuf);
    if (!r.t_m1) return r;
    r.recs = decode_records(r.cap.log, kRecBuf, rec_count(r.cap, 1));
    r.fetch = first_fetch_among(r.cap.log, *r.t_m1, {{kList1, 'A'}, {kList2, 'B'}, {kList3, 'C'}});
    r.ok = r.fetch.has_value();
    if (r.ok && r.t_ms)
        r.precond_c = bracket_holds(r.recs, *r.t_ms, r.fetch->cycle);
    return r;
}

// The three guards of every P4 capture (SPECIFICATION.md P4 item 5).
struct P4Guards { bool a = false, b = false, c = false; bool all() const { return a && b && c; } };

P4Guards p4_guards(const P4Result& r) {
    P4Guards g;
    g.a = r.cap.peeks.size() >= 1 && r.cap.peeks[0] && ((*r.cap.peeks[0]) & 0x0080) == 0;
    g.b = r.t_m1.has_value() &&
          !any_channel_read_in(r.cap.log, kList1, kList1 + 8, 0, *r.t_m1) &&
          !any_channel_read_in(r.cap.log, kList2, kList2 + 8, 0, *r.t_m1) &&
          !any_channel_read_in(r.cap.log, kList3, kList3 + 8, 0, *r.t_m1);
    g.c = r.ok && r.precond_c;
    return g;
}

// ---- SPECIFICATION.md 2.3: refresh recognised by the `refresh` port
// (P3; P1/P2/P4 use the word-address-0 signature instead) ----------------
//
// The harness exposes the RAM-port group's `refresh` output beside
// ram_address/ram_data, so it is read at the same point in the simulation
// loop the harness samples the RAM port at. Capture here steps one clk_sys
// cycle at a time through ports()/run_cycles(1) and records refresh's value
// each cycle, indexed the same way RamAccess.cycle is (both come
// from the same cycle counter), so refresh_at(cycle) can be asked about
// any cycle a RamAccess names.
struct CaptureR : Capture {
    std::vector<uint8_t> refresh; // refresh[i] = the port's value at cycle (refresh_base + i)
    uint64_t refresh_base = 0;
    bool refresh_at(uint64_t cycle) const {
        if (cycle < refresh_base) return false;
        uint64_t i = cycle - refresh_base;
        return i < refresh.size() && refresh[i] != 0;
    }
};

CaptureR run_capture_refresh(const std::string& bin, uint64_t run_budget,
                              const std::vector<uint32_t>& peek_addrs) {
    CaptureR c;
    nmh::Config cfg;
    nmh::NanomigHarness h(cfg);
    c.loaded = h.load_program(bin);
    if (!c.loaded) return c;
    c.released = h.boot(kBootBudget).cpu_released;
    const uint32_t done_w = kDoneAddr / 2;
    c.refresh_base = h.cycles();
    size_t seen = h.ram_log().size();
    uint64_t start = h.cycles();
    while (h.cycles() - start < run_budget) {
        h.run_cycles(1);
        c.refresh.push_back(h.ports()->refresh ? 1 : 0);
        const auto& log = h.ram_log();
        bool found = false;
        for (; seen < log.size(); ++seen)
            if (log[seen].is_write && log[seen].word_addr == done_w && log[seen].data == kDoneValue)
                { c.done = log[seen].cycle; found = true; break; }
        if (found) break;
    }
    c.log = h.ram_log();
    for (uint32_t a : peek_addrs) c.peeks.push_back(h.peek_logical(a));
    return c;
}

// ---- SPECIFICATION.md 2.8: restarts (P6) -------------------------------
//
// A restart is the first Copper fetch after a fetch of the IR2 of a list's
// final WAIT ($FFFF,$FFFE). It is identified at the port, inside the
// list's own region: the next channel read in that region after the
// final-WAIT IR2 fetch must be a read of the list's first word (IR1);
// a pair whose next read in the region is any other word is not counted.
struct Restart { uint64_t t_final; uint64_t t_restart; };

std::vector<Restart> find_list_restarts(const std::vector<nmh::RamAccess>& log, uint32_t list_byte,
                                         uint32_t list_len, uint32_t end_ir2_byte) {
    std::vector<Restart> out;
    for (size_t i = 0; i < log.size(); ++i) {
        const auto& a = log[i];
        if (!is_channel_read(a, list_byte, list_byte + list_len) || a.word_addr != end_ir2_byte / 2) continue;
        for (size_t j = i + 1; j < log.size(); ++j) {
            const auto& b = log[j];
            if (!is_channel_read(b, list_byte, list_byte + list_len)) continue;
            if (b.word_addr == list_byte / 2) out.push_back({a.cycle, b.cycle});
            break;
        }
    }
    return out;
}

// Count of channel reads at one exact word address within [lo,hi) -- P6's
// per-restart MOVE-order check.
int count_channel_reads_at(const std::vector<nmh::RamAccess>& log, uint32_t word_addr,
                            uint64_t lo, uint64_t hi) {
    int n = 0;
    for (const auto& a : log)
        if (!a.is_write && a.word_addr == word_addr && a.cycle >= lo && a.cycle < hi) ++n;
    return n;
}

// First channel read at one exact word address, strictly after after_t.
std::optional<uint64_t> first_channel_read_at(const std::vector<nmh::RamAccess>& log, uint32_t word_addr,
                                               uint64_t after_t) {
    for (const auto& a : log)
        if (!a.is_write && a.word_addr == word_addr && a.cycle > after_t) return a.cycle;
    return std::nullopt;
}

// Count/first/last of writes of one exact value inside a byte region and
// time window -- P8's BLIT case (region D filled with $FFFF).
struct WriteStats { int count = 0; std::optional<uint64_t> first, last; };
WriteStats writes_of_value_in(const std::vector<nmh::RamAccess>& log, uint16_t value,
                               uint32_t region_lo, uint32_t region_hi,
                               uint64_t lo, uint64_t hi) {
    WriteStats s;
    for (const auto& a : log) {
        if (!a.is_write || a.data != value) continue;
        uint32_t b = a.word_addr * 2;
        if (b < region_lo || b >= region_hi) continue;
        if (a.cycle < lo || a.cycle >= hi) continue;
        ++s.count;
        if (!s.first) s.first = a.cycle;
        s.last = a.cycle;
    }
    return s;
}

// ---- SPECIFICATION.md P8: one INTREQ bit polled across markers/events --
//
// A single pass over the poll records grades observation (i)-(iii):
// zero_before is (i), later_one is (ii), after_first_one is (iii).
struct P8Grade {
    bool zero_before = true, later_one = false, after_first_one = true;
};
P8Grade grade_p8_bit(const std::vector<Rec>& recs, uint64_t t_mc, uint64_t t_event, int bit) {
    P8Grade g;
    bool seen_one = false;
    for (const auto& r : recs) {
        if (r.t0 <= t_mc) continue;
        bool bitval = ((r.intreqr >> bit) & 1) != 0;
        if (r.t1 < t_event && bitval) g.zero_before = false;
        if (r.t0 > t_event && bitval) g.later_one = true;
        if (seen_one && !bitval) g.after_first_one = false;
        if (bitval) seen_one = true;
    }
    return g;
}

// Precondition (b): the first poll record stored after t(MC) reads the bit
// as 0, so a later 1 is new.
bool first_after_mc_reads_zero(const std::vector<Rec>& recs, uint64_t t_mc, int bit) {
    for (const auto& r : recs)
        if (r.t0 > t_mc) return ((r.intreqr >> bit) & 1) == 0;
    return false;
}

// ---- SPECIFICATION.md 2.5: reference frame V (P6) ----------------------
//
// Anchor V1: every one of the given video spans has the same sample count
// and the same doubler row count (nmh::find_lines).
bool anchor_V1(const std::vector<nmh::VideoSample>& video, const std::vector<nmh::Span>& spans) {
    if (spans.size() < 2) return false;
    std::vector<nmh::VideoSample> sub0(video.begin() + spans[0].begin, video.begin() + spans[0].end);
    size_t n0 = sub0.size();
    size_t rows0 = nmh::find_lines(sub0).size();
    for (size_t i = 1; i < spans.size(); ++i) {
        std::vector<nmh::VideoSample> sub(video.begin() + spans[i].begin, video.begin() + spans[i].end);
        if (sub.size() != n0) return false;
        if (nmh::find_lines(sub).size() != rows0) return false;
    }
    return true;
}

}  // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    // NMH_MUTATE is a comma-separated list of mutation selectors; each
    // selector changes one stimulus (or swaps one capture) and is named in
    // the // MUTATION records. A plain run sets none.
    const char* mutate_env = std::getenv("NMH_MUTATE");
    std::set<std::string> mutset;
    {
        std::string cur;
        for (const char* p = mutate_env ? mutate_env : ""; ; ++p) {
            if (*p == ',' || *p == '\0') { if (!cur.empty()) mutset.insert(cur); cur.clear(); }
            else cur += *p;
            if (*p == '\0') break;
        }
    }
    auto has = [&](const std::string& k) { return mutset.count(k) != 0; };
    // NMH_ONLY=P1,P4 runs only the named sections (replay convenience; a
    // plain run executes every section).
    const char* only_env = std::getenv("NMH_ONLY");
    const std::string only = only_env ? std::string(",") + only_env + "," : "";
    auto want = [&](const char* tag) {
        return only.empty() || only.find(std::string(",") + tag + ",") != std::string::npos;
    };

    // The program graded in a fixture line's place under a mutation:
    // <label>-NODONE stores no DONE mark, <label>-EMPTY reaches DONE with
    // no marker and no record.
    auto bin = [&](const char* normal, std::initializer_list<const char*> keys) -> std::string {
        for (const char* k : keys) {
            if (has(std::string(k) + "-NODONE")) return dir + "/prog_nodone.bin";
            if (has(std::string(k) + "-EMPTY"))  return dir + "/prog_empty.bin";
        }
        return dir + normal;
    };
    // A Timed capture of a mutated program when `sel` is selected, else the
    // normal one. The mutated capture lives in `storage`.
    const std::vector<uint32_t> pk = {kResults + 4, kResults + 8, kResults + 10};
    auto pick_timed = [&](const char* sel, const char* mbin, const Timed& normal, Timed& storage) -> const Timed& {
        if (has(sel)) {
            storage = make_timed(run_capture(dir + mbin, kSamplerRunBudget, pk));
            return storage;
        }
        return normal;
    };

    // ==== Fixture: a program-stored constant read back correctly, before
    // any proposition's own peek (DMACONR, INTENAR, record counts) is
    // trusted [not graded against the page -- apparatus check] ==========
    if (want("FIXTURE")) {
        bool mut = (has("FIXTURE"));
        Capture c = run_capture(dir + (mut ? "/prog_fixture_mut.bin" : "/prog_fixture.bin"),
                                 kBootBudget, {kResults});
        bool ok = c.loaded && c.done.has_value() && !c.peeks.empty() && c.peeks[0] &&
                  *c.peeks[0] == 0xBEEF;
        checks.expect(ok, "FIXTURE: a program-stored constant is read back correctly through "
                           "peek_logical [not graded against the page -- apparatus check, no "
                           "claim uid]");
    }

    // ==== P1: DMACON bit 7 (COPEN) enables Copper DMA [p.30] ==============
    // ch2-stopping-the-copper.c10, COP2-stopping-the-copper.c13: carried by
    // the aggregate P1-ON-OFF alone. P1 (COPEN set) and P1-OFF (COPEN clear)
    // are its two components and carry no claim id.
    if (want("P1")) {
        Timed on  = make_timed(run_capture(bin("/prog_p1.bin", {"P1-FIX-DONE", "P1-FIX-STRUCT"}),
                                            kSamplerRunBudget, pk));
        Timed off = make_timed(run_capture(bin("/prog_p1_off.bin", {"P1-OFF-FIX-DONE", "P1-OFF-FIX-STRUCT"}),
                                            kSamplerRunBudget, pk));
        checks.expect(on.done,  "P1-FIX-DONE: prog_p1.bin reaches DONE");
        checks.expect(off.done, "P1-OFF-FIX-DONE: prog_p1_off.bin reaches DONE");
        bool on_struct = on.done && on.t_m1.has_value() && on.t_m2.has_value() && on.two_frames;
        bool off_struct = off.done && off.t_m1.has_value() && off.t_m2.has_value() && off.two_frames;
        if (on.done) checks.expect(on_struct,
            "P1-FIX-STRUCT: prog_p1.bin wrote markers M1 and M2, and two complete frames after M2 were captured");
        else checks.skip("P1-FIX-STRUCT", "P1-FIX-DONE failed");
        if (off.done) checks.expect(off_struct,
            "P1-OFF-FIX-STRUCT: prog_p1_off.bin wrote markers M1 and M2, and two complete frames after M2 "
            "were captured");
        else checks.skip("P1-OFF-FIX-STRUCT", "P1-OFF-FIX-DONE failed");

        // ---- Preconditions of the COPEN-set case ----------------------
        // (a) No Copper fetch in W0 = [start, t(M1)).
        Timed sa, sb, sc;
        bool pre_a = false, pre_b = false, pre_c = false;
        if (on_struct) {
            const Timed& ta = pick_timed("P1-A", "/prog_p1_mut_a.bin", on, sa);
            pre_a = ta.t_m1.has_value() && !any_channel_read_in(ta.cap.log, kList1, kList1 + 8, 0, *ta.t_m1);
            checks.expect(pre_a, "P1a precondition: no Copper fetch in L1's region before t(M1)");
            // (b) stored DMACONR bit 9 = 1, bit 7 = 1
            const Timed& tb = pick_timed("P1-B", "/prog_p1_mut_b.bin", on, sb);
            pre_b = !tb.cap.peeks.empty() && tb.cap.peeks[0] && ((*tb.cap.peeks[0]) & 0x0280) == 0x0280;
            checks.expect(pre_b, "P1b precondition: stored DMACONR bit 9 = 1 and bit 7 = 1 [p.222]");
            // (c) anchors B1-B3 over the two complete frames after M2
            const Timed& tc = pick_timed("P1-C", "/prog_p1_mut_c.bin", on, sc);
            pre_c = two_frame_anchors(tc);
            checks.expect(pre_c,
                "P1c precondition: anchor checks B1-B3 hold (lines $10-$FF) over the two "
                "complete frames after M2");
        } else {
            checks.skip("P1a", "P1-FIX-STRUCT failed");
            checks.skip("P1b", "P1-FIX-STRUCT failed");
            checks.skip("P1c", "P1-FIX-STRUCT failed");
        }

        // ---- Preconditions of the COPEN-clear case --------------------
        Timed oa, ob, oc;
        bool off_pre_a = false, off_pre_b = false, off_pre_c = false;
        if (off_struct) {
            const Timed& ta = pick_timed("P1-OFFA", "/prog_p1_off_mut_a.bin", off, oa);
            off_pre_a = ta.t_m1.has_value() && !any_channel_read_in(ta.cap.log, kList1, kList1 + 8, 0, *ta.t_m1);
            checks.expect(off_pre_a, "P1-OFFa precondition: no Copper fetch in L1's region before t(M1) "
                                      "in the COPEN-clear case");
            // (b) DMAEN is set: the stored DMACONR bit 9 reads 1, so the absence of fetches is the
            // COPEN bit's, not the master enable's
            const Timed& tb = pick_timed("P1-OFFB", "/prog_p1_off_mut_b.bin", off, ob);
            off_pre_b = !tb.cap.peeks.empty() && tb.cap.peeks[0] && ((*tb.cap.peeks[0]) & 0x0200) != 0;
            checks.expect(off_pre_b, "P1-OFFb precondition: stored DMACONR bit 9 = 1 (DMAEN set) in the "
                                      "COPEN-clear case [p.222]");
            const Timed& tc = pick_timed("P1-OFFC", "/prog_p1_off_mut_c.bin", off, oc);
            off_pre_c = two_frame_anchors(tc);
            checks.expect(off_pre_c,
                "P1-OFFc precondition: anchor checks B1-B3 hold (lines $10-$FF) over the two complete "
                "frames after M2 in the COPEN-clear case");
        } else {
            checks.skip("P1-OFFa", "P1-OFF-FIX-STRUCT failed");
            checks.skip("P1-OFFb", "P1-OFF-FIX-STRUCT failed");
            checks.skip("P1-OFFc", "P1-OFF-FIX-STRUCT failed");
        }

        bool on_guards = on_struct && pre_a && pre_b && pre_c;
        bool off_guards = off_struct && off_pre_a && off_pre_b && off_pre_c;

        // ---- COPEN set: at least one fetch of L1's IR1 in each of the two
        // complete frames after M2. Own MUTATION: the capture of
        // prog_p1_off.bin (bit 7 clear) graded in its place.
        Timed swap_on;
        const Timed& main_t = pick_timed("P1-SWAP", "/prog_p1_off.bin", on, swap_on);
        bool on_cond = false;
        if (main_t.done && main_t.two_frames) {
            const uint32_t ir1_word = kList1 / 2;
            on_cond = frame_has_read_at(main_t.recs, main_t.frames, main_t.f0, main_t.cap.log, ir1_word) &&
                      frame_has_read_at(main_t.recs, main_t.frames, main_t.f0 + 1, main_t.cap.log, ir1_word);
        }
        if (on_guards) checks.expect(on_cond,
            "P1 [p.30]: with DMACON=$8280 (COPEN set, DMAEN set), the Copper fetches L1's IR1 in each "
            "of the two complete frames after M2 (component of P1-ON-OFF; carries no claim id)");
        else checks.skip("P1", "a P1 fixture or precondition failed");

        // ---- COPEN clear: no Copper fetch in L1's region from t(M2) on
        // (which covers the two complete frames after M2), and DMACONR
        // bit 7 reads 0. Own MUTATION: the capture of prog_p1.bin (bit 7
        // set) graded in its place.
        Timed swap_off;
        const Timed& off_main = pick_timed("P1OFF-SWAP", "/prog_p1.bin", off, swap_off);
        bool off_cond = false;
        if (off_main.done && off_main.t_m2.has_value()) {
            bool no_fetch = !any_channel_read_in(off_main.cap.log, kList1, kList1 + 8, *off_main.t_m2, UINT64_MAX);
            bool bit7_clear = !off_main.cap.peeks.empty() && off_main.cap.peeks[0] &&
                              (*off_main.cap.peeks[0] & 0x0080) == 0;
            off_cond = no_fetch && bit7_clear;
        }
        if (off_guards) checks.expect(off_cond,
            "P1-OFF [p.30,p.222]: control with DMACON=$8200 (COPEN clear, DMAEN set), the Copper makes no "
            "fetch in L1's region from t(M2) on, and DMACONR bit 7 reads 0 (component of P1-ON-OFF; "
            "carries no claim id)");
        else checks.skip("P1-OFF", "a P1-OFF fixture or precondition failed");

        // ---- The pair, both cases fully guarded.
        if (on_guards && off_guards) checks.expect(on_cond && off_cond,
            "P1-ON-OFF ch2-stopping-the-copper.c10,COP2-stopping-the-copper.c13 [p.30,p.222]: with DMAEN "
            "set, DMACON=$8280 (COPEN set) makes the Copper fetch L1's IR1 in each of the two complete "
            "frames after M2, and DMACON=$8200 (COPEN clear) makes no Copper fetch in L1's region from "
            "t(M2) on, with DMACONR bit 9 = 1 in the second case");
        else checks.skip("P1-ON-OFF", "a P1 or P1-OFF fixture or precondition failed");
    }

    // ==== P2: Clearing COPEN stops instruction fetching and data placement
    // [p.30] ==== ch2-stopping-the-copper.c7.s2, COP2-stopping-the-copper.c10
    // P2 is the COPEN-clear case (DMACON=$0080). P2-OFF (DMACON=$0000, a
    // no-op) is a control and carries no claim id.
    if (want("P2")) {
        constexpr uint32_t list_ir2 = kList1 / 2 + 1; // IR2 of L2's MOVE instruction

        Timed base = make_timed(run_capture(bin("/prog_p2.bin", {"P2-FIX-DONE", "P2-FIX-STRUCT"}),
                                             kSamplerRunBudget, pk));
        Timed off  = make_timed(run_capture(bin("/prog_p2_off.bin", {"P2-OFF-FIX-DONE", "P2-OFF-FIX-STRUCT"}),
                                             kSamplerRunBudget, pk));
        checks.expect(base.done, "P2-FIX-DONE: prog_p2.bin reaches DONE");
        checks.expect(off.done, "P2-OFF-FIX-DONE: prog_p2_off.bin reaches DONE");

        // The main assertion's capture: prog_p2_off.bin's, in its place, under P2-SWAP.
        Timed swap_main;
        const Timed& main_t = pick_timed("P2-SWAP", "/prog_p2_off.bin", base, swap_main);
        bool base_struct = base.done && base.t_m2.has_value() && base.two_frames;
        bool off_struct = off.done && off.t_m2.has_value() && off.two_frames;
        if (base.done) checks.expect(base_struct,
            "P2-FIX-STRUCT: prog_p2.bin wrote marker M2, and two complete frames after M2 were captured");
        else checks.skip("P2-FIX-STRUCT", "P2-FIX-DONE failed");
        if (off.done) checks.expect(off_struct,
            "P2-OFF-FIX-STRUCT: prog_p2_off.bin wrote marker M2, and two complete frames after M2 were "
            "captured");
        else checks.skip("P2-OFF-FIX-STRUCT", "P2-OFF-FIX-DONE failed");

        Timed sa, sb, sc;
        bool pre_a = false, pre_b = false, pre_c = false;
        if (base_struct) {
            // (a) a marker MPC sits right after the phase-1 clear; L2's IR2
            // must be fetched after it, with at least one record stored
            // between the clear and that fetch, bit 4 0 in every record
            // before it and 1 in a record after.
            const Timed& ta = pick_timed("P2-A", "/prog_p2_mut_a.bin", base, sa);
            auto t_mpc = first_write_cycle(ta.cap.log, kMarkBuf + 6);
            std::optional<uint64_t> t_fetch;
            if (t_mpc)
                for (const auto& a : ta.cap.log)
                    if (is_channel_read(a, kList1, kList1 + 8) && a.word_addr == list_ir2 &&
                        a.cycle > *t_mpc) { t_fetch = a.cycle; break; }
            pre_a = t_mpc.has_value() && t_fetch.has_value();
            if (pre_a) {
                bool have_between = false, zero_before = true, one_after = false;
                for (const auto& r : ta.recs) {
                    if (r.t0 > *t_mpc && r.t1 < *t_fetch) have_between = true;
                    if (r.t0 > *t_mpc && r.t1 < *t_fetch && (r.intreqr & 0x10)) zero_before = false;
                    if (r.t0 > *t_fetch && (r.intreqr & 0x10)) one_after = true;
                }
                pre_a = have_between && zero_before && one_after;
            }
            checks.expect(pre_a,
                "P2a precondition: after the marker at the phase-1 clear, at least one record "
                "is stored before the first IR2 fetch of L2's MOVE, INTREQR bit 4 reads 0 in "
                "every record before that fetch and 1 in a record after it");

            // (b) INTENAR bits 14-0 read 0, read/stored before phase 2.
            const Timed& tb = pick_timed("P2-B", "/prog_p2_mut_b.bin", base, sb);
            pre_b = !tb.cap.peeks.empty() && tb.cap.peeks[0] && ((*tb.cap.peeks[0]) & 0x7FFF) == 0;
            checks.expect(pre_b, "P2b precondition: stored INTENAR bits 14-0 read 0 before phase 2");

            // (c) anchors B1-B3 over the two complete frames after M2.
            const Timed& tc = pick_timed("P2-C", "/prog_p2_mut_c.bin", base, sc);
            pre_c = two_frame_anchors(tc);
            checks.expect(pre_c,
                "P2c precondition: anchor checks B1-B3 hold (lines $10-$FF) over the two "
                "complete frames after M2");
        } else {
            checks.skip("P2a", "P2-FIX-STRUCT failed");
            checks.skip("P2b", "P2-FIX-STRUCT failed");
            checks.skip("P2c", "P2-FIX-STRUCT failed");
        }

        // ---- Main assertion: zero fetches in L2's region from t(M2) to the
        // end of the second complete frame after M2, and INTREQR bit 4 = 0
        // in every record after M2.
        bool main_cond = false;
        if (main_t.done && main_t.t_m2.has_value() && main_t.two_frames) {
            auto w1 = frame_time_range(main_t.recs, main_t.frames, (size_t)main_t.f0 + 1);
            bool no_fetch = !any_channel_read_in(main_t.cap.log, kList1, kList1 + 8, *main_t.t_m2, w1.second);
            bool have_post_m2 = false, bit4_always_zero = true;
            for (const auto& r : main_t.recs) {
                if (r.t0 > *main_t.t_m2) {
                    have_post_m2 = true;
                    if (r.intreqr & 0x10) bit4_always_zero = false;
                }
            }
            main_cond = no_fetch && have_post_m2 && bit4_always_zero;
        }
        if (base_struct && pre_a && pre_b && pre_c) checks.expect(main_cond,
            "P2 ch2-stopping-the-copper.c7.s2,COP2-stopping-the-copper.c10 [p.30]: after "
            "clearing DMACON bit 7, the Copper makes no fetch in L2's region from t(M2) "
            "through the end of the second complete frame after M2, and INTREQR bit 4 "
            "reads 0 in every record stored after M2");
        else checks.skip("P2", "a P2 fixture or precondition failed");

        // ---- Control (main-off): with DMACON = $0000 (no bit selected --
        // a no-op, p.218), fetches continue in each of the two frames after
        // M2, and bit 4 reads 1 again after the next restart. Own
        // MUTATION: the capture of prog_p2.bin graded in its place. No
        // claim id: this line does not clear COPEN.
        Timed swap_off;
        const Timed& off_main = pick_timed("P2OFF-SWAP", "/prog_p2.bin", off, swap_off);
        bool off_cond = false;
        if (off_main.done && off_main.t_m2.has_value() && off_main.two_frames) {
            const uint32_t ir1_word = kList1 / 2;
            bool f0_fetch = frame_has_read_at(off_main.recs, off_main.frames, off_main.f0, off_main.cap.log, ir1_word);
            bool f1_fetch = frame_has_read_at(off_main.recs, off_main.frames, off_main.f0 + 1, off_main.cap.log, ir1_word);
            bool bit4_returns = false;
            for (const auto& r : off_main.recs)
                if (r.t0 > *off_main.t_m2 && (r.intreqr & 0x10)) bit4_returns = true;
            off_cond = f0_fetch && f1_fetch && bit4_returns;
        }
        if (off_struct) checks.expect(off_cond,
            "P2-OFF [p.30,p.218]: control with DMACON=$0000 (no bit selected, a no-op per p.218), the "
            "Copper keeps fetching L2's IR1 in each of the two frames after M2, and INTREQR bit 4 reads "
            "1 again in a later record (carries no claim id: no COPEN-clear write)");
        else checks.skip("P2-OFF", "a P2-OFF fixture failed");
    }

    // ==== P4: Loading a location register and writing its strobe before
    // enabling gives a known start address [p.30] ====
    // ch2-starting-the-copper-after-reset.c1.s2, .c2.s2
    if (want("P4")) {
        // Six captures: the three runs of the specification, Run 3 reordered,
        // and the two comparator contrasts (COP1LC=B and COP1LC=C, each
        // strobed with COPJMP1). Under a CTRL swap the named slot is
        // graded on a contrast's capture instead.
        struct P4Spec { const char* name; const char* sel; std::string bin; const char* mutstem; uint32_t list; };
        const P4Spec spec[6] = {
            {"Run1", "RUN1",  "/prog_p4_run1.bin", "/prog_p4_run1_mut_", kList1},
            {"Run2", "RUN2",  has("P4-RUN2-CTRL") ? "/prog_p4_ctrl_cop1b.bin" : "/prog_p4_run2.bin",
                 "/prog_p4_run2_mut_", kList2},
            {"Run3", "RUN3",  has("P4-RUN3-CTRL") ? "/prog_p4_ctrl_cop1b.bin" : "/prog_p4_run3.bin",
                 "/prog_p4_run3_mut_", kList2},
            {"Run3-reorder", "RUN3R", has("P4-RUN3R-CTRL") ? "/prog_p4_ctrl_cop1c.bin" : "/prog_p4_run3_reorder.bin",
                 "/prog_p4_run3_reorder_mut_", kList3},
            {"CTRL", "CTRL",  has("P4-CTRL-RUN1") ? "/prog_p4_run1.bin" : "/prog_p4_ctrl_cop1b.bin",
                 "/prog_p4_ctrl_cop1b_mut_", kList2},
            {"CTRL-C", "CTRLC", has("P4-CTRLC-RUN1") ? "/prog_p4_run1.bin" : "/prog_p4_ctrl_cop1c.bin",
                 "/prog_p4_ctrl_cop1c_mut_", kList3},
        };
        P4Result r[6];
        for (int i = 0; i < 6; ++i)
            r[i] = run_p4(bin(spec[i].bin.c_str(), {(std::string("P4-FIX-") + spec[i].name).c_str()}),
                          kSamplerRunBudget);

        // ---- Fixtures: every capture produced a decided first fetch.
        checks.expect(r[0].ok, "P4-FIX-Run1: Run 1 (prog_p4_run1.bin) produced a decided first fetch after M1");
        checks.expect(r[1].ok, "P4-FIX-Run2: Run 2 (prog_p4_run2.bin) produced a decided first fetch after M1");
        checks.expect(r[2].ok, "P4-FIX-Run3: Run 3 (prog_p4_run3.bin) produced a decided first fetch after M1");
        checks.expect(r[3].ok,
            "P4-FIX-Run3-reorder: Run 3 reordered (prog_p4_run3_reorder.bin) produced a decided first "
            "fetch after M1");
        checks.expect(r[4].ok,
            "P4-FIX-CTRL: the comparator contrast (prog_p4_ctrl_cop1b.bin) produced a decided first "
            "fetch after M1");
        checks.expect(r[5].ok,
            "P4-FIX-CTRL-C: the C comparator contrast (prog_p4_ctrl_cop1c.bin) produced a decided "
            "first fetch after M1");

        // ---- The three guards of every capture, each evaluated on its own
        // capture and printed before any line that grades it.
        P4Guards g[6];
        for (int i = 0; i < 6; ++i) {
            g[i] = p4_guards(r[i]);
            std::string s = spec[i].sel;
            if (has("P4-A-" + s)) g[i].a = p4_guards(run_p4(dir + spec[i].mutstem + "a.bin", kSamplerRunBudget)).a;
            if (has("P4-B-" + s)) g[i].b = p4_guards(run_p4(dir + spec[i].mutstem + "b.bin", kSamplerRunBudget)).b;
            if (has("P4-C-" + s)) g[i].c = p4_guards(run_p4(dir + spec[i].mutstem + "c.bin", kSamplerRunBudget)).c;
        }
        // (a) the first stored DMACONR has bit 7 = 0
        if (r[0].ok) checks.expect(g[0].a, "P4a-Run1 precondition: the first stored DMACONR has bit 7 = 0 in Run 1");
        else checks.skip("P4a-Run1", "P4-FIX-Run1 failed");
        if (r[1].ok) checks.expect(g[1].a, "P4a-Run2 precondition: the first stored DMACONR has bit 7 = 0 in Run 2");
        else checks.skip("P4a-Run2", "P4-FIX-Run2 failed");
        if (r[2].ok) checks.expect(g[2].a, "P4a-Run3 precondition: the first stored DMACONR has bit 7 = 0 in Run 3");
        else checks.skip("P4a-Run3", "P4-FIX-Run3 failed");
        if (r[3].ok) checks.expect(g[3].a,
            "P4a-Run3-reorder precondition: the first stored DMACONR has bit 7 = 0 in Run 3-reorder");
        else checks.skip("P4a-Run3-reorder", "P4-FIX-Run3-reorder failed");
        if (r[4].ok) checks.expect(g[4].a,
            "P4a-CTRL precondition: the first stored DMACONR has bit 7 = 0 in the B comparator contrast");
        else checks.skip("P4a-CTRL", "P4-FIX-CTRL failed");
        if (r[5].ok) checks.expect(g[5].a,
            "P4a-CTRL-C precondition: the first stored DMACONR has bit 7 = 0 in the C comparator contrast");
        else checks.skip("P4a-CTRL-C", "P4-FIX-CTRL-C failed");
        // (b) no Copper fetch in A, B or C before t(M1)
        if (r[0].ok) checks.expect(g[0].b, "P4b-Run1 precondition: no Copper fetch in A, B or C's region before t(M1) in Run 1");
        else checks.skip("P4b-Run1", "P4-FIX-Run1 failed");
        if (r[1].ok) checks.expect(g[1].b, "P4b-Run2 precondition: no Copper fetch in A, B or C's region before t(M1) in Run 2");
        else checks.skip("P4b-Run2", "P4-FIX-Run2 failed");
        if (r[2].ok) checks.expect(g[2].b, "P4b-Run3 precondition: no Copper fetch in A, B or C's region before t(M1) in Run 3");
        else checks.skip("P4b-Run3", "P4-FIX-Run3 failed");
        if (r[3].ok) checks.expect(g[3].b,
            "P4b-Run3-reorder precondition: no Copper fetch in A, B or C's region before t(M1) in Run 3-reorder");
        else checks.skip("P4b-Run3-reorder", "P4-FIX-Run3-reorder failed");
        if (r[4].ok) checks.expect(g[4].b,
            "P4b-CTRL precondition: no Copper fetch in A, B or C's region before t(M1) in the B comparator contrast");
        else checks.skip("P4b-CTRL", "P4-FIX-CTRL failed");
        if (r[5].ok) checks.expect(g[5].b,
            "P4b-CTRL-C precondition: no Copper fetch in A, B or C's region before t(M1) in the C comparator contrast");
        else checks.skip("P4b-CTRL-C", "P4-FIX-CTRL-C failed");
        // (c) the stretch from t(MS) to the first fetch lies outside vertical blanking: the last
        // record stored before t(MS) and the record following the first record stored after the
        // first fetch lie in the same frame and both read V > 25 (p.219). The program samples
        // nothing in that stretch, so the bracket is built from the records around it (2.4).
        if (r[0].ok) checks.expect(g[0].c,
            "P4c-Run1 precondition: the last record stored before t(MS) and the record following the "
            "first record stored after the first fetch lie in the same frame and both read V > 25, in Run 1");
        else checks.skip("P4c-Run1", "P4-FIX-Run1 failed");
        if (r[1].ok) checks.expect(g[1].c,
            "P4c-Run2 precondition: the last record stored before t(MS) and the record following the "
            "first record stored after the first fetch lie in the same frame and both read V > 25, in Run 2");
        else checks.skip("P4c-Run2", "P4-FIX-Run2 failed");
        if (r[2].ok) checks.expect(g[2].c,
            "P4c-Run3 precondition: the last record stored before t(MS) and the record following the "
            "first record stored after the first fetch lie in the same frame and both read V > 25, in Run 3");
        else checks.skip("P4c-Run3", "P4-FIX-Run3 failed");
        if (r[3].ok) checks.expect(g[3].c,
            "P4c-Run3-reorder precondition: the last record stored before t(MS) and the record following the "
            "first record stored after the first fetch lie in the same frame and both read V > 25, in Run 3-reorder");
        else checks.skip("P4c-Run3-reorder", "P4-FIX-Run3-reorder failed");
        if (r[4].ok) checks.expect(g[4].c,
            "P4c-CTRL precondition: the last record stored before t(MS) and the record following the "
            "first record stored after the first fetch lie in the same frame and both read V > 25, in the B "
            "comparator contrast");
        else checks.skip("P4c-CTRL", "P4-FIX-CTRL failed");
        if (r[5].ok) checks.expect(g[5].c,
            "P4c-CTRL-C precondition: the last record stored before t(MS) and the record following the "
            "first record stored after the first fetch lie in the same frame and both read V > 25, in the C "
            "comparator contrast");
        else checks.skip("P4c-CTRL-C", "P4-FIX-CTRL-C failed");

        // ---- The first fetch's exact address in each capture, measured.
        for (int i = 0; i < 6; ++i) {
            checks.note(std::string("P4 ") + spec[i].name + ": first Copper fetch after t(M1) is at " +
                        (r[i].fetch ? hexs(r[i].fetch->word_addr * 2) + " (in region " + r[i].fetch->name + ")"
                                    : std::string("none")) +
                        "; the selected list's first word is at " + hexs(spec[i].list));
        }

        bool ok[6];
        for (int i = 0; i < 6; ++i) ok[i] = r[i].ok && g[i].all();

        // ---- Main assertion: one case per register, p.30's "one of ...
        // COP1LC or COP2LC". Run 1 strobes COP1LC/COPJMP1 and predicts the
        // first word of A; Run 2 strobes COP2LC/COPJMP2 and predicts the first
        // word of B.
        if (ok[0] && ok[1]) checks.expect(r[0].first_word_is(kList1) && r[1].first_word_is(kList2),
            "P4 ch2-starting-the-copper-after-reset.c1.s2,ch2-starting-the-copper-after-reset.c2.s2 "
            "[p.25,p.26,p.30,p.276]: the first Copper fetch after Copper DMA is turned on is at the "
            "address of the location register whose strobe was written -- Run 1 (COP1LC/COPJMP1, "
            "expect A's IR1) and Run 2 (COP2LC/COPJMP2, expect B's IR1)");
        else checks.skip("P4", "a Run 1 or Run 2 fixture or guard failed");

        // ---- The same rule, one assertion per run. None carries a claim id
        // of its own: P4-Run2 states P4's own clause (the strobe written
        // while Copper DMA is off sets the start address) for Run 2 alone;
        // P4-Run3 and P4-Run3-reorder grade a narrower clause -- the
        // location register's own value at the moment the strobe is
        // written, p.25 "is loaded into the Copper program counter" -- since
        // each rewrites the strobed register afterwards, before or after
        // the strobe. Green control for P4-Run2 and P4-Run3 (both expect
        // B): P4-CTRL below. Green control for P4-Run3-reorder (expects C):
        // P4-CTRL-C below.
        if (ok[1]) checks.expect(r[1].first_word_is(kList2),
            "P4-Run2 [p.25,p.26,p.30,p.276]: the strobe written while Copper DMA is off sets the "
            "start address -- COP1LC=A, COP2LC=B, COPJMP2: expect B's IR1");
        else checks.skip("P4-Run2", "a Run 2 fixture or guard failed");

        if (ok[2]) checks.expect(r[2].first_word_is(kList2),
            "P4-Run3 [p.25]: the location register's value at the strobe (\"is loaded into the "
            "Copper program counter\") decides the start address -- COP1LC=A, COP2LC=B, COPJMP2, "
            "then COP2LC=C with no further strobe: COP2LC held B when COPJMP2 was written, expect "
            "B's IR1, not C's");
        else checks.skip("P4-Run3", "a Run 3 fixture or guard failed");

        // ---- P4-Run3-reorder alone cannot isolate "the value at the
        // strobe": COP2LC holds C both at the strobe and at DMA-on, so its
        // own comparator repeats P4-Run2's clause with C rather than
        // contrasting an earlier value against a later one. Paired with
        // P4-Run3 (which does hold an earlier value, B, at the strobe) as
        // the specification's own pair (P4 items 4 and 6: "Run 2, Run 3 and
        // Run 3-reorder are graded together against this rule"), the two
        // runs together show the rule discriminating the two orderings.
        if (ok[3]) checks.expect(r[3].first_word_is(kList3),
            "P4-Run3-reorder [p.25]: COP2LC holds C both at the strobe and at DMA-on, so this line "
            "alone repeats P4-Run2's clause with C (see P4-Run3-PAIR for the ordering contrast with "
            "P4-Run3) -- COP1LC=A, COP2LC=B, then COP2LC=C moved before COPJMP2: expect C's IR1");
        else checks.skip("P4-Run3-reorder", "a Run 3-reorder fixture or guard failed");

        // ---- P4-Run3-PAIR: Run 3 and Run 3-reorder graded together, as
        // the specification's own pair. Its own green demonstration
        // substitutes P4-CTRL's and P4-CTRL-C's captures (B and C through
        // COP1LC/COPJMP1, not through the COP2LC/COPJMP2 path Run 3 and
        // Run 3-reorder use) for Run 3's and Run 3-reorder's own, showing
        // the same paired comparator reads true for a live B/C pair.
        bool mut_pair = (has("P4-RUN3-PAIR-A"));
        bool pair_guards = mut_pair ? (ok[4] && ok[5]) : (ok[2] && ok[3]);
        bool pair_ok = mut_pair
            ? (r[4].first_word_is(kList2) && r[5].first_word_is(kList3))
            : (r[2].first_word_is(kList2) && r[3].first_word_is(kList3));
        if (pair_guards) checks.expect(pair_ok,
            "P4-Run3-PAIR [p.25]: Run 3 (COP2LC rewritten to C after the strobe: expect B's IR1) and Run "
            "3-reorder (COP2LC=C moved before the strobe: expect C's IR1), graded together as the "
            "specification's own pair");
        else checks.skip("P4-Run3-PAIR", "a Run 3 or Run 3-reorder fixture or guard failed");

        // ---- Comparator contrasts, always run: COP1LC loaded with another
        // list, then COPJMP1 -- the COP1LC/COPJMP1 path, not the
        // COP2LC/COPJMP2 path Run 2, Run 3 and Run 3-reorder use. P4-CTRL is
        // P4's and P4-Run2's own falsifiability demonstration and
        // P4-Run3's green control (both expect B). P4-CTRL-C is
        // P4-Run3-reorder's and P4-Run3-PAIR's green control for C.
        if (ok[4]) checks.expect(r[4].first_word_is(kList2),
            "P4-CTRL [p.25]: COP1LC=B (not A), COPJMP1 -- the first fetch after DMA-on is B's IR1");
        else checks.skip("P4-CTRL", "a comparator-contrast fixture or guard failed");
        if (ok[5]) checks.expect(r[5].first_word_is(kList3),
            "P4-CTRL-C [p.25]: COP1LC=C (not A or B), COPJMP1 -- the first fetch after DMA-on is "
            "C's IR1");
        else checks.skip("P4-CTRL-C", "a comparator-contrast fixture or guard failed");
    }

    // ==== P3: DMACON controls every DMA channel [p.30, p.222, p.278] ====
    // ch2-stopping-the-copper.c8, COP2-stopping-the-copper.c11: carried by
    // the aggregate P3-ALL alone. P3-BASE, the nine channel cases and case
    // M are its components and carry no claim id.
    if (want("P3")) {
        // ---- Region table (this package's own $10000-$57FFF map,
        // SPECIFICATION.md P3 stimulus step 2): kP3PtrOffset is how far
        // above each region's own start the program writes that channel's
        // own pointer or location register (SPECIFICATION.md P3 stimulus),
        // which precondition (c) compares the lowest in-region access
        // against.
        struct P3Region { const char* name; uint32_t lo, hi; };
        enum { R_BPL = 0, R_SPR, R_AUD0, R_AUD1, R_AUD2, R_AUD3, R_DSK, R_BLT, R_COP, R_N };
        const P3Region kP3Regions[R_N] = {
            {"BPL",  0x010000, 0x018000}, {"SPR",  0x018000, 0x020000},
            {"AUD0", 0x020000, 0x028000}, {"AUD1", 0x028000, 0x030000},
            {"AUD2", 0x030000, 0x038000}, {"AUD3", 0x038000, 0x040000},
            {"DSK",  0x040000, 0x048000}, {"BLT",  0x048000, 0x050000},
            {"COP",  0x050000, 0x058000},
        };
        constexpr uint32_t kP3PtrOffset = 0x100;

        // ---- Apparatus check, part (i): the refresh port is never
        // asserted anywhere but word address 0 (SPECIFICATION.md 2.3: "a
        // refresh access is one made while the `refresh` output ... is
        // asserted", the recognition P3 uses, unlike P1/P2/P4's own
        // word-address-0 signature). Checked here against prog_p1.bin's
        // own captured log (captured for P1), requiring at least
        // one refresh-asserted read to exist -- otherwise the check would
        // pass vacuously on a log with no such read at all. The converse is
        // not asserted: a word-address-0 read can be something other than
        // refresh (measured: one such read, elsewhere in this same log,
        // where the refresh port reads clear) -- the port is the finer
        // signal, which is exactly why P3 uses it instead of the address
        // alone.
        CaptureR rc1 = run_capture_refresh(has("P3-APP-I-REFPTR") ? dir + "/prog_p1_refptr.bin"
                                                                   : dir + "/prog_p1.bin",
                                           kSamplerRunBudget, {});
        bool refresh_ok = rc1.loaded && rc1.done.has_value();
        bool any_read = false, any_refresh_read = false;
        if (refresh_ok) {
            for (const auto& a : rc1.log) {
                if (a.is_write) continue;
                any_read = true;
                if (rc1.refresh_at(a.cycle)) {
                    any_refresh_read = true;
                    if (a.word_addr != 0) { refresh_ok = false; break; }
                }
            }
        }
        checks.expect(refresh_ok && any_read && any_refresh_read,
            "P3-APP-I [SPECIFICATION.md 2.3]: every RAM read where the refresh port reads asserted is "
            "at word address 0, and at least one such read exists, checked on prog_p1.bin's own "
            "captured log [not graded against the page -- apparatus check, no claim uid]");

        // ---- Apparatus check, part (ii): the same recognition checked
        // directly on a P3 capture, inside P3's own regions, rather than
        // inferred from part (i)'s different addresses. At least one
        // refresh-asserted read exists, and none of them falls inside any
        // of the nine regions.
        CaptureR rc3 = run_capture_refresh(has("P3-REFRESH-REFPTR") ? dir + "/prog_p3_refptr.bin"
                                                                     : dir + "/prog_p3_run_base.bin",
                                           kSamplerRunBudget, {});
        bool part2_ok = rc3.loaded && rc3.done.has_value();
        bool part2_any_refresh_read = false, part2_none_in_region = true;
        if (part2_ok) {
            for (const auto& a : rc3.log) {
                if (!rc3.refresh_at(a.cycle)) continue;
                if (!a.is_write) part2_any_refresh_read = true;
                uint32_t b = a.word_addr * 2;
                for (int i = 0; i < R_N; ++i)
                    if (b >= kP3Regions[i].lo && b < kP3Regions[i].hi) part2_none_in_region = false;
            }
        }
        checks.expect(part2_ok && part2_any_refresh_read && part2_none_in_region,
            "P3-REFRESH [SPECIFICATION.md 2.3]: on prog_p3_run_base.bin's own captured log, at "
            "least one RAM access where the refresh port reads asserted exists, and none of them "
            "falls inside any of the nine regions [not graded against the page -- apparatus check, "
            "no claim uid]");

        struct P3CaseResult {
            Capture cap;
            bool ok = false;
            bool have_dskbytr = false, have_dmaconr = false;
            uint16_t dskbytr = 0, dmaconr = 0;
            bool region_access[R_N] = {};
            std::optional<uint32_t> region_lowest[R_N];
        };

        // Region-bounded access, refresh excluded by word address 0 (as
        // P1/P2/P4 already do): every one of this package's regions starts
        // at $10000 or above, so a word-address-0 access -- refresh's own
        // address, per the apparatus check just above, over a substantial
        // captured log -- can never fall inside one. The port and the
        // address method therefore agree on every region-bounded question
        // this proposition asks; run_capture (the fast path P1/P2/P4 use)
        // is used here rather than the slow, cycle-stepped refresh capture.
        auto run_p3_case = [&](const std::string& binname, uint64_t budget) -> P3CaseResult {
            P3CaseResult r;
            r.cap = run_capture(dir + binname, budget,
                                 {kResults + 4, kResults + 6, kResults + 8, kResults + 10});
            if (!r.cap.loaded || !r.cap.done.has_value()) return r;
            if (r.cap.peeks.size() >= 1 && r.cap.peeks[0]) { r.dskbytr = *r.cap.peeks[0]; r.have_dskbytr = true; }
            if (r.cap.peeks.size() >= 2 && r.cap.peeks[1]) { r.dmaconr = *r.cap.peeks[1]; r.have_dmaconr = true; }
            auto t_m2 = first_write_cycle(r.cap.log, kMarkBuf + 2);
            if (!t_m2) return r;
            auto recs = decode_records(r.cap.log, kRecBuf, rec_count(r.cap, 2));
            auto frames = record_frames(recs);
            int f0 = first_frame_after(recs, frames, *t_m2);
            if (f0 < 0) return r;
            // Observation window: t(M2) to the end of the first complete
            // frame after M2 (SPECIFICATION.md P3 Observation) -- not the
            // frame's own [lo,hi), whose low edge sits at the wrap before
            // M2's frame, well before t(M2) itself.
            auto frame_range = frame_time_range(recs, frames, (size_t)f0);
            uint64_t win_lo = *t_m2, win_hi = frame_range.second;
            for (const auto& a : r.cap.log) {
                if (a.cycle < win_lo || a.cycle >= win_hi) continue;
                if (a.word_addr == 0) continue; // refresh
                uint32_t b = a.word_addr * 2;
                for (int i = 0; i < R_N; ++i) {
                    if (b < kP3Regions[i].lo || b >= kP3Regions[i].hi) continue;
                    r.region_access[i] = true;
                    if (!r.region_lowest[i] || b < *r.region_lowest[i]) r.region_lowest[i] = b;
                }
            }
            r.ok = true;
            return r;
        };

        // ---- The eleven runs (SPECIFICATION.md P3 stimulus table). Each
        // is its own full run; off_region is the region cleared (-1: none,
        // Base; -2: every channel, Case M).
        struct P3Case { const char* label; const char* bin; uint16_t expected_p; int off_region; };
        const P3Case kP3Cases[] = {
            {"Base", "/prog_p3_run_base.bin", 0x3FF, -1},
            {"BPL",  "/prog_p3_off_bpl.bin",  0x2FF, R_BPL},
            {"SPR",  "/prog_p3_off_spr.bin",  0x3DF, R_SPR},
            {"AUD0", "/prog_p3_off_aud0.bin", 0x3FE, R_AUD0},
            {"AUD1", "/prog_p3_off_aud1.bin", 0x3FD, R_AUD1},
            {"AUD2", "/prog_p3_off_aud2.bin", 0x3FB, R_AUD2},
            {"AUD3", "/prog_p3_off_aud3.bin", 0x3F7, R_AUD3},
            {"DSK",  "/prog_p3_off_dsk.bin",  0x3EF, R_DSK},
            {"BLT",  "/prog_p3_off_blt.bin",  0x3BF, R_BLT},
            {"COP",  "/prog_p3_off_cop.bin",  0x37F, R_COP},
            {"M",    "/prog_p3_case_m.bin",   0x1FF, -2},
        };
        constexpr int kP3N = sizeof(kP3Cases) / sizeof(kP3Cases[0]);
        constexpr uint64_t kP3Budget = kSamplerRunBudget; // same margin P1/P2/P4 already need

        P3CaseResult case_results[kP3N];
        for (int i = 0; i < kP3N; ++i) {
            std::string b = (has(std::string("P3-FIX-") + kP3Cases[i].label + "-NODONE"))
                                ? std::string("/prog_nodone.bin") : std::string(kP3Cases[i].bin);
            case_results[i] = run_p3_case(b, kP3Budget);
        }

        bool p3_fix = true;
        for (int i = 0; i < kP3N; ++i) {
            checks.expect(case_results[i].ok,
                std::string("P3-FIX-") + kP3Cases[i].label + ": Case " + kP3Cases[i].label + " (" +
                    (kP3Cases[i].bin + 1) + ") reaches DONE and captures one complete frame after M2");
            if (!case_results[i].ok) p3_fix = false;
        }

        // ---- Region DSK in Base, measured and not graded (section 4,
        // item 12: the pages do not say whether disk DMA runs with no drive
        // attached). P3-DSK and P3-ALL are graded only where region DSK
        // shows an access in Base.
        bool dsk_established = false;
        bool pre_b = false, pre_c = false, pre_d = false;
        if (p3_fix) {
            bool mut_a = (has("P3-A"));
            P3CaseResult a_case = mut_a ? run_p3_case("/prog_p3_mut_a.bin", kP3Budget) : P3CaseResult{};
            const P3CaseResult& a_src = mut_a ? a_case : case_results[0];
            dsk_established = a_src.ok && a_src.region_access[R_DSK];
            checks.note(std::string("P3a: region DSK shows ") + (dsk_established ? "at least one access" : "no access") +
                        " in Base; the pages do not say whether disk DMA runs with no drive attached, so this is "
                        "not graded");

            // ---- Precondition (b): in every run, the stored DMACONR bits 9-0
            // equal that run's own pattern P (p.222).
            bool mut_b = (has("P3-B"));
            P3CaseResult b_mut_case = mut_b ? run_p3_case("/prog_p3_mut_b.bin", kP3Budget) : P3CaseResult{};
            pre_b = true;
            for (int i = 0; i < kP3N; ++i) {
                const P3CaseResult& cr = (mut_b && i == 0) ? b_mut_case : case_results[i];
                if (!cr.ok || !cr.have_dmaconr || (cr.dmaconr & 0x03FF) != kP3Cases[i].expected_p) pre_b = false;
            }
            checks.expect(pre_b,
                "P3b precondition: in every one of the eleven runs, the stored DMACONR bits 9-0 equal "
                "that run's own pattern P [p.222]");

            // ---- Precondition (c): in Base, the lowest address accessed in
            // each region is the address the program wrote into that region's
            // own channel -- each channel's own pointer, kP3PtrOffset above its
            // region's own start (SPECIFICATION.md P3 stimulus; for SPR, the
            // lowest of SPR0PT-SPR7PT, which is also SPR0PT at that same
            // offset). Compared against that fixed, program-controlled address,
            // not the region's own start: a region's own start would leave
            // nothing below the pointer for another channel's accesses to land
            // in, so no such intrusion could ever be caught.
            bool mut_c = (has("P3-C"));
            P3CaseResult c_mut_case = mut_c ? run_p3_case("/prog_p3_mut_c.bin", kP3Budget) : P3CaseResult{};
            const P3CaseResult& c_case = mut_c ? c_mut_case : case_results[0];
            pre_c = c_case.ok;
            for (int i = 0; i < R_N; ++i) {
                if (!c_case.region_lowest[i]) continue; // nothing accessed there: no violation to find
                if (*c_case.region_lowest[i] != kP3Regions[i].lo + kP3PtrOffset) pre_c = false;
            }
            checks.expect(pre_c,
                "P3c precondition: in Base, the lowest address accessed in each region is the address "
                "the program wrote into that region's own channel");

            // ---- Precondition (d): anchors B1 and B3 for the frame boundary,
            // over every one of the eleven runs' own graded frame (2.4's own
            // sparse-sampler contrast substitutes for the Base run only).
            bool mut_d = (has("P3-D"));
            P3CaseResult d_mut_case = mut_d ? run_p3_case("/prog_p3_mut_d.bin", kP3Budget) : P3CaseResult{};
            pre_d = true;
            for (int i = 0; i < kP3N; ++i) {
                const P3CaseResult& src = (mut_d && i == 0) ? d_mut_case : case_results[i];
                if (!src.ok) { pre_d = false; continue; }
                auto t_m2 = first_write_cycle(src.cap.log, kMarkBuf + 2);
                auto recs = decode_records(src.cap.log, kRecBuf, rec_count(src.cap, 2));
                auto frames = record_frames(recs);
                int f0 = t_m2 ? first_frame_after(recs, frames, *t_m2) : -1;
                if (f0 < 0) { pre_d = false; continue; }
                bool b1 = anchor_B1(recs, frames[f0], 0x10, 0xFF);
                auto b23 = anchor_B2_B3(recs, frames[f0], 0x10, 0xFF);
                if (!b1 || !b23.second) pre_d = false;
            }
            checks.expect(pre_d,
                "P3d precondition: anchor checks B1 and B3 hold (lines $10-$FF) for the frame boundary, "
                "over every one of the eleven runs' own graded frame");
        } else {
            checks.skip("P3b", "a P3-FIX line failed");
            checks.skip("P3c", "a P3-FIX line failed");
            checks.skip("P3d", "a P3-FIX line failed");
        }
        const bool p3_pre = p3_fix && pre_b && pre_c && pre_d;

        // ---- Base coverage: "Base: every region other than DSK has at
        // least one access" (P3 item 4) -- the "enables" half of
        // "controls". Without this, a channel that never ran in any run
        // would leave every case line and P3-M green without ever being
        // exercised. Own mutation (NMH_MUTATE=P3-BASE-SWAP): Case M's
        // captured run (DMAEN clear, no region has any access)
        // graded in Base's place.
        bool mut_base_swap = (has("P3-BASE-SWAP"));
        const P3CaseResult& base_for_coverage = mut_base_swap ? case_results[10] : case_results[0];
        bool base_cov_ok = base_for_coverage.ok;
        for (int r = 0; r < R_N; ++r) {
            if (r == R_DSK) continue;
            if (!base_for_coverage.region_access[r]) base_cov_ok = false;
        }
        if (p3_pre) checks.expect(base_cov_ok,
            "P3-BASE [p.30,p.222,p.278]: in Base, every region other than DSK has at least one access "
            "from t(M2) to the end of the first complete frame after M2 (component of P3-ALL; carries "
            "no claim id)");
        else checks.skip("P3-BASE", "a P3 fixture or precondition failed");

        // ---- The nine channel cases and case M. Each case's own mutation
        // (NMH_MUTATE=P3-<label>-SWAP) grades Base's own capture in that
        // case's own slot. A case line is a component of
        // P3-ALL and carries no claim id. When region DSK shows no access in
        // Base (measured above, not graded), the DSK case has nothing to
        // contrast with and is not graded; every case's comparator leaves
        // out a region that was silent in Base.
        bool case_ok[kP3N] = {};
        for (int i = 1; i < kP3N; ++i) {
            const P3Case& kc = kP3Cases[i];
            std::string swap_key = std::string("P3-") + kc.label + "-SWAP";
            const P3CaseResult& cr = (has(swap_key)) ? case_results[0] : case_results[i];
            std::string id = std::string("P3-") + kc.label;
            if (kc.off_region == -2) {
                bool all_off = cr.ok;
                for (int r = 0; r < R_N; ++r) if (cr.region_access[r]) all_off = false;
                case_ok[i] = all_off;
                if (p3_pre) checks.expect(all_off,
                    id + " [p.30,p.222,p.278]: with DMAEN (bit 9) clear, no region has an access from "
                    "t(M2) to the end of the first complete frame after M2 (component of P3-ALL; carries "
                    "no claim id)");
                else checks.skip(id, "a P3 fixture or precondition failed");
                continue;
            }
            int idx = kc.off_region;
            bool ok = cr.ok && !cr.region_access[idx] &&
                      case_results[0].ok && case_results[0].region_access[idx];
            for (int r = 0; r < R_N; ++r) {
                if (r == idx) continue;
                if (!case_results[0].ok || !case_results[0].region_access[r]) continue; // Base was silent there too
                if (!cr.region_access[r]) ok = false;
            }
            case_ok[i] = ok;
            char pat_buf[8];
            std::snprintf(pat_buf, sizeof pat_buf, "$%04X", kc.expected_p | 0x8000);
            std::string pat = pat_buf;
            std::string label = id + " [p.30,p.222,p.278]: with DMACON=" + pat +
                " (region " + kc.label + "'s bit clear), region " + kc.label + " has no access "
                "from t(M2) to the end of the first complete frame after M2, region " + kc.label +
                " had one in Base, and every other region that had one in Base still does (component "
                "of P3-ALL; carries no claim id)";
            if (!p3_pre) {
                checks.skip(id, "a P3 fixture or precondition failed");
            } else if (idx == R_DSK && !dsk_established) {
                checks.skip(id, "region DSK showed no access in Base, which the pages leave open, so the "
                                "DSK case has nothing to contrast with");
            } else {
                checks.expect(ok, label);
            }
        }

        // ---- The aggregate: both claims are graded whole by Base's
        // coverage, the nine channel cases and case M together.
        if (p3_pre && dsk_established) {
            bool all = base_cov_ok;
            for (int i = 1; i < kP3N; ++i) all = all && case_ok[i];
            checks.expect(all,
                "P3-ALL ch2-stopping-the-copper.c8,COP2-stopping-the-copper.c11 [p.30,p.222,p.278]: "
                "every region other than DSK has an access in Base, each of the nine channel cases "
                "silences its own region while every other region live in Base stays live, and with "
                "DMAEN clear no region has an access");
        } else if (p3_pre) {
            checks.record("P3-ALL [ids withdrawn: ch2-stopping-the-copper.c8,COP2-stopping-the-copper.c11 "
                          "[p.30,p.222,p.278]]: region DSK's own case is not graded this run (region DSK shows "
                          "no access in Base, which the pages leave open), so the claims are not graded whole "
                          "by the ten cases");
        } else {
            checks.skip("P3-ALL", "a P3 fixture or precondition failed");
        }

        // ---- DSKBYTR bit 14 (DMAON, p.248): 1 in Base, 0 in the DSK case
        // and in case M. Own mutation (NMH_MUTATE=P3-DSKBYTR-SWAP): Base's
        // own DSKBYTR value graded in the DSK case's and
        // case M's own slots.
        {
            bool mut_dskbytr = (has("P3-DSKBYTR-SWAP"));
            const P3CaseResult& dsk_src = mut_dskbytr ? case_results[0] : case_results[7]; // "DSK" is index 7
            const P3CaseResult& m_src = mut_dskbytr ? case_results[0] : case_results[10];  // "M" is index 10
            bool ok = case_results[0].ok && case_results[0].have_dskbytr &&
                      (case_results[0].dskbytr & 0x4000) != 0 &&
                      dsk_src.ok && dsk_src.have_dskbytr && (dsk_src.dskbytr & 0x4000) == 0 &&
                      m_src.ok && m_src.have_dskbytr && (m_src.dskbytr & 0x4000) == 0;
            if (p3_fix && pre_b) checks.expect(ok,
                "P3-DSKBYTR [p.248]: DSKBYTR bit 14 (DMAON) reads 1 in Base and 0 in the DSK case "
                "and in case M");
            else checks.skip("P3-DSKBYTR", "a P3 fixture or precondition P3b failed");
        }
    }

    // ==== P10: INTREQ bit 15 decides whether bits written as 1 are set
    // or cleared [p.35] ==== ch2-the-copper-and-the-680x0.c5,
    // COP2-the-copper-and-the-680x0.c5,.c6,.c7
    if (want("P10")) {
        auto sample_idx_on_line = [](const std::vector<Rec>& recs, int line) -> std::optional<size_t> {
            for (size_t i = 0; i < recs.size(); ++i) if (recs[i].V == line) return i;
            return std::nullopt;
        };
        auto p10_load = [&](const std::string& binpath) {
            return run_capture(binpath, kSamplerRunBudget, {kResults + 4, kResults + 8, kResults + 10});
        };
        auto p10_decode = [&](const Capture& c) {
            return decode_records(c.log, kRecBuf, rec_count(c, 1));
        };
        const int p10_lines[5] = {0x38, 0x48, 0x58, 0x68, 0x78};

        Capture base = p10_load(bin("/prog_p10_main.bin", {"P10-FIX-DONE", "P10-FIX-LINES"}));
        bool base_ok = base.loaded && base.done.has_value();
        checks.expect(base_ok, "P10-FIX-DONE: prog_p10_main.bin reaches DONE");
        std::vector<Rec> recs = base_ok ? p10_decode(base) : std::vector<Rec>{};

        bool have_all = base_ok;
        for (int k = 0; k < 5; ++k)
            if (!sample_idx_on_line(recs, p10_lines[k])) have_all = false;
        if (base_ok) checks.expect(have_all,
            "P10-FIX-LINES: all five sample lines ($38,$48,$58,$68,$78) have a record");
        else checks.skip("P10-FIX-LINES", "P10-FIX-DONE failed");

        Capture main2 = p10_load(bin("/prog_p10_main2.bin", {"P10-MAIN2-FIX-DONE"}));
        bool main2_ok = main2.loaded && main2.done.has_value();
        checks.expect(main2_ok, "P10-MAIN2-FIX-DONE: prog_p10_main2.bin reaches DONE");

        // ---- Preconditions, printed before the lines that depend on them.
        bool pre_a = false, pre_b = false, pre_c = false;
        if (have_all) {
            // (a) order -- MOVE k's IR2 fetch lies after the store of sample
            // k-1 and before the store of the record just before sample k.
            bool mut_a = (has("P10-A"));
            Capture cap_a;
            const Capture& a_cap = mut_a ? (cap_a = p10_load(dir + "/prog_p10_mut_a.bin")) : base;
            bool have_a = a_cap.loaded && a_cap.done.has_value();
            std::vector<Rec> recs_a = have_a ? p10_decode(a_cap) : std::vector<Rec>{};
            std::optional<size_t> idx_a[5];
            if (have_a) for (int k = 0; k < 5; ++k) { idx_a[k] = sample_idx_on_line(recs_a, p10_lines[k]); if (!idx_a[k]) have_a = false; }
            const uint32_t move_ir2_word[4] = {
                kList1 / 2 + 3, kList1 / 2 + 7, kList1 / 2 + 11, kList1 / 2 + 15,
            };
            pre_a = have_a;
            if (have_a) {
                for (int k = 0; k < 4; ++k) {
                    auto t_move = first_channel_read_at(a_cap.log, move_ir2_word[k], 0);
                    if (!t_move) { pre_a = false; continue; }
                    uint64_t lo = recs_a[*idx_a[k]].t1;
                    uint64_t hi = recs_a[*idx_a[k + 1] - 1].t1;
                    if (!(*t_move > lo && *t_move < hi)) pre_a = false;
                }
            }
            checks.expect(pre_a,
                "P10a precondition: each MOVE's IR2 fetch lies after the store of the previous "
                "sample and before the store of the record just before the next sample");

            // (b) the $38 sample shows (0,0).
            bool mut_b = (has("P10-B"));
            Capture cap_b;
            const Capture& b_cap = mut_b ? (cap_b = p10_load(dir + "/prog_p10_mut_b.bin")) : base;
            bool have_b = b_cap.loaded && b_cap.done.has_value();
            std::optional<size_t> idx_b;
            std::vector<Rec> recs_b = have_b ? p10_decode(b_cap) : std::vector<Rec>{};
            if (have_b) { idx_b = sample_idx_on_line(recs_b, 0x38); have_b = idx_b.has_value(); }
            pre_b = have_b && ((recs_b[*idx_b].intreqr & 0x14) == 0);
            checks.expect(pre_b, "P10b precondition: the $38 sample shows bit 4 = 0 and bit 2 = 0");

            // (c) INTENAR bits 14-0 read 0 (read/stored after step 2), and the
            // 68000 writes INTREQ nowhere after step 2 (true by construction
            // of every program here).
            bool mut_c = (has("P10-C"));
            Capture cap_c;
            const Capture& c_cap = mut_c ? (cap_c = p10_load(dir + "/prog_p10_mut_c.bin")) : base;
            pre_c = c_cap.loaded && c_cap.done.has_value() && !c_cap.peeks.empty() &&
                    c_cap.peeks[0] && ((*c_cap.peeks[0]) & 0x7FFF) == 0;
            checks.expect(pre_c, "P10c precondition: stored INTENAR bits 14-0 read 0");
        } else {
            checks.skip("P10a", "P10-FIX-LINES failed");
            checks.skip("P10b", "P10-FIX-LINES failed");
            checks.skip("P10c", "P10-FIX-LINES failed");
        }
        const bool p10_pre = have_all && pre_a && pre_b && pre_c;

        // ---- Main assertion: (bit 4, bit 2) at the five sample lines. Own
        // MUTATION: the capture of prog_p10_main2.bin (2nd MOVE data
        // $8010, 3rd MOVE data $8004) graded in its place.
        std::vector<Rec> recs_swapped;
        if (has("P10-SWAP") && main2_ok) recs_swapped = p10_decode(main2);
        const std::vector<Rec>& table_recs = (has("P10-SWAP")) ? recs_swapped : recs;
        {
            const int expected_b4[5] = {0, 1, 0, 0, 1};
            const int expected_b2[5] = {0, 1, 1, 0, 0};
            bool table_ok = true;
            for (int k = 0; k < 5; ++k) {
                auto ix = sample_idx_on_line(table_recs, p10_lines[k]);
                if (!ix) { table_ok = false; continue; }
                int b4 = (table_recs[*ix].intreqr >> 4) & 1;
                int b2 = (table_recs[*ix].intreqr >> 2) & 1;
                if (b4 != expected_b4[k] || b2 != expected_b2[k]) table_ok = false;
            }
            if (p10_pre) checks.expect(table_ok,
                "P10 ch2-the-copper-and-the-680x0.c5,COP2-the-copper-and-the-680x0.c5,"
                "COP2-the-copper-and-the-680x0.c6,COP2-the-copper-and-the-680x0.c7 [p.35,p.218,p.280]: "
                "(bit 4, bit 2) at lines $38/$48/$58/$68/$78 read (0,0),(1,1),(0,1),(0,0),(1,0)");
            else checks.skip("P10", "a P10 fixture or precondition failed");
        }

        // ---- Main contrast: 2nd MOVE $0010->$8010, 3rd MOVE $0004->$8004
        // (one combined run). No claim id. Own MUTATION: the capture of
        // prog_p10_main.bin graded in its place.
        {
            Capture main_for_contrast;
            if (has("P10-MAIN2-SWAP")) main_for_contrast = p10_load(dir + "/prog_p10_main.bin");
            const Capture& m2 = (has("P10-MAIN2-SWAP")) ? main_for_contrast : main2;
            bool ok = false;
            if (m2.loaded && m2.done.has_value()) {
                auto recs2 = p10_decode(m2);
                auto i58 = sample_idx_on_line(recs2, 0x58);
                auto i68 = sample_idx_on_line(recs2, 0x68);
                ok = i58 && i68 &&
                     ((recs2[*i58].intreqr >> 4) & 1) == 1 && ((recs2[*i58].intreqr >> 2) & 1) == 1 &&
                     ((recs2[*i68].intreqr >> 2) & 1) == 1;
            }
            if (p10_pre && main2_ok) checks.expect(ok,
                "P10-MAIN2 [p.35,p.218]: with the 2nd MOVE's data $0010->$8010, the $58 sample "
                "reads (1,1); with the 3rd MOVE's data $0004->$8004 as well, the $68 sample's "
                "bit 2 reads 1");
            else checks.skip("P10-MAIN2", "a P10 fixture or precondition failed");
        }
    }

    // ==== P8: the 680x0 can poll INTREQ flags that various devices set
    // [p.35] ==== ch2-the-copper-and-the-680x0.c2,
    // COP2-the-copper-and-the-680x0.c2,.c3: carried by the aggregate P8
    // alone. P8-COP, P8-VB and P8-BLIT are its components and carry no
    // claim id.
    if (want("P8")) {
        constexpr uint32_t kP8CopMoveIr2 = kList1 / 2 + 3; // LIST1+6
        auto p8_load = [&](const std::string& binpath, const std::vector<uint32_t>& extra) {
            std::vector<uint32_t> a = {kResults + 4, kResults + 8, kResults + 10};
            for (auto e : extra) a.push_back(e);
            return run_capture(binpath, kSamplerRunBudget, a);
        };
        auto p8_recs = [&](const Capture& c) {
            return decode_records(c.log, kRecBuf, rec_count(c, 1));
        };
        auto done_of = [](const Capture& c) { return c.loaded && c.done.has_value(); };
        auto intenar_zero = [&](const Capture& c) {
            return done_of(c) && !c.peeks.empty() && c.peeks[0] && ((*c.peeks[0]) & 0x7FFF) == 0;
        };

        // ---- Captures. Each is loaded once; a mutation names the capture
        // graded in a line's place.
        Capture cop    = p8_load(bin("/prog_p8_cop.bin",        {"P8-COP-FIX-DONE"}), {});
        Capture vb     = p8_load(bin("/prog_p8_vb.bin",         {"P8-VB-FIX-DONE"}), {});
        Capture blit   = p8_load(bin("/prog_p8_blit.bin",       {"P8-BLIT-FIX-DONE"}), {kResults + 12});
        Capture nomove = p8_load(bin("/prog_p8_cop_nomove.bin", {"P8-COP-DEV-FIX-DONE"}), {});
        Capture iii    = p8_load(bin("/prog_p8_cop_mut_iii.bin", {"P8-COP-III-FIX-DONE"}), {});
        Capture narrow = p8_load(bin("/prog_p8_vb_narrow.bin",  {"P8-VB-DEV-FIX-DONE"}), {});
        Capture nosize = p8_load(bin("/prog_p8_blit_nosize.bin", {"P8-BLIT-DEV-FIX-DONE"}), {});
        // Captures that a swap selector grades in a component's place; they
        // are run only under that selector.
        Capture swap_vb, swap_blit;
        if (has("P8-VB-SWAP"))   swap_vb   = p8_load(dir + "/prog_p8_vb_mut_iii.bin", {});
        if (has("P8-BLIT-SWAP")) swap_blit = p8_load(dir + "/prog_p8_blit_mut_iii.bin", {});
        checks.expect(done_of(cop),    "P8-COP-FIX-DONE: prog_p8_cop.bin reaches DONE");
        checks.expect(done_of(vb),     "P8-VB-FIX-DONE: prog_p8_vb.bin reaches DONE");
        checks.expect(done_of(blit),   "P8-BLIT-FIX-DONE: prog_p8_blit.bin reaches DONE");
        checks.expect(done_of(nomove), "P8-COP-DEV-FIX-DONE: prog_p8_cop_nomove.bin reaches DONE");
        checks.expect(done_of(iii),    "P8-COP-III-FIX-DONE: prog_p8_cop_mut_iii.bin reaches DONE");
        checks.expect(done_of(narrow), "P8-VB-DEV-FIX-DONE: prog_p8_vb_narrow.bin reaches DONE");
        checks.expect(done_of(nosize), "P8-BLIT-DEV-FIX-DONE: prog_p8_blit_nosize.bin reaches DONE");

        // The VB event: the first frame wrap (2.4: a decrease from V >= 256
        // to V <= 1) whose record is stored after t(MC).
        struct VbEvent { std::optional<uint64_t> t_mc, t_ev; size_t a = 0, b = 0; };
        auto vb_event = [&](const Capture& c, const std::vector<Rec>& recs) {
            VbEvent e;
            e.t_mc = first_write_cycle(c.log, kMarkBuf + 6);
            if (!e.t_mc) return e;
            for (size_t i = 1; i < recs.size(); ++i)
                if (recs[i - 1].V >= 256 && recs[i].V <= 1 && recs[i].t0 > *e.t_mc) {
                    e.t_ev = recs[i].t0; e.a = i - 1; e.b = i; break;
                }
            return e;
        };

        // The COP event: the first fetch of the MOVE's IR2 after t(MC).
        auto cop_event = [&](const Capture& c) -> std::optional<uint64_t> {
            auto t_mc = first_write_cycle(c.log, kMarkBuf + 6);
            if (!t_mc) return std::nullopt;
            return first_channel_read_at(c.log, kP8CopMoveIr2, *t_mc);
        };

        // ---- Preconditions (c): the event happens inside the window.
        // COP: MC and a fetch of the Copper's MOVE IR2 after MC are both present.
        bool c_cop = false;
        {
            Capture cap_c;
            const Capture& cc = (has("P8-C")) ? (cap_c = p8_load(dir + "/prog_p8_cop_mut_c.bin", {})) : cop;
            c_cop = done_of(cc) && first_write_cycle(cc.log, kMarkBuf + 6).has_value() &&
                    cop_event(cc).has_value();
        }
        if (done_of(cop)) checks.expect(c_cop,
            "P8c-COP precondition: MC and the event (the first fetch of the Copper's MOVE IR2 after MC) "
            "are both present, so the event happens inside the window (COP case)");
        else checks.skip("P8c-COP", "P8-COP-FIX-DONE failed");
        // VB: MC and a frame wrap stored after MC are both present.
        bool c_vb = false;
        {
            Capture cap_c;
            const Capture& cc = (has("P8-VB-C")) ? (cap_c = narrow) : vb;
            if (done_of(cc)) {
                auto rc = p8_recs(cc);
                VbEvent e = vb_event(cc, rc);
                c_vb = e.t_mc.has_value() && e.t_ev.has_value();
            }
        }
        if (done_of(vb)) checks.expect(c_vb,
            "P8c-VB precondition: MC and a frame wrap stored after MC are both present, so the event "
            "happens inside the window (VB case)");
        else checks.skip("P8c-VB", "P8-VB-FIX-DONE failed");
        // BLIT: MC is present, and 2048 writes of $FFFF appear in D, the first after MC.
        bool c_blit = false;
        {
            const Capture& cc = (has("P8-BLIT-C")) ? nosize : blit;
            auto t_mc = first_write_cycle(cc.log, kMarkBuf + 6);
            if (done_of(cc) && t_mc) {
                auto ws = writes_of_value_in(cc.log, 0xFFFF, kData1, kData1 + 0x1000, *t_mc, UINT64_MAX);
                c_blit = ws.count == 2048 && ws.first && *ws.first > *t_mc;
            }
        }
        if (done_of(blit)) checks.expect(c_blit,
            "P8c-BLIT precondition: MC is present and 2048 writes of $FFFF appear in region D, the "
            "first after MC (BLIT case)");
        else checks.skip("P8c-BLIT", "P8-BLIT-FIX-DONE failed");

        // ---- Preconditions (a): INTENAR bits 14-0 read 0 in every case.
        bool a_cop = false, a_vb = false, a_blit = false;
        {
            Capture cap_a;
            a_cop = intenar_zero((has("P8-A")) ? (cap_a = p8_load(dir + "/prog_p8_cop_mut_a.bin", {})) : cop);
        }
        if (done_of(cop)) checks.expect(a_cop, "P8a-COP precondition: stored INTENAR bits 14-0 read 0 (COP case)");
        else checks.skip("P8a-COP", "P8-COP-FIX-DONE failed");
        {
            Capture cap_a;
            a_vb = intenar_zero((has("P8-VB-A")) ? (cap_a = p8_load(dir + "/prog_p8_vb_mut_a.bin", {})) : vb);
        }
        if (done_of(vb)) checks.expect(a_vb, "P8a-VB precondition: stored INTENAR bits 14-0 read 0 (VB case)");
        else checks.skip("P8a-VB", "P8-VB-FIX-DONE failed");
        {
            Capture cap_a;
            a_blit = intenar_zero((has("P8-BLIT-A")) ? (cap_a = p8_load(dir + "/prog_p8_blit_mut_a.bin", {kResults + 12})) : blit);
        }
        if (done_of(blit)) checks.expect(a_blit, "P8a-BLIT precondition: stored INTENAR bits 14-0 read 0 (BLIT case)");
        else checks.skip("P8a-BLIT", "P8-BLIT-FIX-DONE failed");

        // ---- Preconditions (b): the first record stored after MC reads the
        // case's bit as 0, so a later 1 is new.
        bool b_cop = false, b_vb = false, b_blit = false;
        {
            Capture cap_b;
            const Capture& cb = (has("P8-COP-B")) ? (cap_b = p8_load(dir + "/prog_p8_cop_mut_b.bin", {})) : cop;
            auto t_mc = first_write_cycle(cb.log, kMarkBuf + 6);
            b_cop = done_of(cb) && t_mc && first_after_mc_reads_zero(p8_recs(cb), *t_mc, 4);
        }
        if (done_of(cop)) checks.expect(b_cop,
            "P8b-COP precondition: the first poll record stored after MC reads bit 4 = 0 (COP case)");
        else checks.skip("P8b-COP", "P8-COP-FIX-DONE failed");
        {
            Capture cap_b;
            const Capture& cb = (has("P8-B")) ? (cap_b = p8_load(dir + "/prog_p8_vb_mut_b.bin", {})) : vb;
            auto t_mc = first_write_cycle(cb.log, kMarkBuf + 6);
            b_vb = done_of(cb) && t_mc && first_after_mc_reads_zero(p8_recs(cb), *t_mc, 5);
        }
        if (done_of(vb)) checks.expect(b_vb,
            "P8b-VB precondition: the first poll record stored after MC reads bit 5 = 0 (VB case)");
        else checks.skip("P8b-VB", "P8-VB-FIX-DONE failed");
        {
            Capture cap_b;
            const Capture& cb = (has("P8-BLIT-B")) ? (cap_b = p8_load(dir + "/prog_p8_blit_mut_b.bin", {kResults + 12})) : blit;
            auto t_mc = first_write_cycle(cb.log, kMarkBuf + 6);
            b_blit = done_of(cb) && t_mc && first_after_mc_reads_zero(p8_recs(cb), *t_mc, 6);
        }
        if (done_of(blit)) checks.expect(b_blit,
            "P8b-BLIT precondition: the first poll record stored after MC reads bit 6 = 0 (BLIT case)");
        else checks.skip("P8b-BLIT", "P8-BLIT-FIX-DONE failed");

        // ---- Preconditions (d): the population that (i) speaks about is not
        // empty -- at least one poll record is stored after MC and before
        // the event. Each is printed before the components.
        auto pop_between = [](const std::vector<Rec>& recs, uint64_t t_mc, uint64_t t_ev) {
            int n = 0;
            for (const auto& r : recs) if (r.t0 > t_mc && r.t1 < t_ev) ++n;
            return n;
        };
        bool d_cop = false, d_vb = false, d_blit = false;
        if (done_of(cop) && c_cop) {
            Capture cap_d;
            const Capture& cd = has("P8-COP-D") ? (cap_d = p8_load(dir + "/prog_p8_cop_mut_d.bin", {})) : cop;
            auto t_mc = first_write_cycle(cd.log, kMarkBuf + 6);
            auto t_ev = cop_event(cd);
            int n = (done_of(cd) && t_mc && t_ev) ? pop_between(p8_recs(cd), *t_mc, *t_ev) : -1;
            d_cop = n > 0;
            checks.expect(d_cop,
                "P8d-COP precondition: at least one poll record is stored after MC and before the event "
                "(COP case)");
            checks.note("P8d-COP: " + (n < 0 ? std::string("no event in the capture") : std::to_string(n) + " poll record(s) stored after MC and before the event"));
        } else checks.skip("P8d-COP", "P8-COP-FIX-DONE or P8c-COP failed");
        if (done_of(vb) && c_vb) {
            Capture cap_d;
            const Capture& cd = has("P8-VB-D") ? (cap_d = p8_load(dir + "/prog_p8_vb_mut_d.bin", {})) : vb;
            int n = -1;
            if (done_of(cd)) {
                auto rd = p8_recs(cd);
                VbEvent e = vb_event(cd, rd);
                if (e.t_mc && e.t_ev) n = pop_between(rd, *e.t_mc, *e.t_ev);
            }
            d_vb = n > 0;
            checks.expect(d_vb,
                "P8d-VB precondition: at least one poll record is stored after MC and before the event "
                "(VB case)");
            checks.note("P8d-VB: " + (n < 0 ? std::string("no event in the capture") : std::to_string(n) + " poll record(s) stored after MC and before the event"));
        } else checks.skip("P8d-VB", "P8-VB-FIX-DONE or P8c-VB failed");
        if (done_of(blit) && c_blit) {
            Capture cap_d;
            const Capture& cd = has("P8-BLIT-D") ? (cap_d = p8_load(dir + "/prog_p8_blit_mut_d.bin", {})) : blit;
            auto t_mc = first_write_cycle(cd.log, kMarkBuf + 6);
            int n = -1;
            if (done_of(cd) && t_mc) {
                auto ws = writes_of_value_in(cd.log, 0xFFFF, kData1, kData1 + 0x1000, *t_mc, UINT64_MAX);
                if (ws.last) n = pop_between(p8_recs(cd), *t_mc, *ws.last);
            }
            d_blit = n > 0;
            checks.expect(d_blit,
                "P8d-BLIT precondition: at least one poll record is stored after MC and before the event "
                "(BLIT case)");
            checks.note("P8d-BLIT: " + (n < 0 ? std::string("no event in the capture") : std::to_string(n) + " poll record(s) stored after MC and before the event"));
        } else checks.skip("P8d-BLIT", "P8-BLIT-FIX-DONE or P8c-BLIT failed");

        // ---- VB reference frame: the flag changes from 0 to 1 at the frame
        // wrap. The last record stored before the wrap reads bit 5 = 0, and a
        // record on line 0 or line 1 (the records from the wrap record on,
        // while V <= 1) reads bit 5 = 1.
        bool boundary_ok = false;
        {
            Capture cap_bd;
            const Capture& cb = has("P8-VB-BOUNDARY-A") ? (cap_bd = p8_load(dir + "/prog_p8_vb_mut_bnd_a.bin", {}))
                              : has("P8-VB-BOUNDARY-B") ? (cap_bd = p8_load(dir + "/prog_p8_vb_mut_bnd_b.bin", {}))
                                                        : vb;
            if (done_of(cb)) {
                auto rb = p8_recs(cb);
                VbEvent e = vb_event(cb, rb);
                if (e.t_ev) {
                    bool before_zero = ((rb[e.a].intreqr >> 5) & 1) == 0;
                    bool at_wrap_one = false;
                    for (size_t i = e.b; i < rb.size() && rb[i].V <= 1; ++i)
                        if ((rb[i].intreqr >> 5) & 1) { at_wrap_one = true; break; }
                    boundary_ok = before_zero && at_wrap_one;
                    checks.note(std::string("P8-VB wrap boundary records: V=") + std::to_string(rb[e.a].V) +
                                " -> V=" + std::to_string(rb[e.b].V) + "; bit 5 reads " +
                                (before_zero ? "0" : "1") + " in the last record before the wrap and " +
                                (at_wrap_one ? "1" : "0") + " in a record at V <= 1 from the wrap on");
                }
            }
        }
        if (done_of(vb) && c_vb) checks.expect(boundary_ok,
            "P8-VB-BOUNDARY precondition [p.219]: INTREQR bit 5 reads 0 in the last record stored before the "
            "frame wrap and 1 in a record on line 0 or line 1 from the wrap on");
        else checks.skip("P8-VB-BOUNDARY", "P8-VB-FIX-DONE or P8c-VB failed");

        // ---- Component results: every poll record stored after MC and
        // before the device's event reads the case's bit as 0, a later
        // record reads 1, and every record after the first 1 reads 1.
        // COP: the event is the MOVE's IR2 fetch. Own MUTATION
        // (P8-COP-SWAP): the capture with the MOVE replaced by COLOR00.
        const bool cop_guards = done_of(cop) && c_cop && a_cop && b_cop && d_cop;
        const bool vb_guards = done_of(vb) && c_vb && a_vb && b_vb && d_vb && boundary_ok;
        const bool blit_guards = done_of(blit) && c_blit && a_blit && b_blit && d_blit;
        bool cop_cond = false, vb_cond = false, blit_cond = false;
        bool cop_event_in = false, vb_event_in = false, blit_event_in = false;
        {
            const Capture& cr = (has("P8-COP-SWAP")) ? nomove : cop;
            auto t_mc = first_write_cycle(cr.log, kMarkBuf + 6);
            auto t_ev = cop_event(cr);
            if (done_of(cr) && t_mc && t_ev) {
                cop_event_in = true;
                auto g = grade_p8_bit(p8_recs(cr), *t_mc, *t_ev, 4);
                cop_cond = g.zero_before && g.later_one && g.after_first_one;
            }
        }
        {
            const Capture& cr = (has("P8-VB-SWAP")) ? swap_vb : vb;
            if (done_of(cr)) {
                auto rr = p8_recs(cr);
                VbEvent e = vb_event(cr, rr);
                if (e.t_mc && e.t_ev) {
                    vb_event_in = true;
                    auto g = grade_p8_bit(rr, *e.t_mc, *e.t_ev, 5);
                    vb_cond = g.zero_before && g.later_one && g.after_first_one;
                }
            }
        }
        {
            const Capture& cr = (has("P8-BLIT-SWAP")) ? swap_blit : blit;
            auto t_mc = first_write_cycle(cr.log, kMarkBuf + 6);
            if (done_of(cr) && t_mc) {
                auto ws = writes_of_value_in(cr.log, 0xFFFF, kData1, kData1 + 0x1000, *t_mc, UINT64_MAX);
                if (ws.last) {
                    blit_event_in = true;
                    auto g = grade_p8_bit(p8_recs(cr), *t_mc, *ws.last, 6);
                    blit_cond = g.zero_before && g.later_one && g.after_first_one;
                }
            }
        }
        // A component whose own capture has no event is not graded.
        if (cop_guards && cop_event_in) checks.expect(cop_cond,
            "P8-COP [p.35,p.280,p.219]: every poll record after MC and before the Copper's MOVE executes "
            "reads bit 4 = 0, a later record reads 1, and every record after the first 1 reads 1 "
            "(component of P8; carries no claim id)");
        else if (cop_guards) checks.skip("P8-COP", "the capture graded has no event after MC");
        else checks.skip("P8-COP", "a COP fixture or precondition failed");
        if (vb_guards && vb_event_in) checks.expect(vb_cond,
            "P8-VB [p.35,p.280,p.219]: every poll record after MC and before the frame wrap reads bit 5 "
            "= 0, a later record reads 1, and every record after the first 1 reads 1 (component of P8; "
            "carries no claim id)");
        else if (vb_guards) checks.skip("P8-VB", "the capture graded has no frame wrap after MC");
        else checks.skip("P8-VB", "a VB fixture or precondition failed");
        if (blit_guards && blit_event_in) checks.expect(blit_cond,
            "P8-BLIT [p.35,p.280,p.220,p.187]: every poll record after MC and before the last write in "
            "region D reads bit 6 = 0, a later record reads 1, and every record after the first 1 reads 1 "
            "(component of P8; carries no claim id)");
        else if (blit_guards) checks.skip("P8-BLIT", "the capture graded has no write in region D after MC");
        else checks.skip("P8-BLIT", "a BLIT fixture or precondition failed");

        // ---- The aggregate: the three devices together.
        if (cop_guards && vb_guards && blit_guards && cop_event_in && vb_event_in && blit_event_in)
            checks.expect(cop_cond && vb_cond && blit_cond,
            "P8 ch2-the-copper-and-the-680x0.c2,COP2-the-copper-and-the-680x0.c2,"
            "COP2-the-copper-and-the-680x0.c3 [p.35,p.280,p.219,p.220,p.187]: with every interrupt "
            "disabled, a 68000 polling INTREQR sees the Copper's flag (bit 4), the start-of-vertical-"
            "blank flag (bit 5) and the blitter's flag (bit 6) each read 0 until its device's event, "
            "then 1, and keep reading 1");
        else checks.skip("P8", "a COP, VB or BLIT fixture, precondition or event failed");

        // ---- Device contrasts. Each has its own fixture, printed above, and
        // is a control: it carries no claim id.
        // COP: the MOVE replaced by COLOR00. Bit 4 stays 0. Own MUTATION
        // (P8-COP-DEV-SWAP): the main COP capture graded in its place.
        {
            const Capture& cr = (has("P8-COP-DEV-SWAP")) ? cop : nomove;
            auto t_mc = first_write_cycle(cr.log, kMarkBuf + 6);
            bool ok = done_of(cr) && t_mc.has_value();
            if (ok) for (const auto& r : p8_recs(cr)) if (r.t0 > *t_mc && (r.intreqr & 0x10)) ok = false;
            if (done_of(nomove)) checks.expect(ok, "P8-COP-DEV [p.35]: with the MOVE replaced by $0180,$0F00 "
                                                    "(COLOR00), bit 4 never reads 1 after MC");
            else checks.skip("P8-COP-DEV", "P8-COP-DEV-FIX-DONE failed");
        }
        // COP check (iii) contrast: once a poll record reads bit 4 = 1 the
        // program clears it again, a later record reads 0, and check (iii)
        // fails. Own MUTATION (P8-COP-III-SWAP): the main COP capture
        // graded in its place.
        {
            const Capture& cr = (has("P8-COP-III-SWAP")) ? cop : iii;
            auto t_mc = first_write_cycle(cr.log, kMarkBuf + 6);
            auto t_ev = cop_event(cr);
            bool ok = done_of(cr) && t_mc.has_value() && t_ev.has_value();
            if (ok) {
                auto g = grade_p8_bit(p8_recs(cr), *t_mc, *t_ev, 4);
                ok = !g.after_first_one; // (iii) must now fail
            }
            if (done_of(iii)) checks.expect(ok,
                "P8-COP-III [p.280]: once a poll record reads bit 4 = 1, the program clears it again with a "
                "write to INTREQ; a later record then reads 0, and check (iii) fails");
            else checks.skip("P8-COP-III", "P8-COP-III-FIX-DONE failed");
        }
        // VB: poll only V=$40..$F0 of one frame. Bit 5 stays 0 (no VERTB
        // event; this removes the event too). Own MUTATION
        // (P8-VB-DEV-SWAP): the main VB capture graded in its place.
        {
            const Capture& cr = (has("P8-VB-DEV-SWAP")) ? vb : narrow;
            auto t_mc = first_write_cycle(cr.log, kMarkBuf + 6);
            bool ok = done_of(cr) && t_mc.has_value();
            if (ok) for (const auto& r : p8_recs(cr)) if (r.t0 > *t_mc && (r.intreqr & 0x20)) ok = false;
            if (done_of(narrow)) checks.expect(ok, "P8-VB-DEV [p.219]: polling only from V=$40 to V=$F0, bit 5 "
                                                    "never reads 1");
            else checks.skip("P8-VB-DEV", "P8-VB-DEV-FIX-DONE failed");
        }
        // BLIT: omit BLTSIZE. Bit 6 stays 0. Own MUTATION (P8-BLIT-DEV-SWAP):
        // the main BLIT capture graded in its place.
        {
            const Capture& cr = (has("P8-BLIT-DEV-SWAP")) ? blit : nosize;
            auto t_mc = first_write_cycle(cr.log, kMarkBuf + 6);
            bool ok = done_of(cr) && t_mc.has_value();
            if (ok) for (const auto& r : p8_recs(cr)) if (r.t0 > *t_mc && (r.intreqr & 0x40)) ok = false;
            if (done_of(nosize)) checks.expect(ok, "P8-BLIT-DEV [p.35]: omitting the BLTSIZE write, bit 6 never "
                                                    "reads 1");
            else checks.skip("P8-BLIT-DEV", "P8-BLIT-DEV-FIX-DONE failed");
        }
    }

    // ==== P6: beam coordinates run left to right and top to bottom on the
    // screen [p.27] (no claim id) ====
    if (want("P6")) {
        auto is_color = [](const nmh::VideoSample& s, uint8_t r, uint8_t g, uint8_t b) {
            return s.r == r && s.g == g && s.b == b;
        };
        constexpr uint32_t kP6ListLen = 32;                 // LIST1 .. LIST1+31
        constexpr uint32_t kP6EndIr2 = kList1 + 30;         // LIST1+28/+30 = END (main shape)

        // C0=$0F00 (r=F,g=0,b=0), C1=$00F0 (r=0,g=F,b=0), C2=$000F
        // (r=0,g=0,b=F), C3=$0FF0 (r=F,g=F,b=0).
        struct P6Result {
            bool ok = false;
            std::optional<size_t> r1, r3;      // row indices in the graded frame
            std::optional<size_t> i0, i1, i2;  // leftmost sample index of C0/C1/C2 on row r1
            std::optional<size_t> col2;         // i2 counted from row r1's hs edge (2.5's column)
            bool all_four = false;
            nmh::Span frame{0, 0};
            std::vector<nmh::Span> lines;      // doubler rows inside the graded frame
        };

        // Runs `binpath`, finds the second complete video frame after t(MJ),
        // and measures everything P6's checks need from it.
        auto analyze_p6 = [&](const std::string& binpath) -> P6Result {
            P6Result r;
            FullRun run = run_full(binpath, kSamplerRunBudget);
            if (!run.loaded || !run.done) return r;
            const auto& log = run.h->ram_log();
            const auto& video = run.h->video();
            auto t_mj = first_write_cycle(log, kMarkBuf + 4);
            if (!t_mj) return r;
            auto vframes = nmh::find_frames(video);
            std::optional<size_t> f1;
            for (size_t i = 0; i < vframes.size(); ++i)
                if (vframes[i].begin > *t_mj) { f1 = i; break; }
            if (!f1 || *f1 + 1 >= vframes.size()) return r;
            r.frame = vframes[*f1 + 1];
            auto lines = nmh::find_lines(video);
            for (auto& ln : lines)
                if (ln.begin >= r.frame.begin && ln.end <= r.frame.end) r.lines.push_back(ln);

            auto first_row_with = [&](uint8_t cr, uint8_t cg, uint8_t cb) -> std::optional<size_t> {
                for (size_t i = 0; i < r.lines.size(); ++i)
                    for (size_t j = r.lines[i].begin; j < r.lines[i].end; ++j)
                        if (is_color(video[j], cr, cg, cb)) return i;
                return std::nullopt;
            };
            r.r1 = first_row_with(0x0, 0xF, 0x0); // first row with C1
            r.r3 = first_row_with(0xF, 0xF, 0x0); // first row with C3
            if (r.r1) {
                nmh::Span row = r.lines[*r.r1];
                for (size_t j = row.begin; j < row.end; ++j) {
                    if (!r.i0 && is_color(video[j], 0xF, 0x0, 0x0)) r.i0 = j;
                    if (!r.i1 && is_color(video[j], 0x0, 0xF, 0x0)) r.i1 = j;
                    if (!r.i2 && is_color(video[j], 0x0, 0x0, 0xF)) r.i2 = j;
                }
                if (r.i2) r.col2 = *r.i2 - row.begin;
            }
            bool c0 = false, c1 = false, c2 = false, c3 = false;
            for (size_t j = r.frame.begin; j < r.frame.end; ++j) {
                if (is_color(video[j], 0xF, 0x0, 0x0)) c0 = true;
                if (is_color(video[j], 0x0, 0xF, 0x0)) c1 = true;
                if (is_color(video[j], 0x0, 0x0, 0xF)) c2 = true;
                if (is_color(video[j], 0xF, 0xF, 0x0)) c3 = true;
            }
            r.all_four = c0 && c1 && c2 && c3;
            r.ok = true;
            return r;
        };

        std::string base_bin = bin("/prog_p6_main.bin", {"P6-FIX-FRAME"});
        if (has("P6-FIX-ROWS-MUTA")) base_bin = dir + "/prog_p6_mut_a.bin";
        P6Result base = analyze_p6(base_bin);
        checks.expect(base.ok, "P6-FIX-FRAME: prog_p6_main.bin reaches DONE and yields a second "
                                "complete video frame after the enable");
        bool have_rows = base.ok && base.r1.has_value() && base.r3.has_value();
        if (base.ok) checks.expect(have_rows,
            "P6-FIX-ROWS: row r1 (first with C1) and row r3 (first with C3) were both found");
        else checks.skip("P6-FIX-ROWS", "P6-FIX-FRAME failed");

        P6Result wc1 = analyze_p6(has("P6-I-SWAP") ? dir + "/prog_p6_main.bin"
                                                      : bin("/prog_p6_wait2_c1.bin", {"P6-i-FIX"}));
        P6Result wb0 = analyze_p6(has("P6-II-SWAP") ? dir + "/prog_p6_main.bin"
                                                       : bin("/prog_p6_wait3_b0.bin", {"P6-ii-FIX"}));
        checks.expect(wc1.ok, "P6-i-FIX: prog_p6_wait2_c1.bin reaches DONE and yields a second complete "
                               "video frame after the enable");
        checks.expect(wb0.ok, "P6-ii-FIX: prog_p6_wait3_b0.bin reaches DONE and yields a second complete "
                               "video frame after the enable");

        // ---- Precondition (a): all four colours appear. It reads the same
        // capture as P6-FIX-ROWS, so it is graded only when that line passed.
        bool pre_a = false, pre_b = false, pre_c = false;
        if (have_rows) {
            bool mut_a = (has("P6-A"));
            P6Result a_res = mut_a ? analyze_p6(dir + "/prog_p6_mut_a.bin") : P6Result{};
            const P6Result& a_src = mut_a ? a_res : base;
            pre_a = a_src.ok && a_src.all_four;
            checks.expect(pre_a, "P6a precondition: all four colours (C0-C3) appear in the graded frame");
        } else {
            checks.skip("P6a", "P6-FIX-FRAME or P6-FIX-ROWS failed");
        }
        if (base.ok) {
            // ---- Precondition (b): at least three restarts follow t(MJ),
            // each identified at the port as a fetch of the list's first word
            // after a fetch of the final WAIT's IR2, and between each two
            // consecutive restarts each of the four MOVEs is executed
            // exactly once, in list order.
            bool mut_b = (has("P6-B"));
            Capture cap_b = run_capture(dir + (mut_b ? "/prog_p6_mut_b.bin" : "/prog_p6_main.bin"),
                                         kSamplerRunBudget, {});
            auto t_mj_b = first_write_cycle(cap_b.log, kMarkBuf + 4);
            pre_b = cap_b.loaded && cap_b.done.has_value() && t_mj_b.has_value();
            if (pre_b) {
                auto restarts = find_list_restarts(cap_b.log, kList1, kP6ListLen, kP6EndIr2);
                std::vector<Restart> after;
                for (auto& r : restarts) if (r.t_restart > *t_mj_b) after.push_back(r);
                pre_b = after.size() >= 3;
                const uint32_t move_ir2[4] = {
                    kList1 / 2 + 1, kList1 / 2 + 5, kList1 / 2 + 9, kList1 / 2 + 13,
                };
                for (size_t i = 0; i + 1 < after.size() && pre_b; ++i) {
                    uint64_t lo = after[i].t_restart, hi = after[i + 1].t_final;
                    uint64_t last = lo;
                    for (int k = 0; k < 4 && pre_b; ++k) {
                        int n = count_channel_reads_at(cap_b.log, move_ir2[k], lo, hi);
                        if (n != 1) pre_b = false;
                        auto t = first_channel_read_at(cap_b.log, move_ir2[k], last);
                        if (!t || *t >= hi) pre_b = false; else last = *t;
                    }
                }
            }
            checks.expect(pre_b,
                "P6b precondition: at least three restarts follow t(MJ), each a fetch of the list's first "
                "word after a fetch of the final WAIT's IR2, and between each two consecutive restarts "
                "each of the four MOVEs is executed exactly once, in list order");

            // ---- Precondition (c): V1. Compared only over frames after
            // t(MJ): find_frames() include the leading, irregular-length
            // partial span from simulation start to the first vsync edge
            // (and several frames pass during boot(), before the CPU is
            // released), so the comparison basis has to start after the
            // run's own enable.
            FullRun run_c = run_full(dir + (has("P6-C") ? "/prog_p6_mut_c.bin"
                                           : has("P6-C2") ? "/prog_p6_mut_c2.bin"
                                           : has("P6-C3") ? "/prog_p6_mut_c3.bin" : "/prog_p6_main.bin"),
                                      kSamplerRunBudget);
            pre_c = run_c.loaded && run_c.done;
            if (pre_c) {
                auto t_mj_vc = first_write_cycle(run_c.h->ram_log(), kMarkBuf + 4);
                auto vf = nmh::find_frames(run_c.h->video());
                std::optional<size_t> vc0;
                if (t_mj_vc) for (size_t i = 0; i < vf.size(); ++i) if (vf[i].begin > *t_mj_vc) { vc0 = i; break; }
                pre_c = vc0 && *vc0 + 3 < vf.size();
                if (pre_c) {
                    pre_c = anchor_V1(run_c.h->video(),
                        {vf[*vc0], vf[*vc0 + 1], vf[*vc0 + 2], vf[*vc0 + 3]});
                    std::string m = "P6c: clk_sys samples and doubler rows of the four video frames after the enable:";
                    for (size_t k = 0; k < 4; ++k) {
                        const nmh::Span& f = vf[*vc0 + k];
                        std::vector<nmh::VideoSample> sub(run_c.h->video().begin() + f.begin,
                                                          run_c.h->video().begin() + f.end);
                        m += " " + std::to_string(f.end - f.begin) + "/" + std::to_string(nmh::find_lines(sub).size());
                    }
                    checks.note(m);
                }
            }
            checks.expect(pre_c, "P6c precondition: V1 holds (every one of four video frames after "
                                  "the enable has the same clk_sys sample count and the same doubler "
                                  "row count)");
        } else {
            checks.skip("P6b", "P6-FIX-FRAME failed");
            checks.skip("P6c", "P6-FIX-FRAME failed");
        }
        const bool p6_pre = base.ok && have_rows && pre_a && pre_b && pre_c;

        // ---- Results, graded after every precondition above.
        // Row r3 (first row with C3) is below row r1 (first row with C1).
        // Own MUTATION (P6-BELOW-SWAP): the values of C1 and C3 exchanged.
        {
            P6Result sw;
            if (has("P6-BELOW-SWAP")) sw = analyze_p6(dir + "/prog_p6_mut_swap13.bin");
            const P6Result& gb = (has("P6-BELOW-SWAP")) ? sw : base;
            bool cond = gb.ok && gb.r1.has_value() && gb.r3.has_value() && *gb.r3 > *gb.r1;
            if (p6_pre) checks.expect(cond,
                "P6-BELOW [p.27]: row r3 (first row with C3) is below row r1 (first row with C1)");
            else checks.skip("P6-BELOW", "a P6 fixture or precondition failed");
        }
        // On row r1, reading left to right, a C0 (red) run comes first, then
        // the C1 (green) run, then a C2 (blue) run. Own MUTATION
        // (P6-ORDER-SWAP): the values of C0 and C1 exchanged.
        {
            P6Result sw;
            if (has("P6-ORDER-SWAP")) sw = analyze_p6(dir + "/prog_p6_mut_swap01.bin");
            const P6Result& go = (has("P6-ORDER-SWAP")) ? sw : base;
            bool cond = go.ok && go.i0 && go.i1 && go.i2 && *go.i0 < *go.i1 && *go.i1 < *go.i2;
            if (p6_pre) checks.expect(cond,
                "P6-ORDER [p.27]: on row r1, reading left to right, a C0 (red) run comes first, then "
                "the C1 (green) run, then a C2 (blue) run");
            else checks.skip("P6-ORDER", "a P6 fixture or precondition failed");
        }
        // Contrast (i): WAIT2 $60A1 -> $60C1. The right end of the C1 run on
        // row r1 (i.e. the start of C2) moves to a larger column, a column
        // being the count of clk_sys samples from that row's hs edge (2.5),
        // taken in each run from its own row r1.
        {
            bool cond = wc1.ok && wc1.col2.has_value() && base.col2.has_value() && *wc1.col2 > *base.col2;
            if (p6_pre && wc1.ok && wc1.col2 && base.col2)
                checks.note("P6-i: the C2 run's start on row r1, in clk_sys samples from the row's hs edge: main run " +
                            std::to_string(*base.col2) + ", second WAIT $60C1 " + std::to_string(*wc1.col2));
            if (p6_pre && wc1.ok) checks.expect(cond,
                "P6-i [p.27]: with the second WAIT changed from $60A1 to $60C1, the C2 run's own "
                "start on row r1, counted in clk_sys samples from that row's hs edge, moves to a larger "
                "column than in the main run");
            else checks.skip("P6-i", "a P6 fixture or precondition failed");
        }
        // Contrast (ii): WAIT3 $A061 -> $B061. r3 increases.
        {
            bool cond = wb0.ok && wb0.r3.has_value() && base.r3.has_value() && *wb0.r3 > *base.r3;
            if (p6_pre && wb0.ok) checks.expect(cond,
                "P6-ii [p.27]: with the third WAIT changed from $A061 to $B061, row r3 (first row "
                "with C3) moves to a larger row index than in the main run");
            else checks.skip("P6-ii", "a P6 fixture or precondition failed");
        }
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
