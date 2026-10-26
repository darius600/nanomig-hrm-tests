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
// (NMH_MUTATE unset) never runs a mutant. F4 and F7 are graded in the form that holds under both readings of the
// thrown-away first audio word (SPECIFICATION.md 3 and 4).
//
// The refresh precondition (labels C2-PCR, F4-REF, F5-REF, F7-REF, F9-REF): a read is a refresh read when the refresh output is
// high in the cycle of its log entry; the window holds at least 4 x (k - 1) of them, k being the line starts of the LINES(k) wait
// inside the window. That the four reads of a line form one group, and that no line holds more than four, is unfalsifiable at this
// boundary, because no stimulus changes how many refresh reads a line holds. The records of the precondition shorten the window.
//
// The final-WAIT assertion (C1-FW ... C12-FW, F5-FW, F9-FW) with its precondition (-FWA): SPECIFICATION.md 1.10. C1-C12 and F5 are
// red in the normal run. The records of a C proposition and of F5 print the line of the proposition's -FW assertion as FAIL;
// the lists below name the lines other than that one.
//
// MUTATION records of the form 'MUTATION <label> stimulus red': the second token is the label of the line the contrast moves.
//
// MUTATION_RECORDS_BEGIN
// MUTATION C1 stimulus red NMH_MUTATE=C1: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; fx68k and TG68K: FAIL C1.
// MUTATION C1-FIXTURE stimulus red NMH_MUTATE=C1-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL C1-FIXTURE; NOT GRADED: C1-FW, C1.
// MUTATION C1-PC1 stimulus red NMH_MUTATE=C1-PC1: the strobe COPJMP1 is written after M_wait in place of before M_start, so the Copper reads the MOVE after the window of PC1 and still reads the case list to its end; fx68k and TG68K: FAIL C1-PC1; NOT GRADED: C1.
// MUTATION C1-PC2 stimulus red NMH_MUTATE=C1-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; fx68k and TG68K: FAIL C1-PC2; NOT GRADED: C1.
// MUTATION C1-PC3 stimulus red NMH_MUTATE=C1-PC3: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first frame A; fx68k and TG68K: FAIL C1-PC3; NOT GRADED: C1.
// MUTATION C1-PC4 stimulus red NMH_MUTATE=C1-PC4: the calibration does not write V1; fx68k and TG68K: FAIL C1-PC4; NOT GRADED: C1.
// MUTATION C3 stimulus red NMH_MUTATE=C3: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; fx68k and TG68K: FAIL C3.
// MUTATION C3-FIXTURE stimulus red NMH_MUTATE=C3-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL C3-FIXTURE; NOT GRADED: C3-FW, C3.
// MUTATION C3-PC1 stimulus red NMH_MUTATE=C3-PC1: the strobe COPJMP1 is written after M_wait in place of before M_start, so the Copper reads the MOVE after the window of PC1 and still reads the case list to its end; fx68k and TG68K: FAIL C3-PC1; NOT GRADED: C3.
// MUTATION C3-PC2 stimulus red NMH_MUTATE=C3-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; fx68k and TG68K: FAIL C3-PC2; NOT GRADED: C3.
// MUTATION C3-PC3 stimulus red NMH_MUTATE=C3-PC3: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first frame A; fx68k and TG68K: FAIL C3-PC3; NOT GRADED: C3.
// MUTATION C3-PC4 stimulus red NMH_MUTATE=C3-PC4: the calibration does not write V1; fx68k and TG68K: FAIL C3-PC4; NOT GRADED: C3.
// MUTATION C5 stimulus red NMH_MUTATE=C5: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; fx68k and TG68K: FAIL C5.
// MUTATION C5-FIXTURE stimulus red NMH_MUTATE=C5-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL C5-FIXTURE; NOT GRADED: C5-FW, C5.
// MUTATION C5-PC1 stimulus red NMH_MUTATE=C5-PC1: the strobe COPJMP1 is written after M_wait in place of before M_start, so the Copper reads the MOVE after the window of PC1 and still reads the case list to its end; fx68k and TG68K: FAIL C5-PC1; NOT GRADED: C5.
// MUTATION C5-PC2 stimulus red NMH_MUTATE=C5-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; fx68k and TG68K: FAIL C5-PC2; NOT GRADED: C5.
// MUTATION C5-PC3 stimulus red NMH_MUTATE=C5-PC3: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first frame A; fx68k and TG68K: FAIL C5-PC3; NOT GRADED: C5.
// MUTATION C5-PC4 stimulus red NMH_MUTATE=C5-PC4: the calibration does not write V1; fx68k and TG68K: FAIL C5-PC4; NOT GRADED: C5.
// MUTATION C4 stimulus red NMH_MUTATE=C4: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; fx68k and TG68K: FAIL C4.
// MUTATION C4-FIXTURE stimulus red NMH_MUTATE=C4-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL C4-FIXTURE; NOT GRADED: C4-FW, C4.
// MUTATION C4-PC1 stimulus red NMH_MUTATE=C4-PC1: COPEN is cleared (DMACON = $0080) after COP1LC is written and set again after M_end, so the Copper fetches no case list; fx68k and TG68K: FAIL C4-PC1; also FAIL: C4-FWA; NOT GRADED: C4-FW, C4.
// MUTATION C4-PC2 stimulus red NMH_MUTATE=C4-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; fx68k and TG68K: FAIL C4-PC2; NOT GRADED: C4.
// MUTATION C4-PC3 stimulus red NMH_MUTATE=C4-PC3: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first frame A; fx68k and TG68K: FAIL C4-PC3; NOT GRADED: C4.
// MUTATION C4-PC4 stimulus red NMH_MUTATE=C4-PC4: the calibration does not write VHPOSW; fx68k and TG68K: FAIL C4-PC4; NOT GRADED: C4.
// MUTATION C2 stimulus red NMH_MUTATE=C2: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; fx68k and TG68K: FAIL C2.
// MUTATION C2-FIXTURE stimulus red NMH_MUTATE=C2-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL C2-FIXTURE; NOT GRADED: C2-FW, C2.
// MUTATION C2-PC1 stimulus red NMH_MUTATE=C2-PC1: the strobe COPJMP1 is written after M_wait in place of before M_start, so the Copper reads the MOVE after the window of PC1 and still reads the case list to its end; fx68k and TG68K: FAIL C2-PC1; NOT GRADED: C2.
// MUTATION C2-PC2 stimulus red NMH_MUTATE=C2-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; fx68k and TG68K: FAIL C2-PC2; NOT GRADED: C2.
// MUTATION C2-PC3 stimulus red NMH_MUTATE=C2-PC3: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first frame A; fx68k and TG68K: FAIL C2-PC3; NOT GRADED: C2.
// MUTATION C2-PC4 stimulus red NMH_MUTATE=C2-PC4: the calibration does not write V1; fx68k and TG68K: FAIL C2-PC4; NOT GRADED: C2.
// MUTATION C2-PC5 stimulus red NMH_MUTATE=C2-PC5: DMACONR is read after DMACON = $8200 (without DSKEN) and DMACON = $8210 is written after the read, so the DMA_CHECK reads DSKEN clear and the disk DMA of the run is the normal one; fx68k and TG68K: FAIL C2-PC5; NOT GRADED: C2.
// MUTATION C2-PC6 stimulus red NMH_MUTATE=C2-PC6: in frame B only, DSKLEN's DMAEN write is made once, not twice, so DMAON reads 0 there (HRM p.247 'the DMAEN bit in the DSKLEN register must be turned on twice in order to actually enable the disk DMA hardware'; HRM p.248 DMAON 'Indicates when DMA is actually enabled'); fx68k and TG68K: FAIL C2-PC6; NOT GRADED: C2.
// MUTATION C2-PCR stimulus red NMH_MUTATE=C2-PCR: in frame B the LINES(8) wait that ends the step 8 sequence is left out, so every window of frame B holds fewer than 28 refresh reads and still holds the whole disk DMA; fx68k and TG68K: FAIL C2-PCR; NOT GRADED: C2.
// MUTATION C6 stimulus red NMH_MUTATE=C6: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; fx68k and TG68K: FAIL C6.
// MUTATION C6-FIXTURE stimulus red NMH_MUTATE=C6-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL C6-FIXTURE; NOT GRADED: C6-FW, C6.
// MUTATION C6-PC1 stimulus red NMH_MUTATE=C6-PC1: the strobe COPJMP1 is written after M_wait in place of before M_start, so the Copper reads the MOVE after the window of PC1 and still reads the case list to its end; fx68k and TG68K: FAIL C6-PC1; NOT GRADED: C6.
// MUTATION C6-PC2 stimulus red NMH_MUTATE=C6-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; fx68k and TG68K: FAIL C6-PC2; NOT GRADED: C6.
// MUTATION C6-PC3 stimulus red NMH_MUTATE=C6-PC3: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first frame A; fx68k and TG68K: FAIL C6-PC3; NOT GRADED: C6.
// MUTATION C6-PC4 stimulus red NMH_MUTATE=C6-PC4: the calibration does not write V1; fx68k and TG68K: FAIL C6-PC4; NOT GRADED: C6.
// MUTATION C6-PC7 stimulus red NMH_MUTATE=C6-PC7: in frame B only, one movement of 8 counts on the horizontal axis of port 0 is sent through the mouse inputs between the two reads of each case; fx68k and TG68K: FAIL C6-PC7; NOT GRADED: C6.
// MUTATION C7 stimulus red NMH_MUTATE=C7: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; fx68k and TG68K: FAIL C7.
// MUTATION C7-FIXTURE stimulus red NMH_MUTATE=C7-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL C7-FIXTURE; NOT GRADED: C7-FW, C7.
// MUTATION C7-PC1 stimulus red NMH_MUTATE=C7-PC1: the strobe COPJMP1 is written after M_wait in place of before M_start, so the Copper reads the MOVE after the window of PC1 and still reads the case list to its end; fx68k and TG68K: FAIL C7-PC1; NOT GRADED: C7.
// MUTATION C7-PC2 stimulus red NMH_MUTATE=C7-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; fx68k and TG68K: FAIL C7-PC2; NOT GRADED: C7.
// MUTATION C7-PC3 stimulus red NMH_MUTATE=C7-PC3: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first frame A; fx68k and TG68K: FAIL C7-PC3; NOT GRADED: C7.
// MUTATION C7-PC4 stimulus red NMH_MUTATE=C7-PC4: the calibration does not write V1; fx68k and TG68K: FAIL C7-PC4; NOT GRADED: C7.
// MUTATION C7-PC5 stimulus red NMH_MUTATE=C7-PC5: DMACONR is read after DMACON = $8200 (without BLTEN) and DMACON = $8240 is written after the read, so the DMA_CHECK reads BLTEN clear and the run is otherwise the normal run; fx68k and TG68K: FAIL C7-PC5; NOT GRADED: C7.
// MUTATION C7-PC6 stimulus red NMH_MUTATE=C7-PC6: in frame B only, the observation blit is BLTSIZE $0043 (three words) in place of $0044; fx68k and TG68K: FAIL C7-PC6; NOT GRADED: C7.
// MUTATION C8 stimulus red NMH_MUTATE=C8: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; fx68k and TG68K: FAIL C8.
// MUTATION C8-FIXTURE stimulus red NMH_MUTATE=C8-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL C8-FIXTURE; NOT GRADED: C8-FW, C8.
// MUTATION C8-PC1 stimulus red NMH_MUTATE=C8-PC1: the strobe COPJMP1 is written after M_wait in place of before M_start, so the Copper reads the MOVE after the window of PC1 and still reads the case list to its end; fx68k and TG68K: FAIL C8-PC1; NOT GRADED: C8.
// MUTATION C8-PC2 stimulus red NMH_MUTATE=C8-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; fx68k and TG68K: FAIL C8-PC2; NOT GRADED: C8.
// MUTATION C8-PC3 stimulus red NMH_MUTATE=C8-PC3: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first frame A; fx68k and TG68K: FAIL C8-PC3; NOT GRADED: C8.
// MUTATION C8-PC4 stimulus red NMH_MUTATE=C8-PC4: the calibration does not write V1; fx68k and TG68K: FAIL C8-PC4; NOT GRADED: C8.
// MUTATION C8-PC5 stimulus red NMH_MUTATE=C8-PC5: DMACONR is read after DMACON = $8200 (without BLTEN) and DMACON = $8240 is written after the read, so the DMA_CHECK reads BLTEN clear and the run is otherwise the normal run; fx68k and TG68K: FAIL C8-PC5; NOT GRADED: C8.
// MUTATION C8-PC6 stimulus red NMH_MUTATE=C8-PC6: in frame B only, the observation blit is BLTSIZE $0043 (three words) in place of $0042; fx68k and TG68K: FAIL C8-PC6; NOT GRADED: C8.
// MUTATION C9 stimulus red NMH_MUTATE=C9: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; fx68k and TG68K: FAIL C9.
// MUTATION C9-FIXTURE stimulus red NMH_MUTATE=C9-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL C9-FIXTURE; NOT GRADED: C9-FW, C9.
// MUTATION C9-PC1 stimulus red NMH_MUTATE=C9-PC1: the strobe COPJMP1 is written after M_wait in place of before M_start, so the Copper reads the MOVE after the window of PC1 and still reads the case list to its end; fx68k and TG68K: FAIL C9-PC1; NOT GRADED: C9.
// MUTATION C9-PC2 stimulus red NMH_MUTATE=C9-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; fx68k and TG68K: FAIL C9-PC2; NOT GRADED: C9.
// MUTATION C9-PC3 stimulus red NMH_MUTATE=C9-PC3: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first frame A; fx68k and TG68K: FAIL C9-PC3; NOT GRADED: C9.
// MUTATION C9-PC4 stimulus red NMH_MUTATE=C9-PC4: the calibration does not write V1; fx68k and TG68K: FAIL C9-PC4; NOT GRADED: C9.
// MUTATION C9-PC5 stimulus red NMH_MUTATE=C9-PC5: DMACONR is read after DMACON = $8200 (without BLTEN) and DMACON = $8240 is written after the read, so the DMA_CHECK reads BLTEN clear and the run is otherwise the normal run; fx68k and TG68K: FAIL C9-PC5; NOT GRADED: C9.
// MUTATION C9-PC6 stimulus red NMH_MUTATE=C9-PC6: in frame B only, the observation blit is BLTSIZE $0044 (four words) in place of $0043; fx68k and TG68K: FAIL C9-PC6; NOT GRADED: C9.
// MUTATION C10 stimulus red NMH_MUTATE=C10: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; fx68k and TG68K: FAIL C10.
// MUTATION C10-FIXTURE stimulus red NMH_MUTATE=C10-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL C10-FIXTURE; NOT GRADED: C10-FW, C10.
// MUTATION C10-PC1 stimulus red NMH_MUTATE=C10-PC1: the strobe COPJMP1 is written after M_wait in place of before M_start, so the Copper reads the MOVE after the window of PC1 and still reads the case list to its end; fx68k and TG68K: FAIL C10-PC1; NOT GRADED: C10.
// MUTATION C10-PC2 stimulus red NMH_MUTATE=C10-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; fx68k and TG68K: FAIL C10-PC2; NOT GRADED: C10.
// MUTATION C10-PC3 stimulus red NMH_MUTATE=C10-PC3: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first frame A; fx68k and TG68K: FAIL C10-PC3; NOT GRADED: C10.
// MUTATION C10-PC4 stimulus red NMH_MUTATE=C10-PC4: the calibration does not write V1; fx68k and TG68K: FAIL C10-PC4; NOT GRADED: C10.
// MUTATION C10-PC5 stimulus red NMH_MUTATE=C10-PC5: DMACONR is read after DMACON = $8200 (without BLTEN) and DMACON = $8240 is written after the read, so the DMA_CHECK reads BLTEN clear and the run is otherwise the normal run; fx68k and TG68K: FAIL C10-PC5; NOT GRADED: C10.
// MUTATION C11 stimulus red NMH_MUTATE=C11: the frame B of COPCON $0000 writes COPCON $0002 in place of $0000; fx68k and TG68K: FAIL C11.
// MUTATION C11-FIXTURE stimulus red NMH_MUTATE=C11-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL C11-FIXTURE; NOT GRADED: C11-FW, C11.
// MUTATION C11-PC1 stimulus red NMH_MUTATE=C11-PC1: the strobe COPJMP1 is written after M_wait in place of before M_start, so the Copper reads the MOVE after the window of PC1 and still reads the case list to its end; fx68k and TG68K: FAIL C11-PC1; NOT GRADED: C11.
// MUTATION C11-PC2 stimulus red NMH_MUTATE=C11-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; fx68k and TG68K: FAIL C11-PC2; NOT GRADED: C11.
// MUTATION C11-PC3 stimulus red NMH_MUTATE=C11-PC3: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first frame A; fx68k and TG68K: FAIL C11-PC3; NOT GRADED: C11.
// MUTATION C11-PC4 stimulus red NMH_MUTATE=C11-PC4: the calibration does not write V1; fx68k and TG68K: FAIL C11-PC4; NOT GRADED: C11.
// MUTATION C11-PC5 stimulus red NMH_MUTATE=C11-PC5: DMACONR is read after DMACON = $8200 (without BLTEN) and DMACON = $8240 is written after the read, so the DMA_CHECK reads BLTEN clear and the run is otherwise the normal run; fx68k and TG68K: FAIL C11-PC5; NOT GRADED: C11.
// MUTATION C12 stimulus red NMH_MUTATE=C12: in the frame B of COPCON $0000 the program preloads $5A5A into PX, so the DMA channel loads V1 as the register's value; fx68k and TG68K: FAIL C12.
// MUTATION C12-FIXTURE stimulus red NMH_MUTATE=C12-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL C12-FIXTURE; NOT GRADED: C12-FW, C12.
// MUTATION C12-PC1 stimulus red NMH_MUTATE=C12-PC1: COPEN is cleared (DMACON = $0080) after COP1LC is written and set again after M_end, so the Copper fetches no case list; fx68k and TG68K: FAIL C12-PC1; NOT GRADED: C12.
// MUTATION C12-PC2 stimulus red NMH_MUTATE=C12-PC2: the program waits for the next vertical blank (INTREQR bit 5) between M_wait and the observation, so F_vb = 1; fx68k and TG68K: FAIL C12-PC2; NOT GRADED: C12.
// MUTATION C12-PC3 stimulus red NMH_MUTATE=C12-PC3: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first frame A; fx68k and TG68K: FAIL C12-PC3; NOT GRADED: C12.
// MUTATION C12-PC4 stimulus red NMH_MUTATE=C12-PC4: frame A does not write V1 into PX, so the preload of V1 loads V0 again; fx68k and TG68K: FAIL C12-PC4; NOT GRADED: C12.
// MUTATION C12-PC5 stimulus red NMH_MUTATE=C12-PC5: DMACONR is read after DMACON = $8200 (without BLTEN) and DMACON = $8240 is written after the read, so the DMA_CHECK reads BLTEN clear and the run is otherwise the normal run; fx68k and TG68K: FAIL C12-PC5; NOT GRADED: C12.
// MUTATION F1 stimulus red NMH_MUTATE=F1: the last case writes the size (6,3) while (5,3) is expected; fx68k and TG68K: FAIL F1.
// MUTATION F1-FIXTURE stimulus red NMH_MUTATE=F1-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL F1-FIXTURE; NOT GRADED: F1.
// MUTATION F1-DMA stimulus red NMH_MUTATE=F1-DMA: DMACONR is read after DMACON = $8200 (without BLTEN) and DMACON = $8240 is written after the read; fx68k and TG68K: FAIL F1-DMA; NOT GRADED: F1.
// MUTATION F1-TMO stimulus red NMH_MUTATE=F1-TMO: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first case; fx68k and TG68K: FAIL F1-TMO; NOT GRADED: F1.
// MUTATION F1 stimulus red NMH_MUTATE=F1-IDLE: a blit of one word is started after the last case and not awaited, so a blitter write lies outside the windows of the cases; fx68k and TG68K: FAIL F1.
// MUTATION F2 stimulus red NMH_MUTATE=F2: the case with v = 8 writes v = 9; fx68k and TG68K: FAIL F2.
// MUTATION F2-FIXTURE stimulus red NMH_MUTATE=F2-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL F2-FIXTURE; NOT GRADED: F2.
// MUTATION F2-DMA stimulus red NMH_MUTATE=F2-DMA: DMACONR is read after DMACON = $8200 (without BLTEN) and DMACON = $8240 is written after the read; fx68k and TG68K: FAIL F2-DMA; NOT GRADED: F2.
// MUTATION F2-TMO stimulus red NMH_MUTATE=F2-TMO: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first case; fx68k and TG68K: FAIL F2-TMO; NOT GRADED: F2.
// MUTATION F2 stimulus red NMH_MUTATE=F2-IDLE: a blit is started with BLTSIZH after the last case and not awaited, so a blitter write lies outside the windows of the cases; fx68k and TG68K: FAIL F2.
// MUTATION F3 stimulus red NMH_MUTATE=F3: the full BLTCON0 is written with $FF0F in place of BLTCON0L; fx68k and TG68K: FAIL F3.
// MUTATION F3-FIXTURE stimulus red NMH_MUTATE=F3-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL F3-FIXTURE; NOT GRADED: F3.
// MUTATION F3-DMA stimulus red NMH_MUTATE=F3-DMA: DMACONR is read after DMACON = $8200 (without BLTEN) and DMACON = $8240 is written after the read; fx68k and TG68K: FAIL F3-DMA; NOT GRADED: F3.
// MUTATION F3-TMO stimulus red NMH_MUTATE=F3-TMO: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first case; fx68k and TG68K: FAIL F3-TMO; NOT GRADED: F3.
// MUTATION F3 stimulus red NMH_MUTATE=F3-IDLE: a second blit of two words is started after the window and not awaited, so a blitter write lies outside the window; fx68k and TG68K: FAIL F3.
// MUTATION F4 stimulus red NMH_MUTATE=F4: in the second case (channel 0) AUDcLCL is written with A4 flipped while a is expected, so every read of the channel lies outside [a, a+8); fx68k and TG68K: FAIL F4.
// MUTATION F4-FIXTURE stimulus red NMH_MUTATE=F4-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL F4-FIXTURE; NOT GRADED: F4.
// MUTATION F4-DMA stimulus red NMH_MUTATE=F4-DMA: in the second case (channel 0) DMACONR is read before the channel's enable is written, so the DMA_CHECK reads the enable bit clear; the enable follows; fx68k and TG68K: FAIL F4-DMA; NOT GRADED: F4.
// MUTATION F4-ONLY stimulus red NMH_MUTATE=F4-ONLY: in the second case of channel 1 channel 0 is enabled as well; fx68k and TG68K: FAIL F4-ONLY; NOT GRADED: F4.
// MUTATION F4-QUIET stimulus red NMH_MUTATE=F4-QUIET: in the second case (channel 0) the enable is written before M_start, so audio reads fall between M_start and M_go; fx68k and TG68K: FAIL F4-QUIET; NOT GRADED: F4.
// MUTATION F4-POP stimulus red NMH_MUTATE=F4-POP: in the second case (channel 0) the enable is written after the LINES(20) of the window and M_end follows the read of DMACONR at once, so fewer than two reads fall in a window that still holds the 20 line starts; fx68k and TG68K: FAIL F4-POP; NOT GRADED: F4.
// MUTATION F4-REF stimulus red NMH_MUTATE=F4-REF: in the second case (channel 0) the window is 8 lines in place of 20, so it holds fewer than 76 refresh reads; fx68k and TG68K: FAIL F4-REF; NOT GRADED: F4.
// MUTATION F4-TMO stimulus red NMH_MUTATE=F4-TMO: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first case; fx68k and TG68K: FAIL F4-TMO; NOT GRADED: F4.
// MUTATION F5 stimulus red NMH_MUTATE=F5: SPR3PTL of the family S1 is written with bit 8 flipped; fx68k and TG68K: FAIL F5.
// MUTATION F5-FIXTURE stimulus red NMH_MUTATE=F5-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL F5-FIXTURE; NOT GRADED: F5-FW, F5.
// MUTATION F5-DMA stimulus red NMH_MUTATE=F5-DMA: in the second family DMACONR is read after DMACON = $8200 (without SPREN) and DMACON = $8220 is written after the read; fx68k and TG68K: FAIL F5-DMA; NOT GRADED: F5.
// MUTATION F5-QUIET stimulus red NMH_MUTATE=F5-QUIET: in the second family the program waits 60 lines after the pointer writes before M1, so the sprite reads of the frame fall between M0 and M1; the window then runs over the next vertical blank and 30 lines and holds the sprite reads of the next frame; fx68k and TG68K: FAIL F5-QUIET; NOT GRADED: F5.
// MUTATION F5-POP stimulus red NMH_MUTATE=F5-POP: in the second family the program waits 30 lines after the first VBL_SYNC before M0, so the sprite reads of the frame lie before both windows, and the window (M1, M2) still holds the 40 line starts of its LINES(40) wait; fx68k and TG68K: FAIL F5-POP; NOT GRADED: F5.
// MUTATION F5-REF stimulus red NMH_MUTATE=F5-REF: in the second family the window ends after 30 lines in place of 40 lines and the vertical blank, so it holds the sprite reads and fewer than 156 refresh reads; fx68k and TG68K: FAIL F5-REF; NOT GRADED: F5.
// MUTATION F5-TMO stimulus red NMH_MUTATE=F5-TMO: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first case; fx68k and TG68K: FAIL F5-TMO; NOT GRADED: F5.
// MUTATION F5 stimulus red NMH_MUTATE=F5-STRAY: SPR3PTL of the family S1 is written with bit 10 flipped (A10, outside A8-A6), so the sprite read lies outside all eight regions and the stray class fires; fx68k and TG68K: FAIL F5.
// MUTATION F6 stimulus red NMH_MUTATE=F6: the band rows of planes 3 and 4 are swapped in memory; fx68k and TG68K: FAIL F6.
// MUTATION F6-FIXTURE stimulus red NMH_MUTATE=F6-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL F6-FIXTURE; NOT GRADED: F6.
// MUTATION F6-DMA stimulus red NMH_MUTATE=F6-DMA: DMACONR is read after DMACON = $8200 (without BPLEN) and DMACON = $8300 is written after the read; fx68k and TG68K: FAIL F6-DMA; NOT GRADED: F6.
// MUTATION F6-FRAMES stimulus red NMH_MUTATE=F6-FRAMES: one VBL_SYNC in place of four between M_ready and M_done, so fewer than three complete frames lie in the interval; F6-READS, F6-COPPER and F6 print NOT GRADED; fx68k and TG68K: FAIL F6-FRAMES; NOT GRADED: F6-COPPER, F6.
// MUTATION F6 stimulus red NMH_MUTATE=F6-READS: BPLCON0 is $7000 (BPU = 7), so plane 8 is not fetched: no read in B8 and no band C8; fx68k and TG68K: FAIL F6.
// MUTATION F6-COPPER stimulus red NMH_MUTATE=F6-COPPER: the Copper runs a copy of the list at another address ($090500), which loads the same pointers, so the words of the list at $090400 are not read and the planes are fetched; fx68k and TG68K: FAIL F6-COPPER; NOT GRADED: F6.
// MUTATION F6-TMO stimulus red NMH_MUTATE=F6-TMO: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first case; fx68k and TG68K: FAIL F6-TMO; NOT GRADED: F6.
// MUTATION F7 stimulus red NMH_MUTATE=F7: the byte $C0 (-64) of the third word is $60 (+96), so the levels are not in the order of the two's-complement values of the bytes in either byte order; fx68k and TG68K: FAIL F7.
// MUTATION F7-FIXTURE stimulus red NMH_MUTATE=F7-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL F7-FIXTURE; NOT GRADED: F7.
// MUTATION F7-DMA stimulus red NMH_MUTATE=F7-DMA: in the first case DMACONR is read before the channel's enable is written, so the DMA_CHECK reads the enable bit clear; the enable follows; fx68k and TG68K: FAIL F7-DMA; NOT GRADED: F7.
// MUTATION F7-ADK stimulus red NMH_MUTATE=F7-ADK: in the first case ADKCON bit 0 (USE0V1) is set before ADKCONR is read; fx68k and TG68K: FAIL F7-ADK; NOT GRADED: F7.
// MUTATION F7 stimulus red NMH_MUTATE=F7-POP: in the first case the enable is written 95 lines after M_start, in a window that still holds the 100 line starts of its LINES waits, so fewer than eight reads of [AD, AD+8) fall in it; fx68k and TG68K: FAIL F7.
// MUTATION F7-STRAY stimulus red NMH_MUTATE=F7-STRAY: in the first case AUDcLCL is rewritten to AD + $100 sixty lines after the enable, so the next restart reads outside [AD, AD+8); fx68k and TG68K: FAIL F7-STRAY; NOT GRADED: F7.
// MUTATION F7-REF stimulus red NMH_MUTATE=F7-REF: in the first case the window is 40 lines in place of 100, so it holds fewer than 396 refresh reads; fx68k and TG68K: FAIL F7-REF; NOT GRADED: F7.
// MUTATION F7-TMO stimulus red NMH_MUTATE=F7-TMO: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first case; fx68k and TG68K: FAIL F7-TMO; NOT GRADED: F7.
// MUTATION F8 stimulus red NMH_MUTATE=F8: the third source word of each source under test is $0000; fx68k and TG68K: FAIL F8.
// MUTATION F8-FIXTURE stimulus red NMH_MUTATE=F8-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL F8-FIXTURE; NOT GRADED: F8.
// MUTATION F8-DMA stimulus red NMH_MUTATE=F8-DMA: DMACONR is read after DMACON = $8200 (without BLTEN) and DMACON = $8240 is written after the read; fx68k and TG68K: FAIL F8-DMA; NOT GRADED: F8.
// MUTATION F8-TMO stimulus red NMH_MUTATE=F8-TMO: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first case; fx68k and TG68K: FAIL F8-TMO; NOT GRADED: F8.
// MUTATION F8 stimulus red NMH_MUTATE=F8-READS: USEx is cleared in BLTCON0, so the blit makes no read of SRC_x; fx68k and TG68K: FAIL F8.
// MUTATION F8 stimulus red NMH_MUTATE=F8-AFTER: a second blit is started after M_end of the last case and not awaited, so a blitter write lies outside the windows of the three blits; fx68k and TG68K: FAIL F8.
// MUTATION F9 stimulus red NMH_MUTATE=F9: DSKLEN is $C003 in place of $C004 (both writes) in the LENGTH = 4 case, which expects four reads; fx68k and TG68K: FAIL F9.
// MUTATION F9-FIXTURE stimulus red NMH_MUTATE=F9-FIXTURE: the run loads prog_run_nofix.bin, which does not store the constant $BEEF; fx68k and TG68K: FAIL F9-FIXTURE; NOT GRADED: F9-FW, F9.
// MUTATION F9-DMA stimulus red NMH_MUTATE=F9-DMA: DMACONR is read after DMACON = $8200 (without DSKEN) and DMACON = $8210 is written after the read; fx68k and TG68K: FAIL F9-DMA; NOT GRADED: F9.
// MUTATION F9-DMAON stimulus red NMH_MUTATE=F9-DMAON: in the LENGTH = 4 case DSKLEN's DMAEN write is made once, not twice, so DMAON reads 0 (HRM p.247 'the DMAEN bit in the DSKLEN register must be turned on twice in order to actually enable the disk DMA hardware'; HRM p.248 DMAON 'Indicates when DMA is actually enabled'); fx68k and TG68K: FAIL F9-DMAON; NOT GRADED: F9.
// MUTATION F9-REF stimulus red NMH_MUTATE=F9-REF: in the LENGTH = 4 case M_end follows the DSKBYTR read, so the window holds fewer than 12 refresh reads; fx68k and TG68K: FAIL F9-REF; NOT GRADED: F9.
// MUTATION F9-TMO stimulus red NMH_MUTATE=F9-TMO: a WAITB on DMACONR bit 15, which is never set, ends by its timeout before the first case; fx68k and TG68K: FAIL F9-TMO; NOT GRADED: F9.
// MUTATION F9 stimulus red NMH_MUTATE=F9-STRAY: in the LENGTH = 4 case DSKPTH/DSKPTL point 0x1000 bytes above DB, outside [DB, DB+12), so the four disk reads are stray DMA reads; fx68k and TG68K: FAIL F9.
// MUTATION F9-FW stimulus red NMH_MUTATE=F9-FW: IDLE is {$0001, $FFFE, $FFFF, $FFFE}: the first WAIT ends at once, so the word at IDLE + 4 that the comparator names is the first word of the list's final WAIT and is read; fx68k and TG68K: FAIL F9-FW.
// MUTATION F5-FWA stimulus red NMH_MUTATE=F5-FWA: DMACON = $8200 in the prologue, so Copper DMA is never turned on and no list is read; fx68k and TG68K: FAIL F5-FWA; NOT GRADED: F5-FW.
// MUTATION F9-FWA stimulus red NMH_MUTATE=F9-FWA: DMACON = $8200 in the prologue, so Copper DMA is never turned on and no list is read; fx68k and TG68K: FAIL F9-FWA; NOT GRADED: F9-FW.
// MUTATION C1-FWA stimulus red NMH_MUTATE=C1-FWA: the frame that writes COPCON $0002 writes $0000 instead, so the MOVE is refused in every frame and the case lists are not run to their final WAIT; fx68k and TG68K: FAIL C1-FWA; also FAIL: C1; NOT GRADED: C1-FW.
// MUTATION C2-FWA stimulus red NMH_MUTATE=C2-FWA: the frame that writes COPCON $0002 writes $0000 instead, so the MOVE is refused in every frame and the case lists are not run to their final WAIT; fx68k and TG68K: FAIL C2-FWA; also FAIL: C2; NOT GRADED: C2-FW.
// MUTATION C3-FWA stimulus red NMH_MUTATE=C3-FWA: the frame that writes COPCON $0002 writes $0000 instead, so the MOVE is refused in every frame and the case lists are not run to their final WAIT; fx68k and TG68K: FAIL C3-FWA; also FAIL: C3; NOT GRADED: C3-FW.
// MUTATION C4-FWA stimulus red NMH_MUTATE=C4-FWA: the frame that writes COPCON $0002 writes $0000 instead, so the MOVE is refused in every frame and the case lists are not run to their final WAIT; fx68k and TG68K: FAIL C4-FWA; also FAIL: C4; NOT GRADED: C4-FW.
// MUTATION C5-FWA stimulus red NMH_MUTATE=C5-FWA: the frame that writes COPCON $0002 writes $0000 instead, so the MOVE is refused in every frame and the case lists are not run to their final WAIT; fx68k and TG68K: FAIL C5-FWA; also FAIL: C5; NOT GRADED: C5-FW.
// MUTATION C6-FWA stimulus red NMH_MUTATE=C6-FWA: the frame that writes COPCON $0002 writes $0000 instead, so the MOVE is refused in every frame and the case lists are not run to their final WAIT; fx68k and TG68K: FAIL C6-FWA; also FAIL: C6; NOT GRADED: C6-FW.
// MUTATION C7-FWA stimulus red NMH_MUTATE=C7-FWA: the frame that writes COPCON $0002 writes $0000 instead, so the MOVE is refused in every frame and the case lists are not run to their final WAIT; fx68k and TG68K: FAIL C7-FWA; also FAIL: C7; NOT GRADED: C7-FW.
// MUTATION C8-FWA stimulus red NMH_MUTATE=C8-FWA: the frame that writes COPCON $0002 writes $0000 instead, so the MOVE is refused in every frame and the case lists are not run to their final WAIT; fx68k and TG68K: FAIL C8-FWA; also FAIL: C8; NOT GRADED: C8-FW.
// MUTATION C9-FWA stimulus red NMH_MUTATE=C9-FWA: the frame that writes COPCON $0002 writes $0000 instead, so the MOVE is refused in every frame and the case lists are not run to their final WAIT; fx68k and TG68K: FAIL C9-FWA; also FAIL: C9; NOT GRADED: C9-FW.
// MUTATION C10-FWA stimulus red NMH_MUTATE=C10-FWA: the frame that writes COPCON $0002 writes $0000 instead, so the MOVE is refused in every frame and the case lists are not run to their final WAIT; fx68k and TG68K: FAIL C10-FWA; also FAIL: C10; NOT GRADED: C10-FW.
// MUTATION C11-FWA stimulus red NMH_MUTATE=C11-FWA: the frame that writes COPCON $0002 writes $0000 instead, so the MOVE is refused in every frame and the case lists are not run to their final WAIT; fx68k and TG68K: FAIL C11-FWA; also FAIL: C11; NOT GRADED: C11-FW.
// MUTATION C1-FW implementation red, no NMH_MUTATE selector: the normal run prints FAIL on both cores; reads of the word after a final WAIT, per list name (lists with a read in): fx68k and TG68K: IDLE 5 reads, DSKLEN lists (32) 31 reads in 31. The same comparator prints PASS for F9-FW on both cores (IDLE + 4 read 0 times, the final WAIT read), and NMH_MUTATE=F9-FW moves F9-FW to FAIL.
// MUTATION C2-FW implementation red, no NMH_MUTATE selector: the normal run prints FAIL on both cores; reads of the word after a final WAIT, per list name (lists with a read in): fx68k and TG68K: IDLE 53 reads, DSKPTH lists (32) 32 reads in 32, DSKPTL lists (32) 31 reads in 31. The same comparator prints PASS for F9-FW on both cores (IDLE + 4 read 0 times, the final WAIT read), and NMH_MUTATE=F9-FW moves F9-FW to FAIL.
// MUTATION C3-FW implementation red, no NMH_MUTATE selector: the normal run prints FAIL on both cores; reads of the word after a final WAIT, per list name (lists with a read in): fx68k and TG68K: IDLE 5 reads. The same comparator prints PASS for F9-FW on both cores (IDLE + 4 read 0 times, the final WAIT read), and NMH_MUTATE=F9-FW moves F9-FW to FAIL.
// MUTATION C4-FW implementation red, no NMH_MUTATE selector: the normal run prints FAIL on both cores; reads of the word after a final WAIT, per list name (lists with a read in): fx68k and TG68K: IDLE 2 reads. The same comparator prints PASS for F9-FW on both cores (IDLE + 4 read 0 times, the final WAIT read), and NMH_MUTATE=F9-FW moves F9-FW to FAIL.
// MUTATION C5-FW implementation red, no NMH_MUTATE selector: the normal run prints FAIL on both cores; reads of the word after a final WAIT, per list name (lists with a read in): fx68k and TG68K: IDLE 5 reads, POTGO lists (16) 15 reads in 15. The same comparator prints PASS for F9-FW on both cores (IDLE + 4 read 0 times, the final WAIT read), and NMH_MUTATE=F9-FW moves F9-FW to FAIL.
// MUTATION C6-FW implementation red, no NMH_MUTATE selector: the normal run prints FAIL on both cores; reads of the word after a final WAIT, per list name (lists with a read in): fx68k and TG68K: IDLE 9 reads, JOYTEST lists (32) 31 reads in 31. The same comparator prints PASS for F9-FW on both cores (IDLE + 4 read 0 times, the final WAIT read), and NMH_MUTATE=F9-FW moves F9-FW to FAIL.
// MUTATION C7-FW implementation red, no NMH_MUTATE selector: the normal run prints FAIL on both cores; reads of the word after a final WAIT, per list name (lists with a read in): fx68k and TG68K: IDLE 13 reads, BLTCON0 lists (1) 1 reads in 1, BLTCON0L lists (32) 31 reads in 31. The same comparator prints PASS for F9-FW on both cores (IDLE + 4 read 0 times, the final WAIT read), and NMH_MUTATE=F9-FW moves F9-FW to FAIL.
// MUTATION C8-FW implementation red, no NMH_MUTATE selector: the normal run prints FAIL on both cores; reads of the word after a final WAIT, per list name (lists with a read in): fx68k and TG68K: IDLE 5 reads. The same comparator prints PASS for F9-FW on both cores (IDLE + 4 read 0 times, the final WAIT read), and NMH_MUTATE=F9-FW moves F9-FW to FAIL.
// MUTATION C9-FW implementation red, no NMH_MUTATE selector: the normal run prints FAIL on both cores; reads of the word after a final WAIT, per list name (lists with a read in): fx68k and TG68K: IDLE 17 reads, BLTAFWM lists (32) 32 reads in 32, BLTALWM lists (32) 31 reads in 31. The same comparator prints PASS for F9-FW on both cores (IDLE + 4 read 0 times, the final WAIT read), and NMH_MUTATE=F9-FW moves F9-FW to FAIL.
// MUTATION C10-FW implementation red, no NMH_MUTATE selector: the normal run prints FAIL on both cores; reads of the word after a final WAIT, per list name (lists with a read in): fx68k: IDLE 29 reads, BLTSIZE lists (32) 34 reads in 32, BLTSIZH lists (22) 21 reads in 21; TG68K: IDLE 29 reads, BLTSIZE lists (32) 32 reads in 32, BLTSIZH lists (22) 21 reads in 21. The same comparator prints PASS for F9-FW on both cores (IDLE + 4 read 0 times, the final WAIT read), and NMH_MUTATE=F9-FW moves F9-FW to FAIL.
// MUTATION C11-FW implementation red, no NMH_MUTATE selector: the normal run prints FAIL on both cores; reads of the word after a final WAIT, per list name (lists with a read in): fx68k: IDLE 5 reads, BLTSIZV lists (8) 7 reads in 7; TG68K: IDLE 5 reads, BLTSIZV lists (8) 8 reads in 7. The same comparator prints PASS for F9-FW on both cores (IDLE + 4 read 0 times, the final WAIT read), and NMH_MUTATE=F9-FW moves F9-FW to FAIL.
// MUTATION C12-FW implementation red, no NMH_MUTATE selector: the normal run prints FAIL on both cores; reads of the word after a final WAIT, per list name (lists with a read in): fx68k and TG68K: IDLE 4 reads. The same comparator prints PASS for F9-FW on both cores (IDLE + 4 read 0 times, the final WAIT read), and NMH_MUTATE=F9-FW moves F9-FW to FAIL.
// MUTATION F5-FW implementation red, no NMH_MUTATE selector: the normal run prints FAIL on both cores; reads of the word after a final WAIT, per list name (lists with a read in): fx68k and TG68K: IDLE 20 reads. The same comparator prints PASS for F9-FW on both cores (IDLE + 4 read 0 times, the final WAIT read), and NMH_MUTATE=F9-FW moves F9-FW to FAIL.
// MUTATION C2 implementation red private copy of the chip set outside the repository, whose disk channel never reads, normal run with NMH_ONLY=C2: DMAON reads 1 and no disk read follows, C2-PC4 prints NOT GRADED and C2 prints FAIL through 'Disk DMA runs'; fx68k and TG68K: FAIL C2 (and C2-FW as in every run), NOT GRADED C2-PC4.
// MUTATION C12-FWA not exercised, by a scope decision: C12 grades IDLE only, whose final WAIT the Copper reads whenever it runs at all, and C12-PC1 contrasts the Copper not running its lists; no stimulus keeps the other preconditions of C12 and removes the reads of IDLE.
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
    print_c(ck, p, e, "(" + std::string(kP6) + ")" + (claims.empty() ? "" : " claims " + claims), what, pc5, pc6);
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
    if (want("C1")) do_c(ck, prop_c1(), "",
        "a Copper MOVE to DSKLEN of each of the 32 walking values (1 << k and its complement, k = 0-15) changes DSKBYTR bit 13, the mirror of DSKLEN bit 14 WRITE, only while COPCON bit 1 is set: 0 with COPCON $0000 and $FFFD, bit 14 of the value with $0002 (AA p.23)", "");
    if (want("C3")) do_c(ck, prop_c3(), "",
        "a Copper MOVE to VPOSW ($8000) changes VPOSR bit 15, the frame flop LOF, only while COPCON bit 1 is set: 0 with COPCON $0000 and $FFFD, 1 with $0002 (AA p.29)", "");
    if (want("C5")) do_c(ck, prop_c5(), "",
        "a Copper MOVE to POTGO of each of the 16 walking values of bits 15-8 (OUTRY, DATRY, OUTRX, DATRX, OUTLY, DATLY, OUTLX, DATLX) changes the level of the pot pins that the value drives, read in POTINP, only while COPCON bit 1 is set: all driven pins high ($FF00 of V0) with COPCON $0000 and $FFFD, each driven pin at the level of its DAT bit with $0002 (AA p.26-27)", "");
    if (want("C7")) do_c(ck, prop_c7(), "opus:AA-bltcon0l.c5 sol:AA-bltcon0l.c5",
        "a Copper MOVE to BLTCON0 ($01FF), or to BLTCON0L of each of the 32 walking values (1 << k and its complement, k = 0-15), changes the next blit only while COPCON bit 1 is set: BLTCON0 $01FF gives four writes of $0000 with COPCON $0000 and $FFFD and four of $FFFF with $0002; BLTCON0L gives, with V0 BLTCON0 = $0F00 and the sources A = $F0F0, B = $CCCC, C = $AAAA, one write of $0000 with COPCON $0000 and $FFFD and one write of LF7-LF0 of the value in both bytes with $0002 (AA p.13, p.14)", pc5blt,
        "every observed blit made exactly the blitter writes and reads of its set-up");
    if (want("C8")) do_c(ck, prop_c8(), "",
        "a Copper MOVE to BLTCON1 ($0002, DESC) changes the address direction of the next blit only while COPCON bit 1 is set: writes at DST, DST+2 with COPCON $0000 and $FFFD, at DST, DST-2 with $0002 (AA p.14)", pc5blt,
        "every observed blit made exactly two blitter writes");
    if (want("C9")) do_c(ck, prop_c9(), "opus:AA-bltafwm.c4 sol:AA-bltafwm.c4 opus:AA-bltalwm.c4 sol:AA-bltalwm.c4",
        "a Copper MOVE of each of the 32 walking values (1 << k and its complement, k = 0-15) to BLTAFWM or to BLTALWM changes the first-word or last-word mask of the next blit only while COPCON bit 1 is set: the first or last word written is $FFFF with COPCON $0000 and $FFFD and the value itself with $0002 (AA p.13)", pc5blt,
        "every observed blit made exactly three blitter reads of ASRC and three blitter writes");
    if (want("C10")) do_c(ck, prop_c10(), "opus:AA-bltsize.c4 sol:AA-bltsize.c4 opus:AA-bltsizh.c5 sol:AA-bltsizh.c5",
        "a Copper MOVE to BLTSIZE (w = 1 << k and 63 minus 1 << k, h = 1 << k and 1023 minus 1 << k, the other field 1: 32 values) or to BLTSIZH (w = 1 << k and 2047 minus 1 << k, k = 0-10, with BLTSIZV = 1: 22 values) starts the blitter only while COPCON bit 1 is set: no blitter write with COPCON $0000 and $FFFD, w x h writes of $FFFF with $0002 (AA p.14)", pc5blt);
    if (want("C11")) do_c(ck, prop_c11(), "",
        "a Copper MOVE to BLTSIZV of each of the heights 1, 2, 4, ..., 128 changes the height of the next blit only while COPCON bit 1 is set: 2 writes with COPCON $0000 and $FFFD, as many writes as the height with $0002 (AA p.14)", pc5blt);
    if (want("C2")) do_c(ck, prop_c2(), "opus:AA-dskpth.c5 sol:AA-dskpth.c5 opus:AA-dskptl.c5 sol:AA-dskptl.c5",
        "a Copper MOVE to DSKPTH or to DSKPTL changes the address from which disk DMA reads only while COPCON bit 1 is set: the first disk read is at Rd with COPCON $0000 and $FFFD and at Re (DSKPTH) or Re' (DSKPTL) with $0002, and disk DMA with DMAON set reads RAM in every run (AA p.23 'This pair of registers contains the 20 bit address of Disk DMA data.')",
        "DMACONR reads DMAEN and DSKEN set, and no disk read lies outside the windows of the cases, so every disk DMA finished inside its case (AA p.22, p.23)",
        "DSKBYTR bit 14 DMAON reads 1 in every run of the disk DMA sequence (AA p.23 'DMAEN(DSKLEN) & DMAEN(DMACON) & DSKEN(DMACON)'; HRM p.247 'the DMAEN bit in the DSKLEN register must be turned on twice in order to actually enable the disk DMA hardware'; HRM p.248 DMAON 'Indicates when DMA is actually enabled')");
    if (want("C4")) do_c(ck, prop_c4(), "",
        "a Copper MOVE to VHPOSW ($C000) moves the beam counter only while COPCON bit 1 is set: V7 (VHPOSR bit 15, R & $80) is clear at the first poll past line $40 with COPCON $0000 and $FFFD and set with $0002 (AA p.29)", "");
    if (want("C6")) do_c(ck, prop_c6(), "opus:AA-joytest.c4 sol:AA-joytest.c4",
        "a Copper MOVE to JOYTEST ($FCFC) changes the mouse counters of JOY0DAT and JOY1DAT (read & $FCFC) only while COPCON bit 1 is set: $0000 with COPCON $0000 and $FFFD, $FCFC with $0002 (AA p.26)", "",
        "the two reads of each of JOY0DAT and JOY1DAT in a case are equal, so nothing but the write moved the counters");
    if (want("C12")) { C12Run r = run_c12(g_mutate ? g_mutate : ""); eval_print_c12(ck, r, ""); }
    const std::string sel = g_mutate ? g_mutate : "";
    if (want("F1")) run_f1_f2(ck, false, sel);
    if (want("F2")) run_f1_f2(ck, true, sel);
    if (want("F3")) run_f3(ck, sel);
    if (want("F4")) run_f4(ck, sel);
    if (want("F5")) run_f5(ck, sel);
    if (want("F6")) run_f6(ck, sel);
    if (want("F7")) run_f7(ck, sel);
    if (want("F8")) run_f8(ck, sel);
    if (want("F9")) run_f9(ck, sel);
    if (!g_mutate && !only) {
        // claims the package declines (SPECIFICATION.md section 0)
        ck.record("DECLINED the ~ on a read address or a dummy address (SPECIFICATION.md 0.1): no page states an effect of a write to it, by the CPU or by the Copper, so COPCON bit 1 set and clear predict the same outcome at every output: opus:AA-adkconr.c4 sol:AA-adkconr.c4 opus:AA-dskbytr.c4 sol:AA-dskbytr.c4 opus:AA-intenar.c4 sol:AA-intenar.c4 opus:AA-intreqr.c4 sol:AA-intreqr.c4 opus:AA-joy0dat.c4 sol:AA-joy0dat.c4 opus:AA-joy1dat.c4 sol:AA-joy1dat.c4 opus:AA-pot0dat.c4 sol:AA-pot0dat.c4 opus:AA-pot1dat.c4 sol:AA-pot1dat.c4 opus:AA-potinp.c4 sol:AA-potinp.c4 opus:AA-vhposr.c4 sol:AA-vhposr.c4 opus:AA-vposr.c4 sol:AA-vposr.c4 opus:AA-lisaid.c5 sol:AA-lisaid.c5 opus:AA-dskdatr.c5 sol:AA-dskdatr.c5 opus:AA-bltddat.c5 sol:AA-bltddat.c5");
        ck.record("DECLINED the disk-side effects (SPECIFICATION.md 0.2): each needs data read from or written to a disk surface, and the harness serves no disk image: opus:AA-dskdat.c5 sol:AA-dskdat.c5 opus:AA-dsksync.c4 sol:AA-dsksync.c4 opus:AA-dskdatr.c4 sol:AA-dskdatr.c4 opus:AA-dskdatr.c2 sol:AA-dskdatr.c2");
        ck.record("DECLINED the ~ of BLTADAT, BLTBDAT and BLTCDAT (SPECIFICATION.md 0.3, D1): with COPCON bit 1 set the pages disagree on what a Copper write does; C12 grades the bit-1-clear half and carries no claim id: opus:AA-bltadat.c5 sol:AA-bltadat.c5 opus:AA-bltbdat.c5 sol:AA-bltbdat.c5 opus:AA-bltcdat.c5 sol:AA-bltcdat.c5");
        ck.record("DECLINED the & of BPL2DAT-BPL8DAT (SPECIFICATION.md 0.3, D1): 'only' is contradicted by They may also be written by either micro; F6 grades the DMA part and carries no claim id: opus:AA-bpl2dat.c4 sol:AA-bpl2dat.c4 opus:AA-bpl3dat.c4 sol:AA-bpl3dat.c4 opus:AA-bpl4dat.c4 sol:AA-bpl4dat.c4 opus:AA-bpl5dat.c4 sol:AA-bpl5dat.c4 opus:AA-bpl6dat.c4 sol:AA-bpl6dat.c4 opus:AA-bpl7dat.c4 sol:AA-bpl7dat.c4 opus:AA-bpl8dat.c4 sol:AA-bpl8dat.c4");
        ck.record("DECLINED the & of AUD0DAT-AUD3DAT (SPECIFICATION.md 0.3, D1): 'only' is contradicted by The Processor can also write directly to this register; F7 grades the DMA part and carries the W claims only: opus:AA-aud0dat.c4 sol:AA-aud0dat.c4 opus:AA-aud1dat.c4 sol:AA-aud1dat.c4 opus:AA-aud2dat.c4 sol:AA-aud2dat.c4 opus:AA-aud3dat.c4 sol:AA-aud3dat.c4");
        ck.record("WITHDRAWN the ~ of DSKLEN (SPECIFICATION.md 0.5): C1 writes the 32 walking values of the 16 bits with the Copper and observes DSKBYTR bit 13, the mirror of DSKLEN bit 14 only, so bit 14 is the one bit whose effect is observed; bit 15 (DMAEN: its effect needs DSKEN, a second write and a disk DMA) and bits 13-0 (LENGTH: the length of that DMA, up to 16383 words) are not observed: opus:AA-dsklen.c4 sol:AA-dsklen.c4");
        ck.record("WITHDRAWN the ~ of VPOSW (SPECIFICATION.md 0.5): C3 writes the one value $8000 (V0 = $0000) and observes VPOSR bit 15, the frame flop LOF; bits 14-8 (chip identification), bit 7 (LOL, the line length) and bits 2-0 (V10-V8, which move the beam counter) are not written with 1 and not observed, the pages state no value for them: opus:AA-vposw.c4 sol:AA-vposw.c4");
        ck.record("WITHDRAWN the ~ of VHPOSW (SPECIFICATION.md 0.5): C4 writes the one value $C000 and observes V7 (VHPOSR bit 15) at the first poll past line $40; V6-V0 and H8-H1 are not observed, because the write moves the beam counter and the poll reads it while it runs on, so the pages give no value for them: opus:AA-vhposw.c4 sol:AA-vhposw.c4");
        ck.record("WITHDRAWN the ~ of POTGO (SPECIFICATION.md 0.5): C5 writes the 16 walking values of bits 15-8 (OUTRY, DATRY, OUTRX, DATRX, OUTLY, DATLY, OUTLX, DATLX) and observes the level of each driven pin in POTINP; bit 0 (START, the pot counters) and bits 7-1 are not written with 1 and not observed: opus:AA-potgo.c4 sol:AA-potgo.c4");
        ck.record("WITHDRAWN the ~ of BLTCON0 (SPECIFICATION.md 0.5): C7 writes BLTCON0 = $01FF against $0100 (USED and LF = $FF against LF = $00) and observes the four words written; the other bits are not observed one by one: a walking-one value has USED clear and the blit writes nothing, and ASH, USEA-USEC and LF7-LF0 need a set-up in which the value has USED set (BLTCON0L carries LF7-LF0 and the upper byte, see C7): opus:AA-bltcon0.c4 sol:AA-bltcon0.c4");
        ck.record("WITHDRAWN the ~ of BLTCON1 (SPECIFICATION.md 0.5): C8 writes the one value $0002 (DESC) and observes the direction of the two words written; BSH3-0, EFE, IFE, FCI and LINE are not written with 1 and not observed: LINE selects line drawing, which changes the meaning of BLTSIZE and of the pointers, and the shift and fill bits need other set-ups: opus:AA-bltcon1.c4 sol:AA-bltcon1.c4");
        ck.record("WITHDRAWN the ~ of BLTSIZV (SPECIFICATION.md 0.5): C11 writes the heights 1, 2, 4, ..., 128 (bits 7-0 one at a time) and observes the number of words written; bits 14-8 and the walking-zero values are not observed, because their heights (up to 32766 lines) make a blit longer than the frame in which the Copper list runs: opus:AA-bltsizv.c5 sol:AA-bltsizv.c5");
        ck.record("DECLINED the h mark of BLTSIZV, BLTSIZH and BLTCON0L (SPECIFICATION.md 0.4): whether a register is new or changed in the hires chips is not observable at the ports; F2 and F3 grade that the registers work with the AA chip set selected and carry no claim id: opus:AA-bltsizv.c4 sol:AA-bltsizv.c4 opus:AA-bltsizh.c4 sol:AA-bltsizh.c4 opus:AA-bltcon0l.c4 sol:AA-bltcon0l.c4");
        ck.record("DECLINED the & of BLTADAT, BLTBDAT, BLTCDAT, BLTDDAT and DSKDAT (SPECIFICATION.md 0.3, D1): 'only' is contradicted by it may also be preloaded by the microprocessor or is not observable; F8 and F9 grade the DMA part and carry no claim id: opus:AA-bltadat.c4 sol:AA-bltadat.c4 opus:AA-bltbdat.c4 sol:AA-bltbdat.c4 opus:AA-bltcdat.c4 sol:AA-bltcdat.c4 opus:AA-bltddat.c4 sol:AA-bltddat.c4 opus:AA-dskdat.c4 sol:AA-dskdat.c4");
    }
    std::printf("%d passed, %d failed\n", ck.passed, ck.failed);
    return ck.failed == 0 ? 0 : 1;
}
