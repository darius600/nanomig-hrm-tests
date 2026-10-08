// test_ch4_shres_bitplane.cpp -- ch4-shres-bitplane: SuperHires bitplane
// colour encoding. See SPECIFICATION.md for every proposition's page,
// stimulus, observation and expected value; this file only implements
// what SPECIFICATION.md already committed to (section 3's palettes and
// pattern generator, section 4's stimulus, section 5's reference frames,
// section 6's preconditions, section 7's propositions).
//
// The Copper-list and pixel-stream data poked into chip RAM by the 68000
// program (prog_main.s and its mutation wrappers) is data transcribed from
// SPECIFICATION.md sections 3 and 4; this file re-derives the same literal
// tables independently (not by reading the data tables' own bytes) so the
// check is against the specification, not against how the tables were
// built.
//
// NMH_MUTATE selects one mutation in place of the normal run; a plain `make test`
// exercises none.
//
// Every record below is one line:
//   // MUTATION <label> <kind> <result> NMH_MUTATE=<selector>: <what changes> -> <what moves> (<passed> passed, <failed> failed)
// <label> is the leading token of a printed line (P4.n<n>b<b> for the P4 lines). <kind> is stimulus (the program, a
// Copper list or the harness Config changes; the comparator and the expected value never do) or none. <result> is
// red when the line, or a precondition that conditions it, prints FAIL through the unchanged comparator; green when
// a live input turns a line that is FAIL at baseline into PASS through the same comparator; impossible, with the
// reason, when no stimulus of this package can change the line.
// "->" lists what moves against the baseline run: lines turning FAIL, lines turning PASS, lines printing "NOT
// GRADED" in place of PASS or FAIL because a precondition of a frame they read failed (such a line carries no
// verdict), and lines not printed because the run ended or the line has no population. The counts are those of
// fx68k; TG68K prints the same PASS, FAIL, RECORDED and MEASURED lines on every record.
//
// MUTATION fixture none impossible: the line 'program loaded and CPU released from reset' can print FAIL only when no program image is loaded or the CPU is never released; the run then has no program to execute, never writes DONE and grades nothing further, so no mutation can leave the program and the measurement setup running (not exercised at this boundary)
// MUTATION fixture stimulus red NMH_MUTATE=FIX: the program stores $DEAD instead of $BEEF, so the line 'the program-stored constant $BEEF was read back' prints FAIL -> FAIL fixture (85 passed, 47 failed)
// MUTATION C2 none impossible: the line prints FAIL only when the program does not write DONE, and a run that never writes DONE is not an allowed mutation, so no negative DONE mutation is recorded (not exercised at this boundary)
// MUTATION C1 stimulus red NMH_MUTATE=C1: OCS instead of ECS (harness Config, no program change) -> FAIL C1, C5/R2, R2 x2, C8 x2, P2-DIVERSITY, C5-F7F10; NOT GRADED P1 x12, P2.R, P2.G, P2.B, P3.H, P3.L, P7.1, V4c x3, P4 x192, P5.L x3, P1K x12, P1J x12, P6.1-F9PF, P6.1, P6.2, P5.U, P7.2, P7.3, P7.4; 3 lines not printed (23 passed, 9 failed)
// MUTATION R0 stimulus red NMH_MUTATE=R0: the pattern section of both stream buffers repeats a 40-word period of runs of 4 or 6 pixels of one value, so every run boundary is a colour change on every pattern line and no sample width divides all the run lengths: the line 'a unique hires-period sample width w and phase phi exist' prints FAIL -> FAIL R0, C5/R2, R2 x4, C6 x6, C3, C8 x2, R1, P2-DIVERSITY, C5-F7F10, R1-F7F10; NOT GRADED P1 x12, P2.R, P2.G, P2.B, P3.H, P3.L, P7.1, V4c x3, P4 x192, P5.L x2, P1K x12, P1J x12, C11, RUN-ENDS, P6.1-F9PF, P6.1, P6.2, P5.U, P7.2, P7.3, P7.4; 4 lines not printed (11 passed, 19 failed)
// MUTATION C5/R2 stimulus red NMH_MUTATE=C5: F1's BPLCON0=$A000 (hires instead of SuperHires) -> FAIL C5/R2, R2, C4, P2-DIVERSITY; NOT GRADED P1 x12, P2.R, P2.G, P2.B, P3.H, P3.L, P7.1, P1J x3 (64 passed, 47 failed)
// MUTATION C5/R2 stimulus red NMH_MUTATE=V2: F1 run in hires (BPLCON0=$A000), the list of C5 -> FAIL C5/R2, R2, C4, P2-DIVERSITY; NOT GRADED P1 x12, P2.R, P2.G, P2.B, P3.H, P3.L, P7.1, P1J x3 (64 passed, 47 failed)
// MUTATION R2 stimulus red NMH_MUTATE=R2M: F5's BPLCON0=$2040 (SuperHires instead of hires) and F6's BPLCON0=$A000 (hires instead of lores): the lines 'F5's hires pixel mapping' and 'F6's lores pixel mapping' print FAIL -> FAIL R2 x3, C4; NOT GRADED P3.H, P3.L, P7.1, P5.L; 1 line not printed (77 passed, 50 failed)
// MUTATION R2 stimulus red NMH_MUTATE=V4B: F2-F4 run in hires (BPLCON0=$A000): the line 'F2-F4's SuperHires pixel mapping' prints FAIL -> FAIL R2 x2, C4; NOT GRADED P1 x12, P2.R, P2.G, P2.B, V4c x3, P4 x192, P5.L, P1K x12, P1J x12, P7.3 (38 passed, 8 failed)
// MUTATION R2 stimulus red NMH_MUTATE=V5: F2-F4 run in hires (BPLCON0=$A000, the program of V4B) -> FAIL R2 x2, C4; NOT GRADED P1 x12, P2.R, P2.G, P2.B, V4c x3, P4 x192, P5.L, P1K x12, P1J x12, P7.3 (38 passed, 8 failed)
// MUTATION R2 stimulus red NMH_MUTATE=R2B: F1's closing band also writes BPLCON1=$0011, which delays the bitplane data for the rest of the frame: the line 'the same s that fits the top Q0 lines also fits the bottom Q0 lines' prints FAIL -> FAIL R2, C8; NOT GRADED P1 x12, P2.R, P2.G, P2.B, P3.H, P3.L, P7.1, P1J x3 (66 passed, 45 failed)
// MUTATION C6 stimulus red NMH_MUTATE=C6: F1's DIWSTRT/DIWSTOP narrowed to a 64-lores-pixel window (at most 128 hires periods wide): the run of F1's pure R lines falls outside 256-576 -> FAIL C5/R2, R2, C6, C4, C8 x2, P2-DIVERSITY; NOT GRADED P1 x12, P2.R, P2.G, P2.B, P3.H, P3.L, P7.1, P1J x3 (61 passed, 50 failed)
// MUTATION C6 stimulus red NMH_MUTATE=C6B: F2-F5's DIWSTRT/DIWSTOP narrowed to a 64-lores-pixel window: the lines of F2, F3, F4 and F5 print FAIL -> FAIL R2 x3, C6 x4, C4, C8; NOT GRADED P1 x12, P2.R, P2.G, P2.B, P3.H, P7.1, V4c x3, P4 x192, P5.L x2, P1K x12, P1J x12, P7.3; 1 line not printed (28 passed, 14 failed)
// MUTATION C3 stimulus red NMH_MUTATE=C3: F1's BPLCON0=$1040 (one bitplane, only plane-1 fetched) -> FAIL C5/R2, R2, C3, C4, C8 x2, P2-DIVERSITY; NOT GRADED P1 x12, P2.R, P2.G, P2.B, P3.H, P3.L, P7.1, P1J x3 (61 passed, 50 failed)
// MUTATION C3 stimulus red NMH_MUTATE=C3P: F1's BPLCON0=$B000 (hires, three bitplanes): plane 3 is fetched from the unprogrammed BPL3PT, so the clause 'no other bitplane address inside a line's fetches' fails -> FAIL C5/R2, R2, C3, C4, C9, C8 x2, P2-DIVERSITY; NOT GRADED P1 x12, P2.R, P2.G, P2.B, P3.H, P3.L, P7.1, P1J x3 (60 passed, 51 failed)
// MUTATION C4 stimulus red NMH_MUTATE=C4: F5's list points at itself instead of F6 -> FAIL R2 x2, C6, C3, C4, C9, C7 x2, R1, C4-F7F10, C5-F7F10, C7-F7F10, C10, R1-F7F10; NOT GRADED P3.H, P3.L, P7.1, P5.L x2, C11, RUN-ENDS, P6.1-F9PF, P6.1, P6.2, P5.U, P7.4; 3 lines not printed (61 passed, 56 failed)
// MUTATION C4 stimulus red NMH_MUTATE=C4S: F2's header omits the MOVEs to SPR1PTH/SPR1PTL: the pointer values of the planned header are missing -> FAIL C4, C9; NOT GRADED P1 x12, P2.R, P2.G, P2.B, V4c, P4 x80, P5.L, P1K x12, P1J x12, P7.3 (41 passed, 7 failed)
// MUTATION C9 stimulus red NMH_MUTATE=C9: F1's SPR0PT points at the real sprite 0 structure -> FAIL C4, C9; NOT GRADED P1 x12, P2.R, P2.G, P2.B, P3.H, P3.L, P7.1, P1J x3 (66 passed, 45 failed)
// MUTATION C9 stimulus red NMH_MUTATE=C4S: F2's header omits the MOVEs to SPR1PTH/SPR1PTL: sprite 1 fetches from the address its pointer reached in F1's frame, past the null sprite -> FAIL C4, C9; NOT GRADED P1 x12, P2.R, P2.G, P2.B, V4c, P4 x80, P5.L, P1K x12, P1J x12, P7.3 (41 passed, 7 failed)
// MUTATION C7 stimulus red NMH_MUTATE=C7: F1 band 1's WAIT at horizontal $80 instead of $00: the timing line prints FAIL, the band-count line holds -> FAIL C7; NOT GRADED P1 x12, P2.R, P2.G, P2.B, P3.H, P3.L, P7.1, P1J x3 (67 passed, 44 failed)
// MUTATION C7 stimulus red NMH_MUTATE=C7C: F1's Q3 band change is absent (11 bands instead of 12): the band-count line prints FAIL -> FAIL C7; NOT GRADED P1 x12, P2.R, P2.G, P2.B, P3.H, P3.L, P7.1, P1J x3 (67 passed, 44 failed)
// MUTATION C7 stimulus red NMH_MUTATE=V3A: F1 in hires with sixteen COLOR01 bands instead of the S0-S3 bands (16 bands instead of 12): the band-count line prints FAIL -> FAIL C5/R2, R2, C4, C7, P2-DIVERSITY; NOT GRADED P1 x12, P2.R, P2.G, P2.B, P3.H, P3.L, P7.1, P1J x3 (63 passed, 48 failed)
// MUTATION C8 stimulus red NMH_MUTATE=C8: the pattern section restricted to values 0 and 3: the line 'F1 Q0 diversity' (8 of 64 triples) and the line of pair coverage (0 of 203 bands) print FAIL -> FAIL C8 x2, P2-DIVERSITY; NOT GRADED P1 x12, P2.R, P2.G, P2.B, P3.H, P3.L, P7.1, V4c x3, P4 x192, P5.L, P1K x12, P1J x12, P7.3 (35 passed, 8 failed)
// MUTATION C8 stimulus red NMH_MUTATE=C8: as above, the line 'F1 Q0 diversity' -> FAIL C8 x2, P2-DIVERSITY; NOT GRADED P1 x12, P2.R, P2.G, P2.B, P3.H, P3.L, P7.1, V4c x3, P4 x192, P5.L, P1K x12, P1J x12, P7.3 (35 passed, 8 failed)
// MUTATION R1 stimulus red NMH_MUTATE=C4: F5's list points at itself, so the frames after F5 are decoded from repeated lines: the pairs of output copies of a graded source line differ -> FAIL R2 x2, C6, C3, C4, C9, C7 x2, R1, C4-F7F10, C5-F7F10, C7-F7F10, C10, R1-F7F10; NOT GRADED P3.H, P3.L, P7.1, P5.L x2, C11, RUN-ENDS, P6.1-F9PF, P6.1, P6.2, P5.U, P7.4; 3 lines not printed (61 passed, 56 failed)
// MUTATION P1.0R stimulus red NMH_MUTATE=V1: F1 writes Q3x instead of Q3 for the Q3 band -> FAIL P1 x12, P1J x3 (71 passed, 61 failed)
// MUTATION P1.0G stimulus red NMH_MUTATE=V1: F1 writes Q3x instead of Q3 for the Q3 band -> FAIL P1 x12, P1J x3 (71 passed, 61 failed)
// MUTATION P1.0B stimulus red NMH_MUTATE=V1: F1 writes Q3x instead of Q3 for the Q3 band -> FAIL P1 x12, P1J x3 (71 passed, 61 failed)
// MUTATION P1.1R stimulus red NMH_MUTATE=V1: F1 writes Q3x instead of Q3 for the Q3 band -> FAIL P1 x12, P1J x3 (71 passed, 61 failed)
// MUTATION P1.1G stimulus red NMH_MUTATE=V1: F1 writes Q3x instead of Q3 for the Q3 band -> FAIL P1 x12, P1J x3 (71 passed, 61 failed)
// MUTATION P1.1B stimulus red NMH_MUTATE=V1: F1 writes Q3x instead of Q3 for the Q3 band -> FAIL P1 x12, P1J x3 (71 passed, 61 failed)
// MUTATION P1.2R stimulus red NMH_MUTATE=V1: F1 writes Q3x instead of Q3 for the Q3 band -> FAIL P1 x12, P1J x3 (71 passed, 61 failed)
// MUTATION P1.2G stimulus red NMH_MUTATE=V1: F1 writes Q3x instead of Q3 for the Q3 band -> FAIL P1 x12, P1J x3 (71 passed, 61 failed)
// MUTATION P1.2B stimulus red NMH_MUTATE=V1: F1 writes Q3x instead of Q3 for the Q3 band -> FAIL P1 x12, P1J x3 (71 passed, 61 failed)
// MUTATION P1.3R stimulus red NMH_MUTATE=V1: F1 writes Q3x instead of Q3 for the Q3 band -> FAIL P1 x12, P1J x3 (71 passed, 61 failed)
// MUTATION P1.3G stimulus red NMH_MUTATE=V1: F1 writes Q3x instead of Q3 for the Q3 band -> FAIL P1 x12, P1J x3 (71 passed, 61 failed)
// MUTATION P1.3B stimulus red NMH_MUTATE=V1: F1 writes Q3x instead of Q3 for the Q3 band -> FAIL P1 x12, P1J x3 (71 passed, 61 failed)
// MUTATION P2-DIVERSITY stimulus red NMH_MUTATE=C5: F1's BPLCON0=$A000: only F2-F4's single palette Q0 populates the groups, 0 of 12 (c, X) combinations have two palettes -> FAIL C5/R2, R2, C4, P2-DIVERSITY; NOT GRADED P1 x12, P2.R, P2.G, P2.B, P3.H, P3.L, P7.1, P1J x3 (64 passed, 47 failed)
// MUTATION P2-DIVERSITY stimulus red NMH_MUTATE=V2: F1 run in hires (BPLCON0=$A000), the list of C5 -> FAIL C5/R2, R2, C4, P2-DIVERSITY; NOT GRADED P1 x12, P2.R, P2.G, P2.B, P3.H, P3.L, P7.1, P1J x3 (64 passed, 47 failed)
// MUTATION P2.R stimulus green NMH_MUTATE=P2G: F1's palette bands Q1-Q5, Z and O write Color 0 as Q0's Color 0 in every register and Colors 1-3 as before, so the registers of a value-c pixel differ between the palettes only in Color c's letters; P2-DIVERSITY prints 12 of 12 as at baseline and the unchanged comparator prints PASS on 223440 pixels in 16 groups -> FAIL P1 x3, P1J x3; PASS P2.R, P2.G, P2.B (83 passed, 49 failed)
// MUTATION P2.G stimulus green NMH_MUTATE=P2G: F1's palette bands Q1-Q5, Z and O write Color 0 as Q0's Color 0 in every register and Colors 1-3 as before, so the registers of a value-c pixel differ between the palettes only in Color c's letters; P2-DIVERSITY prints 12 of 12 as at baseline and the unchanged comparator prints PASS on 223440 pixels in 16 groups -> FAIL P1 x3, P1J x3; PASS P2.R, P2.G, P2.B (83 passed, 49 failed)
// MUTATION P2.B stimulus green NMH_MUTATE=P2G: F1's palette bands Q1-Q5, Z and O write Color 0 as Q0's Color 0 in every register and Colors 1-3 as before, so the registers of a value-c pixel differ between the palettes only in Color c's letters; P2-DIVERSITY prints 12 of 12 as at baseline and the unchanged comparator prints PASS on 223440 pixels in 16 groups -> FAIL P1 x3, P1J x3; PASS P2.R, P2.G, P2.B (83 passed, 49 failed)
// MUTATION P3.H stimulus red NMH_MUTATE=V3B: F5's COLOR01 sweep masked with $0CCC, so Hires shows 4 distinct values per component and SuperHires (7, 9, 9) no longer lies below it -> FAIL P3.H, P7.1 (84 passed, 48 failed)
// MUTATION P3.L stimulus red NMH_MUTATE=V3C: F6's COLOR01 sweep masked with $0CCC, so Lores shows 4 distinct values per component and SuperHires (7, 9, 9) no longer lies below it -> FAIL P3.L, P7.1 (84 passed, 48 failed)
// MUTATION P7.1 stimulus red NMH_MUTATE=V3B: the Hires half as for P3.H: P7.1 prints FAIL (P3.H fails, P3.L holds) -> FAIL P3.H, P7.1 (84 passed, 48 failed)
// MUTATION P7.1 stimulus red NMH_MUTATE=V3C: the Lores half as for P3.L: P7.1 prints FAIL (P3.H holds, P3.L fails) -> FAIL P3.L, P7.1 (84 passed, 48 failed)
// MUTATION V4c stimulus red NMH_MUTATE=V4C: F2's closing restore band also inverts bit 11 of COLOR01 for part of its own first line (after its restore MOVE it writes the inverted value 23 times and then restores COLOR01), so pixels of that line differ from their baseline triples: the F2 line prints FAIL through a difference (checked=1120, diffs=160), not through its population guard -> FAIL V4c x2; NOT GRADED P4 x160, P5.L, P1K x12, P1J x12, P7.3 (54 passed, 10 failed)
// MUTATION V4c stimulus red NMH_MUTATE=V4C: F3's closing restore band also inverts bit 11 of COLOR01 for part of its own first line (after its restore MOVE it writes the inverted value 23 times and then restores COLOR01), so pixels of that line differ from their baseline triples: the F3 line prints FAIL through a difference (checked=1120, diffs=160), not through its population guard -> FAIL V4c x2; NOT GRADED P4 x160, P5.L, P1K x12, P1J x12, P7.3 (54 passed, 10 failed)
// MUTATION V4c none impossible: the F4 line cannot print FAIL through a difference: the lines of F4's null band (window lines 90-91) lie inside the population its baseline is built from (window lines 89-199), so a compared pixel with a matched triple equals the baseline of that triple by construction; only the population guard (checked=0) can fail it, and that is not a difference (unfalsifiable at this boundary)
// MUTATION P4.n0b2 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n0b3 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n0b6 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n0b7 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n0b10 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n0b11 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n1b2 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n1b3 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n1b6 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n1b7 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n1b10 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n1b11 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n2b2 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n2b3 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n2b6 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n2b7 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n2b10 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n2b11 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n3b2 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n3b3 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n3b6 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n3b7 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n3b10 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n3b11 stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P4.n0b11 stimulus red NMH_MUTATE=V4A: F2 band 11 (n=00, b=11) also inverts COLOR01 bit 11, a bit of another Color -> FAIL P4 (85 passed, 47 failed)
// MUTATION P4.n1b0 stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit b+2 of COLOR00 instead, a bit that P4's p-half lines show live -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P4.n1b1 stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit b+2 of COLOR00 instead, a bit that P4's p-half lines show live -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P4.n1b4 stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit b+2 of COLOR00 instead, a bit that P4's p-half lines show live -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P4.n1b5 stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit b+2 of COLOR00 instead, a bit that P4's p-half lines show live -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P4.n1b8 stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit b+2 of COLOR00 instead, a bit that P4's p-half lines show live -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P4.n1b9 stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit b+2 of COLOR00 instead, a bit that P4's p-half lines show live -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P4.n2b0 stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit b+2 of COLOR00 instead, a bit that P4's p-half lines show live -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P4.n2b1 stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit b+2 of COLOR00 instead, a bit that P4's p-half lines show live -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P4.n2b4 stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit b+2 of COLOR00 instead, a bit that P4's p-half lines show live -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P4.n2b5 stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit b+2 of COLOR00 instead, a bit that P4's p-half lines show live -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P4.n2b8 stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit b+2 of COLOR00 instead, a bit that P4's p-half lines show live -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P4.n2b9 stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit b+2 of COLOR00 instead, a bit that P4's p-half lines show live -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P4.n3b0 stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit b+2 of COLOR00 instead, a bit that P4's p-half lines show live -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P4.n3b1 stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit b+2 of COLOR00 instead, a bit that P4's p-half lines show live -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P4.n3b4 stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit b+2 of COLOR00 instead, a bit that P4's p-half lines show live -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P4.n3b5 stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit b+2 of COLOR00 instead, a bit that P4's p-half lines show live -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P4.n3b8 stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit b+2 of COLOR00 instead, a bit that P4's p-half lines show live -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P4.n3b9 stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit b+2 of COLOR00 instead, a bit that P4's p-half lines show live -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P5.L stimulus green NMH_MUTATE=VLIVE: (SuperHires half) F2 band 48 (n=04, b=00) also writes COLOR01=$0C9E, the mechanism of V4A, and band 49 restores it: the any-bit count over the 144 bands becomes 288 of 161280 pixels and the line prints PASS through the same comparator; P4.n4b0 turns from NOT OBSERVABLE to FAIL on the same change. The count takes every band n>=04 before P4's NOT OBSERVABLE branch, so a band whose only change lies in bits 1-0 of the component would count too; no stimulus of this package produces one -> FAIL P4; PASS P5.L, P7.3 (88 passed, 45 failed)
// MUTATION P5.L stimulus red NMH_MUTATE=V5B: (Hires half, registers COLOR04-0F) F5's band planned to invert COLOR05 inverts COLOR01 instead -> FAIL P5.L (85 passed, 47 failed)
// MUTATION P5.L stimulus red NMH_MUTATE=V5D: (Hires half, registers COLOR00-03) F5's band planned to invert COLOR02 inverts COLOR03 instead -> FAIL P5.L (85 passed, 47 failed)
// MUTATION P1K.1R stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit of COLOR00 instead, so no off-key band changes R of a value-1 pixel and the unchanged comparator prints PASS -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P1K.1G stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit of COLOR00 instead, so no off-key band changes G of a value-1 pixel and the unchanged comparator prints PASS -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P1K.1B stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit of COLOR00 instead, so no off-key band changes B of a value-1 pixel and the unchanged comparator prints PASS -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P1K.2R stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit of COLOR00 instead, so no off-key band changes R of a value-2 pixel and the unchanged comparator prints PASS -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P1K.2G stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit of COLOR00 instead, so no off-key band changes G of a value-2 pixel and the unchanged comparator prints PASS -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P1K.2B stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit of COLOR00 instead, so no off-key band changes B of a value-2 pixel and the unchanged comparator prints PASS -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P1K.3R stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit of COLOR00 instead, so no off-key band changes R of a value-3 pixel and the unchanged comparator prints PASS -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P1K.3G stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit of COLOR00 instead, so no off-key band changes G of a value-3 pixel and the unchanged comparator prints PASS -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P1K.3B stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit of COLOR00 instead, so no off-key band changes B of a value-3 pixel and the unchanged comparator prints PASS -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P1K.0R stimulus red NMH_MUTATE=VKEYA: F2's band 79 (n=06, b=7), an off-key band for Color 0, also inverts bits 11, 7 and 3 of COLOR00, the upper letters of Color 0, which the closing band restores: R of the value-0 pixels changes in an off-key band -> FAIL P4, P1K x3, P1J x3; PASS P5.L, P7.3 (82 passed, 51 failed)
// MUTATION P1K.0R stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit, so a keyed band changes both of bits 3-2 of R on the value-0 pixels -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P1K.0G stimulus red NMH_MUTATE=VKEYA: F2's band 79 (n=06, b=7), an off-key band for Color 0, also inverts bits 11, 7 and 3 of COLOR00, the upper letters of Color 0, which the closing band restores: G of the value-0 pixels changes in an off-key band -> FAIL P4, P1K x3, P1J x3; PASS P5.L, P7.3 (82 passed, 51 failed)
// MUTATION P1K.0G stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit, so a keyed band changes both of bits 3-2 of G on the value-0 pixels -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P1K.0B stimulus red NMH_MUTATE=VKEYA: F2's band 79 (n=06, b=7), an off-key band for Color 0, also inverts bits 11, 7 and 3 of COLOR00, the upper letters of Color 0, which the closing band restores: B of the value-0 pixels changes in an off-key band -> FAIL P4, P1K x3, P1J x3; PASS P5.L, P7.3 (82 passed, 51 failed)
// MUTATION P1K.0B stimulus red NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit, so a keyed band changes both of bits 3-2 of B on the value-0 pixels -> FAIL P4 x24, P1K x3, P1J x3 (56 passed, 76 failed)
// MUTATION P1J.0R stimulus red NMH_MUTATE=V1: F1 writes Q3x instead of Q3 for the Q3 band, so P1.0R fails -> FAIL P1 x12, P1J x3 (71 passed, 61 failed)
// MUTATION P1J.0R stimulus red NMH_MUTATE=VKEYA: F2's band 79 also inverts the upper letters of Color 0 in COLOR00, so P1K.0R fails -> FAIL P4, P1K x3, P1J x3; PASS P5.L, P7.3 (82 passed, 51 failed)
// MUTATION P1J.0G stimulus red NMH_MUTATE=V1: F1 writes Q3x instead of Q3 for the Q3 band, so P1.0G fails -> FAIL P1 x12, P1J x3 (71 passed, 61 failed)
// MUTATION P1J.0G stimulus red NMH_MUTATE=VKEYA: F2's band 79 also inverts the upper letters of Color 0 in COLOR00, so P1K.0G fails -> FAIL P4, P1K x3, P1J x3; PASS P5.L, P7.3 (82 passed, 51 failed)
// MUTATION P1J.0B stimulus red NMH_MUTATE=V1: F1 writes Q3x instead of Q3 for the Q3 band, so P1.0B fails -> FAIL P1 x12, P1J x3 (71 passed, 61 failed)
// MUTATION P1J.0B stimulus red NMH_MUTATE=VKEYA: F2's band 79 also inverts the upper letters of Color 0 in COLOR00, so P1K.0B fails -> FAIL P4, P1K x3, P1J x3; PASS P5.L, P7.3 (82 passed, 51 failed)
// MUTATION P1J.1R stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit of COLOR00 instead, so P1.1R and P1K.1R both hold -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P1J.1G stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit of COLOR00 instead, so P1.1G and P1K.1G both hold -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P1J.1B stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit of COLOR00 instead, so P1.1B and P1K.1B both hold -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P1J.2R stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit of COLOR00 instead, so P1.2R and P1K.2R both hold -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P1J.2G stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit of COLOR00 instead, so P1.2G and P1K.2G both hold -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P1J.2B stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit of COLOR00 instead, so P1.2B and P1K.2B both hold -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P1J.3R stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit of COLOR00 instead, so P1.3R and P1K.3R both hold -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P1J.3G stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit of COLOR00 instead, so P1.3G and P1K.3G both hold -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION P1J.3B stimulus green NMH_MUTATE=VP1JG: each q-half band of COLOR01-03 inverts the matching p-half bit of COLOR00 instead, so P1.3B and P1K.3B both hold -> PASS P4 x18, P1K x9, P1J x9 (122 passed, 10 failed)
// MUTATION C4-F7F10 stimulus red NMH_MUTATE=C4B: F8's header writes BPLCON2=$0000 instead of $0024 -> FAIL C4-F7F10; NOT GRADED P6.1-F9PF, P6.1, P6.2, P5.U, P7.4; 3 lines not printed (80 passed, 44 failed)
// MUTATION C4-F7F10 stimulus red NMH_MUTATE=C4C: F7's list points at itself instead of F8, so F8-F10 never run -> FAIL C4-F7F10, C5-F7F10, C7-F7F10, C10; NOT GRADED P6.1-F9PF, P6.1, P6.2, P5.U, P7.4; 3 lines not printed (77 passed, 47 failed)
// MUTATION C4-F7F10 stimulus red NMH_MUTATE=V6C: F7 runs with BPLCON0=$A000 -> FAIL C4-F7F10, C5-F7F10, RUN-ENDS; NOT GRADED P6.1-F9PF, P6.1, P6.2, P5.U, P7.4; 3 lines not printed (78 passed, 46 failed)
// MUTATION C5-F7F10 stimulus red NMH_MUTATE=V6C: F7 runs with BPLCON0=$A000 (no s fits F7's SuperHires mapping; the hires mapping fits) -> FAIL C4-F7F10, C5-F7F10, RUN-ENDS; NOT GRADED P6.1-F9PF, P6.1, P6.2, P5.U, P7.4; 3 lines not printed (78 passed, 46 failed)
// MUTATION C7-F7F10 stimulus red NMH_MUTATE=C7B: the block 3 band's WAIT of F7 at horizontal $80 instead of $00 -> FAIL C7-F7F10; NOT GRADED P6.1-F9PF, P6.1, P6.2, P5.U, P7.4; 3 lines not printed (80 passed, 44 failed)
// MUTATION C10 stimulus red NMH_MUTATE=C10: F7's SPR0PT points at the null sprite -> FAIL C10; NOT GRADED C11, RUN-ENDS, P6.1-F9PF, P6.1, P6.2, P5.U, P7.4; 3 lines not printed (79 passed, 43 failed)
// MUTATION C11 stimulus red NMH_MUTATE=C11: sprite 0's data words are $FF00 where they are $FFFF (8 pixels wide): C11 stays FAIL as at baseline, and the runs halve (F7, F8 and F9 8 periods, F10 16 periods) -> no other line moves (86 passed, 46 failed)
// MUTATION C11 stimulus green NMH_MUTATE=C11G: F7 and F8 run with BPLCON0=$A000: runs of 32 periods in F7, F8 and F10, so the same comparator prints PASS -> FAIL C4-F7F10, C5-F7F10; PASS C11; NOT GRADED P6.1-F9PF, P6.1, P6.2, P5.U, P7.4; 3 lines not printed (80 passed, 44 failed)
// MUTATION RUN-ENDS stimulus red NMH_MUTATE=RUNENDS: F7's band at beam $5C changes COLOR00 to $0F00 for the rest of the frame: the run ends on 6 of 21 F7 baseline lines are [269,284], the other 15 lines differ from the background over the whole interval -> FAIL RUN-ENDS; NOT GRADED P6.1, P6.2, P5.U, P7.4; 3 lines not printed (81 passed, 44 failed)
// MUTATION R1-F7F10 none impossible: not exercised, by a scope decision: the copies of a source line are two read-outs of one stored line; no stimulus of this package changes one read-out and not the other, and none was found that does
// MUTATION P6.1-F9PF stimulus red NMH_MUTATE=V6Q: F9's band change at line 30 writes COLOR00-0F=Q0 instead of Q1 -> FAIL P6.1-F9PF; NOT GRADED P6.1 (84 passed, 47 failed)
// MUTATION P6.1 stimulus red NMH_MUTATE=V6A: F7 writes COLOR16-31=T0c while the expectation stays T0: the F7 clause and the F9 clause fail -> FAIL P6.1 (85 passed, 47 failed)
// MUTATION P6.1 stimulus red NMH_MUTATE=V6E: F8 writes COLOR10-1F=T0 while the expectation stays T0c: the F8 clause fails -> FAIL P6.1 (85 passed, 47 failed)
// MUTATION P6.1 stimulus red NMH_MUTATE=V6B: F9 writes COLOR10-1F=T0c: the F9 clause fails -> FAIL P6.1 (85 passed, 47 failed)
// MUTATION P6.2-SHR stimulus green NMH_MUTATE=V6S: the block of F7 planned to invert COLOR20 inverts COLOR17 instead: 14 inner pixels of block 3's value-1 line change and the same comparator prints PASS -> PASS P6.2-SHR, P6.2, P7.4 (89 passed, 43 failed)
// MUTATION P6.2-HIRES stimulus red NMH_MUTATE=V6D: the block of F10 planned to invert COLOR20 inverts COLOR17 instead -> FAIL P6.2-HIRES (85 passed, 47 failed)
// MUTATION P6.2-LIVE stimulus red NMH_MUTATE=V6L: the block of F10 planned to invert COLOR17 inverts COLOR20 instead -> FAIL P6.2-LIVE (85 passed, 47 failed)
// MUTATION P6.2 stimulus green NMH_MUTATE=V6S: as above: the SuperHires half, the Hires half and the Hires comparator all hold -> PASS P6.2-SHR, P6.2, P7.4 (89 passed, 43 failed)
// MUTATION P6.2 stimulus green NMH_MUTATE=VLIVES: the block of F7 planned to invert COLOR20 inverts COLOR17 instead, with the stimulus of VLIVE -> FAIL P4; PASS P5.L, P6.2-SHR, P6.2, P7.2, P7.3, P7.4 (92 passed, 41 failed)
// MUTATION P6.2 stimulus red NMH_MUTATE=V6D: as above: the Hires half fails -> FAIL P6.2-HIRES (85 passed, 47 failed)
// MUTATION P5.U stimulus red NMH_MUTATE=V5C: block 2 of F7 also writes Color 0's inverted letters into the seven lower registers that hold them, block 3 restores them (the first lines of blocks 2 and 3 are spacer lines): the F7 part of the comparator fails -> FAIL P5.U (85 passed, 47 failed)
// MUTATION P5.U stimulus red NMH_MUTATE=V5E: F8's line-30 band also writes the seven lower registers that hold Color 0's letters, inverted: the F8 part of the comparator fails -> FAIL P5.U (85 passed, 47 failed)
// MUTATION P7.2 stimulus green NMH_MUTATE=VLIVES: the stimulus of VLIVE (a live input for P5.L's SuperHires half) and of V6S (a live input for P6.2): both halves hold and the joint prints PASS -> FAIL P4; PASS P5.L, P6.2-SHR, P6.2, P7.2, P7.3, P7.4 (92 passed, 41 failed)
// MUTATION P7.3 stimulus green NMH_MUTATE=VLIVE: the stimulus of VLIVE (a live input for P5.L's SuperHires half): both halves hold and the joint prints PASS -> FAIL P4; PASS P5.L, P7.3 (88 passed, 45 failed)
// MUTATION P7.4 stimulus green NMH_MUTATE=V6S: the block of F7 planned to invert COLOR20 inverts COLOR17 instead: P6.2's SuperHires half holds with P6.1, and the joint prints PASS -> PASS P6.2-SHR, P6.2, P7.4 (89 passed, 43 failed)

#include <algorithm>
#include <array>
#include <climits>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <optional>
#include <set>
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

// ---- the fixed assertion call form: expect grades, note measures, record declines ---
struct Checks {
    void expect(bool condition, const std::string& label) {
        std::printf("%s: %s\n", condition ? "PASS" : "FAIL", label.c_str());
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::printf("MEASURED: %s\n", msg.c_str()); }
    void record(const std::string& msg) { std::printf("RECORDED: %s\n", msg.c_str()); }
    int passed = 0, failed = 0;
};

// ---- addresses (nanomig.i / ch4bp.i / SPECIFICATION.md 4.2, restated) ----
constexpr uint32_t kResults   = 0x0C0000;
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;

constexpr uint32_t kPlane1Buf = 0x010000;
constexpr uint32_t kPlane2Buf = 0x020000;
constexpr uint32_t kZeroBuf   = 0x030000;
constexpr uint32_t kSpr0Struct= 0x040000;
constexpr uint32_t kNullSpr   = 0x040100;
constexpr uint32_t kCopBase   = 0x041000;
constexpr uint32_t kCopStep   = 0x001000;
constexpr size_t   kBufWords  = 32768;

constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kRunBudget  = 60'000'000;  // 12 vertical blanks plus margin (measured)

uint32_t word_addr(uint32_t byte_addr) { return byte_addr / 2; }
uint32_t cop_addr(int frame) { return word_addr(kCopBase + kCopStep * (uint32_t)frame); }
int frame_of_word(uint32_t wa) {
    uint32_t base = word_addr(kCopBase);
    if (wa < base || wa >= base + 11u * (kCopStep/2)) return -1;
    return (int)((wa - base) / (kCopStep/2));
}

// ---- section 3's palettes, transcribed literally from SPECIFICATION.md ----
const std::vector<uint16_t> kQ0 = {0x005A,0x049E,0x08D2,0x0C16,0x016B,0x05AF,0x09E3,0x0D27,
                                    0x0278,0x06BC,0x0AF0,0x0E34,0x0349,0x078D,0x0BC1,0x0F05};
const std::vector<uint16_t> kQ1(16, 0x0AAA);
const std::vector<uint16_t> kQ2 = {0x05A5,0x0969,0x05A5,0x0969,0x0696,0x0A5A,0x0696,0x0A5A,
                                    0x05A5,0x0969,0x05A5,0x0969,0x0696,0x0A5A,0x0696,0x0A5A};
const std::vector<uint16_t> kQ3 = {0x005F,0x0817,0x0C93,0x04DB,0x024D,0x0A05,0x0E81,0x06C9,
                                    0x036C,0x0B24,0x0FA0,0x07E8,0x017E,0x0936,0x0DB2,0x05FA};
const std::vector<uint16_t> kQ4 = {0x0000,0x04CC,0x0C80,0x0004,0x0133,0x05FF,0x0DB3,0x0137,
                                    0x0320,0x07EC,0x0FA0,0x0324,0x0001,0x04CD,0x0C81,0x0005};
const std::vector<uint16_t> kQ5 = {0x0000,0x0000,0x004C,0x0CCC,0x0000,0x0000,0x004C,0x0CCC,
                                    0x0013,0x0013,0x005F,0x0CDF,0x0333,0x0333,0x037F,0x0FFF};
const std::vector<uint16_t> kZ(16, 0x0000);
const std::vector<uint16_t> kO(16, 0x0FFF);
const std::vector<uint16_t> kS0 = {0x005A,0x0012,0x08D2,0x0C16,0x0048,0x0000,0x08C0,0x0C04,
                                    0x0278,0x0230,0x0AF0,0x0E34,0x0349,0x0301,0x0BC1,0x0F05};
const std::vector<uint16_t> kS1 = {0x005A,0x0456,0x08D2,0x0C16,0x0159,0x0555,0x09D1,0x0D15,
                                    0x0278,0x0674,0x0AF0,0x0E34,0x0349,0x0745,0x0BC1,0x0F05};
const std::vector<uint16_t> kS2 = {0x005A,0x089A,0x08D2,0x0C16,0x026A,0x0AAA,0x0AE2,0x0E26,
                                    0x0278,0x0AB8,0x0AF0,0x0E34,0x0349,0x0B89,0x0BC1,0x0F05};
const std::vector<uint16_t> kS3 = {0x005A,0x0CDE,0x08D2,0x0C16,0x037B,0x0FFF,0x0BF3,0x0F37,
                                    0x0278,0x0EFC,0x0AF0,0x0E34,0x0349,0x0FCD,0x0BC1,0x0F05};
const std::vector<uint16_t> kQ3x= {0x0FA0,0x07E8,0x036C,0x0B24,0x0DB2,0x05FA,0x017E,0x0936,
                                    0x0C93,0x04DB,0x005F,0x0817,0x0E81,0x06C9,0x024D,0x0A05};

// F1's own band order and palettes (SPECIFICATION.md 4.6)
const std::vector<std::pair<std::string,const std::vector<uint16_t>*>> kF1Bands = {
    {"Q1",&kQ1},{"Q2",&kQ2},{"Q3",&kQ3},{"Q4",&kQ4},{"Q5",&kQ5},{"Z",&kZ},{"O",&kO},
    {"S0",&kS0},{"S1",&kS1},{"S2",&kS2},{"S3",&kS3},
};

// ---- section 4.2's xorshift generator, reimplemented independently for
// C3's "data equal to what the program wrote" check ------------------------
std::pair<std::vector<uint16_t>, std::vector<uint16_t>> build_pattern() {
    std::vector<uint16_t> p1(32512, 0), p2(32512, 0);
    uint32_t x = 0x2545F491u;
    for (int k = 0; k < 32512; ++k) {
        uint16_t w1 = 0, w2 = 0;
        for (int j = 0; j < 16; ++j) {
            x ^= (x << 13); x ^= (x >> 17); x ^= (x << 5);
            uint32_t v = x >> 30;
            int bit = 15 - j;
            if (v & 1) w1 |= (uint16_t)(1u << bit);
            if (v & 2) w2 |= (uint16_t)(1u << bit);
        }
        p1[k] = w1; p2[k] = w2;
    }
    return {p1, p2};
}

// ---- one captured run -------------------------------------------------
struct Capture {
    bool loaded=false, released=false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;
    std::vector<nmh::VideoSample> video;
    std::optional<uint16_t> fixture, vposr, deniseid;
};

Capture run_case(const std::string& dir, const std::string& prog, const nmh::Config& cfg) {
    Capture c;
    nmh::NanomigHarness h(cfg);
    c.loaded = h.load_program(dir + "/" + prog + ".bin") > 0;
    if (!c.loaded) return c;
    c.released = h.boot(kBootBudget).cpu_released;
    c.done = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    c.log = h.ram_log();
    c.video = h.video();
    c.fixture  = h.peek_logical(kResults + 0);
    c.vposr    = h.peek_logical(kResults + 2);
    c.deniseid = h.peek_logical(kResults + 4);
    return c;
}

// ---- Copper-list decoding (R5): one pass per frame -------------------------
struct CopEvent { uint64_t cycle; bool is_wait; uint16_t w1, w2; };

std::vector<CopEvent> parse_frame_events(const std::vector<nmh::RamAccess>& log, int frame) {
    uint32_t base = cop_addr(frame);
    std::vector<nmh::RamAccess> reads;
    for (auto& a : log) {
        if (a.is_write) continue;
        if (a.word_addr < base || a.word_addr >= base + kCopStep/2) continue;
        reads.push_back(a);
    }
    std::stable_sort(reads.begin(), reads.end(), [](auto&a, auto&b){ return a.cycle < b.cycle; });
    auto c = nmh::coalesce(reads);
    std::vector<CopEvent> events;
    for (size_t i = 0; i + 1 < c.size(); i += 2) {
        uint16_t w1 = c[i].data, w2 = c[i+1].data;
        bool is_wait = (w1 & 1) != 0;
        events.push_back({c[i].cycle, is_wait, w1, w2});
        if (is_wait && w1 == 0xFFFF && w2 == 0xFFFE) break;
    }
    return events;
}

struct Band {
    uint64_t cycle; int beam; int line;
    uint16_t wait_w1 = 0;          // the WAIT event's own first word (C7: horizontal field)
    // The WAIT instruction word is FETCHED as soon as the Copper reaches it
    // in the list (immediately after the previous band's own last MOVE),
    // well before its condition is met; `cycle` is that fetch, not useful
    // for ordering. `first_move_cycle` is the cycle of the band's own first
    // MOVE, fetched only once the WAIT actually releases -- the right
    // reference point for "after the previous line's last bitplane fetch".
    uint64_t first_move_cycle = 0;
    uint64_t last_event_cycle = 0; // cycle of the last MOVE fetched in this band (C7: ordering)
    std::vector<std::pair<uint16_t,uint16_t>> moves;
};

struct FrameCop {
    bool found = false;
    std::map<uint16_t,uint16_t> header;               // register -> value, in header order
    std::vector<Band> bands;                            // one per WAIT (excluding the terminator)
    uint64_t start_cycle = 0, end_cycle = 0;
};

FrameCop decode_frame(const std::vector<nmh::RamAccess>& log, int frame) {
    FrameCop fc;
    auto ev = parse_frame_events(log, frame);
    if (ev.empty()) return fc;
    fc.found = true;
    fc.start_cycle = ev.front().cycle;
    fc.end_cycle = ev.back().cycle;
    bool in_header = true;
    for (auto& e : ev) {
        if (e.is_wait) {
            if (e.w1 == 0xFFFF && e.w2 == 0xFFFE) break; // terminator
            in_header = false;
            Band b; b.cycle = e.cycle; b.beam = (e.w1 >> 8) & 0xFF; b.line = b.beam - 0x2C;
            b.wait_w1 = e.w1; b.last_event_cycle = e.cycle;
            fc.bands.push_back(b);
        } else if (in_header) {
            fc.header[e.w1] = e.w2;
        } else {
            if (fc.bands.back().moves.empty()) fc.bands.back().first_move_cycle = e.cycle;
            fc.bands.back().moves.push_back({e.w1, e.w2});
            fc.bands.back().last_event_cycle = e.cycle;
        }
    }
    return fc;
}

// replay header + bands up to (not including) band index `upto` to get the
// effective register state at that point (SPECIFICATION.md 4.7's "rebuilt
// from the Copper MOVEs fetched before the line's first bitplane fetch").
std::map<uint16_t,uint16_t> effective_regs(const FrameCop& fc, size_t upto_band) {
    std::map<uint16_t,uint16_t> regs = fc.header;
    for (size_t i = 0; i < upto_band && i < fc.bands.size(); ++i)
        for (auto& mv : fc.bands[i].moves) regs[mv.first] = mv.second;
    return regs;
}

uint16_t color_reg(int n) { return (uint16_t)(0x180 + 2*n); } // n=0..31

// ---- bitplane fetch bursts (source lines) ----------------------------------

struct SourceLine {
    uint64_t cycle_lo, cycle_hi;
    std::vector<uint16_t> p1, p2;          // fetched data, in address order
    std::vector<uint32_t> p1_off, p2_off;  // word offset within the buffer
    bool pure_r = false;
    bool zero = false;                     // every fetched word came from the zero buffer
};

// Groups plane1/plane2 reads within [c_lo,c_hi) into per-line bursts by a
// cycle-gap threshold. `also_zero`: for F7+ the reload band switches the
// pointers to the zero buffer partway through -- callers pass the buffer
// base that applies (plane1/plane2, or the zero buffer) via `p1base/p2base`.
std::vector<SourceLine> find_source_lines(const std::vector<nmh::RamAccess>& log,
                                           uint64_t c_lo, uint64_t c_hi,
                                           uint32_t p1base, uint32_t p2base) {
    uint32_t b1 = word_addr(p1base), b2 = word_addr(p2base);
    std::vector<nmh::RamAccess> reads;
    for (auto& a : log) {
        if (a.is_write) continue;
        if (a.cycle < c_lo || a.cycle >= c_hi) continue;
        bool in1 = a.word_addr >= b1 && a.word_addr < b1 + kBufWords;
        bool in2 = a.word_addr >= b2 && a.word_addr < b2 + kBufWords;
        if (in1 || in2) reads.push_back(a);
    }
    std::stable_sort(reads.begin(), reads.end(), [](auto&a,auto&b){ return a.cycle < b.cycle; });
    auto co = nmh::coalesce(reads);
    std::vector<SourceLine> lines;
    constexpr uint64_t kGap = 300; // cycles; a raster line's active fetch is a
                                    // contiguous burst, separated from the next
                                    // line's burst by a much longer blanking gap
    for (auto& a : co) {
        if (lines.empty() || a.cycle - lines.back().cycle_hi > kGap) {
            lines.push_back(SourceLine{a.cycle, a.cycle, {}, {}, {}, {}, false});
        }
        auto& L = lines.back();
        L.cycle_hi = a.cycle;
        if (a.word_addr >= b1 && a.word_addr < b1 + kBufWords) {
            L.p1.push_back(a.data); L.p1_off.push_back(a.word_addr - b1);
        } else {
            L.p2.push_back(a.data); L.p2_off.push_back(a.word_addr - b2);
        }
    }
    for (auto& L : lines) {
        bool pure = !L.p1.empty();
        for (auto d : L.p1) if (d != 0xFFFF) pure = false;
        for (auto d : L.p2) if (d != 0xFFFF) pure = false;
        L.pure_r = pure;
    }
    return lines;
}

// ---- video helpers ----------------------------------------------------------

struct RGB2 { int r,g,b; }; // bits 3-2 of each channel
RGB2 top2(const nmh::VideoSample& s) { return { (s.r>>2)&3, (s.g>>2)&3, (s.b>>2)&3 }; }
bool eq2(const RGB2&a, const RGB2&b){ return a.r==b.r && a.g==b.g && a.b==b.b; }

struct Full12 { int r,g,b; }; // all 4 bits of each channel
Full12 full12(const nmh::VideoSample& s) { return { s.r, s.g, s.b }; }
bool eqfull(const Full12&a, const Full12&b){ return a.r==b.r && a.g==b.g && a.b==b.b; }

// ---- R1: which output-line span(s) a source-line burst is shown on --------
// (see the file header note on why direct cycle-range overlap is used
// instead of deriving an abstract m/Delta formula first).

struct LineMap { std::vector<size_t> span_idx; }; // indices into find_lines()

LineMap map_source_to_output(const std::vector<nmh::Span>& out_lines, uint64_t c_lo, uint64_t c_hi) {
    LineMap lm;
    for (size_t i = 0; i < out_lines.size(); ++i) {
        // overlap test: the burst's cycle range intersects the output line's
        // sample-index range (sample index == clk_sys cycle, see file header)
        if (out_lines[i].end > c_lo && out_lines[i].begin <= c_hi) lm.span_idx.push_back(i);
    }
    return lm;
}

// ---- pixel value from fetched words (R2) -----------------------------------
// v[i] = bit0 of plane1's word, bit1 of plane2's word, MSB (bit 15) first.
int pixel_value(const SourceLine& L, int i) {
    if (i < 0) return -1;
    size_t k = (size_t)(i / 16);
    int bitpos = 15 - (i % 16);
    if (k >= L.p1.size() || k >= L.p2.size()) return -1;
    int b1 = (L.p1[k] >> bitpos) & 1;
    int b2 = (L.p2[k] >> bitpos) & 1;
    return b1 | (b2 << 1);
}
int fetch_words(const SourceLine& L) { return (int)std::min(L.p1.size(), L.p2.size()); }

// ---- R0: hires-period sample width w and phase phi -------------------------
struct WPhi { int w = 0, phi = 0; bool ok = false; };

WPhi measure_w_phi(const std::vector<nmh::VideoSample>& video, nmh::Span span) {
    std::vector<std::pair<size_t,size_t>> runs;
    size_t i = span.begin;
    while (i < span.end) {
        size_t j = i + 1;
        while (j < span.end && eqfull(full12(video[j]), full12(video[i]))) ++j;
        runs.push_back({i, j});
        i = j;
    }
    if (runs.size() < 5) return {};
    int w = INT_MAX;
    for (size_t k = 1; k + 1 < runs.size(); ++k)
        w = std::min(w, (int)(runs[k].second - runs[k].first));
    if (w <= 0 || w == INT_MAX) return {};
    for (size_t k = 1; k + 1 < runs.size(); ++k)
        if ((int)(runs[k].second - runs[k].first) % w != 0) return {};
    int phi = -1;
    for (size_t k = 1; k + 1 < runs.size(); ++k) {
        int ph = (int)((runs[k].first - span.begin) % (size_t)w);
        if (phi < 0) phi = ph;
        else if (phi != ph) return {};
    }
    return {w, phi, true};
}

// ---- R3: the check interval on a pure-R line's output span ----------------
// The run of one constant colour bounded on both sides by pixels of one and
// the same other colour (the border colour, which shows left and right of the
// display window; blanking, beyond the border, differs from it); xL/xR are
// absolute video-sample indices.
struct Run12 { size_t lo = 0, hi = 0; bool ok = false; }; // [lo,hi)

// mode 0 (SuperHires): compares bits 3-2 only (L2: bits 1-0 carry no letter
// and are not predicted). mode 1/2 (hires/lores): compares all twelve bits
// (SPECIFICATION.md R3: "The value compared is bits 3-2 of R,G,B in
// SuperHires frames and all twelve bits in hires and lores frames.")
Run12 find_constant_run(const std::vector<nmh::VideoSample>& video, nmh::Span span, int mode) {
    Run12 best;
    size_t i = span.begin;
    auto same = [&](size_t a, size_t b) {
        if (mode == 0) return eq2(top2(video[a]), top2(video[b]));
        return eqfull(full12(video[a]), full12(video[b]));
    };
    while (i < span.end) {
        size_t j = i + 1;
        while (j < span.end && same(j, i)) ++j;
        // A run counts only when it touches neither edge of the span and the
        // pixel on its left shows the same colour as the pixel on its right:
        // the display window lies between two stretches of the border colour.
        // The border run beyond the window has blanking on its far side.
        bool inside = i > span.begin && j < span.end;
        if (inside && same(i - 1, j) && j - i > best.hi - best.lo) { best.lo = i; best.hi = j; best.ok = true; }
        i = j;
    }
    return best;
}

// ---- R1+R2 combined: given a frame's source lines and w/phi/mode, find the
// unique s that makes bits3-2 (SuperHires) or all 12 bits (hires/lores) a
// consistent function of v[i], over every graded pixel of a set of lines.
// mode: 0=SuperHires (i=2x-s), 1=hires (i=x-s), 2=lores (i=floor((x-s)/2))
int pixel_index(int mode, int x, int s) {
    if (mode == 0) return 2*x - s;
    if (mode == 1) return x - s;
    int d = x - s;
    return (int)std::floor(d / 2.0);
}

// ---- P4 bit-to-(component,Color,upper/lower) table (SPECIFICATION.md 3,
// "Writing the table"): COLOR[n] = R_p<<10|R_q<<8|G_p<<6|G_q<<4|B_p<<2|B_q,
// p=n%4, q=n/4. Re-derived independently from the printed formula, not
// read from the data tables' own bytes.
enum class Comp { R, G, B };
struct BitTarget { Comp x; int c; bool upper; };
BitTarget bit_target(int n, int b) {
    int p = n % 4, q = n / 4;
    switch (b) {
        case 11: return {Comp::R, p, true};
        case 10: return {Comp::R, p, false};
        case 9:  return {Comp::R, q, true};
        case 8:  return {Comp::R, q, false};
        case 7:  return {Comp::G, p, true};
        case 6:  return {Comp::G, p, false};
        case 5:  return {Comp::G, q, true};
        case 4:  return {Comp::G, q, false};
        case 3:  return {Comp::B, p, true};
        case 2:  return {Comp::B, p, false};
        case 1:  return {Comp::B, q, true};
        default: return {Comp::B, q, false}; // b==0
    }
}
// The 192 (n,b) pairs in the SAME row-major order the stimulus plan uses
// (SPECIFICATION.md 4.6: n outer 00-0F, b inner 00-11, split 80/80/32
// across F2/F3/F4) -- this labels which proposition a band is FOR; the
// verdict itself is always read from the port, never assumed.
std::vector<std::pair<int,int>> perturbation_plan_all() {
    std::vector<std::pair<int,int>> v;
    for (int n = 0; n < 16; ++n) for (int b = 0; b < 12; ++b) v.push_back({n, b});
    return v;
}

// ---- a frame's graded lines, resolved once and reused by every proposition
struct GradedFrame {
    bool ok = false;
    int mode = 0;                 // 0=SuperHires,1=hires,2=lores
    FrameCop fc;
    std::vector<SourceLine> lines;              // every source line, in order
    std::vector<std::vector<nmh::Span>> out_of; // out_of[i] = output spans of lines[i]
    int s = 0;
    size_t xL = 0, xR = 0;        // check interval [xL,xR) in each line's own local x (0-based within its first output span)
    int w = 0, phi = 0;
};

int frame_mode(int frame_bplcon0) {
    if (frame_bplcon0 == 0x2040) return 0;
    if (frame_bplcon0 == 0xA000 || frame_bplcon0 == 0x1040 || frame_bplcon0 == 0x9000) return 1;
    if (frame_bplcon0 == 0x2000) return 2;
    return 0;
}

// ---- R0 applied: one video sample per visible pixel x -----------------
// x is counted in hires periods from the span's own begin; its representative
// sample is span.begin + phi + x*w (R0: "visible pixel x ... is the sample
// t_hs + phi + x*w"). Returns nullopt if that sample falls outside the span.
std::optional<size_t> pixel_sample(const nmh::Span& sp, int w, int phi, long x) {
    if (w <= 0) return std::nullopt;
    long long idx = (long long)sp.begin + phi + x * (long long)w;
    if (idx < (long long)sp.begin || (size_t)idx >= sp.end) return std::nullopt;
    return (size_t)idx;
}

// finds s by scanning candidate s in a generous range over lines[line_idxs],
// using their FIRST mapped output span only (R1: every copy must agree,
// checked separately as a precondition where it matters).
template <typename ColourT, typename SampleFn, typename EqFn>
bool try_s(const std::vector<nmh::VideoSample>& video, const std::vector<SourceLine>& lines,
           const std::vector<std::vector<nmh::Span>>& out_of, const std::vector<int>& line_idxs,
           int mode, int s, int w, int phi, long xlo, long xhi, SampleFn sample_fn, EqFn eq_fn) {
    std::map<int, ColourT> byval;
    bool any = false;
    for (int li : line_idxs) {
        if (out_of[li].empty()) continue;
        nmh::Span sp = out_of[li].back();
        for (long x = xlo; x < xhi; ++x) {
            auto oidx = pixel_sample(sp, w, phi, x);
            if (!oidx) continue;
            int i = pixel_index(mode, (int)x, s);
            int v = pixel_value(lines[li], i);
            if (v < 0) continue;
            ColourT c = sample_fn(video[*oidx]);
            auto it = byval.find(v);
            if (it == byval.end()) byval[v] = c;
            else if (!eq_fn(it->second, c)) return false;
            any = true;
        }
    }
    return any && byval.size() >= 2;
}

struct SSearch { int s = 0; int n_fit = 0; bool ok = false; };

SSearch find_s(const std::vector<nmh::VideoSample>& video, const std::vector<SourceLine>& lines,
               const std::vector<std::vector<nmh::Span>>& out_of, const std::vector<int>& line_idxs,
               int mode, int w, int phi, long xlo, long xhi) {
    SSearch r;
    // s absorbs the fetch-to-display offset (DDFSTRT starts fetching well
    // before DIWSTRT's own display start), so it is not small; search a
    // range comfortably wider than the whole check interval.
    for (int s = -900; s <= 900; ++s) {
        bool fits;
        if (mode == 0)
            fits = try_s<RGB2>(video, lines, out_of, line_idxs, mode, s, w, phi, xlo, xhi,
                                [](const nmh::VideoSample& v){ return top2(v); }, eq2);
        else
            fits = try_s<Full12>(video, lines, out_of, line_idxs, mode, s, w, phi, xlo, xhi,
                                  [](const nmh::VideoSample& v){ return full12(v); }, eqfull);
        if (fits) { r.s = s; ++r.n_fit; }
    }
    r.ok = (r.n_fit == 1);
    return r;
}

// ---- baseline builder for P4/P5.L: keyed by (v[i-1],v[i],v[i+1]) ----------
struct Triple { int a, b, c; bool operator<(const Triple& o) const {
    return std::tie(a,b,c) < std::tie(o.a,o.b,o.c); } };

struct Baseline {
    std::map<Triple, Full12> value;
    std::set<Triple> ambiguous;
};

Baseline build_baseline(const std::vector<nmh::VideoSample>& video, const std::vector<SourceLine>& lines,
                         const std::vector<std::vector<nmh::Span>>& out_of, const std::vector<int>& line_idxs,
                         int mode, int s, int w, int phi, long xlo, long xhi) {
    Baseline bl;
    for (int li : line_idxs) {
        if (out_of[li].empty()) continue;
        nmh::Span sp = out_of[li].back();
        for (long x = xlo; x < xhi; ++x) {
            auto oidx = pixel_sample(sp, w, phi, x);
            if (!oidx) continue;
            int i = pixel_index(mode, (int)x, s);
            int v = pixel_value(lines[li], i);
            // SPECIFICATION.md 7/P4 item 3 writes the baseline key literally as
            // (v[i-1], v[i], v[i+1]) -- one source-pixel step, not one
            // visible-pixel step, in every mode.
            int vm1 = pixel_value(lines[li], i - 1);
            int vp1 = pixel_value(lines[li], i + 1);
            if (v < 0) continue;
            Triple t{vm1, v, vp1};
            Full12 c = full12(video[*oidx]);
            auto it = bl.value.find(t);
            if (it == bl.value.end()) bl.value[t] = c;
            else if (!eqfull(it->second, c)) bl.ambiguous.insert(t);
        }
    }
    for (auto& t : bl.ambiguous) bl.value.erase(t);
    return bl;
}

struct FrameLines {
    FrameCop fc;
    std::vector<SourceLine> lines;
    std::vector<std::vector<nmh::Span>> out_of;
};

FrameLines decode_frame_lines(const Capture& cap, int frame_idx, uint32_t p1base, uint32_t p2base) {
    FrameLines fl;
    fl.fc = decode_frame(cap.log, frame_idx);
    if (!fl.fc.found) return fl;
    FrameCop nextfc = frame_idx < 10 ? decode_frame(cap.log, frame_idx + 1) : FrameCop{};
    uint64_t hi = (frame_idx < 10 && nextfc.found) ? nextfc.start_cycle : fl.fc.end_cycle + 3'000'000;
    fl.lines = find_source_lines(cap.log, fl.fc.start_cycle, hi, p1base, p2base);
    auto outlines = nmh::find_lines(cap.video);
    for (auto& L : fl.lines) {
        LineMap lm = map_source_to_output(outlines, L.cycle_lo, L.cycle_hi);
        std::vector<nmh::Span> spans;
        for (size_t idx : lm.span_idx) spans.push_back(outlines[idx]);
        fl.out_of.push_back(spans);
    }
    return fl;
}

// R0: measure w,phi once from a line comfortably inside the pattern section
// (not the R section) of any hires or SuperHires frame's own decoded lines.
WPhi measure_global_w_phi(const std::vector<nmh::VideoSample>& video, const FrameLines& fl) {
    for (size_t li = 10; li < fl.lines.size() && li < 40; ++li) {
        if (fl.lines[li].pure_r || fl.out_of[li].empty()) continue;
        WPhi wp = measure_w_phi(video, fl.out_of[li].back());
        if (wp.ok) return wp;
    }
    return {};
}

GradedFrame finish_graded_frame(const Capture& cap, FrameLines&& fl, int w, int phi, int top_n = 24) {
    GradedFrame gf;
    gf.fc = std::move(fl.fc);
    gf.lines = std::move(fl.lines);
    gf.out_of = std::move(fl.out_of);
    gf.w = w; gf.phi = phi;
    if (!gf.fc.found || w <= 0) return gf;
    auto it0 = gf.fc.header.find(0x100);
    int bplcon0 = it0 != gf.fc.header.end() ? it0->second : -1;
    gf.mode = frame_mode(bplcon0);
    // check interval (in pixel units) from a pure-R line whose run falls
    // inside C6's own stated bound (256-576 hires periods); the very first
    // source line of a frame is occasionally an edge artifact (its mapped
    // output span picks up extra content), so every pure-R line is tried
    // and the first one landing in-bound wins -- not a tolerance of our
    // own, C6's own numbers (SPECIFICATION.md 6).
    for (size_t li = 0; li < gf.lines.size(); ++li) {
        if (!gf.lines[li].pure_r || gf.out_of[li].empty()) continue;
        Run12 r = find_constant_run(cap.video, gf.out_of[li].back(), gf.mode);
        if (!r.ok) continue;
        nmh::Span sp = gf.out_of[li].back();
        long xlo = (long)((r.lo - sp.begin - phi + w - 1) / w);
        long xhi = (long)((r.hi - sp.begin - phi) / w);
        long width = xhi - xlo;
        if (width < 256 || width > 576) continue;
        gf.xL = (size_t)(xlo + 8);
        gf.xR = (size_t)(xhi - 8);
        break;
    }
    // The top Q0 lines are source lines 0 to top_n-1 of the frame: window line 0, the line
    // immediately after the Copper header, shows the fetched bitplane data like every other line.
    std::vector<int> top_idx;
    for (int i = 0; i < top_n && i < (int)gf.lines.size(); ++i) top_idx.push_back(i);
    if (gf.xR > gf.xL) {
        SSearch ss = find_s(cap.video, gf.lines, gf.out_of, top_idx, gf.mode, w, phi,
                             (long)gf.xL, (long)gf.xR);
        gf.s = ss.s;
        gf.ok = ss.ok;
        if (gf.ok) {
            // SPECIFICATION.md R3: the check interval also satisfies 16 <= i(x) <= 16F-17, which leaves out one
            // data word at each end of the fetch (F = fetched words per plane on a line, the count most lines show).
            std::map<int, int> per_count;
            for (auto& L : gf.lines) { int fw = fetch_words(L); if (fw > 0) ++per_count[fw]; }
            size_t f_words = SIZE_MAX;
            int best_n = 0;
            for (auto& kv : per_count) if (kv.second > best_n) { best_n = kv.second; f_words = (size_t)kv.first; }
            if (f_words != SIZE_MAX) {
                long i_hi = 16L * (long)f_words - 17;
                long x_lo = (long)gf.xL, x_hi = (long)gf.xR - 1;
                while (x_lo <= x_hi && pixel_index(gf.mode, (int)x_lo, gf.s) < 16) ++x_lo;
                while (x_hi >= x_lo && pixel_index(gf.mode, (int)x_hi, gf.s) > i_hi) --x_hi;
                if (x_hi >= x_lo) { gf.xL = (size_t)x_lo; gf.xR = (size_t)(x_hi + 1); }
            }
        }
    }
    return gf;
}

// P1: per (Color,Component) proposition state, accumulated across every
// stimulus instance this package grades (F1's own bands; F2-F4's Q0
// sections). index = color*3+component (R=0,G=1,B=2).
struct P1State { long total[12] = {0}; long fail[12] = {0}; };
// P2 (SPECIFICATION.md P2): grouped by (component, the two encoded letters
// of X of Color c AS THE PALETTE GIVES THEM) -> every pixel in a group must
// show the identical full 4-bit component value. key = (comp, letters 0-3).
struct P2State {
    std::map<std::pair<int,int>, int> bykey[3];
    std::set<std::pair<int,int>> bad[3];
    long total[3] = {0};
    // SPECIFICATION.md 7/P2 item 4: "at least one group must contain pixels
    // from two palettes that differ in some other Color's letters of X" --
    // tracked as the set of distinct palettes that contributed to each
    // (c, letters) group.
    std::map<std::pair<int,int>, std::set<std::string>> sources[3];
};

void grade_range_p1p2(const std::vector<nmh::VideoSample>& video, const GradedFrame& gf,
                       const std::vector<uint16_t>& palette, int line_lo, int line_hi,
                       P1State* p1, P2State* p2, const std::string& band_id = "") {
    if (gf.mode != 0) return; // P1/P2 are SuperHires-only
    for (int li = line_lo; li <= line_hi && li < (int)gf.lines.size(); ++li) {
        if (gf.out_of[li].empty()) continue;
        nmh::Span sp = gf.out_of[li].back();
        for (size_t x = gf.xL; x < gf.xR; ++x) {
            auto oidx = pixel_sample(sp, gf.w, gf.phi, (long)x);
            if (!oidx) continue;
            size_t idx = *oidx;
            int i = pixel_index(0, (int)x, gf.s);
            int v = pixel_value(gf.lines[li], i);
            if (v < 0 || v > 3) continue;
            RGB2 top = top2(video[idx]);
            int shown[3] = { top.r, top.g, top.b };
            int full[3] = { video[idx].r, video[idx].g, video[idx].b };
            int shift[3] = {10, 6, 2};
            for (int comp = 0; comp < 3; ++comp) {
                int expected = (palette[v] >> shift[comp]) & 3;
                if (p1) {
                    int pidx = v*3+comp;
                    p1->total[pidx]++;
                    if (shown[comp] != expected) {
                        p1->fail[pidx]++;
                    }
                }
                if (p2) {
                    p2->total[comp]++;
                    auto k = std::make_pair(v, expected); // (c, letters)
                    auto it = p2->bykey[comp].find(k);
                    if (it == p2->bykey[comp].end()) p2->bykey[comp][k] = full[comp];
                    else if (it->second != full[comp]) {
                        p2->bad[comp].insert(k);
                    }
                    if (!band_id.empty()) p2->sources[comp][k].insert(band_id);
                }
            }
        }
    }
}

// P3 (coarseness): collects, per component, the set of distinct full 4-bit
// values shown at value-1 pixels across a set of bands.
struct P3Comp { std::set<int> vals[3]; long total = 0; };

void collect_p3(const std::vector<nmh::VideoSample>& video, const GradedFrame& gf,
                 int line_lo, int line_hi, int target_value, P3Comp& out) {
    for (int li = line_lo; li <= line_hi && li < (int)gf.lines.size(); ++li) {
        if (gf.out_of[li].empty()) continue;
        nmh::Span sp = gf.out_of[li].back();
        for (size_t x = gf.xL; x < gf.xR; ++x) {
            auto oidx = pixel_sample(sp, gf.w, gf.phi, (long)x);
            if (!oidx) continue;
            size_t idx = *oidx;
            int i = pixel_index(gf.mode, (int)x, gf.s);
            int v = pixel_value(gf.lines[li], i);
            if (v != target_value) continue;
            out.vals[0].insert(video[idx].r);
            out.vals[1].insert(video[idx].g);
            out.vals[2].insert(video[idx].b);
            out.total++;
        }
    }
}

// ---- P4: per-bit table propositions ----------------------------------------
struct P4Verdict {
    bool observed = false; bool fail = false;
    long changed = 0, checked = 0, excluded = 0;
    long any_changed = 0; // P5.L: counts ANY bit differing, not just the allowed one
    // P1K: per pixel value and component, the checked pixels and the OR over them of (baseline XOR observed) in all four bits
    long vcnt[4] = {0, 0, 0, 0};
    int xdiff[4][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
};

// Grades one perturbation band (2 graded lines, both content -- these bands
// are 1-2 MOVEs, well under the 4-MOVE spacer threshold) against the
// baseline, for the (n,b) proposition this band is FOR.
P4Verdict grade_p4_band(const std::vector<nmh::VideoSample>& video, const GradedFrame& gf,
                         const Baseline& bl, int band_idx, int n, int b) {
    P4Verdict pv;
    if (band_idx < 0 || band_idx >= (int)gf.fc.bands.size()) return pv;
    const Band& band = gf.fc.bands[band_idx];
    int line_lo = band.line, line_hi = band.line + 1; // 2 content lines, not a spacer
    BitTarget t = bit_target(n, b);
    int shift = t.x == Comp::R ? 0 : t.x == Comp::G ? 1 : 2; // index into {r,g,b}
    for (int li = line_lo; li <= line_hi && li < (int)gf.lines.size(); ++li) {
        if (gf.out_of[li].empty()) continue;
        nmh::Span sp = gf.out_of[li].back();
        for (size_t x = gf.xL; x < gf.xR; ++x) {
            auto oidx = pixel_sample(sp, gf.w, gf.phi, (long)x);
            if (!oidx) continue;
            size_t idx = *oidx;
            int i = pixel_index(gf.mode, (int)x, gf.s);
            int v = pixel_value(gf.lines[li], i);
            // literal spec key: one source-pixel step, every mode.
            int vm1 = pixel_value(gf.lines[li], i - 1);
            int vp1 = pixel_value(gf.lines[li], i + 1);
            if (v < 0) continue;
            Triple tr{vm1, v, vp1};
            auto it = bl.value.find(tr);
            if (it == bl.value.end()) { pv.excluded++; continue; }
            Full12 base = it->second;
            Full12 obs = full12(video[idx]);
            pv.checked++;
            int base_c[3] = {base.r, base.g, base.b};
            int obs_c[3]  = {obs.r, obs.g, obs.b};
            if (v >= 0 && v <= 3) {
                pv.vcnt[v]++;
                for (int comp = 0; comp < 3; ++comp) pv.xdiff[v][comp] |= base_c[comp] ^ obs_c[comp];
            }
            bool any_diff = false;
            // "observed" (the allowed bit itself moved) and "no OTHER bit
            // moved" are separate questions -- a mutation (V4d) can make the
            // allowed bit move together with an extra one; that must show as
            // OBSERVED and FAIL through the extra bit, never as "not
            // observable".
            bool allowed_only = true;
            bool saw_allowed_change = false;
            for (int comp = 0; comp < 3; ++comp) {
                if (base_c[comp] == obs_c[comp]) continue;
                any_diff = true;
                int diff = base_c[comp] ^ obs_c[comp];
                if (comp == shift && v == t.c) {
                    int own_bit = t.upper ? 0x8 : 0x4; // bit3 or bit2 of the nibble: the letter THIS proposition names
                    // SPECIFICATION.md 7/P4 item 4: the allowed places are "bit 3
                    // or 2 of X (per L) AND bits 1-0 of X" on a value-c pixel --
                    // the dashes (L2) are unencoded and may move on ANY
                    // inversion of a bit that shares X's nibble, not just this
                    // one; only OBSERVED tracks the bit this proposition itself
                    // names.
                    if (diff & own_bit) saw_allowed_change = true;
                    if ((diff & ~(own_bit | 0x3)) == 0) continue; // only allowed positions moved here
                }
                allowed_only = false;
            }
            if (!allowed_only) pv.fail = true;
            if (saw_allowed_change) { pv.observed = true; pv.changed++; }
            if (any_diff) pv.any_changed++;
        }
    }
    return pv;
}

// ---- Chip-RAM reads below $80000 (C3, C9) ---------------------------------
// In F1-F6 the chips read there only words of the two stream buffers
// (bitplane DMA), words of the Copper lists (Copper DMA) and the two words of
// the null sprite (sprite DMA, p.291), apart from the refresh slots. Every other read below $80000 is a
// stray read: another bitplane address, or a sprite DMA word taken from
// somewhere other than the null sprite. The program, its stack and the stored
// results lie at $80000 and above.
enum class ReadKind { Plane, Copper, NullSprite, Refresh, Stray, Outside };

ReadKind classify_read(uint32_t wa) {
    uint32_t b = wa * 2;
    if (b >= 0x080000) return ReadKind::Outside;
    // The port shows four reads of word 0 in every line, the memory-refresh slots; no pointer of F1-F6 addresses word 0.
    if (wa == 0) return ReadKind::Refresh;
    if ((b >= kPlane1Buf && b < kPlane1Buf + 2 * kBufWords) || (b >= kPlane2Buf && b < kPlane2Buf + 2 * kBufWords))
        return ReadKind::Plane;
    if (b >= kCopBase && b < kCopBase + 11 * kCopStep) return ReadKind::Copper;
    if (b >= kNullSpr && b < kNullSpr + 4) return ReadKind::NullSprite;
    return ReadKind::Stray;
}

// Cycles of every stray read of the run, ascending.
std::vector<uint64_t> stray_read_cycles(const std::vector<nmh::RamAccess>& log) {
    std::vector<uint64_t> v;
    for (auto& a : log)
        if (!a.is_write && classify_read(a.word_addr) == ReadKind::Stray) v.push_back(a.cycle);
    std::sort(v.begin(), v.end());
    return v;
}

// ---- C9: every sprite DMA read of a frame is the null sprite ---------------
// The sprite DMA reads of a frame are its Chip-RAM reads below $80000 that are
// neither bitplane nor Copper words. C9 holds when the frame has such reads,
// all of them from the null sprite, and every one returns $0000.
struct SprDma { long null_reads = 0, stray = 0; bool null_zero = true; };

SprDma sprite_dma_reads(const std::vector<nmh::RamAccess>& log, uint64_t lo, uint64_t hi) {
    SprDma s;
    for (auto& a : log) {
        if (a.is_write || a.cycle < lo || a.cycle >= hi) continue;
        ReadKind k = classify_read(a.word_addr);
        if (k == ReadKind::NullSprite) { ++s.null_reads; if (a.data != 0x0000) s.null_zero = false; }
        else if (k == ReadKind::Stray) ++s.stray;
    }
    return s;
}
bool precondition_c9(const SprDma& s) { return s.null_reads > 0 && s.stray == 0 && s.null_zero; }

// ---- C3 on one frame: F>=16, same count per plane, consecutive addresses in
// both planes, data equal to what the program wrote (independent xorshift
// regenerate), and no other bitplane address: no stray read inside the burst
// of the line's own bitplane fetches.
bool precondition_c3(const GradedFrame& gf, const std::vector<uint16_t>& p1ref,
                      const std::vector<uint16_t>& p2ref, const std::vector<uint64_t>& stray,
                      long* bad_lines) {
    long bad = 0;
    for (auto& L : gf.lines) {
        if (L.p1.size() < 16 || L.p1.size() != L.p2.size()) { ++bad; continue; }
        bool line_bad = false;
        for (size_t k = 0; k + 1 < L.p1_off.size(); ++k)
            if (L.p1_off[k+1] != L.p1_off[k] + 1) { line_bad = true; break; }
        for (size_t k = 0; k + 1 < L.p2_off.size(); ++k)
            if (L.p2_off[k+1] != L.p2_off[k] + 1) { line_bad = true; break; }
        for (size_t k = 0; k < L.p1.size(); ++k) {
            uint32_t off = L.p1_off[k];
            uint16_t exp1 = off < 256 ? 0xFFFF : (off-256 < p1ref.size() ? p1ref[off-256] : 0xFFFF);
            uint32_t off2 = L.p2_off[k];
            uint16_t exp2 = off2 < 256 ? 0xFFFF : (off2-256 < p2ref.size() ? p2ref[off2-256] : 0xFFFF);
            if (L.p1[k] != exp1 || L.p2[k] != exp2) { line_bad = true; break; }
        }
        auto it = std::lower_bound(stray.begin(), stray.end(), L.cycle_lo);
        if (it != stray.end() && *it <= L.cycle_hi) line_bad = true;
        if (line_bad) ++bad;
    }
    if (bad_lines) *bad_lines = bad;
    return bad == 0 && !gf.lines.empty();
}

// ---- C8 contrast pattern (SPECIFICATION.md 6/C8): "the pattern section
// restricted to values 0 and 3" -- confirmed against bp_data_pattern1_vc8.i
// and bp_data_pattern2_vc8.i, which are word-for-word the baseline's own
// plane-2 table on BOTH planes (v = 2*b2+b1 = 2*b2+b2 = 3*b2, so v in {0,3}).
std::pair<std::vector<uint16_t>, std::vector<uint16_t>> build_pattern_vc8() {
    auto base = build_pattern();
    return {base.second, base.second};
}

// ---- C4: the header MOVEs carry the SPECIFICATION.md 4.3 data,
// not just the planned count. q0 gives COLOR00-0F; COLOR10-1F (the sprite
// palette T0) carries the identical sixteen values (SPECIFICATION.md 3: T0
// and Q0 are numerically the same table).
bool check_frame_header(const FrameCop& fc, uint16_t expected_bplcon0, const std::vector<uint16_t>& q0) {
    if (!fc.found) return false;
    auto get = [&](uint16_t reg) -> long {
        auto it = fc.header.find(reg);
        return it != fc.header.end() ? (long)it->second : -1;
    };
    if (get(0x100) != expected_bplcon0) return false;
    if (get(0x102) != 0x0000) return false;
    if (get(0x104) != 0x0024) return false;
    if (get(0x08E) != 0x2C81) return false;
    if (get(0x090) != 0xF4A1) return false;
    if (get(0x092) != 0x003C) return false;
    if (get(0x094) != 0x00D4) return false;
    if (get(0x108) != 0x0000) return false;
    if (get(0x10A) != 0x0000) return false;
    if (get(0x0E0) != (long)(kPlane1Buf >> 16)) return false;
    if (get(0x0E2) != (long)(kPlane1Buf & 0xFFFF)) return false;
    if (get(0x0E4) != (long)(kPlane2Buf >> 16)) return false;
    if (get(0x0E6) != (long)(kPlane2Buf & 0xFFFF)) return false;
    // SPR0PT-SPR7PT ($120-$13E, four bytes apart) all hold the null sprite (SPECIFICATION.md 4.3)
    for (int k = 0; k < 8; ++k) {
        if (get((uint16_t)(0x120 + 4 * k)) != (long)(kNullSpr >> 16)) return false;
        if (get((uint16_t)(0x122 + 4 * k)) != (long)(kNullSpr & 0xFFFF)) return false;
    }
    if (q0.size() != 16) return false;
    for (int n = 0; n < 16; ++n) if (get(color_reg(n)) != q0[n]) return false;
    for (int n = 16; n < 32; ++n) if (get(color_reg(n)) != q0[n-16]) return false; // COLOR10-1F = T0 = Q0's values
    return true;
}

// ---- R2: "in F1-F6 the same s fits the bottom Q0 lines" --
// re-runs find_s on the tail population (after the frame's own closing
// band) and checks it agrees with the s already found from the top lines.
bool check_bottom_s(const std::vector<nmh::VideoSample>& video, const GradedFrame& gf, int close_band_idx) {
    if (!gf.ok || close_band_idx < 0 || close_band_idx >= (int)gf.fc.bands.size()) return false;
    std::vector<int> idxs;
    int lo = gf.fc.bands[close_band_idx].line + 1;
    for (int i = lo; i <= 199 && i < (int)gf.lines.size(); ++i) idxs.push_back(i);
    if (idxs.empty() || gf.xR <= gf.xL) return false;
    SSearch ss = find_s(video, gf.lines, gf.out_of, idxs, gf.mode, gf.w, gf.phi, (long)gf.xL, (long)gf.xR);
    return ss.ok && ss.s == gf.s;
}

// ---- C6: "identical ends on all pure R lines", asserted, not
// silently filtered to the first line that happens to fit.
// Every pure-R line is checked: it must show a run (R3), the run must be
// 256-576 hires periods wide (SPECIFICATION.md 6/C6's own bound), and its
// ends must equal those of the first pure-R line's run. A line with no run,
// a run outside the bound or other ends counts as mismatched.
struct C6Result { bool ok = false; long xL = 0, xR = 0; int lines_checked = 0, lines_mismatch = 0, bad_width = 0, no_run = 0; };
C6Result check_c6(const std::vector<nmh::VideoSample>& video, const GradedFrame& gf) {
    C6Result r;
    bool have = false;
    if (gf.w <= 0) return r;
    for (size_t li = 0; li < gf.lines.size(); ++li) {
        if (!gf.lines[li].pure_r || li >= gf.out_of.size() || gf.out_of[li].empty()) continue;
        ++r.lines_checked;
        Run12 run = find_constant_run(video, gf.out_of[li].back(), gf.mode);
        if (!run.ok) { ++r.no_run; ++r.lines_mismatch; continue; }
        nmh::Span sp = gf.out_of[li].back();
        long xlo = (long)((run.lo - sp.begin - gf.phi + gf.w - 1) / gf.w);
        long xhi = (long)((run.hi - sp.begin - gf.phi) / gf.w);
        long width = xhi - xlo;
        if (width < 256 || width > 576) { ++r.bad_width; ++r.lines_mismatch; continue; }
        long xL = xlo + 8, xR = xhi - 8;
        if (!have) { r.xL = xL; r.xR = xR; have = true; }
        else if (xL != r.xL || xR != r.xR) ++r.lines_mismatch;
    }
    r.ok = have && r.lines_checked > 0 && r.lines_mismatch == 0 && r.xR > r.xL;
    return r;
}

// ---- C7: SPECIFICATION.md 6/C7's own timing condition -- every
// band's WAIT is at horizontal $00, fetched after the previous line's last
// bitplane fetch and, spacer lines excepted, before its own line's first.
// A spacer is a band of more than four MOVEs (4.4: "A band change of at
// most four MOVEs is planned to end before the line's first bitplane
// fetch... A band change that writes a whole palette (sixteen MOVEs)... is
// a spacer line").
struct C7Result { bool ok = false; int bands_checked = 0, bad_h = 0, bad_after = 0, bad_before = 0; };
C7Result check_c7_timing(const GradedFrame& gf) {
    C7Result r;
    for (const Band& band : gf.fc.bands) {
        ++r.bands_checked;
        if ((band.wait_w1 & 0xFF) != 0x01) ++r.bad_h;
        bool after_prev = true;
        if (band.line > 0 && band.line - 1 < (int)gf.lines.size() && !band.moves.empty())
            after_prev = gf.lines[band.line - 1].cycle_hi < band.first_move_cycle;
        if (!after_prev) ++r.bad_after;
        bool is_spacer = band.moves.size() > 4;
        bool before_own = true;
        if (!is_spacer && band.line >= 0 && band.line < (int)gf.lines.size())
            before_own = band.last_event_cycle < gf.lines[band.line].cycle_lo;
        if (!before_own) ++r.bad_before;
    }
    r.ok = r.bands_checked > 0 && r.bad_h == 0 && r.bad_after == 0 && r.bad_before == 0;
    return r;
}

// ---- C8: SPECIFICATION.md 6/C8's own population -- every one of
// the 16 ordered (v,v') pairs occurs, as (v[i],v[i+1]) and as (v[i-1],v[i]),
// over a band's own graded lines. i ranges a little past the check
// interval's own mapped span on each side, so the pair straddling either
// edge is covered too.
bool band_pair_coverage(const GradedFrame& gf, int line_lo, int line_hi) {
    if (gf.w <= 0 || gf.xR <= gf.xL) return false;
    long ilo = pixel_index(gf.mode, (int)gf.xL, gf.s) - 2;
    long ihi = pixel_index(gf.mode, (int)gf.xR, gf.s) + 2;
    std::set<std::pair<int,int>> fwd, bwd;
    for (int li = line_lo; li <= line_hi && li < (int)gf.lines.size(); ++li) {
        for (long i = ilo; i < ihi; ++i) {
            int v0 = pixel_value(gf.lines[li], (int)i), v1 = pixel_value(gf.lines[li], (int)i + 1);
            if (v0 >= 0 && v1 >= 0) fwd.insert({v0, v1});
        }
        for (long i = ilo + 1; i <= ihi; ++i) {
            int vm1 = pixel_value(gf.lines[li], (int)i - 1), v = pixel_value(gf.lines[li], (int)i);
            if (vm1 >= 0 && v >= 0) bwd.insert({vm1, v});
        }
    }
    return fwd.size() == 16 && bwd.size() == 16;
}

// ---- R1: "every copy is graded and the copies must agree"
// (SPECIFICATION.md 4.7) -- compares the FIRST and LAST output span of every
// source line that maps to two or more, at every graded x.
struct R1Result { bool ok = false; long pairs = 0, mismatches = 0; };
R1Result check_r1_copies(const std::vector<nmh::VideoSample>& video, const GradedFrame& gf) {
    R1Result r;
    if (gf.w <= 0 || gf.xR <= gf.xL) return r;
    // A source line's own two doubled copies are NOT out_of[li].front() and
    // out_of[li].back(): the fetch burst's cycle range overlaps the tail of
    // the PREVIOUS line's second display copy and the head of this line's
    // own first (SPECIFICATION.md 10.2's own measurement -- "the package's
    // 'last' span of line k is the FIRST of the two output lines that show
    // k, and its 'first' span of line k is the SECOND copy of line k-1").
    // So line li's own pair is out_of[li].back() (its first copy) and
    // out_of[li+1].front() (its second copy, found under li+1's own
    // mapping).
    for (size_t li = 0; li + 1 < gf.out_of.size(); ++li) {
        if (gf.out_of[li].empty() || gf.out_of[li+1].empty()) continue;
        nmh::Span a = gf.out_of[li].back();
        nmh::Span b = gf.out_of[li+1].front();
        for (size_t x = gf.xL; x < gf.xR; ++x) {
            auto ia = pixel_sample(a, gf.w, gf.phi, (long)x);
            auto ib = pixel_sample(b, gf.w, gf.phi, (long)x);
            if (!ia || !ib) continue;
            ++r.pairs;
            if (!eqfull(full12(video[*ia]), full12(video[*ib]))) ++r.mismatches;
        }
    }
    r.ok = r.pairs > 0 && r.mismatches == 0;
    return r;
}

// ============================================================================
// F7-F10: the sprite colour frames (SPECIFICATION.md 4.6, P5.U, P6.1, P6.2,
// C10, C11)
// ============================================================================

// Source lines of a frame whose bitplane pointers move from the stream buffers
// to the zero buffer partway down the frame (SPECIFICATION.md 4.6, F7-F10).
// Plane-1 and plane-2 reads of the zero buffer address the same words, so a
// line of the zero buffer is kept as a burst of zero words, not as two planes.
std::vector<SourceLine> find_source_lines_z(const std::vector<nmh::RamAccess>& log,
                                             uint64_t c_lo, uint64_t c_hi) {
    uint32_t b1 = word_addr(kPlane1Buf), b2 = word_addr(kPlane2Buf), bz = word_addr(kZeroBuf);
    struct Raw { uint64_t cycle; uint32_t wa; uint16_t data; };
    std::vector<Raw> all_reads;
    std::vector<int> kind; // 0 stream, 1 zero buffer
    for (auto& a : log) {
        if (a.is_write || a.cycle < c_lo || a.cycle >= c_hi) continue;
        bool in1 = a.word_addr >= b1 && a.word_addr < b1 + kBufWords;
        bool in2 = a.word_addr >= b2 && a.word_addr < b2 + kBufWords;
        bool inz = a.word_addr >= bz && a.word_addr < bz + kBufWords;
        if (!(in1 || in2 || inz)) continue;
        all_reads.push_back({a.cycle, a.word_addr, a.data});
        kind.push_back(inz ? 1 : 0);
    }
    std::vector<SourceLine> lines;
    constexpr uint64_t kGap = 300;
    std::vector<long> zero_reads; std::vector<bool> zero_all;
    nmh::RamAccess prev{}; bool have_prev = false;
    for (size_t i = 0; i < all_reads.size(); ++i) {
        const Raw& r = all_reads[i];
        if (lines.empty() || r.cycle - lines.back().cycle_hi > kGap) {
            lines.push_back(SourceLine{r.cycle, r.cycle, {}, {}, {}, {}, false, false});
            zero_reads.push_back(0); zero_all.push_back(true);
            have_prev = false;
        }
        auto& L = lines.back();
        L.cycle_hi = r.cycle;
        if (kind[i] == 1) {
            zero_reads.back()++;
            if (r.data != 0) zero_all.back() = false;
            continue;
        }
        // stream read: same coalescing rule as find_source_lines
        if (have_prev && prev.word_addr == r.wa && prev.data == r.data) continue;
        prev = nmh::RamAccess{r.cycle, r.wa, r.data, false, false, false};
        have_prev = true;
        if (r.wa >= b1 && r.wa < b1 + kBufWords) { L.p1.push_back(r.data); L.p1_off.push_back(r.wa - b1); }
        else                                      { L.p2.push_back(r.data); L.p2_off.push_back(r.wa - b2); }
    }
    for (size_t k = 0; k < lines.size(); ++k) {
        auto& L = lines[k];
        bool stream_empty = L.p1.empty() && L.p2.empty();
        if (stream_empty && zero_reads[k] > 0 && zero_all[k]) {
            L.zero = true;
            L.p1.assign((size_t)zero_reads[k] / 2, 0);
            L.p2.assign((size_t)zero_reads[k] / 2, 0);
            continue;
        }
        bool pure = !L.p1.empty() && zero_reads[k] == 0;
        for (auto d : L.p1) if (d != 0xFFFF) pure = false;
        for (auto d : L.p2) if (d != 0xFFFF) pure = false;
        L.pure_r = pure;
    }
    return lines;
}

FrameLines decode_frame_lines_z(const Capture& cap, int frame_idx, const std::vector<nmh::Span>& outlines) {
    FrameLines fl;
    fl.fc = decode_frame(cap.log, frame_idx);
    if (!fl.fc.found) return fl;
    FrameCop nextfc = frame_idx < 10 ? decode_frame(cap.log, frame_idx + 1) : FrameCop{};
    uint64_t hi = (frame_idx < 10 && nextfc.found) ? nextfc.start_cycle : fl.fc.end_cycle + 3'000'000;
    fl.lines = find_source_lines_z(cap.log, fl.fc.start_cycle, hi);
    if (fl.lines.size() > 200) fl.lines.resize(200);   // window lines 0-199 of this frame
    for (auto& L : fl.lines) {
        LineMap lm = map_source_to_output(outlines, L.cycle_lo, L.cycle_hi);
        std::vector<nmh::Span> spans;
        for (size_t idx : lm.span_idx) spans.push_back(outlines[idx]);
        fl.out_of.push_back(spans);
    }
    return fl;
}

constexpr int kSprPairs = 60;

// What the port shows of sprite 0's and the other sprites' DMA in one frame.
struct SprFetch {
    bool ctl_seen[2] = {false, false};
    uint16_t ctl[2] = {0, 0};
    std::array<int, 120> half;                 // data word of each pair half, -1 if never read
    std::array<int, kSprPairs> line;           // source line the pair's first word was fetched on, -1 if none
    long pair_reads = 0;                       // reads of the 120 data words
    long null_reads = 0;                       // reads of the null sprite
    bool null_zero = true;                     // every null-sprite read returned $0000
    SprFetch() { half.fill(-1); line.fill(-1); }
    int value(int j) const {                   // pair j's colour value: low word = bit 0, high word = bit 1
        if (half[2*j] < 0 || half[2*j+1] < 0) return -1;
        return (half[2*j] != 0 ? 1 : 0) + (half[2*j+1] != 0 ? 2 : 0);
    }
    int pairs_seen() const { int n = 0; for (int j = 0; j < kSprPairs; ++j) if (value(j) >= 0) ++n; return n; }
};

SprFetch decode_sprite_fetch(const Capture& cap, const FrameLines& fl) {
    SprFetch sf;
    if (!fl.fc.found || fl.lines.empty()) return sf;
    uint64_t lo = fl.fc.start_cycle, hi = fl.lines.back().cycle_hi;
    uint32_t s0 = word_addr(kSpr0Struct), ns = word_addr(kNullSpr);
    for (auto& a : cap.log) {
        if (a.is_write || a.cycle < lo || a.cycle > hi) continue;
        if (a.word_addr >= s0 && a.word_addr < s0 + 124) {
            int off = (int)(a.word_addr - s0);
            if (off < 2) { if (!sf.ctl_seen[off]) { sf.ctl_seen[off] = true; sf.ctl[off] = a.data; } }
            else if (off < 122) {
                int h = off - 2;
                ++sf.pair_reads;
                if (sf.half[h] < 0) sf.half[h] = a.data;
                if ((h & 1) == 0 && sf.line[h/2] < 0) {
                    // the line whose own bitplane burst follows this fetch
                    size_t k = 0;
                    while (k < fl.lines.size() && fl.lines[k].cycle_lo <= a.cycle) ++k;
                    sf.line[h/2] = k < fl.lines.size() ? (int)k : -1;
                }
            }
        } else if (a.word_addr >= ns && a.word_addr < ns + 2) {
            ++sf.null_reads;
            if (a.data != 0) sf.null_zero = false;
        }
    }
    return sf;
}

// A graded frame's pixels over the check interval [xL,xR) of one source line.
struct Row { bool ok = false; std::vector<uint16_t> px; };

uint16_t pack12(const nmh::VideoSample& s) { return (uint16_t)((s.r << 8) | (s.g << 4) | s.b); }

Row get_row(const std::vector<nmh::VideoSample>& video, const GradedFrame& gf, int li) {
    Row r;
    if (li < 0 || li >= (int)gf.lines.size() || li >= (int)gf.out_of.size() || gf.out_of[li].empty() ||
        gf.xR <= gf.xL || gf.w <= 0) return r;
    nmh::Span sp = gf.out_of[li].back();
    r.px.resize(gf.xR - gf.xL);
    for (size_t x = gf.xL; x < gf.xR; ++x) {
        auto o = pixel_sample(sp, gf.w, gf.phi, (long)x);
        if (!o) return Row{};
        r.px[x - gf.xL] = pack12(video[*o]);
    }
    r.ok = true;
    return r;
}

// SPECIFICATION.md P6 item 3: the run of a sprite line is the maximal set of
// consecutive visible pixels whose twelve bits differ from those at the same x
// on window lines 31-35 of the same frame.
struct RunInfo { bool shows = false; long xa = 0, xb = 0; long len() const { return xb - xa + 1; } };

struct SprFrame {
    int f = 0;
    bool found = false;
    GradedFrame g;
    SprFetch sf;
    std::vector<char> spacer;                  // per source line: its band change is a spacer line
    std::vector<Row> row;                      // per sprite line j
    std::vector<RunInfo> run;                  // per sprite line j
    std::vector<uint16_t> bg;                  // window lines 31-35, per x - xL
    std::vector<char> bg_ok;
    bool bg_have = false;
    int lines_ok() const { return (int)g.lines.size(); }
    int src_line(int j) const { return sf.line[j]; }
    bool line_graded(int j) const {
        if (!found || j < 0 || j >= (int)row.size()) return false;
        int li = sf.line[j];
        return li >= 0 && li < (int)spacer.size() && !spacer[li] && row[j].ok;
    }
};

SprFrame build_sprite_frame(const Capture& cap, FrameLines&& fl, int f, int w, int phi) {
    SprFrame s;
    s.f = f;
    s.sf = decode_sprite_fetch(cap, fl);
    s.g = finish_graded_frame(cap, std::move(fl), w, phi, 30);
    s.found = s.g.fc.found && s.g.lines.size() == 200;
    if (!s.found) return s;
    s.spacer.assign(s.g.lines.size(), 0);
    for (const Band& b : s.g.fc.bands)
        if (b.moves.size() > 4 && b.line >= 0 && b.line < (int)s.spacer.size()) s.spacer[b.line] = 1;
    s.row.assign(kSprPairs, Row{});
    s.run.assign(kSprPairs, RunInfo{});
    // background rows
    std::vector<Row> bgr;
    for (int li = 31; li <= 35; ++li) bgr.push_back(get_row(cap.video, s.g, li));
    bool all_ok = s.g.xR > s.g.xL;
    for (auto& r : bgr) all_ok = all_ok && r.ok;
    if (all_ok) {
        size_t n = s.g.xR - s.g.xL;
        s.bg.assign(n, 0); s.bg_ok.assign(n, 0);
        for (size_t i = 0; i < n; ++i) {
            bool same = true;
            for (auto& r : bgr) same = same && r.px[i] == bgr[0].px[i];
            s.bg[i] = bgr[0].px[i]; s.bg_ok[i] = same ? 1 : 0;
        }
        s.bg_have = true;
    }
    for (int j = 0; j < kSprPairs; ++j) {
        int li = s.sf.line[j];
        if (li < 0) continue;
        s.row[j] = get_row(cap.video, s.g, li);
        if (!s.row[j].ok || !s.bg_have) continue;
        // longest run of consecutive differing pixels
        long best_lo = -1, best_len = 0, cur_lo = -1, cur_len = 0;
        size_t n = s.row[j].px.size();
        for (size_t i = 0; i < n; ++i) {
            bool d = s.bg_ok[i] && s.row[j].px[i] != s.bg[i];
            if (d) { if (cur_len == 0) cur_lo = (long)i; ++cur_len; if (cur_len > best_len) { best_len = cur_len; best_lo = cur_lo; } }
            else cur_len = 0;
        }
        if (best_len > 0) {
            s.run[j].shows = true;
            s.run[j].xa = (long)s.g.xL + best_lo;
            s.run[j].xb = (long)s.g.xL + best_lo + best_len - 1;
        }
    }
    return s;
}

// Which of a frame's sprite blocks are baseline blocks (SPECIFICATION.md 4.6).
bool is_baseline_block(int f, int blk) {
    if (f == 7) return blk <= 1 || blk >= 15;
    if (f == 10) return blk <= 1 || blk >= 18;
    return true;
}

// Twelve-bit values shown at (sprite value v, position) on a frame's baseline sprite lines.
struct BaseSets { std::array<std::vector<std::set<uint16_t>>, 4> s; };

BaseSets build_base_sets(const SprFrame& sfr) {
    BaseSets b;
    size_t n = sfr.g.xR > sfr.g.xL ? sfr.g.xR - sfr.g.xL : 0;
    for (auto& v : b.s) v.assign(n, std::set<uint16_t>{});
    for (int j = 0; j < kSprPairs; ++j) {
        if (!is_baseline_block(sfr.f, j / 3) || !sfr.line_graded(j)) continue;
        int v = sfr.sf.value(j);
        if (v < 1 || v > 3) continue;
        for (size_t i = 0; i < n; ++i) b.s[v][i].insert(sfr.row[j].px[i]);
    }
    return b;
}

struct RunSummary {
    int lines = 0, with_run = 0, same_ends = 0;
    long xa = 0, xb = 0;
    long len_min = LONG_MAX, len_max = 0;
    bool consistent() const { return with_run > 0 && same_ends == with_run; }
};

RunSummary summarise_runs(const SprFrame& sfr, bool baseline_only) {
    RunSummary r;
    bool first = true;
    for (int j = 0; j < kSprPairs; ++j) {
        if (baseline_only && !is_baseline_block(sfr.f, j / 3)) continue;
        if (!sfr.line_graded(j)) continue;
        ++r.lines;
        if (!sfr.run[j].shows) continue;
        ++r.with_run;
        if (first) { r.xa = sfr.run[j].xa; r.xb = sfr.run[j].xb; first = false; }
        if (sfr.run[j].xa == r.xa && sfr.run[j].xb == r.xb) ++r.same_ends;
        r.len_min = std::min(r.len_min, sfr.run[j].len());
        r.len_max = std::max(r.len_max, sfr.run[j].len());
    }
    return r;
}

// C4 for F7-F10: the header MOVEs that SPECIFICATION.md 4.3 lists (BPLCON0-2,
// DIW, DDF, modulos, bitplane pointers) carry the planned data. The colour
// registers and the sprite pointers are the stimuli under test and are not read here.
bool check_sprite_header(const FrameCop& fc, uint16_t expected_bplcon0) {
    if (!fc.found || fc.header.size() != 61) return false;
    auto get = [&](uint16_t reg) -> long {
        auto it = fc.header.find(reg);
        return it != fc.header.end() ? (long)it->second : -1;
    };
    return get(0x100) == expected_bplcon0 && get(0x102) == 0x0000 && get(0x104) == 0x0024 &&
           get(0x08E) == 0x2C81 && get(0x090) == 0xF4A1 && get(0x092) == 0x003C && get(0x094) == 0x00D4 &&
           get(0x108) == 0x0000 && get(0x10A) == 0x0000 &&
           get(0x0E0) == (long)(kPlane1Buf >> 16) && get(0x0E2) == (long)(kPlane1Buf & 0xFFFF) &&
           get(0x0E4) == (long)(kPlane2Buf >> 16) && get(0x0E6) == (long)(kPlane2Buf & 0xFFFF);
}

std::string hex2(int v) { char b[8]; std::snprintf(b, sizeof b, "%02X", v); return b; }
std::string dec2(int v) { char b[8]; std::snprintf(b, sizeof b, "%02d", v); return b; }

// Prints a graded line, or the NOT GRADED record that replaces it. When the
// comparator ran on the frames that were usable, its verdict is printed as a
// MEASURED line next to the record.
void emit_line(Checks& checks, bool graded, bool ran, bool ok, const std::string& label, const std::string& why) {
    size_t cut = label.find(": ");
    std::string head = cut == std::string::npos ? label : label.substr(0, cut);
    std::string text = cut == std::string::npos ? std::string() : label.substr(cut + 2);
    std::string id = head.substr(0, head.find(' '));
    if (graded) { checks.expect(ok, label); return; }
    checks.record(head + ": NOT GRADED, " + why);
    if (ran) checks.note(id + " comparator without the preconditions (not graded) gives " + std::string(ok ? "PASS" : "FAIL") + ": " + text);
}

// Grades F7-F10 (SPECIFICATION.md P5.U, P6.1, P6.2, P7.2, P7.3 and their
// preconditions C4, C5, C7, C10, C11). `p5l_state`: 0 not graded, 1 PASS, 2 FAIL.
void grade_sprite_frames(const Capture& cap, Checks& checks, const WPhi& wp, const GradedFrame& f6, int p5l_state, bool c1) {
    auto outlines = nmh::find_lines(cap.video);
    SprFrame fr[4];
    for (int i = 0; i < 4; ++i) {
        FrameLines fl = decode_frame_lines_z(cap, 7 + i, outlines);
        fr[i] = build_sprite_frame(cap, std::move(fl), 7 + i, wp.w, wp.phi);
    }
    SprFrame& F7 = fr[0]; SprFrame& F8 = fr[1]; SprFrame& F9 = fr[2]; SprFrame& F10 = fr[3];
    static const char* kName[4] = {"F7", "F8", "F9", "F10"};
    static const uint16_t kBplcon0[4] = {0x2040, 0x2040, 0x2040, 0xA000};
    static const int kBands[4] = {15, 1, 1, 18};
    static const int kMode[4] = {0, 0, 0, 1};

    // ---- C4: F7-F10 ran their own lists in order, each with the planned header
    bool c4 = f6.fc.found;
    for (int i = 0; i < 4; ++i) {
        c4 = c4 && fr[i].found && check_sprite_header(fr[i].g.fc, kBplcon0[i]);
        uint64_t prev = i == 0 ? f6.fc.start_cycle : fr[i-1].g.fc.start_cycle;
        c4 = c4 && fr[i].g.fc.start_cycle > prev;
    }
    checks.expect(c4, "C4-F7F10 precondition: F7-F10 ran their own lists in order after F6, each with the planned "
                      "BPLCON0-2, DIW, DDF, modulo and bitplane-pointer header values");

    // ---- C5: the pixel mapping of lines 0-29 fits uniquely, in the mode the frame runs in
    bool c5 = true; std::string c5s;
    for (int i = 0; i < 4; ++i) {
        bool ok = fr[i].found && fr[i].g.ok && fr[i].g.mode == kMode[i] &&
                  (i == 3 || (fr[i].g.xL == fr[0].g.xL && fr[i].g.xR == fr[0].g.xR));
        c5 = c5 && ok;
        c5s += std::string(" ") + kName[i] + (ok ? "=ok" : "=no") + ":s=" + std::to_string(fr[i].g.s);
    }
    checks.expect(c5, "C5-F7F10 precondition: on lines 0-29 of F7-F9 the SuperHires pixel mapping (i=2x-s) and on F10 the hires "
                      "mapping (i=x-s) fits uniquely, with one check interval for F7-F9;" + c5s +
                      " interval F7-F9 [" + std::to_string(fr[0].g.xL) + "," + std::to_string(fr[0].g.xR) + "), F10 [" +
                      std::to_string(fr[3].g.xL) + "," + std::to_string(fr[3].g.xR) + ")");

    // ---- C7: every planned band change found once, every WAIT at h=$00, after the previous line's
    // last bitplane fetch and, spacer lines excepted, before its own line's first
    bool c7 = true; int bh = 0, ba = 0, bb = 0; std::string c7s;
    for (int i = 0; i < 4; ++i) {
        bool count_ok = fr[i].found && (int)fr[i].g.fc.bands.size() == kBands[i];
        C7Result t = fr[i].found ? check_c7_timing(fr[i].g) : C7Result{};
        c7 = c7 && count_ok && t.ok;
        bh += t.bad_h; ba += t.bad_after; bb += t.bad_before;
        c7s += std::string(" ") + kName[i] + ":bands=" + std::to_string(fr[i].found ? (int)fr[i].g.fc.bands.size() : -1);
    }
    checks.expect(c7, "C7-F7F10 precondition (SPECIFICATION.md 6/C7, block changes included): every planned band change of F7-F10 "
                      "was found once, every WAIT is at h=$00, after the previous line's last bitplane fetch and, spacer lines "
                      "excepted, before its own line's first, bad_h=" + std::to_string(bh) + " bad_after=" + std::to_string(ba) +
                      " bad_before=" + std::to_string(bb) + ";" + c7s);

    // ---- C10: sprite 0 fetches its two control words and 60 data pairs, sprites 1-7 only $0000 control words
    bool c10 = true; std::string c10s;
    for (int i = 0; i < 4; ++i) {
        const SprFetch& sf = fr[i].sf;
        bool ok = fr[i].found && sf.ctl_seen[0] && sf.ctl_seen[1] && sf.ctl[0] == 0x5060 && sf.ctl[1] == 0x8C00 &&
                  sf.pairs_seen() == kSprPairs && sf.pair_reads == 2 * kSprPairs &&
                  sf.null_reads > 0 && sf.null_zero;
        for (int j = 0; ok && j < kSprPairs; ++j) {
            if (sf.value(j) != (j % 3) + 1) ok = false;                   // line j with j mod 3 = 0 has value 1, 1 -> 2, 2 -> 3
            if (j > 0 && sf.line[j] != sf.line[j-1] + 1) ok = false;      // one pair per consecutive line
            if (sf.line[j] < 0) ok = false;
        }
        c10 = c10 && ok;
        c10s += std::string(" ") + kName[i] + ":pairs=" + std::to_string(sf.pairs_seen()) + ",reads=" + std::to_string(sf.pair_reads) +
                ",first_line=" + std::to_string(sf.line[0]) + ",null_reads=" + std::to_string(sf.null_reads);
    }
    checks.expect(c10, "C10 precondition p.105-108: in each of F7-F10 the port shows sprite 0 fetching its two control words and "
                       "60 data pairs, one pair on each of 60 consecutive lines with the planned values, and sprites 1-7 fetching only "
                       "$0000 control words;" + c10s);

    // ---- runs
    RunSummary r7 = summarise_runs(F7, true), r8 = summarise_runs(F8, true), r10 = summarise_runs(F10, true);
    RunSummary r9 = summarise_runs(F9, true);
    checks.note("F7 baseline sprite lines: " + std::to_string(r7.lines) + " graded, " + std::to_string(r7.with_run) +
                " show a run, ends [" + std::to_string(r7.xa) + "," + std::to_string(r7.xb) + "], length " +
                std::to_string(r7.with_run ? r7.len_min : 0) + ".." + std::to_string(r7.len_max) + " hires periods");
    checks.note("F8 sprite lines: " + std::to_string(r8.lines) + " graded, " + std::to_string(r8.with_run) +
                " show a run, ends [" + std::to_string(r8.xa) + "," + std::to_string(r8.xb) + "], length " +
                std::to_string(r8.with_run ? r8.len_min : 0) + ".." + std::to_string(r8.len_max));
    checks.note("F9 sprite lines: " + std::to_string(r9.lines) + " graded, " + std::to_string(r9.with_run) +
                " show a run, ends [" + std::to_string(r9.xa) + "," + std::to_string(r9.xb) + "], length " +
                std::to_string(r9.with_run ? r9.len_min : 0) + ".." + std::to_string(r9.len_max));
    checks.note("F10 baseline sprite lines: " + std::to_string(r10.lines) + " graded, " + std::to_string(r10.with_run) +
                " show a run, ends [" + std::to_string(r10.xa) + "," + std::to_string(r10.xb) + "], length " +
                std::to_string(r10.with_run ? r10.len_min : 0) + ".." + std::to_string(r10.len_max));

    bool have7 = r7.with_run > 0, have10 = r10.with_run > 0;

    // ---- C11: the run of every baseline line that shows one is 32 +- 1 hires periods
    // (p.101: a sprite is 16 pixels wide in every playfield mode; a lores pixel is two hires periods)
    bool c11 = have7;
    for (const RunSummary* r : {&r7, &r8, &r10})
        if (r->with_run > 0 && (r->len_min < 31 || r->len_max > 33)) c11 = false;
    if (have7) {
        checks.expect(c11, "C11 p.101: the run of every baseline sprite line that shows one is 32 +- 1 hires periods long in F7, F8 and F10, "
                           "lengths F7 " + std::to_string(r7.len_min) + ".." + std::to_string(r7.len_max) + ", F8 " +
                           std::to_string(r8.with_run ? r8.len_min : 0) + ".." + std::to_string(r8.len_max) + ", F10 " +
                           std::to_string(r10.with_run ? r10.len_min : 0) + ".." + std::to_string(r10.len_max));
    } else {
        checks.record("C11 p.101: NOT GRADED, F7 shows no run on any baseline sprite line");
    }

    // ---- the run's ends are the same on every baseline line that shows one; F8's are F7's
    bool ends = have7 && r7.consistent() && (r8.with_run == 0 || (r8.consistent() && r8.xa == r7.xa && r8.xb == r7.xb)) &&
                (!have10 || r10.consistent());
    if (have7) {
        checks.expect(ends, "RUN-ENDS precondition (SPECIFICATION.md 7/P6 item 3): the run has the same ends on every baseline sprite line that shows one, "
                            "F7 [" + std::to_string(r7.xa) + "," + std::to_string(r7.xb) + "] on " + std::to_string(r7.same_ends) + " of " +
                            std::to_string(r7.with_run) + " lines, F8 on " + std::to_string(r8.same_ends) + " of " + std::to_string(r8.with_run) +
                            " lines with its own ends and the ends of F7 " + std::string(r8.with_run == 0 || (r8.xa == r7.xa && r8.xb == r7.xb) ? "equal" : "different") +
                            ", F10 [" + std::to_string(r10.xa) + "," + std::to_string(r10.xb) + "] on " +
                            std::to_string(r10.same_ends) + " of " + std::to_string(r10.with_run) + " lines");
    } else {
        checks.record("RUN-ENDS precondition (SPECIFICATION.md 7/P6 item 3): NOT GRADED, F7 shows no run on any baseline sprite line");
    }

    // ---- R1 for F7-F10: every output copy of a graded source line agrees with the others
    long r1_pairs = 0, r1_diff = 0;
    for (int i = 0; i < 4; ++i) if (fr[i].found) {
        R1Result r = check_r1_copies(cap.video, fr[i].g);
        r1_pairs += r.pairs; r1_diff += r.mismatches;
    }
    checks.expect(r1_pairs > 0 && r1_diff == 0,
                  "R1-F7F10 precondition (SPECIFICATION.md 4.7): every output copy of a graded source line of F7-F10 agrees with the others, " +
                  std::to_string(r1_pairs) + " pairs checked, " + std::to_string(r1_diff) + " differing");

    bool bg_all = F7.bg_have && F8.bg_have && F9.bg_have && F10.bg_have;
    bool pre = c1 && c4 && c5 && c7 && c10 && bg_all;
    // The run anchors the inner pixels. Its width (C11) does not gate: P5.U, P6.1 and P6.2 depend on a run being present
    // on each baseline line of the frames they read, with the same ends.
    bool run_pre = pre && ends;
    const std::string why_sp = "a precondition (C1, C4, C5, C7, C10, RUN-ENDS or the background rows of F7-F10) did not hold";

    // ---- F9's lower registers changed: the playfield outside F7's run differs at every compared position
    bool f9pf_ok = false; long f9pf_n = 0, f9pf_same = 0;
    if (pre && have7) {
        for (long x = (long)F7.g.xL; x < (long)F7.g.xR; ++x) {
            if (x >= r7.xa && x <= r7.xb) continue;
            size_t i = (size_t)(x - (long)F7.g.xL);
            if (!F7.bg_ok[i] || !F9.bg_ok[i]) continue;
            ++f9pf_n;
            if (F9.bg[i] == F7.bg[i]) ++f9pf_same;
        }
        f9pf_ok = f9pf_n > 0 && f9pf_same == 0;
        checks.expect(f9pf_ok, "P6.1-F9PF precondition (SPECIFICATION.md 7/P6 item 4): COLOR00-0F changed from Q0 to Q1 between F7 and F9, so "
                               "the playfield outside the run differs at every compared position, positions " + std::to_string(f9pf_n) +
                               ", equal " + std::to_string(f9pf_same));
    } else {
        checks.record("P6.1-F9PF precondition: NOT GRADED, a precondition of the F7-F10 frames did not hold");
    }

    auto letters = [](int v, int comp) { return comp == 0 ? (v & 3) : comp == 1 ? ((v + 1) & 3) : ((v + 2) & 3); };
    auto comp_bits = [](uint16_t p, int c) { return (c == 0 ? (p >> 10) : c == 1 ? (p >> 6) : (p >> 2)) & 3; };
    int p61 = 0, p62shr = 0, p62 = 0, p5u = 0;   // 0 not graded, 1 PASS, 2 FAIL
    auto state_of = [](bool graded, bool ok) { return graded ? (ok ? 1 : 2) : 0; };

    // ---- P6.1 (p.302)
    if (pre && !have7) {
        // SPECIFICATION.md 7/P6 item 3: no baseline line shows a run although C10 holds
        checks.expect(have7, "P6.1 p.302: no baseline sprite line of F7 shows a run although C10 holds, so no inner pixel shows the letters of Color 16+v");
        p61 = 2;
    } else {
        bool ran = run_pre && f9pf_ok, ok = false;
        std::string label = "P6.1 p.302: the inner pixels of a sprite-0 line of value v show the encoded letters of Color 16+v in bits 3-2 of R, G and B "
                            "(F7 under T0, F8 under T0c) and are unchanged by COLOR00-0F (F9 against F7)";
        if (ran) {
            long xa = r7.xa, xb = r7.xb;
            long n7 = 0, bad7 = 0, norun7 = 0, n8 = 0, bad8 = 0, n9 = 0, bad9 = 0;
            BaseSets b7 = build_base_sets(F7);
            for (int j = 0; j < kSprPairs; ++j) {
                int v = F7.sf.value(j);
                if (is_baseline_block(7, j / 3) && F7.line_graded(j)) {
                    if (!F7.run[j].shows) ++norun7;
                    else for (long x = xa + 1; x <= xb - 1; ++x) {
                        uint16_t p = F7.row[j].px[(size_t)(x - (long)F7.g.xL)];
                        for (int c = 0; c < 3; ++c) { ++n7; if (comp_bits(p, c) != letters(v, c)) ++bad7; }
                    }
                }
                if (F8.line_graded(j)) {
                    for (long x = xa + 1; x <= xb - 1; ++x) {
                        uint16_t p = F8.row[j].px[(size_t)(x - (long)F8.g.xL)];
                        for (int c = 0; c < 3; ++c) { ++n8; if (comp_bits(p, c) != 3 - letters(v, c)) ++bad8; }
                    }
                }
                if (F9.line_graded(j) && v >= 1 && v <= 3) {
                    for (long x = xa + 1; x <= xb - 1; ++x) {
                        size_t i = (size_t)(x - (long)F9.g.xL);
                        const std::set<uint16_t>& ref = b7.s[v][i];
                        ++n9;
                        if (!(ref.size() == 1 && *ref.begin() == F9.row[j].px[i])) ++bad9;
                    }
                }
            }
            checks.note("P6.1: F7 baseline " + std::to_string(n7) + " component values, " + std::to_string(bad7) + " differ from the letters of T0, " +
                        std::to_string(norun7) + " baseline lines show no run; F8 " + std::to_string(n8) + " component values, " +
                        std::to_string(bad8) + " differ from the letters of T0c; F9 " + std::to_string(n9) + " pixels, " +
                        std::to_string(bad9) + " differ from the baseline pixel of F7");
            ok = n7 > 0 && n8 > 0 && n9 > 0 && bad7 == 0 && norun7 == 0 && bad8 == 0 && bad9 == 0;
            label += ", pixels F7 " + std::to_string(n7) + " F8 " + std::to_string(n8) + " F9 " + std::to_string(n9);
        }
        emit_line(checks, ran, ran, ok, label, why_sp + " or P6.1-F9PF did not hold");
        p61 = state_of(ran, ok);
    }

    // ---- P6.2 (CH4-appC-context.c6, p.301; p.108, p.103)
    if (run_pre && have10) {
        BaseSets b7 = build_base_sets(F7), b10 = build_base_sets(F10);
        // SuperHires half: some block among the thirteen inversion blocks changes an inner pixel
        long cmp7 = 0, chg7 = 0; std::string per;
        for (int blk = 2; blk <= 14; ++blk) {
            long ch = 0;
            for (int j = 3 * blk; j < 3 * blk + 3; ++j) {
                int v = F7.sf.value(j);
                if (!F7.line_graded(j) || v < 1 || v > 3) continue;
                for (long x = r7.xa + 1; x <= r7.xb - 1; ++x) {
                    size_t i = (size_t)(x - (long)F7.g.xL);
                    if (b7.s[v][i].empty()) continue;
                    ++cmp7;
                    if (!b7.s[v][i].count(F7.row[j].px[i])) { ++chg7; ++ch; }
                }
            }
            per += " block" + std::to_string(blk) + "=" + std::to_string(ch);
        }
        checks.note("P6.2 F7 inner pixels changed per block (blocks 2-14 invert COLOR16, COLOR20-31 in that order):" + per);
        bool shr_ok = cmp7 > 0 && chg7 > 0;
        checks.expect(shr_ok,
                   "P6.2-SHR p.301 (SuperHires half): at least one of the thirteen inversion blocks (COLOR16, COLOR20-31) of F7 changes an inner pixel of sprite 0, "
                   "inner pixels compared " + std::to_string(cmp7) + ", changed " + std::to_string(chg7));
        // Hires half: none of them changes an inner pixel in F10
        long cmp10 = 0, chg10 = 0;
        for (int blk = 2; blk <= 14; ++blk)
            for (int j = 3 * blk; j < 3 * blk + 3; ++j) {
                int v = F10.sf.value(j);
                if (!F10.line_graded(j) || v < 1 || v > 3) continue;
                for (long x = r10.xa + 1; x <= r10.xb - 1; ++x) {
                    size_t i = (size_t)(x - (long)F10.g.xL);
                    if (b10.s[v][i].empty()) continue;
                    ++cmp10;
                    if (!b10.s[v][i].count(F10.row[j].px[i])) ++chg10;
                }
            }
        bool hires_ok = cmp10 > 0 && chg10 == 0;
        checks.expect(hires_ok,
                   "P6.2-HIRES p.108, p.103 (Hires half): none of the thirteen inversion blocks of F10 changes an inner pixel of sprite 0, "
                   "inner pixels compared " + std::to_string(cmp10) + ", changed " + std::to_string(chg10));
        // the Hires comparator is live: the baseline shows register 16+v, blocks 15-17 invert 17, 18, 19 on the lines of value 1, 2, 3
        long cmpb = 0, badb = 0, cmpl = 0, badl = 0, cmpo = 0, bado = 0;
        for (int v = 1; v <= 3; ++v)
            for (long x = r10.xa + 1; x <= r10.xb - 1; ++x) {
                size_t i = (size_t)(x - (long)F10.g.xL);
                ++cmpb;
                const auto& st = b10.s[v][i];
                if (!(st.size() == 1 && *st.begin() == kQ0[v])) ++badb;
            }
        for (int blk = 15; blk <= 17; ++blk)
            for (int j = 3 * blk; j < 3 * blk + 3; ++j) {
                int v = F10.sf.value(j);
                if (!F10.line_graded(j) || v < 1 || v > 3) continue;
                for (long x = r10.xa + 1; x <= r10.xb - 1; ++x) {
                    size_t i = (size_t)(x - (long)F10.g.xL);
                    const auto& st = b10.s[v][i];
                    uint16_t p = F10.row[j].px[i];
                    if (v == blk - 14) {
                        ++cmpl;
                        if (!(st.size() == 1 && p == (uint16_t)(*st.begin() ^ 0x0FFF))) ++badl;
                    } else {
                        ++cmpo;
                        if (!st.count(p)) ++bado;
                    }
                }
            }
        bool live_ok = cmpb > 0 && cmpl > 0 && cmpo > 0 && badb == 0 && badl == 0 && bado == 0;
        checks.expect(live_ok,
                   "P6.2-LIVE p.108 (Hires half): baseline inner pixels of F10 show all twelve bits of register 16+v, blocks 15-17 invert all twelve bits of "
                   "the lines of value 1, 2, 3 and no other line, baseline " + std::to_string(cmpb) + " pixels (" + std::to_string(badb) +
                   " differ), inverted " + std::to_string(cmpl) + " (" + std::to_string(badl) + " differ), other " + std::to_string(cmpo) +
                   " (" + std::to_string(bado) + " changed)");
        bool p62_ok = shr_ok && hires_ok && live_ok;
        checks.expect(p62_ok,
                   "P6.2 CH4-appC-context.c6 p.301, p.108: some inner sprite-0 pixel depends on an upper register other than COLOR17-19 in SuperHires "
                   "and none does in Hires (SuperHires half " + std::string(shr_ok ? "holds" : "fails") + ", Hires half " +
                   std::string(hires_ok ? "holds" : "fails") + ", Hires comparator " + std::string(live_ok ? "live" : "not live") + ")");
        p62shr = shr_ok ? 1 : 2;
        p62 = p62_ok ? 1 : 2;
    } else if (pre && !have7) {
        checks.record("P6.2 CH4-appC-context.c6 p.301: NOT GRADED, no baseline sprite line of F7 shows a run, so there are no inner pixels");
    } else {
        checks.record("P6.2 CH4-appC-context.c6 p.301: NOT GRADED, a precondition (C1, C4, C5, C7, C10, RUN-ENDS) did not hold or F10 shows no run");
    }

    // ---- P5.U: no visible playfield pixel depends on an upper register. F7 inverts COLOR16 and COLOR20-31 one block
    // at a time; F8 writes all sixteen upper registers as T0c, COLOR17-19 included, and runs no inversion.
    if (pre && (!have7 || ends)) {
        BaseSets b7 = build_base_sets(F7);
        long ua = have7 ? r7.xa : 1, ub = have7 ? r7.xb : 0;     // no run: every graded pixel is outside it
        long cmp = 0, chg = 0, blines = 0; std::string per;
        for (int blk = 2; blk <= 14; ++blk) {
            long ch = 0;
            for (int j = 3 * blk; j < 3 * blk + 3; ++j) {
                int v = F7.sf.value(j);
                if (!F7.line_graded(j) || v < 1 || v > 3) continue;
                ++blines;
                for (long x = (long)F7.g.xL; x < (long)F7.g.xR; ++x) {
                    if (x >= ua && x <= ub) continue;
                    size_t i = (size_t)(x - (long)F7.g.xL);
                    if (b7.s[v][i].empty()) continue;
                    ++cmp;
                    if (!b7.s[v][i].count(F7.row[j].px[i])) { ++chg; ++ch; }
                }
            }
            per += " block" + std::to_string(blk) + "=" + std::to_string(ch);
        }
        long cmp8 = 0, chg8 = 0, lines8 = 0;
        for (int j = 0; j < kSprPairs; ++j) {
            int v = F8.sf.value(j);
            if (!F8.line_graded(j) || v < 1 || v > 3) continue;
            ++lines8;
            for (long x = (long)F8.g.xL; x < (long)F8.g.xR; ++x) {
                if (x >= ua && x <= ub) continue;
                size_t i = (size_t)(x - (long)F8.g.xL);
                if (b7.s[v][i].empty()) continue;
                ++cmp8;
                if (!b7.s[v][i].count(F8.row[j].px[i])) ++chg8;
            }
        }
        checks.note("P5.U F7 playfield pixels outside the run changed per block (blocks 2-14 invert COLOR16, COLOR20-31 in that order):" + per);
        bool ok = cmp > 0 && chg == 0 && cmp8 > 0 && chg8 == 0;
        bool graded = !have7 || run_pre;
        emit_line(checks, graded, true, ok,
                  "P5.U p.301/302: no inversion of COLOR16 or COLOR20-31 of F7 and no change of all sixteen upper registers to T0c in F8 changes a visible "
                  "playfield pixel outside the sprite run, F7 lines " + std::to_string(blines) + " pixels compared " + std::to_string(cmp) +
                  " changed " + std::to_string(chg) + ", F8 lines " + std::to_string(lines8) + " pixels compared " + std::to_string(cmp8) +
                  " changed " + std::to_string(chg8), why_sp);
        p5u = state_of(graded, ok);
    } else {
        checks.record("P5.U p.301/302: NOT GRADED, " + why_sp);
    }

    // ---- P7.2, P7.3 and P7.4: claims that name the clauses of two propositions at once
    // (SPECIFICATION.md 7/P7: PASS when both halves pass, FAIL when either fails, not graded when either is not graded)
    auto joint = [&](int a, int b, const std::string& label, const std::string& ng) {
        std::string halves = std::string(a == 1 ? "holds" : a == 2 ? "fails" : "not graded") + " / " + (b == 1 ? "holds" : b == 2 ? "fails" : "not graded");
        bool failed = a == 2 || b == 2;
        if (!failed && (a == 0 || b == 0)) checks.record(ng);
        else checks.expect(a == 1 && b == 1, label + halves);
    };
    joint(p5l_state, p62, "P7.2 APPC-shr.c6 p.301: multiplexing occurs for bitplane and sprite colour information (P5.L and P6.2 together; P5.L / P6.2): ",
          "P7.2 APPC-shr.c6 p.301 (P5.L and P6.2 together): NOT GRADED, P5.L or P6.2 is not graded");
    joint(p5l_state, p5u, "P7.3 APPC-shr.c8 p.301: the bitplane colours are encoded in the sixteen lower registers (P5.L and P5.U together; P5.L / P5.U): ",
          "P7.3 APPC-shr.c8 p.301 (P5.L and P5.U together): NOT GRADED, P5.L or P5.U is not graded");
    joint(p61, p62shr, "P7.4 APPC-shr.c9 p.302: the sprite colours are encoded in the upper sixteen registers by a scheme similar to the bitplane one "
                       "(P6.1 and the SuperHires half of P6.2 together; P6.1 / P6.2-SHR): ",
          "P7.4 APPC-shr.c9 p.302 (P6.1 and the SuperHires half of P6.2 together): NOT GRADED, P6.1 or the SuperHires half of P6.2 is not graded");
}

// ---- per-frame preconditions of F1-F6 and the gate every proposition uses ---
// Each proposition names the frames whose stimulus it grades. It is graded
// only when C1, C2 and R0 hold and every precondition of every one of those
// frames holds; otherwise it is NOT GRADED and says which precondition failed.
struct FramePre {
    bool map = false, r2b = false, c6 = false, c3 = false, c9 = false, c4 = false, c7 = false, c8 = true, r1 = false;
    std::string failed() const {
        std::string s;
        auto add = [&](bool ok, const char* n) { if (!ok) { if (!s.empty()) s += ' '; s += n; } };
        add(map, "C5/R2"); add(r2b, "R2-bottom"); add(c6, "C6"); add(c3, "C3"); add(c9, "C9");
        add(c4, "C4"); add(c7, "C7"); add(c8, "C8"); add(r1, "R1");
        return s;
    }
    bool ok() const { return failed().empty(); }
};

// Grades one capture against every Phase-A proposition and precondition.
// Shared by the normal run (prog_main, default Config) and every mutation
// (a different program and/or Config, same comparator, unchanged): the
// SAME checks.expect() calls run either way: a mutation changes the stimulus
// and never the expected verdict.
void grade_capture(const Capture& cap, Checks& checks, const std::string& mutate) {
    checks.expect(cap.loaded && cap.released, "fixture: program loaded and CPU released from reset");
    checks.expect(cap.fixture.has_value() && *cap.fixture == 0xBEEF,
                  "fixture: the program-stored constant $BEEF was read back before anything else is graded");
    checks.expect(cap.done.has_value(), "C2 precondition: the program's DONE mark was written (p.223/337/339 setup completed)");

    bool c1 = false;
    if (cap.vposr && cap.deniseid) {
        int chip = (*cap.vposr >> 8) & 0x7F;
        c1 = (chip == 0x20 || chip == 0x30) && (*cap.deniseid & 0xFF) == 0xFC;
        if (chip == 0x20) checks.note("VPOSR chip id $20: PAL enhanced Agnus (p.299)");
        else if (chip == 0x30) checks.note("VPOSR chip id $30: NTSC enhanced Agnus (p.299)");
    }
    checks.expect(c1, "C1 precondition: the Enhanced Chip Set is present (VPOSR chip id $20/$30, DENISEID low byte $FC, p.299)");

    if (!cap.done) {
        checks.record("run did not reach DONE; no proposition graded");
        return;
    }

    // C8's own contrast rewrites the pattern section, so C3 must
    // compare against what THIS run's program actually wrote, not always
    // the baseline generator.
    auto pat = (mutate == "C8") ? build_pattern_vc8() : build_pattern();

    // ---- graded frames F1-F6. F7-F10 are graded by grade_sprite_frames at
    // the end of this function.
    FrameLines fl1 = decode_frame_lines(cap, 1, kPlane1Buf, kPlane2Buf);
    FrameLines fl2 = decode_frame_lines(cap, 2, kPlane1Buf, kPlane2Buf);
    FrameLines fl3 = decode_frame_lines(cap, 3, kPlane1Buf, kPlane2Buf);
    FrameLines fl4 = decode_frame_lines(cap, 4, kPlane1Buf, kPlane2Buf);
    FrameLines fl5 = decode_frame_lines(cap, 5, kPlane1Buf, kPlane2Buf);
    FrameLines fl6 = decode_frame_lines(cap, 6, kPlane1Buf, kPlane2Buf);

    // R0: w and phi, measured once (F5's own pattern lines: hires mode,
    // easiest to get unambiguous single-sample runs from) and reused for
    // every frame (SPECIFICATION.md 5, R0: "the same w and phi serve every
    // frame, because a hires period lasts the same time in every mode").
    WPhi wp = measure_global_w_phi(cap.video, fl5);
    if (!wp.ok) wp = measure_global_w_phi(cap.video, fl1);
    checks.expect(wp.ok, "R0 precondition: a unique hires-period sample width w and phase phi exist");
    if (wp.ok) checks.note("R0: w=" + std::to_string(wp.w) + " phi=" + std::to_string(wp.phi));

    GradedFrame f1 = finish_graded_frame(cap, std::move(fl1), wp.w, wp.phi);
    GradedFrame f2 = finish_graded_frame(cap, std::move(fl2), wp.w, wp.phi);
    GradedFrame f3 = finish_graded_frame(cap, std::move(fl3), wp.w, wp.phi);
    GradedFrame f4 = finish_graded_frame(cap, std::move(fl4), wp.w, wp.phi);
    GradedFrame f5 = finish_graded_frame(cap, std::move(fl5), wp.w, wp.phi);
    GradedFrame f6 = finish_graded_frame(cap, std::move(fl6), wp.w, wp.phi);
    GradedFrame* gfr[7] = {nullptr, &f1, &f2, &f3, &f4, &f5, &f6};
    static const int kExpMode[7]  = {0, 0, 0, 0, 0, 1, 2};
    static const size_t kExpBands[7] = {0, 12, 81, 81, 34, 33, 17};
    FramePre pre[7];

    // R2's own check requires the SuperHires mapping specifically
    // (mode == 0), not just "a mapping fits uniquely" -- the hires mapping
    // fits just as uniquely when F2-F4 run in hires (NMH_MUTATE=V4B/V4D/V5),
    // which the plain f2.ok/f3.ok/f4.ok test could not tell apart.
    for (int f = 1; f <= 6; ++f) pre[f].map = gfr[f]->ok && gfr[f]->mode == kExpMode[f];
    checks.expect(pre[1].map, "C5/R2 precondition: F1's SuperHires pixel mapping (i=2x-s) fits uniquely");
    checks.expect(pre[5].map, "R2 precondition: F5's hires pixel mapping (i=x-s) fits uniquely");
    checks.expect(pre[6].map, "R2 precondition: F6's lores pixel mapping fits uniquely");
    checks.expect(pre[2].map && pre[3].map && pre[4].map,
                  "R2 precondition: F2-F4's SuperHires pixel mapping (i=2x-s) fits uniquely, and is the SuperHires mapping");

    checks.note("R0/R1/R2 F1: ok=" + std::to_string(f1.ok) + " mode=" + std::to_string(f1.mode) +
                " s=" + std::to_string(f1.s) + " xL=" + std::to_string(f1.xL) + " xR=" + std::to_string(f1.xR));

    // "in F1-F6 the same s fits the bottom Q0 lines" (5/R2) --
    // re-searched independently from the top-line s already found. The
    // closing restore band is the last band of a frame, F4's the one before
    // its null band.
    for (int f = 1; f <= 6; ++f) {
        int nb = (int)gfr[f]->fc.bands.size();
        int close_idx = f == 4 ? nb - 2 : nb - 1;
        pre[f].r2b = pre[f].map && close_idx >= 0 && check_bottom_s(cap.video, *gfr[f], close_idx);
    }
    bool r2_bottom = true;
    for (int f = 1; f <= 6; ++f) r2_bottom = r2_bottom && pre[f].r2b;
    checks.expect(r2_bottom, "R2 precondition: the same s that fits the top Q0 lines also fits the bottom Q0 lines, in F1-F6");

    // C6: every pure-R line has a run of 256-576 hires periods with identical
    // ends, asserted per frame.
    static const char* kFrameNames6[7] = {"", "F1","F2","F3","F4","F5","F6"};
    for (int f = 1; f <= 6; ++f) {
        C6Result c6r = check_c6(cap.video, *gfr[f]);
        pre[f].c6 = c6r.ok;
        checks.expect(c6r.ok,
            std::string("C6 precondition ") + kFrameNames6[f] +
            ": the check interval has identical ends on every pure-R line (256-576 hires periods wide), " +
            std::to_string(c6r.lines_checked) + " lines checked, " + std::to_string(c6r.lines_mismatch) + " mismatched");
    }

    // C3: every graded line of F1-F6, independent of C5 (a fetch-continuity
    // precondition does not depend on whether the pixel mapping also fit).
    std::vector<uint64_t> stray = stray_read_cycles(cap.log);
    long c3_bad_sum = 0;
    bool c3 = true;
    for (int f = 1; f <= 6; ++f) {
        long bad = 0;
        pre[f].c3 = precondition_c3(*gfr[f], pat.first, pat.second, stray, &bad);
        c3_bad_sum += bad;
        c3 = c3 && pre[f].c3;
    }
    checks.expect(c3, "C3 precondition: F1-F6's graded lines fetch >=16 consecutive words per plane in both planes, data as written, "
                      "no other bitplane address inside a line's fetches, bad lines=" + std::to_string(c3_bad_sum));

    // C4: the header MOVEs carry the planned register VALUES
    // (BPLCON0-2, DIW, DDF, modulos, bitplane pointers, SPR0PT-SPR7PT, both
    // palettes), not just the planned count; F0 (the lead-in copy of F1,
    // never graded) also ran.
    FrameCop fc0 = decode_frame(cap.log, 0);
    static const uint16_t kHdrBplcon0[7] = {0x2040, 0x2040, 0x2040, 0x2040, 0x2040, 0xA000, 0x2000};
    bool c4 = fc0.found && fc0.header.size() == 61 && check_frame_header(fc0, 0x2040, kQ0);
    uint64_t prev_start = fc0.start_cycle;
    for (int f = 1; f <= 6; ++f) {
        const FrameCop& fc = gfr[f]->fc;
        pre[f].c4 = fc.found && fc.header.size() == 61 && check_frame_header(fc, kHdrBplcon0[f], kQ0) && fc.start_cycle > prev_start;
        prev_start = fc.start_cycle;
        c4 = c4 && pre[f].c4;
    }
    checks.expect(c4, "C4 precondition: F0-F6 ran their own lists in order, each with the planned header register values");

    // C9: every sprite DMA read of F1-F6 is a null-sprite word
    FrameCop fc7 = decode_frame(cap.log, 7);
    bool c9 = true; std::string c9s;
    for (int f = 1; f <= 6; ++f) {
        const FrameCop& fc = gfr[f]->fc;
        uint64_t hi = f < 6 ? gfr[f + 1]->fc.start_cycle : (fc7.found ? fc7.start_cycle : fc.end_cycle + 3'000'000);
        SprDma sd = fc.found ? sprite_dma_reads(cap.log, fc.start_cycle, hi) : SprDma{};
        pre[f].c9 = fc.found && precondition_c9(sd);
        c9 = c9 && pre[f].c9;
        c9s += std::string(" ") + kFrameNames6[f] + ":null_reads=" + std::to_string(sd.null_reads) + ",stray_reads=" + std::to_string(sd.stray);
    }
    checks.expect(c9, "C9 precondition p.291: in each of F1-F6 every sprite DMA read is a word of the null sprite and returns $0000;" + c9s);

    // C7 band count (a coarse proxy kept alongside the timing check below):
    // a MOVE landing behind the line's own fetch would either merge into
    // the next band or go missing, changing this count.
    bool c7_count = true;
    for (int f = 1; f <= 6; ++f) c7_count = c7_count && gfr[f]->fc.bands.size() == kExpBands[f];
    checks.expect(c7_count, "C7 precondition (band count): every planned band change was found, once each, in F1-F6");

    // C7 timing: SPECIFICATION.md 6/C7's own condition -- every
    // band's WAIT at h=$00, after the previous line's last bitplane fetch
    // and, spacer lines excepted, before its own line's first.
    bool c7_timing = true; int c7_bad_h=0,c7_bad_after=0,c7_bad_before=0;
    for (int f = 1; f <= 6; ++f) {
        C7Result t = check_c7_timing(*gfr[f]);
        pre[f].c7 = gfr[f]->fc.bands.size() == kExpBands[f] && t.ok;
        c7_timing = c7_timing && t.ok;
        c7_bad_h += t.bad_h; c7_bad_after += t.bad_after; c7_bad_before += t.bad_before;
    }
    checks.expect(c7_timing,
        "C7 precondition (SPECIFICATION.md 6/C7, timing): every band's WAIT is at h=$00, after the previous line's "
        "last bitplane fetch and, spacer lines excepted, before its own line's first, in F1-F6 -- bad_h=" +
        std::to_string(c7_bad_h) + " bad_after=" + std::to_string(c7_bad_after) + " bad_before=" + std::to_string(c7_bad_before));

    // C8 (F1 Q0 diversity): F1's own Q0 lines show enough neighbour triples
    std::vector<int> f1_top_tail;
    for (int i = 0; i < 24 && i < (int)f1.lines.size(); ++i) f1_top_tail.push_back(i);
    if (f1.fc.bands.size() == 12) {
        int tail_lo = f1.fc.bands[11].line + 1, tail_hi = 199;
        for (int i = tail_lo; i <= tail_hi && i < (int)f1.lines.size(); ++i) f1_top_tail.push_back(i);
    }
    Baseline bl_q0_f1 = f1.ok ? build_baseline(cap.video, f1.lines, f1.out_of, f1_top_tail, f1.mode, f1.s, f1.w, f1.phi, (long)f1.xL, (long)f1.xR)
                               : Baseline{};
    bool c8_div = bl_q0_f1.value.size() >= 32;
    checks.expect(c8_div,
                  "C8 precondition (F1 Q0 diversity): F1's own Q0 lines show at least half of the 64 possible neighbour triples, " +
                  std::to_string(bl_q0_f1.value.size()) + " of 64");

    // C8: SPECIFICATION.md 6/C8's own population -- all 16
    // ordered pairs, both as (v[i],v[i+1]) and (v[i-1],v[i]), in every band
    // of F1-F4 (F1's own eleven content bands, and F2-F4's 192 perturbation
    // bands).
    int c8_bands_ok = 0, c8_bands_total = 0;
    bool c8_frame[5] = {true, true, true, true, true};
    for (size_t bi = 0; bi < kF1Bands.size(); ++bi) {
        ++c8_bands_total;
        bool ok = f1.ok && bi < f1.fc.bands.size() && band_pair_coverage(f1, f1.fc.bands[bi].line + 1, f1.fc.bands[bi].line + 12);
        if (ok) ++c8_bands_ok; else c8_frame[1] = false;
    }
    for (int gi = 0; gi < 192; ++gi) {
        GradedFrame* gf; int band_idx; int fi;
        if (gi < 80) { gf=&f2; band_idx=gi; fi=2; } else if (gi < 160) { gf=&f3; band_idx=gi-80; fi=3; } else { gf=&f4; band_idx=gi-160; fi=4; }
        ++c8_bands_total;
        bool ok = gf->ok && band_idx < (int)gf->fc.bands.size() &&
                  band_pair_coverage(*gf, gf->fc.bands[band_idx].line, gf->fc.bands[band_idx].line + 1);
        if (ok) ++c8_bands_ok; else c8_frame[fi] = false;
    }
    checks.expect(c8_bands_total > 0 && c8_bands_ok == c8_bands_total,
        "C8 precondition (SPECIFICATION.md 6/C8): every ordered pair occurs both as (v[i],v[i+1]) and (v[i-1],v[i]) "
        "in every band of F1-F4, " + std::to_string(c8_bands_ok) + " of " + std::to_string(c8_bands_total) + " bands");
    for (int f = 1; f <= 4; ++f) pre[f].c8 = c8_frame[f] && (f != 1 || c8_div);

    // R1: every output copy of a graded source line agrees with
    // the others (SPECIFICATION.md 4.7) -- checked directly, not assumed
    // from using only the last copy throughout.
    long r1_pairs = 0, r1_mismatch = 0;
    for (int f = 1; f <= 6; ++f) {
        R1Result r = check_r1_copies(cap.video, *gfr[f]);
        pre[f].r1 = r.ok;
        r1_pairs += r.pairs; r1_mismatch += r.mismatches;
    }
    checks.expect(r1_pairs > 0 && r1_mismatch == 0,
        "R1 precondition (SPECIFICATION.md 4.7): every output copy of a graded source line agrees with the others, " +
        std::to_string(r1_pairs) + " pairs checked, " + std::to_string(r1_mismatch) + " differing");

    // The gate: C1, C2 and R0, then every precondition of every frame named.
    auto gate = [&](std::initializer_list<int> frames, std::string* why) -> bool {
        std::vector<std::string> parts;
        if (!c1) parts.push_back("C1");
        if (!wp.ok) parts.push_back("R0");
        for (int f : frames) {
            std::string fs = pre[f].failed();
            if (!fs.empty()) parts.push_back(std::string(kFrameNames6[f]) + ": " + fs);
        }
        if (why) {
            std::string w;
            for (size_t i = 0; i < parts.size(); ++i) { if (i) w += "; "; w += parts[i]; }
            *why = w;
        }
        return parts.empty();
    };
    auto shr_pop = [&](const GradedFrame& g) { return g.ok && g.mode == 0; };   // frames whose pixels a comparator may read

    // ==== P1, P2 (SuperHires key), P3 (coarseness) ==========================
    P1State p1s; P2State p2s;
    // The diversity source id names the PALETTE, not the
    // stimulus instance: F1's own Q0 top/bottom and F2-F4's Q0 top/bottom
    // are all literally the same sixteen register values, so they must
    // collapse to one source ("Q0"), not count as six distinct ones -- only
    // a genuinely different palette (Q1, Q2, ...) should add diversity.
    if (shr_pop(f1)) {
        grade_range_p1p2(cap.video, f1, kQ0, 0, 23, &p1s, &p2s, "Q0");
        for (size_t bi = 0; bi < kF1Bands.size() && bi < f1.fc.bands.size(); ++bi) {
            const Band& b = f1.fc.bands[bi];
            grade_range_p1p2(cap.video, f1, *kF1Bands[bi].second, b.line+1, b.line+12, &p1s, &p2s,
                              kF1Bands[bi].first);
        }
        if (f1.fc.bands.size() == 12) {
            int tail_lo = f1.fc.bands[11].line + 1;
            grade_range_p1p2(cap.video, f1, kQ0, tail_lo, 199, &p1s, &p2s, "Q0");
        }
    }
    for (GradedFrame* gf : {&f2, &f3, &f4}) {
        if (!shr_pop(*gf)) continue;
        grade_range_p1p2(cap.video, *gf, kQ0, 0, 23, &p1s, &p2s, "Q0");
        int last_band = (int)gf->fc.bands.size() - 1;
        if (last_band >= 0) grade_range_p1p2(cap.video, *gf, kQ0, gf->fc.bands[last_band].line+1, 199, &p1s, &p2s, "Q0");
    }

    std::string why_p1;
    bool g_p1 = gate({1, 2, 3, 4}, &why_p1);
    static const char* kColorName[4] = {"0","1","2","3"};
    static const char* kCompName[3] = {"R","G","B"};
    static const char* kP1Id[12] = {
        "P1.0R","P1.0G","P1.0B","P1.1R","P1.1G","P1.1B","P1.2R","P1.2G","P1.2B","P1.3R","P1.3G","P1.3B",
    };
    for (int idx = 0; idx < 12; ++idx) {
        std::string label = std::string(kP1Id[idx]) + " p.301: Color " + kColorName[idx/3] + " " +
            (idx%3==0?"red":idx%3==1?"green":"blue") + " shows the key's letters in bits 3-2, checked " +
            std::to_string(p1s.total[idx]) + " graded pixels";
        emit_line(checks, g_p1, false, p1s.total[idx] > 0 && p1s.fail[idx] == 0, label, why_p1);
    }

    // ---- P2's palette-diversity precondition (SPECIFICATION.md 7/P2 item 4):
    // "For every (c, X), at least one group must contain pixels from two
    // palettes that differ in some other Color's letters of X." Printed before
    // P2, whose PASS it conditions: it is what tells a P2 that exercised the
    // claim from one that saw a single palette.
    std::map<std::string, const std::vector<uint16_t>*> pal_by_name;
    pal_by_name["Q0"] = &kQ0;
    for (auto& kb : kF1Bands) pal_by_name[kb.first] = kb.second;
    static const int kShift[3] = {10, 6, 2};
    int div_ok = 0;
    for (int comp = 0; comp < 3; ++comp) {
        for (int c = 0; c < 4; ++c) {
            bool found = false;
            for (auto& kv : p2s.sources[comp]) {
                if (kv.first.first != c || found) continue;
                const std::set<std::string>& names = kv.second;
                for (auto a = names.begin(); a != names.end() && !found; ++a)
                    for (auto b = std::next(a); b != names.end() && !found; ++b)
                        for (int o = 0; o < 4 && !found; ++o) {
                            if (o == c) continue;
                            int la = ((*pal_by_name[*a])[o] >> kShift[comp]) & 3, lb = ((*pal_by_name[*b])[o] >> kShift[comp]) & 3;
                            if (la != lb) found = true;
                        }
            }
            if (found) ++div_ok;
        }
    }
    std::string div_detail;
    for (int comp = 0; comp < 3; ++comp) {
        int groups_diverse = 0;
        for (auto& kv : p2s.sources[comp]) if (kv.second.size() >= 2) ++groups_diverse;
        div_detail += std::string(" ") + kCompName[comp] + ": " + std::to_string(groups_diverse) + " of " +
                      std::to_string(p2s.bykey[comp].size()) + " groups have >=2 contributing palettes;";
    }
    bool p2_div = div_ok == 12;
    checks.expect(p2_div,
                  "P2-DIVERSITY precondition (SPECIFICATION.md 7/P2 item 4): for every (Color c, component X) some group of pixels of value c with equal "
                  "letters of X is populated from two palettes that differ in some other Color's letters of X, " +
                  std::to_string(div_ok) + " of 12 (c, X);" + div_detail);

    static const char* kP2Id[3] = {
        "P2.R APPC-shr.c3 CH4-appC-context.c3", "P2.G APPC-shr.c4 CH4-appC-context.c4",
        "P2.B APPC-shr.c5 CH4-appC-context.c5",
    };
    std::string why_p2 = why_p1;
    if (!p2_div) why_p2 += std::string(why_p2.empty() ? "" : "; ") + "P2-DIVERSITY";
    for (int comp = 0; comp < 3; ++comp) {
        std::string label = std::string(kP2Id[comp]) + " p.301: " + kCompName[comp] +
            " depends only on its two encoded letters, " + std::to_string(p2s.total[comp]) + " graded pixels, " +
            std::to_string(p2s.bykey[comp].size()) + " groups";
        emit_line(checks, g_p1 && p2_div, true, p2s.total[comp] > 0 && p2s.bad[comp].empty(), label, why_p2);
    }

    // ---- P3: SuperHires coarser than Hires and than Lores ----------------
    // SPECIFICATION.md 7/P3 item 2: SuperHires is read over every band of F1
    // -- the top and bottom Q0 lines and every Q1-Q5/Z/O/S0-S3 band -- the
    // whole population F1's own stimulus plan covers, not the S0-S3 bands alone.
    P3Comp p3shr;
    if (shr_pop(f1)) {
        collect_p3(cap.video, f1, 0, 23, 1, p3shr);
        for (size_t bi = 0; bi < kF1Bands.size() && bi < f1.fc.bands.size(); ++bi) {
            const Band& b = f1.fc.bands[bi];
            collect_p3(cap.video, f1, b.line+1, b.line+12, 1, p3shr);
        }
        if (f1.fc.bands.size() == 12) {
            int tail_lo = f1.fc.bands[11].line + 1;
            collect_p3(cap.video, f1, tail_lo, 199, 1, p3shr);
        }
    }
    P3Comp p3hires;
    if (f5.ok && f5.mode == 1) for (int k = 0; k < 16; ++k) if (k < (int)f5.fc.bands.size())
        collect_p3(cap.video, f5, f5.fc.bands[k].line, f5.fc.bands[k].line+1, 1, p3hires);
    P3Comp p3lores;
    if (f6.ok && f6.mode == 2) for (int k = 0; k < 16; ++k) if (k < (int)f6.fc.bands.size())
        collect_p3(cap.video, f6, f6.fc.bands[k].line, f6.fc.bands[k].line+1, 1, p3lores);

    // "coarser granularity of color control" (p.301): the SuperHires mode
    // shows fewer distinct values of each component than the other mode does.
    bool p3h_ok = p3shr.total > 0 && p3hires.total > 0, p3l_ok = p3shr.total > 0 && p3lores.total > 0;
    for (int c = 0; c < 3; ++c) {
        if (p3shr.vals[c].size() >= p3hires.vals[c].size()) p3h_ok = false;
        if (p3shr.vals[c].size() >= p3lores.vals[c].size()) p3l_ok = false;
    }
    std::string shr_cnt = "SuperHires R=" + std::to_string(p3shr.vals[0].size()) + " G=" + std::to_string(p3shr.vals[1].size()) +
                          " B=" + std::to_string(p3shr.vals[2].size());
    std::string hi_cnt = "Hires R=" + std::to_string(p3hires.vals[0].size()) + " G=" + std::to_string(p3hires.vals[1].size()) +
                         " B=" + std::to_string(p3hires.vals[2].size());
    std::string lo_cnt = "Lores R=" + std::to_string(p3lores.vals[0].size()) + " G=" + std::to_string(p3lores.vals[1].size()) +
                         " B=" + std::to_string(p3lores.vals[2].size());
    checks.note("P3 SuperHires distinct values per component: R=" + std::to_string(p3shr.vals[0].size()) +
                " G=" + std::to_string(p3shr.vals[1].size()) + " B=" + std::to_string(p3shr.vals[2].size()));
    checks.note("P3 hires distinct values per component: R=" + std::to_string(p3hires.vals[0].size()) +
                " G=" + std::to_string(p3hires.vals[1].size()) + " B=" + std::to_string(p3hires.vals[2].size()));
    checks.note("P3 lores distinct values per component: R=" + std::to_string(p3lores.vals[0].size()) +
                " G=" + std::to_string(p3lores.vals[1].size()) + " B=" + std::to_string(p3lores.vals[2].size()));

    std::string why_p3h, why_p3l;
    bool g_p3h = gate({1, 5}, &why_p3h), g_p3l = gate({1, 6}, &why_p3l);
    emit_line(checks, g_p3h, true, p3h_ok,
              "P3.H CH4-appC-context.c1 p.301: SuperHires has a coarser granularity of colour control than Hires, fewer distinct values of each "
              "component (F1 against F5; " + shr_cnt + ", " + hi_cnt + ")", why_p3h);
    emit_line(checks, g_p3l, true, p3l_ok,
              "P3.L CH4-appC-context.c2 p.301: SuperHires has a coarser granularity of colour control than Lores, fewer distinct values of each "
              "component (F1 against F6; " + shr_cnt + ", " + lo_cnt + ")", why_p3l);
    // a joint claim: PASS when both halves pass, FAIL when either fails, not graded when either is not graded
    auto state_of = [](bool graded, bool ok) { return graded ? (ok ? 1 : 2) : 0; };
    auto joint = [&](int a, int b, const std::string& label, const std::string& ng) {
        std::string halves = std::string(a == 1 ? "holds" : a == 2 ? "fails" : "not graded") + " / " + (b == 1 ? "holds" : b == 2 ? "fails" : "not graded");
        bool failed = a == 2 || b == 2;
        if (!failed && (a == 0 || b == 0)) checks.record(ng);
        else checks.expect(a == 1 && b == 1, label + halves);
    };
    joint(state_of(g_p3h, p3h_ok), state_of(g_p3l, p3l_ok),
          "P7.1 APPC-shr.c1 p.301: SuperHires has a coarser granularity of colour control than either Hires or Lores (P3.H and P3.L together; P3.H / P3.L): ",
          "P7.1 APPC-shr.c1 p.301 (P3.H and P3.L together): NOT GRADED, P3.H or P3.L is not graded");

    // ---- P4: 192 per-bit propositions (F2, F3, F4) -------------------------
    auto plan = perturbation_plan_all();
    Baseline bl2, bl3, bl4;
    Baseline* bls[5] = {nullptr, nullptr, &bl2, &bl3, &bl4};
    for (int f = 2; f <= 4; ++f) {
        GradedFrame& g = *gfr[f];
        if (!shr_pop(g)) continue;
        std::vector<int> idxs; for (int i=0;i<24 && i<(int)g.lines.size();++i) idxs.push_back(i);
        int close_idx = f == 4 ? 32 : 80;
        if ((int)g.fc.bands.size() > close_idx) { int lo=g.fc.bands[close_idx].line+1; for (int i=lo;i<=199 && i<(int)g.lines.size();++i) idxs.push_back(i); }
        *bls[f] = build_baseline(cap.video, g.lines, g.out_of, idxs, g.mode, g.s, g.w, g.phi, (long)g.xL, (long)g.xR);
    }

    // ---- V4c, the null case: each frame's own closing restore
    // band (no new inversion) is predicted to change no graded pixel; it
    // must come out NOT OBSERVED -- SPECIFICATION.md 7/P4's own contrast.
    // Printed before the 192 propositions it conditions.
    bool v4c_ok[5] = {false, false, false, false, false};
    auto grade_null_band = [&](int f, int band_idx, const char* fname) {
        GradedFrame& gf = *gfr[f]; Baseline& bl = *bls[f];
        std::string why;
        bool gp = gate({f}, &why);
        bool have_band = gf.ok && band_idx >= 0 && band_idx < (int)gf.fc.bands.size();
        long checked = 0, diffs = 0;
        if (have_band) {
            const Band& band = gf.fc.bands[band_idx];
            for (int li = band.line; li <= band.line + 1 && li < (int)gf.lines.size(); ++li) {
                if (gf.out_of[li].empty()) continue;
                nmh::Span sp = gf.out_of[li].back();
                for (size_t x = gf.xL; x < gf.xR; ++x) {
                    auto oidx = pixel_sample(sp, gf.w, gf.phi, (long)x);
                    if (!oidx) continue;
                    int i = pixel_index(gf.mode, (int)x, gf.s);
                    int v = pixel_value(gf.lines[li], i);
                    int vm1 = pixel_value(gf.lines[li], i - 1), vp1 = pixel_value(gf.lines[li], i + 1);
                    if (v < 0) continue;
                    auto it = bl.value.find(Triple{vm1, v, vp1});
                    if (it == bl.value.end()) continue;
                    ++checked;
                    if (!eqfull(it->second, full12(cap.video[*oidx]))) ++diffs;
                }
            }
        }
        if (!have_band) { checks.record(std::string("V4c precondition (") + fname + "): closing restore band not decoded, NOT GRADED"); return; }
        std::string label = std::string("V4c precondition (SPECIFICATION.md 7/P4 contrast, ") + fname +
                       "'s closing restore band): the null case changes no graded pixel (NOT OBSERVED), checked=" +
                       std::to_string(checked) + " diffs=" + std::to_string(diffs);
        emit_line(checks, gp, true, checked > 0 && diffs == 0, label, why);
        v4c_ok[f] = gp && checked > 0 && diffs == 0;
    };
    grade_null_band(2, 80, "F2");
    grade_null_band(3, 80, "F3");
    grade_null_band(4, 33, "F4");

    long p5l_shr_changed = 0, p5l_shr_any = 0, p5l_bands = 0, p5l_pixels = 0, p5l_bands_graded = 0;
    long not_observable = 0, not_graded_p4 = 0;
    // P1K reads the per-band changes of the graded bands, filled as the 192-bit loop runs.
    std::vector<P4Verdict> p4pv(192);
    std::vector<char> p4have(192, 0);
    std::string why_f[5];
    bool g_p4f[5] = {false, false, false, false, false};
    for (int f = 2; f <= 4; ++f) {
        bool gp = gate({f}, &why_f[f]);
        if (gp && !v4c_ok[f]) why_f[f] = "V4c";
        g_p4f[f] = gp && v4c_ok[f];
    }
    for (int gi = 0; gi < 192; ++gi) {
        int n = plan[gi].first, b = plan[gi].second;
        int f = gi < 80 ? 2 : gi < 160 ? 3 : 4;
        int band_idx = gi < 80 ? gi : gi < 160 ? gi - 80 : gi - 160;
        GradedFrame* gf = gfr[f]; Baseline* bl = bls[f];
        std::string idtok = "P4.n" + std::to_string(n) + "b" + std::to_string(b) + " APPC-bpltab.r" + hex2(n) + "b" + dec2(b);
        if (!shr_pop(*gf) || band_idx >= (int)gf->fc.bands.size()) {
            checks.record(idtok + " p.301: bit " + std::to_string(b) + " of COLOR" + hex2(n) + " -- NOT GRADED (" +
                          (g_p4f[f] ? std::string(kFrameNames6[f]) + ": band not decoded" : why_f[f]) + ")");
            ++not_graded_p4;
            continue;
        }
        P4Verdict pv = grade_p4_band(cap.video, *gf, *bl, band_idx, n, b);
        if (n >= 4) {
            // P5.L (SuperHires): any change of any bit counts (SPECIFICATION.md 7/P5 item 4),
            // whether or not P4 can attribute it to the band's own bit
            ++p5l_bands;
            if (pv.checked > 0) ++p5l_bands_graded;
            p5l_pixels += pv.checked;
            p5l_shr_changed += pv.changed;
            p5l_shr_any += pv.any_changed;
        }
        if (!g_p4f[f]) {
            checks.record(idtok + " p.301: bit " + std::to_string(b) + " of COLOR" + hex2(n) + " -- NOT GRADED (" + why_f[f] + ")");
            ++not_graded_p4;
            continue;
        }
        p4pv[gi] = pv;
        p4have[gi] = 1;
        std::string label = idtok + " p.301: bit " + std::to_string(b) + " of COLOR" + hex2(n) +
            " changes only its own place, checked=" + std::to_string(pv.checked) +
            " changed=" + std::to_string(pv.changed) + " excluded=" + std::to_string(pv.excluded);
        // SPECIFICATION.md 7/P4 item 4: FAIL if any graded pixel differs
        // anywhere other than the allowed places; OBSERVED if the allowed
        // bit itself moved; PASS when OBSERVED and not FAIL; NOT OBSERVABLE
        // when NEITHER. A failing, unobserved bit is a FAIL.
        if (!pv.observed && !pv.fail) {
            checks.record(label + " -- NOT OBSERVABLE (L1)");
            ++not_observable;
            continue;
        }
        bool pass = pv.observed && !pv.fail;
        checks.expect(pass, label);
    }
    checks.note("P4: " + std::to_string(not_observable) + " of 192 bits not observable (L1), " + std::to_string(not_graded_p4) + " not graded");

    // ---- P5.L (SuperHires half): some register 04-0F changes a pixel ------
    // SPECIFICATION.md 7/P5 item 4: "Any change counts, whether P4 grades it
    // PASS or FAIL" -- graded on the any-bit count over every n>=4 band,
    // including the bands P4 reports NOT OBSERVABLE. A PASS needs one changed
    // pixel; a FAIL ("no register changes a pixel") needs all 144 bands to
    // have shown graded pixels.
    std::string why_p5s;
    bool g_p5s = gate({2, 3, 4}, &why_p5s);
    if (g_p5s && !(v4c_ok[2] && v4c_ok[3] && v4c_ok[4])) { g_p5s = false; why_p5s = "V4c"; }
    bool p5s_full = p5l_bands == 144 && p5l_bands_graded == 144;
    std::string p5s_label = "P5.L CH4-appC-context.c7 p.301/94 (SuperHires half): a register outside COLOR00-03 changes a visible playfield pixel, any bit, "
                  "bands graded " + std::to_string(p5l_bands_graded) + " of 144, pixels checked " + std::to_string(p5l_pixels) +
                  ", changed=" + std::to_string(p5l_shr_any) + " (allowed-bit-only count " + std::to_string(p5l_shr_changed) + ")";
    bool p5s_pass = p5l_shr_any > 0;
    if (g_p5s && !p5s_pass && !p5s_full) {
        g_p5s = false;
        why_p5s = "the population is incomplete, so \"no register changes a pixel\" cannot be established";
    }
    emit_line(checks, g_p5s, true, p5s_pass, p5s_label, why_p5s);
    int p5s_state = state_of(g_p5s, p5s_pass);

    // ---- P5.L (Hires half): F5's registers 04-0F change nothing, 00-03
    // invert exactly the value-n pixels in all twelve bits --------------
    std::string why_p5h;
    bool g_p5h = gate({5}, &why_p5h);
    int p5h_state = 0;
    if (f5.ok && f5.mode == 1 && f5.fc.bands.size() == 33) {
        std::vector<int> idxs; for (int i=0;i<24 && i<(int)f5.lines.size();++i) idxs.push_back(i);
        int lo = f5.fc.bands[32].line+1; for (int i=lo;i<=199 && i<(int)f5.lines.size();++i) idxs.push_back(i);
        Baseline bl5 = build_baseline(cap.video, f5.lines, f5.out_of, idxs, f5.mode, f5.s, f5.w, f5.phi,
                                       (long)f5.xL, (long)f5.xR);
        bool hires_high_clean = true, hires_low_ok = true;
        long total_checked = 0; // a real population guard, not a discarded count
        for (int n = 0; n < 16; ++n) {
            int band_idx = 16 + n; // second half of F5's bands
            const Band& band = f5.fc.bands[band_idx];
            for (int li = band.line; li <= band.line+1 && li < (int)f5.lines.size(); ++li) {
                if (f5.out_of[li].empty()) continue;
                nmh::Span sp = f5.out_of[li].back();
                for (size_t x = f5.xL; x < f5.xR; ++x) {
                    auto oidx = pixel_sample(sp, f5.w, f5.phi, (long)x);
                    if (!oidx) continue;
                    size_t idx = *oidx;
                    int i = pixel_index(1,(int)x,f5.s);
                    int v = pixel_value(f5.lines[li], i);
                    int vm1 = pixel_value(f5.lines[li], i-1), vp1 = pixel_value(f5.lines[li], i+1);
                    if (v<0) continue;
                    auto it = bl5.value.find(Triple{vm1,v,vp1});
                    if (it==bl5.value.end()) continue;
                    Full12 base=it->second, obs=full12(cap.video[idx]);
                    total_checked++;
                    bool differs = !eqfull(base,obs);
                    if (n>=4 && differs) hires_high_clean = false;
                    if (n<4) {
                        if (v==n) { if (!(obs.r==(base.r^0xF)&&obs.g==(base.g^0xF)&&obs.b==(base.b^0xF))) hires_low_ok=false; }
                        else if (differs) hires_low_ok=false;
                    }
                }
            }
        }
        bool hi_ok = total_checked > 0 && hires_high_clean, lo_ok = total_checked > 0 && hires_low_ok;
        emit_line(checks, g_p5h, true, hi_ok,
                  "P5.L p.301/94 (Hires half): registers COLOR04-0F change no graded pixel in Hires mode, checked=" +
                  std::to_string(total_checked), why_p5h);
        emit_line(checks, g_p5h, true, lo_ok,
                  "P5.L p.301/94 (Hires half): registers COLOR00-03 invert exactly the value-n pixels, all twelve bits, checked=" +
                  std::to_string(total_checked), why_p5h);
        p5h_state = g_p5h ? ((hi_ok && lo_ok) ? 1 : 2) : 0;
    } else {
        checks.record("P5.L (Hires half): NOT GRADED, " + (g_p5h ? std::string("F5's register-invert bands not decoded as planned") : why_p5h));
    }
    // the joint P5.L state: FAIL when either half fails, PASS when both pass
    int p5l_state = (p5s_state == 2 || p5h_state == 2) ? 2 : (p5s_state == 1 && p5h_state == 1) ? 1 : 0;

    // ---- P1K: the two dashes of the key and every other register bit, per (Color c, component X)
    // (SPECIFICATION.md 7/P1 item 8). Over all 192 perturbation bands of F2-F4, the bands whose bit is not one of the two
    // letters of (c, X) leave all four bits of X of every value-c pixel as the baseline shows it, and the bands whose bit is
    // one of the two letters change, among bits 3-2 of X, only the output bit of that letter; each letter is seen to move
    // its own output bit in some band. The lines carry no claim id: P1J joins them with P1.
    int p1k_state[4][3] = {};   // 0 not graded, 1 PASS, 2 FAIL, 3 NOT OBSERVABLE
    {
        bool gate_p1k = g_p4f[2] && g_p4f[3] && g_p4f[4];
        std::string why_p1k;
        for (int f = 2; f <= 4; ++f)
            if (!g_p4f[f]) why_p1k += std::string(why_p1k.empty() ? "" : "; ") + kFrameNames6[f] + ": " + why_f[f];
        bool all_have = true;
        for (int gi = 0; gi < 192; ++gi) all_have = all_have && p4have[gi];
        for (int c = 0; c < 4; ++c) {
            for (int comp = 0; comp < 3; ++comp) {
                std::string id = "P1K." + std::to_string(c) + kCompName[comp];
                std::string what = std::string("Color ") + std::to_string(c) + " " + (comp == 0 ? "red" : comp == 1 ? "green" : "blue");
                long off_bands = 0, off_px_bands = 0, off_changed = 0, off_px = 0;
                long key_bands = 0, key_px_bands = 0, key_sib = 0;
                bool up_obs = false, lo_obs = false;
                if (gate_p1k && all_have) {
                    for (int gi = 0; gi < 192; ++gi) {
                        BitTarget t = bit_target(plan[gi].first, plan[gi].second);
                        const P4Verdict& pv = p4pv[gi];
                        int d = pv.xdiff[c][comp];
                        if (!((int)t.x == comp && t.c == c)) {
                            ++off_bands;
                            if (pv.vcnt[c] > 0) { ++off_px_bands; off_px += pv.vcnt[c]; }
                            if (d != 0) ++off_changed;
                        } else {
                            ++key_bands;
                            if (pv.vcnt[c] > 0) ++key_px_bands;
                            int own = t.upper ? 8 : 4, sib = t.upper ? 4 : 8;
                            if (d & own) { if (t.upper) up_obs = true; else lo_obs = true; }
                            if (d & sib) ++key_sib;
                        }
                    }
                }
                if (!gate_p1k) { checks.record(id + " p.301: " + what + " -- NOT GRADED (" + why_p1k + ")"); continue; }
                bool complete = all_have && key_bands > 0 && off_bands > 0 && off_px_bands == off_bands && key_px_bands == key_bands;
                if (!complete) {
                    checks.record(id + " p.301: " + what + " -- NOT GRADED (a band of the 192 was not graded or shows no value-" + std::to_string(c) + " pixel)");
                    continue;
                }
                std::string label = id + " p.301: no register bit other than the two letters of " + what + " changes any of the four bits of " +
                    kCompName[comp] + " on a value-" + std::to_string(c) + " pixel, " + std::to_string(off_changed) + " of " + std::to_string(off_bands) +
                    " other bands change it (" + std::to_string(off_px) + " pixels checked), and each of the two letters changes only its own one of bits 3-2, " +
                    std::to_string(key_sib) + " of " + std::to_string(key_bands) + " letter bands change the other, upper letter " +
                    (up_obs ? "observed" : "not observed") + ", lower letter " + (lo_obs ? "observed" : "not observed");
                bool violation = off_changed > 0 || key_sib > 0;
                bool p1k_ok = !violation && up_obs && lo_obs;
                if (!violation && !p1k_ok) {
                    checks.record(label + " -- NOT OBSERVABLE (L1)");
                    p1k_state[c][comp] = 3;
                    continue;
                }
                checks.expect(p1k_ok, label);
                p1k_state[c][comp] = p1k_ok ? 1 : 2;
            }
        }
    }

    // ---- P1J: the full key claim, letters and dashes together (SPECIFICATION.md 7/P1 item 8):
    // P1.cX, the two letters, and P1K.cX, the two dashes and every other register bit. PASS when both pass, FAIL when
    // either fails, not graded when either is not graded and neither fails.
    {
        static const char* kSuffix[3] = {"r","g","b"};
        for (int c = 0; c < 4; ++c) {
            for (int comp = 0; comp < 3; ++comp) {
                int p1idx = c*3+comp;
                int a = g_p1 ? ((p1s.total[p1idx] > 0 && p1s.fail[p1idx] == 0) ? 1 : 2) : 0;
                int bk = p1k_state[c][comp] == 3 ? 0 : p1k_state[c][comp];
                std::string cx = std::to_string(c) + kCompName[comp];
                std::string head = "P1J." + cx + " APPC-bplkeykey.c" + std::to_string(c) + kSuffix[comp] + " p.301: ";
                std::string why = !g_p1 ? "P1 preconditions: " + why_p1
                                        : p1k_state[c][comp] == 3 ? std::string("P1K NOT OBSERVABLE, L1") : std::string("P1K not graded");
                joint(a, bk,
                      head + "P1." + cx + " (the two letters) and P1K." + cx +
                      " (the two dashes: no other register bit carries this component of the Color) together; P1." + cx + " / P1K." + cx + ": ",
                      head + "P1." + cx + " and P1K." + cx + " together -- NOT GRADED (" + why + ")");
            }
        }
    }

    // ---- F7-F10: P5.U, P6.1, P6.2 and, with P5.L, P7.2, P7.3 and P7.4
    grade_sprite_frames(cap, checks, wp, f6, p5l_state, c1);
}

void run_normal(const std::string& dir, Checks& checks) {
    nmh::Config cfg;
    Capture cap = run_case(dir, "prog_main", cfg);
    grade_capture(cap, checks, "");
}

// Every mutation's program (or Config) and the comparator it is run
// through -- always grade_capture(), unchanged. C1 selects OCS via a
// harness Config change (no program symbol exists for it); every other
// mutation selects a prog_<id>.bin built from its own MUT_<ID> wrapper.
void run_mutation(const std::string& dir, const std::string& mut, Checks& checks) {
    nmh::Config cfg; // ECS default; only C1 changes it
    std::string prog = "prog_" + [&]{ std::string s=mut; for (auto& c: s) c=(char)std::tolower((unsigned char)c); return s; }();
    if (mut == "C1") { cfg.chipset_config = 0x00; prog = "prog_main"; }
    else if (mut == "C5") prog = "prog_c5";
    else if (mut == "V5") prog = "prog_v4b"; // P5's SuperHires contrast reuses P4's V4b build (same data)
    Capture cap = run_case(dir, prog, cfg);
    checks.record("NMH_MUTATE=" + mut + " ran " + prog + ".bin, chipset_config=" +
                   std::to_string((int)cfg.chipset_config));
    grade_capture(cap, checks, mut);
}

} // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    const char* env = std::getenv("NMH_MUTATE");
    std::string mutate = env ? env : "";
    std::printf("NMH_MUTATE: %s\n", mutate.empty() ? "(none)" : mutate.c_str());

    Checks checks;

    if (std::getenv("NMH_DIAG")) {
    nmh::Config cfg;
    Capture c = run_case(dir, "prog_main", cfg);
    std::printf("DIAG loaded=%d released=%d done=%s log=%zu video=%zu\n",
                c.loaded, c.released, c.done ? std::to_string(*c.done).c_str() : "(none)",
                c.log.size(), c.video.size());
    if (c.fixture) std::printf("DIAG fixture=%04X vposr=%04X deniseid=%04X\n",
                                *c.fixture, c.vposr ? *c.vposr : 0, c.deniseid ? *c.deniseid : 0);
    for (int f = 0; f <= 10; ++f) {
        FrameCop fc = decode_frame(c.log, f);
        std::printf("DIAG frame %2d found=%d header_regs=%zu bands=%zu start=%llu end=%llu\n",
                    f, fc.found, fc.header.size(), fc.bands.size(),
                    (unsigned long long)fc.start_cycle, (unsigned long long)fc.end_cycle);
        if (fc.found && f <= 6) {
            for (size_t bi = 0; bi < fc.bands.size() && bi < 3; ++bi)
                std::printf("  band[%zu] line=%d beam=%02X moves=%zu\n", bi, fc.bands[bi].line,
                            fc.bands[bi].beam, fc.bands[bi].moves.size());
        }
    }
    // source lines in F1's own window (start..end cycle of F1's Copper pass)
    {
        FrameCop f1 = decode_frame(c.log, 1);
        FrameCop f2 = decode_frame(c.log, 2);
        uint64_t lo = f1.start_cycle, hi = f2.found ? f2.start_cycle : f1.end_cycle + 2000000;
        auto lines = find_source_lines(c.log, lo, hi, kPlane1Buf, kPlane2Buf);
        std::printf("DIAG F1 source lines: %zu\n", lines.size());
        for (size_t i = 0; i < lines.size() && i < 5; ++i)
            std::printf("  line[%zu] cyc=[%llu,%llu] p1=%zu p2=%zu pure_r=%d\n", i,
                        (unsigned long long)lines[i].cycle_lo, (unsigned long long)lines[i].cycle_hi,
                        lines[i].p1.size(), lines[i].p2.size(), lines[i].pure_r);
        auto outlines = nmh::find_lines(c.video);
        std::printf("DIAG output lines in video: %zu\n", outlines.size());
        auto frames = nmh::find_frames(c.video);
        std::printf("DIAG output frames in video: %zu\n", frames.size());
    }
    // extra: DMACON writes, CIA writes, program-range fetch spread, buffer writes
    {
        uint32_t dmacon_wa = word_addr(0xDFF096);
        int dmacon_writes = 0; uint16_t last_dmacon = 0; uint64_t last_dmacon_cycle=0;
        int cia_writes = 0;
        int zerobuf_writes = 0, plane1_writes = 0, plane2_writes = 0, spr0_writes=0;
        uint32_t prog_lo = word_addr(0x080400), prog_hi = word_addr(0x080400) + 4000;
        uint32_t min_pc = 0xFFFFFFFF, max_pc = 0;
        uint64_t last_cycle = 0;
        for (auto& a : c.log) {
            if (a.cycle > last_cycle) last_cycle = a.cycle;
            if (a.is_write && a.word_addr == dmacon_wa) { ++dmacon_writes; last_dmacon = a.data; last_dmacon_cycle=a.cycle; }
            if (a.is_write && a.word_addr >= word_addr(0xBFE000) && a.word_addr < word_addr(0xBFE400)) ++cia_writes;
            if (a.is_write && a.word_addr >= word_addr(kZeroBuf) && a.word_addr < word_addr(kZeroBuf)+kBufWords) ++zerobuf_writes;
            if (a.is_write && a.word_addr >= word_addr(kPlane1Buf) && a.word_addr < word_addr(kPlane1Buf)+kBufWords) ++plane1_writes;
            if (a.is_write && a.word_addr >= word_addr(kPlane2Buf) && a.word_addr < word_addr(kPlane2Buf)+kBufWords) ++plane2_writes;
            if (a.is_write && a.word_addr >= word_addr(kSpr0Struct) && a.word_addr < word_addr(kSpr0Struct)+124) ++spr0_writes;
            if (!a.is_write && a.word_addr >= prog_lo && a.word_addr < prog_hi) {
                if (a.word_addr < min_pc) min_pc = a.word_addr;
                if (a.word_addr > max_pc) max_pc = a.word_addr;
            }
        }
        std::printf("DIAG dmacon_writes=%d last=%04X@%llu cia_writes=%d zerobuf_w=%d plane1_w=%d plane2_w=%d spr0_w=%d\n",
                    dmacon_writes, last_dmacon, (unsigned long long)last_dmacon_cycle, cia_writes,
                    zerobuf_writes, plane1_writes, plane2_writes, spr0_writes);
        std::printf("DIAG program fetch range: min=%08X max=%08X (org=%08X) last_log_cycle=%llu\n",
                    min_pc*2, max_pc*2, 0x080400, (unsigned long long)last_cycle);
        // last 20 program-range fetches, in order
        std::vector<uint32_t> tail;
        for (auto& a : c.log) if (!a.is_write && a.word_addr>=prog_lo && a.word_addr<prog_hi) tail.push_back(a.word_addr*2);
        std::printf("DIAG last program fetches:");
        for (size_t i = tail.size() > 20 ? tail.size()-20 : 0; i < tail.size(); ++i) std::printf(" %06X", tail[i]);
        std::printf("\n");
    }
    {
        FrameLines fl5 = decode_frame_lines(c, 5, kPlane1Buf, kPlane2Buf);
        std::printf("DIAG F5 found=%d lines=%zu\n", fl5.fc.found, fl5.lines.size());
        for (size_t li = 8; li < 14 && li < fl5.lines.size(); ++li) {
            std::printf("DIAG F5 line[%zu] pure_r=%d p1=%zu out_spans=%zu\n", li, fl5.lines[li].pure_r,
                        fl5.lines[li].p1.size(), fl5.out_of[li].size());
            if (fl5.out_of[li].empty()) continue;
            nmh::Span sp = fl5.out_of[li].back();
            std::printf("  span=[%zu,%zu) width=%zu\n", sp.begin, sp.end, sp.end-sp.begin);
            // run-length list across the WHOLE span
            std::vector<std::pair<int,int>> runs; // (value-as-int, length)
            size_t i = sp.begin;
            while (i < sp.end) {
                size_t j = i+1;
                while (j < sp.end && eqfull(full12(c.video[j]), full12(c.video[i]))) ++j;
                runs.push_back({(c.video[i].r<<8)|(c.video[i].g<<4)|c.video[i].b, (int)(j-i)});
                i = j;
            }
            std::printf("  %zu runs, lengths:", runs.size());
            for (size_t k = 0; k < runs.size() && k < 40; ++k) std::printf(" %03X:%d", runs[k].first, runs[k].second);
            std::printf("\n");
        }
    }
    {
        FrameLines fl1 = decode_frame_lines(c, 1, kPlane1Buf, kPlane2Buf);
        std::printf("DIAG F1 lines=%zu\n", fl1.lines.size());
        for (size_t li : {size_t(0), size_t(10), size_t(11)}) {
            if (li >= fl1.lines.size()) continue;
            std::printf("DIAG F1 line[%zu] pure_r=%d p1=%zu out_spans=%zu\n", li, fl1.lines[li].pure_r,
                        fl1.lines[li].p1.size(), fl1.out_of[li].size());
            if (fl1.out_of[li].empty()) continue;
            nmh::Span sp = fl1.out_of[li].back();
            Run12 r = find_constant_run(c.video, sp, 0);
            std::printf("  span=[%zu,%zu) run.ok=%d run=[%zu,%zu) len=%zd\n", sp.begin, sp.end, r.ok,
                        r.lo, r.hi, r.ok ? (ptrdiff_t)(r.hi-r.lo) : -1);
            std::vector<std::pair<int,int>> runs;
            size_t ii = sp.begin;
            while (ii < sp.end) {
                size_t jj = ii+1;
                while (jj < sp.end && eq2(top2(c.video[jj]), top2(c.video[ii]))) ++jj;
                runs.push_back({(top2(c.video[ii]).r<<4)|(top2(c.video[ii]).g<<2)|top2(c.video[ii]).b, (int)(jj-ii)});
                ii = jj;
            }
            std::printf("  %zu top2-runs:", runs.size());
            for (size_t k=0;k<runs.size()&&k<50;++k) std::printf(" %02X:%d", runs[k].first, runs[k].second);
            std::printf("\n");
        }
    }
    checks.record("diagnostic run, no assertions graded");
    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
    }

    static const std::set<std::string> kKnownMuts = {
        "V1","V2","V3A","V3B","V3C","V4A","V4B","V4D","V5","V5B",
        "C1","C3","C4","C5","C6","C7","C8","C9","FIX",
        "V5C","V6A","V6B","V6C","V6D","V6E","V6L","V6Q","V6S",
        "C4B","C4C","C7B","C10","C11","C11G","RUNENDS","VLIVE","VLIVES","VP1JG","C4S",
        "R2M","R2B","C6B","C7C","C3P","V4C","V5D","V5E","R0","P2G","VKEYA",
    };
    if (mutate.empty()) {
        run_normal(dir, checks);
    } else if (kKnownMuts.count(mutate)) {
        run_mutation(dir, mutate, checks);
    } else {
        checks.record("unrecognised or not-yet-implemented NMH_MUTATE value: " + mutate);
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
