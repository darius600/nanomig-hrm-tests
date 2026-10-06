// test_aa_integ_s7.cpp -- aa-integ-s7: see SPECIFICATION.md.
//
// Copper-protected, DMA-only and address-pair registers of the AA register list, graded at the ports of
// nanomig.v on both CPU cores with the AA chip set selected (chipset_config = 0x10). One program (prog_run)
// executes a script that the test places in Chip memory; the test reads the RAM port, the refresh output, the
// video and audio samples and the markers the program writes to the word MK.
//
// NMH_MUTATE=<label> runs the proposition that owns the label with one changed stimulus and the unchanged
// comparator; a mutation run of a C proposition runs its first register case only, and a mutation run of F1,
// F2 and F5 keeps the first cases. NMH_ONLY=C1,F3 restricts a run to the named propositions. The normal run
// (NMH_MUTATE unset) never runs a mutant. F4 and F7 are not graded (SPECIFICATION.md 6.4) and have no record.
//
// MUTATION_RECORDS_BEGIN
// MUTATION C1 stimulus red NMH_MUTATE=C1: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; TG68K FAIL, fx68k FAIL.
// MUTATION C1-FIXTURE stimulus red NMH_MUTATE=C1-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; TG68K FAIL, fx68k FAIL.
// MUTATION C1-PC1 stimulus red NMH_MUTATE=C1-PC1: COPEN is cleared (DMACON = $0080) after COP1LC is written and set again after M_end, so the Copper fetches no case list; TG68K FAIL, fx68k FAIL.
// MUTATION C1-PC2 stimulus red NMH_MUTATE=C1-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; TG68K FAIL, fx68k FAIL.
// MUTATION C1-PC3 stimulus red NMH_MUTATE=C1-PC3: every VBL_SYNC has timeout 0, so each wait ends by its timeout; TG68K FAIL, fx68k FAIL. Also printing FAIL: C1-PC1.
// MUTATION C1-PC4 stimulus red NMH_MUTATE=C1-PC4: the calibration does not write V1; TG68K FAIL, fx68k FAIL.
// MUTATION C3 stimulus red NMH_MUTATE=C3: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; TG68K FAIL, fx68k FAIL.
// MUTATION C3-FIXTURE stimulus red NMH_MUTATE=C3-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; TG68K FAIL, fx68k FAIL.
// MUTATION C3-PC1 stimulus red NMH_MUTATE=C3-PC1: COPEN is cleared (DMACON = $0080) after COP1LC is written and set again after M_end, so the Copper fetches no case list; TG68K FAIL, fx68k FAIL.
// MUTATION C3-PC2 stimulus red NMH_MUTATE=C3-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; TG68K FAIL, fx68k FAIL.
// MUTATION C3-PC3 stimulus red NMH_MUTATE=C3-PC3: every VBL_SYNC has timeout 0, so each wait ends by its timeout; TG68K FAIL, fx68k FAIL. Also printing FAIL: C3-PC1.
// MUTATION C3-PC4 stimulus red NMH_MUTATE=C3-PC4: the calibration does not write V1; TG68K FAIL, fx68k FAIL.
// MUTATION C5 stimulus red NMH_MUTATE=C5: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; TG68K FAIL, fx68k FAIL.
// MUTATION C5-FIXTURE stimulus red NMH_MUTATE=C5-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; TG68K FAIL, fx68k FAIL.
// MUTATION C5-PC1 stimulus red NMH_MUTATE=C5-PC1: COPEN is cleared (DMACON = $0080) after COP1LC is written and set again after M_end, so the Copper fetches no case list; TG68K FAIL, fx68k FAIL.
// MUTATION C5-PC2 stimulus red NMH_MUTATE=C5-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; TG68K FAIL, fx68k FAIL.
// MUTATION C5-PC3 stimulus red NMH_MUTATE=C5-PC3: every VBL_SYNC has timeout 0, so each wait ends by its timeout; TG68K FAIL, fx68k FAIL. Also printing FAIL: C5-PC1.
// MUTATION C5-PC4 stimulus red NMH_MUTATE=C5-PC4: the calibration does not write V1; TG68K FAIL, fx68k FAIL.
// MUTATION C4 stimulus red NMH_MUTATE=C4: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; TG68K FAIL, fx68k FAIL.
// MUTATION C4-FIXTURE stimulus red NMH_MUTATE=C4-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; TG68K FAIL, fx68k FAIL.
// MUTATION C4-PC1 stimulus red NMH_MUTATE=C4-PC1: COPEN is cleared (DMACON = $0080) after COP1LC is written and set again after M_end, so the Copper fetches no case list; TG68K FAIL, fx68k FAIL.
// MUTATION C4-PC2 stimulus red NMH_MUTATE=C4-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; TG68K FAIL, fx68k FAIL.
// MUTATION C4-PC3 stimulus red NMH_MUTATE=C4-PC3: every VBL_SYNC has timeout 0, so each wait ends by its timeout; TG68K FAIL, fx68k FAIL. Also printing FAIL: C4-PC1.
// MUTATION C4-PC4 stimulus red NMH_MUTATE=C4-PC4: the calibration does not write VHPOSW; TG68K FAIL, fx68k FAIL.
// MUTATION C2 stimulus red NMH_MUTATE=C2: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; TG68K FAIL, fx68k FAIL.
// MUTATION C2-FIXTURE stimulus red NMH_MUTATE=C2-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; TG68K FAIL, fx68k FAIL.
// MUTATION C2-PC1 stimulus red NMH_MUTATE=C2-PC1: COPEN is cleared (DMACON = $0080) after COP1LC is written and set again after M_end, so the Copper fetches no case list; TG68K FAIL, fx68k FAIL.
// MUTATION C2-PC2 stimulus red NMH_MUTATE=C2-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; TG68K FAIL, fx68k FAIL.
// MUTATION C2-PC3 stimulus red NMH_MUTATE=C2-PC3: every VBL_SYNC has timeout 0, so each wait ends by its timeout; TG68K FAIL, fx68k FAIL. Also printing FAIL: C2-PC1.
// MUTATION C2-PC4 stimulus red NMH_MUTATE=C2-PC4: the calibration does not write V1; TG68K FAIL, fx68k FAIL.
// MUTATION C2-PC5 stimulus red NMH_MUTATE=C2-PC5: DMACON is written without DSKEN; TG68K FAIL, fx68k FAIL. Also printing FAIL: C2-PC4, C2-PC6.
// MUTATION C2-PC6 stimulus red NMH_MUTATE=C2-PC6: DSKLEN's DMAEN write is made once, not twice, so DMAON reads 0; TG68K FAIL, fx68k FAIL. Also printing FAIL: C2-PC4.
// MUTATION C2-PCR stimulus red NMH_MUTATE=C2-PCR: M_end is written before M_wait, so the observation window is empty; TG68K FAIL, fx68k FAIL. Also printing FAIL: C2-PC5.
// MUTATION C6 stimulus red NMH_MUTATE=C6: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; TG68K FAIL, fx68k FAIL.
// MUTATION C6-FIXTURE stimulus red NMH_MUTATE=C6-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; TG68K FAIL, fx68k FAIL.
// MUTATION C6-PC1 stimulus red NMH_MUTATE=C6-PC1: COPEN is cleared (DMACON = $0080) after COP1LC is written and set again after M_end, so the Copper fetches no case list; TG68K FAIL, fx68k FAIL.
// MUTATION C6-PC2 stimulus red NMH_MUTATE=C6-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; TG68K FAIL, fx68k FAIL.
// MUTATION C6-PC3 stimulus red NMH_MUTATE=C6-PC3: every VBL_SYNC has timeout 0, so each wait ends by its timeout; TG68K FAIL, fx68k FAIL. Also printing FAIL: C6-PC1.
// MUTATION C6-PC4 stimulus red NMH_MUTATE=C6-PC4: the calibration does not write V1; TG68K FAIL, fx68k FAIL.
// MUTATION C6-PC7 stimulus red NMH_MUTATE=C6-PC7: one movement of 8 counts on the horizontal axis of port 0 is sent through the mouse inputs between the two reads; TG68K FAIL, fx68k FAIL. Also printing FAIL: C6-PC4.
// MUTATION C7 stimulus red NMH_MUTATE=C7: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; TG68K FAIL, fx68k FAIL.
// MUTATION C7-FIXTURE stimulus red NMH_MUTATE=C7-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; TG68K FAIL, fx68k FAIL.
// MUTATION C7-PC1 stimulus red NMH_MUTATE=C7-PC1: COPEN is cleared (DMACON = $0080) after COP1LC is written and set again after M_end, so the Copper fetches no case list; TG68K FAIL, fx68k FAIL.
// MUTATION C7-PC2 stimulus red NMH_MUTATE=C7-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; TG68K FAIL, fx68k FAIL.
// MUTATION C7-PC3 stimulus red NMH_MUTATE=C7-PC3: every VBL_SYNC has timeout 0, so each wait ends by its timeout; TG68K FAIL, fx68k FAIL. Also printing FAIL: C7-PC1.
// MUTATION C7-PC4 stimulus red NMH_MUTATE=C7-PC4: the calibration does not write V1; TG68K FAIL, fx68k FAIL.
// MUTATION C7-PC5 stimulus red NMH_MUTATE=C7-PC5: DMACON is written without BLTEN; TG68K FAIL, fx68k FAIL. Also printing FAIL: C7-PC2, C7-PC3, C7-PC4, C7-PC6.
// MUTATION C7-PC6 stimulus red NMH_MUTATE=C7-PC6: the observation blit is BLTSIZE $0043 (three words) in place of $0044; TG68K FAIL, fx68k FAIL. Also printing FAIL: C7-PC4.
// MUTATION C8 stimulus red NMH_MUTATE=C8: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; TG68K FAIL, fx68k FAIL.
// MUTATION C8-FIXTURE stimulus red NMH_MUTATE=C8-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; TG68K FAIL, fx68k FAIL.
// MUTATION C8-PC1 stimulus red NMH_MUTATE=C8-PC1: COPEN is cleared (DMACON = $0080) after COP1LC is written and set again after M_end, so the Copper fetches no case list; TG68K FAIL, fx68k FAIL.
// MUTATION C8-PC2 stimulus red NMH_MUTATE=C8-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; TG68K FAIL, fx68k FAIL.
// MUTATION C8-PC3 stimulus red NMH_MUTATE=C8-PC3: every VBL_SYNC has timeout 0, so each wait ends by its timeout; TG68K FAIL, fx68k FAIL. Also printing FAIL: C8-PC1.
// MUTATION C8-PC4 stimulus red NMH_MUTATE=C8-PC4: the calibration does not write V1; TG68K FAIL, fx68k FAIL.
// MUTATION C8-PC5 stimulus red NMH_MUTATE=C8-PC5: DMACON is written without BLTEN; TG68K FAIL, fx68k FAIL. Also printing FAIL: C8-PC2, C8-PC3, C8-PC4, C8-PC6.
// MUTATION C8-PC6 stimulus red NMH_MUTATE=C8-PC6: the observation blit is BLTSIZE $0043 (three words) in place of $0042; TG68K FAIL, fx68k FAIL. Also printing FAIL: C8-PC4.
// MUTATION C9 stimulus red NMH_MUTATE=C9: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; TG68K FAIL, fx68k FAIL.
// MUTATION C9-FIXTURE stimulus red NMH_MUTATE=C9-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; TG68K FAIL, fx68k FAIL.
// MUTATION C9-PC1 stimulus red NMH_MUTATE=C9-PC1: COPEN is cleared (DMACON = $0080) after COP1LC is written and set again after M_end, so the Copper fetches no case list; TG68K FAIL, fx68k FAIL.
// MUTATION C9-PC2 stimulus red NMH_MUTATE=C9-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; TG68K FAIL, fx68k FAIL.
// MUTATION C9-PC3 stimulus red NMH_MUTATE=C9-PC3: every VBL_SYNC has timeout 0, so each wait ends by its timeout; TG68K FAIL, fx68k FAIL. Also printing FAIL: C9-PC1.
// MUTATION C9-PC4 stimulus red NMH_MUTATE=C9-PC4: the calibration does not write V1; TG68K FAIL, fx68k FAIL.
// MUTATION C9-PC5 stimulus red NMH_MUTATE=C9-PC5: DMACON is written without BLTEN; TG68K FAIL, fx68k FAIL. Also printing FAIL: C9-PC2, C9-PC3, C9-PC4, C9-PC6.
// MUTATION C9-PC6 stimulus red NMH_MUTATE=C9-PC6: the observation blit is BLTSIZE $0044 (four words) in place of $0043; TG68K FAIL, fx68k FAIL. Also printing FAIL: C9-PC4.
// MUTATION C10 stimulus red NMH_MUTATE=C10: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; TG68K FAIL, fx68k FAIL.
// MUTATION C10-FIXTURE stimulus red NMH_MUTATE=C10-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; TG68K FAIL, fx68k FAIL.
// MUTATION C10-PC1 stimulus red NMH_MUTATE=C10-PC1: COPEN is cleared (DMACON = $0080) after COP1LC is written and set again after M_end, so the Copper fetches no case list; TG68K FAIL, fx68k FAIL.
// MUTATION C10-PC2 stimulus red NMH_MUTATE=C10-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; TG68K FAIL, fx68k FAIL.
// MUTATION C10-PC3 stimulus red NMH_MUTATE=C10-PC3: every VBL_SYNC has timeout 0, so each wait ends by its timeout; TG68K FAIL, fx68k FAIL. Also printing FAIL: C10-PC1.
// MUTATION C10-PC4 stimulus red NMH_MUTATE=C10-PC4: the calibration does not write V1; TG68K FAIL, fx68k FAIL.
// MUTATION C10-PC5 stimulus red NMH_MUTATE=C10-PC5: DMACON is written without BLTEN; TG68K FAIL, fx68k FAIL. Also printing FAIL: C10-PC2, C10-PC3, C10-PC4.
// MUTATION C11 stimulus red NMH_MUTATE=C11: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; TG68K FAIL, fx68k FAIL.
// MUTATION C11-FIXTURE stimulus red NMH_MUTATE=C11-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; TG68K FAIL, fx68k FAIL.
// MUTATION C11-PC1 stimulus red NMH_MUTATE=C11-PC1: COPEN is cleared (DMACON = $0080) after COP1LC is written and set again after M_end, so the Copper fetches no case list; TG68K FAIL, fx68k FAIL.
// MUTATION C11-PC2 stimulus red NMH_MUTATE=C11-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; TG68K FAIL, fx68k FAIL.
// MUTATION C11-PC3 stimulus red NMH_MUTATE=C11-PC3: every VBL_SYNC has timeout 0, so each wait ends by its timeout; TG68K FAIL, fx68k FAIL. Also printing FAIL: C11-PC1.
// MUTATION C11-PC4 stimulus red NMH_MUTATE=C11-PC4: the calibration does not write V1; TG68K FAIL, fx68k FAIL.
// MUTATION C11-PC5 stimulus red NMH_MUTATE=C11-PC5: DMACON is written without BLTEN; TG68K FAIL, fx68k FAIL. Also printing FAIL: C11-PC2, C11-PC3, C11-PC4.
// MUTATION C12 stimulus red NMH_MUTATE=C12: in the frame B of COPCON $0000 the program preloads $5A5A into PX, so the DMA channel loads V1 as the register's value; TG68K FAIL, fx68k FAIL.
// MUTATION C12-FIXTURE stimulus red NMH_MUTATE=C12-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; TG68K FAIL, fx68k FAIL.
// MUTATION C12-PC1 stimulus red NMH_MUTATE=C12-PC1: COPEN is cleared (DMACON = $0080) after COP1LC is written and set again after M_end, so the Copper fetches no case list; TG68K FAIL, fx68k FAIL.
// MUTATION C12-PC2 stimulus red NMH_MUTATE=C12-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; TG68K FAIL, fx68k FAIL.
// MUTATION C12-PC3 stimulus red NMH_MUTATE=C12-PC3: every VBL_SYNC has timeout 0, so each wait ends by its timeout; TG68K FAIL, fx68k FAIL. Also printing FAIL: C12-PC1.
// MUTATION C12-PC4 stimulus red NMH_MUTATE=C12-PC4: frame A does not write V1 into PX, so the preload of V1 loads V0 again; TG68K FAIL, fx68k FAIL. Also printing FAIL: C12-PC5.
// MUTATION C12-PC5 stimulus red NMH_MUTATE=C12-PC5: DMACON is written without BLTEN; TG68K FAIL, fx68k FAIL. Also printing FAIL: C12-FIXTURE, C12-PC1, C12-PC2, C12-PC3, C12-PC4.
// MUTATION F1 stimulus red NMH_MUTATE=F1: the last case writes the size (6,3) while (5,3) is expected; TG68K FAIL, fx68k FAIL.
// MUTATION F1-FIXTURE stimulus red NMH_MUTATE=F1-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; TG68K FAIL, fx68k FAIL.
// MUTATION F1-DMA stimulus red NMH_MUTATE=F1-DMA: DMACON is written without BLTEN; TG68K FAIL, fx68k FAIL. Also printing FAIL: F1-TMO.
// MUTATION F1-TMO stimulus red NMH_MUTATE=F1-TMO: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first case; TG68K FAIL, fx68k FAIL.
// MUTATION F1-IDLE stimulus red NMH_MUTATE=F1-IDLE: a blit of one word is started after the last case and not awaited; TG68K FAIL, fx68k FAIL.
// MUTATION F2 stimulus red NMH_MUTATE=F2: the case with v = 8 writes v = 9; TG68K FAIL, fx68k FAIL.
// MUTATION F2-FIXTURE stimulus red NMH_MUTATE=F2-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; TG68K FAIL, fx68k FAIL.
// MUTATION F2-DMA stimulus red NMH_MUTATE=F2-DMA: DMACON is written without BLTEN; TG68K FAIL, fx68k FAIL. Also printing FAIL: F2-TMO.
// MUTATION F2-TMO stimulus red NMH_MUTATE=F2-TMO: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first case; TG68K FAIL, fx68k FAIL.
// MUTATION F2-IDLE stimulus red NMH_MUTATE=F2-IDLE: a blit is started with BLTSIZH after the last case and not awaited; TG68K FAIL, fx68k FAIL.
// MUTATION F3 stimulus red NMH_MUTATE=F3: the full BLTCON0 is written with $FF0F in place of BLTCON0L; TG68K FAIL, fx68k FAIL.
// MUTATION F3-FIXTURE stimulus red NMH_MUTATE=F3-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; TG68K FAIL, fx68k FAIL.
// MUTATION F3-DMA stimulus red NMH_MUTATE=F3-DMA: DMACON is written without BLTEN; TG68K FAIL, fx68k FAIL. Also printing FAIL: F3-TMO.
// MUTATION F3-TMO stimulus red NMH_MUTATE=F3-TMO: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first case; TG68K FAIL, fx68k FAIL.
// MUTATION F3-IDLE stimulus red NMH_MUTATE=F3-IDLE: a second blit of two words is started after the window and not awaited; TG68K FAIL, fx68k FAIL.
// MUTATION F5 stimulus red NMH_MUTATE=F5: SPR3PTL of the family S1 is written with bit 8 flipped; TG68K FAIL, fx68k FAIL.
// MUTATION F5-FIXTURE stimulus red NMH_MUTATE=F5-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; TG68K FAIL, fx68k FAIL.
// MUTATION F5-DMA stimulus red NMH_MUTATE=F5-DMA: DMACON is written without SPREN; TG68K FAIL, fx68k FAIL. Also printing FAIL: F5-POP.
// MUTATION F5-QUIET stimulus red NMH_MUTATE=F5-QUIET: the program waits 60 lines after the pointer writes of family S1 before M1, so sprite reads fall between M0 and M1; TG68K FAIL, fx68k FAIL. Also printing FAIL: F5-POP.
// MUTATION F5-POP stimulus red NMH_MUTATE=F5-POP: the second VBL_SYNC of family S1 is three lines, so fewer than eight reads fall between M1 and M2; TG68K FAIL, fx68k FAIL.
// MUTATION F5-REF stimulus red NMH_MUTATE=F5-REF: M2 is written before M1 in family S1, so the window is empty; TG68K FAIL, fx68k FAIL. Also printing FAIL: F5-POP.
// MUTATION F5-TMO stimulus red NMH_MUTATE=F5-TMO: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first case; TG68K FAIL, fx68k FAIL.
// MUTATION F6 stimulus red NMH_MUTATE=F6: the band rows of planes 3 and 4 are swapped in memory; TG68K FAIL, fx68k FAIL.
// MUTATION F6-FIXTURE stimulus red NMH_MUTATE=F6-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; TG68K FAIL, fx68k FAIL.
// MUTATION F6-DMA stimulus red NMH_MUTATE=F6-DMA: DMACON is written without BPLEN; TG68K FAIL, fx68k FAIL. Also printing FAIL: F6-READS.
// MUTATION F6-FRAMES stimulus red NMH_MUTATE=F6-FRAMES: one VBL_SYNC in place of four between M_ready and M_done, so fewer than three complete frames lie in the interval; TG68K FAIL, fx68k FAIL. Also printing FAIL: F6-COPPER, F6-READS.
// MUTATION F6-READS stimulus red NMH_MUTATE=F6-READS: BPLCON0 is $7000 (BPU = 7), so plane 8 is not fetched; TG68K FAIL, fx68k FAIL.
// MUTATION F6-COPPER stimulus red NMH_MUTATE=F6-COPPER: COP1LC is not written, so the list is not read; TG68K FAIL, fx68k FAIL. Also printing FAIL: F6-READS.
// MUTATION F6-TMO stimulus red NMH_MUTATE=F6-TMO: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first case; TG68K FAIL, fx68k FAIL.
// MUTATION F8 stimulus red NMH_MUTATE=F8: the third source word of each source under test is $0000; TG68K FAIL, fx68k FAIL.
// MUTATION F8-FIXTURE stimulus red NMH_MUTATE=F8-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; TG68K FAIL, fx68k FAIL.
// MUTATION F8-DMA stimulus red NMH_MUTATE=F8-DMA: DMACON is written without BLTEN; TG68K FAIL, fx68k FAIL. Also printing FAIL: F8-READS, F8-TMO.
// MUTATION F8-TMO stimulus red NMH_MUTATE=F8-TMO: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first case; TG68K FAIL, fx68k FAIL.
// MUTATION F8-READS stimulus red NMH_MUTATE=F8-READS: USEx is cleared in BLTCON0, so the blit makes no read of SRC_x; TG68K FAIL, fx68k FAIL.
// MUTATION F8-AFTER stimulus red NMH_MUTATE=F8-AFTER: a second blit is started after M_end of the last case and not awaited; TG68K FAIL, fx68k FAIL.
// MUTATION F9 stimulus red NMH_MUTATE=F9: DSKLEN is $C003 in place of $C004 (both writes); TG68K FAIL, fx68k FAIL.
// MUTATION F9-FIXTURE stimulus red NMH_MUTATE=F9-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; TG68K FAIL, fx68k FAIL.
// MUTATION F9-DMA stimulus red NMH_MUTATE=F9-DMA: DMACON is written without DSKEN; TG68K FAIL, fx68k FAIL. Also printing FAIL: F9-DMAON.
// MUTATION F9-DMAON stimulus red NMH_MUTATE=F9-DMAON: DSKLEN's DMAEN write is made once, not twice, so DMAON reads 0; TG68K FAIL, fx68k FAIL.
// MUTATION F9-REF stimulus red NMH_MUTATE=F9-REF: M_end is written before M_start, so the window is empty; TG68K FAIL, fx68k FAIL.
// MUTATION F9-TMO stimulus red NMH_MUTATE=F9-TMO: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first case; TG68K FAIL, fx68k FAIL.
// MUTATION_RECORDS_END

#include "s7_f6.h"

using namespace s7;

namespace {

const char* kP6 = "AA p.6 '~=Address not writable by the Coprocessor unless COPCON bit 1 is set true.'; AA p.19 COPCON bit 01 CDANG '(if 0, access to RGA>7E)'";

// The proposition a selector belongs to ("C7-PC1" -> "C7"; "FIXTURE" belongs to every one).
std::string sel_prop() {
    if (!g_mutate) return "";
    std::string s = g_mutate;
    size_t i = s.find('-');
    return i == std::string::npos ? s : s.substr(0, i);
}

void do_c(Checks& ck, const Prop& p, const std::string& claims, const std::string& what, const std::string& pc5, const std::string& pc6 = "") {
    CRun r = run_c(p, g_mutate ? g_mutate : "");
    CEval e = eval_c(ck, p, r);
    print_c(ck, p, e, "(" + std::string(kP6) + ") claims " + claims, what, pc5, pc6);
}

} // namespace

int main(int argc, char** argv) {
    g_dir = argc > 1 ? argv[1] : ".";
    g_mutate = std::getenv("NMH_MUTATE");
    const char* only = std::getenv("NMH_ONLY");   // NMH_ONLY=C1,C3 restricts a run to the named propositions
    std::string core = NMH_XSTR(NANOMIG_CORE_NAME);
    std::printf("CORE: %s\n", core.c_str());
    if (g_mutate) std::printf("NMH_MUTATE=%s\n", g_mutate);
    Checks ck;
    const std::string sp = sel_prop();
    auto want = [&](const char* p) {
        if (only) {
            std::string l = std::string(",") + only + ",";
            if (l.find(std::string(",") + p + ",") == std::string::npos) return false;
        }
        return sp.empty() || sp == "FIXTURE" || sp == p;
    };
    const std::string pc5blt = "DMACONR reads DMAEN and BLTEN set, and no blitter access lies outside the windows of the cases, so every blit finished inside its case (AA p.22)";
    if (want("C1")) do_c(ck, prop_c1(), "opus:AA-dsklen.c4 sol:AA-dsklen.c4",
        "a Copper MOVE to DSKLEN ($4000) changes DSKBYTR bit 13, the mirror of DSKLEN bit 14 WRITE, only while COPCON bit 1 is set: 0 with COPCON $0000 and $FFFD, 1 with $0002 (AA p.23)", "");
    if (want("C3")) do_c(ck, prop_c3(), "opus:AA-vposw.c4 sol:AA-vposw.c4",
        "a Copper MOVE to VPOSW ($8000) changes VPOSR bit 15, the frame flop LOF, only while COPCON bit 1 is set: 0 with COPCON $0000 and $FFFD, 1 with $0002 (AA p.29)", "");
    if (want("C5")) do_c(ck, prop_c5(), "opus:AA-potgo.c4 sol:AA-potgo.c4",
        "a Copper MOVE to POTGO ($AA00) changes the pot-pin drive read in POTINP & $5500 only while COPCON bit 1 is set: $5500 with COPCON $0000 and $FFFD, $0000 with $0002 (AA p.26-27)", "");
    if (want("C7")) do_c(ck, prop_c7(), "opus:AA-bltcon0.c4 sol:AA-bltcon0.c4 opus:AA-bltcon0l.c5 sol:AA-bltcon0l.c5",
        "a Copper MOVE to BLTCON0 ($01FF) or to BLTCON0L ($00FF) changes the logic function of the next blit only while COPCON bit 1 is set: four writes of $0000 with COPCON $0000 and $FFFD, four of $FFFF with $0002 (AA p.14)", pc5blt,
        "every observed blit made exactly four blitter writes");
    if (want("C8")) do_c(ck, prop_c8(), "opus:AA-bltcon1.c4 sol:AA-bltcon1.c4",
        "a Copper MOVE to BLTCON1 ($0002, DESC) changes the address direction of the next blit only while COPCON bit 1 is set: writes at DST, DST+2 with COPCON $0000 and $FFFD, at DST, DST-2 with $0002 (AA p.14)", pc5blt,
        "every observed blit made exactly two blitter writes");
    if (want("C9")) do_c(ck, prop_c9(), "opus:AA-bltafwm.c4 sol:AA-bltafwm.c4 opus:AA-bltalwm.c4 sol:AA-bltalwm.c4",
        "a Copper MOVE of $0F0F to BLTAFWM or to BLTALWM changes the first-word or last-word mask of the next blit only while COPCON bit 1 is set (AA p.13)", pc5blt,
        "every observed blit made exactly three blitter reads of ASRC and three blitter writes");
    if (want("C10")) do_c(ck, prop_c10(), "opus:AA-bltsize.c4 sol:AA-bltsize.c4 opus:AA-bltsizh.c5 sol:AA-bltsizh.c5",
        "a Copper MOVE to BLTSIZE ($0044) or to BLTSIZH ($0004) starts the blitter only while COPCON bit 1 is set: no blitter write with COPCON $0000 and $FFFD, four writes of $FFFF with $0002 (AA p.14)", pc5blt);
    if (want("C11")) do_c(ck, prop_c11(), "opus:AA-bltsizv.c5 sol:AA-bltsizv.c5",
        "a Copper MOVE of 5 to BLTSIZV changes the height of the next blit only while COPCON bit 1 is set: 2 writes with COPCON $0000 and $FFFD, 5 with $0002 (AA p.14)", pc5blt);
    if (want("C2")) do_c(ck, prop_c2(), "opus:AA-dskpth.c5 sol:AA-dskpth.c5 opus:AA-dskptl.c5 sol:AA-dskptl.c5",
        "a Copper MOVE to DSKPTH or to DSKPTL changes the address from which disk DMA reads only while COPCON bit 1 is set: the first disk read is at Rd with COPCON $0000 and $FFFD and at Re (DSKPTH) or Re' (DSKPTL) with $0002, and disk DMA with DMAON set reads RAM in every run (AA p.23 'This pair of registers contains the 20 bit address of Disk DMA data.')",
        "DMACONR reads DMAEN and DSKEN set, and no disk read lies outside the windows of the cases, so every disk DMA finished inside its case (AA p.22, p.23)",
        "DSKBYTR bit 14 DMAON reads 1 in every run of the disk DMA sequence (AA p.23 'DMAEN(DSKLEN) & DMAEN(DMACON) & DSKEN(DMACON)')");
    if (want("C4")) do_c(ck, prop_c4(), "opus:AA-vhposw.c4 sol:AA-vhposw.c4",
        "a Copper MOVE to VHPOSW ($C000) moves the beam counter only while COPCON bit 1 is set: VHPOSR bits 15-8 read $41 or $42 at the first poll past $40 with COPCON $0000 and $FFFD, $C0 or $C1 with $0002 (AA p.29)", "");
    if (want("C6")) do_c(ck, prop_c6(), "opus:AA-joytest.c4 sol:AA-joytest.c4",
        "a Copper MOVE to JOYTEST ($FCFC) changes the mouse counters of JOY0DAT and JOY1DAT (read & $FCFC) only while COPCON bit 1 is set: $0000 with COPCON $0000 and $FFFD, $FCFC with $0002 (AA p.26)", "",
        "the two reads of each of JOY0DAT and JOY1DAT in a case are equal, so nothing but the write moved the counters");
    if (want("C12")) { C12Run r = run_c12(g_mutate ? g_mutate : ""); eval_print_c12(ck, r, ""); }
    const std::string sel = g_mutate ? g_mutate : "";
    if (want("F1")) run_f1_f2(ck, false, sel);
    if (want("F2")) run_f1_f2(ck, true, sel);
    if (want("F3")) run_f3(ck, sel);
    if (want("F4")) run_f4(ck);
    if (want("F5")) run_f5(ck, sel);
    if (want("F6")) run_f6(ck, sel);
    if (want("F7")) run_f7(ck);
    if (want("F8")) run_f8(ck, sel);
    if (want("F9")) run_f9(ck, sel);
    if (!g_mutate && !only) {
        // claims the package declines (SPECIFICATION.md section 0)
        ck.record("DECLINED the ~ on a read address or a dummy address (SPECIFICATION.md 0.1): no page states an effect of a write to it, by the CPU or by the Copper, so COPCON bit 1 set and clear predict the same outcome at every output: opus:AA-adkconr.c4 sol:AA-adkconr.c4 opus:AA-dskbytr.c4 sol:AA-dskbytr.c4 opus:AA-intenar.c4 sol:AA-intenar.c4 opus:AA-intreqr.c4 sol:AA-intreqr.c4 opus:AA-joy0dat.c4 sol:AA-joy0dat.c4 opus:AA-joy1dat.c4 sol:AA-joy1dat.c4 opus:AA-pot0dat.c4 sol:AA-pot0dat.c4 opus:AA-pot1dat.c4 sol:AA-pot1dat.c4 opus:AA-potinp.c4 sol:AA-potinp.c4 opus:AA-vhposr.c4 sol:AA-vhposr.c4 opus:AA-vposr.c4 sol:AA-vposr.c4 opus:AA-lisaid.c5 sol:AA-lisaid.c5 opus:AA-dskdatr.c5 sol:AA-dskdatr.c5 opus:AA-bltddat.c5 sol:AA-bltddat.c5");
        ck.record("DECLINED the disk-side effects (SPECIFICATION.md 0.2): each needs data read from or written to a disk surface, and the harness serves no disk image: opus:AA-dskdat.c5 sol:AA-dskdat.c5 opus:AA-dsksync.c4 sol:AA-dsksync.c4 opus:AA-dskdatr.c4 sol:AA-dskdatr.c4 opus:AA-dskdatr.c2 sol:AA-dskdatr.c2");
        ck.record("DECLINED the ~ of BLTADAT, BLTBDAT and BLTCDAT (SPECIFICATION.md 0.3, D1): with COPCON bit 1 set the pages disagree on what a Copper write does; C12 grades the bit-1-clear half and carries no claim id: opus:AA-bltadat.c5 sol:AA-bltadat.c5 opus:AA-bltbdat.c5 sol:AA-bltbdat.c5 opus:AA-bltcdat.c5 sol:AA-bltcdat.c5");
        ck.record("DECLINED the & of BPL2DAT-BPL8DAT (SPECIFICATION.md 0.3, D1): 'only' is contradicted by They may also be written by either micro; F6 grades the DMA part and carries no claim id: opus:AA-bpl2dat.c4 sol:AA-bpl2dat.c4 opus:AA-bpl3dat.c4 sol:AA-bpl3dat.c4 opus:AA-bpl4dat.c4 sol:AA-bpl4dat.c4 opus:AA-bpl5dat.c4 sol:AA-bpl5dat.c4 opus:AA-bpl6dat.c4 sol:AA-bpl6dat.c4 opus:AA-bpl7dat.c4 sol:AA-bpl7dat.c4 opus:AA-bpl8dat.c4 sol:AA-bpl8dat.c4");
        ck.record("DECLINED the & of AUD0DAT-AUD3DAT (SPECIFICATION.md 0.3, D1): 'only' is contradicted by The Processor can also write directly to this register; F7 grades the DMA part through the W claims: opus:AA-aud0dat.c4 sol:AA-aud0dat.c4 opus:AA-aud1dat.c4 sol:AA-aud1dat.c4 opus:AA-aud2dat.c4 sol:AA-aud2dat.c4 opus:AA-aud3dat.c4 sol:AA-aud3dat.c4");
        ck.record("DECLINED the & of BLTADAT, BLTBDAT, BLTCDAT, BLTDDAT and DSKDAT (SPECIFICATION.md 0.3, D1): 'only' is contradicted by it may also be preloaded by the microprocessor or is not observable; F8 and F9 grade the DMA part and carry no claim id: opus:AA-bltadat.c4 sol:AA-bltadat.c4 opus:AA-bltbdat.c4 sol:AA-bltbdat.c4 opus:AA-bltcdat.c4 sol:AA-bltcdat.c4 opus:AA-bltddat.c4 sol:AA-bltddat.c4 opus:AA-dskdat.c4 sol:AA-dskdat.c4");
    }
    std::printf("%d passed, %d failed\n", ck.passed, ck.failed);
    return ck.failed == 0 ? 0 : 1;
}
