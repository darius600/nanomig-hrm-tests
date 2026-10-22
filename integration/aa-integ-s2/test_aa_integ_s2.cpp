// test_aa_integ_s2.cpp
//
// aa-integ-s2: see SPECIFICATION.md. Blitter pointer and modulo registers
// (P1-P7) and BEAMCON0 (P8-P14), graded at the ports of nanomig.v only.
//
// Every run selects the AA chip set (chipset_config 0x10, a port input
// independent of the CPU core) and 2 MB of Chip memory, so both cores run AA and
// every proposition, precondition and anchor is graded (PASS:/FAIL:) on both.
//
// NMH_MUTATE selects one mutation in place of the normal run: it names the
// leading id of one assertion or precondition, and only the proposition that
// owns it runs, with a changed stimulus and the unchanged comparator. The
// normal run (NMH_MUTATE unset) never runs a mutant. A precondition whose run
// the mutation does not touch is not evaluated in that mutation run.
//
// NMH_ONLY=Pn runs the proposition Pn alone, without a mutation.
//
// Kinds: stimulus changes what a program or a register list writes, or the Config object every run is built from (CONFIG);
// the private build of SPECIFICATION.md 6.6 (a copy of the chip set made by private/*.patch, make private-uhres) is named where a
// record gives its result beside the repository build's. implementation green and implementation red run that copy, none
// impossible says why no stimulus can falsify the line at this boundary.
// MUTATION CONFIG stimulus red NMH_MUTATE=CONFIG: the Config object every run is built from is set to chipset_config 0x08 in place of 0x10 (P1 is the proposition that runs, built from that object); TG68K: FAIL CONFIG; NOT GRADED 5; fx68k: FAIL CONFIG; NOT GRADED 5; with NMH_ONLY=P8, P11, P12, P13 or P14 in place of P1: FAIL CONFIG; NOT GRADED 1 on each core; with NMH_ONLY=P9, P10 or F5: FAIL CONFIG; NOT GRADED 1 (one line for F5, P9 and P10) on each core
// MUTATION P1-FIXTURE stimulus red NMH_MUTATE=P1-FIXTURE: the program stores $DEAD in place of $BEEF; TG68K: FAIL P1-FIXTURE; NOT GRADED 5; fx68k: FAIL P1-FIXTURE; NOT GRADED 5
// MUTATION P1-ANCHOR stimulus red NMH_MUTATE=P1-ANCHOR: the control run keeps BLTEN set (DMACON $8240) in place of clearing it ($8200); TG68K: FAIL P1-ANCHOR; NOT GRADED 5; fx68k: FAIL P1-ANCHOR; NOT GRADED 5
// MUTATION P1-FINISHED stimulus red NMH_MUTATE=P1-FINISHED: BLTSIZE $00C2 (height 3) in place of $0082; TG68K: FAIL P1-FINISHED; NOT GRADED 5; fx68k: FAIL P1-FINISHED; NOT GRADED 5
// MUTATION P1-INC stimulus red NMH_MUTATE=P1-INC: the second blit is descending (BLTCON1 bit 1) with its four pointers $40 higher: its accesses stay in the channel regions with step -2, so the step is no longer one constant (prog_p1_desc); TG68K: FAIL P1-INC; NOT GRADED 5; fx68k: FAIL P1-INC; NOT GRADED 5
// MUTATION P1-A stimulus red NMH_MUTATE=P1-A: BLTAMOD written 28 in place of 24; TG68K: FAIL P1-A, P1; fx68k: FAIL P1-A, P1
// MUTATION P1-B stimulus red NMH_MUTATE=P1-B: the pointer written to BLTBPTH and BLTBPTL is $0D0100 in place of $0D0000; TG68K: FAIL P1-B, P1; fx68k: FAIL P1-B, P1
// MUTATION P1-C stimulus red NMH_MUTATE=P1-C: BLTCMOD written 12 in place of 8; TG68K: FAIL P1-C, P1; fx68k: FAIL P1-C, P1
// MUTATION P1-D stimulus red NMH_MUTATE=P1-D: the pointer written to BLTDPTH and BLTDPTL is $0F0100 in place of $0F0000; TG68K: FAIL P1-A, P1-B, P1-C, P1-D, P1; fx68k: FAIL P1-A, P1-B, P1-C, P1-D, P1
// MUTATION P1 stimulus red NMH_MUTATE=P1: the values written to BLTCMOD and BLTAMOD swapped (24 and 8); TG68K: FAIL P1-A, P1-C, P1; fx68k: FAIL P1-A, P1-C, P1
// MUTATION P2-FIXTURE stimulus red NMH_MUTATE=P2-FIXTURE: the program stores $DEAD in place of $BEEF; TG68K: FAIL P2-FIXTURE; NOT GRADED 6; fx68k: FAIL P2-FIXTURE; NOT GRADED 6
// MUTATION P2W-FIXTURE stimulus red NMH_MUTATE=P2W-FIXTURE: the walking-bit program stores $DEAD in place of $BEEF; TG68K: FAIL P2W-FIXTURE; NOT GRADED 3; fx68k: FAIL P2W-FIXTURE; NOT GRADED 3
// MUTATION P2-MAP stimulus red NMH_MUTATE=P2-MAP: the second pointer value $0AAAAA replaced by $0C0000, an address of the results area; TG68K: FAIL P2-MAP, P2-FINISHED; NOT GRADED 6; fx68k: FAIL P2-MAP, P2-FINISHED; NOT GRADED 6
// MUTATION P2W-MAP stimulus red NMH_MUTATE=P2W-MAP: the pointer of walking-bit blit 5 is $0C0000, an address of the results area (prog_p2w_map); TG68K: FAIL P2W-MAP, P2W-FINISHED; NOT GRADED 3; fx68k: FAIL P2W-MAP, P2W-FINISHED; NOT GRADED 3
// MUTATION P2-ANCHOR stimulus red NMH_MUTATE=P2-ANCHOR: the control run keeps BLTEN set (DMACON $8240) in place of clearing it ($8200); TG68K: FAIL P2-ANCHOR; NOT GRADED 6; fx68k: FAIL P2-ANCHOR; NOT GRADED 6
// MUTATION P2W-ANCHOR stimulus red NMH_MUTATE=P2W-ANCHOR: the control run keeps BLTEN set (DMACON $8240) in place of clearing it ($8200); TG68K: FAIL P2W-ANCHOR; NOT GRADED 3; fx68k: FAIL P2W-ANCHOR; NOT GRADED 3
// MUTATION P2-FINISHED stimulus red NMH_MUTATE=P2-FINISHED: BLTSIZE $00C1 (height 3) in place of $0041; TG68K: FAIL P2-FINISHED; NOT GRADED 6; fx68k: FAIL P2-FINISHED; NOT GRADED 6
// MUTATION P2W-FINISHED stimulus red NMH_MUTATE=P2W-FINISHED: BLTSIZE $00C1 (height 3) in place of $0041; TG68K: FAIL P2W-FINISHED; NOT GRADED 3; fx68k: FAIL P2W-FINISHED; NOT GRADED 3
// MUTATION P2-V1 stimulus red NMH_MUTATE=P2-V1: PTH written $0014 in place of $0015 in the V1 blits; TG68K: FAIL P2-V1, P2; fx68k: FAIL P2-V1, P2
// MUTATION P2-V2 stimulus red NMH_MUTATE=P2-V2: PTL written $AAA8 in place of $AAAA in the V2 blits; TG68K: FAIL P2-V2, P2; fx68k: FAIL P2-V2, P2
// MUTATION P2-PTH5 stimulus red NMH_MUTATE=P2-PTH5: PTH written $0025 in place of $0035 in the four contrast blits; TG68K: FAIL P2-PTH5, P2; fx68k: FAIL P2-PTH5, P2
// MUTATION P2-WALK1 stimulus red NMH_MUTATE=P2-WALK1: the walking-one blit of channel B for address bit 7 is written with the pointer of address bit 8 (prog_p2w_pos); TG68K: FAIL P2-WALK1, P2; fx68k: FAIL P2-WALK1, P2
// MUTATION P2-WALK0 stimulus red NMH_MUTATE=P2-WALK0: the walking-zero blit of channel C for address bit 12 is written with its pointer shifted up one bit (prog_p2w_pos0); TG68K: FAIL P2-WALK0, P2; fx68k: FAIL P2-WALK0, P2
// MUTATION P2 stimulus red NMH_MUTATE=P2: PTH written $0014 in place of $0015 in the V1 blits; TG68K: FAIL P2-V1, P2; fx68k: FAIL P2-V1, P2
// MUTATION P3-FIXTURE stimulus red NMH_MUTATE=P3-FIXTURE: the program stores $DEAD in place of $BEEF; TG68K: FAIL P3-FIXTURE; NOT GRADED 3; fx68k: FAIL P3-FIXTURE; NOT GRADED 3
// MUTATION P3-ANCHOR stimulus red NMH_MUTATE=P3-ANCHOR: the control run keeps BLTEN set (DMACON $8240) in place of clearing it ($8200); TG68K: FAIL P3-ANCHOR; NOT GRADED 3; fx68k: FAIL P3-ANCHOR; NOT GRADED 3
// MUTATION P3-FINISHED stimulus red NMH_MUTATE=P3-FINISHED: BLTSIZE $00C3 (height 3) in place of $0083; TG68K: FAIL P3-FINISHED; NOT GRADED 3; fx68k: FAIL P3-FINISHED; NOT GRADED 3
// MUTATION P3-ASC stimulus red NMH_MUTATE=P3-ASC: channel A's pointer $0E0200 in place of $0E0100 in the ascending blit; TG68K: FAIL P3-ASC, P3; fx68k: FAIL P3-ASC, P3
// MUTATION P3-DESC stimulus red NMH_MUTATE=P3-DESC: channel A's pointer $0E0200 in place of $0E0100 in the descending blit; TG68K: FAIL P3-DESC, P3; fx68k: FAIL P3-DESC, P3
// MUTATION P3 stimulus red NMH_MUTATE=P3: channel A's pointer $0E0200 in place of $0E0100 in both blits; TG68K: FAIL P3-ASC, P3-DESC, P3; fx68k: FAIL P3-ASC, P3-DESC, P3
// MUTATION P4-FIXTURE stimulus red NMH_MUTATE=P4-FIXTURE: the program stores $DEAD in place of $BEEF; TG68K: FAIL P4-FIXTURE; NOT GRADED 5; fx68k: FAIL P4-FIXTURE; NOT GRADED 5
// MUTATION P4-ANCHOR stimulus red NMH_MUTATE=P4-ANCHOR: the control run keeps BLTEN set (DMACON $8240) in place of clearing it ($8200); TG68K: FAIL P4-ANCHOR; NOT GRADED 5; fx68k: FAIL P4-ANCHOR; NOT GRADED 5
// MUTATION P4-FINISHED stimulus red NMH_MUTATE=P4-FINISHED: BLTEN clear in the run that must have finished its first blit; TG68K: FAIL P4-FINISHED; NOT GRADED 5; fx68k: FAIL P4-FINISHED; NOT GRADED 5
// MUTATION P4-INC none impossible unfalsifiable at this boundary: AA p.13 (BLTxMOD) adds the modulo only at the end of a line, so within a line of an area blit the step is the word size on every channel; BLTCON1 bit 1 (DESC, p.14) reverses the direction of every channel of a blit at once, so within one blit the step stays one constant, +2 or -2, and no stimulus makes it differ between lines or channels of that blit; the population of this line is one blit (the first blit of the run), whereas P1-INC and P5-INC measure one step over several blits, a descending one among ascending ones, and are falsifiable; line mode changes the population the clause is graded on
// MUTATION P4-A stimulus red NMH_MUTATE=P4-A: BLTAMOD written 14 in place of 6 in the first blit; TG68K: FAIL P4-A, P4; fx68k: FAIL P4-A, P4
// MUTATION P4-B stimulus red NMH_MUTATE=P4-B: BLTBMOD written 4 in place of -2 in the first blit; TG68K: FAIL P4-B, P4; fx68k: FAIL P4-B, P4
// MUTATION P4-C stimulus red NMH_MUTATE=P4-C: BLTCMOD written 4 in place of 0 in the first blit; TG68K: FAIL P4-C, P4; fx68k: FAIL P4-C, P4
// MUTATION P4-D stimulus red NMH_MUTATE=P4-D: BLTDMOD written 14 in place of 10 in the first blit; TG68K: FAIL P4-D, P4; fx68k: FAIL P4-D, P4
// MUTATION P4 stimulus red NMH_MUTATE=P4: BLTAMOD written 14 in place of 6 in the first blit; TG68K: FAIL P4-A, P4; fx68k: FAIL P4-A, P4
// MUTATION P5-FIXTURE stimulus red NMH_MUTATE=P5-FIXTURE: the program stores $DEAD in place of $BEEF; TG68K: FAIL P5-FIXTURE; NOT GRADED 5; fx68k: FAIL P5-FIXTURE; NOT GRADED 5
// MUTATION P5-REGIONS stimulus red NMH_MUTATE=P5-REGIONS: the modulo $3FFE of the set replaced by $7FFE; TG68K: FAIL P5-REGIONS; NOT GRADED 5; fx68k: FAIL P5-REGIONS; NOT GRADED 5
// MUTATION P5-ANCHOR stimulus red NMH_MUTATE=P5-ANCHOR: the control run keeps BLTEN set (DMACON $8240) in place of clearing it ($8200); TG68K: FAIL P5-ANCHOR; NOT GRADED 5; fx68k: FAIL P5-ANCHOR; NOT GRADED 5
// MUTATION P5-FINISHED stimulus red NMH_MUTATE=P5-FINISHED: BLTSIZE $0142 (height 5) in place of $0102; TG68K: FAIL P5-FINISHED; NOT GRADED 5; fx68k: FAIL P5-FINISHED; NOT GRADED 5
// MUTATION P5-INC stimulus red NMH_MUTATE=P5-INC: the third blit is descending (BLTCON1 bit 1) with its four pointers $10 higher: its accesses stay in the channel regions with step -2, so the step is no longer one constant (prog_p5_desc); TG68K: FAIL P5-INC; NOT GRADED 5; fx68k: FAIL P5-INC; NOT GRADED 5
// MUTATION P5-A stimulus red NMH_MUTATE=P5-A: the modulo of channel A in the first blit is $0006 in place of $0000; TG68K: FAIL P5-A, P5; fx68k: FAIL P5-A, P5
// MUTATION P5-B stimulus red NMH_MUTATE=P5-B: the modulo of channel B in the first blit is $0008 in place of $0006; TG68K: FAIL P5-B, P5; fx68k: FAIL P5-B, P5
// MUTATION P5-C stimulus red NMH_MUTATE=P5-C: the modulo of channel C in the first blit is $0004 in place of $FFFC; TG68K: FAIL P5-C, P5; fx68k: FAIL P5-C, P5
// MUTATION P5-D stimulus red NMH_MUTATE=P5-D: the modulo of channel D in the first blit is $3FFC in place of $3FFE; TG68K: FAIL P5-D, P5; fx68k: FAIL P5-D, P5
// MUTATION P5 stimulus red NMH_MUTATE=P5: the modulo $0000 of the set replaced by $0006; TG68K: FAIL P5-A, P5-B, P5-C, P5-D, P5; fx68k: FAIL P5-A, P5-B, P5-C, P5-D, P5
// MUTATION P6-FIXTURE stimulus red NMH_MUTATE=P6-FIXTURE: the program stores $DEAD in place of $BEEF; TG68K: FAIL P6-FIXTURE; NOT GRADED 3; fx68k: FAIL P6-FIXTURE; NOT GRADED 3
// MUTATION P6-ANCHOR stimulus red NMH_MUTATE=P6-ANCHOR: the control run keeps BLTEN set (DMACON $8240) in place of clearing it ($8200); TG68K: FAIL P6-ANCHOR; NOT GRADED 3; fx68k: FAIL P6-ANCHOR; NOT GRADED 3
// MUTATION P6-FINISHED stimulus red NMH_MUTATE=P6-FINISHED: BLTEN clear in the run that must have finished its blit; TG68K: FAIL P6-FINISHED; NOT GRADED 3; fx68k: FAIL P6-FINISHED; NOT GRADED 3
// MUTATION P6-INC none impossible unfalsifiable at this boundary: AA p.13 (BLTxMOD) adds the modulo only at the end of a line, so within a line of an area blit the step is the word size on every channel; BLTCON1 bit 1 (DESC, p.14) reverses the direction of every channel of a blit at once, so within one blit the step stays one constant, +2 or -2, and no stimulus makes it differ between lines or channels of that blit; the population of this line is one blit (the one blit of the run), whereas P1-INC and P5-INC measure one step over several blits, a descending one among ascending ones, and are falsifiable; line mode changes the population the clause is graded on
// MUTATION P6-STEPS stimulus red NMH_MUTATE=P6-STEPS: BLTBMOD written 12 in place of -4; TG68K: FAIL P6-STEPS, P6; fx68k: FAIL P6-STEPS, P6
// MUTATION P6-COUNT stimulus red NMH_MUTATE=P6-COUNT: BLTCON0 $0DF0 in place of $0FF0: channel C disabled; TG68K: FAIL P6-STEPS, P6-COUNT, P6; fx68k: FAIL P6-STEPS, P6-COUNT, P6
// MUTATION P6 stimulus red NMH_MUTATE=P6: BLTBMOD written 12 in place of -4; TG68K: FAIL P6-STEPS, P6; fx68k: FAIL P6-STEPS, P6
// MUTATION P7-FIXTURE stimulus red NMH_MUTATE=P7-FIXTURE: the program stores $DEAD in place of $BEEF; TG68K: FAIL P7-FIXTURE; NOT GRADED 27; fx68k: FAIL P7-FIXTURE; NOT GRADED 27
// MUTATION P7-FETCH stimulus red NMH_MUTATE=P7-FETCH: the Copper's MOVE to each register is a MOVE to COLOR00, COPCON bit 1 set; TG68K: FAIL P7-FETCH; NOT GRADED 15; fx68k: FAIL P7-FETCH; NOT GRADED 15
// MUTATION P7-COPER stimulus red NMH_MUTATE=P7-COPER: the MOVE to INTREQ in every list is a MOVE to the no-operation register; TG68K: FAIL P7-COPER; NOT GRADED 15; fx68k: FAIL P7-COPER; NOT GRADED 15
// MUTATION P7-BLIT stimulus red NMH_MUTATE=P7-BLIT: BLTEN clear for the final blit; TG68K: FAIL P7-BLIT; NOT GRADED 27; fx68k: FAIL P7-BLIT; NOT GRADED 27
// MUTATION P7-ANCHOR stimulus red NMH_MUTATE=P7-ANCHOR: the control run keeps BLTEN set (DMACON $8240) in place of clearing it ($8200); TG68K: FAIL P7-ANCHOR; NOT GRADED 27; fx68k: FAIL P7-ANCHOR; NOT GRADED 27
// MUTATION P7-0-PTH-A stimulus red NMH_MUTATE=P7-0-PTH-A: the CPU writes the Copper's value as the base value of BLTAPTH; TG68K: FAIL P7-0-PTH-A; NOT GRADED 15; fx68k: FAIL P7-0-PTH-A; NOT GRADED 15
// MUTATION P7-0-PTL-A stimulus red NMH_MUTATE=P7-0-PTL-A: the CPU writes the Copper's value as the base value of BLTAPTL; TG68K: FAIL P7-0-PTL-A; NOT GRADED 15; fx68k: FAIL P7-0-PTL-A; NOT GRADED 15
// MUTATION P7-0-MOD-A stimulus red NMH_MUTATE=P7-0-MOD-A: the CPU writes the Copper's value as the base value of BLTAMOD; TG68K: FAIL P7-0-MOD-A; NOT GRADED 15; fx68k: FAIL P7-0-MOD-A; NOT GRADED 15
// MUTATION P7-0-PTH-B stimulus red NMH_MUTATE=P7-0-PTH-B: the CPU writes the Copper's value as the base value of BLTBPTH; TG68K: FAIL P7-0-PTH-B; NOT GRADED 15; fx68k: FAIL P7-0-PTH-B; NOT GRADED 15
// MUTATION P7-0-PTL-B stimulus red NMH_MUTATE=P7-0-PTL-B: the CPU writes the Copper's value as the base value of BLTBPTL; TG68K: FAIL P7-0-PTL-B; NOT GRADED 15; fx68k: FAIL P7-0-PTL-B; NOT GRADED 15
// MUTATION P7-0-MOD-B stimulus red NMH_MUTATE=P7-0-MOD-B: the CPU writes the Copper's value as the base value of BLTBMOD; TG68K: FAIL P7-0-MOD-B; NOT GRADED 15; fx68k: FAIL P7-0-MOD-B; NOT GRADED 15
// MUTATION P7-0-PTH-C stimulus red NMH_MUTATE=P7-0-PTH-C: the CPU writes the Copper's value as the base value of BLTCPTH; TG68K: FAIL P7-0-PTH-C; NOT GRADED 15; fx68k: FAIL P7-0-PTH-C; NOT GRADED 15
// MUTATION P7-0-PTL-C stimulus red NMH_MUTATE=P7-0-PTL-C: the CPU writes the Copper's value as the base value of BLTCPTL; TG68K: FAIL P7-0-PTL-C; NOT GRADED 15; fx68k: FAIL P7-0-PTL-C; NOT GRADED 15
// MUTATION P7-0-MOD-C stimulus red NMH_MUTATE=P7-0-MOD-C: the CPU writes the Copper's value as the base value of BLTCMOD; TG68K: FAIL P7-0-MOD-C; NOT GRADED 15; fx68k: FAIL P7-0-MOD-C; NOT GRADED 15
// MUTATION P7-0-PTH-D stimulus red NMH_MUTATE=P7-0-PTH-D: the CPU writes the Copper's value as the base value of BLTDPTH; TG68K: FAIL P7-0-PTH-D; NOT GRADED 15; fx68k: FAIL P7-0-PTH-D; NOT GRADED 15
// MUTATION P7-0-PTL-D stimulus red NMH_MUTATE=P7-0-PTL-D: the CPU writes the Copper's value as the base value of BLTDPTL; TG68K: FAIL P7-0-PTL-D; NOT GRADED 15; fx68k: FAIL P7-0-PTL-D; NOT GRADED 15
// MUTATION P7-0-MOD-D stimulus red NMH_MUTATE=P7-0-MOD-D: the CPU writes the Copper's value as the base value of BLTDMOD; TG68K: FAIL P7-0-MOD-D; NOT GRADED 15; fx68k: FAIL P7-0-MOD-D; NOT GRADED 15
// MUTATION P7-1-PTH-A stimulus red NMH_MUTATE=P7-1-PTH-A: the Copper moves the base value to BLTAPTH; TG68K: FAIL P7-1-PTH-A; NOT GRADED 15; fx68k: FAIL P7-1-PTH-A; NOT GRADED 15
// MUTATION P7-1-PTL-A stimulus red NMH_MUTATE=P7-1-PTL-A: the Copper moves the base value to BLTAPTL; TG68K: FAIL P7-1-PTL-A; NOT GRADED 15; fx68k: FAIL P7-1-PTL-A; NOT GRADED 15
// MUTATION P7-1-MOD-A stimulus red NMH_MUTATE=P7-1-MOD-A: the Copper moves the base value to BLTAMOD; TG68K: FAIL P7-1-MOD-A; NOT GRADED 15; fx68k: FAIL P7-1-MOD-A; NOT GRADED 15
// MUTATION P7-1-PTH-B stimulus red NMH_MUTATE=P7-1-PTH-B: the Copper moves the base value to BLTBPTH; TG68K: FAIL P7-1-PTH-B; NOT GRADED 15; fx68k: FAIL P7-1-PTH-B; NOT GRADED 15
// MUTATION P7-1-PTL-B stimulus red NMH_MUTATE=P7-1-PTL-B: the Copper moves the base value to BLTBPTL; TG68K: FAIL P7-1-PTL-B; NOT GRADED 15; fx68k: FAIL P7-1-PTL-B; NOT GRADED 15
// MUTATION P7-1-MOD-B stimulus red NMH_MUTATE=P7-1-MOD-B: the Copper moves the base value to BLTBMOD; TG68K: FAIL P7-1-MOD-B; NOT GRADED 15; fx68k: FAIL P7-1-MOD-B; NOT GRADED 15
// MUTATION P7-1-PTH-C stimulus red NMH_MUTATE=P7-1-PTH-C: the Copper moves the base value to BLTCPTH; TG68K: FAIL P7-1-PTH-C; NOT GRADED 15; fx68k: FAIL P7-1-PTH-C; NOT GRADED 15
// MUTATION P7-1-PTL-C stimulus red NMH_MUTATE=P7-1-PTL-C: the Copper moves the base value to BLTCPTL; TG68K: FAIL P7-1-PTL-C; NOT GRADED 15; fx68k: FAIL P7-1-PTL-C; NOT GRADED 15
// MUTATION P7-1-MOD-C stimulus red NMH_MUTATE=P7-1-MOD-C: the Copper moves the base value to BLTCMOD; TG68K: FAIL P7-1-MOD-C; NOT GRADED 15; fx68k: FAIL P7-1-MOD-C; NOT GRADED 15
// MUTATION P7-1-PTH-D stimulus red NMH_MUTATE=P7-1-PTH-D: the Copper moves the base value to BLTDPTH; TG68K: FAIL P7-1-PTH-D; NOT GRADED 15; fx68k: FAIL P7-1-PTH-D; NOT GRADED 15
// MUTATION P7-1-PTL-D stimulus red NMH_MUTATE=P7-1-PTL-D: the Copper moves the base value to BLTDPTL; TG68K: FAIL P7-1-PTL-D; NOT GRADED 15; fx68k: FAIL P7-1-PTL-D; NOT GRADED 15
// MUTATION P7-1-MOD-D stimulus red NMH_MUTATE=P7-1-MOD-D: the Copper moves the base value to BLTDMOD; TG68K: FAIL P7-1-MOD-D; NOT GRADED 15; fx68k: FAIL P7-1-MOD-D; NOT GRADED 15
// MUTATION P7-PTH stimulus red NMH_MUTATE=P7-PTH: COPCON bit 1 clear in the run that grades the Copper's values; TG68K: FAIL P7-1-PTH-A, P7-1-PTL-A, P7-1-MOD-A, P7-1-PTH-B, P7-1-PTL-B, P7-1-MOD-B and 9 more; fx68k: FAIL P7-1-PTH-A, P7-1-PTL-A, P7-1-MOD-A, P7-1-PTH-B, P7-1-PTL-B, P7-1-MOD-B and 9 more
// MUTATION P7-PTL stimulus red NMH_MUTATE=P7-PTL: COPCON bit 1 clear in the run that grades the Copper's values; TG68K: FAIL P7-1-PTH-A, P7-1-PTL-A, P7-1-MOD-A, P7-1-PTH-B, P7-1-PTL-B, P7-1-MOD-B and 9 more; fx68k: FAIL P7-1-PTH-A, P7-1-PTL-A, P7-1-MOD-A, P7-1-PTH-B, P7-1-PTL-B, P7-1-MOD-B and 9 more
// MUTATION P7-MOD stimulus red NMH_MUTATE=P7-MOD: COPCON bit 1 clear in the run that grades the Copper's values; TG68K: FAIL P7-1-PTH-A, P7-1-PTL-A, P7-1-MOD-A, P7-1-PTH-B, P7-1-PTL-B, P7-1-MOD-B and 9 more; fx68k: FAIL P7-1-PTH-A, P7-1-PTL-A, P7-1-MOD-A, P7-1-PTH-B, P7-1-PTL-B, P7-1-MOD-B and 9 more
// MUTATION P8-FIXTURE stimulus red NMH_MUTATE=P8-FIXTURE: the program stores $DEAD in place of $BEEF; repository build: TG68K: FAIL P8-FIXTURE; NOT GRADED 1; fx68k: FAIL P8-FIXTURE; NOT GRADED 1
// MUTATION P8-ANCHOR stimulus red NMH_MUTATE=P8-ANCHOR: the dry run stores each sample below the one before (prog_lpen_rev); repository build: TG68K: FAIL P8-ANCHOR; NOT GRADED 6; NOT CHECKABLE 1; fx68k: FAIL P8-ANCHOR; NOT GRADED 6; NOT CHECKABLE 1
// MUTATION P8-FIT stimulus red NMH_MUTATE=P8-FIT: the pulse placed at the third sample, so that fewer than 4 samples fall before it; repository build: TG68K: FAIL P8-FIT; NOT GRADED 8; NOT CHECKABLE 1; fx68k: FAIL P8-FIT; NOT GRADED 8; NOT CHECKABLE 1
// MUTATION P8-TRIGGER stimulus red NMH_MUTATE=P8-TRIGGER: the pulse is scheduled and timed as in the normal run and the fire inputs are not driven (they stay released); repository build: TG68K: FAIL P8-TRIGGER; NOT GRADED 6; NOT CHECKABLE 1; fx68k: FAIL P8-TRIGGER; NOT GRADED 6; NOT CHECKABLE 1; private build of 6.6: TG68K: FAIL P8-TRIGGER; NOT GRADED 6; fx68k: FAIL P8-TRIGGER; NOT GRADED 6; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and PASS on TG68K and PASS on fx68k on the repository build
// MUTATION P8-ALIGN stimulus red NMH_MUTATE=P8-ALIGN: the pulse runs use the program with four more instructions in the sample loop (prog_lpen_slow); repository build: TG68K: FAIL P8-ALIGN; NOT GRADED 6; NOT CHECKABLE 1; fx68k: FAIL P8-ALIGN; NOT GRADED 6; NOT CHECKABLE 1; private build of 6.6: TG68K: FAIL P8-ALIGN; NOT GRADED 6; fx68k: FAIL P8-ALIGN; NOT GRADED 6; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and PASS on TG68K and PASS on fx68k on the repository build
// MUTATION P8-LPE stimulus red NMH_MUTATE=P8-LPE: BPLCON0 bit 3 set in the control run, so that the pulse freezes the samples again; repository build: TG68K: FAIL P8-0-VH, P8; NOT GRADED 3; NOT CHECKABLE 1; fx68k: FAIL P8-0-VH, P8; NOT GRADED 3; NOT CHECKABLE 1; private build of 6.6: TG68K: FAIL P8-LPE; NOT GRADED 6; fx68k: FAIL P8-LPE; NOT GRADED 6; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and PASS on TG68K and PASS on fx68k on the repository build
// MUTATION P8-HHRUN stimulus red NMH_MUTATE=P8-HHRUN: the dry run that must show HHPOSR running takes one sample, in which HHPOSR cannot take two values; repository build: TG68K: FAIL P8-0-VH, P8; NOT GRADED 3; NOT CHECKABLE 1; fx68k: FAIL P8-0-VH, P8; NOT GRADED 3; NOT CHECKABLE 1; private build of 6.6: TG68K: no FAIL; NOT GRADED 3; NOT CHECKABLE 1; fx68k: no FAIL; NOT GRADED 3; NOT CHECKABLE 1; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and NOT CHECKABLE on TG68K and NOT CHECKABLE on fx68k on the repository build; the private results of this line rest on an alternate counter that follows the main counter with DUAL clear, which the pages do not state: they are recorded and are not an implementation-green control
// MUTATION P8-0-VH stimulus red NMH_MUTATE=P8-0-VH: the P8-0 pulse is placed on line 256, after the first sample of the dry run on that line; repository build: TG68K: FAIL P8-0-VH, P8; NOT GRADED 3; NOT CHECKABLE 1; fx68k: FAIL P8-0-VH, P8; NOT GRADED 3; NOT CHECKABLE 1; private build of 6.6: TG68K: FAIL P8-0-VH, P8; fx68k: FAIL P8-0-VH, P8; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and FAIL on TG68K and FAIL on fx68k on the repository build; the private results of P8-0-HH, P8-1-HH and P8 in this record rest on an alternate counter that follows the main counter with DUAL clear, which the pages do not state: they are recorded and are not an implementation-green control
// MUTATION P8-0-HH stimulus red NMH_MUTATE=P8-0-HH: LPENDIS is set in the P8-0 run and clear in the P8-1 run in place of the other way round; repository build: TG68K: FAIL P8-0-VH, P8; NOT GRADED 3; NOT CHECKABLE 1; fx68k: FAIL P8-0-VH, P8; NOT GRADED 3; NOT CHECKABLE 1; private build of 6.6: TG68K: FAIL P8-0-VH, P8-0-HH, P8-1-VH, P8-1-HH, P8; fx68k: FAIL P8-0-VH, P8-0-HH, P8-1-VH, P8-1-HH, P8; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and NOT GRADED on TG68K and NOT GRADED on fx68k on the repository build; the private results of this line rest on an alternate counter that follows the main counter with DUAL clear, which the pages do not state: they are recorded and are not an implementation-green control
// MUTATION P8-1-VH stimulus red NMH_MUTATE=P8-1-VH: LPENDIS = 0 in the P8-1 run; repository build: TG68K: FAIL P8-0-VH, P8; NOT GRADED 3; NOT CHECKABLE 1; fx68k: FAIL P8-0-VH, P8; NOT GRADED 3; NOT CHECKABLE 1; private build of 6.6: TG68K: FAIL P8-1-VH, P8-1-HH, P8; fx68k: FAIL P8-1-VH, P8-1-HH, P8; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and PASS on TG68K and PASS on fx68k on the repository build; the private results of P8-0-HH, P8-1-HH and P8 in this record rest on an alternate counter that follows the main counter with DUAL clear, which the pages do not state: they are recorded and are not an implementation-green control
// MUTATION P8-1-HH stimulus red NMH_MUTATE=P8-1-HH: LPENDIS = 0 in the P8-1 run; repository build: TG68K: FAIL P8-0-VH, P8; NOT GRADED 3; NOT CHECKABLE 1; fx68k: FAIL P8-0-VH, P8; NOT GRADED 3; NOT CHECKABLE 1; private build of 6.6: TG68K: FAIL P8-1-VH, P8-1-HH, P8; fx68k: FAIL P8-1-VH, P8-1-HH, P8; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and NOT GRADED on TG68K and NOT GRADED on fx68k on the repository build; the private results of this line rest on an alternate counter that follows the main counter with DUAL clear, which the pages do not state: they are recorded and are not an implementation-green control
// MUTATION P8-VBREL stimulus red NMH_MUTATE=P8-VBREL: a second trigger at about line 30 of the next frame, so that the counters freeze again before the lines graded; repository build: TG68K: FAIL P8-0-VH, P8; NOT GRADED 3; NOT CHECKABLE 1; fx68k: FAIL P8-0-VH, P8; NOT GRADED 3; NOT CHECKABLE 1; the line itself is NOT GRADED there, on TG68K and on fx68k, since no counter freezes after the first trigger and there is no freeze to release; private build of 6.6: TG68K: FAIL P8-VBREL; fx68k: FAIL P8-VBREL; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k (the first sample after the wrap on line 20 on both), and NOT GRADED on TG68K and NOT GRADED on fx68k on the repository build; the copy of the private build without the release of the latch (make private-novbrelease) prints, on TG68K: FAIL P8-VBREL; on fx68k: FAIL P8-VBREL; the copy that releases the latch at line 45 in place of vertical position 20 (make private-laterelease) prints, on TG68K: FAIL P8-VBREL; on fx68k: FAIL P8-VBREL, with the first sample after the wrap on line 45 and 11 samples in lines 25-60 that run correctly, so that the line goes red through the bound on the first sample after the wrap alone
// MUTATION P8 stimulus red NMH_MUTATE=P8: LPENDIS = 0 in the P8-1 run; repository build: TG68K: FAIL P8-0-VH, P8; NOT GRADED 3; NOT CHECKABLE 1; fx68k: FAIL P8-0-VH, P8; NOT GRADED 3; NOT CHECKABLE 1; private build of 6.6: TG68K: FAIL P8-1-VH, P8-1-HH, P8; fx68k: FAIL P8-1-VH, P8-1-HH, P8; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and FAIL on TG68K and FAIL on fx68k on the repository build; the private results of P8-0-HH, P8-1-HH and P8 in this record rest on an alternate counter that follows the main counter with DUAL clear, which the pages do not state: they are recorded and are not an implementation-green control
// MUTATION F5-FIXTURE stimulus red NMH_MUTATE=F5-FIXTURE: the program stores $DEAD in place of $BEEF; TG68K: FAIL F5-FIXTURE, F5-LINES, F5-SV-INV; NOT CHECKABLE 3; fx68k: FAIL F5-FIXTURE, F5-LINES, F5-SV-INV; NOT CHECKABLE 3
// MUTATION F5-LINES stimulus red NMH_MUTATE=F5-LINES: a bit plane fetched in the S-H1 run; TG68K: FAIL F5-LINES, F5-ALIGN, F5-SV-INV; NOT CHECKABLE 3; fx68k: FAIL F5-LINES, F5-ALIGN, F5-SV-INV; NOT CHECKABLE 3
// MUTATION F5-ALIGN stimulus red NMH_MUTATE=F5-ALIGN: HTOTAL $00E0 in place of $00E2 in S-H2; TG68K: FAIL F5-ALIGN, F5-SV-INV; NOT CHECKABLE 3; fx68k: FAIL F5-ALIGN, F5-SV-INV; NOT CHECKABLE 3
// MUTATION F5-SH-DIFF none impossible not observable at this boundary (the scan doubler regenerates hs: nanomig.v dblscan 1'b1): the harness shows the horizontal sync only after the scan doubler, so the anchor S-H cannot hold at this boundary and no stimulus makes the line gradable
// MUTATION F5-SH-INV none impossible not observable at this boundary (the scan doubler regenerates hs: nanomig.v dblscan 1'b1): the harness shows the horizontal sync only after the scan doubler, so the anchor S-H cannot hold at this boundary and no stimulus makes the line gradable
// MUTATION F5-SV-DIFF stimulus red NMH_MUTATE=F5-SV-DIFF: S-V2 with VSSTOP 103, like S-V1; TG68K: FAIL F5-SV-DIFF, F5-SV-INV; NOT CHECKABLE 4; fx68k: FAIL F5-SV-DIFF, F5-SV-INV; NOT CHECKABLE 4
// MUTATION F5-SV-INV implementation green the private build of 6.6 makes the line PASS on TG68K and PASS on fx68k, where it is FAIL on TG68K and FAIL on fx68k on the repository build
// MUTATION P9-AH none impossible not observable at this boundary (the scan doubler regenerates hs: nanomig.v dblscan 1'b1): the harness shows the horizontal sync only after the scan doubler, so the anchor S-H cannot hold at this boundary and no stimulus makes the line gradable
// MUTATION P9-AC none impossible not observable at this boundary (the scan doubler regenerates hs: nanomig.v dblscan 1'b1): the harness shows the horizontal sync only after the scan doubler, so the anchor S-H cannot hold at this boundary and no stimulus makes the line gradable
// MUTATION P9-NH stimulus red NMH_MUTATE=P9-NH: VARVSYEN and VARHSYEN set in both runs of the pair (BEAMCON0 $03A0 in place of $00A0), so that HSSTRT is the horizontal position of the variable VSY on short fields; repository build: TG68K: FAIL F5-SV-INV, P9-NH, P9-BV, P9-B, P9-ID; NOT GRADED 3; NOT CHECKABLE 3; fx68k: FAIL F5-SV-INV, P9-NH, P9-BV, P9-B, P9-ID; NOT GRADED 3; NOT CHECKABLE 3; private build of 6.6: TG68K: FAIL P9-NH; NOT GRADED 3; NOT CHECKABLE 3; fx68k: FAIL P9-NH; NOT GRADED 3; NOT CHECKABLE 3; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and PASS on TG68K and PASS on fx68k on the repository build
// MUTATION P9-NC stimulus red NMH_MUTATE=P9-NC: VARVSYEN, VARHSYEN and interlace set in both runs of the pair (BEAMCON0 $03A0 in place of $00A0, BPLCON0 $0004), so that HCENTER is the horizontal position of the variable VSY on long fields; repository build: TG68K: FAIL F5-SV-INV, P9-NC, P9-BV, P9-B, P9-ID; NOT GRADED 3; NOT CHECKABLE 3; fx68k: FAIL F5-SV-INV, P9-NC, P9-BV, P9-B, P9-ID; NOT GRADED 3; NOT CHECKABLE 3; private build of 6.6: TG68K: FAIL P9-NC; NOT GRADED 3; NOT CHECKABLE 3; fx68k: FAIL P9-NC; NOT GRADED 3; NOT CHECKABLE 3; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and PASS on TG68K and PASS on fx68k on the repository build
// MUTATION P9-BV implementation green the private build of 6.6 makes the line PASS on TG68K and PASS on fx68k, where it is FAIL on TG68K and FAIL on fx68k on the repository build; with NMH_MUTATE=P9-BV (VBSTRT 150 in both runs of the pair) the private build prints TG68K: FAIL P9-BV, P9-B, P9-ID; NOT GRADED 3; NOT CHECKABLE 3; fx68k: FAIL P9-BV, P9-B, P9-ID; NOT GRADED 3; NOT CHECKABLE 3
// MUTATION P9-NV stimulus red NMH_MUTATE=P9-NV: CSCBEN set in both runs of the pair (BEAMCON0 $04A0 in place of $00A0); repository build: TG68K: FAIL F5-SV-INV, P9-BV, P9-B, P9-ID; NOT GRADED 3; NOT CHECKABLE 3; fx68k: FAIL F5-SV-INV, P9-BV, P9-B, P9-ID; NOT GRADED 3; NOT CHECKABLE 3; private build of 6.6: TG68K: FAIL P9-NV, P9-B, P9-ID; NOT GRADED 3; NOT CHECKABLE 3; fx68k: FAIL P9-NV, P9-B, P9-ID; NOT GRADED 3; NOT CHECKABLE 3; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and PASS on TG68K and PASS on fx68k on the repository build
// MUTATION P9-A none impossible not observable at this boundary (the scan doubler regenerates hs: nanomig.v dblscan 1'b1): the harness shows the horizontal sync only after the scan doubler, so the anchor S-H cannot hold at this boundary and no stimulus makes the line gradable
// MUTATION P9-B implementation green the private build of 6.6 makes the line PASS on TG68K and PASS on fx68k, where it is FAIL on TG68K and FAIL on fx68k on the repository build; with NMH_MUTATE=P9-B (VBSTRT 150 in both runs of the CSCBEN-set pair) the private build prints TG68K: FAIL P9-BV, P9-B, P9-ID; NOT GRADED 3; NOT CHECKABLE 3; fx68k: FAIL P9-BV, P9-B, P9-ID; NOT GRADED 3; NOT CHECKABLE 3
// MUTATION P9-ID implementation green the private build of 6.6 makes the line PASS on TG68K and PASS on fx68k, where it is FAIL on TG68K and FAIL on fx68k on the repository build; with NMH_MUTATE=P9-ID (VBSTRT 150 in both runs of the CSCBEN-set pair) the private build prints TG68K: FAIL P9-BV, P9-B, P9-ID; NOT GRADED 3; NOT CHECKABLE 3; fx68k: FAIL P9-BV, P9-B, P9-ID; NOT GRADED 3; NOT CHECKABLE 3
// MUTATION P10-CPAIR none impossible not observable at this boundary (the scan doubler regenerates hs: nanomig.v dblscan 1'b1): identity of the hs samples of the pair cannot show that the comparator HSY is independent of HCENTER, so the line is recorded and not graded; the VSY* samples of the pair are a MEASURED line only
// MUTATION P10-SPAIR none impossible not observable at this boundary (the scan doubler regenerates hs: nanomig.v dblscan 1'b1): the harness shows the horizontal sync only after the scan doubler, so the anchor S-H cannot hold at this boundary and no stimulus makes the line gradable
// MUTATION P10-VPAIR stimulus red NMH_MUTATE=P10-VPAIR: CSCBEN set and VARVSYEN clear in both runs of the pair (BEAMCON0 $04A0 in place of $06A0); repository build: TG68K: FAIL F5-SV-INV; NOT GRADED 3; NOT CHECKABLE 3; fx68k: FAIL F5-SV-INV; NOT GRADED 3; NOT CHECKABLE 3; private build of 6.6: TG68K: FAIL P10-VPAIR; NOT GRADED 3; NOT CHECKABLE 3; fx68k: FAIL P10-VPAIR; NOT GRADED 3; NOT CHECKABLE 3; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and PASS on TG68K and PASS on fx68k on the repository build
// MUTATION P10-VSPAIR stimulus red NMH_MUTATE=P10-VSPAIR: VSSTOP 103 in both runs of the pair; TG68K: FAIL F5-SV-INV, P10-VSPAIR; NOT GRADED 3; NOT CHECKABLE 3; fx68k: FAIL F5-SV-INV, P10-VSPAIR; NOT GRADED 3; NOT CHECKABLE 3
// MUTATION P10 none impossible not observable at this boundary (the scan doubler regenerates hs: nanomig.v dblscan 1'b1): the harness shows the horizontal sync only after the scan doubler, so the anchor S-H cannot hold at this boundary and no stimulus makes the line gradable
// MUTATION P11-FIXTURE stimulus red NMH_MUTATE=P11-FIXTURE: the program stores $DEAD in place of $BEEF; TG68K: FAIL P11-FIXTURE, P11-HHRUN; NOT GRADED 3; fx68k: FAIL P11-FIXTURE, P11-HHRUN; NOT GRADED 3
// MUTATION P11-ANCHOR stimulus red NMH_MUTATE=P11-ANCHOR: the samples are stored below the one before; TG68K: FAIL P11-ANCHOR, P11-HHRUN; NOT GRADED 3; fx68k: FAIL P11-ANCHOR, P11-HHRUN; NOT GRADED 3
// MUTATION P11-HHRUN implementation green the private build of 6.6 makes the line PASS on TG68K and PASS on fx68k, where it is FAIL on TG68K and FAIL on fx68k on the repository build
// MUTATION P11-MAXH stimulus red NMH_MUTATE=P11-MAXH: BEAMCON0 $08A0 (VARBEAMEN, LOLDIS, DUAL clear) with VTOTAL $0138 in place of $0060 in the contrast run, whose samples give the maximum H and whose stopped frame gives the port line of 57 colour clocks; HHPOSR liveness is taken from the unmodified DUAL run; repository build: TG68K: FAIL P11-HHRUN; NOT GRADED 3; fx68k: FAIL P11-HHRUN; NOT GRADED 3; private build of 6.6: TG68K: FAIL P11-MAXH, P11; fx68k: FAIL P11-MAXH, P11; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and NOT GRADED on TG68K and NOT GRADED on fx68k on the repository build
// MUTATION P11-HHMAX stimulus red NMH_MUTATE=P11-HHMAX: HTOTAL $0070 in place of $0038; repository build: TG68K: FAIL P11-HHRUN; NOT GRADED 3; fx68k: FAIL P11-HHRUN; NOT GRADED 3; private build of 6.6: TG68K: FAIL P11-HHMAX, P11; fx68k: FAIL P11-HHMAX, P11; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and NOT GRADED on TG68K and NOT GRADED on fx68k on the repository build
// MUTATION P11 stimulus red NMH_MUTATE=P11: HTOTAL $0070 in place of $0038 in the DUAL run; repository build: TG68K: FAIL P11-HHRUN; NOT GRADED 3; fx68k: FAIL P11-HHRUN; NOT GRADED 3; private build of 6.6: TG68K: FAIL P11-HHMAX, P11; fx68k: FAIL P11-HHMAX, P11; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and NOT GRADED on TG68K and NOT GRADED on fx68k on the repository build
// MUTATION P12-KFIXTURE stimulus red NMH_MUTATE=P12-KFIXTURE: the program of the DUAL run stores $DEAD in place of $BEEF; repository build: TG68K: FAIL P12-KFIXTURE; NOT GRADED 11; fx68k: FAIL P12-KFIXTURE; NOT GRADED 11
// MUTATION P12-KANCHOR stimulus red NMH_MUTATE=P12-KANCHOR: the samples of the DUAL run are stored below the one before; repository build: TG68K: FAIL P12-KANCHOR; NOT GRADED 11; fx68k: FAIL P12-KANCHOR; NOT GRADED 11; private build of 6.6: TG68K: FAIL P12-KANCHOR; NOT GRADED 11; fx68k: FAIL P12-KANCHOR; NOT GRADED 11; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and PASS on TG68K and PASS on fx68k on the repository build
// MUTATION P11-PERIOD implementation green the private build of 6.6 makes the line PASS on TG68K and PASS on fx68k, where it is NOT GRADED on TG68K and NOT GRADED on fx68k on the repository build; with NMH_MUTATE=P11-PERIOD (HTOTAL $0038 in place of $00E2 in the contrast run) the private build prints TG68K: FAIL P11-PERIOD; fx68k: FAIL P11-PERIOD
// MUTATION P12-FIXTURE stimulus red NMH_MUTATE=P12-FIXTURE: the program stores $DEAD in place of $BEEF; repository build: TG68K: FAIL P12-FIXTURE, P12-UHRES, P12-K; NOT GRADED 7; fx68k: FAIL P12-FIXTURE, P12-UHRES, P12-K; NOT GRADED 7; private build of 6.6: TG68K: FAIL P12-FIXTURE; NOT GRADED 7; fx68k: FAIL P12-FIXTURE; NOT GRADED 7; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and PASS on TG68K and PASS on fx68k on the repository build
// MUTATION P12-F3 stimulus red NMH_MUTATE=P12-F3: a lores bit plane fetched from byte address $000000 with BPL1MOD -40 and BPLCON0 $1080 (UHRES set), so that every line reads the same 20 words of the refresh addresses; repository build: TG68K: FAIL P12-F3, P12-K; NOT GRADED 9; fx68k: FAIL P12-F3, P12-K; NOT GRADED 9; private build of 6.6: TG68K: FAIL P12-F3; NOT GRADED 9; fx68k: FAIL P12-F3; NOT GRADED 9; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and PASS on TG68K and PASS on fx68k on the repository build
// MUTATION P12-UHRES stimulus red NMH_MUTATE=P12-UHRES: one lores bit plane fetched from the bit-plane region and a sprite with data in the sprite region in place of UHRES (BPLCON0 $1000), so that both regions are accessed on every graded line without the sprite, bit-plane, sprite pointer sequence at the start of a line; repository build: TG68K: FAIL P12-UHRES, P12-UHRESEN, P12-K; NOT GRADED 7; fx68k: FAIL P12-UHRES, P12-UHRESEN, P12-K; NOT GRADED 7; private build of 6.6: TG68K: FAIL P12-UHRES, P12-UHRESEN; NOT GRADED 7; fx68k: FAIL P12-UHRES, P12-UHRESEN; NOT GRADED 7; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and FAIL on TG68K and FAIL on fx68k on the repository build
// MUTATION P12-UHRESEN stimulus red NMH_MUTATE=P12-UHRESEN: BPLCON0 $0000 in place of $0080 in the P12-0 run; repository build: TG68K: FAIL P12-UHRES, P12-UHRESEN, P12-K; NOT GRADED 7; fx68k: FAIL P12-UHRES, P12-UHRESEN, P12-K; NOT GRADED 7; private build of 6.6: TG68K: FAIL P12-UHRES, P12-UHRESEN; NOT GRADED 7; fx68k: FAIL P12-UHRES, P12-UHRESEN; NOT GRADED 7; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and PASS on TG68K and PASS on fx68k on the repository build
// MUTATION P12-BANDWIDTH stimulus red NMH_MUTATE=P12-BANDWIDTH: a lores bit plane fetched from $0F8000 on lines 102-108, BPLCON0 $1080 (UHRES set); repository build: TG68K: FAIL P12-UHRES, P12-BANDWIDTH, P12-K; NOT GRADED 7; fx68k: FAIL P12-UHRES, P12-BANDWIDTH, P12-K; NOT GRADED 7; private build of 6.6: TG68K: FAIL P12-BANDWIDTH; NOT GRADED 7; fx68k: FAIL P12-BANDWIDTH; NOT GRADED 7; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and PASS on TG68K and PASS on fx68k on the repository build
// MUTATION P12-K implementation green the private build of 6.6 makes the line PASS on TG68K and PASS on fx68k, where it is FAIL on TG68K and FAIL on fx68k on the repository build; with NMH_MUTATE=P12-K (HTOTAL $00E2 in place of $0038 in the DUAL run of P11) the private build prints TG68K: FAIL P12-K; NOT GRADED 6; fx68k: FAIL P12-K; NOT GRADED 6
// MUTATION P12-0 stimulus red NMH_MUTATE=P12-0: DUAL set in the P12-0 run; repository build: TG68K: FAIL P12-UHRES, P12-K; NOT GRADED 7; fx68k: FAIL P12-UHRES, P12-K; NOT GRADED 7; private build of 6.6: TG68K: FAIL P12-0, P12; fx68k: FAIL P12-0, P12; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and NOT GRADED on TG68K and NOT GRADED on fx68k on the repository build
// MUTATION P12-1-BPL stimulus red NMH_MUTATE=P12-1-BPL: DUAL clear in the P12-1 run; repository build: TG68K: FAIL P12-UHRES, P12-K; NOT GRADED 7; fx68k: FAIL P12-UHRES, P12-K; NOT GRADED 7; private build of 6.6: TG68K: FAIL P12-1-BPL, P12-1-SPR, P12; fx68k: FAIL P12-1-BPL, P12-1-SPR, P12; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and NOT GRADED on TG68K and NOT GRADED on fx68k on the repository build
// MUTATION P12-1-SPR stimulus red NMH_MUTATE=P12-1-SPR: DUAL clear in the P12-1 run; repository build: TG68K: FAIL P12-UHRES, P12-K; NOT GRADED 7; fx68k: FAIL P12-UHRES, P12-K; NOT GRADED 7; private build of 6.6: TG68K: FAIL P12-1-BPL, P12-1-SPR, P12; fx68k: FAIL P12-1-BPL, P12-1-SPR, P12; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and NOT GRADED on TG68K and NOT GRADED on fx68k on the repository build
// MUTATION P12 stimulus red NMH_MUTATE=P12: DUAL clear in the P12-1 run; repository build: TG68K: FAIL P12-UHRES, P12-K; NOT GRADED 7; fx68k: FAIL P12-UHRES, P12-K; NOT GRADED 7; private build of 6.6: TG68K: FAIL P12-1-BPL, P12-1-SPR, P12; fx68k: FAIL P12-1-BPL, P12-1-SPR, P12; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and NOT GRADED on TG68K and NOT GRADED on fx68k on the repository build
// MUTATION P12-PHFIXTURE stimulus red NMH_MUTATE=P12-PHFIXTURE: the program that writes the alternate counter stores $DEAD in place of $BEEF; repository build: TG68K: FAIL P12-UHRES, P12-K; NOT GRADED 7; fx68k: FAIL P12-UHRES, P12-K; NOT GRADED 7; private build of 6.6: TG68K: FAIL P12-PHFIXTURE; NOT GRADED 3; fx68k: FAIL P12-PHFIXTURE; NOT GRADED 3; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and NOT GRADED on TG68K and NOT GRADED on fx68k on the repository build
// MUTATION P12-HHW stimulus red NMH_MUTATE=P12-HHW: the value $08 written to HHPOSW in the second run in place of $1C, the expected difference of the two values written staying $14; repository build: TG68K: FAIL P12-UHRES, P12-K; NOT GRADED 7; fx68k: FAIL P12-UHRES, P12-K; NOT GRADED 7; private build of 6.6: TG68K: FAIL P12-HHW; NOT GRADED 2; fx68k: FAIL P12-HHW; NOT GRADED 2; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and NOT GRADED on TG68K and NOT GRADED on fx68k on the repository build
// MUTATION P12-PHASE stimulus red NMH_MUTATE=P12-PHASE: DUAL clear in the second run, so that the chain starts at the normal counter's reset, P12-HHW being measured on a third run with DUAL set; repository build: TG68K: FAIL P12-UHRES, P12-K; NOT GRADED 7; fx68k: FAIL P12-UHRES, P12-K; NOT GRADED 7; private build of 6.6: TG68K: FAIL P12-PHASE, P12; fx68k: FAIL P12-PHASE, P12; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and NOT GRADED on TG68K and NOT GRADED on fx68k on the repository build
// MUTATION P13-FIXTURE stimulus red NMH_MUTATE=P13-FIXTURE: the program stores $DEAD in place of $BEEF; TG68K: FAIL P13-FIXTURE; NOT GRADED 3; fx68k: FAIL P13-FIXTURE; NOT GRADED 3
// MUTATION P13-F3 stimulus red NMH_MUTATE=P13-F3: a bit plane fetched in frame B; TG68K: FAIL P13-F3; NOT GRADED 3; fx68k: FAIL P13-F3; NOT GRADED 3
// MUTATION P13-F4 stimulus red NMH_MUTATE=P13-F4: the samples are stored below the one before; TG68K: FAIL P13-F4; NOT GRADED 3; fx68k: FAIL P13-F4; NOT GRADED 3
// MUTATION P13-VARMODE stimulus red NMH_MUTATE=P13-VARMODE: HTOTAL $00E2 in place of $00C0 in the contrast run; TG68K: FAIL P13-VARMODE; NOT GRADED 3; fx68k: FAIL P13-VARMODE; NOT GRADED 3
// MUTATION P13-LOLDIS stimulus red NMH_MUTATE=P13-LOLDIS: LOLDIS clear in the contrast run (BEAMCON0 $0080); TG68K: FAIL P13-LOLDIS; NOT GRADED 3; fx68k: FAIL P13-LOLDIS; NOT GRADED 3
// MUTATION P13-1-LINES stimulus red NMH_MUTATE=P13-1-LINES: PAL clear in the P13-1 run; repository build: TG68K: FAIL P13-1-LOL, P13; fx68k: FAIL P13-1-LOL, P13; private build of 6.6: TG68K: FAIL P13-1-LINES, P13-1-LOL, P13; fx68k: FAIL P13-1-LINES, P13-1-LOL, P13; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and PASS on TG68K and PASS on fx68k on the repository build
// MUTATION P13-1-LOL stimulus red NMH_MUTATE=P13-1-LOL: PAL clear in the P13-1 run; repository build: TG68K: FAIL P13-1-LOL, P13; fx68k: FAIL P13-1-LOL, P13
// MUTATION P13 stimulus red NMH_MUTATE=P13: PAL clear in the P13-1 run; repository build: TG68K: FAIL P13-1-LOL, P13; fx68k: FAIL P13-1-LOL, P13
// MUTATION P13-0-FIXTURE stimulus red NMH_MUTATE=P13-0-FIXTURE: the program of the PAL-clear run stores $DEAD in place of $BEEF; TG68K: FAIL P13-0-FIXTURE; NOT GRADED 3; fx68k: FAIL P13-0-FIXTURE; NOT GRADED 3
// MUTATION P13-0-F3 stimulus red NMH_MUTATE=P13-0-F3: a bit plane fetched in frame B of the PAL-clear run; TG68K: FAIL P13-0-F3; NOT GRADED 3; fx68k: FAIL P13-0-F3; NOT GRADED 3
// MUTATION P13-0-F4 stimulus red NMH_MUTATE=P13-0-F4: the samples of the PAL-clear run are stored below the one before; TG68K: FAIL P13-0-F4; NOT GRADED 3; fx68k: FAIL P13-0-F4; NOT GRADED 3
// MUTATION P13-0-LINES implementation green the private build of 6.6 makes the line PASS on TG68K and PASS on fx68k, where it is FAIL on TG68K and FAIL on fx68k on the repository build; with NMH_MUTATE=P13-0-LINES (PAL set in the P13-0 run) the private build prints TG68K: FAIL P13-0-LINES, P13-0-LOL; fx68k: FAIL P13-0-LINES, P13-0-LOL
// MUTATION P13-0-LOL stimulus red NMH_MUTATE=P13-0-LOL: PAL set in the P13-0 run; repository build: TG68K: FAIL P13-0-LINES, P13-0-LOL; fx68k: FAIL P13-0-LINES, P13-0-LOL; private build of 6.6: TG68K: FAIL P13-0-LINES, P13-0-LOL; fx68k: FAIL P13-0-LINES, P13-0-LOL; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and PASS on TG68K and PASS on fx68k on the repository build
// MUTATION P13-0-COUNT implementation green the private build of 6.6 makes the line PASS on TG68K and PASS on fx68k, where it is FAIL on TG68K and FAIL on fx68k on the repository build
// MUTATION P14-FIXTURE stimulus red NMH_MUTATE=P14-FIXTURE: the program stores $DEAD in place of $BEEF; TG68K: FAIL P14-FIXTURE; NOT GRADED 5; fx68k: FAIL P14-FIXTURE; NOT GRADED 5
// MUTATION P14-F3 stimulus red NMH_MUTATE=P14-F3: a bit plane fetched in the stopped frame; TG68K: FAIL P14-F3; NOT GRADED 5; fx68k: FAIL P14-F3; NOT GRADED 5
// MUTATION P14-F4 stimulus red NMH_MUTATE=P14-F4: the samples are stored below the one before; TG68K: FAIL P14-F4; NOT GRADED 5; fx68k: FAIL P14-F4; NOT GRADED 5
// MUTATION P14-STABLE stimulus red NMH_MUTATE=P14-STABLE: BPLCON0 LACE set: consecutive frames differ in length; TG68K: FAIL P14-STABLE; NOT GRADED 5; fx68k: FAIL P14-STABLE; NOT GRADED 5
// MUTATION P14-NORMAL stimulus red NMH_MUTATE=P14-NORMAL: BEAMCON0 $00A0 (VARBEAMEN set) in the PAL-set run; TG68K: FAIL P14-NORMAL; NOT GRADED 5; fx68k: FAIL P14-NORMAL; NOT GRADED 5
// MUTATION P14-0-LOL stimulus red NMH_MUTATE=P14-0-LOL: PAL set in the P14-0 run; repository build: TG68K: FAIL P14-0-LOL, P14-0-LINES, P14-1, P14; NOT GRADED 1; fx68k: FAIL P14-0-LOL, P14-0-LINES, P14-1, P14; NOT GRADED 1; private build of 6.6: TG68K: FAIL P14-0-LOL, P14-0-LINES, P14-1, P14; NOT GRADED 1; fx68k: FAIL P14-0-LOL, P14-0-LINES, P14-1, P14; NOT GRADED 1; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and PASS on TG68K and PASS on fx68k on the repository build
// MUTATION P14-0-LINES implementation green the private build of 6.6 makes the line PASS on TG68K and PASS on fx68k, where it is FAIL on TG68K and FAIL on fx68k on the repository build; with NMH_MUTATE=P14-0-LINES (PAL set in the P14-0 run) the private build prints TG68K: FAIL P14-0-LOL, P14-0-LINES, P14-1, P14; NOT GRADED 1; fx68k: FAIL P14-0-LOL, P14-0-LINES, P14-1, P14; NOT GRADED 1
// MUTATION P14-0-LONGONLY implementation red the private build of 6.6 with every line long (private/longline.patch, which differs from private/uhres.patch in the long-line toggle only; make private-longline): the normal run prints, on TG68K, FAIL P13-0-LINES, P13-0-LOL, P14-0-LOL, P14-0-LINES, P14-0-LONGONLY, P14; NOT GRADED 6; NOT CHECKABLE 3; on fx68k, FAIL P13-0-LINES, P13-0-LOL, P14-0-LOL, P14-0-LINES, P14-0-LONGONLY, P14; NOT GRADED 6; NOT CHECKABLE 3; the line is PASS on TG68K and PASS on fx68k in the normal run of the private build without that change
// MUTATION P14-1 stimulus red NMH_MUTATE=P14-1: PAL clear in the P14-1 run; repository build: TG68K: FAIL P14-0-LINES, P14-1, P14; NOT GRADED 1; fx68k: FAIL P14-0-LINES, P14-1, P14; NOT GRADED 1; private build of 6.6: TG68K: FAIL P14-1, P14; fx68k: FAIL P14-1, P14; the line is PASS in the normal run of the private build on TG68K and PASS on fx68k, and PASS on TG68K and PASS on fx68k on the repository build
// MUTATION P14 implementation green the private build of 6.6 makes the line PASS on TG68K and PASS on fx68k, where it is FAIL on TG68K and FAIL on fx68k on the repository build; with NMH_MUTATE=P14 (PAL clear in the P14-1 run) the private build prints TG68K: FAIL P14-1, P14; fx68k: FAIL P14-1, P14

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

// ---- nanomig.i and s2.i, restated: the harness side of the contract -------
constexpr uint32_t kResults   = 0x0C0000;
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
constexpr uint32_t kMarker    = 0x0C1000;
constexpr uint32_t kTable     = 0x080C00;   // blit.i: the blit table
constexpr uint16_t kMFrame    = 0xF001;
constexpr uint16_t kMBlitGo   = 0xA100;
constexpr uint16_t kMBlitDone = 0xA200;
constexpr uint32_t kProgLo = 0x080400, kProgHi = 0x081000;
constexpr uint32_t kStackLo = 0x0FFE00, kStackHi = 0x100000;

constexpr uint64_t kBootBudget = 8'000'000;

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

Checks checks;
std::string g_dir = ".";
const char* g_mutate = nullptr;

// Every proposition, precondition and anchor goes through Grader: PASS:/FAIL: on
// both cores.
class Grader {
public:
    void expect(bool cond, const std::string& label) { checks.expect(cond, label); }
    // An anchor the specification allows to fail: it prints PASS when it holds and
    // a NOT CHECKABLE record when it does not, never FAIL.
    void expect_anchor(bool ok, const std::string& label) {
        if (ok) expect(true, label);
        else checks.record("NOT CHECKABLE: " + label + " [the anchor does not hold in this harness]");
    }
    // A line whose precondition does not hold is NOT GRADED, never PASS or FAIL.
    void expect_if(bool pre, bool cond, const std::string& label) {
        if (pre) expect(cond, label);
        else checks.record("NOT GRADED: " + label + " [a precondition does not hold]");
    }
};
Grader g;

bool mutating(const char* label) { return g_mutate && std::string(g_mutate) == label; }

// Runs the proposition `prop` in a normal run, and in a mutation run only
// when the mutation names one of its ids.
bool want(const char* prop) {
    const char* only = std::getenv("NMH_ONLY");   // a single proposition, for a partial run
    if (only && std::string(only) != prop) return false;
    if (!g_mutate) return true;
    std::string m = g_mutate, p = prop;
    if (m.compare(0, 4, "P2W-") == 0) return p == "P2";   // the walking-bit lines belong to P2
    if (m == "CONFIG") return only ? true : p == "P1";   // the mutated configuration is the one a run is built from; with NMH_ONLY it reaches the proposition named
    return m == p || m.compare(0, p.size() + 1, p + "-") == 0;
}

// The precondition whose own run a mutation does not touch is not evaluated
// in that mutation run.
bool full(const char* label) { return !g_mutate || mutating(label); }

std::string sel(const char* normal, std::initializer_list<std::pair<const char*, const char*>> m) {
    for (const auto& p : m) if (mutating(p.first)) return p.second;
    return normal;
}

std::string hexs(uint32_t v, int digits = 6) {
    char b[16];
    std::snprintf(b, sizeof b, "%0*X", digits, v);
    return b;
}
std::string dec(long long v) { return std::to_string(v); }
std::string fnum(double v) { char b[32]; std::snprintf(b, sizeof b, "%.3f", v); return b; }

// ---- running a program -----------------------------------------------------

struct Acc { uint64_t cyc; uint32_t byte; bool wr; uint16_t data; };
struct Mark { uint64_t cyc; uint16_t code; };

struct RunData {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::vector<Acc> acc;          // every port access, held bus cycles collapsed
    std::vector<Mark> marks;       // every write to the marker area
    uint64_t period = 0;           // clk_sys cycles per port sample
    uint64_t ncycles = 0;          // the harness's cycle count when the run ended
    uint64_t pulse_start = 0, pulse_end = 0;
    uint64_t vid_t0 = 0;
    std::vector<uint8_t> vid;      // hs (bit 0) and vs (bit 1) per cycle from vid_t0, when kept
    std::map<uint32_t, uint16_t> mem;   // snapshot of the captured ranges, by byte address
    uint16_t word(uint32_t byte_addr) const {
        auto it = mem.find(byte_addr);
        return it == mem.end() ? 0 : it->second;
    }
};

nmh::Config make_config() {
    nmh::Config cfg;
    cfg.chipset_config = 0x10;     // AA selected (bit 4)
    cfg.memory_config  = 0x03;     // 2.0 MB Chip memory
    return cfg;
}
// The one Config object: every harness of every run is built from it, and CONFIG reads it.
nmh::Config g_cfg;
bool g_cfg_ok = false;

using Range = std::pair<uint32_t, uint32_t>;

// What a run needs beyond the program: parameters poked into Chip memory
// before the program starts, a light-pen pulse driven at a marker, and what
// to keep from the video capture.
struct RunOpts {
    std::string prog;
    uint64_t budget = 40'000'000;
    std::vector<Range> ranges;
    std::vector<std::pair<uint32_t, uint16_t>> pokes;    // byte address, word
    bool pulse = false;                  // drive both controller ports' fire inputs
    uint16_t pulse_marker = 0;
    uint64_t pulse_delay = 0, pulse_len = 0;
    uint64_t pulse2_delay = 0;           // a second pulse of the same length at the marker's time plus this, when non-zero
    bool pulse_drive = true;             // false: the pulse is scheduled and timed as usual, but the fire inputs are not written
    bool video_graded = false;           // keep hs/vs of the graded frame: the 2nd to the 3rd frame marker after the register list
};

std::shared_ptr<RunData> run_opts(const RunOpts& o) {
    auto r = std::make_shared<RunData>();
    nmh::NanomigHarness h(g_cfg);
    r->loaded = h.load_program(g_dir + "/" + o.prog + ".bin") > 0;
    if (!r->loaded) return r;
    for (const auto& p : o.pokes) h.poke_word(p.first / 2, p.second);
    r->released = h.boot(kBootBudget).cpu_released;
    if (o.pulse) {
        auto t = h.run_until_write(kMarker, o.pulse_marker, o.budget);
        if (t) {
            r->pulse_start = *t + o.pulse_delay;
            while (h.cycles() < r->pulse_start) h.run_cycles(1);
            if (o.pulse_drive) { h.ports()->joystick0 = 0x10; h.ports()->joystick1 = 0x10; }
            h.run_cycles(o.pulse_len);
            if (o.pulse_drive) { h.ports()->joystick0 = 0x00; h.ports()->joystick1 = 0x00; }
            r->pulse_end = h.cycles();
            if (o.pulse2_delay) {
                uint64_t s2 = *t + o.pulse2_delay;
                if (h.cycles() < s2) h.run_cycles(s2 - h.cycles());
                if (o.pulse_drive) { h.ports()->joystick0 = 0x10; h.ports()->joystick1 = 0x10; }
                h.run_cycles(o.pulse_len);
                if (o.pulse_drive) { h.ports()->joystick0 = 0x00; h.ports()->joystick1 = 0x00; }
            }
        }
    }
    r->done = h.run_until_write(kDoneAddr, kDoneValue, o.budget);
    const auto& log = h.ram_log();
    uint64_t period = 0;
    for (size_t i = 1; i < log.size(); ++i) {
        uint64_t d = log[i].cycle - log[i - 1].cycle;
        if (d > 0 && (period == 0 || d < period)) period = d;
    }
    r->period = period;
    std::vector<nmh::RamAccess> col = period ? nmh::coalesce_adjacent(log, period) : log;
    for (const auto& a : col) {
        r->acc.push_back({a.cycle, a.word_addr * 2, a.is_write, a.data});
        if (a.is_write && a.word_addr * 2 == kMarker) r->marks.push_back({a.cycle, a.data});
    }
    r->ncycles = h.cycles();
    auto ranges = o.ranges;
    ranges.push_back({kResults, kResults + 0x1000});
    ranges.push_back({kTable, kTable + 0x400});
    for (const auto& rg : ranges)
        for (uint32_t a = rg.first; a < rg.second; a += 2) {
            auto w = h.peek_logical(a);
            if (w) r->mem[a] = *w;
        }
    if (o.video_graded) {
        // the video samples (hs bit 0, vs bit 1) of the graded frame
        std::vector<uint64_t> fm;
        bool after = false;
        for (const auto& m : r->marks) {
            if (m.code == 0xF003) after = true;
            else if (after && m.code == 0xF001) fm.push_back(m.cyc);
        }
        if (fm.size() >= 3) {
            r->vid_t0 = fm[1];
            const auto& v = h.video();
            for (uint64_t c = fm[1]; c < fm[2] && c < v.size(); ++c)
                r->vid.push_back((v[c].hs ? 1 : 0) | (v[c].vs ? 2 : 0));
        }
    }
    return r;
}

std::shared_ptr<RunData> run_prog(const std::string& name, uint64_t run_budget = 40'000'000,
                                  std::vector<Range> ranges = {}) {
    RunOpts o;
    o.prog = name; o.budget = run_budget; o.ranges = ranges;
    return run_opts(o);
}

std::map<std::string, std::shared_ptr<RunData>> g_cache;
std::shared_ptr<RunData> run_cached(const std::string& name) {
    auto it = g_cache.find(name);
    if (it != g_cache.end()) return it->second;
    auto r = run_prog(name);
    g_cache[name] = r;
    return r;
}

// The first mark with `code` at or after index `from`; the result is its index.
std::optional<size_t> find_mark(const RunData& r, uint16_t code, size_t from = 0) {
    for (size_t i = from; i < r.marks.size(); ++i)
        if (r.marks[i].code == code) return i;
    return std::nullopt;
}

struct Win { bool ok = false; uint64_t t0 = 0, t1 = 0; };

Win blit_win(const RunData& r, int i) {
    Win w;
    auto gm = find_mark(r, kMBlitGo + i);
    if (!gm) return w;
    auto d = find_mark(r, kMBlitDone + i, *gm);
    if (!d) return w;
    w.ok = true; w.t0 = r.marks[*gm].cyc; w.t1 = r.marks[*d].cyc;
    return w;
}

// F1: the port accesses of a channel are those whose byte address lies in
// [lo,hi) between BLIT_GO and BLIT_DONE.
std::vector<Acc> accs_in(const RunData& r, const Win& w, uint32_t lo, uint32_t hi) {
    std::vector<Acc> out;
    if (!w.ok) return out;
    for (const auto& a : r.acc)
        if (a.cyc >= w.t0 && a.cyc <= w.t1 && a.byte >= lo && a.byte < hi) out.push_back(a);
    return out;
}

// F2: within every line of W accesses consecutive addresses differ by one
// constant step. Returns that step, or nullopt (none, or not constant).
std::optional<long long> line_step(const std::vector<Acc>& a, int W, int H) {
    std::optional<long long> inc;
    if (W < 2 || (int)a.size() != W * H) return std::nullopt;
    for (int k = 0; k < H; ++k)
        for (int j = 1; j < W; ++j) {
            long long d = (long long)a[k * W + j].byte - (long long)a[k * W + j - 1].byte;
            if (!inc) inc = d;
            else if (*inc != d) return std::nullopt;
        }
    return inc;
}

// The addresses the programs themselves use: the vector the level-3 interrupt
// fetches, the program image and its tables, the parameter, result and marker
// areas, the stack and the sample buffers. A port access outside them is a
// refresh access, a Blitter access or a DMA access (SPECIFICATION.md F3).
bool program_range(uint32_t a) {
    return (a >= 0x6C && a < 0x70) || (a >= 0x080000 && a < 0x082000) || (a >= 0x0B0000 && a < 0x0C2000) ||
           (a >= 0x0FE000 && a < 0x100000) || (a >= 0x1B0000 && a < 0x200000);
}

// ---- the blit programs: table readers and the run record -------------------

// Channel order in every array below: C, B, A, D.
constexpr int kC = 0, kB = 1, kA = 2, kD = 3;

struct Layout { uint32_t lo[4], hi[4]; };
// the regions of P1, P3, P4, P6
const Layout kLay1 = {{0x0B0000, 0x0D0000, 0x0E0000, 0x0F0000}, {0x0C0000, 0x0E0000, 0x0F0000, 0x0F8000}};
// the regions of P5
const Layout kLay5 = {{0x140000, 0x120000, 0x100000, 0x160000}, {0x160000, 0x140000, 0x120000, 0x180000}};

// What the program wrote, read back from its table in chip memory.
uint32_t tbl_ptr(const RunData& r, int blit, int ch) {
    uint32_t base = kTable + 2 + 32 * blit;
    return ((uint32_t)r.word(base + 2 * (4 + 2 * ch)) << 16) | r.word(base + 2 * (5 + 2 * ch));
}
long long tbl_mod(const RunData& r, int blit, int ch) {
    uint32_t base = kTable + 2 + 32 * blit;
    return (int16_t)r.word(base + 2 * (12 + ch));
}

struct BRun {
    std::shared_ptr<RunData> r;
    bool fixture = false;
    int n = 0;
    std::vector<Win> win;
    std::vector<bool> seen, bbusy_clear;
    std::vector<std::array<std::vector<Acc>, 4>> ch;   // [blit][channel]
};

BRun brun(const std::string& prog, int n, const Layout* lay, uint16_t fixval = 0xBEEF) {
    BRun b;
    b.r = run_cached(prog);
    b.n = n;
    b.fixture = b.r->loaded && b.r->done && b.r->word(kResults) == fixval;
    for (int i = 0; i < n; ++i) {
        b.win.push_back(blit_win(*b.r, i));
        b.seen.push_back(b.r->word(kResults + 0x10 + 4 * i) == 1);
        b.bbusy_clear.push_back((b.r->word(kResults + 0x12 + 4 * i) & 0x4000) == 0);
        std::array<std::vector<Acc>, 4> c;
        if (lay)
            for (int k = 0; k < 4; ++k) c[k] = accs_in(*b.r, b.win[i], lay->lo[k], lay->hi[k]);
        b.ch.push_back(c);
    }
    return b;
}

// The channel's accesses as the page predicts for a blit of W words and H
// lines: the first access at the pointer, every line start at the previous
// line's last access plus the increment plus the channel's modulo, every
// access in the expected direction.
bool chan_matches(const std::vector<Acc>& a, bool enabled, uint32_t start, long long mod, bool write,
                  int W, int H, long long inc) {
    if (!enabled) return a.empty();
    if ((int)a.size() != W * H) return false;
    if (a[0].byte != start) return false;
    for (const auto& x : a) if (x.wr != write) return false;
    for (int k = 1; k < H; ++k)
        if ((long long)a[k * W].byte != (long long)a[k * W - 1].byte + inc + mod) return false;
    return true;
}

std::string noteaddr(const std::vector<Acc>& a) {
    std::string s;
    for (size_t i = 0; i < a.size() && i < 12; ++i) s += (i ? "," : "") + hexs(a[i].byte);
    return s;
}

bool config_ok(const nmh::Config& c) { return c.chipset_config == 0x10 && c.memory_config == 0x03; }

// ---- P1 ---------------------------------------------------------------------

void proposition_p1() {
    if (!want("P1")) return;
    const int W = 2, H = 2;
    // [blit][channel C,B,A,D]
    const bool use[4][4] = {{false, false, true, true},    // P1-A: USEA, USED
                            {false, true, false, true},    // P1-B: USEB, USED
                            {true, false, false, true},    // P1-C: USEC, USED
                            {false, false, false, true}};  // P1-D: USED only
    std::string main_prog = sel("prog_p1", {{"P1-A", "prog_p1_moda"}, {"P1-B", "prog_p1_ptrb"},
                                            {"P1-C", "prog_p1_modc"}, {"P1-D", "prog_p1_ptrd"},
                                            {"P1", "prog_p1_swapmod"}, {"P1-INC", "prog_p1_desc"}});
    BRun mn = brun(main_prog, 4, &kLay1);

    BRun fx = mutating("P1-FIXTURE") ? brun("prog_p1_nofix", 4, &kLay1) : mn;
    g.expect(fx.fixture, "P1-FIXTURE: the program stores the constant $BEEF first and the harness reads it back before any other result is used");


    bool anchor = true;
    if (full("P1-ANCHOR")) {
        BRun c = brun(mutating("P1-ANCHOR") ? "prog_p1" : "prog_p1_nobl", 4, &kLay1);
        bool windows = true, empty = true;
        for (int i = 0; i < 4; ++i) {
            windows = windows && c.win[i].ok;
            for (int k = 0; k < 4; ++k) empty = empty && c.ch[i][k].empty();
        }
        anchor = c.fixture && windows && empty;
        g.expect(anchor, "P1-ANCHOR: with DMAEN set and BLTEN clear, the same four blits make no port access in any channel region between BLIT_GO and BLIT_DONE");
    } else checks.record("P1-ANCHOR: not evaluated in this mutation run");

    BRun fin = mutating("P1-FINISHED") ? brun("prog_p1_h3", 4, &kLay1) : mn;
    bool finished = fin.fixture;
    for (int i = 0; i < 4; ++i) {
        finished = finished && fin.win[i].ok && fin.seen[i] && fin.bbusy_clear[i];
        for (int k = 0; k < 4; ++k)
            finished = finished && fin.ch[i][k].size() == (use[i][k] ? (size_t)(W * H) : 0);
    }
    g.expect(finished, "P1-FINISHED: every blit raised BLIT, read BBUSY clear, and every enabled channel made exactly W*H = 4 accesses");

    std::optional<long long> inc;
    bool inc_const = mn.fixture;
    for (int i = 0; i < 4; ++i)
        for (int k = 0; k < 4; ++k) {
            if (!use[i][k]) continue;
            auto s = line_step(mn.ch[i][k], W, H);
            if (!s) { inc_const = false; continue; }
            if (!inc) inc = s;
            else if (*inc != *s) inc_const = false;
        }
    g.expect(inc_const && inc.has_value(), "P1-INC: within every line of every enabled channel consecutive addresses differ by one constant step");
    checks.note("P1 measured: increment = " + (inc ? dec(*inc) : std::string("n/a")) + " bytes");

    bool pre = fx.fixture && g_cfg_ok && anchor && finished && inc_const && inc.has_value();
    long long step = inc.value_or(0);

    // The pointers and modulos of the page's table: C 048/04A 060, B 04C/04E 062,
    // A 050/052 064, D 054/056 066.
    const uint32_t ptr[4] = {0x0B0000, 0x0D0000, 0x0E0000, 0x0F0000};
    const long long mod[4] = {8, 16, 24, 32};
    const bool wr[4] = {false, false, false, true};
    bool comp[4];
    for (int i = 0; i < 4; ++i) {
        comp[i] = mn.win[i].ok;
        for (int k = 0; k < 4; ++k)
            comp[i] = comp[i] && chan_matches(mn.ch[i][k], use[i][k], ptr[k], mod[k], wr[k], W, H, step);
    }
    g.expect_if(pre, comp[0], "P1-A: blit with USEA, USED: the accesses fall in the A region (pointer $0E0000) and the D region (pointer $0F0000) only, A reads start at $0E0000 and each line start is the previous line's last access plus the increment plus 24 (BLTAMOD), D writes start at $0F0000 with the line step increment plus 32 (BLTDMOD)");
    g.expect_if(pre, comp[1], "P1-B: blit with USEB, USED: the accesses fall in the B region (pointer $0D0000) and the D region only, B reads start at $0D0000 with the line step increment plus 16 (BLTBMOD), D writes as in P1-A");
    g.expect_if(pre, comp[2], "P1-C: blit with USEC, USED: the accesses fall in the C region (pointer $0B0000) and the D region only, C reads start at $0B0000 with the line step increment plus 8 (BLTCMOD), D writes as in P1-A");
    g.expect_if(pre, comp[3], "P1-D: blit with USED only: the writes fall in the D region only, start at $0F0000 and step the increment plus 32 per line, and no access falls in any other channel region");
    bool all = comp[0] && comp[1] && comp[2] && comp[3];
    g.expect_if(pre, all,
        "P1: opus:AA-bltxpth.c1 opus:AA-bltxptl.c1 opus:AA-bltxmod.c1 sol:AA-bltxpth.c1 sol:AA-bltxptl.c1 sol:AA-bltxmod.c1 sol:AA-bltxpth.c2 sol:AA-bltxptl.c2 sol:AA-bltxmod.c2 opus:AA-bltxpth.c9 sol:AA-bltxpt.c2 opus:AA-bltxmod.c5 sol:AA-bltxmod.c4: "
        "a write to each of the twelve registers (pointer pairs 048/04A C, 04C/04E B, 050/052 A, 054/056 D, modulos 060 C, 062 B, 064 A, 066 D) takes effect for its channel, and channels A, B and C read while channel D writes");
}

// ---- P2 ---------------------------------------------------------------------

void proposition_p2() {
    if (!want("P2")) return;
    // blit index -> channel under test (C,B,A,D index) and pointer value
    //  0 A V1, 1 A V2, 2 B V1, 3 B V2, 4 C V1, 5 C V2, 6 D V1, 7 D V2, 8-11 A,B,C,D with PTH bit 5
    const int chn[12] = {kA, kA, kB, kB, kC, kC, kD, kD, kA, kB, kC, kD};
    const uint32_t V1 = 0x155554, V2 = 0x0AAAAA;
    auto expect_ptr = [&](int i) { return (i < 8 && (i & 1)) ? V2 : V1; };
    const uint32_t kNeutralD = 0x0F0000;

    std::string main_prog = sel("prog_p2", {{"P2-V1", "prog_p2_v1bit"}, {"P2-V2", "prog_p2_v2bit"},
                                            {"P2-PTH5", "prog_p2_pth5bit"}, {"P2", "prog_p2_v1bit"},
                                            {"P2-MAP", "prog_p2_map"}});
    BRun mn = brun(main_prog, 12, nullptr);
    // The accesses of a blit are the port accesses between BLIT_GO and BLIT_DONE
    // that no program range explains. The neutral D region's are channel D's in
    // the blits of A, B and C; the others are the channel under test's.
    auto fill = [&](BRun& b) {
        for (int i = 0; i < 12; ++i) {
            std::vector<Acc> own, neutral;
            if (b.win[i].ok)
                for (const auto& a : b.r->acc) {
                    // refresh accesses (low addresses) are the chip set's, not the channel's
                    if (a.cyc < b.win[i].t0 || a.cyc > b.win[i].t1 || program_range(a.byte) || a.byte < 0x1000) continue;
                    if (chn[i] != kD && a.byte >= kNeutralD && a.byte < kNeutralD + 0x100) neutral.push_back(a);
                    else own.push_back(a);
                }
            b.ch[i][chn[i]] = own;
            if (chn[i] != kD) b.ch[i][kD] = neutral;
        }
    };
    fill(mn);

    BRun fx = mutating("P2-FIXTURE") ? brun("prog_p2_nofix", 12, nullptr) : mn;
    g.expect(fx.fixture, "P2-FIXTURE: the program stores the constant $BEEF first and the harness reads it back before any other result is used");


    // the program image, stack, markers and results do not overlap a region:
    // computed from the pointers the program wrote
    bool map_ok = true;
    for (int i = 0; i < 12; ++i) {
        uint32_t p = tbl_ptr(*mn.r, i, chn[i]);
        uint32_t spans[2][2] = {{p, p + 0x100}, {kNeutralD, kNeutralD + 0x100}};
        for (int s = 0; s < (chn[i] == kD ? 1 : 2); ++s) {
            uint32_t lo = spans[s][0], hi = spans[s][1];
            auto overlaps = [&](uint32_t a, uint32_t b) { return lo < b && a < hi; };
            if (overlaps(kProgLo, kProgHi) || overlaps(kResults, kMarker + 2) || overlaps(kStackLo, kStackHi)) map_ok = false;
        }
    }
    g.expect(map_ok, "P2-MAP: no channel region of any blit overlaps the program image, the results area, the marker area or the stack");

    bool anchor = true;
    if (full("P2-ANCHOR")) {
        BRun c = brun(mutating("P2-ANCHOR") ? "prog_p2" : "prog_p2_nobl", 12, nullptr);
        fill(c);
        bool windows = true, empty = true;
        for (int i = 0; i < 12; ++i) {
            windows = windows && c.win[i].ok;
            for (int k = 0; k < 4; ++k) empty = empty && c.ch[i][k].empty();
        }
        anchor = c.fixture && windows && empty;
        g.expect(anchor, "P2-ANCHOR: with DMAEN set and BLTEN clear, the same twelve blits make no port access in any channel region between BLIT_GO and BLIT_DONE");
    } else checks.record("P2-ANCHOR: not evaluated in this mutation run");

    BRun fin = mn;
    if (mutating("P2-FINISHED")) { fin = brun("prog_p2_h3", 12, nullptr); fill(fin); }
    bool finished = fin.fixture;
    for (int i = 0; i < 12; ++i) {
        finished = finished && fin.win[i].ok && fin.seen[i] && fin.bbusy_clear[i];
        finished = finished && fin.ch[i][chn[i]].size() == 1;
        if (chn[i] != kD) finished = finished && fin.ch[i][kD].size() == 1;
    }
    g.expect(finished, "P2-FINISHED: every blit raised BLIT, read BBUSY clear, and the channel under test and channel D each made exactly W*H = 1 access");

    bool pre = fx.fixture && g_cfg_ok && map_ok && anchor && finished;

    // V1 and V2: the access sits at the written address, for each channel
    auto one_at = [&](int i, uint32_t want_byte) {
        return mn.win[i].ok && mn.ch[i][chn[i]].size() == 1 && mn.ch[i][chn[i]][0].byte == want_byte;
    };
    bool v1 = true, v2 = true, p5 = true;
    for (int i = 0; i < 8; ++i) (i & 1 ? v2 : v1) = (i & 1 ? v2 : v1) && one_at(i, expect_ptr(i));
    for (int i = 8; i < 12; ++i) {
        auto hi = accs_in(*mn.r, mn.win[i], 0x355554, 0x355654);
        p5 = p5 && one_at(i, V1) && hi.empty();
    }
    std::string m;
    for (int i = 0; i < 12; ++i) m += (i ? " " : "") + hexs(mn.ch[i][chn[i]].empty() ? 0 : mn.ch[i][chn[i]][0].byte);
    checks.note("P2 measured: first access of the channel under test per blit: " + m);
    g.expect_if(pre, v1, "P2-V1: with the pointer written as PTH $0015, PTL $5554 (byte address $155554) each of channels A, B, C and D accesses word address $0AAAAA");
    g.expect_if(pre, v2, "P2-V2: with the pointer written as PTH $000A, PTL $AAAA (byte address $0AAAAA) each of channels A, B, C and D accesses word address $055555");
    g.expect_if(pre, p5, "P2-PTH5: with PTH bit 5 set in addition (PTH $0035, PTL $5554) each of channels A, B, C and D accesses the same word address $0AAAAA as with it clear, and nothing is accessed at byte $355554-$355653");
    // ---- walking one and walking zero: every address bit on every channel (prog_p2w) ----
    std::string wprog = sel("prog_p2w", {{"P2W-MAP", "prog_p2w_map"}, {"P2-WALK1", "prog_p2w_pos"},
                                         {"P2-WALK0", "prog_p2w_pos0"}});
    BRun wm = brun(wprog, 160, nullptr);
    const int wch[4] = {kA, kB, kC, kD};
    auto wcpu = [](uint32_t a) {
        return (a >= 0x080400 && a < 0x081000) || (a >= 0x0C0000 && a < 0x0C1002) || (a >= 0x0FFE00 && a < 0x0FFF00);
    };
    auto wfill = [&](BRun& b) {
        for (int i = 0; i < 160; ++i) {
            const int ch = wch[i / 40];
            std::vector<Acc> own, neutral;
            if (b.win[i].ok)
                for (const auto& a : b.r->acc) {
                    if (a.cyc < b.win[i].t0 || a.cyc > b.win[i].t1 || wcpu(a.byte) || a.byte == 0) continue;
                    if (ch != kD && a.wr && a.byte == kNeutralD) neutral.push_back(a);
                    else own.push_back(a);
                }
            b.ch[i][ch] = own;
            if (ch != kD) b.ch[i][kD] = neutral;
        }
    };
    wfill(wm);
    auto wptr = [&](const BRun& b, int i) {
        return ((uint32_t)b.r->word(kResults + 0x400 + 4 * i) << 16) | b.r->word(kResults + 0x402 + 4 * i);
    };
    auto wexpect = [&](int i) {
        const int pos = i % 20 + 1;
        return ((i % 40) < 20) ? (uint32_t)(1u << pos) : (uint32_t)(0x1FFFFEu ^ (1u << pos));
    };
    BRun wfx = mutating("P2W-FIXTURE") ? brun("prog_p2w_nofix", 160, nullptr) : wm;
    g.expect(wfx.fixture, "P2W-FIXTURE: the walking-bit program stores the constant $BEEF first and the harness reads it back before any other result is used");
    bool wmap_ok = true;
    for (int i = 0; i < 160; ++i) {
        uint32_t p = wptr(wm, i), lo = p, hi = p + 2;
        auto overlaps = [&](uint32_t a, uint32_t b2) { return lo < b2 && a < hi; };
        if (overlaps(kProgLo, kProgHi) || overlaps(kResults, kMarker + 2) || overlaps(0x0FFE00, 0x0FFF00)) wmap_ok = false;
    }
    g.expect(wmap_ok, "P2W-MAP: no pointer of any walking-bit blit addresses the program image, the results area, the marker area or the stack");
    bool wanchor = true;
    if (full("P2W-ANCHOR")) {
        // the sparse variant: address bits 1 and 20 of every channel and mode, 16 blits (a blit that makes no
        // memory cycle waits for the timeout of 2.2)
        BRun c = brun(mutating("P2W-ANCHOR") ? "prog_p2w_sparse" : "prog_p2w_nobl", 16, nullptr);
        bool windows = c.fixture, empty = true;
        std::string s;
        for (int i = 0; i < 16; ++i) {
            windows = windows && c.win[i].ok;
            if (!c.win[i].ok) continue;
            for (const auto& a : c.r->acc)
                if (a.cyc >= c.win[i].t0 && a.cyc <= c.win[i].t1 && !wcpu(a.byte) && a.byte != 0) {
                    empty = false;
                    if (s.size() < 60) s += " " + dec(i) + ":" + hexs(a.byte) + (a.wr ? "w" : "r");
                }
        }
        wanchor = windows && empty;
        if (!s.empty()) checks.note("P2W-ANCHOR measured, BLTEN clear: accesses (blit:byte address) that no CPU range explains:" + s);
        g.expect(wanchor, "P2W-ANCHOR: with DMAEN set and BLTEN clear, the 16 walking-bit blits of address bits 1 and 20 make no access outside the program's own ranges between BLIT_GO and BLIT_DONE");
    } else checks.record("P2W-ANCHOR: not evaluated in this mutation run");
    BRun wfin = wm;
    if (mutating("P2W-FINISHED")) { wfin = brun("prog_p2w_h3", 160, nullptr); wfill(wfin); }
    bool wfinished = wfin.fixture;
    for (int i = 0; i < 160; ++i) {
        const int ch = wch[i / 40];
        wfinished = wfinished && wfin.win[i].ok && wfin.seen[i] && wfin.bbusy_clear[i] && wfin.ch[i][ch].size() == 1;
        if (ch != kD) wfinished = wfinished && wfin.ch[i][kD].size() == 1;
    }
    g.expect(wfinished, "P2W-FINISHED: every walking-bit blit raised BLIT, read BBUSY clear, and the channel under test and channel D each made exactly W*H = 1 access");
    bool prew = pre && wfx.fixture && wmap_ok && wanchor && wfinished;
    bool w1 = true, w0 = true;
    std::string wm1, wm0;
    for (int i = 0; i < 160; ++i) {
        const int ch = wch[i / 40];
        bool good = wm.win[i].ok && wm.ch[i][ch].size() == 1 && wm.ch[i][ch][0].byte == wexpect(i) && wm.ch[i][ch][0].wr == (ch == kD);
        if ((i % 40) < 20) { w1 = w1 && good; if (!good) wm1 += " " + dec(i); }
        else { w0 = w0 && good; if (!good) wm0 += " " + dec(i); }
    }
    checks.note("P2 measured: walking-bit blits whose access is not at the expected address: walking one:" + (wm1.empty() ? std::string(" none") : wm1) +
                "; walking zero:" + (wm0.empty() ? std::string(" none") : wm0));
    g.expect_if(prew, w1, "P2-WALK1: with a pointer that has exactly one of the address bits 1-20 set (PTH bits 4-0 are address bits 20-16, PTL bits 15-1 address bits 15-1) each of channels A, B, C and D accesses the word whose address has exactly that bit set, for each of the 20 bits");
    g.expect_if(prew, w0, "P2-WALK0: with a pointer that has exactly one of the address bits 1-20 clear each of channels A, B, C and D accesses the word whose address has exactly that bit clear, for each of the 20 bits");
    g.expect_if(pre && prew, v1 && v2 && p5 && w1 && w0,
        "P2: opus:AA-bltxpth.c7 opus:AA-bltxptl.c6 opus:AA-bltxpth.c8 sol:AA-bltxpth.c4 sol:AA-bltxptl.c4 sol:AA-bltxpt.c1 opus:AA-bltxpth.c5 opus:AA-bltxptl.c4: "
        "a pointer pair is one 20-bit address, PTH bits 4-0 are address bits 20-16 and PTL bits 15-1 are address bits 15-1, for each of the four channels, and PTH bit 5 is not an address bit");
}

// ---- P3 ---------------------------------------------------------------------

void proposition_p3() {
    if (!want("P3")) return;
    const int W = 3, H = 2;
    const uint32_t ptr[4] = {0x0B0100, 0x0D0100, 0x0E0100, 0x0F0100};
    std::string main_prog = sel("prog_p3", {{"P3-ASC", "prog_p3_pointa0"}, {"P3-DESC", "prog_p3_pointa1"},
                                            {"P3", "prog_p3_pointa"}});
    BRun mn = brun(main_prog, 2, &kLay1);
    BRun fx = mutating("P3-FIXTURE") ? brun("prog_p3_nofix", 2, &kLay1) : mn;
    g.expect(fx.fixture, "P3-FIXTURE: the program stores the constant $BEEF first and the harness reads it back before any other result is used");

    bool anchor = true;
    if (full("P3-ANCHOR")) {
        BRun c = brun(mutating("P3-ANCHOR") ? "prog_p3" : "prog_p3_nobl", 2, &kLay1);
        bool windows = true, empty = true;
        for (int i = 0; i < 2; ++i) {
            windows = windows && c.win[i].ok;
            for (int k = 0; k < 4; ++k) empty = empty && c.ch[i][k].empty();
        }
        anchor = c.fixture && windows && empty;
        g.expect(anchor, "P3-ANCHOR: with DMAEN set and BLTEN clear, the same two blits make no port access in any channel region between BLIT_GO and BLIT_DONE");
    } else checks.record("P3-ANCHOR: not evaluated in this mutation run");

    BRun fin = mutating("P3-FINISHED") ? brun("prog_p3_h3", 2, &kLay1) : mn;
    bool finished = fin.fixture;
    for (int i = 0; i < 2; ++i) {
        finished = finished && fin.win[i].ok && fin.seen[i] && fin.bbusy_clear[i];
        for (int k = 0; k < 4; ++k) finished = finished && fin.ch[i][k].size() == (size_t)(W * H);
    }
    g.expect(finished, "P3-FINISHED: both blits raised BLIT, read BBUSY clear, and every channel made exactly W*H = 6 accesses");
    bool pre = fx.fixture && g_cfg_ok && anchor && finished;

    auto first_ok = [&](int blit) {
        bool ok = mn.win[blit].ok;
        std::string s;
        for (int k = 0; k < 4; ++k) {
            ok = ok && !mn.ch[blit][k].empty() && mn.ch[blit][k][0].byte == ptr[k];
            s += (k ? " " : "") + hexs(mn.ch[blit][k].empty() ? 0 : mn.ch[blit][k][0].byte);
        }
        checks.note(std::string("P3 measured: first access of C B A D in blit ") + (blit ? "P3-DESC: " : "P3-ASC: ") + s);
        return ok;
    };
    bool asc = first_ok(0), desc = first_ok(1);
    g.expect_if(pre, asc, "P3-ASC: in the ascending blit the first access of each of channels A, B, C and D is at the pointer written to it ($0E0100, $0D0100, $0B0100, $0F0100)");
    g.expect_if(pre, desc, "P3-DESC: in the descending blit (BLTCON1 bit 1) the first access of each of channels A, B, C and D is at the pointer written to it");
    g.expect_if(pre, asc && desc,
        "P3: opus:AA-bltxpth.c10 sol:AA-bltxpt.c3: after BLTSIZE is written the first access of each enabled channel is at the address its pointer pair was preloaded with, in both directions");
}

// ---- P4 ---------------------------------------------------------------------

void proposition_p4() {
    if (!want("P4")) return;
    const int W = 3, H = 2;
    const long long mod[4] = {0, -2, 6, 10};
    std::string main_prog = sel("prog_p4", {{"P4-A", "prog_p4_moda14"}, {"P4-B", "prog_p4_modb"},
                                            {"P4-C", "prog_p4_modc"}, {"P4-D", "prog_p4_modd"},
                                            {"P4", "prog_p4_moda14"}});
    BRun mn = brun(main_prog, 2, &kLay1);
    BRun fx = mutating("P4-FIXTURE") ? brun("prog_p4_nofix", 2, &kLay1) : mn;
    g.expect(fx.fixture, "P4-FIXTURE: the program stores the constant $BEEF first and the harness reads it back before any other result is used");

    bool anchor = true;
    if (full("P4-ANCHOR")) {
        BRun c = brun(mutating("P4-ANCHOR") ? "prog_p4" : "prog_p4_nobl", 2, &kLay1);
        bool windows = true, empty = true;
        for (int i = 0; i < 2; ++i) {
            windows = windows && c.win[i].ok;
            for (int k = 0; k < 4; ++k) empty = empty && c.ch[i][k].empty();
        }
        anchor = c.fixture && windows && empty;
        g.expect(anchor, "P4-ANCHOR: with DMAEN set and BLTEN clear, the same two blits make no port access in any channel region between BLIT_GO and BLIT_DONE");
    } else checks.record("P4-ANCHOR: not evaluated in this mutation run");

    BRun fin = mutating("P4-FINISHED") ? brun("prog_p4_nobl", 2, &kLay1) : mn;
    bool finished = fin.fixture && fin.win[0].ok && fin.seen[0] && fin.bbusy_clear[0];
    for (int k = 0; k < 4; ++k) finished = finished && fin.ch[0][k].size() == (size_t)(W * H);
    g.expect(finished, "P4-FINISHED: the first blit raised BLIT, read BBUSY clear, and every channel made exactly W*H = 6 accesses before the second blit started");

    std::optional<long long> inc;
    bool inc_const = mn.fixture;
    for (int k = 0; k < 4; ++k) {
        auto s = line_step(mn.ch[0][k], W, H);
        if (!s) { inc_const = false; continue; }
        if (!inc) inc = s;
        else if (*inc != *s) inc_const = false;
    }
    g.expect(inc_const && inc.has_value(), "P4-INC: within every line of the first blit consecutive addresses of every channel differ by one constant step");
    checks.note("P4 measured: increment = " + (inc ? dec(*inc) : std::string("n/a")) + " bytes");
    checks.record("P4-NOWRITE: that no pointer or modulo register is written between the two blits follows from the program text and is not visible at the port, so it is not graded");
    bool pre = fx.fixture && g_cfg_ok && anchor && finished && inc_const && inc.has_value();
    long long step = inc.value_or(0);

    bool ok[4];
    const char* nm[4] = {"C", "B", "A", "D"};
    for (int k = 0; k < 4; ++k) {
        const auto& a1 = mn.ch[0][k];
        const auto& a2 = mn.ch[1][k];
        ok[k] = mn.win[0].ok && mn.win[1].ok && !a1.empty() && !a2.empty() &&
                (long long)a2.front().byte == (long long)a1.back().byte + step + mod[k];
        checks.note(std::string("P4 measured: channel ") + nm[k] + " last access of blit 1 = " +
                    (a1.empty() ? "none" : hexs(a1.back().byte)) + ", first access of blit 2 = " +
                    (a2.empty() ? "none" : hexs(a2.front().byte)) + ", modulo written " + dec(mod[k]));
    }
    g.expect_if(pre, ok[kA], "P4-A: the first access of channel A in the second blit is the last access of the first blit plus the increment plus the modulo written (+6)");
    g.expect_if(pre, ok[kB], "P4-B: the first access of channel B in the second blit is the last access of the first blit plus the increment plus the modulo written (-2)");
    g.expect_if(pre, ok[kC], "P4-C: the first access of channel C in the second blit is the last access of the first blit plus the increment plus the modulo written (0)");
    g.expect_if(pre, ok[kD], "P4-D: the first access of channel D in the second blit is the last access of the first blit plus the increment plus the modulo written (+10)");
    g.expect_if(pre, ok[0] && ok[1] && ok[2] && ok[3],
        "P4: opus:AA-bltxpth.c11 sol:AA-bltxpt.c4: when a blit has finished each enabled channel's pointer holds the channel's last data address plus the increment plus its modulo, shown by a second blit that writes no pointer or modulo register");
}

// ---- P5 ---------------------------------------------------------------------

void proposition_p5() {
    if (!want("P5")) return;
    const int W = 2, H = 4;
    const long long vals[5] = {0, 6, -4, 16382, -16382};
    std::string main_prog = sel("prog_p5", {{"P5-A", "prog_p5_moda"}, {"P5-B", "prog_p5_modb"},
                                            {"P5-C", "prog_p5_modc"}, {"P5-D", "prog_p5_modd"},
                                            {"P5", "prog_p5_mod0to6"}, {"P5-INC", "prog_p5_desc"}});
    BRun mn = brun(main_prog, 5, &kLay5);
    BRun fx = mutating("P5-FIXTURE") ? brun("prog_p5_nofix", 5, &kLay5) : mn;
    g.expect(fx.fixture, "P5-FIXTURE: the program stores the constant $BEEF first and the harness reads it back before any other result is used");

    // the largest excursion of every channel stays inside its region, from the
    // pointers and modulos the program wrote
    bool regions_ok = true;
    {
        BRun big = mutating("P5-REGIONS") ? brun("prog_p5_bigmod", 5, &kLay5) : mn;
        for (int i = 0; i < 5; ++i)
            for (int k = 0; k < 4; ++k) {
                long long p = tbl_ptr(*big.r, i, k), m = tbl_mod(*big.r, i, k);
                for (int line = 0; line < H; ++line) {
                    long long first = p + line * (2 * W + m), last = first + 2 * (W - 1);
                    if (first < (long long)kLay5.lo[k] || last >= (long long)kLay5.hi[k]) regions_ok = false;
                }
            }
    }
    g.expect(regions_ok, "P5-REGIONS: the largest excursion of every channel in every blit, computed from the pointer and modulo the program wrote, stays inside that channel's region");

    bool anchor = true;
    if (full("P5-ANCHOR")) {
        BRun c = brun(mutating("P5-ANCHOR") ? "prog_p5" : "prog_p5_nobl", 5, &kLay5);
        bool windows = true, empty = true;
        for (int i = 0; i < 5; ++i) {
            windows = windows && c.win[i].ok;
            for (int k = 0; k < 4; ++k) empty = empty && c.ch[i][k].empty();
        }
        anchor = c.fixture && windows && empty;
        g.expect(anchor, "P5-ANCHOR: with DMAEN set and BLTEN clear, the same five blits make no port access in any channel region between BLIT_GO and BLIT_DONE");
    } else checks.record("P5-ANCHOR: not evaluated in this mutation run");

    BRun fin = mutating("P5-FINISHED") ? brun("prog_p5_h5", 5, &kLay5) : mn;
    bool finished = fin.fixture;
    for (int i = 0; i < 5; ++i) {
        finished = finished && fin.win[i].ok && fin.seen[i] && fin.bbusy_clear[i];
        for (int k = 0; k < 4; ++k) finished = finished && fin.ch[i][k].size() == (size_t)(W * H);
    }
    g.expect(finished, "P5-FINISHED: every blit raised BLIT, read BBUSY clear, and every channel made exactly W*H = 8 accesses");

    std::optional<long long> inc;
    bool inc_const = mn.fixture;
    for (int i = 0; i < 5; ++i)
        for (int k = 0; k < 4; ++k) {
            auto s = line_step(mn.ch[i][k], W, H);
            if (!s) { inc_const = false; continue; }
            if (!inc) inc = s;
            else if (*inc != *s) inc_const = false;
        }
    g.expect(inc_const && inc.has_value(), "P5-INC: within every line of every channel in every blit consecutive addresses differ by one constant step");
    checks.note("P5 measured: increment = " + (inc ? dec(*inc) : std::string("n/a")) + " bytes");
    bool pre = fx.fixture && g_cfg_ok && regions_ok && anchor && finished && inc_const && inc.has_value();
    long long step = inc.value_or(0);

    // blit j gives A, B, C, D the values with indices j, j+1, j+2, j+3 (mod 5)
    const uint32_t start[4] = {0x150000, 0x130000, 0x110000, 0x170000};
    bool ok[4] = {true, true, true, true};
    int boundaries = 0;
    for (int j = 0; j < 5; ++j)
        for (int k = 0; k < 4; ++k) {
            int idx = (k == kA ? j : k == kB ? j + 1 : k == kC ? j + 2 : j + 3) % 5;
            long long m = vals[idx];
            const auto& a = mn.ch[j][k];
            bool good = mn.win[j].ok && (int)a.size() == W * H && a[0].byte == start[k];
            for (int line = 1; good && line < H; ++line) {
                ++boundaries;
                good = (long long)a[line * W].byte == (long long)a[line * W - 1].byte + step + m;
            }
            ok[k] = ok[k] && good;
        }
    checks.note("P5 measured: " + dec(boundaries) + " line boundaries compared");
    g.expect_if(pre, ok[kA], "P5-A: at each of the 3 line boundaries of the 5 blits channel A's next line starts at the last address of the line plus the increment plus its modulo (0, +6, -4, +16382, -16382 in turn)");
    g.expect_if(pre, ok[kB], "P5-B: at each of the 3 line boundaries of the 5 blits channel B's next line starts at the last address of the line plus the increment plus its modulo (0, +6, -4, +16382, -16382 in turn)");
    g.expect_if(pre, ok[kC], "P5-C: at each of the 3 line boundaries of the 5 blits channel C's next line starts at the last address of the line plus the increment plus its modulo (0, +6, -4, +16382, -16382 in turn)");
    g.expect_if(pre, ok[kD], "P5-D: at each of the 3 line boundaries of the 5 blits channel D's next line starts at the last address of the line plus the increment plus its modulo (0, +6, -4, +16382, -16382 in turn)");
    g.expect_if(pre, ok[0] && ok[1] && ok[2] && ok[3],
        "P5: opus:AA-bltxmod.c6 sol:AA-bltxmod.c5 sol:AA-bltxmod.c6: at the end of every line the channel's modulo, sign-extended, is added so that the next line starts at the last address plus the increment plus the modulo, for positive, zero and negative modulos on every channel (60 boundaries)");
}

// ---- P6 ---------------------------------------------------------------------

void proposition_p6() {
    if (!want("P6")) return;
    const int W = 2, H = 3;
    const long long mod[4] = {0, -4, 8, 20};
    std::string main_prog = sel("prog_p6", {{"P6-STEPS", "prog_p6_modb12"}, {"P6-COUNT", "prog_p6_nousec"},
                                            {"P6", "prog_p6_modb12"}});
    BRun mn = brun(main_prog, 1, &kLay1);
    BRun fx = mutating("P6-FIXTURE") ? brun("prog_p6_nofix", 1, &kLay1) : mn;
    g.expect(fx.fixture, "P6-FIXTURE: the program stores the constant $BEEF first and the harness reads it back before any other result is used");

    bool anchor = true;
    if (full("P6-ANCHOR")) {
        BRun c = brun(mutating("P6-ANCHOR") ? "prog_p6" : "prog_p6_nobl", 1, &kLay1);
        bool empty = c.win[0].ok;
        for (int k = 0; k < 4; ++k) empty = empty && c.ch[0][k].empty();
        anchor = c.fixture && empty;
        g.expect(anchor, "P6-ANCHOR: with DMAEN set and BLTEN clear, the same blit makes no port access in any channel region between BLIT_GO and BLIT_DONE");
    } else checks.record("P6-ANCHOR: not evaluated in this mutation run");

    BRun fin = mutating("P6-FINISHED") ? brun("prog_p6_nobl", 1, &kLay1) : mn;
    bool finished = fin.fixture && fin.win[0].ok && fin.seen[0] && fin.bbusy_clear[0];
    g.expect(finished, "P6-FINISHED: the blit raised BLIT and BBUSY read clear");
    bool pre = fx.fixture && g_cfg_ok && anchor && finished;

    // the increment is measured on the channels that make W*H accesses; how many
    // accesses each channel makes is what P6-COUNT grades
    std::optional<long long> inc;
    bool inc_ok = true;
    for (int k = 0; k < 4; ++k) {
        auto s = line_step(mn.ch[0][k], W, H);
        if (!s) continue;
        if (!inc) inc = s;
        else if (*inc != *s) inc_ok = false;
    }
    g.expect(inc_ok && inc.has_value(), "P6-INC: within every line of the channels that make W*H = 6 accesses, consecutive addresses differ by one constant step, the same for every such channel");
    long long step = inc.value_or(0);
    pre = pre && inc_ok && inc.has_value();

    bool steps = mn.win[0].ok;
    bool counts = mn.win[0].ok;
    std::string m;
    for (int k = 0; k < 4; ++k) {
        const auto& a = mn.ch[0][k];
        counts = counts && (int)a.size() == W * H;
        for (int line = 1; line < H; ++line) {
            bool good = (int)a.size() == W * H &&
                        (long long)a[line * W].byte - (long long)a[line * W - 1].byte == step + mod[k];
            steps = steps && good;
        }
        m += " " + dec(a.size());
    }
    checks.note("P6 measured: accesses per channel C B A D:" + m);
    g.expect_if(pre, steps, "P6-STEPS: at both line boundaries each channel's line step is the increment plus its own modulo (C 0, B -4, A +8, D +20): four different steps");
    g.expect_if(pre, counts, "P6-COUNT: every channel makes exactly 6 accesses, 2 per line");
    g.expect_if(pre, steps && counts,
        "P6: opus:AA-bltxmod.c7 sol:AA-bltxmod.c7 sol:AA-bltxmod.c8: each source or destination uses its own modulo, and an identical area of each is used in the blit");
}

// ---- P7 ---------------------------------------------------------------------

constexpr uint32_t kLists = 0x0A0000;   // list k (0-based) at kLists + 16*k
// registers written by the lists, in list order: A PTH, A PTL, A MOD, B ..., C ..., D ...
constexpr uint16_t kP7Reg[12] = {0x050, 0x052, 0x064, 0x04C, 0x04E, 0x062, 0x048, 0x04A, 0x060, 0x054, 0x056, 0x066};
const char* const kP7Name[12] = {"BLTAPTH", "BLTAPTL", "BLTAMOD", "BLTBPTH", "BLTBPTL", "BLTBMOD",
                                 "BLTCPTH", "BLTCPTL", "BLTCMOD", "BLTDPTH", "BLTDPTL", "BLTDMOD"};
// channel order C,B,A,D: both the base range and the range of the Copper's values
constexpr uint32_t kP7Lo[4][2] = {{0x0B0000, 0x130000}, {0x0D0000, 0x120000}, {0x0E0000, 0x110000}, {0x0F0000, 0x140000}};
constexpr uint32_t kP7Hi[4][2] = {{0x0C0000, 0x140000}, {0x0E0000, 0x130000}, {0x0F0000, 0x120000}, {0x0F8000, 0x150000}};

struct P7Run {
    std::shared_ptr<RunData> r;
    bool fixture = false;
    bool coper[12] = {};
    bool fetched[12] = {};
    Win win;
    bool seen = false, bbusy_clear = false;
    std::array<std::vector<Acc>, 4> ch;
    // effective register contents read from the blit, per list index
    std::optional<long long> inc;
    long long val[12] = {};
    bool val_ok[12] = {};
    bool blit_counts = false;      // 4 accesses per channel
};

P7Run p7_run(const std::string& prog) {
    P7Run p;
    p.r = run_cached(prog);
    p.fixture = p.r->loaded && p.r->done && p.r->word(kResults) == 0xBEEF;
    for (int k = 0; k < 12; ++k) {
        p.coper[k] = p.r->word(kResults + 0x40 + 2 * k) == 1;
        uint32_t a = kLists + 16 * k + 8;
        for (const auto& x : p.r->acc)
            if (!x.wr && x.byte == a && x.data == kP7Reg[k]) { p.fetched[k] = true; break; }
    }
    p.win = blit_win(*p.r, 0);
    p.seen = p.r->word(kResults + 0x10) == 1;
    p.bbusy_clear = (p.r->word(kResults + 0x12) & 0x4000) == 0;
    for (int c = 0; c < 4; ++c) {
        auto a = accs_in(*p.r, p.win, kP7Lo[c][0], kP7Hi[c][0]);
        auto b = accs_in(*p.r, p.win, kP7Lo[c][1], kP7Hi[c][1]);
        a.insert(a.end(), b.begin(), b.end());
        std::sort(a.begin(), a.end(), [](const Acc& x, const Acc& y) { return x.cyc < y.cyc; });
        p.ch[c] = a;
    }
    p.blit_counts = p.win.ok;
    for (int c = 0; c < 4; ++c) p.blit_counts = p.blit_counts && p.ch[c].size() == 4;
    if (p.blit_counts) {
        bool same = true;
        for (int c = 0; c < 4; ++c) {
            long long s = (long long)p.ch[c][1].byte - (long long)p.ch[c][0].byte;
            if (!p.inc) p.inc = s;
            else if (*p.inc != s) same = false;
        }
        if (!same) p.inc.reset();
    }
    if (p.blit_counts && p.inc) {
        // list order A PTH,PTL,MOD, B ..., C ..., D ...: channel index for the table C,B,A,D
        const int chan[4] = {kA, kB, kC, kD};
        for (int g4 = 0; g4 < 4; ++g4) {
            const auto& a = p.ch[chan[g4]];
            p.val[3 * g4 + 0] = a[0].byte >> 16;
            p.val[3 * g4 + 1] = a[0].byte & 0xFFFF;
            p.val[3 * g4 + 2] = (long long)a[2].byte - (long long)a[1].byte - *p.inc;
            for (int j = 0; j < 3; ++j) p.val_ok[3 * g4 + j] = true;
        }
    }
    return p;
}

void proposition_p7() {
    if (!want("P7")) return;
    // expected register contents, per list index: base values with COPCON bit 1 clear,
    // the Copper's values with it set
    const long long base[12] = {0x0E, 0x0000, 8, 0x0D, 0x0000, 8, 0x0B, 0x0000, 8, 0x0F, 0x0000, 8};
    const long long copv[12] = {0x11, 0x4000, 24, 0x12, 0x4000, 24, 0x13, 0x4000, 24, 0x14, 0x4000, 24};

    // The mutation of every P7-0 component line has the CPU write the Copper's value
    // as the register's base value in the run it grades, and that of every P7-1
    // component line has the Copper move the base value. A mutation run of a
    // component line needs the one run it grades.
    bool run0 = true, run1 = true;
    std::string n0 = "prog_p7", n1 = "prog_p7_c1";
    if (g_mutate) {
        std::string m = g_mutate;
        // P7-<m>-<N>-<X>: the register BLT<X><N>; the mutation of a P7-0 line has the CPU
        // write the Copper's value as the register's base value, that of a P7-1 line has
        // the Copper move the base value
        auto regname = [&](const std::string& lab) { return "BLT" + lab.substr(lab.size() - 1) + lab.substr(5, 3); };
        if (m.rfind("P7-0-", 0) == 0) { run1 = false; n0 = "prog_p7_b_" + regname(m); }
        if (m.rfind("P7-1-", 0) == 0) { run0 = false; n1 = "prog_p7_c_" + regname(m); }
        if (m == "P7-PTH" || m == "P7-PTL" || m == "P7-MOD") n1 = "prog_p7";
        if (m == "P7-FETCH") n1 = "prog_p7_c1color";
        if (m == "P7-COPER") n1 = "prog_p7_nocoper";
        if (m == "P7-BLIT") n1 = "prog_p7_nobl";
        if (m == "P7-FIXTURE") n1 = "prog_p7_nofix";
    }
    std::shared_ptr<P7Run> a0 = run0 ? std::make_shared<P7Run>(p7_run(n0)) : nullptr;
    std::shared_ptr<P7Run> a1 = run1 ? std::make_shared<P7Run>(p7_run(n1)) : nullptr;

    bool fx = true;
    if (a0) fx = fx && a0->fixture;
    if (a1) fx = fx && a1->fixture;
    g.expect(fx, "P7-FIXTURE: the program stores the constant $BEEF first and the harness reads it back before any other result is used");

    bool fetch_all = true, coper_all = true, blit_all = true;
    bool fetched[2][12] = {}, coper_k[2][12] = {};
    for (int m = 0; m < 2; ++m) {
        const P7Run* p = m == 0 ? a0.get() : a1.get();
        if (!p) continue;
        for (int k = 0; k < 12; ++k) {
            fetched[m][k] = p->fetched[k];
            coper_k[m][k] = p->coper[k];
            fetch_all = fetch_all && p->fetched[k];
            coper_all = coper_all && p->coper[k];
        }
        blit_all = blit_all && p->win.ok && p->seen && p->bbusy_clear && p->blit_counts && p->inc.has_value();
    }
    g.expect(fetch_all, "P7-FETCH: in every frame k the Copper fetched the first word of its MOVE to register k: a read of the list's word at offset 8 returned that register's address");
    g.expect(coper_all, "P7-COPER: the COPER request was seen in every one of the twelve frames, so the Copper executed the MOVE to INTREQ that precedes the MOVE under test");
    g.expect(blit_all, "P7-BLIT: the blit raised BLIT, read BBUSY clear, and every channel made exactly 4 accesses with one constant step inside each line");

    bool anchor = true;
    if (full("P7-ANCHOR")) {
        P7Run c = p7_run(mutating("P7-ANCHOR") ? "prog_p7" : "prog_p7_nobl");
        bool empty = c.win.ok;
        for (int k = 0; k < 4; ++k) empty = empty && c.ch[k].empty();
        anchor = c.fixture && empty;
        g.expect(anchor, "P7-ANCHOR: with DMAEN set and BLTEN clear, the same blit makes no port access in any channel region between BLIT_GO and BLIT_DONE");
    } else checks.record("P7-ANCHOR: not evaluated in this mutation run");

    bool pre = fx && g_cfg_ok && anchor && blit_all;

    // per register and per run: the value the blit shows, against the page's prediction
    bool ok[2][12] = {};
    std::string mstr0, mstr1;
    for (int m = 0; m < 2; ++m) {
        const P7Run* p = m == 0 ? a0.get() : a1.get();
        if (!p) continue;
        for (int k = 0; k < 12; ++k) {
            ok[m][k] = p->val_ok[k] && p->val[k] == (m == 0 ? base[k] : copv[k]);
            (m == 0 ? mstr0 : mstr1) += " " + std::string(kP7Name[k]) + "=" + dec(p->val[k]);
        }
        // the second word of each MOVE (offset 10) is observed, not required
        int n2 = 0;
        for (int k = 0; k < 12; ++k)
            for (const auto& x : p->r->acc)
                if (!x.wr && x.byte == kLists + 16 * k + 10) { ++n2; break; }
        checks.note(std::string("P7 measured, COPCON bit 1 ") + (m == 0 ? "clear" : "set") +
                    ": the data word at offset 10 was fetched for " + dec(n2) + " of 12 lists");
    }
    if (a0) checks.note("P7 measured, COPCON bit 1 clear:" + mstr0);
    if (a1) checks.note("P7 measured, COPCON bit 1 set:" + mstr1);
    // a register whose MOVE was not fetched, or whose frame did not run the MOVE to INTREQ, is not graded
    auto cond = [&](int m, int k) { return ok[m][k]; };
    auto pr = [&](int m, int k) { return pre && fetched[m][k] && coper_k[m][k]; };
    g.expect_if(pr(0, 0), cond(0, 0), "P7-0-PTH-A: with COPCON bit 1 clear, a Copper MOVE to BLTAPTH does not change the register (PTH stays $000E)");
    g.expect_if(pr(0, 1), cond(0, 1), "P7-0-PTL-A: with COPCON bit 1 clear, a Copper MOVE to BLTAPTL does not change the register (PTL stays $0000)");
    g.expect_if(pr(0, 2), cond(0, 2), "P7-0-MOD-A: with COPCON bit 1 clear, a Copper MOVE to BLTAMOD does not change the register (modulo stays 8)");
    g.expect_if(pr(0, 3), cond(0, 3), "P7-0-PTH-B: with COPCON bit 1 clear, a Copper MOVE to BLTBPTH does not change the register (PTH stays $000D)");
    g.expect_if(pr(0, 4), cond(0, 4), "P7-0-PTL-B: with COPCON bit 1 clear, a Copper MOVE to BLTBPTL does not change the register (PTL stays $0000)");
    g.expect_if(pr(0, 5), cond(0, 5), "P7-0-MOD-B: with COPCON bit 1 clear, a Copper MOVE to BLTBMOD does not change the register (modulo stays 8)");
    g.expect_if(pr(0, 6), cond(0, 6), "P7-0-PTH-C: with COPCON bit 1 clear, a Copper MOVE to BLTCPTH does not change the register (PTH stays $000B)");
    g.expect_if(pr(0, 7), cond(0, 7), "P7-0-PTL-C: with COPCON bit 1 clear, a Copper MOVE to BLTCPTL does not change the register (PTL stays $0000)");
    g.expect_if(pr(0, 8), cond(0, 8), "P7-0-MOD-C: with COPCON bit 1 clear, a Copper MOVE to BLTCMOD does not change the register (modulo stays 8)");
    g.expect_if(pr(0, 9), cond(0, 9), "P7-0-PTH-D: with COPCON bit 1 clear, a Copper MOVE to BLTDPTH does not change the register (PTH stays $000F)");
    g.expect_if(pr(0, 10), cond(0, 10), "P7-0-PTL-D: with COPCON bit 1 clear, a Copper MOVE to BLTDPTL does not change the register (PTL stays $0000)");
    g.expect_if(pr(0, 11), cond(0, 11), "P7-0-MOD-D: with COPCON bit 1 clear, a Copper MOVE to BLTDMOD does not change the register (modulo stays 8)");
    g.expect_if(pr(1, 0), cond(1, 0), "P7-1-PTH-A: with COPCON bit 1 set, a Copper MOVE of $0011 to BLTAPTH changes the register");
    g.expect_if(pr(1, 1), cond(1, 1), "P7-1-PTL-A: with COPCON bit 1 set, a Copper MOVE of $4000 to BLTAPTL changes the register");
    g.expect_if(pr(1, 2), cond(1, 2), "P7-1-MOD-A: with COPCON bit 1 set, a Copper MOVE of 24 to BLTAMOD changes the register");
    g.expect_if(pr(1, 3), cond(1, 3), "P7-1-PTH-B: with COPCON bit 1 set, a Copper MOVE of $0012 to BLTBPTH changes the register");
    g.expect_if(pr(1, 4), cond(1, 4), "P7-1-PTL-B: with COPCON bit 1 set, a Copper MOVE of $4000 to BLTBPTL changes the register");
    g.expect_if(pr(1, 5), cond(1, 5), "P7-1-MOD-B: with COPCON bit 1 set, a Copper MOVE of 24 to BLTBMOD changes the register");
    g.expect_if(pr(1, 6), cond(1, 6), "P7-1-PTH-C: with COPCON bit 1 set, a Copper MOVE of $0013 to BLTCPTH changes the register");
    g.expect_if(pr(1, 7), cond(1, 7), "P7-1-PTL-C: with COPCON bit 1 set, a Copper MOVE of $4000 to BLTCPTL changes the register");
    g.expect_if(pr(1, 8), cond(1, 8), "P7-1-MOD-C: with COPCON bit 1 set, a Copper MOVE of 24 to BLTCMOD changes the register");
    g.expect_if(pr(1, 9), cond(1, 9), "P7-1-PTH-D: with COPCON bit 1 set, a Copper MOVE of $0014 to BLTDPTH changes the register");
    g.expect_if(pr(1, 10), cond(1, 10), "P7-1-PTL-D: with COPCON bit 1 set, a Copper MOVE of $4000 to BLTDPTL changes the register");
    g.expect_if(pr(1, 11), cond(1, 11), "P7-1-MOD-D: with COPCON bit 1 set, a Copper MOVE of 24 to BLTDMOD changes the register");

    bool all_pr = a0 && a1;
    for (int m = 0; m < 2; ++m) for (int k = 0; k < 12; ++k) all_pr = all_pr && pr(m, k);
    auto agg = [&](int off) {
        bool r = true;
        for (int m = 0; m < 2; ++m)
            for (int g4 = 0; g4 < 4; ++g4) r = r && ok[m][3 * g4 + off];
        return r;
    };
    g.expect_if(all_pr, agg(0), "P7-PTH: opus:AA-bltxpth.c6: a Copper MOVE to each of BLTAPTH, BLTBPTH, BLTCPTH and BLTDPTH changes the register only when COPCON bit 1 is set");
    g.expect_if(all_pr, agg(1), "P7-PTL: opus:AA-bltxptl.c5: a Copper MOVE to each of BLTAPTL, BLTBPTL, BLTCPTL and BLTDPTL changes the register only when COPCON bit 1 is set");
    g.expect_if(all_pr, agg(2), "P7-MOD: opus:AA-bltxmod.c4: a Copper MOVE to each of BLTAMOD, BLTBMOD, BLTCMOD and BLTDMOD changes the register only when COPCON bit 1 is set");
}

// ---- beam samples (F4) and the colour clock (F3) ----------------------------

// A sample is the long word VPOSR:VHPOSR and, when the program stores it, the
// word HHPOSR; each field is a word stored at the buffer pointer, and the
// port shows the stores with their times.
struct Sm {
    uint64_t t;      // port time of the store of the VHPOSR word
    uint64_t tend = 0;   // port time of the last store of the sample
    int V, H;        // V = (VPOSR & 7) * 256 + (VHPOSR >> 8), H = VHPOSR & $FF
    int lol;         // VPOSR bit 7
    uint16_t vposr, vhposr, hh, cia;
};

// The long word is read as two bus cycles, VPOSR first. A sample taken at the
// step from line 255 to line 256 can carry the old V8 with the new V7-0: its V
// then falls by 255 or 256 against the sample before it while the sample after it
// reads V >= 256. Such an isolated torn sample is completed in V with the V8 it
// lacks and counted in g_tears. The VPOSR word itself is never changed: V8 is
// graded on it as read.
int g_tears = 0;

std::vector<Sm> samples_of(const RunData& r, uint32_t base, uint32_t limit, bool hh, bool cia = false) {
    const uint32_t rec = cia ? 8 : hh ? 6 : 4;
    struct Rec { uint16_t w[4] = {0, 0, 0, 0}; uint64_t t1 = 0, tl = 0; int got = 0; };
    std::map<uint32_t, Rec> m;
    for (const auto& a : r.acc) {
        if (!a.wr || a.byte < base || a.byte >= limit) continue;
        uint32_t off = a.byte - base, idx = off / rec, f = (off % rec) / 2;
        Rec& x = m[idx];
        x.w[f] = a.data;
        x.got |= 1 << f;
        if (f == 1) x.t1 = a.cyc;
        x.tl = std::max(x.tl, a.cyc);
    }
    std::vector<Sm> out;
    for (const auto& kv : m) {
        const Rec& x = kv.second;
        if ((x.got & 3) != 3 || (hh && !(x.got & 4)) || (cia && !(x.got & 8))) continue;
        Sm s;
        s.t = x.t1; s.tend = x.tl; s.vposr = x.w[0]; s.vhposr = x.w[1]; s.hh = hh ? x.w[2] : 0; s.cia = cia ? x.w[3] : 0;
        s.V = ((x.w[0] & 7) << 8) | (x.w[1] >> 8);
        s.H = x.w[1] & 0xFF;
        s.lol = (x.w[0] >> 7) & 1;
        out.push_back(s);
    }
    std::vector<int> raw;
    for (const auto& x : out) raw.push_back(x.V);
    for (size_t i = 1; i + 1 < out.size(); ++i) {
        int d = raw[i - 1] - raw[i];
        if (d >= 255 && d <= 256 && raw[i - 1] < 256 && raw[i + 1] >= 256) { out[i].V += 256; ++g_tears; }
    }
    return out;
}

// F3: T_C is the time per unit of H: per line with at least two samples, the
// store time of its last sample minus that of its first, divided by their
// difference in H, averaged over the lines and rounded to a whole number of
// system clocks. Returns 0 if no line has two samples.
long long colour_clock(const std::vector<Sm>& s, double* raw = nullptr) {
    double sum = 0; int n = 0;
    size_t i = 0;
    while (i < s.size()) {
        size_t j = i;
        while (j + 1 < s.size() && s[j + 1].V == s[i].V) ++j;
        if (j > i && s[j].H > s[i].H) { sum += double(s[j].t - s[i].t) / double(s[j].H - s[i].H); ++n; }
        i = j + 1;
    }
    if (!n) return 0;
    if (raw) *raw = sum / n;
    return std::llround(sum / n);
}

// F4 anchor: the counters run and are being read. The page gives the position
// as the fields V and H of VHPOSR and does not say in which colour clock V
// steps relative to the wrap of H, so the anchor asks what the page supports of
// consecutive samples a and b:
//   (i)   they are not equal in (V,H): a counter that runs is read at two times;
//   (ii)  V does not decrease and steps by at most one (a frame sample ends at
//         VERTB, so a decrease of V can only fall on its last two samples);
//   (iii) within one value of V, H decreases at most once (the wrap at the end
//         of the line, whichever colour clock V steps in).
// f4_violations returns the number of pairs that break one of them. f4_lag counts
// the pairs of the third kind (H wrapped, V not yet stepped), which the strict
// lexicographic order of (V,H) would also reject; they are MEASURED.
int f4_scan(const std::vector<Sm>& s, bool frame_end, int* first_bad, int* lag) {
    int bad = 0, nlag = 0, dec_in_v = 0;
    if (first_bad) *first_bad = -1;
    for (size_t i = 1; i < s.size(); ++i) {
        const Sm& a = s[i - 1];
        const Sm& b = s[i];
        bool wrap = frame_end && i + 2 >= s.size() && b.V < a.V;
        bool viol = false;
        if (b.V == a.V && b.H == a.H) viol = true;                        // (i)
        if (!wrap && (b.V < a.V || b.V > a.V + 1)) viol = true;          // (ii)
        if (b.V != a.V) dec_in_v = 0;
        else if (b.H < a.H) {                                             // (iii) and the lag pairs
            ++nlag;
            if (++dec_in_v > 1) viol = true;
        }
        if (viol) { ++bad; if (first_bad && *first_bad < 0) *first_bad = (int)i; }
    }
    if (lag) *lag = nlag;
    return bad;
}
int f4_violations(const std::vector<Sm>& s, bool frame_end = true) { return f4_scan(s, frame_end, nullptr, nullptr); }
int f4_lag(const std::vector<Sm>& s) { int n = 0; f4_scan(s, true, nullptr, &n); return n; }

// ---- the beam programs: runs, port lines (F3) and frames (F6) ---------------

constexpr uint32_t kCfg = 0x0B8000;
constexpr uint32_t kSbuf0 = 0x1B0000, kSbuf1 = 0x1D0000, kSbuf2 = 0x1E0000;
constexpr uint16_t kMPreEnd = 0xF002, kMRegs = 0xF003, kMPostEnd = 0xF004;

// BEAMCON0 and the beam registers, by offset
constexpr uint16_t kHTOTAL = 0x1C0, kHSSTOP = 0x1C2, kHBSTRT = 0x1C4, kHBSTOP = 0x1C6, kVTOTAL = 0x1C8,
                   kVSSTOP = 0x1CA, kVBSTRT = 0x1CC, kVBOSTOP = 0x1CE, kBEAMCON0 = 0x1DC, kHSSTRT = 0x1DE,
                   kVSSTRT = 0x1E0, kHCENTER = 0x1E2;

using RegList = std::vector<std::pair<uint16_t, uint16_t>>;

struct BeamOpts {
    std::string prog = "prog_beam";
    bool pre = false;
    int post = 0;
    bool hh = false;
    int stops = 4;
    RegList regs;
    bool video = false;       // keep hs and vs of the graded frame
};

struct BeamRun {
    std::shared_ptr<RunData> r;
    bool fixture = false;
    std::vector<Sm> pre, post1, post2;
    std::vector<uint64_t> fm;     // the frame markers after the register list: m1, m2, ...
    uint64_t t_regs = 0;
    long long tc = 0;
};

BeamRun beam_run(const BeamOpts& b) {
    RunOpts o;
    o.prog = b.prog;
    o.pokes.push_back({kCfg + 0, (uint16_t)(b.pre ? 1 : 0)});
    o.pokes.push_back({kCfg + 2, (uint16_t)b.post});
    o.pokes.push_back({kCfg + 4, (uint16_t)(b.hh ? 1 : 0)});
    o.pokes.push_back({kCfg + 6, (uint16_t)b.stops});
    o.pokes.push_back({kCfg + 8, (uint16_t)b.regs.size()});
    for (size_t i = 0; i < b.regs.size(); ++i) {
        o.pokes.push_back({kCfg + 0x10 + 4 * (uint32_t)i, b.regs[i].first});
        o.pokes.push_back({kCfg + 0x12 + 4 * (uint32_t)i, b.regs[i].second});
    }
    o.video_graded = b.video;
    BeamRun r;
    r.r = run_opts(o);
    const RunData& d = *r.r;
    r.fixture = d.loaded && d.done && d.word(kResults) == 0xBEEF;
    auto ir = find_mark(d, kMRegs);
    if (ir) {
        r.t_regs = d.marks[*ir].cyc;
        for (size_t i = *ir + 1; i < d.marks.size(); ++i)
            if (d.marks[i].code == kMFrame) r.fm.push_back(d.marks[i].cyc);
    }
    r.pre = samples_of(d, kSbuf0, kSbuf0 + 0x20000, b.hh);
    r.post1 = samples_of(d, kSbuf1, kSbuf1 + 0x10000, b.hh);
    r.post2 = samples_of(d, kSbuf2, kSbuf2 + 0x10000, b.hh);
    r.tc = colour_clock(!r.post1.empty() ? r.post1 : r.pre);
    return r;
}

// Port lines (F3): the accesses in a window that no program range explains
// are refresh accesses; a line starts at the first access of each group.
struct Grp { uint64_t t; int n; };


std::vector<Grp> refresh_groups(const RunData& r, uint64_t t0, uint64_t t1, bool filter = true) {
    std::vector<uint64_t> t;
    for (const auto& a : r.acc)
        if (a.cyc > t0 && a.cyc < t1 && (!filter || !program_range(a.byte))) t.push_back(a.cyc);
    std::vector<Grp> out;
    if (t.size() < 2) return out;
    uint64_t gmax = 0;
    for (size_t i = 1; i < t.size(); ++i) gmax = std::max(gmax, t[i] - t[i - 1]);
    uint64_t thr = gmax / 2;
    out.push_back({t[0], 1});
    for (size_t i = 1; i < t.size(); ++i) {
        if (t[i] - t[i - 1] > thr) out.push_back({t[i], 1});
        else ++out.back().n;
    }
    return out;
}

// The F3 anchor over a window: every group but the first and the last holds
// exactly four accesses and every line period is a whole number of colour
// clocks. `lines` receives the line periods in system clocks.
bool f3_anchor(const std::vector<Grp>& g4, long long tc, std::vector<long long>* lines = nullptr) {
    if (g4.size() < 4 || tc <= 0) return false;
    bool ok = true;
    for (size_t i = 1; i + 1 < g4.size(); ++i) {
        if (g4[i].n != 4) ok = false;
        long long tl = (long long)(g4[i + 1].t - g4[i].t);
        if (tl % tc != 0) ok = false;
    }
    if (lines)
        for (size_t i = 0; i + 1 < g4.size(); ++i) lines->push_back((long long)(g4[i + 1].t - g4[i].t));
    return ok;
}

// The LOL of a sampled line: that of the line's first sample. V steps a few
// colour clocks after H wraps, so a sample taken in that gap carries the next
// line's LOL with the previous line's V; the first sample of a V never does.
std::map<int, int> lol_by_line(const std::vector<Sm>& s) {
    std::map<int, int> m;
    for (const auto& x : s) if (!m.count(x.V)) m[x.V] = x.lol;
    return m;
}

// The F3 numbers of a run's graded frame: the line periods in colour clocks.
struct LineData {
    std::vector<Grp> groups;
    std::vector<long long> lines;      // line periods in system clocks
    std::vector<long long> cck;        // line periods in colour clocks (rounded down)
    bool anchor = false;               // groups of exactly four, whole colour clocks
    bool frame_ok = false;             // the window holds a full frame's lines
};

LineData line_data(const BeamRun& b, size_t m_from, size_t m_to, bool filter = true) {
    LineData d;
    if (b.fm.size() <= m_to) return d;
    d.groups = refresh_groups(*b.r, b.fm[m_from], b.fm[m_to], filter);
    d.anchor = f3_anchor(d.groups, b.tc, &d.lines);
    for (long long l : d.lines) d.cck.push_back(b.tc > 0 ? l / b.tc : 0);
    d.frame_ok = d.lines.size() >= 250;
    return d;
}

// ---- P11 --------------------------------------------------------------------

// the mean number of HHPOSR decreases per main line of the frame, and whether HHPOSR
// takes at least two values on every main line
double wraps_per_line(const BeamRun& b, bool* moves) {
    std::map<int, std::set<int>> vals;
    std::map<int, int> w;
    for (const auto& x : b.post1) vals[x.V].insert(x.hh);
    for (size_t i = 1; i < b.post1.size(); ++i)
        if (b.post1[i].V == b.post1[i - 1].V && (b.post1[i].hh & 0xFF) < (b.post1[i - 1].hh & 0xFF)) ++w[b.post1[i].V];
    long long total = 0;
    bool m = !vals.empty();
    for (const auto& kv : vals) { total += w.count(kv.first) ? w[kv.first] : 0; if (kv.second.size() < 2) m = false; }
    if (moves) *moves = m;
    return vals.empty() ? 0.0 : (double)total / (double)vals.size();
}

void proposition_p11() {
    if (!want("P11")) return;
    if (!g_cfg_ok) { checks.record("NOT GRADED: P11 [CONFIG does not hold: the harness is not configured as the specification requires]"); return; }
    // DUAL set, VARBEAMEN clear, HTOTAL $38
    auto regs = [&](uint16_t htotal, uint16_t bc0) {
        return RegList{{kHTOTAL, htotal}, {kBEAMCON0, bc0}};
    };
    BeamOpts o;
    o.post = 1; o.hh = true; o.stops = 2;
    o.regs = regs(0x0038, 0x0060);
    if (mutating("P11-FIXTURE")) o.prog = "prog_beam_nofix";
    if (mutating("P11-HHMAX") || mutating("P11")) o.regs = regs(0x0070, 0x0060);
    BeamRun d = beam_run(o);
    // the contrast of P11-MAXH: the same sample loop with VARBEAMEN set and DUAL clear, where the page predicts that HTOTAL bounds the main counter
    BeamRun dm = d;
    if (mutating("P11-MAXH")) {
        BeamOpts om = o;
        om.regs = RegList{{kHTOTAL, 0x0038}, {kVTOTAL, 0x0138}, {kBEAMCON0, 0x08A0}};
        dm = beam_run(om);
    }
    g.expect(d.fixture, "P11-FIXTURE: the program stores the constant $BEEF first and the harness reads it back before any other result is used");
    int viol = f4_violations(d.post1);
    {
        BeamOpts a = o;
        if (mutating("P11-ANCHOR")) a.prog = "prog_beam_rev";
        BeamRun an = mutating("P11-ANCHOR") ? beam_run(a) : d;
        viol = f4_violations(an.post1);
        checks.note("P11 measured: " + dec((long long)an.post1.size()) + " samples in the frame, F4 order violations = " + dec(viol));
        g.expect(!an.post1.empty() && viol == 0, "P11-ANCHOR: the samples of the frame show the counters running: V never decreases within the frame and H increases within a line");
    }
    // the alternate counter runs: on every main line HHPOSR takes at least two values
    std::map<int, std::set<int>> hh_by_line;
    int maxH = 0, hhmax = 0;
    for (const auto& x : d.post1) { hh_by_line[x.V].insert(x.hh); maxH = std::max(maxH, x.H); hhmax = std::max(hhmax, (int)(x.hh & 0xFF)); }
    bool hh_runs = !hh_by_line.empty();
    for (const auto& kv : hh_by_line) if (kv.second.size() < 2) hh_runs = false;
    g.expect(hh_runs, "P11-HHRUN: on every main line of the frame HHPOSR takes at least two different values, so the alternate counter runs");
    // the main counter's line length, from the refresh groups of a stopped frame (F3): the period of the port lines
    auto port_line_cck = [&](const BeamRun& b, bool* anchor) {
        LineData l = line_data(b, 1, 2);
        if (anchor) *anchor = l.anchor && l.frame_ok;
        std::vector<long long> c = l.cck;
        if (c.empty()) return 0LL;
        std::sort(c.begin(), c.end());
        return c[c.size() / 2];
    };
    bool d_line_anchor = false, dm_line_anchor = false;
    long long d_line = port_line_cck(d, &d_line_anchor), dm_line = port_line_cck(dm, &dm_line_anchor);
    int maxH_m = 0;
    for (const auto& x : dm.post1) maxH_m = std::max(maxH_m, x.H);
    checks.note("P11 measured: median port line of a stopped frame = " + dec(d_line) + " colour clocks in the DUAL run (anchor " + dec(d_line_anchor ? 1 : 0) + ")" +
                (mutating("P11-MAXH") ? ", " + dec(dm_line) + " in the contrast run with VARBEAMEN set (anchor " + dec(dm_line_anchor ? 1 : 0) + "), maximum H of its samples = " + dec(maxH_m) : std::string("")));
    long long wraps = 0; int lines = 0;
    {
        std::map<int, int> w;
        for (size_t i = 1; i < d.post1.size(); ++i)
            if (d.post1[i].V == d.post1[i - 1].V && (d.post1[i].hh & 0xFF) < (d.post1[i - 1].hh & 0xFF)) ++w[d.post1[i].V];
        for (const auto& x : hh_by_line) { wraps += w.count(x.first) ? w[x.first] : 0; ++lines; }
    }
    checks.note("P11 measured: HTOTAL=" + hexs(o.regs[0].second, 4) + ": maximum H = " + dec(maxH) + ", maximum HHPOSR (bits 7-0) = " + dec(hhmax) +
                ", HHPOSR decreases per main line = " + fnum(lines ? (double)wraps / lines : 0) + " (" + dec(wraps) + " over " + dec(lines) + " lines)");
    bool pre = d.fixture && hh_runs && viol == 0;
    // the contrast run is live when its fixture and F4 anchor hold and HTOTAL + 1 = 57 colour clocks is what its port lines measure
    bool dm_live = !mutating("P11-MAXH") || (dm.fixture && f4_violations(dm.post1) == 0 && !dm.post1.empty() && dm_line_anchor && dm_line == 0x38 + 1);
    pre = pre && dm_live;
    bool maxh_ok = (mutating("P11-MAXH") ? maxH_m : maxH) > 0x38;
    bool hhmax_ok = hhmax <= 0x38 && !d.post1.empty();
    g.expect_if(pre, maxh_ok, "P11-MAXH: with DUAL set and HTOTAL = $38 the maximum H of the main horizontal counter over the frame exceeds $38: HTOTAL does not reset the main counter");
    g.expect_if(pre, hhmax_ok, "P11-HHMAX: with DUAL set and HTOTAL = $38 every HHPOSR sample (bits 7-0) is at most $38: HTOTAL is the alternate counter's highest count");
    g.expect_if(pre, maxh_ok && hhmax_ok, "P11: with DUAL set and VARBEAMEN clear HTOTAL bounds the alternate horizontal counter read through HHPOSR and does not bound the main counter read through VHPOSR");

    // the contrast: HTOTAL is the alternate counter's highest count, so its period grows by (HTOTAL' + 1) / (HTOTAL + 1)
    // and the number of HHPOSR decreases per main line falls by that ratio, to within 1
    if (!g_mutate || mutating("P11-PERIOD")) {
        BeamOpts o2;
        o2.post = 1; o2.hh = true; o2.stops = 2;
        o2.regs = regs(mutating("P11-PERIOD") ? 0x0038 : 0x00E2, 0x0060);
        BeamRun d2 = beam_run(o2);
        bool m1 = false, m2 = false;
        double w1 = wraps_per_line(d, &m1), w2 = wraps_per_line(d2, &m2);
        double ratio = double(0x00E2 + 1) / double(0x0038 + 1);
        checks.note("P11 measured: HHPOSR decreases per main line " + fnum(w1) + " with HTOTAL $38 and " + fnum(w2) + " with HTOTAL $E2, expected ratio " + fnum(ratio));
        g.expect_if(pre && d2.fixture && m2 && m1, std::fabs(w1 / ratio - w2) <= 1.0,
            "P11-PERIOD: raising HTOTAL from $38 to $E2 in the DUAL run makes the number of HHPOSR decreases per main line fall by the ratio ($E2 + 1) / ($38 + 1), to within 1");
    }
}

// ---- P13 and P14: line lengths and LOL --------------------------------------


RegList p13_regs(uint16_t htotal, uint16_t bc0) {
    return RegList{{kHTOTAL, htotal}, {kVTOTAL, 0x0138}, {kBEAMCON0, bc0}};
}

// bitplane DMA on one plane: accesses a refresh filter cannot explain
RegList dma_regs() {
    return RegList{{0x08E, 0x2C81}, {0x090, 0xF4C1}, {0x092, 0x0038}, {0x094, 0x00D0},
                   {0x0E0, 0x0010}, {0x0E2, 0x0000}, {0x100, 0x1000}, {0x096, 0x8300}};
}

void proposition_p13() {
    if (!want("P13")) return;
    if (!g_cfg_ok) { checks.record("NOT GRADED: P13 [CONFIG does not hold: the harness is not configured as the specification requires]"); return; }
    BeamOpts o1;
    o1.post = 1; o1.stops = 4;
    o1.regs = p13_regs(0x00E2, 0x00A0);
    BeamOpts o0 = o1;
    o0.regs = p13_regs(0x00E2, 0x0080);
    // mutations of the lines: the other run's stimulus in its place
    if (mutating("P13") || mutating("P13-1-LINES") || mutating("P13-1-LOL")) o1.regs = p13_regs(0x00E2, 0x0080);
    if (mutating("P13-0-LINES") || mutating("P13-0-LOL") || mutating("P13-0-COUNT"))
        o0.regs = p13_regs(0x00E2, 0x00A0);
    if (mutating("P13-FIXTURE")) o1.prog = "prog_beam_nofix";
    if (mutating("P13-0-FIXTURE")) o0.prog = "prog_beam_nofix";
    if (mutating("P13-0-F3")) { auto x = dma_regs(); o0.regs.insert(o0.regs.begin(), x.begin(), x.end()); }
    if (mutating("P13-F3")) { auto x = dma_regs(); o1.regs.insert(o1.regs.begin(), x.begin(), x.end()); }
    BeamRun r1 = beam_run(o1);
    g.expect(r1.fixture, "P13-FIXTURE: the program stores the constant $BEEF first and the harness reads it back before any other result is used");
    bool need0 = !g_mutate || mutating("P13-0-LINES") || mutating("P13-0-LOL") || mutating("P13-0-COUNT") ||
                 mutating("P13-0-FIXTURE") || mutating("P13-0-F3") || mutating("P13-0-F4");
    BeamRun r0;
    if (need0) r0 = beam_run(o0);

    LineData l1 = line_data(r1, 1, 2);
    LineData l0 = need0 ? line_data(r0, 1, 2) : LineData();
    checks.note("P13 measured: frame markers " + dec((long long)r1.fm.size()) + ", frame B " +
                (r1.fm.size() > 2 ? dec((long long)(r1.fm[2] - r1.fm[1])) : std::string("n/a")) + " system clocks, refresh groups " + dec((long long)l1.groups.size()));
    g.expect(l1.anchor && l1.frame_ok, "P13-F3: in frame B the refresh accesses come in groups of exactly four, one group per line, and every line period is a whole number of colour clocks");
    BeamRun rev = r1;
    if (mutating("P13-F4")) { BeamOpts a = o1; a.prog = "prog_beam_rev"; rev = beam_run(a); }
    int viol_a = f4_violations(rev.post1);
    checks.note("P13 measured: colour clock T_C = " + dec(r1.tc) + " system clocks, frame A samples = " + dec((long long)r1.post1.size()) +
                ", F4 violations = " + dec(viol_a) + ", line wraps with V not yet stepped = " + dec(f4_lag(r1.post1)) +
                ", port lines in frame B = " + dec((long long)l1.lines.size()));
    g.expect(!rev.post1.empty() && viol_a == 0, "P13-F4: the samples of frame A show the counters running: V never decreases within the frame and H increases within a line");

    // precondition 1: line length follows HTOTAL: HTOTAL $C0 shortens every line by $E2 - $C0 = 34 colour clocks
    bool vm = true;
    if (full("P13-VARMODE")) {
        BeamOpts oc = o1;
        oc.regs = p13_regs(mutating("P13-VARMODE") ? 0x00E2 : 0x00C0, 0x00A0);
        BeamRun rc = beam_run(oc);
        LineData lc = line_data(rc, 1, 2);
        vm = lc.anchor && l1.anchor && lc.cck.size() >= 250 && lc.cck.size() == l1.cck.size();
        for (size_t i = 0; vm && i < lc.cck.size(); ++i)
            if (l1.cck[i] - lc.cck[i] != 34) vm = false;
        g.expect(vm, "P13-VARMODE: with HTOTAL = $C0 instead of $E2 every port line of frame B is shorter by 34 colour clocks, so the line length follows HTOTAL");
    } else checks.record("P13-VARMODE: not evaluated in this mutation run");
    // precondition 2: LOLDIS clear, so that only PAL can suppress the toggle: with LOLDIS set there is no alternation
    bool ld = true;
    if (full("P13-LOLDIS")) {
        BeamOpts oc = o1;
        oc.regs = p13_regs(0x00E2, mutating("P13-LOLDIS") ? 0x0080 : 0x0880);
        BeamRun rc = beam_run(oc);
        LineData lc = line_data(rc, 1, 2);
        bool lines_const = !lc.cck.empty(), lol_const = true;
        for (size_t i = 1; i < lc.cck.size(); ++i) if (lc.cck[i] != lc.cck[0]) lines_const = false;
        auto lm = lol_by_line(rc.post1);
        for (const auto& kv : lm) if (kv.second != lm.begin()->second) lol_const = false;
        ld = lc.anchor && lines_const && lol_const && !lm.empty();
        g.expect(ld, "P13-LOLDIS: with LOLDIS set (BEAMCON0 $0880) neither the port line lengths nor LOL alternate");
    } else checks.record("P13-LOLDIS: not evaluated in this mutation run");

    bool pre = r1.fixture && l1.anchor && l1.frame_ok && viol_a == 0 && !rev.post1.empty() && vm && ld;

    // P13-1: every line is short
    auto all_equal = [&](const LineData& l, long long v) {
        if (l.cck.size() < 250) return false;
        for (long long c : l.cck) if (c != v) return false;
        return true;
    };
    bool lines1 = all_equal(l1, 0x00E2 + 1);
    bool lol1 = !r1.post1.empty();
    {
        auto lm = lol_by_line(r1.post1);
        for (const auto& kv : lm) if (kv.second != 0) lol1 = false;
    }
    g.expect_if(pre, lines1, "P13-1-LINES: with VARBEAMEN set, LOLDIS clear and PAL set every port line of frame B has HTOTAL + 1 = 227 colour clocks");
    g.expect_if(pre, lol1, "P13-1-LOL: with VARBEAMEN set, LOLDIS clear and PAL set VPOSR bit 7 (LOL) is 0 on every sampled line of frame A");
    g.expect_if(pre, lines1 && lol1,
        "P13: sol:AA-beamcon0.c53: in variable beam counter mode the PAL bit disables the long line/short line toggle and every line ends up short: all lines of a full frame have HTOTAL + 1 colour clocks and LOL is 0");

    // P13-0: the toggle returns
    if (!need0) return;
    // the PAL-clear run has its own fixture, port-line anchor and counter anchor
    g.expect(r0.fixture, "P13-0-FIXTURE: the program of the PAL-clear run stores the constant $BEEF first and the harness reads it back before any other result of that run is used");
    g.expect(l0.anchor && l0.frame_ok, "P13-0-F3: in frame B of the PAL-clear run the refresh accesses come in groups of exactly four, one group per line, and every line period is a whole number of colour clocks");
    BeamRun rev0 = r0;
    if (mutating("P13-0-F4")) { BeamOpts a = o0; a.prog = "prog_beam_rev"; rev0 = beam_run(a); }
    int viol0 = f4_violations(rev0.post1);
    g.expect(!rev0.post1.empty() && viol0 == 0, "P13-0-F4: the samples of frame A of the PAL-clear run show the counters running: V never decreases within the frame and H increases within a line");
    bool pre0 = pre && r0.fixture && l0.anchor && l0.frame_ok && !rev0.post1.empty() && viol0 == 0;
    bool alt_lines = l0.cck.size() >= 250, alt_lol = false;
    for (size_t i = 0; i < l0.cck.size(); ++i) {
        if (l0.cck[i] != 0x00E2 + 1 && l0.cck[i] != 0x00E2 + 2) alt_lines = false;
        if (i && l0.cck[i] == l0.cck[i - 1]) alt_lines = false;
    }
    int nlong_b = 0;
    for (long long c : l0.cck) if (c == 0x00E2 + 2) ++nlong_b;
    int nlol1 = 0;
    {
        auto lm = lol_by_line(r0.post1);
        alt_lol = lm.size() >= 250;
        int prev = -1, prevV = -10;
        for (const auto& kv : lm) {
            if (kv.second) ++nlol1;
            if (prevV == kv.first - 1 && prev == kv.second) alt_lol = false;
            prev = kv.second; prevV = kv.first;
        }
    }
    bool count_ok = std::abs(nlong_b - nlol1) <= 1;
    checks.note("P13 measured, PAL clear: port line lengths in colour clocks " +
                (l0.cck.size() > 5 ? dec(l0.cck[0]) + "," + dec(l0.cck[1]) + "," + dec(l0.cck[2]) + "," + dec(l0.cck[3]) + "..." : std::string("n/a")) +
                "; lines of 228 in frame B = " + dec(nlong_b) + ", lines with LOL = 1 in frame A = " + dec(nlol1));
    g.expect_if(pre0, alt_lines, "P13-0-LINES: with PAL clear (VARBEAMEN set, LOLDIS clear) the port lines of frame B alternate between HTOTAL + 1 = 227 and HTOTAL + 2 = 228 colour clocks");
    g.expect_if(pre0, alt_lol, "P13-0-LOL: with PAL clear (VARBEAMEN set, LOLDIS clear) LOL alternates between consecutive sampled lines of frame A");
    g.expect_if(pre0, count_ok, "P13-0-COUNT: with PAL clear the number of 228-colour-clock port lines in frame B equals the number of LOL = 1 lines in frame A to within 1");
}

// ---- P14 --------------------------------------------------------------------

void proposition_p14() {
    if (!want("P14")) return;
    if (!g_cfg_ok) { checks.record("NOT GRADED: P14 [CONFIG does not hold: the harness is not configured as the specification requires]"); return; }
    BeamOpts o1;
    o1.post = 2; o1.stops = 4;
    o1.regs = {{kBEAMCON0, 0x0020}};
    BeamOpts o0 = o1;
    o0.regs = {{kBEAMCON0, 0x0000}};
    if (mutating("P14-0-LOL") || mutating("P14-0-LINES") || mutating("P14-0-LONGONLY")) o0.regs = {{kBEAMCON0, 0x0020}};
    if (mutating("P14-1") || mutating("P14")) o1.regs = {{kBEAMCON0, 0x0000}};
    if (mutating("P14-FIXTURE")) o1.prog = "prog_beam_nofix";
    if (mutating("P14-STABLE")) o1.regs.insert(o1.regs.begin(), {0x100, 0x0004});   // BPLCON0 LACE: the two fields differ in length
    if (mutating("P14-NORMAL")) o1.regs = {{kHTOTAL, 0x00E2}, {kVTOTAL, 0x0138}, {kBEAMCON0, 0x00A0}};
    if (mutating("P14-F3")) { auto x = dma_regs(); o1.regs.insert(o1.regs.begin(), x.begin(), x.end()); }
    BeamRun r1 = beam_run(o1);
    BeamRun r0 = beam_run(o0);
    g.expect(r1.fixture && r0.fixture, "P14-FIXTURE: the program stores the constant $BEEF first and the harness reads it back before any other result is used");

    LineData l1 = line_data(r1, 1, 2), l0 = line_data(r0, 1, 2);
    g.expect(l1.anchor && l1.frame_ok && l0.anchor && l0.frame_ok,
             "P14-F3: in the stopped frame of both runs the refresh accesses come in groups of exactly four, one group per line, and every line period is a whole number of colour clocks");
    BeamRun rv = r1;
    if (mutating("P14-F4")) { BeamOpts a = o1; a.prog = "prog_beam_rev"; rv = beam_run(a); }
    int va1 = f4_violations(rv.post1), va2 = f4_violations(rv.post2), vb1 = f4_violations(r0.post1), vb2 = f4_violations(r0.post2);
    checks.note("P14 measured: F4 violations, PAL set frames " + dec(va1) + " and " + dec(va2) + ", PAL clear frames " + dec(vb1) + " and " + dec(vb2));
    for (const auto* sv : {&rv.post1, &rv.post2, &r0.post1, &r0.post2}) {
        int fb = -1;
        f4_scan(*sv, true, &fb, nullptr);
        if (fb > 0) {
            const Sm& a = (*sv)[fb - 1]; const Sm& b = (*sv)[fb];
            checks.note("P14 measured: sample " + dec(fb) + ": " + dec(a.V) + ":" + dec(a.H) + " then " + dec(b.V) + ":" + dec(b.H));
        }
    }
    g.expect(!rv.post1.empty() && !r0.post1.empty() && va1 == 0 && va2 == 0 && vb1 == 0 && vb2 == 0,
             "P14-F4: the samples of both frames of both runs show the counters running: V never decreases within a frame and H increases within a line");

    auto vmax = [](const std::vector<Sm>& s) { int m = 0; for (const auto& x : s) m = std::max(m, x.V); return m; };
    long long f1a = vmax(rv.post1) + 1, f1b = vmax(rv.post2) + 1, f0a = vmax(r0.post1) + 1, f0b = vmax(r0.post2) + 1;
    auto period = [&](const BeamRun& b, size_t i) { return b.fm.size() > i + 1 ? (long long)(b.fm[i + 1] - b.fm[i]) : 0LL; };
    long long p1a = period(r1, 1), p1b = period(r1, 2), p0a = period(r0, 1), p0b = period(r0, 2);
    checks.note("P14 measured, PAL set: frame lines " + dec(f1a) + " and " + dec(f1b) + ", frame periods " + dec(p1a) + " and " + dec(p1b) +
                " system clocks; PAL clear: frame lines " + dec(f0a) + " and " + dec(f0b) + ", frame periods " + dec(p0a) + " and " + dec(p0b));
    // the frame period in units of T_L: the period over the mean line period of the stopped frame, rounded
    auto mean_line = [](const LineData& l) {
        double s = 0; for (long long x : l.lines) s += (double)x;
        return l.lines.empty() ? 0.0 : s / (double)l.lines.size();
    };
    auto in_lines = [](long long p, double ml) { return ml > 0 ? std::llround((double)p / ml) : 0LL; };
    long long u1a = in_lines(p1a, mean_line(l1)), u1b = in_lines(p1b, mean_line(l1));
    long long u0a = in_lines(p0a, mean_line(l0)), u0b = in_lines(p0b, mean_line(l0));
    checks.note("P14 measured: frame periods in units of T_L: PAL set " + dec(u1a) + " and " + dec(u1b) + ", PAL clear " + dec(u0a) + " and " + dec(u0b));
    bool stable = f1a == f1b && f0a == f0b && u1a == u1b && u0a == u0b && u1a > 0 && u0a > 0;
    g.expect(stable, "P14-STABLE: in both runs two consecutive frames give the same number of lines and the same frame period in units of T_L");
    bool normal = true;   // VARBEAMEN and LOLDIS are clear in both runs: asserted from the BEAMCON0 values written
    for (const auto* ob : {&o1, &o0}) for (const auto& rg : ob->regs) if (rg.first == kBEAMCON0 && (rg.second & 0x0880)) normal = false;
    g.expect(normal, "P14-NORMAL: both runs are in normal mode, VARBEAMEN and LOLDIS clear in the BEAMCON0 value written");

    bool pre = r1.fixture && r0.fixture && l1.anchor && l1.frame_ok && l0.anchor && l0.frame_ok && va1 == 0 && va2 == 0 &&
               vb1 == 0 && vb2 == 0 && stable && normal;

    // P14-0: LOL alternates, the port lines alternate with the longer ones 228 colour clocks, never two long lines in a row
    bool lol0 = false;
    {
        auto lm = lol_by_line(r0.post1);
        lol0 = lm.size() >= 200;
        int prev = -1, prevV = -10;
        for (const auto& kv : lm) {
            if (prevV == kv.first - 1 && prev == kv.second) lol0 = false;
            prev = kv.second; prevV = kv.first;
        }
    }
    bool lines0 = l0.cck.size() >= 200;
    long long longlen = 0;
    for (long long c : l0.cck) longlen = std::max(longlen, c);
    for (size_t i = 1; i < l0.cck.size(); ++i) if (l0.cck[i] == l0.cck[i - 1]) lines0 = false;
    lines0 = lines0 && longlen == 228;
    bool nolongpair = l0.cck.size() >= 200;
    for (size_t i = 1; i < l0.cck.size(); ++i) if (l0.cck[i] == 228 && l0.cck[i - 1] == 228) nolongpair = false;
    checks.note("P14 measured, PAL clear: port line lengths in colour clocks " +
                (l0.cck.size() > 5 ? dec(l0.cck[0]) + "," + dec(l0.cck[1]) + "," + dec(l0.cck[2]) + "," + dec(l0.cck[3]) + "..." : std::string("n/a")) +
                ", longest = " + dec(longlen));
    g.expect_if(pre, lol0, "P14-0-LOL: with PAL clear in normal mode LOL alternates between consecutive sampled lines");
    g.expect_if(pre, lines0, "P14-0-LINES: with PAL clear in normal mode the port lines alternate and the longer ones are 228 colour clocks");
    g.expect_if(pre && longlen == 228, nolongpair, "P14-0-LONGONLY: with PAL clear in normal mode no two consecutive port lines are both long");
    // P14-1: at least one of the frame length and the line-length pattern differs from P14-0
    bool differs = f1a != f0a || l1.cck != l0.cck;
    g.expect_if(pre, differs, "P14-1: with PAL set in normal mode the frame length in lines or the sequence of port line lengths differs from that with PAL clear (a control that carries no claim id: the pages give no PAL decodes, so the difference is not graded as appropriate)");
    g.expect_if(pre, lol0 && lines0 && nolongpair && differs,
        "P14: in normal mode the PAL bit selects decodes that differ from those with PAL clear, which has every other line a long line of 228 colour clocks");
}

// ---- P12 --------------------------------------------------------------------

constexpr uint16_t kMVmark = 0xF020;
constexpr uint32_t kBplhLo = 0x180000, kBplhHi = 0x190000, kSprhLo = 0x190000, kSprhHi = 0x1A0000;

// the average HHPOSR count per colour clock between consecutive samples of one
// main line, over the lines of the frame (0 if HHPOSR never moves)
double hh_rate(const BeamRun& b) {
    double sum = 0; int n = 0;
    for (size_t i = 1; i < b.post1.size(); ++i) {
        const Sm& a = b.post1[i - 1]; const Sm& c = b.post1[i];
        int ha = a.hh & 0xFF, hc = c.hh & 0xFF;
        if (a.V == c.V && hc > ha && c.t > a.t && b.tc > 0) { sum += double(hc - ha) / (double(c.t - a.t) / double(b.tc)); ++n; }
    }
    return n ? sum / n : 0.0;
}

// the fewest HHPOSR decreases on a complete main line of the frame (the first and the last
// sampled line are cut by the frame sample's start and end)
int hh_wraps_min(const BeamRun& b, bool* every_line_twice) {
    std::map<int, int> w;
    std::set<int> lines;
    for (const auto& x : b.post1) lines.insert(x.V);
    if (lines.size() > 2) { lines.erase(lines.begin()); lines.erase(std::prev(lines.end())); }
    for (size_t i = 1; i < b.post1.size(); ++i)
        if (b.post1[i].V == b.post1[i - 1].V && (b.post1[i].hh & 0xFF) < (b.post1[i - 1].hh & 0xFF)) ++w[b.post1[i].V];
    int mn = 1 << 30;
    for (int v : lines) mn = std::min(mn, w.count(v) ? w[v] : 0);
    if (every_line_twice) *every_line_twice = !lines.empty() && mn >= 2;
    return lines.empty() ? 0 : mn;
}

struct UhRun {
    std::shared_ptr<RunData> r;
    bool fixture = false;
    uint64_t tvm = 0;                  // VMARK(90) of the graded frame
    long long tc = 0;
    std::vector<Grp> groups;           // refresh groups from before VMARK to the end of the frame
    int g90 = -1;                      // the group of the port line containing VMARK(90)
    bool anchor = false;
    std::vector<int> bplh, sprh;       // accesses per port line, lines 102..108
    int other = 0;                     // accesses on those lines that refresh, UHRES and the null sprite do not explain
    std::string other_first;           // the first of them
    std::vector<double> len_cck;       // those lines' lengths in colour clocks
    std::vector<std::string> seq;      // per line 102..108, the region accesses in time order: S sprite pointer region, B bit-plane pointer region
    RegList regs;                      // the register list the program wrote
};

// a refresh access: a read of the refresh addresses (byte address below $1000), not the level-3 vector the
// interrupt that ends a stopped stretch reads
bool is_refresh(const Acc& a) { return !a.wr && a.byte < 0x1000 && !(a.byte >= 0x6C && a.byte < 0x70); }

// extra: registers added to the list, and sprite words poked at the sprite data address
UhRun uh_run(uint16_t bc0, uint16_t bplcon0, long long tc, const char* prog = "prog_uhres", RegList extra = {},
             bool sprite_data = false, int hhw = -1) {
    RunOpts o;
    o.prog = prog;
    if (hhw >= 0) o.pokes.push_back({kCfg + 4, (uint16_t)hhw});
    RegList regs = {{kHTOTAL, 0x0038}, {0x1E6, 0x0000}, {0x1D4, 100}, {0x1D6, 110}, {0x1D0, 100}, {0x1D2, 110},
                    {0x1EC, 0x0018}, {0x1EE, 0x0000}, {0x1E8, 0x0019}, {0x1EA, 0x0000},
                    {kBEAMCON0, bc0}, {0x100, bplcon0}};
    regs.insert(regs.end(), extra.begin(), extra.end());
    regs.push_back({0x096, 0x8320});
    if (sprite_data) {
        // a sprite from line 90 to line 130, its data words zero
        o.pokes.push_back({0x190000, 0x5A40});
        o.pokes.push_back({0x190002, 0x8200});
        for (uint32_t a = 0x190004; a < 0x190004 + 2 * 2 * 41; a += 2) o.pokes.push_back({a, 0x0000});
    }
    o.pokes.push_back({kCfg + 6, 5});
    o.pokes.push_back({kCfg + 8, (uint16_t)regs.size()});
    for (size_t i = 0; i < regs.size(); ++i) {
        o.pokes.push_back({kCfg + 0x10 + 4 * (uint32_t)i, regs[i].first});
        o.pokes.push_back({kCfg + 0x12 + 4 * (uint32_t)i, regs[i].second});
    }
    UhRun u;
    u.regs = regs;
    u.r = run_opts(o);
    const RunData& d = *u.r;
    u.fixture = d.loaded && d.done && d.word(kResults) == 0xBEEF;
    u.tc = tc;
    std::vector<uint64_t> vm;
    for (const auto& m : d.marks) if (m.code == kMVmark) vm.push_back(m.cyc);
    if (vm.size() < 4) return u;
    u.tvm = vm[2];
    uint64_t tnext = 0;
    for (const auto& m : d.marks) if (m.code == kMFrame && m.cyc > u.tvm) { tnext = m.cyc; break; }
    if (!tnext || tc <= 0) return u;
    // refresh accesses: reads of the refresh addresses (below $1000, the vector $6C the interrupt reads excepted)
    std::vector<uint64_t> t;
    uint64_t t0 = u.tvm > 4 * 230 * (uint64_t)tc ? u.tvm - 4 * 230 * (uint64_t)tc : 0;
    for (const auto& a : d.acc)
        if (a.cyc > t0 && a.cyc < tnext && is_refresh(a)) t.push_back(a.cyc);
    if (t.size() < 8) return u;
    uint64_t gmax = 0;
    for (size_t i = 1; i < t.size(); ++i) gmax = std::max(gmax, t[i] - t[i - 1]);
    u.groups.push_back({t[0], 1});
    for (size_t i = 1; i < t.size(); ++i) {
        if (t[i] - t[i - 1] > gmax / 2) u.groups.push_back({t[i], 1});
        else ++u.groups.back().n;
    }
    std::vector<long long> lines;
    u.anchor = f3_anchor(u.groups, tc, &lines);
    for (size_t i = 0; i < u.groups.size(); ++i) if (u.groups[i].t <= u.tvm) u.g90 = (int)i;
    if (u.g90 < 0 || u.g90 + 20 >= (int)u.groups.size()) { u.anchor = false; return u; }
    // the group holding VMARK(90) and every group of lines 90-109 is a four-access group of one line period
    for (int k = 0; k <= 19; ++k) {
        const Grp& ga = u.groups[(size_t)(u.g90 + k)];
        const Grp& gb = u.groups[(size_t)(u.g90 + k + 1)];
        if (ga.n != 4 || (long long)(gb.t - ga.t) % tc != 0) u.anchor = false;
    }
    for (int ln = 102; ln <= 108; ++ln) {
        size_t gi = (size_t)(u.g90 + (ln - 90));
        uint64_t a = u.groups[gi].t, b = u.groups[gi + 1].t;
        int nb = 0, ns = 0;
        std::string sq;
        for (const auto& x : d.acc) {
            if (x.cyc < a || x.cyc >= b) continue;
            if (x.byte >= kBplhLo && x.byte < kBplhHi) { ++nb; sq += 'B'; }
            if (x.byte >= kSprhLo && x.byte < kSprhHi) { ++ns; sq += 'S'; }
        }
        u.seq.push_back(sq);
        for (const auto& x : d.acc) {
            if (x.cyc < a || x.cyc >= b) continue;
            if (x.byte < 0x1000 || (x.byte >= kBplhLo && x.byte < 0x1B0000)) continue;
            ++u.other;
            if (u.other_first.size() < 60) u.other_first += " " + hexs(x.byte) + (x.wr ? "w" : "r");
        }
        u.bplh.push_back(nb); u.sprh.push_back(ns);
        u.len_cck.push_back(double(b - a) / double(tc));
    }
    return u;
}


// ---- the chain against the reset of the alternate counter (P12-PHASE) ----
struct PhaseData {
    bool ok = false;
    uint64_t tw = 0;                 // the time of the marker written right after the write of HHPOSW
    std::vector<uint64_t> t1;        // the first access of every complete chain (sprite, bit plane, sprite pointer) in the graded frame
    std::vector<uint64_t> other;     // the accesses of the window outside the two regions
};
PhaseData phase_data(const UhRun& u) {
    PhaseData p;
    if (!u.fixture || u.tvm == 0) return p;
    const RunData& d = *u.r;
    std::vector<uint64_t> hw;
    for (const auto& m : d.marks) if (m.code == 0xF021) hw.push_back(m.cyc);
    if (hw.size() < 3) return p;
    uint64_t tnext = 0;
    for (const auto& m : d.marks) if (m.code == kMFrame && m.cyc > u.tvm) { tnext = m.cyc; break; }
    if (!tnext) return p;
    p.tw = hw[2];
    std::vector<std::pair<uint64_t, char>> ev;
    for (const auto& x : d.acc) {
        if (x.cyc < u.tvm || x.cyc >= tnext) continue;
        if (x.byte >= kBplhLo && x.byte < kBplhHi) ev.push_back({x.cyc, 'B'});
        else if (x.byte >= kSprhLo && x.byte < kSprhHi) ev.push_back({x.cyc, 'S'});
        else p.other.push_back(x.cyc);
    }
    for (size_t i = 0; i + 2 < ev.size();) {
        if (ev[i].second == 'S' && ev[i + 1].second == 'B' && ev[i + 2].second == 'S') { p.t1.push_back(ev[i].first); i += 3; }
        else ++i;
    }
    p.ok = true;
    return p;
}

// The alternate counter is written with v right after the marker; it then counts up to HTOTAL and resets every
// (HTOTAL + 1) colour clocks, so its resets fall at tw + (HTOTAL + 1 - v) T_C + k (HTOTAL + 1) T_C, up to the one latency of the
// write, which is the same in every run of the same program. A chain that starts at those resets starts a constant time after
// them, for every chain that waits for no busy cycle. The delay of such an undisturbed chain, from the predicted reset, is
// returned for every chain; a chain is disturbed when an access outside the two regions falls between 64 system clocks before
// the predicted reset and its first access (it waits for a free cycle, p.15).
std::vector<long long> phase_delays(const PhaseData& p, int v, long long tc, int htotal, int* disturbed) {
    std::vector<long long> out;
    const long long per = (long long)(htotal + 1) * tc;
    const long long first = (long long)(htotal + 1 - v) * tc;
    if (disturbed) *disturbed = 0;
    for (uint64_t t1 : p.t1) {
        long long rel = (long long)(t1 - p.tw) - first;
        if (rel < 0) continue;
        long long k = rel / per;
        uint64_t r = p.tw + (uint64_t)(first + k * per);
        bool dist = false;
        for (uint64_t o : p.other) if (o + 64 >= r && o < t1) { dist = true; break; }
        if (dist) { if (disturbed) ++*disturbed; continue; }
        out.push_back((long long)(t1 - r));
    }
    return out;
}

void proposition_p12() {
    if (!want("P12")) return;
    if (!g_cfg_ok) { checks.record("NOT GRADED: P12 [CONFIG does not hold: the harness is not configured as the specification requires]"); return; }
    // P11's DUAL run: the alternate counter's rate and its wraps per main line
    BeamOpts od;
    od.post = 1; od.hh = true; od.stops = 2;
    od.regs = {{kHTOTAL, mutating("P12-K") ? (uint16_t)0x00E2 : (uint16_t)0x0038}, {kBEAMCON0, 0x0060}};
    if (mutating("P12-KFIXTURE")) od.prog = "prog_beam_nofix";
    if (mutating("P12-KANCHOR")) od.prog = "prog_beam_rev";
    BeamRun dr = beam_run(od);
    // the DUAL run has its own fixture and F4 anchor; they gate P12-K and every check derived from its alternate-counter rate
    g.expect(dr.fixture, "P12-KFIXTURE: the program of the DUAL run stores the constant $BEEF first and the harness reads it back before any other result of that run is used");
    int kviol = f4_violations(dr.post1);
    bool kanchor = !dr.post1.empty() && kviol == 0;
    g.expect(kanchor, "P12-KANCHOR: the samples of the DUAL run show the counters running: V never decreases within the frame and H increases within a line");
    bool kgate = dr.fixture && kanchor;
    // the UHRES runs are timed in the colour clock that DUAL run measures, so every line timed in it is gated on its fixture and anchor
    long long uh_tc = kgate ? dr.tc : 0;
    double rate = hh_rate(dr);
    bool every_twice = false;
    int minwraps = hh_wraps_min(dr, &every_twice);
    double t_alt = rate > 0 ? double(od.regs[0].second + 1) / rate : 0.0;
    checks.note("P12 measured: colour clock T_C = " + dec(dr.tc) + " system clocks; HHPOSR rate per colour clock = " + fnum(rate) +
                ", alternate counter period = " + fnum(t_alt) + " colour clocks, fewest HHPOSR decreases on a main line = " + dec(minwraps));

    std::string prog = mutating("P12-FIXTURE") ? "prog_uhres_nofix" : "prog_uhres";
    // the display registers a plain one-bit-plane display needs
    RegList disp = {{0x08E, 0x2C81}, {0x090, 0xF4C1}, {0x092, 0x0038}, {0x094, 0x00D0}};
    UhRun p0;
    if (mutating("P12-UHRES")) {
        // a live input for the same check: one lores bit plane fetched from the bit-plane region and a sprite
        // with data in the sprite region, so that both regions are accessed on every graded line
        RegList ex = disp;
        ex.push_back({0x0E0, 0x0018}); ex.push_back({0x0E2, 0x0000});
        p0 = uh_run(0x0020, 0x1000, uh_tc, "prog_uhres_ctl", ex, true);   // BPLCON0 $1000: UHRES clear, one bit plane
    } else if (mutating("P12-F3")) {
        // a lores bit plane fetched from byte address $000000 with BPL1MOD -40, so that every line reads the same 20
        // words of the refresh addresses: its reads look like refresh accesses
        RegList ex = disp;
        ex.push_back({0x0E0, 0x0000}); ex.push_back({0x0E2, 0x0000}); ex.push_back({0x108, 0xFFD8});
        p0 = uh_run(0x0020, 0x1080, uh_tc, prog.c_str(), ex);   // BPLCON0 $1080: UHRES set, one bit plane
    } else if (mutating("P12-BANDWIDTH")) {
        // a bit plane fetched in the lines under test
        RegList ex = disp;
        ex.push_back({0x0E0, 0x000F}); ex.push_back({0x0E2, 0x8000});
        p0 = uh_run(0x0020, 0x1080, uh_tc, prog.c_str(), ex);   // BPLCON0 $1080: UHRES set, one bit plane
    } else if (mutating("P12-UHRESEN")) {
        p0 = uh_run(0x0020, 0x0000, uh_tc, prog.c_str());   // UHRES clear
    } else p0 = uh_run(mutating("P12-0") ? 0x0060 : 0x0020, 0x0080, uh_tc, prog.c_str());   // P12-0: DUAL set in the P12-0 run
    UhRun p1 = uh_run(mutating("P12-1-BPL") || mutating("P12-1-SPR") || mutating("P12") ? 0x0020 : 0x0060, 0x0080, uh_tc, prog.c_str());
    g.expect(p0.fixture, "P12-FIXTURE: the program stores the constant $BEEF first and the harness reads it back before any other result is used");
    bool f3 = p0.anchor && p0.g90 >= 0;
    g.expect_if(kgate, f3, "P12-F3: in the stopped stretch after VMARK(90) the refresh accesses (reads of byte addresses below $1000) come in groups of exactly four, one group per line period, every line period is a whole number of colour clocks, and the port line holding VMARK(90), which the test calls 90, and every line of lines 90-109 is such a group");
    // UHRES is enabled in the runs that grade the chain: BPLCON0 bit 7 is written set and DMACON sets DMAEN
    auto uh_enabled = [](const UhRun& u) {
        bool bpl = false, dma = false;
        for (const auto& rg : u.regs) {
            if (rg.first == 0x100) bpl = (rg.second & 0x0080) != 0;
            if (rg.first == 0x096) dma = (rg.second & 0x8200) == 0x8200;
        }
        return bpl && dma;
    };
    // UHRES DMA exists: on every graded line of P12-0 the first three accesses in the two regions are, in this order, the sprite
    // pointer (first free cycle), the bit-plane pointer (second) and the sprite pointer (third) of the chain (p.15, p.27). Ordinary
    // bit-plane and sprite data fetches do not make that sequence at the start of a line.
    bool exists = !p0.bplh.empty() && p0.seq.size() == p0.bplh.size();
    for (size_t i = 0; i < p0.bplh.size(); ++i) if (p0.bplh[i] == 0 || p0.sprh[i] == 0 || p0.seq[i].compare(0, 3, "SBS") != 0) exists = false;
    {
        std::string s;
        for (size_t i = 0; i < p0.bplh.size(); ++i) s += " " + dec(p0.bplh[i]) + "/" + dec(p0.sprh[i]);
        checks.note("P12 measured, DUAL clear: BPLHPT/SPRHPT accesses on lines 102-108:" + s);
    }
    g.expect_if(kgate && f3, exists, "P12-UHRES: with UHRES enabled and DUAL clear the UHRES pointer regions are accessed on every graded line, and the first three accesses of the line in them are the sprite pointer, the bit-plane pointer and the sprite pointer, in this order");
    bool uhen0 = uh_enabled(p0) && uh_enabled(p1);
    g.expect(uhen0, "P12-UHRESEN: the runs that grade the UHRES chain write BPLCON0 with bit 7 (UHRES) set and DMACON with SET and DMAEN set, with DUAL clear and with DUAL set");
    // bandwidth is left: on the graded lines the port shows only refresh, UHRES and null-sprite accesses
    bool bw = p0.other == 0;
    checks.note("P12 measured, DUAL clear: accesses on lines 102-108 that refresh, UHRES and the null sprite do not explain = " + dec(p0.other) +
                (p0.other ? ", first:" + p0.other_first : std::string("")));
    g.expect_if(kgate && f3, bw, "P12-BANDWIDTH: on lines 102-108 the port shows only refresh, UHRES and null-sprite accesses: no bit plane is fetched, no other DMA runs and the CPU is stopped");
    g.expect_if(kgate, every_twice, "P12-K: in the DUAL run of P11 HHPOSR decreases at least twice on every complete main line, so the alternate counter wraps at least twice per main line");
    bool pre = p0.fixture && f3 && uhen0 && exists && bw && every_twice && kgate;

    bool pre1 = pre && p1.anchor && p1.g90 >= 0 && p1.other == 0;
    {
        std::string s;
        for (size_t i = 0; i < p1.bplh.size(); ++i) s += " " + dec(p1.bplh[i]) + "/" + dec(p1.sprh[i]);
        checks.note("P12 measured, DUAL set: BPLHPT/SPRHPT accesses on lines 102-108:" + s);
    }
    // P12-0: exactly 1 bit-plane pointer access and 2 sprite pointer accesses on every graded line;
    // the DUAL-clear case does not use the alternate counter, so P12-K is not among its preconditions
    bool pre0 = p0.fixture && f3 && uhen0 && exists && bw && kgate;
    bool c0 = !p0.bplh.empty();
    for (size_t i = 0; i < p0.bplh.size(); ++i) if (p0.bplh[i] != 1 || p0.sprh[i] != 2) c0 = false;
    g.expect_if(pre0, c0, "P12-0: with DUAL clear every graded line shows exactly 1 UHRES bit-plane pointer access and exactly 2 UHRES sprite pointer accesses");
    // P12-1: k alternate-counter resets per line
    bool b1 = !p1.bplh.empty(), s1 = !p1.sprh.empty();
    for (size_t i = 0; i < p1.bplh.size(); ++i) {
        double k = t_alt > 0 ? p1.len_cck[i] / t_alt : 0;
        if (p1.bplh[i] < 2 || p1.bplh[i] < (int)std::floor(k) || p1.bplh[i] > (int)std::ceil(k)) b1 = false;
        if (p1.sprh[i] <= 2) s1 = false;
    }
    g.expect_if(pre1, b1, "P12-1-BPL: with DUAL set every graded line shows at least 2 UHRES bit-plane pointer accesses, and their number is floor(k) or ceil(k) for the line's k alternate-counter resets");
    g.expect_if(pre1, s1, "P12-1-SPR: with DUAL set every graded line shows more than 2 UHRES sprite pointer accesses");
    // P12-PHASE: the chain starts at the resets of the alternate counter. The counter is written with vA in one run and with vB in
    // another, the programs being the same and the runs cycle-aligned, so that its resets, and with them the chains, move by
    // (vA - vB) colour clocks if the chain starts at them.
    // the values the page-faithful run writes and the difference HHPOSR must show; the contrast of P12-HHW changes only the value written
    const int vA = 0x08, vB = 0x1C, vW = mutating("P12-HHW") ? 0x08 : vB;
    const char* phprog = mutating("P12-PHFIXTURE") ? "prog_uhphase_nofix" : "prog_uhphase";
    UhRun qa, qb, qh;
    PhaseData pa, pb;
    if (pre1) {
        qa = uh_run(0x0060, 0x0080, uh_tc, phprog, {}, false, vA);
        qb = uh_run(mutating("P12-PHASE") ? 0x0020 : 0x0060, 0x0080, uh_tc, phprog, {}, false, vW);   // P12-PHASE: DUAL clear in the second run
        // P12-HHW is measured on a pair of runs with DUAL set: when the second run has DUAL clear, a third run with DUAL set takes its place
        qh = mutating("P12-PHASE") ? uh_run(0x0060, 0x0080, uh_tc, phprog, {}, false, vW) : qb;
        pa = phase_data(qa); pb = phase_data(qb);
    }
    bool phfix = qa.fixture && qb.fixture && qh.fixture;
    g.expect_if(pre1, phfix, "P12-PHFIXTURE: the program that writes the alternate counter stores the constant $BEEF first and the harness reads it back before any other result of its runs is used");
    int rba = qa.r ? qa.r->word(kResults + 0x100) : 0, rbb = qh.r ? qh.r->word(kResults + 0x100) : 0;
    checks.note("P12 measured: HHPOSR read right after the write of HHPOSW: " + dec(rba) + " after writing " + dec(vA) + ", " + dec(rbb) + " after writing " + dec(vW));
    bool hhw = phfix && (rbb - rba == vB - vA);
    g.expect_if(pre1 && phfix, hhw, "P12-HHW: a write to HHPOSW sets the alternate counter: HHPOSR read right after the write differs between the two runs by the difference of the two values written");
    int dsa = 0, dsb = 0;
    std::vector<long long> ca = phase_delays(pa, vA, uh_tc, 0x38, &dsa), cb = phase_delays(pb, vB, uh_tc, 0x38, &dsb);
    bool phase_ok = !ca.empty() && !cb.empty();
    for (long long c : ca) if (c != ca[0]) phase_ok = false;
    for (long long c : cb) if (!ca.empty() && c != ca[0]) phase_ok = false;
    checks.note("P12 measured: chains in the graded frame " + dec((long long)pa.t1.size()) + " and " + dec((long long)pb.t1.size()) + ", disturbed " + dec(dsa) + " and " + dec(dsb) +
                ", delay of the first undisturbed chain after the predicted reset " + (ca.empty() ? std::string("n/a") : dec(ca[0])) + " and " + (cb.empty() ? std::string("n/a") : dec(cb[0])) + " system clocks");
    g.expect_if(pre1 && phfix && hhw, phase_ok, "P12-PHASE: the chains that wait for no busy cycle start the same time after the predicted resets of the alternate counter in both runs: with the counter written with $08 and with $1C, every such chain in the graded frame, at least one in each run, starts the same number of system clocks after the reset that follows from the value written");
    g.expect_if(pre1 && phfix && hhw, c0 && b1 && s1 && phase_ok,
        "P12: opus:AA-beamcon0.c31 opus:AA-beamcon0.c32 sol:AA-beamcon0.c48 sol:AA-beamcon0.c49: with UHRES enabled and DUAL set the UHRES pointer chain starts at each reset of the alternate counter, so the UHRES bit-plane and sprite pointers come out more than once per main line, against once and twice per line with DUAL clear");
}

// ---- P9 and P10: the variable sync outputs (F5, F6) --------------------------

// One run of prog_beam with a dry-run sample frame, the register set of F5 and
// four stopped frames; its graded frame's hs and vs are kept.
struct SyncSet {
    uint16_t bc0 = 0x00A0;
    uint16_t hsstrt = 0x20, hsstop = 0x30, hcenter = 0x91, vsstop = 103, vbstrt = 150;
    bool fix = true;
    bool dma = false;       // a bit plane is fetched: accesses the refresh filter cannot explain
    uint16_t htotal = 0x00E2;
    uint16_t bplcon0 = 0;   // written before BEAMCON0 when non-zero (LACE for the long-field mutations)
    std::string key() const {
        return hexs(bc0, 4) + "-" + dec(hsstrt) + "-" + dec(hsstop) + "-" + dec(hcenter) + "-" + dec(vsstop) + "-" + dec(vbstrt) +
               (fix ? "" : "-nofix") + (dma ? "-dma" : "") + "-ht" + dec(htotal) + "-bp" + dec(bplcon0);
    }
};

struct SyncRun {
    BeamRun b;
    bool ok = false;                   // the graded frame and its lines exist
    LineData ln;                       // refresh groups and line periods of the graded frame
    std::vector<uint8_t> vid;          // bit 0 hs, bit 1 vs, one per system clock from t0
    uint64_t t0 = 0;
    long long tc = 0;
};

std::map<std::string, std::shared_ptr<SyncRun>> g_sync_cache;

std::shared_ptr<SyncRun> sync_run(const SyncSet& s) {
    auto it = g_sync_cache.find(s.key());
    if (it != g_sync_cache.end()) return it->second;
    BeamOpts o;
    o.prog = s.fix ? "prog_beam" : "prog_beam_nofix";
    o.pre = true; o.post = 0; o.stops = 3; o.video = true;
    o.regs = {{kHTOTAL, s.htotal}, {kVTOTAL, 0x0138}, {kHSSTRT, s.hsstrt}, {kHSSTOP, s.hsstop}, {kHBSTRT, 0x0010},
              {kHBSTOP, 0x0081}, {kHCENTER, s.hcenter}, {kVSSTRT, 100}, {kVSSTOP, s.vsstop}, {kVBSTRT, s.vbstrt},
              {kVBOSTOP, 170}, {kBEAMCON0, s.bc0}};
    if (s.dma) { auto x = dma_regs(); o.regs.insert(o.regs.begin(), x.begin(), x.end()); }
    if (s.bplcon0) o.regs.insert(o.regs.begin(), {0x100, s.bplcon0});
    auto r = std::make_shared<SyncRun>();
    r->b = beam_run(o);
    r->tc = colour_clock(r->b.pre);
    r->b.tc = r->tc;
    r->vid = r->b.r->vid;
    r->t0 = r->b.r->vid_t0;
    r->ln = line_data(r->b, 1, 2);
    r->ok = r->b.fixture && !r->vid.empty() && r->ln.anchor && r->ln.frame_ok && r->tc > 0;
    g_sync_cache[s.key()] = r;
    return r;
}

struct Itv { size_t a, b; };   // [a,b) in samples from the start of the graded frame

std::vector<Itv> diff_itv(const SyncRun& x, const SyncRun& y, int bit) {
    std::vector<Itv> out;
    size_t n = std::min(x.vid.size(), y.vid.size());
    bool in = false; size_t a = 0;
    for (size_t i = 0; i < n; ++i) {
        bool d = ((x.vid[i] ^ y.vid[i]) >> bit) & 1;
        if (d && !in) { in = true; a = i; }
        if (!d && in) { in = false; out.push_back({a, i}); }
    }
    if (in) out.push_back({a, n});
    return out;
}

// F6: the paired runs are cycle-aligned: their refresh groups start at the same times
bool aligned(const SyncRun& x, const SyncRun& y) {
    if (x.ln.groups.size() != y.ln.groups.size() || x.ln.groups.empty()) return false;
    for (size_t i = 0; i < x.ln.groups.size(); ++i)
        if (x.ln.groups[i].t != y.ln.groups[i].t) return false;
    return x.b.r->vid_t0 == y.b.r->vid_t0;
}

// the line (index into the refresh groups) containing sample i of the graded frame
int line_of(const SyncRun& x, size_t i) {
    uint64_t t = x.t0 + i;
    int idx = -1;
    for (size_t k = 0; k < x.ln.groups.size(); ++k) if (x.ln.groups[k].t <= t) idx = (int)k;
    return idx;
}

std::string itv_note(const char* what, const std::vector<Itv>& v, const SyncRun& x) {
    std::string s = std::string(what) + ": " + dec((long long)v.size()) + " intervals";
    for (size_t i = 0; i < v.size() && i < 4; ++i)
        s += " [line " + dec(line_of(x, v[i].a)) + ", length " + dec((long long)(v[i].b - v[i].a)) + "]";
    return s;
}

// the active interval of vs in a run: the interval at the minority level
std::optional<Itv> vs_active(const SyncRun& x) {
    size_t hi = 0;
    for (auto v : x.vid) if (v & 2) ++hi;
    int lvl = (hi * 2 < x.vid.size()) ? 1 : 0;   // the level that is held for the shorter time
    std::optional<Itv> best;
    bool in = false; size_t a = 0;
    for (size_t i = 0; i <= x.vid.size(); ++i) {
        bool m = i < x.vid.size() && (((x.vid[i] >> 1) & 1) == lvl);
        if (m && !in) { in = true; a = i; }
        if (!m && in) { in = false; if (!best || i - a > best->b - best->a) best = Itv{a, i}; }
    }
    return best;
}

long long median_line(const SyncRun& x) {
    std::vector<long long> l = x.ln.lines;
    if (l.empty()) return 0;
    std::sort(l.begin(), l.end());
    return l[l.size() / 2];
}

// H pair: on every chip line the two runs differ in one interval of exactly len_cck colour clocks
bool h_pair_every_line(const SyncRun& x, const SyncRun& y, long long len_cck, std::vector<Itv>* out = nullptr) {
    std::vector<Itv> d = diff_itv(x, y, 0);
    if (out) *out = d;
    if (d.empty() || !aligned(x, y)) return false;
    std::map<int, int> per;
    for (const auto& i : d) {
        if ((long long)(i.b - i.a) != len_cck * x.tc) return false;
        per[line_of(x, i.a)]++;
    }
    // every chip line but the first and the last of the window has exactly one
    for (size_t k = 1; k + 1 < x.ln.groups.size(); ++k)
        if (per[(int)k] != 1) return false;
    for (const auto& kv : per) if (kv.second != 1) return false;
    return true;
}

bool inverted(const SyncRun& x, const SyncRun& y, int bit) {
    size_t n = std::min(x.vid.size(), y.vid.size());
    if (!n || !aligned(x, y)) return false;
    for (size_t i = 0; i < n; ++i) if ((((x.vid[i] ^ y.vid[i]) >> bit) & 1) == 0) return false;
    return true;
}

// V pair on vs: exactly one differing interval of exactly 2*T_L
bool v_pair_one_interval(const SyncRun& x, const SyncRun& y, std::vector<Itv>* out = nullptr) {
    std::vector<Itv> d = diff_itv(x, y, 1);
    if (out) *out = d;
    if (d.size() != 1 || !aligned(x, y)) return false;
    int li = line_of(x, d[0].a);
    if (li < 0 || li + 1 >= (int)x.ln.lines.size()) return false;
    long long two = x.ln.lines[li] + x.ln.lines[li + 1];
    long long len = (long long)(d[0].b - d[0].a);
    // the two lines of the interval are those it starts in and the next
    return len == two || len == x.ln.lines[li] + x.ln.lines[li > 0 ? li - 1 : li];
}

void proposition_p9_p10() {
    bool w9 = want("P9"), w10 = want("P10"), wf5 = want("F5");
    if (!w9 && !w10 && !wf5) return;
    if (!g_cfg_ok) { checks.record("NOT GRADED: F5, P9 and P10 [CONFIG does not hold: the harness is not configured as the specification requires]"); return; }
    // Which F5 runs? the mutations of the F5 anchors change one of them.
    SyncSet h1; h1.bc0 = 0x01A0;
    SyncSet h2 = h1; h2.hsstop = 0x34;
    SyncSet h3; h3.bc0 = 0x01A1;
    SyncSet v1; v1.bc0 = 0x02A0;
    SyncSet v2 = v1; v2.vsstop = 105;
    SyncSet v3; v3.bc0 = 0x02A2;
    SyncSet r0; r0.bc0 = 0x00A0;
    if (mutating("F5-SH-DIFF")) h2 = h1;
    if (mutating("F5-SH-INV")) h3.bc0 = 0x01A0;
    if (mutating("F5-SV-DIFF")) v2 = v1;
    if (mutating("F5-SV-INV")) v3.bc0 = 0x02A0;
    if (mutating("F5-FIXTURE")) h1.fix = false;
    if (mutating("F5-LINES")) h1.dma = true;
    if (mutating("F5-ALIGN")) h2.htotal = 0x00E0;   // lines two colour clocks shorter: the paired runs' groups no longer start together

    auto rh1 = sync_run(h1), rh2 = sync_run(h2), rh3 = sync_run(h3), rv1 = sync_run(v1), rv2 = sync_run(v2), rv3 = sync_run(v3), rr0 = sync_run(r0);
    g.expect(rh1->b.fixture && rh2->b.fixture && rh3->b.fixture && rv1->b.fixture && rv2->b.fixture && rv3->b.fixture && rr0->b.fixture,
             "F5-FIXTURE: the program stores the constant $BEEF first and the harness reads it back before any other result is used");
    g.expect(rh1->ok && rh2->ok && rh3->ok && rv1->ok && rv2->ok && rv3->ok && rr0->ok,
             "F5-LINES: in the graded frame of every anchor run the refresh accesses come in groups of exactly four, every line period is a whole number of colour clocks, and the frame holds its lines");
    bool al = aligned(*rh1, *rh2) && aligned(*rh1, *rh3) && aligned(*rv1, *rv2) && aligned(*rv1, *rv3) && aligned(*rv1, *rr0);
    g.expect(al, "F5-ALIGN: the paired runs of the anchors are cycle-aligned: their refresh groups in the graded frame start at the same times");

    std::vector<Itv> dh, dv;
    bool sh_diff = h_pair_every_line(*rh1, *rh2, 4, &dh);
    checks.note("F5 measured: " + itv_note("S-H1 against S-H2, hs", dh, *rh1) + "; T_C = " + dec(rh1->tc) + ", lines in the frame " + dec((long long)rh1->ln.lines.size()));
    g.expect_anchor(sh_diff, "F5-SH-DIFF: S-H1 against S-H2 (HSSTOP $30 against $34): the observed horizontal sync differs on every chip line in one interval of exactly 4 colour clocks and is identical elsewhere");
    bool sh_inv = inverted(*rh3, *rh1, 0);
    g.expect_anchor(sh_inv, "F5-SH-INV: S-H3 against S-H1 (HSYTRUE set): the observed horizontal sync is the logical inverse at every sample");
    bool sv_diff = v_pair_one_interval(*rv1, *rv2, &dv);
    checks.note("F5 measured: " + itv_note("S-V1 against S-V2, vs", dv, *rv1));
    // the vertical sync is passed through to the harness, so a failure of S-V1 against S-V2 is the chip's, not the harness's: FAIL
    g.expect_if(rv1->ok && rv2->ok && aligned(*rv1, *rv2), sv_diff, "F5-SV-DIFF: S-V1 against S-V2 (VSSTOP 103 against 105): the observed vertical sync differs in exactly one interval, of exactly two line periods, and is identical elsewhere");
    // S-V3 is a line of its own: the VSY* pin is the vertical sync as the chip outputs it, and VSYTRUE changes its polarity
    bool sv_inv = inverted(*rv3, *rv1, 1);
    g.expect_if(rv1->ok && rv3->ok && aligned(*rv1, *rv3), sv_inv,
                "F5-SV-INV: p.13 \"HSYTRUE,VSYTRUE,CSYTRUE= These change the polarity of the HSY*, VSY*, & CSY* pins\": S-V3 against S-V1 (VSYTRUE set): the observed vertical sync is inverted at every sample");
    auto vvar = vs_active(*rv1);
    auto vnorm = vs_active(*rr0);
    checks.note("F5 measured: V_var length " + (vvar ? dec((long long)(vvar->b - vvar->a)) : std::string("n/a")) + " system clocks starting in line " +
                (vvar ? dec(line_of(*rv1, vvar->a)) : std::string("n/a")) + "; V_norm length " + (vnorm ? dec((long long)(vnorm->b - vnorm->a)) : std::string("n/a")) +
                " starting in line " + (vnorm ? dec(line_of(*rr0, vnorm->a)) : std::string("n/a")));
    bool s_h = rh1->ok && rh2->ok && rh3->ok && al && sh_diff && sh_inv;
    // anchor S-V: S-V1 against S-V2 differ in one interval of two line periods and V_var and V_norm exist
    bool s_v = rv1->ok && rv2->ok && rr0->ok && aligned(*rv1, *rv2) && aligned(*rv1, *rr0) && sv_diff && vvar && vnorm;
    if (!s_h) checks.record("NOT CHECKABLE: F5-SH: the HSY* parts of P9 and P10, since anchor S-H does not hold in this harness");
    if (!s_v) checks.record("NOT CHECKABLE: F5-SV: the VSY* parts of P9 and P10, since anchor S-V does not hold in this harness");
    if (!w9 && !w10) return;
    // when neither part is checkable the runs of P9 and P10 are not taken: every line below is not graded
    const bool any = s_h || s_v;
    auto L = [&](const SyncSet& x) { return any ? sync_run(x) : std::make_shared<SyncRun>(); };

    // ---- P9 ----
    SyncSet c_h1; c_h1.bc0 = 0x04A0;
    SyncSet c_h2 = c_h1; c_h2.hsstrt = 0x24;
    SyncSet c_c2 = c_h1; c_c2.hcenter = 0x99;
    SyncSet c_v2 = c_h1; c_v2.vbstrt = 152;
    // the CSCBEN-clear pairs: each pair has its own base run, so that the mutation of one pair moves only its own line
    SyncSet n_base; n_base.bc0 = 0x00A0;
    SyncSet nh1 = n_base, nh2 = n_base; nh2.hsstrt = 0x24;
    SyncSet nc1 = n_base, nc2 = n_base; nc2.hcenter = 0x99;
    SyncSet nv1 = n_base, nv2 = n_base; nv2.vbstrt = 152;
    if (mutating("P9-AH")) c_h2 = c_h1;
    if (mutating("P9-AC")) c_c2 = c_h1;
    if (mutating("P9-BV") || mutating("P9-B") || mutating("P9-ID")) c_v2 = c_h1;
    // VARVSYEN and VARHSYEN set in the compared runs: HSSTRT is the horizontal position of the variable VSY on short fields (P9-NH);
    // and with interlace HCENTER is its horizontal position on long fields (P9-NC)
    if (mutating("P9-NH")) nh1.bc0 = nh2.bc0 = 0x03A0;
    if (mutating("P9-NC")) { nc1.bc0 = nc2.bc0 = 0x03A0; nc1.bplcon0 = nc2.bplcon0 = 0x0004; }
    if (mutating("P9-NV")) nv1.bc0 = nv2.bc0 = 0x04A0;
    if (w9) {
        auto a1 = L(c_h1), a2 = L(c_h2), cc2 = L(c_c2), b2 = L(c_v2);
        std::vector<Itv> dA, dC, dB;
        bool ah_ok = h_pair_every_line(*a1, *a2, 4, &dA);
        if (any) checks.note("P9 measured: " + itv_note("CSCBEN set, HSSTRT $20 against $24, hs", dA, *a1));
        // C pair: differing lines consecutive, each in one interval of exactly 8 colour clocks
        dC = diff_itv(*a1, *cc2, 0);
        bool c_ok = !dC.empty() && aligned(*a1, *cc2);
        std::set<int> clines;
        for (const auto& i : dC) {
            if ((long long)(i.b - i.a) != 8 * a1->tc) c_ok = false;
            clines.insert(line_of(*a1, i.a));
        }
        bool consec = !clines.empty() && (long long)clines.size() == (long long)(*clines.rbegin() - *clines.begin() + 1) && clines.size() == dC.size();
        long long nvar = vvar ? 3 : -1;
        long long lnorm = (vnorm && median_line(*rr0) > 0) ? (long long)(vnorm->b - vnorm->a) / median_line(*rr0) : -1;
        long long lnorm_hi = (vnorm && median_line(*rr0) > 0) ? ((long long)(vnorm->b - vnorm->a) + median_line(*rr0) - 1) / median_line(*rr0) : -1;
        bool count_ok = (long long)clines.size() == nvar || (long long)clines.size() == lnorm || (long long)clines.size() == lnorm_hi;
        bool ac_ok = c_ok && consec && count_ok;
        if (any) checks.note("P9 measured: " + itv_note("CSCBEN set, HCENTER $91 against $99, hs", dC, *a1) + "; differing lines " + dec((long long)clines.size()) +
                    " (V_var gives 3, V_norm gives " + dec(lnorm) + " or " + dec(lnorm_hi) + ")");
        // V pair on vs
        dB = diff_itv(*a1, *b2, 1);
        bool bv_ok = !dB.empty() && aligned(*a1, *b2);
        if (bv_ok && vvar) {
            size_t first = dB.front().a, last = dB.back().b;
            long long tl = median_line(*a1);
            // The blank starts in line 150 at a horizontal position the page does not fix, and V_var starts
            // in line 100 at HSSTRT (p.12, short fields), so the two starts lie between 49 and 51 line
            // periods apart.
            long long lo = (long long)vvar->a + 49 * tl;
            bv_ok = (long long)first >= lo && (long long)first < lo + 2 * tl && (long long)(last - first) <= 2 * tl;
            bool half1 = false, half2 = false;
            for (const auto& i : dB) {
                if ((long long)i.a < (long long)first + tl) half1 = true;
                if ((long long)i.b > (long long)first + tl) half2 = true;
            }
            bv_ok = bv_ok && half1 && half2;
        } else bv_ok = false;
        if (any) checks.note("P9 measured: " + itv_note("CSCBEN set, VBSTRT 150 against 152, vs", dB, *a1));
        else checks.record("P9 and P10: the runs are not taken, since neither anchor S-H nor S-V holds");
        // CSCBEN clear: each pair identical at every sample, on both pins
        // P9-NH and P9-NC assert identity at every sample: a deterministic scan doubler keeps identical inputs identical,
        // so they do not need anchor S-H and their runs are always taken
        auto bh1 = sync_run(nh1), bh2 = sync_run(nh2), bc1 = sync_run(nc1), bc2 = sync_run(nc2), bv1 = L(nv1), bv2 = L(nv2);
        // the vertical sync reaches the harness unchanged, so identity of VSY* is observed; the HSY* halves are not observable (the scan doubler regenerates hs)
        auto same = [&](const SyncRun& x, const SyncRun& y) { return aligned(x, y) && diff_itv(x, y, 1).empty(); };
        bool nh_ok = same(*bh1, *bh2), nc_ok = same(*bc1, *bc2), nv_ok = same(*bv1, *bv2);
        bool pre_a = s_h && a1->ok && a2->ok && cc2->ok && aligned(*a1, *a2) && aligned(*a1, *cc2) && vvar && vnorm;
        bool pre_b = s_v && a1->ok && b2->ok && aligned(*a1, *b2) && vvar && vnorm;
        g.expect_if(pre_a, ah_ok, "P9-AH: with CSCBEN set and VARHSYEN and VARVSYEN clear, changing HSSTRT from $20 to $24 changes the HSY* output on every chip line, in one interval of exactly 4 colour clocks");
        g.expect_if(pre_a, ac_ok, "P9-AC: with CSCBEN set, changing HCENTER from $91 to $99 changes the HSY* output on consecutive chip lines only, as many as there are lines in vertical sync, each in one interval of exactly 8 colour clocks");
        bool pre_nh = s_v && bh1->ok && bh2->ok, pre_nc = s_v && bc1->ok && bc2->ok;
        checks.record("NOT OBSERVABLE: the HSY* halves of P9-NH, P9-NC and P9-NV: the harness shows hs only after the scan doubler, which regenerates it, so identity of the hs samples cannot show that HSSTRT, HCENTER or VBSTRT does not reach the HSY* pin");
        g.expect_if(pre_nh, nh_ok, "P9-NH: with CSCBEN clear, changing HSSTRT from $20 to $24 does not change the VSY* output at any sample (the HSY* half is not observable at this boundary)");
        g.expect_if(pre_nc, nc_ok, "P9-NC: with CSCBEN clear, changing HCENTER from $91 to $99 does not change the VSY* output at any sample (the HSY* half is not observable at this boundary)");
        g.expect_if(pre_b, bv_ok, "P9-BV: with CSCBEN set, changing VBSTRT from 150 to 152 changes the VSY* output only inside one interval of at most two line periods that starts between 49 and 51 line periods after the start of V_var, each of its two line periods holding differing samples");
        g.expect_if(s_v && bv1->ok && bv2->ok, nv_ok, "P9-NV: with CSCBEN clear, changing VBSTRT from 150 to 152 does not change the VSY* output at any sample (the HSY* half is not observable at this boundary)");
        bool a_all = ah_ok && ac_ok && nh_ok && nc_ok, b_all = bv_ok && nv_ok;
        g.expect_if(pre_a, a_all,
            "P9-A: opus:AA-beamcon0.c15 sol:AA-beamcon0.c33 opus:AA-beamcon0.c38: with CSCBEN set the variable composite sync comes out on the HSY* pin: set on HSSTRT on every line and on HCENTER in vertical sync; with CSCBEN clear neither register reaches the VSY* pin (the HSY* half is not observable at this boundary)");
        g.expect_if(pre_b, b_all,
            "P9-B: opus:AA-beamcon0.c16 sol:AA-beamcon0.c34: with CSCBEN set the variable composite blank comes out on the VSY* pin and follows VBSTRT; with CSCBEN clear VBSTRT does not reach the VSY* pin");
        g.expect_if(pre_a || pre_b, (!pre_a || a_all) && (!pre_b || b_all),
            "P9-ID: opus:AA-beamcon0.b10 sol:AA-beamcon0.c11: BEAMCON0 bit 10 is CSCBEN: setting it routes the variable composite sync to HSY* and the variable composite blank to VSY*, on whichever pin this harness can check");
    }

    // ---- P10 ----
    if (w10) {
        // each pair has its own base run, so that the mutation of one pair moves only its own line
        SyncSet hc1; hc1.bc0 = 0x05A0;
        SyncSet hc2 = hc1; hc2.hcenter = 0x99;
        SyncSet hs1 = hc1, hs2 = hc1; hs2.hsstop = 0x34;
        SyncSet wv1; wv1.bc0 = 0x06A0;
        SyncSet wv2 = wv1; wv2.vbstrt = 152;
        SyncSet ws1 = wv1, ws2 = wv1; ws2.vsstop = 105;
        if (mutating("P10-CPAIR")) hc2.hsstop = 0x34;
        if (mutating("P10-SPAIR")) hs2.hsstop = 0x30;
        if (mutating("P10-VPAIR")) wv1.bc0 = wv2.bc0 = 0x04A0;
        if (mutating("P10-VSPAIR")) ws2.vsstop = 103;
        // P10-CPAIR asserts identity at every sample and does not need anchor S-H: its runs are always taken
        auto p1 = sync_run(hc1), p2 = sync_run(hc2), ps1 = L(hs1), p3 = L(hs2), q1 = L(wv1), q2 = L(wv2), qs1 = L(ws1), q3 = L(ws2);
        bool cpair = aligned(*p1, *p2) && diff_itv(*p1, *p2, 0).empty();
        std::vector<Itv> dS, dVS;
        bool spair = h_pair_every_line(*ps1, *p3, 4, &dS);
        bool vpair = aligned(*q1, *q2) && diff_itv(*q1, *q2, 0).empty() && diff_itv(*q1, *q2, 1).empty();
        bool vspair = v_pair_one_interval(*qs1, *q3, &dVS);
        if (any) checks.note("P10 measured: " + itv_note("VARHSYEN, HSSTOP $30 against $34, hs", dS, *ps1) + "; " + itv_note("VARVSYEN, VSSTOP 103 against 105, vs", dVS, *qs1));
        checks.note("P10 measured: " + itv_note("CSCBEN and VARHSYEN, HCENTER $91 against $99, hs", diff_itv(*p1, *p2, 0), *p1) + "; " +
                    itv_note("same pair, vs", diff_itv(*p1, *p2, 1), *p1));
        bool pre_s = s_h && ps1->ok && p3->ok && aligned(*ps1, *p3), pre_c = p1->ok && p2->ok;
        bool pre_vp = s_v && q1->ok && q2->ok, pre_vs = s_v && qs1->ok && q3->ok && aligned(*qs1, *q3);
        const bool cpair_observable = false;   // HSY* is not observable at this boundary: the scan doubler regenerates hs
        bool pre_h = pre_s && pre_c && cpair_observable, pre_v = pre_vp && pre_vs;
        g.expect_if(cpair_observable, cpair, "P10-CPAIR: with CSCBEN and VARHSYEN set, changing HCENTER from $91 to $99 changes nothing at the HSY* pin: the comparator HSY is independent of HCENTER (not observable at this boundary, since the scan doubler regenerates hs; the VSY* samples of the pair are a MEASURED line)");
        g.expect_if(pre_s, spair, "P10-SPAIR: with CSCBEN and VARHSYEN set, changing HSSTOP from $30 to $34 changes the HSY* output on every chip line, vertical-sync lines included, in one interval of exactly 4 colour clocks");
        g.expect_if(pre_vp, vpair, "P10-VPAIR: with CSCBEN and VARVSYEN set, changing VBSTRT from 150 to 152 changes nothing at any sample: the comparator VSY is independent of VBSTRT");
        g.expect_if(pre_vs, vspair, "P10-VSPAIR: with CSCBEN and VARVSYEN set, changing VSSTOP from 103 to 105 changes the VSY* output in exactly one interval of exactly two line periods");
        g.expect_if(pre_h && pre_v, cpair && spair && vpair && vspair,
            "P10: opus:AA-beamcon0.c19 sol:AA-beamcon0.c35: VARHSYEN and VARVSYEN have priority over CSCBEN: with CSCBEN set HSY* carries the comparator HSY (set on HSSTRT, reset on HSSTOP on every line, independent of HCENTER) and VSY* the comparator VSY (independent of VBSTRT)");
    }
}

// ---- P8 ---------------------------------------------------------------------

constexpr uint32_t kLcfg = 0x0B8000;
constexpr uint32_t kLpBuf = 0x1B0000;
constexpr uint16_t kMLpGo = 0xF010;

struct LpRun {
    std::shared_ptr<RunData> r;
    std::vector<Sm> s;
    bool fixture = false;
    uint64_t t_go = 0;
};

LpRun lp_run(uint16_t bc0, int nsamp, uint16_t bplcon0, bool pulse, uint64_t delay, uint64_t len,
             const char* prog = "prog_lpen", uint64_t delay2 = 0, bool drive = true) {
    RunOpts o;
    o.prog = prog;
    o.pulse2_delay = delay2;
    o.pulse_drive = drive;
    o.pokes = {{kLcfg + 0, bc0}, {kLcfg + 2, (uint16_t)(nsamp - 1)}, {kLcfg + 4, bplcon0}};
    o.pulse = pulse; o.pulse_marker = kMLpGo; o.pulse_delay = delay; o.pulse_len = len;
    LpRun l;
    l.r = run_opts(o);
    l.fixture = l.r->loaded && l.r->done && l.r->word(kResults) == 0xBEEF;
    l.s = samples_of(*l.r, kLpBuf, kLpBuf + 0x20000, true, true);
    auto g1 = find_mark(*l.r, kMLpGo);
    if (g1) l.t_go = l.r->marks[*g1].cyc;
    return l;
}

bool vh_equal(const Sm& a, const Sm& b) { return a.V == b.V && a.H == b.H; }
bool vh_less(const Sm& a, const Sm& b) { return a.V < b.V || (a.V == b.V && a.H < b.H); }

void proposition_p8() {
    if (!want("P8")) return;
    if (!g_cfg_ok) { checks.record("NOT GRADED: P8 [CONFIG does not hold: the harness is not configured as the specification requires]"); return; }
    // the dry run: the program as P8-0 without the pulse
    LpRun dry = lp_run(0x0020, 120, 0x0008, false, 0, 0, mutating("P8-FIXTURE") ? "prog_lpen_nofix" : "prog_lpen");
    // the fixture is asserted before any sample of the dry run is printed or used
    g.expect(dry.fixture, "P8-FIXTURE: the program stores the constant $BEEF first and the harness reads it back before any other result is used");
    if (!dry.fixture) { checks.record("NOT GRADED: P8: the fixture does not hold, so no sample of the dry run is used"); return; }
    std::vector<Sm> ds = dry.s;
    long long tc = colour_clock(ds);
    // fit D and N: the pulse falls on line 255, three samples before its last sample
    int iL = -1;
    for (size_t i = 0; i < ds.size(); ++i) if (ds[i].V == 255) iL = (int)i;
    bool fit_ok = iL >= 7 && tc > 0 && dry.t_go > 0;
    int ip = fit_ok ? (mutating("P8-FIT") ? 2 : iL - 3) : 0;
    int nsamp = fit_ok ? iL + 14 : 0;
    if (nsamp > 120) fit_ok = false;
    uint64_t delay = fit_ok ? (ds[ip].tend + 2 - dry.t_go) : 0;
    long long interval = 0;   // the median store-time difference of consecutive samples, in system clocks
    if (ds.size() > 8) {
        std::vector<long long> d;
        for (size_t i = 1; i < ds.size(); ++i) d.push_back((long long)(ds[i].t - ds[i - 1].t));
        std::sort(d.begin(), d.end());
        interval = d[d.size() / 2];
    }
    // the pulse lasts one sample interval and 2*T_C, so that at least one read of the CIA fire inputs falls inside it
    uint64_t plen = fit_ok ? (uint64_t)(interval + 2 * tc) : 0;
    checks.note("P8 measured: colour clock T_C = " + dec(tc) + " system clocks, sample interval = " + dec(interval) +
                " system clocks, " + dec((long long)ds.size()) + " dry-run samples, last line-255 sample index " + dec(iL) +
                ", pulse delay D = " + dec((long long)delay) + ", pulse length = " + dec((long long)plen) + ", samples N = " + dec(nsamp));

    {
        std::string lst;
        for (size_t i = 0; i < ds.size() && i < 24; ++i)
            lst += " " + dec(ds[i].V) + ":" + dec(ds[i].H) + ":" + hexs(ds[i].hh, 4) + "@" + dec((long long)(ds[i].t - dry.t_go));
        checks.note("P8 measured: first dry-run samples V:H:HHPOSR@time-after-LP_GO:" + lst);
    }
    // preconditions on the dry run
    bool crosses = false;
    for (size_t i = 1; i < ds.size(); ++i) if (ds[i - 1].V == 255 && ds[i].V == 256) crosses = true;
    checks.note("P8 measured: dry run over all " + dec((long long)ds.size()) + " samples: F4 violations = " + dec(f4_violations(ds, false)) + ", line wraps with V not yet stepped = " +
                dec(f4_lag(ds)) + ", crosses 255 to 256 = " + dec(crosses ? 1 : 0) +
                ", last sample V:H = " + (ds.empty() ? std::string("n/a") : dec(ds.back().V) + ":" + dec(ds.back().H)));
    LpRun dry_anchor = dry;
    if (mutating("P8-ANCHOR")) dry_anchor = lp_run(0x0020, nsamp > 0 ? nsamp : 120, 0x0008, false, 0, 0, "prog_lpen_rev");
    // the anchor is checked over the samples of the window the pulse runs take
    if (nsamp > 0 && (int)dry_anchor.s.size() > nsamp) dry_anchor.s.resize(nsamp);
    int viol_a = f4_violations(dry_anchor.s, false);
    g.expect(!dry_anchor.s.empty() && viol_a == 0 && crosses,
             "P8-ANCHOR: in the dry run (no pulse) the samples of the window show the counters running: V never decreases and H increases within a line, and the samples cross from V = 255 to V = 256");
    bool fit_geom = fit_ok;
    if (fit_ok) {
        // the pulse falls on line 255 and at least 4 samples fall before it and 8 after it
        fit_geom = ds[ip].V == 255 && ds[ip + 1].V == 255 && ip >= 4 &&
                   (int)ds.size() - ip - 3 >= 8 && ds[ds.size() - 1].V >= 256;
    }
    g.expect(fit_geom, "P8-FIT: the pulse of the dry run's timing falls on line 255 with at least 4 samples before it and 8 after it, and the samples after it reach V >= 256");

    bool pre_base = dry.fixture && fit_geom && !dry_anchor.s.empty() && viol_a == 0 && crosses;

    // the pulse runs
    // P8-TRIGGER: the pulse is scheduled and timed as in the normal run, and the fire inputs are not driven
    const bool pulse_on = true, drive_on = !mutating("P8-TRIGGER");
    const char* pulse_prog = mutating("P8-ALIGN") ? "prog_lpen_slow" : "prog_lpen";
    // P8-0-HH swaps LPENDIS between the two pulse runs (the latch is then in the P8-1 run and the
    // P8-0 run reads the running counters); P8-1-VH, P8-1-HH and P8 clear it in the P8-1 run
    uint16_t bc0_for_0 = mutating("P8-0-HH") ? 0x2020 : 0x0020;
    uint16_t bc0_for_1 = (mutating("P8-0-HH") || mutating("P8-1-VH") || mutating("P8-1-HH") || mutating("P8")) ? 0x0020 : 0x2020;
    // P8-0-VH places the P8-0 pulse on line 256: the first sample of the dry run on that line
    uint64_t delay0 = delay;
    if (mutating("P8-0-VH") && fit_geom)
        for (size_t i = 0; i + 1 < ds.size(); ++i)
            if (ds[i].V == 256) { delay0 = ds[i + 1].tend + 2 - dry.t_go; break; }
    LpRun p0 = fit_geom ? lp_run(bc0_for_0, nsamp, 0x0008, pulse_on, delay0, plen, pulse_prog, 0, drive_on) : LpRun();
    LpRun p1 = fit_geom ? lp_run(bc0_for_1, nsamp, 0x0008, pulse_on, delay, plen, pulse_prog, 0, drive_on) : LpRun();

    auto split = [&](const LpRun& L, std::vector<Sm>& before, std::vector<Sm>& after) {
        before.clear(); after.clear();
        if (!L.r) return;
        for (const auto& x : L.s) {
            if (x.tend < L.r->pulse_start) before.push_back(x);
            else if (x.t > L.r->pulse_end + (uint64_t)interval) after.push_back(x);
        }
    };
    std::vector<Sm> b0, a0s, b1, a1s;
    split(p0, b0, a0s);
    split(p1, b1, a1s);
    auto frozen = [&](const std::vector<Sm>& a) {
        if (a.size() < 2) return false;
        for (const auto& x : a) if (!vh_equal(x, a[0])) return false;
        return true;
    };

    // P8-TRIGGER: the pulse reaches pin 6 of the controller ports, shown by the program's own read of the CIA-A fire inputs
    // (HRM p.339: CIA-A PRA bits 7 and 6 are /FIR1 and /FIR0, low when the fire button is pressed): a sample read while the pulse
    // is driven shows both bits low, and no sample read before or after it does
    auto delivered = [&](const LpRun& L) {
        if (!L.r || L.s.empty() || L.r->pulse_start == 0) return false;
        bool in = false, ok = true;
        for (const auto& x : L.s) {
            bool pressed = (x.cia & 0xC0) == 0;
            if (x.tend < L.r->pulse_start || x.t > L.r->pulse_end + (uint64_t)interval) { if (pressed) ok = false; }
            else if (pressed) in = true;
        }
        return in && ok;
    };
    bool trig = delivered(p0) && delivered(p1);
    g.expect_if(fit_geom, trig, "P8-TRIGGER: the pulse reaches pin 6 of the controller ports: the CIA-A fire inputs (PRA bits 7 and 6) read low in a sample read during the pulse and high in every sample read before it or after it, in both pulse runs");

    // P8-ALIGN: the pulse runs are cycle-aligned with the dry run up to the pulse, so the dry run's samples date the running counters
    std::string align_diag;
    auto aligned_with_dry = [&](const LpRun& L, const std::vector<Sm>& before) {
        if (before.size() < 4) return false;
        for (size_t j = 0; j < before.size(); ++j) {
            if (j >= ds.size()) return false;
            if (before[j].t - L.t_go != ds[j].t - dry.t_go || !vh_equal(before[j], ds[j]) || before[j].hh != ds[j].hh) {
                if (align_diag.empty())
                    align_diag = "sample " + dec((long long)j) + ": time after LP_GO " + dec((long long)(before[j].t - L.t_go)) + " against " +
                                 dec((long long)(ds[j].t - dry.t_go)) + ", V:H:HHPOSR " + dec(before[j].V) + ":" + dec(before[j].H) + ":" + dec(before[j].hh) +
                                 " against " + dec(ds[j].V) + ":" + dec(ds[j].H) + ":" + dec(ds[j].hh);
                return false;
            }
        }
        return true;
    };
    bool align = fit_geom && aligned_with_dry(p0, b0) && aligned_with_dry(p1, b1);
    if (!align_diag.empty()) checks.note("P8 measured: the first sample before the pulse that differs from the dry run: " + align_diag);
    bool pulsed = fit_geom && p0.r && p1.r && p0.r->pulse_start && p1.r->pulse_start;
    g.expect_if(pulsed, align, "P8-ALIGN: before the pulse each sample of the pulse runs is stored at the same time after LP_GO as the dry run's sample of the same index and shows the same V, H and HHPOSR, so the dry run's samples date the running counters in the pulse runs");

    // LPE enables the latch: with BPLCON0 bit 3 clear the samples advance through the pulse
    bool lpe_ok = true;
    if (full("P8-LPE") && fit_geom) {
        LpRun c = lp_run(0x0020, nsamp, mutating("P8-LPE") ? 0x0008 : 0x0000, true, delay, plen);
        std::vector<Sm> bc, ac;
        split(c, bc, ac);
        lpe_ok = !ac.empty() && !frozen(ac) && f4_violations(ac, false) == 0;
        g.expect(lpe_ok, "P8-LPE: with BPLCON0 bit 3 (LPEN) clear the same pulse does not freeze the samples: they advance through and after it");
    } else checks.record("P8-LPE: not evaluated in this run");

    // the alternate counter runs in the dry run (HHPOSR takes at least two values) and rises across the pulse
    LpRun hh_run = dry;
    if (mutating("P8-HHRUN")) hh_run = lp_run(0x0020, 1, 0x0008, false, 0, 0);
    bool hh_moves = false;
    for (size_t i = 1; i < hh_run.s.size(); ++i) if (hh_run.s[i].hh != hh_run.s[0].hh) hh_moves = true;
    // the dry run's HHPOSR at the last sample before the pulse and at the first sample after it
    size_t ia0 = 0;
    for (size_t j = 0; j < p0.s.size(); ++j) if (!a0s.empty() && p0.s[j].t == a0s.front().t) ia0 = j;
    bool have_hh_idx = fit_geom && !b0.empty() && !a0s.empty() && ia0 < ds.size();
    int hh_lo = have_hh_idx ? (int)(ds[b0.size() - 1].hh & 0xFF) : 0, hh_hi = have_hh_idx ? (int)(ds[ia0].hh & 0xFF) : 0;
    bool hh_rises = have_hh_idx && hh_hi > hh_lo;
    g.expect_anchor(hh_moves && hh_rises,
        "P8-HHRUN: in the dry run HHPOSR takes at least two different values and is higher at the first sample after the pulse than at the last sample before it, so the alternate counter runs across the pulse and HHPOSR is graded");

    bool pre = dry.fixture && pre_base && trig && align && lpe_ok && !b0.empty() && !a0s.empty();
    bool pre_hh = pre && hh_moves && hh_rises;
    checks.note("P8 measured: P8-0 run: " + dec((long long)b0.size()) + " samples before the pulse, " + dec((long long)a0s.size()) +
                " after; P8-1 run: " + dec((long long)b1.size()) + " before, " + dec((long long)a1s.size()) + " after");
    if (!a0s.empty()) checks.note("P8 measured: P8-0 first sample after the pulse: V=" + dec(a0s.front().V) + " H=" + dec(a0s.front().H) +
                                  " HHPOSR=" + dec(a0s.front().hh) + "; last: V=" + dec(a0s.back().V) + " H=" + dec(a0s.back().H) + " HHPOSR=" + dec(a0s.back().hh));
    if (!a1s.empty()) checks.note("P8 measured: P8-1 first sample after the pulse: V=" + dec(a1s.front().V) + " H=" + dec(a1s.front().H) +
                                  " HHPOSR=" + dec(a1s.front().hh) + "; last: V=" + dec(a1s.back().V) + " H=" + dec(a1s.back().H) + " HHPOSR=" + dec(a1s.back().hh));
    if (have_hh_idx) checks.note("P8 measured: the dry run's HHPOSR is " + dec(hh_lo) + " at the last sample before the pulse and " + dec(hh_hi) + " at the first sample after it");

    // P8-0, VPOSR and VHPOSR
    bool v0 = !b0.empty() && frozen(a0s);
    if (v0) {
        const Sm& hit = a0s[0];
        const Sm& lb = b0.back();
        double elapsed_cck = double(p0.r->pulse_end - lb.t) / double(tc ? tc : 1);
        double interval_cck = double(interval) / double(tc ? tc : 1);
        v0 = !vh_less(hit, lb) && hit.V == 255 && lb.V == 255 &&
             double(hit.H) <= double(lb.H) + elapsed_cck + interval_cck;
        for (const auto& x : a0s) if ((x.vposr & 1) != 0) v0 = false;
    }
    // HHPOSR: every sample after the pulse is the latched value, and it lies between the running alternate counter
    // before the pulse and the running alternate counter after it, dated by the dry run
    bool h0 = !a0s.empty();
    for (const auto& x : a0s) if (x.hh != a0s[0].hh) h0 = false;
    if (h0) h0 = have_hh_idx && (int)(a0s[0].hh & 0xFF) >= hh_lo && (int)(a0s[0].hh & 0xFF) <= hh_hi;
    // P8-1, VPOSR and VHPOSR
    bool v1 = !p1.s.empty() && f4_violations(p1.s, false) == 0;
    if (v1) {
        const Sm& last = p1.s.back();
        v1 = last.V >= 256 && (last.vposr & 1) == 1;
    }
    bool h1 = false;
    for (size_t i = 1; i < a1s.size(); ++i) if (a1s[i].hh != a1s[i - 1].hh) h1 = true;
    g.expect_if(pre, v0, "P8-0-VH: with LPENDIS = 0, LPEN set and a light-pen trigger, VHPOSR and VPOSR return the position latched at the trigger: every sample after the pulse is one identical (V,H) on line 255, not earlier than the last sample before the pulse and not later than the running counter at the end of the pulse, and VPOSR bit 0 (V8) stays 0");
    g.expect_if(pre_hh, h0, "P8-0-HH: with LPENDIS = 0, LPEN set and a light-pen trigger, HHPOSR returns the latched value: every sample after the pulse is identical, and it is not lower than the dry run's HHPOSR at the last sample before the pulse and not higher than the dry run's at the first sample after it");
    g.expect_if(pre, v1, "P8-1-VH: with LPENDIS = 1 and the same trigger, VHPOSR and VPOSR return the running counters: (V,H) increases strictly from each sample to the next across the whole window, and the last sample has V >= 256 with VPOSR bit 0 (V8) set");
    g.expect_if(pre_hh, h1, "P8-1-HH: with LPENDIS = 1 and the same trigger, HHPOSR returns the running alternate counter: consecutive samples after the pulse are not all identical");
    // P8-VBREL: the freeze is released at the end of the internal vertical blank (HRM p.239: vertical position 20). The same
    // trigger, with the samples taken on into the next frame. Graded when a freeze took place (the samples after the trigger are
    // identical up to the wrap of V, at least two of them): the first sample after the wrap lies on line 21 or earlier, and the
    // samples of lines 25-60 show the counters running, satisfying the F4 criteria and not all identical.
    if (pre) {
        uint64_t to_line = (uint64_t)((313 - 254 + 45) * 227) * (uint64_t)tc;   // LP_GO to line 45 of the next frame, about
        int nsamp2 = (int)(to_line / (uint64_t)(interval > 0 ? interval : 1)) + 10;
        if (nsamp2 > 3000) nsamp2 = 3000;
        uint64_t second = mutating("P8-VBREL") ? (uint64_t)((313 - 254 + 30) * 227) * (uint64_t)tc : 0;   // a second trigger at about line 30 of the next frame
        LpRun pv = lp_run(0x0020, nsamp2, 0x0008, pulse_on, delay, plen, pulse_prog, second);
        size_t wrap = 0;
        for (size_t j = 1; j < pv.s.size(); ++j) if (pv.s[j].V + 100 < pv.s[j - 1].V) { wrap = j; break; }
        // the samples after the trigger, up to the wrap (or to the last sample when V never wraps)
        std::vector<Sm> frz;
        for (size_t j = 0; j < (wrap ? wrap : pv.s.size()); ++j)
            if (pv.r && pv.s[j].t > pv.r->pulse_end + (uint64_t)interval) frz.push_back(pv.s[j]);
        bool froze = frozen(frz);
        std::vector<Sm> tail;
        for (size_t j = wrap; wrap && j < pv.s.size(); ++j) if (pv.s[j].V >= 25 && pv.s[j].V <= 60) tail.push_back(pv.s[j]);
        bool first_ok = wrap && pv.s[wrap].V <= 21;
        bool vbrel = first_ok && tail.size() >= 8 && f4_violations(tail, false) == 0 && !frozen(tail);
        checks.note("P8 measured: " + dec((long long)pv.s.size()) + " samples into the next frame, " + dec((long long)frz.size()) +
                    " after the trigger before the wrap, first sample after the wrap on line " + (wrap ? dec(pv.s[wrap].V) : std::string("n/a")) + ", " +
                    dec((long long)tail.size()) + " of them in lines 25-60 after the wrap");
        if (froze)
            g.expect(vbrel, "P8-VBREL: after the end of the internal vertical blank of the next frame the first sample that no longer shows the frozen position lies on line 21 or earlier, and the samples of lines 25-60 show the counters running again: they satisfy the F4 criteria and are not all identical");
        else checks.record("NOT GRADED: P8-VBREL [no counter froze after the trigger, so there is no freeze to release]");
    } else checks.record("NOT GRADED: P8-VBREL [a precondition does not hold]");
    // the aggregate needs all three registers; it is graded when they can all be graded, and also when a graded part is already red
    bool agg_known_red = !(v0 && v1);
    g.expect_if(pre && (pre_hh || agg_known_red), v0 && h0 && v1 && h1,
        "P8: opus:AA-beamcon0.b13 sol:AA-beamcon0.c8 opus:AA-beamcon0.c9 opus:AA-beamcon0.c10 opus:AA-beamcon0.c8 sol:AA-beamcon0.c24 sol:AA-beamcon0.c25 sol:AA-beamcon0.c26: "
        "with LPEN set, BEAMCON0 bit 13 (LPENDIS) low makes VHPOSR, VPOSR and HHPOSR read the latched light-pen position, and high makes them read the actual beam counters");
}

} // namespace

int main(int argc, char** argv) {
    g_dir = argc > 1 ? argv[1] : ".";
    g_mutate = std::getenv("NMH_MUTATE");
    if (g_mutate && !*g_mutate) g_mutate = nullptr;
    std::string core = NMH_XSTR(NANOMIG_CORE_NAME);
    std::printf("CORE: %s\n", core.c_str());
    if (g_mutate) std::printf("NMH_MUTATE=%s\n", g_mutate);

    // CONFIG: the configuration every harness of every run below is built from.
    g_cfg = make_config();
    if (mutating("CONFIG")) g_cfg.chipset_config = 0x08;
    g_cfg_ok = config_ok(g_cfg);
    g.expect(g_cfg_ok, "CONFIG: the harness configuration of every run selects the AA chip set (chipset_config 0x10) and 2 MB of Chip memory (memory_config 0x03)");

    proposition_p1();
    proposition_p2();
    proposition_p3();
    proposition_p4();
    proposition_p5();
    proposition_p6();
    proposition_p7();
    proposition_p8();
    proposition_p9_p10();
    proposition_p11();
    proposition_p12();
    proposition_p13();
    proposition_p14();

    // the claims of SPECIFICATION.md section 1, which no proposition grades
    checks.record("DECLINED-CHIP: opus:AA-bltxmod.c3 opus:AA-bltxpth.c3 opus:AA-bltxptl.c3 sol:AA-bltxmod.c3 sol:AA-bltxpth.c3 sol:AA-bltxptl.c3: the chip column A names the chip that holds the register, and no output of nanomig.v shows which chip a register lives in");
    checks.record("DECLINED-MARK: opus:AA-bltxpth.c4 opus:AA-bltxptl.c7: the revision marks h are statements about the document's history and no output shows them");
    checks.record("DECLINED-UNUSED: opus:AA-beamcon0.b15 sol:AA-beamcon0.c6 opus:AA-beamcon0.b3 sol:AA-beamcon0.c18: the page gives the unused bits 15 and 3 no function and no value, so no read-back value and no effect is stated");
    checks.record("DECLINED-CSY: opus:AA-beamcon0.c37 sol:AA-beamcon0.c54: the CSY* pin is not among the outputs the harness shows");
    checks.record("DECLINED-INTERLACE: sol:AA-beamcon0.c50: the page does not say what 'doesn't work' looks like in 640*400*4 interlace mode, so no observation can be graded against it");
    checks.record("DECLINED-NORMAL: opus:AA-beamcon0.c18: the claim joins a statement about the CSY* pin, which cannot be seen, to the CSCBEN routing that P9 grades under other ids");
    checks.record("DECLINED-WRITEONLY: opus:AA-bltxpth.c2 opus:AA-bltxptl.c2 opus:AA-bltxmod.c2: the page gives no value for a read of a W address, so 'write only' has no observable consequence");
    checks.record("DECLINED-PAL: opus:AA-beamcon0.c35 sol:AA-beamcon0.c52 opus:AA-beamcon0.b5 sol:AA-beamcon0.c16: p.12 says the PAL bit sets the appropriate decodes for PAL in normal mode and gives no PAL decode value, so that PAL-set timing differs from PAL-clear timing is a proxy that cannot show the decodes are appropriate, or that the difference is PAL");
    checks.record("DECLINED-DUAL: opus:AA-beamcon0.c30 sol:AA-beamcon0.c47: DUAL runs all the horizontal comparators with the alternate counter, and P11 grades HTOTAL only");
    if (g_tears) checks.note("samples taken across the step from line 255 to line 256 and completed with the V8 they lacked: " + dec(g_tears));
    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
