// test_ch1_cpu_system.cpp -- ch1-cpu-system. See SPECIFICATION.md for every
// assertion's proposition, program, observed value and expected value; this
// file implements what that document commits to.
//
// NMH_MUTATE selects a mutated program (or configuration) in place of its
// normal counterpart, one assertion at a time; a plain `make test` runs none.
// Every program is assembled to an image and, separately, to a vasm listing;
// the test uses a listing only after checking that it reproduces the image
// byte for byte.
//
// Baseline (no mutation), measured: fx68k 67 passed/0 failed, TG68K 170
// passed/3 failed. The three TG68K FAIL lines are results, not defects: P4
// (CLR.B and CLR.W on the 68020), P4.L (CLR.L on the 68020) and P6
// (D(68020 mode) > D(68000 mode)); each is graded through its page sentence
// (p.13, p.263) and stays red (SPECIFICATION.md sections 6 and 8). The claims
// c52, c56, c64, c65 and c67 (opus/sol:1-guidelines) are RECORDED with their
// reasons and none of them prints a FAIL. C5 is graded in every run: it
// proves that ports()->cpu_config shows the requested value, not the mode the
// core runs in.
//
// Each record below is one stimulus mutation, replayed on both cores with the
// built binaries (NMH_MUTATE=<selector> ./Vnanomig <package directory>). The
// comparator and the expected value stay as they are; the program or the
// configuration changes. The label after MUTATION is the leading token of the
// printed line; several lines share a token (a program's fixture lines share
// it with the program's relation, and C1 to C4 are printed in every
// program's run) and each record says which line it reddens. "TG68K" and
// "fx68k" give the printed totals and the red labels; "not applicable" marks
// a mutation whose image or run the core never loads, so its totals equal the
// baseline.
//
// MUTATION P1 stimulus red NMH_MUTATE=NODONE-P1: the fixture's DONE clause: the program stops in place after the S_END write without writing DONE (p1_mutnodone.bin); the DONE clause of every P1 run's fixture line is red and the relations that depend on it are not evaluated; TG68K 168 passed/4 failed (P1; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P1).
// MUTATION P1 stimulus red NMH_MUTATE=PEEK-P1: the fixture's readback clause: S_START is overwritten with $0000 after the S_END write, the CPU write of $0001 stays in the log (p1_mutpeek.bin); the peek_word readback line of every P1 run is red; TG68K 168 passed/4 failed (P1; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P1).
// MUTATION C1 stimulus red NMH_MUTATE=C1-P1: an extra INTENA write ($C000) follows step 2 (p1_mutc1.bin); TG68K 168 passed/4 failed (C1; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (C1).
// MUTATION C2 stimulus red NMH_MUTATE=C2-P1: an extra DMACON write ($8240) follows step 4 (p1_mutc2.bin); TG68K 168 passed/4 failed (C2; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (C2).
// MUTATION C3 stimulus red NMH_MUTATE=C3-P1: the S_END write is made twice (p1_mutc3.bin); TG68K 168 passed/4 failed (C3; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (C3).
// MUTATION C4 stimulus red NMH_MUTATE=C4-P1: the S_START write is made twice (p1_mutc4.bin); TG68K 168 passed/4 failed (C4; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (C4).
// MUTATION P2 stimulus red NMH_MUTATE=NODONE-P2: the fixture's DONE clause: the program stops in place after the S_END write without writing DONE (p2_mutnodone.bin); the DONE clause of every P2 run's fixture line is red and the relations that depend on it are not evaluated; TG68K 168 passed/4 failed (P2; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P2).
// MUTATION P2 stimulus red NMH_MUTATE=PEEK-P2: the fixture's readback clause: S_START is overwritten with $0000 after the S_END write, the CPU write of $0001 stays in the log (p2_mutpeek.bin); the peek_word readback line of every P2 run is red; TG68K 168 passed/4 failed (P2; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P2).
// MUTATION C1 stimulus red NMH_MUTATE=C1-P2: an extra INTENA write ($C000) follows step 2 (p2_mutc1.bin); TG68K 168 passed/4 failed (C1; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (C1).
// MUTATION C2 stimulus red NMH_MUTATE=C2-P2: an extra DMACON write ($8240) follows step 4 (p2_mutc2.bin); TG68K 168 passed/4 failed (C2; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (C2).
// MUTATION C3 stimulus red NMH_MUTATE=C3F-P1: the S_FAIL clause: a branch to the failure path is made just before the S_END write, so S_FAIL is written and S_END still once (p1_mutc3f.bin); TG68K 168 passed/4 failed (C3; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (C3).
// MUTATION C3 stimulus red NMH_MUTATE=C3-P2: the S_END write is made twice (p2_mutc3.bin); TG68K 168 passed/4 failed (C3; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (C3).
// MUTATION C4 stimulus red NMH_MUTATE=C4-P2: the S_START write is made twice (p2_mutc4.bin); TG68K 168 passed/4 failed (C4; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (C4).
// MUTATION P3 stimulus red NMH_MUTATE=NODONE-P3: the fixture's DONE clause: the program stops in place after the S_END write without writing DONE (p3_mutnodone.bin); the DONE clause of every P3 run's fixture line is red and the relations that depend on it are not evaluated; TG68K 164 passed/6 failed (P3 x3; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P3).
// MUTATION P3 stimulus red NMH_MUTATE=PEEK-P3: the fixture's readback clause: S_START is overwritten with $0000 after the S_END write, the CPU write of $0001 stays in the log (p3_mutpeek.bin); the peek_word readback line of every P3 run is red; TG68K 164 passed/6 failed (P3 x3; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P3).
// MUTATION C1 stimulus red NMH_MUTATE=C1-P3: an extra INTENA write ($C000) follows step 2 (p3_mutc1.bin); TG68K 164 passed/6 failed (C1 x3; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (C1).
// MUTATION C2 stimulus red NMH_MUTATE=C2-P3: an extra DMACON write ($8240) follows step 4 (p3_mutc2.bin); TG68K 164 passed/6 failed (C2 x3; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (C2).
// MUTATION C3 stimulus red NMH_MUTATE=C3-P3: the S_END write is made twice (p3_mutc3.bin); TG68K 164 passed/6 failed (C3 x3; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (C3).
// MUTATION C4 stimulus red NMH_MUTATE=C4-P3: the S_START write is made twice (p3_mutc4.bin); TG68K 164 passed/6 failed (C4 x3; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (C4).
// MUTATION P5 stimulus red NMH_MUTATE=NODONE-P5: the fixture's DONE clause: the program stops in place after the S_END write without writing DONE (p5_mutnodone.bin (68000 image) and p5_vbr_mutnodone.bin (VBR image)); the fixture line of every P5 run goes red, its P5.a and P5.c lines and the relations that depend on them are not evaluated; TG68K 158 passed/6 failed (P5 x3; baseline reds P4, P4.L, P6), fx68k 64 passed/1 failed (P5).
// MUTATION P5 stimulus red NMH_MUTATE=PEEK-P5: the fixture's readback clause: S_START is overwritten with $0000 after the S_END write, the CPU write of $0001 stays in the log (p5_mutpeek.bin (68000 image) and p5_vbr_mutpeek.bin (VBR image)); the peek_word readback line of every P5 run is red; TG68K 164 passed/6 failed (P5 x3; baseline reds P4, P4.L, P6), fx68k 66 passed/1 failed (P5).
// MUTATION C1 stimulus red NMH_MUTATE=C1-P5: an extra INTENA write ($C000) follows step 2 (p5_mutc1.bin (68000 image) and p5_vbr_mutc1.bin (VBR image)); TG68K 164 passed/6 failed (C1 x3; baseline reds P4, P4.L, P6), fx68k 66 passed/1 failed (C1).
// MUTATION C2 stimulus red NMH_MUTATE=C2-P5: an extra DMACON write ($8240) follows step 4 (p5_mutc2.bin (68000 image) and p5_vbr_mutc2.bin (VBR image)); TG68K 164 passed/6 failed (C2 x3; baseline reds P4, P4.L, P6), fx68k 66 passed/1 failed (C2).
// MUTATION C3 stimulus red NMH_MUTATE=C3-P5: the S_END write is made twice (p5_mutc3.bin (68000 image) and p5_vbr_mutc3.bin (VBR image)); TG68K 164 passed/6 failed (C3 x3; baseline reds P4, P4.L, P6), fx68k 66 passed/1 failed (C3).
// MUTATION C4 stimulus red NMH_MUTATE=C4-P5: the S_START write is made twice (p5_mutc4.bin (68000 image) and p5_vbr_mutc4.bin (VBR image)); TG68K 164 passed/6 failed (C4 x3; baseline reds P4, P4.L, P6), fx68k 66 passed/1 failed (C4).
// MUTATION P6 stimulus red NMH_MUTATE=NODONE-P6: the fixture's DONE clause: the program stops in place after the S_END write without writing DONE (p6_mutnodone.bin); the DONE clause of every P6 run's fixture line is red and the relations that depend on it are not evaluated; TG68K 167 passed/4 failed (P6; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P6).
// MUTATION P6 stimulus red NMH_MUTATE=PEEK-P6: the fixture's readback clause: S_START is overwritten with $0000 after the S_END write, the CPU write of $0001 stays in the log (p6_mutpeek.bin); the peek_word readback line of every P6 run is red; TG68K 167 passed/4 failed (P6; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P6).
// MUTATION C1 stimulus red NMH_MUTATE=C1-P6: an extra INTENA write ($C000) follows step 2 (p6_mutc1.bin); TG68K 167 passed/4 failed (C1 x2; baseline reds P4, P4.L; not red: P6), fx68k 65 passed/1 failed (C1).
// MUTATION C2 stimulus red NMH_MUTATE=C2-P6: an extra DMACON write ($8240) follows step 4 (p6_mutc2.bin); TG68K 167 passed/4 failed (C2 x2; baseline reds P4, P4.L; not red: P6), fx68k 65 passed/1 failed (C2).
// MUTATION C3 stimulus red NMH_MUTATE=C3-P6: the S_END write is made twice (p6_mutc3.bin); TG68K 167 passed/4 failed (C3 x2; baseline reds P4, P4.L; not red: P6), fx68k 65 passed/1 failed (C3).
// MUTATION C4 stimulus red NMH_MUTATE=C4-P6: the S_START write is made twice (p6_mutc4.bin); TG68K 167 passed/4 failed (C4 x2; baseline reds P4, P4.L; not red: P6), fx68k 65 passed/1 failed (C4).
// MUTATION C4 stimulus red NMH_MUTATE=C4B-P1: S_START is written after case 1's S_A write instead of in the setup (p1_mutc4b.bin): still written once with $0001, but not before every window; TG68K 168 passed/4 failed (C4; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (C4).
// MUTATION C4 stimulus red NMH_MUTATE=C4E-P1: S_END is written once, before case 21's S_B write, and the ending's own S_END write is left out (p1_mutc4e.bin): still written once with $E0D0, so C3 holds, but not after every window; TG68K 168 passed/4 failed (C4; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (C4).
// MUTATION P1 stimulus red NMH_MUTATE=P1-MOVE: every CLR.x replaced by MOVE.x #0 (p1_move.bin), and the program's table marker names MOVE: MOVE.x #0 makes a single write and no read, the page's own contrast, so P1's read-then-write relation goes red through the unchanged comparator in all 21 cases; TG68K 169 passed/4 failed (P1; baseline reds P4, P4.L, P6), fx68k 66 passed/1 failed (P1).
// MUTATION P1.a stimulus red NMH_MUTATE=P1A: case 5's S_A store left out (p1_muta.bin); TG68K 168 passed/4 failed (P1.a; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P1.a).
// MUTATION P1.a stimulus red NMH_MUTATE=P1A-ORD: case 4's S_B write and readback moved behind case 5's S_A write (p1_mutord.bin): every case keeps one ordered S_A/S_B pair, windows 4 and 5 overlap; TG68K 168 passed/4 failed (P1.a; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P1.a).
// MUTATION P1.b stimulus red NMH_MUTATE=P1B: case 8's effective address is T_8+8 (p1_mutb.bin); TG68K 168 passed/4 failed (P1.b; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P1.b).
// MUTATION P1.c stimulus red NMH_MUTATE=P1C: case 11 assembled as CLR.L while the table names CLR.W (p1_mutc.bin); TG68K 168 passed/4 failed (P1.c; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P1.c).
// MUTATION P1.c stimulus red NMH_MUTATE=P1C-MOVE: case 11 assembled as MOVE.L #0 while the table (the program's marker names MOVE) names MOVE.W (p1_move_mutc.bin), graded against the MOVE opcodes; TG68K 168 passed/4 failed (P1.c; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P1.c).
// MUTATION P1.c stimulus red NMH_MUTATE=P1C-KIND: the program writes no table marker (p1_mutkind.bin): P1.c has no expected table and fails; TG68K 168 passed/4 failed (P1.c; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P1.c).
// MUTATION P4 stimulus red NMH_MUTATE=NODONE-P4: the P4 run's program (p1_mutnodone.bin) stops in place after S_END without writing DONE: the P4 fixture line of the run is red and the P4 lines are not evaluated; TG68K 169 passed/2 failed (baseline reds only: P4, P6; not red: P4.L), fx68k 67 passed/0 failed (not applicable: the first core has no 68020 run).
// MUTATION P2 stimulus red NMH_MUTATE=P2-LOW: every register loaded with its pattern's lower half zero-extended, USP with $0000E0F0 (p2_low.bin); TG68K 169 passed/4 failed (P2; baseline reds P4, P4.L, P6), fx68k 66 passed/1 failed (P2).
// MUTATION P2.a stimulus red NMH_MUTATE=P2A: step 8's S_B write made twice (p2_muta.bin): the window up to the first S_B stays usable; TG68K 168 passed/4 failed (P2.a; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P2.a).
// MUTATION P2.b stimulus red NMH_MUTATE=P2B: the store of D3 left out (p2_mutb.bin); TG68K 168 passed/4 failed (P2.b; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P2.b).
// MUTATION P2.c stimulus red NMH_MUTATE=P2C: a TRAP #0 through an RTE-only handler between steps 6 and 7 (p2_mutc.bin); TG68K 168 passed/4 failed (P2.c; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P2.c).
// MUTATION P2.c stimulus red NMH_MUTATE=P2C-RD: a TST.W of $4110 between steps 6 and 7 (p2_mutrd.bin): a read inside the window outside the program image and SAVE_SP, and no write; P2.b holds and the P2 relation is not evaluated; TG68K 168 passed/4 failed (P2.c; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P2.c).
// MUTATION P2.c stimulus red NMH_MUTATE=P2C-VEC: a TST.W of vector word $0010 between steps 6 and 7 (p2_mutvec.bin): a read inside the window in the vector area, not of word 0, and no write; P2.b holds, the P2 relation is not evaluated; TG68K 168 passed/4 failed (P2.c; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P2.c).
// MUTATION P3.a stimulus red NMH_MUTATE=P3A: block 2's landing marker written twice (p3_muta.bin); TG68K 164 passed/6 failed (P3.a x3; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P3.a).
// MUTATION P3.a stimulus red NMH_MUTATE=P3A-ORD: block 2's progress marker written inside block 1, so the markers run P1,P2,L1,L2,... with one S_P and one S_L write per block (p3_mutord.bin); TG68K 164 passed/6 failed (P3.a x3; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P3.a).
// MUTATION P3.a stimulus red NMH_MUTATE=P3A-P6: block 2's landing marker written twice in P6's eight blocks (p6_muta.bin), the reused P3.a line; TG68K 167 passed/4 failed (P3.a x2; baseline reds P4, P4.L; not red: P6), fx68k 65 passed/1 failed (P3.a).
// MUTATION P3.b stimulus red NMH_MUTATE=P3B: block 1's BRA.S aimed behind the first guard word, so the guard's S_X store runs and the guard branches to L_1 (p3_mutb.bin); TG68K 164 passed/6 failed (P3.b x3; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P3.b).
// MUTATION P3.b stimulus red NMH_MUTATE=P3B-P6: block 1's BRA.S aimed behind the first guard word in P6 (p6_mutb.bin), the reused P3.b line; TG68K 167 passed/4 failed (P3.b x2; baseline reds P4, P4.L; not red: P6), fx68k 65 passed/1 failed (P3.b).
// MUTATION P3.c stimulus red NMH_MUTATE=P3C: a TST.W W_3 (absolute long) before block 3 (p3_mutc.bin); TG68K 164 passed/6 failed (P3.c x3; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P3.c).
// MUTATION P3.c stimulus red NMH_MUTATE=P3C-PC: a MOVE.W W_3(PC),D2 before block 3 (p3_mutcpc.bin); TG68K 164 passed/6 failed (P3.c x3; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P3.c).
// MUTATION P3.c stimulus red NMH_MUTATE=P3C-REG: a LEA W_3(PC),A1 and a TST.W (A1) before block 3 (p3_mutcreg.bin); TG68K 164 passed/6 failed (P3.c x3; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P3.c).
// MUTATION P3.c stimulus red NMH_MUTATE=P3C-P6: a MOVE.W W_3(PC),D2 before block 3 in P6 (p6_mutc.bin), the reused P3.c line; TG68K 167 passed/4 failed (P3.c x2; baseline reds P4, P4.L; not red: P6), fx68k 65 passed/1 failed (P3.c).
// MUTATION P3.c stimulus red NMH_MUTATE=P3C-IDX: a LEA W_3(PC),A1, a MOVEQ #0,D1 and a TST.W 0(A1,D1.W) before block 3 (p3_mutcidx.bin): the indexed read lands on the guard word W_3 and the static check reports a guard access; TG68K 164 passed/6 failed (P3.c x3; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P3.c).
// MUTATION P3.c stimulus red NMH_MUTATE=P3C-OP: an ADDQ.W #1,(W_3).l before block 3 (p3_mutcop.bin): the instruction reads and writes the guard word W_3 and the static check reports a guard access; TG68K 164 passed/6 failed (P3.c x3; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P3.c).
// MUTATION P3.d stimulus red NMH_MUTATE=P3D: block 1 assembled as BRA.W while the table names BRA.S (p3_mutd.bin); TG68K 164 passed/6 failed (P3.d x3; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P3.d).
// MUTATION P3.d stimulus red NMH_MUTATE=P3D-P6: block 1 assembled as BRA.W while the table names BRA.S in P6, the pair's pad NOP dropped (p6_mutd.bin), the reused P3.d line; TG68K 167 passed/4 failed (P3.d x2; baseline reds P4, P4.L; not red: P6), fx68k 65 passed/1 failed (P3.d).
// MUTATION P3.d stimulus red NMH_MUTATE=P3-WIN: the four blocks run before S_START is written, and the markers P1,L1,...,P4,L4 and then S_END follow (p3_mutwin.bin): P3.a, P3.b, P3.c and C1-C4 hold, the run window contains no block, so no CPU read of X_k lies inside it and P3.d goes red alone; the main relation is not evaluated; TG68K 164 passed/6 failed (P3.d x3; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (P3.d).
// MUTATION P3 none impossible p.13: "All 68000 family processors have a pre-fetch feature.": every processor the system can run prefetches, so no program makes a read at a guard word W_k inside the run window disappear while the preconditions hold, and the main relation has no stimulus contrast; the calibration line (P3-CAL) shows that the counter can report absence.
// MUTATION P3 stimulus red NMH_MUTATE=P3-CAL: a TST.W of the far word Z before the ending (p3_mutcal.bin): the calibration line goes red; TG68K 167 passed/6 failed (P3 x3; baseline reds P4, P4.L, P6), fx68k 66 passed/1 failed (P3).
// MUTATION P5.a stimulus red NMH_MUTATE=P5A: case 1 executes TRAP #1 with its handler also installed at vector 33, in the 68000-mode image (p5_muta.bin) and in the VBR image of the 68010 and 68020 runs (p5_vbr_muta.bin): the exception is taken and the window populated, but the installed vector 32 is not read; the pair relations are not evaluated; TG68K 164 passed/6 failed (P5.a x3; baseline reds P4, P4.L, P6), fx68k 66 passed/1 failed (P5.a).
// MUTATION P5.b stimulus red NMH_MUTATE=P5B: SUBQ.L #4,A7 and a NOP take the place of step 3's MOVEC in the 68010 image (p5_mutb.bin): the image still differs from the 68000-mode image only inside step 3, PRE_SP differs by four, so the dynamic half goes red; the pair relations are not evaluated; TG68K 166 passed/4 failed (P5.b; baseline reds P4, P4.L, P6), fx68k 67 passed/0 failed (not applicable: this core never loads the 68010 image).
// MUTATION P5.b stimulus red NMH_MUTATE=P5B-SPAN: the 68010 run loads the VBR image with case 3's MOVE.W #1,CCR (p5_mutspan.bin, MOVEC kept): it differs from the 68000-mode image inside step 3, as every VBR image does, and at one byte (image offset $FB) outside it, so the static half's 'only inside step 3' clause fails; the pair relations are not evaluated; TG68K 166 passed/4 failed (P5.b; baseline reds P4, P4.L, P6), fx68k 67 passed/0 failed (not applicable: this core never loads a VBR-side image).
// MUTATION P5.c stimulus red NMH_MUTATE=P5C: an extra MOVE.W D0,S_P inside W_2, in the 68000-mode image (p5_mutc.bin) and in the VBR image of the 68010 and 68020 runs (p5_vbr_mutc.bin); the pair relations are not evaluated; TG68K 164 passed/6 failed (P5.c x3; baseline reds P4, P4.L, P6), fx68k 66 passed/1 failed (P5.c).
// MUTATION P5 stimulus red NMH_MUTATE=P5-PAIR-0010: the 68000-mode image runs under the 68010 cpu_config value while the 68010 run keeps the VBR image: the pair (68000,68010) writes equal frames; TG68K 169 passed/4 failed (P5; baseline reds P4, P4.L, P6), fx68k 67 passed/0 failed (not applicable: the pair relations run on the second core only).
// MUTATION P5 stimulus red NMH_MUTATE=P5-PAIR-0020: the 68000-mode image runs under the 68020 cpu_config value while the 68020 run keeps the VBR image: the pair (68000,68020) writes equal frames; TG68K 169 passed/4 failed (P5; baseline reds P4, P4.L, P6), fx68k 67 passed/0 failed (not applicable: the pair relations run on the second core only).
// MUTATION P5 stimulus red NMH_MUTATE=P5-PAIR-1020: the 68010 run loads the VBR image under cpu_config 68020: the pair (68010,68020) writes equal frames; TG68K 169 passed/4 failed (P5; baseline reds P4, P4.L, P6), fx68k 67 passed/0 failed (not applicable: the pair relations run on the second core only).
// MUTATION P6 stimulus red NMH_MUTATE=P6ALIGN: the first pair's extra pad word left out (p6_mutalign.bin): X1 and X2 land at the same parity mod 4; TG68K 168 passed/4 failed (P6; baseline reds P4, P4.L, P6), fx68k 66 passed/1 failed (P6).
// MUTATION P6.e stimulus red NMH_MUTATE=P6E: every guard shortened to one word, with one pad NOP per guard so every pair keeps opposite parity (p6_mute.bin): D(68000 mode) equals the guard length; TG68K 169 passed/3 failed (P6.e; baseline reds P4, P4.L; not red: P6), fx68k 66 passed/1 failed (P6.e).
// MUTATION C5 stimulus red NMH_MUTATE=C5-P1: the P1 run requests cpu_config 00 and is driven with 03: the port shows 03, the C5 line of the run is red and P1's relation is not evaluated; TG68K 168 passed/4 failed (C5; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (C5).
// MUTATION C5 stimulus red NMH_MUTATE=C5-P2: the P2 run requests cpu_config 00 and is driven with 03: the C5 line of the run is red and P2's relation is not evaluated; TG68K 168 passed/4 failed (C5; baseline reds P4, P4.L, P6), fx68k 65 passed/1 failed (C5).
// MUTATION C5 stimulus red NMH_MUTATE=C5-P3: the P3 runs that request 01 and 11 are driven with 00: the C5 line of each is red and P3's relation is not evaluated for them; TG68K 166 passed/5 failed (C5 x2; baseline reds P4, P4.L, P6), fx68k 67 passed/0 failed (not applicable: the first core requests 00 only).
// MUTATION C5 stimulus red NMH_MUTATE=C5-P4: the P4 run requests cpu_config 11 and is driven with 00: the C5 line of the run is red and the P4 lines are not evaluated; TG68K 169 passed/2 failed (C5; baseline reds P6; not red: P4, P4.L), fx68k 67 passed/0 failed (not applicable: the first core has no 68020 run).
// MUTATION C5 stimulus red NMH_MUTATE=C5-P5: the P5 runs that request 01 and 11 are driven with the other advanced value (11 and 01): the C5 line of each is red, MOVEC and every P5 precondition stay live, and the pair relations are not evaluated; TG68K 165 passed/5 failed (C5 x2; baseline reds P4, P4.L, P6), fx68k 67 passed/0 failed (not applicable: the first core requests 00 only).
// MUTATION C5 stimulus red NMH_MUTATE=C5-P6: the P6 run that requests 11 is driven with 00: the C5 line of the run is red and the P6 relation is not evaluated; TG68K 169 passed/3 failed (C5; baseline reds P4, P4.L; not red: P6), fx68k 67 passed/0 failed (not applicable: the first core requests 00 only).
// MUTATION C5 stimulus red NMH_MUTATE=C5-P7: the P7 runs that request 01 and 11 are driven with 00: the C5 line of each is red and the pair lines are not evaluated; TG68K 166 passed/5 failed (C5 x2; baseline reds P4, P4.L, P6), fx68k 67 passed/0 failed (not applicable: the first core requests 00 only).
// MUTATION C4 measured NMH_MUTATE=C4POP-P2: calibration of the not-evaluated rule, not a stimulus-red record of C4: every S_A and S_B write of P2 is left out (p2_mutc4pop.bin), the opener and closer population is empty, so the C4 line prints not evaluated (RECORDED) in place of PASS and no C4 line is red; P2.a and P2.b go red as the program's own anchors; TG68K 166 passed/5 failed (P2.a, P2.b; baseline reds P4, P4.L, P6), fx68k 63 passed/2 failed (P2.a, P2.b).
// MUTATION C4 measured NMH_MUTATE=C4POP-P3: calibration of the not-evaluated rule, not a stimulus-red record of C4: every S_P and S_L write of P3 is left out (p3_mutc4pop.bin), the population is empty, so the C4 line prints not evaluated (RECORDED) in place of PASS and no C4 line is red; P3.a goes red as the program's own anchor; TG68K 161 passed/6 failed (P3.a x3; baseline reds P4, P4.L, P6), fx68k 64 passed/1 failed (P3.a).
// MUTATION C4 measured NMH_MUTATE=C4POP-P7: calibration of the not-evaluated rule, not a stimulus-red record of C4: every S_A and S_B write of P7 is left out (p7_mutc4pop.bin), the population is empty, so the C4 line prints not evaluated (RECORDED) in place of PASS in every run, the control run included, and no C4 line is red; P7.a and P7.c go red as the program's own anchors; TG68K 155 passed/11 failed (P7.a x4, P7.c x4; baseline reds P4, P4.L, P6), fx68k 64 passed/2 failed (P7.a, P7.c).
// MUTATION P3.c measured NMH_MUTATE=P3C-UNSUP: calibration of the unsupported-decoder rule, not a stimulus-red record of P3.c: a SWAP D2 before block 3 (p3_mutcunsup.bin), the static check does not decode SWAP, so the P3.c line prints not evaluated (RECORDED) in place of PASS and the main relation is not evaluated; no line is red; TG68K 164 passed/3 failed (baseline reds only: P4, P4.L, P6), fx68k 65 passed/0 failed.
// MUTATION P4 stimulus red NMH_MUTATE=P4-68000: the P4 run requests cpu_config 00 and is driven with 00 (the equal control of the contrasting case): C5 holds, the byte and word relation goes red through the unchanged comparator in all 14 cases because CLR reads first on the 68000; TG68K 170 passed/3 failed (baseline reds only: P4, P4.L, P6), fx68k 67 passed/0 failed (not applicable: the first core has no 68020 run).
// MUTATION P4 stimulus green NMH_MUTATE=P4-MOVE: the P4 run loads P1's MOVE variant (p1_move.bin, the table marker names MOVE) under cpu_config 11: MOVE.x #0 makes a single write and no read, so the byte and word relation is green through the unchanged comparator (the baseline is red); TG68K 172 passed/1 failed (baseline reds only: P6; not red: P4, P4.L), fx68k 67 passed/0 failed (not applicable: the first core has no 68020 run).
// MUTATION P4.L stimulus red NMH_MUTATE=P4-68000: the P4 run requests and is driven with cpu_config 00: CLR.L reads before it writes, the long-word relation goes red in all 7 cases through the unchanged comparator; TG68K 170 passed/3 failed (baseline reds only: P4, P4.L, P6), fx68k 67 passed/0 failed (not applicable: the first core has no 68020 run).
// MUTATION P4.L stimulus green NMH_MUTATE=P4-MOVE: the P4 run loads P1's MOVE variant under cpu_config 11: MOVE.L #0 writes both words and reads none, so the long-word relation is green through the unchanged comparator (the baseline is red); TG68K 172 passed/1 failed (baseline reds only: P6; not red: P4, P4.L), fx68k 67 passed/0 failed (not applicable: the first core has no 68020 run).
// MUTATION P1.a stimulus red NMH_MUTATE=P4-P1A: case 5's S_A store left out of the program the P4 run loads (p1_muta.bin): the P1.a line of the P4 run is red; TG68K 169 passed/2 failed (P1.a; baseline reds P6; not red: P4, P4.L), fx68k 67 passed/0 failed (not applicable: the first core has no 68020 run).
// MUTATION P1.b stimulus red NMH_MUTATE=P4-P1B: case 8's effective address is T_8+8 in the program the P4 run loads (p1_mutb.bin): the P1.b line of the P4 run is red; TG68K 169 passed/2 failed (P1.b; baseline reds P6; not red: P4, P4.L), fx68k 67 passed/0 failed (not applicable: the first core has no 68020 run).
// MUTATION P1.c stimulus red NMH_MUTATE=P4-P1C: case 11 assembled as CLR.L while the table names CLR.W in the program the P4 run loads (p1_mutc.bin): the P1.c line of the P4 run is red; TG68K 169 passed/2 failed (P1.c; baseline reds P6; not red: P4, P4.L), fx68k 67 passed/0 failed (not applicable: the first core has no 68020 run).
// MUTATION C1 stimulus red NMH_MUTATE=C1-P4: an extra INTENA write ($C000) follows step 2 in the program the P4 run loads (p1_mutc1.bin): the C1 line of the P4 run is red; TG68K 169 passed/2 failed (C1; baseline reds P6; not red: P4, P4.L), fx68k 67 passed/0 failed (not applicable: the first core has no 68020 run).
// MUTATION C2 stimulus red NMH_MUTATE=C2-P4: an extra DMACON write ($8240) follows step 4 in the program the P4 run loads (p1_mutc2.bin): the C2 line of the P4 run is red; TG68K 169 passed/2 failed (C2; baseline reds P6; not red: P4, P4.L), fx68k 67 passed/0 failed (not applicable: the first core has no 68020 run).
// MUTATION C3 stimulus red NMH_MUTATE=C3-P4: the S_END write is made twice in the program the P4 run loads (p1_mutc3.bin): the C3 line of the P4 run is red; TG68K 169 passed/2 failed (C3; baseline reds P6; not red: P4, P4.L), fx68k 67 passed/0 failed (not applicable: the first core has no 68020 run).
// MUTATION C4 stimulus red NMH_MUTATE=C4-P4: the S_START write is made twice in the program the P4 run loads (p1_mutc4.bin): the C4 line of the P4 run is red; TG68K 169 passed/2 failed (C4; baseline reds P6; not red: P4, P4.L), fx68k 67 passed/0 failed (not applicable: the first core has no 68020 run).
// MUTATION P4 stimulus red NMH_MUTATE=PEEK-P4: S_START is overwritten with $0000 after the S_END write in the program the P4 run loads (p1_mutpeek.bin): the peek_word readback line of the P4 run is red and the P4 lines are not evaluated; TG68K 169 passed/2 failed (baseline reds only: P4, P6; not red: P4.L), fx68k 67 passed/0 failed (not applicable: the first core has no 68020 run).
// MUTATION P6 stimulus red NMH_MUTATE=P6-68000: the second P6 run requests cpu_config 00 and is driven with 00 (the equal-68000 control): C5 holds, both runs are 68000-mode runs of the same image, D equals D and the relation D(second) > D(first) goes red; TG68K 170 passed/3 failed (baseline reds only: P4, P4.L, P6), fx68k 67 passed/0 failed (not applicable: the first core runs one P6 run only).
// MUTATION P7.a stimulus red NMH_MUTATE=P7A: the write of $E702 to S_A left out (p7_muta.bin): P7.a is red in every run and the pair lines are not evaluated; TG68K 163 passed/7 failed (P7.a x4, the control run included; baseline reds P4, P4.L, P6), fx68k 66 passed/1 failed (P7.a).
// MUTATION P7.b stimulus red NMH_MUTATE=P7B: the instruction at X_m assembled as MOVE.W SR,D2 (p7_mutb.bin): the opcode read at X_m is $40C2, P7.b is red in every run; TG68K 163 passed/7 failed (P7.b x4, the control run included; baseline reds P4, P4.L, P6), fx68k 66 passed/1 failed (P7.b).
// MUTATION P7.c stimulus red NMH_MUTATE=P7C: TRAP #1 executed in place of the privileged instruction, with the handler also installed at vector 33 (p7_mutc.bin): the window closes but no read at the privilege vector lies inside it, P7.c is red in every run; TG68K 163 passed/7 failed (P7.c x4, the control run included; baseline reds P4, P4.L, P6), fx68k 66 passed/1 failed (P7.c).
// MUTATION P7 stimulus red NMH_MUTATE=P7-CTL: the control run of the 68000 mode loads a variant whose TRAP handler writes $7003 in place of $7002 to MRK_TRAP (p7_mutctl.bin): the control run passes its fixture, C1-C5, P7.a, P7.b and P7.c, its outcome (DEST, MRK_PRIV, MRK_TRAP) differs from the first 68000-mode run's, so the same-mode control goes red through the unchanged comparator and both pair lines are not evaluated; TG68K 167 passed/4 failed (P7; baseline reds P4, P4.L, P6), fx68k 67 passed/0 failed (not applicable: the first core takes no part in the control).
// MUTATION P7 stimulus red NMH_MUTATE=P7-PAIR-0010: the 68010 run requests and is driven with cpu_config 00: C5 holds, the two outcomes (DEST, MRK_PRIV, MRK_TRAP) of the pair (68000,68010) are equal and that pair's relation goes red alone; TG68K 169 passed/4 failed (P7; baseline reds P4, P4.L, P6), fx68k 67 passed/0 failed (not applicable: the first core takes no part in the pair lines).
// MUTATION P7 stimulus red NMH_MUTATE=P7-PAIR-0020: the 68020 run requests and is driven with cpu_config 00: C5 holds, the two outcomes (DEST, MRK_PRIV, MRK_TRAP) of the pair (68000,68020) are equal and that pair's relation goes red alone; TG68K 169 passed/4 failed (P7; baseline reds P4, P4.L, P6), fx68k 67 passed/0 failed (not applicable: the first core takes no part in the pair lines).
// MUTATION P7 stimulus red NMH_MUTATE=NODONE-P7: the fixture's DONE clause: the program stops in place after the S_END write without writing DONE (p7_mutnodone.bin); the DONE clause of every P7 run's fixture line is red and the relations that depend on it are not evaluated; TG68K 163 passed/7 failed (P7 x4; baseline reds P4, P4.L, P6), fx68k 66 passed/1 failed (P7).
// MUTATION P7 stimulus red NMH_MUTATE=PEEK-P7: the fixture's readback clause: S_START is overwritten with $0000 after the S_END write, the CPU write of $0001 stays in the log (p7_mutpeek.bin); the peek_word readback line of every P7 run is red; TG68K 163 passed/7 failed (P7 x4; baseline reds P4, P4.L, P6), fx68k 66 passed/1 failed (P7).
// MUTATION C1 stimulus red NMH_MUTATE=C1-P7: an extra INTENA write ($C000) follows step 2 (p7_mutc1.bin); TG68K 163 passed/7 failed (C1 x4, the control run included; baseline reds P4, P4.L, P6), fx68k 66 passed/1 failed (C1).
// MUTATION C2 stimulus red NMH_MUTATE=C2-P7: an extra DMACON write ($8240) follows step 4 (p7_mutc2.bin); TG68K 163 passed/7 failed (C2 x4, the control run included; baseline reds P4, P4.L, P6), fx68k 66 passed/1 failed (C2).
// MUTATION C3 stimulus red NMH_MUTATE=C3-P7: the S_END write is made twice (p7_mutc3.bin); TG68K 163 passed/7 failed (C3 x4, the control run included; baseline reds P4, P4.L, P6), fx68k 66 passed/1 failed (C3).
// MUTATION C4 stimulus red NMH_MUTATE=C4-P7: the S_START write is made twice (p7_mutc4.bin); TG68K 163 passed/7 failed (C4 x4, the control run included; baseline reds P4, P4.L, P6), fx68k 66 passed/1 failed (C4).
//
// Not mutated, because they print no graded line: P5's per-member frames, P7's
// pair (68010, 68020) and the claims c52, c56, c64, c65 and c67 are RECORDED or
// withdrawn lines.
//
// P4, P4.L and P6 are red at the baseline, so their stimulus records cannot be
// "made red". Each has the equal control the specification names: the same
// comparator on the 68000 setting (P4-68000, P6-68000) is red because CLR reads
// first and the processors are equal, and a wrong-mode run (C5-P4, C5-P6) is
// not evaluated. P4 and P4.L also have the live comparator: P1's MOVE variant
// under the 68020 setting (P4-MOVE) makes a single write and no read, and the
// same comparator is green. No stimulus is known on which P6's relation is
// green: the page states no prefetch depth.
//
// P5's and P6's contrasting case across the two cores spans two executables, and
// neither build can read the other's data; each core prints its own RECORDED
// frame or depth data and the comparison is drawn once both outputs exist
// (SPECIFICATION.md section 11).

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <map>
#include <set>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

// ---- nanomig.i, restated -------------------------------------------------
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kRunBudget  = 4'000'000;

// ---- ch1.i, restated (see that file for the reasoning) -------------------
constexpr uint32_t S_A      = 0x4000;
constexpr uint32_t S_B      = 0x4002;
constexpr uint32_t S_START  = 0x4004;
constexpr uint32_t S_END    = 0x4006;
constexpr uint32_t S_FAIL   = 0x4008;
constexpr uint32_t S_P      = 0x400A;
constexpr uint32_t S_L      = 0x400C;
constexpr uint32_t S_X      = 0x400E;
constexpr uint32_t R_INT    = 0x4010;
constexpr uint32_t R_DMA    = 0x4012;
constexpr uint32_t SAVE_SP  = 0x4014;
constexpr uint32_t S_KIND   = 0x401C;
constexpr uint32_t PRE_SP   = 0x4018;
constexpr uint32_t POST_SP[6] = {0,0x4024,0x4028,0x402C,0x4030,0x4034}; // index e=1..5 -> POST_SP[e]; POST_SP1=$4024 etc
constexpr uint32_t T_BASE   = 0x4100;
constexpr uint32_t R_BASE   = 0x4300;
constexpr uint32_t XTAB     = 0x4400;
constexpr uint32_t M_BASE   = 0x4500;
constexpr uint32_t M_USP    = 0x4540;
constexpr uint32_t XTAB3    = 0x4550;
constexpr uint32_t XTAB6    = 0x4570;
constexpr uint32_t DEST     = 0x4040;
constexpr uint32_t MRK_PRIV = 0x4042;
constexpr uint32_t MRK_TRAP = 0x4044;
constexpr uint32_t XTAB7    = 0x4590;
constexpr uint32_t FARZ     = 0x1FFFFE;
constexpr uint32_t kProgramOrg = 0x080400; // nanomig_harness.h: load_program()'s fixed code entry

struct Checks {
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    // A precondition gates its relation: only graded when the precondition
    // holds (SPECIFICATION.md 2.5, "not evaluated, never green").
    void expect_gated(bool precond_ok, bool relation, const std::string& label) {
        if (precond_ok) expect(relation, label);
        else record(label + " -- not evaluated: a precondition failed");
    }
    int passed = 0, failed = 0;
};

uint32_t wa(uint32_t byte_addr) { return byte_addr / 2; }

// The label of a run is the cpu_config value the test requests for it.
std::string mode_name(uint8_t c) { return c == 0x00 ? "68000 mode" : c == 0x01 ? "68010 mode" : "68020 mode"; }

// NMH_MUTATE, set by main. Only the wrong-mode contrast of C5 (C5-<program>)
// drives another cpu_config value than the one a run requests; every other
// selector changes a program or a request.
std::string g_mutate;
uint8_t driven_cfg(uint8_t requested, const char* prog) {
    if (g_mutate != std::string("C5-") + prog) return requested;
    // P5 executes MOVEC in its 68010 and 68020 runs, which faults in the
    // 68000 mode: each advanced request is driven with the other advanced
    // value, so MOVEC and the P5 preconditions stay live.
    if (requested != 0x00 && std::string(prog) == "P5") return requested == 0x01 ? 0x03 : 0x01;
    if (requested != 0x00) return 0x00;
    const std::string p = prog;
    return (p == "P1" || p == "P2") ? 0x03 : requested;
}

std::string hex2(unsigned v) { char b[8]; std::snprintf(b, sizeof b, "%02X", v); return b; }

// C5 (SPECIFICATION.md 2.5): ports()->cpu_config shows the requested value
// after the boot and after the run. It proves the input, not the core's
// internal mode.
struct ModeSeen { uint8_t boot = 0xFF, end = 0xFF; };
bool c5_line(Checks& checks, const ModeSeen& m, uint8_t requested, const std::string& run) {
    const bool ok = m.boot == requested && m.end == requested;
    checks.expect(ok, "C5: ports()->cpu_config shows the requested value 0x" + hex2(requested) +
                      " after the boot (0x" + hex2(m.boot) + ") and after the run (0x" + hex2(m.end) +
                      "); the input is shown, not the core's internal mode (" + run + ")");
    return ok;
}

uint16_t peekw(nmh::NanomigHarness& h, uint32_t byte_addr) { return h.peek_word(wa(byte_addr)); }
uint32_t peekl(nmh::NanomigHarness& h, uint32_t byte_addr) {
    return (uint32_t(peekw(h, byte_addr)) << 16) | peekw(h, byte_addr + 2);
}

// First write of `data` to `byte_addr`, and how many such writes occurred.
struct WHit { bool found = false; uint64_t cycle = 0; int count = 0; };
WHit find_write(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr, uint16_t data) {
    WHit w;
    uint32_t a = wa(byte_addr);
    for (const auto& x : log) {
        if (x.is_write && x.word_addr == a && x.data == data) {
            if (!w.found) { w.found = true; w.cycle = x.cycle; }
            ++w.count;
        }
    }
    return w;
}

// Every access at byte_addr with open < cycle < close, cycle-sorted.
std::vector<nmh::RamAccess> accesses_between(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr,
                                              uint64_t open, uint64_t close) {
    uint32_t a = wa(byte_addr);
    std::vector<nmh::RamAccess> out;
    for (const auto& x : log) if (x.word_addr == a && x.cycle > open && x.cycle < close) out.push_back(x);
    std::sort(out.begin(), out.end(), [](const nmh::RamAccess& p, const nmh::RamAccess& q) { return p.cycle < q.cycle; });
    return out;
}

// Count of CPU reads at byte_addr strictly inside (open,close).
int read_count_between(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr, uint64_t open, uint64_t close) {
    int n = 0;
    for (const auto& a : accesses_between(log, byte_addr, open, close)) if (!a.is_write) ++n;
    return n;
}

struct WHitData { bool found = false; uint16_t data = 0; uint64_t cycle = 0; };
// Every CPU write to byte_addr with cycle > after; count and last data.
struct WriteInfo { int count = 0; uint16_t last_data = 0; };
WriteInfo writes_strictly_after(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr, uint64_t after) {
    WriteInfo w;
    uint32_t a = wa(byte_addr);
    for (const auto& x : log) if (x.is_write && x.word_addr == a && x.cycle > after) { ++w.count; w.last_data = x.data; }
    return w;
}
// The first CPU write to byte_addr in the whole log.
WHitData first_write(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr) {
    uint32_t a = wa(byte_addr);
    for (const auto& x : log) if (x.is_write && x.word_addr == a) return {true, x.data, x.cycle};
    return {};
}
// The last CPU read of byte_addr with open < cycle < close; not found when
// there is none. P3.d and P6's reused P3.d ask for the opcode word of a CPU
// read inside the run window R.
WHitData last_read_between(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr, uint64_t open, uint64_t close) {
    WHitData w;
    uint32_t a = wa(byte_addr);
    for (const auto& x : log) if (!x.is_write && x.word_addr == a && x.cycle > open && x.cycle < close) w = {true, x.data, x.cycle};
    return w;
}
// The last CPU read of byte_addr in the whole log: P1.c asks for the opcode
// word a CPU read actually carried, not the memory model's own final word at
// that address.
WHitData last_read(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr) {
    WHitData w;
    uint32_t a = wa(byte_addr);
    for (const auto& x : log) if (!x.is_write && x.word_addr == a) w = {true, x.data, x.cycle};
    return w;
}

// Reads a whole file (a package .bin) into bytes; empty if it cannot be opened.
std::vector<uint8_t> read_file_bytes(const std::string& path) {
    std::vector<uint8_t> out;
    std::FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) return out;
    std::fseek(f, 0, SEEK_END);
    long n = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    if (n > 0) { out.resize(size_t(n)); if (std::fread(out.data(), 1, out.size(), f) != out.size()) out.clear(); }
    std::fclose(f);
    return out;
}

// ---- assembler listings and the static checks built on them --------------
//
// vasm's listing (-L) gives, for every source line that emits code, the
// address and the emitted bytes, and at its end the symbol table. The test
// uses the listing of the very program it loaded: after the listing has been
// checked against the image byte for byte, the listing's instruction
// boundaries and symbols are what the image holds.
struct ListItem { uint32_t addr = 0; std::vector<uint8_t> bytes; };
struct Listing {
    bool ok = false;
    std::vector<ListItem> items;            // in file order
    std::map<std::string, uint32_t> sym;    // labels (A:) and equates (E:)
    std::set<uint32_t> labels;              // addresses of the labels
};

int hex_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

std::string listing_path(const std::string& bin_path) {
    const std::string ext = ".bin";
    if (bin_path.size() > ext.size() && bin_path.compare(bin_path.size() - ext.size(), ext.size(), ext) == 0)
        return bin_path.substr(0, bin_path.size() - ext.size()) + ".lst";
    return bin_path + ".lst";
}

Listing read_listing(const std::string& path) {
    Listing L;
    std::FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) return L;
    std::string text;
    char buf[4096];
    size_t n;
    while ((n = std::fread(buf, 1, sizeof(buf), f)) > 0) text.append(buf, n);
    std::fclose(f);
    bool in_names = false;
    size_t pos = 0;
    while (pos < text.size()) {
        size_t eol = text.find('\n', pos);
        if (eol == std::string::npos) eol = text.size();
        std::string line = text.substr(pos, eol - pos);
        pos = eol + 1;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.rfind("Symbols by name:", 0) == 0) { in_names = true; continue; }
        if (line.rfind("Symbols by value:", 0) == 0) { in_names = false; continue; }
        if (in_names) {
            size_t sp = line.find_first_of(" \t");
            if (sp == std::string::npos || sp == 0) continue;
            size_t k = line.find_first_not_of(" \t", sp);
            if (k == std::string::npos || k + 10 > line.size()) continue;
            char kind = line[k];
            if ((kind != 'A' && kind != 'E') || line[k + 1] != ':') continue;
            uint32_t v = 0;
            bool good = true;
            for (size_t i = 0; i < 8; ++i) {
                int h = hex_value(line[k + 2 + i]);
                if (h < 0) { good = false; break; }
                v = (v << 4) | uint32_t(h);
            }
            if (!good) continue;
            L.sym[line.substr(0, sp)] = v;
            if (kind == 'A') L.labels.insert(v);
            continue;
        }
        // code line: "SS:AAAAAAAA HEXBYTES<spaces><tab> ..."
        if (line.size() < 14 || line[2] != ':' || line[11] != ' ') continue;
        if (hex_value(line[0]) < 0 || hex_value(line[1]) < 0) continue;
        uint32_t addr = 0;
        bool good = true;
        for (size_t i = 3; i < 11; ++i) {
            int h = hex_value(line[i]);
            if (h < 0) { good = false; break; }
            addr = (addr << 4) | uint32_t(h);
        }
        if (!good) continue;
        ListItem it;
        it.addr = addr;
        size_t i = 12;
        while (i + 1 < line.size() && hex_value(line[i]) >= 0 && hex_value(line[i + 1]) >= 0) {
            it.bytes.push_back(uint8_t(hex_value(line[i]) * 16 + hex_value(line[i + 1])));
            i += 2;
        }
        if (!it.bytes.empty()) L.items.push_back(std::move(it));
    }
    L.ok = !L.items.empty();
    return L;
}

// The listing reproduces the image: its items are contiguous from org and
// every byte equals the image's byte.
bool listing_matches_image(const Listing& L, const std::vector<uint8_t>& img, uint32_t org) {
    if (!L.ok || img.empty()) return false;
    uint32_t next = org;
    for (const auto& it : L.items) {
        if (it.addr != next) return false;
        if (size_t(it.addr - org) + it.bytes.size() > img.size()) return false;
        for (size_t i = 0; i < it.bytes.size(); ++i)
            if (img[it.addr - org + i] != it.bytes[i]) return false;
        next += uint32_t(it.bytes.size());
    }
    return next - org == img.size();
}

// The three outcomes of P3.c's static check (SPECIFICATION.md section 5):
// a data access inside a guard range (violation), an instruction or an
// address the check cannot analyse (unsupported: not a guard read, and not
// evaluated), or neither (ok).
struct ScanResult {
    bool violation = false, unsupported = false;
    std::string why;
    bool ok() const { return !violation && !unsupported; }
};

// P3.c's static check. Decodes every instruction of the listing (the 68000
// instructions the package's programs use; any other opcode is unsupported
// analysis, not a guard read) and follows every effective-address path
// that reads or writes memory: absolute short/long, PC-relative, and
// register-indirect with the address register's value tracked from the
// instructions that set it (LEA, MOVEA, MOVE.L #imm,An), and indexed
// d8(An,Dn) / d8(PC,Dn) with the data register's value tracked from MOVEQ. A
// data access whose address cannot be resolved is unsupported analysis. LEA
// and PEA compute an address without accessing it; JMP, JSR and the branches
// transfer control and fetch from their target (that is P3.b's business).
// Address and data registers A0-A6 / D0-D7 are forgotten after an
// unconditional transfer and at every label, and a data register at every
// instruction that may write it; A7 is tracked through the stack
// instructions. No data access may overlap any of the byte ranges
// [first,last] (inclusive).
ScanResult scan_data_accesses(const Listing& L, const std::vector<std::pair<uint32_t, uint32_t>>& regions) {
    ScanResult res;
    auto note_why = [&](uint32_t pc, const std::string& why) {
        if (res.why.empty()) {
            char b[96];
            std::snprintf(b, sizeof(b), "at $%06X: ", pc);
            res.why = std::string(b) + why;
        }
    };
    // unsupported analysis
    auto fail = [&](uint32_t pc, const std::string& why) {
        if (!res.unsupported && !res.violation) note_why(pc, why);
        res.unsupported = true;
    };
    // a data access inside a guard range
    auto violate = [&](uint32_t pc, const std::string& why) {
        if (!res.violation) { res.why.clear(); note_why(pc, why); }
        res.violation = true;
    };
    struct AReg { bool known = false; uint32_t v = 0; };
    struct DReg { bool known = false; int32_t v = 0; };
    AReg A[8];
    DReg D[8];
    std::set<uint32_t> targets = L.labels;
    for (int pass = 0; pass < 2; ++pass) {
        for (auto& r : A) r = AReg();
        for (auto& d : D) d = DReg();
        bool after_uncond = false;
        for (const auto& it : L.items) {
            const auto& b = it.bytes;
            const uint32_t pc = it.addr;
            if (after_uncond || targets.count(pc)) {
                for (int i = 0; i < 7; ++i) A[i] = AReg();
                for (auto& d : D) d = DReg();
            }
            after_uncond = false;
            if (b.size() < 2 || (b.size() & 1)) { fail(pc, "item is not a whole number of words"); continue; }
            auto be16 = [&](size_t off) { return uint16_t((b[off] << 8) | b[off + 1]); };
            auto be32 = [&](size_t off) { return (uint32_t(be16(off)) << 16) | be16(off + 2); };
            const uint16_t op = be16(0);
            size_t ext = 2;

            struct EA { enum K { Dn, An, Mem, Imm } k = Dn; bool known = false; uint32_t addr = 0; };
            // d8(base,Xn) in the 68000's brief extension word format; the index
            // register is a tracked Dn/An, taken as a word or a long.
            auto indexed = [&](bool base_known, uint32_t base) -> EA {
                EA e;
                e.k = EA::Mem;
                if (ext + 2 > b.size()) { fail(pc, "truncated indexed address"); return e; }
                const uint16_t xw = be16(ext);
                ext += 2;
                if (xw & 0x0100) { fail(pc, "full-format extension word"); return e; }
                const int xr = (xw >> 12) & 7;
                bool ik = false;
                int32_t iv = 0;
                if (xw & 0x8000) { if (A[xr].known) { ik = true; iv = int32_t(A[xr].v); } }
                else if (D[xr].known) { ik = true; iv = D[xr].v; }
                if (ik && !(xw & 0x0800)) iv = int32_t(int16_t(iv));
                e.known = base_known && ik;
                e.addr = base + uint32_t(int32_t(int8_t(xw & 0xFF))) + uint32_t(iv);
                return e;
            };
            auto take = [&](int mode, int reg, int size) -> EA {
                EA e;
                auto step = [&](int sz) { return (sz == 1 && reg == 7) ? 2 : sz; };
                switch (mode) {
                case 0: e.k = EA::Dn; break;
                case 1: e.k = EA::An; break;
                case 2: e.k = EA::Mem; e.known = A[reg].known; e.addr = A[reg].v; break;
                case 3: e.k = EA::Mem; e.known = A[reg].known; e.addr = A[reg].v; A[reg].v += uint32_t(step(size)); break;
                case 4: A[reg].v -= uint32_t(step(size)); e.k = EA::Mem; e.known = A[reg].known; e.addr = A[reg].v; break;
                case 5:
                    if (ext + 2 > b.size()) { fail(pc, "truncated d16(An)"); break; }
                    e.k = EA::Mem; e.known = A[reg].known;
                    e.addr = A[reg].v + uint32_t(int32_t(int16_t(be16(ext)))); ext += 2; break;
                case 6:
                    e = indexed(A[reg].known, A[reg].v); break;
                case 7:
                    if (reg == 0) {
                        if (ext + 2 > b.size()) { fail(pc, "truncated abs.W"); break; }
                        e.k = EA::Mem; e.known = true; e.addr = uint32_t(int32_t(int16_t(be16(ext)))); ext += 2;
                    } else if (reg == 1) {
                        if (ext + 4 > b.size()) { fail(pc, "truncated abs.L"); break; }
                        e.k = EA::Mem; e.known = true; e.addr = be32(ext); ext += 4;
                    } else if (reg == 2) {
                        if (ext + 2 > b.size()) { fail(pc, "truncated d16(PC)"); break; }
                        e.k = EA::Mem; e.known = true;
                        e.addr = pc + uint32_t(ext) + uint32_t(int32_t(int16_t(be16(ext)))); ext += 2;
                    } else if (reg == 3) {
                        e = indexed(true, pc + uint32_t(ext));
                    } else if (reg == 4) {
                        e.k = EA::Imm; ext += (size == 4) ? 4 : 2;
                    } else fail(pc, "undefined effective address");
                    break;
                }
                return e;
            };
            auto touch = [&](const EA& e, int size) {
                if (e.k != EA::Mem) return;
                if (!e.known) { fail(pc, "a data access whose address is not resolvable"); return; }
                for (const auto& r : regions)
                    if (e.addr <= r.second && e.addr + uint32_t(size) - 1 >= r.first) {
                        char t[112];
                        std::snprintf(t, sizeof(t), "a %d-byte data access at $%06X lies in guard range $%06X-$%06X",
                                      size, e.addr, r.first, r.second);
                        violate(pc, t);
                    }
            };
            auto set_a = [&](int reg, bool known, uint32_t v) { A[reg].known = known; A[reg].v = v; };
            auto stack_push = [&](int size) {
                if (!A[7].known) { fail(pc, "stack pointer not tracked"); return; }
                A[7].v -= uint32_t(size);
                EA e; e.k = EA::Mem; e.known = true; e.addr = A[7].v; touch(e, size);
            };
            auto stack_pop = [&](int size) {
                if (!A[7].known) { fail(pc, "stack pointer not tracked"); return; }
                EA e; e.k = EA::Mem; e.known = true; e.addr = A[7].v; touch(e, size);
                A[7].v += uint32_t(size);
            };

            const int nib = op >> 12;
            if (nib == 1 || nib == 2 || nib == 3) {                              // MOVE, MOVEA
                const int size = (nib == 1) ? 1 : (nib == 3) ? 2 : 4;
                EA src = take((op >> 3) & 7, op & 7, size);
                EA dst = take((op >> 6) & 7, (op >> 9) & 7, size);
                touch(src, size);
                if (((op >> 6) & 7) == 0) D[(op >> 9) & 7] = DReg();                // MOVE to Dn
                if (((op >> 6) & 7) == 1) {                                         // MOVEA
                    const int an = (op >> 9) & 7;
                    if (src.k == EA::Imm && ext >= 4) {
                        uint32_t v = (size == 2) ? uint32_t(int32_t(int16_t(be16(ext - 2)))) : be32(ext - 4);
                        set_a(an, true, v);
                    } else if (src.k == EA::An && A[op & 7].known) {
                        set_a(an, true, A[op & 7].v);
                    } else set_a(an, false, 0);
                } else touch(dst, size);
            } else if ((op & 0xF1C0) == 0x41C0) {                                // LEA
                EA e = take((op >> 3) & 7, op & 7, 4);
                const int an = (op >> 9) & 7;
                if (e.k == EA::Mem && e.known) set_a(an, true, e.addr); else set_a(an, false, 0);
            } else if ((op & 0xFFC0) == 0x4840 && ((op >> 3) & 7) >= 2) {        // PEA
                take((op >> 3) & 7, op & 7, 4);
                stack_push(4);
            } else if ((op & 0xFFC0) == 0x4EC0) {                                // JMP
                EA e = take((op >> 3) & 7, op & 7, 4);
                if (e.k == EA::Mem && e.known && pass == 0) targets.insert(e.addr);
                after_uncond = true;
            } else if ((op & 0xFFC0) == 0x4E80) {                                // JSR
                EA e = take((op >> 3) & 7, op & 7, 4);
                if (e.k == EA::Mem && e.known && pass == 0) targets.insert(e.addr);
                stack_push(4);
                for (int i = 0; i < 7; ++i) A[i] = AReg();
                for (auto& d : D) d = DReg();
            } else if (op == 0x4E75) {                                           // RTS
                stack_pop(4);
                after_uncond = true;
            } else if (op == 0x4E71 || op == 0x4AFC) {                           // NOP, ILLEGAL
            } else if (nib == 6) {                                               // BRA, Bcc
                const int cond = (op >> 8) & 0xF;
                const int d8 = op & 0xFF;
                if (cond == 1) { fail(pc, "BSR is not analysed"); }
                else if (d8 == 0xFF) { fail(pc, "32-bit branch displacement"); }
                else {
                    int32_t disp = 0;
                    if (d8 == 0) {
                        if (b.size() < 4) fail(pc, "truncated branch");
                        else { disp = int16_t(be16(2)); ext += 2; }
                    } else disp = int8_t(uint8_t(d8));
                    if (pass == 0) targets.insert(pc + 2 + uint32_t(disp));
                    if (cond == 0) after_uncond = true;
                }
            } else if (nib == 7 && (op & 0x0100) == 0) {                         // MOVEQ
                D[(op >> 9) & 7].known = true;
                D[(op >> 9) & 7].v = int32_t(int8_t(op & 0xFF));
            } else if ((op & 0xF000) == 0x5000 && ((op >> 6) & 3) != 3) {        // ADDQ, SUBQ
                const int size = 1 << ((op >> 6) & 3);
                const int mode = (op >> 3) & 7, reg = op & 7;
                EA e = take(mode, reg, size);
                if (mode == 0) D[reg] = DReg();
                else if (mode == 1) set_a(reg, false, 0);
                else touch(e, size);
            } else if (nib == 0xB && (((op >> 6) & 7) <= 3 || ((op >> 6) & 7) == 7)) { // CMP, CMPA
                const int om = (op >> 6) & 7;
                const int size = (om == 0) ? 1 : (om == 1 || om == 3) ? 2 : 4;
                touch(take((op >> 3) & 7, op & 7, size), size);
            } else if ((op & 0xF1C0) == 0x80C0 || (op & 0xF1C0) == 0x81C0) {     // DIVU.W, DIVS.W
                touch(take((op >> 3) & 7, op & 7, 2), 2);
                D[(op >> 9) & 7] = DReg();
            } else if (((op & 0xFF00) == 0x4200 || (op & 0xFF00) == 0x4A00 || (op & 0xFF00) == 0x4400 ||
                        (op & 0xFF00) == 0x4600 || (op & 0xFF00) == 0x4000) && ((op >> 6) & 3) != 3) { // CLR TST NEG NOT NEGX
                const int size = 1 << ((op >> 6) & 3);
                touch(take((op >> 3) & 7, op & 7, size), size);
                if (((op >> 3) & 7) == 0 && (op & 0xFF00) != 0x4A00) D[op & 7] = DReg();
            } else if ((op & 0xFFC0) == 0x44C0 || (op & 0xFFC0) == 0x46C0 || (op & 0xFFC0) == 0x40C0) { // MOVE to CCR/SR, from SR
                touch(take((op >> 3) & 7, op & 7, 2), 2);
                if ((op & 0xFFC0) == 0x40C0 && ((op >> 3) & 7) == 0) D[op & 7] = DReg();
            } else if ((op & 0xFFF0) == 0x4E60) {                                // MOVE USP
                if (op & 8) set_a(op & 7, false, 0);
            } else {
                char t[64];
                std::snprintf(t, sizeof(t), "opcode $%04X is not analysable", op);
                fail(pc, t);
            }
            if (!res.unsupported && !res.violation && ext != b.size()) {
                char t[112];
                std::snprintf(t, sizeof(t), "decoded length %d differs from the listing's %d bytes (opcode $%04X)",
                              int(ext), int(b.size()), op);
                fail(pc, t);
            }
        }
    }
    return res;
}

// P3.c: the program's listing reproduces its image and carries the run's
// own X_k (k = 1..n), and no instruction accesses data in any guard range
// W_k to W_k+11 (W_k = X_k+2). A listing that does not reproduce the image,
// or that does not name the run's own X_k, makes the check red: its input is
// not the program that ran.
ScanResult p3c_static(const std::string& bin, const uint32_t* x, int n) {
    ScanResult bad;
    bad.violation = true;
    auto img = read_file_bytes(bin);
    Listing L = read_listing(listing_path(bin));
    if (!listing_matches_image(L, img, kProgramOrg)) { bad.why = "the listing does not reproduce the image"; return bad; }
    std::vector<std::pair<uint32_t, uint32_t>> regions;
    for (int k = 1; k <= n; ++k) {
        auto s = L.sym.find("X" + std::to_string(k));
        if (s == L.sym.end() || s->second != x[k]) { bad.why = "the listing's X" + std::to_string(k) + " is not the run's own"; return bad; }
        regions.push_back({x[k] + 2, x[k] + 2 + 11});
    }
    return scan_data_accesses(L, regions);
}

// P3.c's printed line: red on a guard access, "not evaluated" on an
// unsupported analysis. Returns true only when the check passed.
bool p3c_line(Checks& checks, const ScanResult& r, const std::string& label) {
    if (r.unsupported && !r.violation) {
        checks.record(label + " -- not evaluated: the static analysis is unsupported (" + r.why + ")");
        return false;
    }
    checks.expect(r.ok(), label);
    if (!r.ok()) checks.note("P3.c first violation: " + r.why);
    return r.ok();
}

// P5.b's static half: the two images have the same length, and every
// differing byte lies inside step 3's four-byte span (VBR_STEP to VBR_END in
// both listings, each listing reproducing its image). Zero differences are
// valid.
bool p5b_static_ok(const std::string& bin_a, const std::string& bin_b, std::string& why) {
    auto a = read_file_bytes(bin_a);
    auto b = read_file_bytes(bin_b);
    Listing la = read_listing(listing_path(bin_a)), lb = read_listing(listing_path(bin_b));
    if (!listing_matches_image(la, a, kProgramOrg) || !listing_matches_image(lb, b, kProgramOrg)) {
        why = "a listing does not reproduce its image";
        return false;
    }
    auto sa = la.sym.find("VBR_STEP"), ea = la.sym.find("VBR_END");
    auto sb = lb.sym.find("VBR_STEP"), eb = lb.sym.find("VBR_END");
    if (sa == la.sym.end() || ea == la.sym.end() || sb == lb.sym.end() || eb == lb.sym.end()) { why = "step 3 is not labelled"; return false; }
    if (sa->second != sb->second || ea->second != eb->second || ea->second - sa->second != 4) {
        why = "step 3's span is not the same four bytes in both listings";
        return false;
    }
    if (a.size() != b.size()) { why = "the images differ in length"; return false; }
    const size_t lo = sa->second - kProgramOrg, hi = lo + 4;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] == b[i]) continue;
        if (i < lo || i >= hi) {
            char t[64];
            std::snprintf(t, sizeof(t), "byte at image offset $%zX differs outside step 3", i);
            why = t;
            return false;
        }
    }
    return true;
}

// P3.a's marker order: the CPU writes to S_P and S_L, in time order, are
// exactly P1,L1,P2,L2,...,Pn,Ln -- one S_P and one S_L write per block, and
// no other write to either.
bool markers_in_order(const std::vector<nmh::RamAccess>& log, int n) {
    std::vector<std::pair<uint64_t, std::pair<uint32_t, uint16_t>>> seq;
    for (const auto& x : log)
        if (x.is_write && (x.word_addr == wa(S_P) || x.word_addr == wa(S_L)))
            seq.push_back({x.cycle, {x.word_addr, x.data}});
    std::stable_sort(seq.begin(), seq.end(), [](const auto& p, const auto& q) { return p.first < q.first; });
    if (int(seq.size()) != 2 * n) return false;
    for (int k = 1; k <= n; ++k) {
        if (seq[2*(k-1)].second != std::make_pair(wa(S_P), uint16_t(k))) return false;
        if (seq[2*(k-1)+1].second != std::make_pair(wa(S_L), uint16_t(k))) return false;
    }
    return true;
}

// The macro-level contrasts (ch1.i) are selected as <contrast>-<program>,
// e.g. C1-P2; each has its own image <stem>_mut<contrast>.bin.
std::string pick_macro_variant(const std::string& mv, const std::string& program,
                               const std::string& dir, const std::string& stem, const std::string& dflt) {
    static const std::pair<const char*, const char*> kMacro[] = {
        {"C1", "mutc1"}, {"C2", "mutc2"}, {"C3", "mutc3"}, {"C3F", "mutc3f"}, {"C4", "mutc4"}, {"C4B", "mutc4b"}, {"C4E", "mutc4e"}, {"C4POP", "mutc4pop"},
        {"NODONE", "mutnodone"}, {"PEEK", "mutpeek"}};
    for (const auto& m : kMacro)
        if (mv == std::string(m.first) + "-" + program) return dir + "/" + stem + "_" + m.second + ".bin";
    return dflt;
}

struct Run {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;
};

// Runs bin under cfg, waits for DONE, returns the coalesced log. The
// harness instance itself is destroyed on return (its memory can only be
// inspected while it is alive), so callers that also need peek_word()
// results pass a callback.
template <class Peeker>
Run run_and_peek(const std::string& bin, const nmh::Config& cfg, Peeker peek) {
    Run r;
    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;
    r.done = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    r.log = nmh::coalesce(h.ram_log());
    peek(h);
    return r;
}

Run run_plain(const std::string& bin, const nmh::Config& cfg) {
    return run_and_peek(bin, cfg, [](nmh::NanomigHarness&) {});
}

bool common_ok(const Run& r) { return r.loaded && r.released && r.done.has_value(); }

// ---------------------------------------------------------------------
// C1-C4 (SPECIFICATION.md 2.5), evaluated on any capture's log; C5 is recorded, not graded.
// ---------------------------------------------------------------------
bool check_c1(const std::vector<nmh::RamAccess>& log, uint16_t& r_int_out) {
    // R_INT's stored value must show bits 14-0 all zero (bit 15 masked).
    // We read it back from the log itself: the CPU write of R_INT.
    for (auto it = log.rbegin(); it != log.rend(); ++it)
        if (it->is_write && it->word_addr == wa(R_INT)) { r_int_out = it->data; return (it->data & 0x7FFF) == 0; }
    return false;
}
bool check_c2(const std::vector<nmh::RamAccess>& log, uint16_t& r_dma_out) {
    for (auto it = log.rbegin(); it != log.rend(); ++it)
        if (it->is_write && it->word_addr == wa(R_DMA)) { r_dma_out = it->data; return (it->data & 0x07FF) == 0; }
    return false;
}
bool check_c3(const std::vector<nmh::RamAccess>& log) {
    auto end_hit = find_write(log, S_END, 0xE0D0);
    auto fail_hit = find_write(log, S_FAIL, 0xFA11);
    return end_hit.found && end_hit.count == 1 && fail_hit.count == 0;
}
// C4's program-specific population (SPECIFICATION.md 2.5): the window-open
// sentinel (S_A, or S_P in P3 and P6) and the window-close sentinel (S_B, or
// S_L) with the values that program's windows write.
struct Pop {
    uint32_t open_addr, close_addr;
    std::vector<uint16_t> open_data, close_data;
};
std::vector<uint16_t> values(uint16_t base, int n) {
    std::vector<uint16_t> v;
    for (int i = 1; i <= n; ++i) v.push_back(uint16_t(base + i));
    return v;
}
Pop pop_p1() { return {S_A, S_B, values(0xA000, 21), values(0xB000, 21)}; }
Pop pop_p2() { return {S_A, S_B, {0xA200}, {0xB200}}; }
Pop pop_p3() { return {S_P, S_L, values(0, 4), values(0, 4)}; }
Pop pop_p5() { return {S_A, S_B, values(0xC000, 5), values(0xD000, 5)}; }
Pop pop_p6() { return {S_P, S_L, values(0, 8), values(0, 8)}; }
Pop pop_p7() { return {S_A, S_B, {0xE701, 0xE702}, {0xF701, 0xF702}}; }

enum class C4 { NotEvaluated, Red, Green };
C4 check_c4(const std::vector<nmh::RamAccess>& log, const Pop& pop) {
    // The population: every CPU write to the program's opener and closer
    // sentinels. Empty on either side, C4 is not evaluated (never green).
    uint64_t first_open = 0, last_close = 0;
    int n_open = 0, n_close = 0;
    bool values_ok = true;
    for (const auto& x : log) {
        if (!x.is_write) continue;
        if (x.word_addr == wa(pop.open_addr)) {
            if (std::find(pop.open_data.begin(), pop.open_data.end(), x.data) == pop.open_data.end()) values_ok = false;
            if (n_open++ == 0 || x.cycle < first_open) first_open = x.cycle;
        } else if (x.word_addr == wa(pop.close_addr)) {
            if (std::find(pop.close_data.begin(), pop.close_data.end(), x.data) == pop.close_data.end()) values_ok = false;
            ++n_close;
            if (x.cycle > last_close) last_close = x.cycle;
        }
    }
    if (n_open == 0 || n_close == 0) return C4::NotEvaluated;
    if (!values_ok) return C4::Red;
    // "S_START comes before every window, and S_END comes after them": S_START
    // is written exactly once with its data, the first opener comes later, and
    // the S_END write comes after the last closer.
    auto s = find_write(log, S_START, 0x0001);
    if (!(s.found && s.count == 1)) return C4::Red;
    if (!(first_open > s.cycle)) return C4::Red;
    auto e = find_write(log, S_END, 0xE0D0);
    if (!(e.found && e.cycle > last_close)) return C4::Red;
    return C4::Green;
}

// C4's printed line; returns true only when C4 was evaluated and holds.
bool c4_line(Checks& checks, const std::vector<nmh::RamAccess>& log, const Pop& pop, const std::string& run) {
    const C4 r = check_c4(log, pop);
    if (r == C4::NotEvaluated) {
        checks.record("C4: not evaluated -- the program's opener or closer population is empty (" + run + ")");
        return false;
    }
    checks.expect(r == C4::Green, "C4: S_START written exactly once, before every window, and S_END written after every window; "
                                  "the opener and closer population is nonempty and takes only the program's own values (" + run + ")");
    return r == C4::Green;
}

// Entry-for-entry comparison of two coalesced RAM-port logs: time, word
// address, data and direction. Used to state, from the run itself, whether a
// cpu_config value changes anything at the port.
std::string compare_logs(const std::vector<nmh::RamAccess>& a, const std::vector<nmh::RamAccess>& b) {
    size_t n = std::min(a.size(), b.size());
    for (size_t i = 0; i < n; ++i) {
        if (a[i].cycle != b[i].cycle || a[i].word_addr != b[i].word_addr || a[i].data != b[i].data ||
            a[i].is_write != b[i].is_write) {
            return "differs: entry " + std::to_string(i) + " of " + std::to_string(a.size()) + " and " +
                   std::to_string(b.size()) + " is the first that differs";
        }
    }
    if (a.size() != b.size())
        return "differs: " + std::to_string(a.size()) + " entries against " + std::to_string(b.size());
    return "identical, entry for entry (time, address, data, direction), " + std::to_string(a.size()) + " entries";
}

} // namespace

// ===========================================================================
// P1 / P4 -- CLR (SPECIFICATION.md sections 3 and 6)
// ===========================================================================
namespace p1 {

struct CaseSpec { int c; char size; };
const std::vector<CaseSpec> kCases = {
    {1,'B'},{2,'B'},{3,'B'},{4,'B'},{5,'B'},{6,'B'},{7,'B'},
    {8,'W'},{9,'W'},{10,'W'},{11,'W'},{12,'W'},{13,'W'},{14,'W'},
    {15,'L'},{16,'L'},{17,'L'},{18,'L'},{19,'L'},{20,'L'},{21,'L'}
};

// Opcode word for case c's CLR.<size> <ea>, from the 68000 instruction
// format (CLR: 0100 0010 ss mmm rrr) and the addressing modes and
// registers p1.s assigns per case (A0-A4 for the five An-modes, in the
// order (An),(An)+,-(An),d16(An),d8(An,Xn); abs.W mode/reg 111/000;
// abs.L mode/reg 111/001).
uint16_t expected_clr_opcode(int c) {
    static const uint16_t kOps[22] = {
        0, 0x4210,0x4219,0x4222,0x422B,0x4234,0x4238,0x4239,
        0x4250,0x4259,0x4262,0x426B,0x4274,0x4278,0x4279,
        0x4290,0x4299,0x42A2,0x42AB,0x42B4,0x42B8,0x42B9
    };
    return kOps[c];
}

// Opcode word for case c's MOVE.<size> #0,<ea> (the P1-MOVE contrast,
// p1_move.s): same destination <ea> as expected_clr_opcode(c), re-fielded
// into MOVE's own layout (68000 instruction format). MOVE encodes size as
// 01=byte,11=word,10=long (bits 13-12) and the destination as register
// (bits 11-9) then mode (bits 8-6) -- reversed from CLR's mode-then-register
// low byte -- with an immediate source (mode 111, register 100, bits 5-0).
uint16_t expected_move_opcode(int c) {
    uint16_t clr = expected_clr_opcode(c);
    uint8_t clr_size = (clr >> 6) & 0x3;      // 00=B,01=W,10=L (CLR's own encoding)
    uint8_t modereg = clr & 0x3F;              // mmm rrr
    uint8_t mode = (modereg >> 3) & 0x7;
    uint8_t reg  = modereg & 0x7;
    uint16_t move_size = (clr_size == 0) ? 0x1 : (clr_size == 1 ? 0x3 : 0x2);
    uint16_t dest_field = uint16_t((reg << 3) | mode); // bits 11-6: register then mode
    return uint16_t((move_size << 12) | (dest_field << 6) | 0x3C);
}

struct Result {
    bool loaded=false, released=false, done=false;
    std::vector<nmh::RamAccess> log;
    uint32_t xtab[22]={0};
    uint16_t opcode[22]={0};
    uint16_t r_int=0, r_dma=0;
    uint16_t fixture_peek=0; // S_START read back through peek_word, before any other peek is trusted
    ModeSeen seen;           // ports()->cpu_config after the boot and after the run (C5)
};

Result run(const std::string& bin, uint8_t cpu_config, const char* tag = "P1") {
    Result res;
    nmh::Config cfg; cfg.cpu_config = driven_cfg(cpu_config, tag);
    nmh::NanomigHarness h(cfg);
    res.loaded = h.load_program(bin) > 0;
    if (!res.loaded) return res;
    res.released = h.boot(kBootBudget).cpu_released;
    res.seen.boot = h.ports()->cpu_config;
    auto d = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    res.seen.end = h.ports()->cpu_config;
    res.done = d.has_value();
    res.log = nmh::coalesce(h.ram_log());
    res.fixture_peek = peekw(h, S_START);
    for (int c = 1; c <= 21; ++c) {
        res.xtab[c] = peekl(h, XTAB + 4u*(c-1));
        res.opcode[c] = last_read(res.log, res.xtab[c]).data;
    }
    res.r_int = peekw(h, R_INT);
    res.r_dma = peekw(h, R_DMA);
    return res;
}

// Window for case c: (open,close) cycles of the S_A/S_B sentinel writes.
struct Win { bool ok=false; uint64_t open=0, close=0; };
Win window(const Result& r, int c) {
    Win w;
    auto a = find_write(r.log, S_A, uint16_t(0xA000+c));
    auto b = find_write(r.log, S_B, uint16_t(0xB000+c));
    w.ok = a.found && a.count==1 && b.found && b.count==1 && a.cycle < b.cycle;
    w.open = a.cycle; w.close = b.cycle;
    return w;
}

bool byte_readback_ok(uint16_t word) { return word == 0x00A5 || word == 0xA500; }

// Grades the CLR relation for one case. expect_single_write: false = P1
// (read then write), true = P4 (a single write, no read).
bool relation_ok(const Result& r, const CaseSpec& cs, const Win& w, bool expect_single_write) {
    uint32_t Tc = T_BASE + 16u*cs.c;
    if (cs.size != 'L') {
        auto acc = accesses_between(r.log, Tc, w.open, w.close);
        if (!expect_single_write) return acc.size()==2 && !acc[0].is_write && acc[1].is_write;
        return acc.size()==1 && acc[0].is_write;
    }
    auto lo = accesses_between(r.log, Tc, w.open, w.close);
    auto hi = accesses_between(r.log, Tc+2, w.open, w.close);
    std::vector<nmh::RamAccess> all = lo; all.insert(all.end(), hi.begin(), hi.end());
    std::sort(all.begin(), all.end(), [](const nmh::RamAccess&a,const nmh::RamAccess&b){return a.cycle<b.cycle;});
    if (all.empty()) return false;
    bool lo_write=false, hi_write=false;
    for (auto& x : lo) if (x.is_write) lo_write = true;
    for (auto& x : hi) if (x.is_write) hi_write = true;
    if (!expect_single_write) return !all.front().is_write && lo_write && hi_write;
    // P4: no read at all, and at least one write to each word.
    bool any_read = false; for (auto& x: all) if (!x.is_write) any_read = true;
    return !any_read && lo_write && hi_write;
}

// P1.b, as specified: the last CPU write to the operand word(s) before W_c
// carries the prefill ($A5A5, and $5A5A at T_c+2 for .L), and the readback
// is the CPU write to R_c (and R_c+2 for .L) -- not just memory's final
// value, which a missing prefill (left at $0000 by the memory model) could
// satisfy by accident.
// T_c and R_c are each dedicated to case c (T_BASE+16*c, R_BASE+4*c: no
// other case ever touches them), so the FIRST CPU write to T_c is always
// the prefill (step 1, before the CLR/MOVE that clears it) and the write
// to R_c is always the single readback (step 6) -- in every program
// variant, independent of whether case c's own S_A/S_B window (P1.a) was
// found, so a missing window (P1A) does not also fail P1.b for that case.
bool precond_b_ok(const Result& r, const CaseSpec& cs) {
    uint32_t Tc = T_BASE + 16u*cs.c;
    uint32_t Rc = R_BASE + 4u*cs.c;
    auto pre_lo = first_write(r.log, Tc);
    if (!pre_lo.found || pre_lo.data != 0xA5A5) return false;
    if (cs.size == 'L') {
        auto pre_hi = first_write(r.log, Tc+2);
        if (!pre_hi.found || pre_hi.data != 0x5A5A) return false;
    }
    auto rb_lo = writes_strictly_after(r.log, Rc, 0);
    if (rb_lo.count != 1) return false;
    if (cs.size == 'B') return byte_readback_ok(rb_lo.last_data);
    if (rb_lo.last_data != 0x0000) return false;
    if (cs.size == 'W') return true;
    auto rb_hi = writes_strictly_after(r.log, Rc+2, 0);
    return rb_hi.count == 1 && rb_hi.last_data == 0x0000;
}

} // namespace p1

// ===========================================================================
// P2 -- 32-bit registers (SPECIFICATION.md section 4)
// ===========================================================================
namespace p2 {

struct Result {
    bool loaded=false, released=false, done=false;
    std::vector<nmh::RamAccess> log;
    uint32_t m[17]={0}; // D0..D7=0..7, A0..A7=8..15, USP=16
    uint16_t fixture_peek=0;
    ModeSeen seen;
};

const uint32_t kPatterns[17] = {
    0x10203040,0x11213141,0x12223242,0x13233343,0x14243444,0x15253545,0x16263646,0x17273747,
    0x8090A0B0,0x8191A1B0,0x8292A2B0,0x8393A3B0,0x8494A4B0,0x8595A5B0,0x8696A6B0,0x8797A7B0,
    0xC0D0E0F0
};

Result run(const std::string& bin, uint8_t cpu_config) {
    Result res;
    nmh::Config cfg; cfg.cpu_config = driven_cfg(cpu_config, "P2");
    nmh::NanomigHarness h(cfg);
    res.loaded = h.load_program(bin) > 0;
    if (!res.loaded) return res;
    res.released = h.boot(kBootBudget).cpu_released;
    res.seen.boot = h.ports()->cpu_config;
    auto d = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    res.seen.end = h.ports()->cpu_config;
    res.done = d.has_value();
    res.log = nmh::coalesce(h.ram_log());
    res.fixture_peek = peekw(h, S_START);
    for (int i = 0; i < 16; ++i) res.m[i] = peekl(h, M_BASE + 4u*i);
    res.m[16] = peekl(h, M_USP);
    return res;
}

struct Win { bool ok=false; uint64_t open=0, close=0; };
Win window(const Result& r) {
    Win w;
    auto a = find_write(r.log, S_A, 0xA200);
    auto b = find_write(r.log, S_B, 0xB200);
    w.ok = a.found && a.count==1 && b.found && b.count==1 && a.cycle < b.cycle;
    w.open = a.cycle; w.close = b.cycle;
    return w;
}

} // namespace p2

// ===========================================================================
// P3 -- prefetch existence (SPECIFICATION.md section 5)
// ===========================================================================
namespace p3 {

struct Result {
    bool loaded=false, released=false, done=false;
    std::vector<nmh::RamAccess> log;
    uint32_t x[5]={0}; // x[1..4]
    uint16_t opcode[5]={0}; // opcode word at x[1..4]
    uint16_t fixture_peek=0;
    ModeSeen seen;
};

Result run(const std::string& bin, uint8_t cpu_config) {
    Result res;
    nmh::Config cfg; cfg.cpu_config = driven_cfg(cpu_config, "P3");
    nmh::NanomigHarness h(cfg);
    res.loaded = h.load_program(bin) > 0;
    if (!res.loaded) return res;
    res.released = h.boot(kBootBudget).cpu_released;
    res.seen.boot = h.ports()->cpu_config;
    auto d = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    res.seen.end = h.ports()->cpu_config;
    res.done = d.has_value();
    res.log = nmh::coalesce(h.ram_log());
    res.fixture_peek = peekw(h, S_START);
    for (int k = 1; k <= 4; ++k) {
        res.x[k] = peekl(h, XTAB3 + 4u*(k-1));
        res.opcode[k] = 0;
    }
    // The opcode word of block k's stop instruction is the one a CPU read at
    // X_k carried inside the run window R (S_START to S_END); no such read
    // leaves it 0, which no opcode family of the table matches.
    auto ws = find_write(res.log, S_START, 0x0001);
    auto we = find_write(res.log, S_END, 0xE0D0);
    if (ws.found && we.found && ws.cycle < we.cycle)
        for (int k = 1; k <= 4; ++k) res.opcode[k] = last_read_between(res.log, res.x[k], ws.cycle, we.cycle).data;
    return res;
}

// P3.d: block k's stop instruction has the opcode family the table names
// (BRA.S $60xx nonzero low byte; JMP (A0) $4ED0; RTS $4E75; BEQ.S $67xx
// nonzero low byte), from the 68000 instruction format.
bool opcode_matches_table(int k, uint16_t op) {
    switch (k) {
        case 1: return (op & 0xFF00) == 0x6000 && (op & 0x00FF) != 0x00; // BRA.S
        case 2: return op == 0x4ED0; // JMP (A0)
        case 3: return op == 0x4E75; // RTS
        case 4: return (op & 0xFF00) == 0x6700 && (op & 0x00FF) != 0x00; // BEQ.S
    }
    return false;
}

struct Win { bool ok=false; uint64_t start=0, end=0; };
Win run_window(const Result& r) {
    Win w;
    auto s = find_write(r.log, S_START, 0x0001);
    auto e = find_write(r.log, S_END, 0xE0D0);
    w.ok = s.found && e.found && s.cycle < e.cycle;
    w.start = s.cycle; w.end = e.cycle;
    return w;
}

} // namespace p3

// ===========================================================================
// P5 -- exception stack frames (SPECIFICATION.md section 7)
// ===========================================================================
namespace p5 {

struct Frame { std::vector<std::pair<int32_t,uint16_t>> words; }; // (addr-PRE_SP, data), last write per word wins

struct Result {
    bool loaded=false, released=false, done=false;
    std::vector<nmh::RamAccess> log;
    uint32_t pre_sp[6]={0};   // e=1..5
    uint32_t post_sp[6]={0};  // e=1..5
    uint16_t fixture_peek=0;
    ModeSeen seen;
};

Result run(const std::string& bin, uint8_t cpu_config) {
    Result res;
    nmh::Config cfg; cfg.cpu_config = driven_cfg(cpu_config, "P5");
    nmh::NanomigHarness h(cfg);
    res.loaded = h.load_program(bin) > 0;
    if (!res.loaded) return res;
    res.released = h.boot(kBootBudget).cpu_released;
    res.seen.boot = h.ports()->cpu_config;
    auto d = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    res.seen.end = h.ports()->cpu_config;
    res.done = d.has_value();
    res.log = nmh::coalesce(h.ram_log());
    res.fixture_peek = peekw(h, S_START);
    for (int e = 1; e <= 5; ++e) {
        res.pre_sp[e] = peekl(h, PRE_SP);   // overwritten each case; last (e=5) value stays, but we re-derive per-e below from the log
        res.post_sp[e] = peekl(h, POST_SP[e]);
    }
    return res;
}

// PRE_SP is a single memory slot rewritten every case (step a); its
// per-case value is recovered from the log itself (the CPU write of
// PRE_SP that precedes case e's own S_A write).
struct Win { bool ok=false; uint64_t a_cycle=0, b_cycle=0; uint32_t pre_sp=0; };
Win case_window(const Result& r, int e) {
    Win w;
    auto a = find_write(r.log, S_A, uint16_t(0xC000+e));
    auto b = find_write(r.log, S_B, uint16_t(0xD000+e));
    w.ok = a.found && a.count==1 && b.found && b.count==1 && a.cycle < b.cycle;
    w.a_cycle = a.cycle; w.b_cycle = b.cycle;
    // PRE_SP: last write to that word before a.cycle.
    uint32_t aw = wa(PRE_SP);
    uint64_t best = 0; bool have=false; uint32_t lo=0, hi=0;
    for (auto& x : r.log) {
        if (x.is_write && x.cycle < a.cycle) {
            if (x.word_addr == aw) { lo = x.data; have = true; best = x.cycle; }
            if (x.word_addr == aw+1 && x.cycle >= best) { hi = x.data; }
        }
    }
    w.pre_sp = (uint32_t(lo) << 16) | hi;
    return w;
}

// P5.a's vector-read clause: inside window e, a CPU read at the long word
// installed as case e's vector (step 2).
int vector_of(int e) { return (e==1)?0x80 : (e==2)?0x10 : (e==3)?0x28 : (e==4)?0x1C : 0x20; }
bool vector_read_ok(const Result& r, const Win& w, int e) {
    return read_count_between(r.log, uint32_t(vector_of(e)), w.a_cycle, w.b_cycle) > 0;
}

// Frame F_e: every CPU write inside (a_cycle,b_cycle), offset from pre_sp,
// last write per word wins.
Frame frame_of(const Result& r, const Win& w) {
    Frame f;
    std::vector<std::pair<uint32_t,uint16_t>> raw; // word_addr(bytes), data, in order
    for (auto& x : r.log) {
        if (x.is_write && x.cycle > w.a_cycle && x.cycle < w.b_cycle) {
            raw.push_back({x.word_addr*2, x.data});
        }
    }
    // last write per byte-address wins
    std::vector<uint32_t> order;
    std::vector<std::pair<uint32_t,uint16_t>> last;
    for (auto& p : raw) {
        bool found=false;
        for (auto& l : last) if (l.first==p.first) { l.second = p.second; found=true; break; }
        if (!found) last.push_back(p);
    }
    for (auto& l : last) f.words.push_back({int32_t(l.first) - int32_t(w.pre_sp), l.second});
    std::sort(f.words.begin(), f.words.end());
    return f;
}

bool frames_differ(const Frame& a, const Frame& b) { return a.words != b.words; }

// P5.c: every CPU write inside W_e lies at an address from POST_SP_e up
// to PRE_SP-2 (offsets, relative to PRE_SP, in [post_sp-pre_sp, -2]).
bool writes_within_stack(const Frame& f, uint32_t pre_sp, uint32_t post_sp) {
    int32_t lo = int32_t(post_sp) - int32_t(pre_sp);
    for (auto& w : f.words) if (w.first < lo || w.first > -2) return false;
    return true;
}

std::string frame_str(const Frame& f) {
    std::string s;
    char buf[32];
    for (auto& w : f.words) { std::snprintf(buf, sizeof(buf), "[%d]=%04X ", w.first, w.second); s += buf; }
    return s;
}

} // namespace p5

// ===========================================================================
// P6 -- prefetch depth (SPECIFICATION.md section 8)
// ===========================================================================
namespace p6 {

struct Result {
    bool loaded=false, released=false, done=false;
    std::vector<nmh::RamAccess> log;
    uint32_t x[9]={0}; // k=1..8
    uint16_t opcode[9]={0}; // opcode word at x[1..8]
    uint16_t fixture_peek=0;
    uint16_t guard_opcode=0; // the guard's own first word, read back from the image
    ModeSeen seen;
};

Result run(const std::string& bin, uint8_t cpu_config, const char* tag = "P6") {
    Result res;
    nmh::Config cfg; cfg.cpu_config = driven_cfg(cpu_config, tag);
    nmh::NanomigHarness h(cfg);
    res.loaded = h.load_program(bin) > 0;
    if (!res.loaded) return res;
    res.released = h.boot(kBootBudget).cpu_released;
    res.seen.boot = h.ports()->cpu_config;
    auto d = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    res.seen.end = h.ports()->cpu_config;
    res.done = d.has_value();
    res.log = nmh::coalesce(h.ram_log());
    res.fixture_peek = peekw(h, S_START);
    for (int k = 1; k <= 8; ++k) res.x[k] = peekl(h, XTAB6 + 4u*(k-1));
    {
        // as in P3: the opcode word a CPU read at X_k carried inside R; 0 when there is none
        auto ws = find_write(res.log, S_START, 0x0001);
        auto we = find_write(res.log, S_END, 0xE0D0);
        for (int k = 1; k <= 8; ++k)
            res.opcode[k] = (ws.found && we.found && ws.cycle < we.cycle)
                                ? last_read_between(res.log, res.x[k], ws.cycle, we.cycle).data : 0;
    }
    res.guard_opcode = peekw(h, res.x[1] + 2);
    return res;
}

// Block k's stop kind, 1..4 (BRA.S, JMP, RTS, BEQ.S), matching p3's table:
// P6 runs each kind twice (k, k+1).
int kind_of(int k) { return (k - 1) / 2 + 1; }

struct Win { bool ok=false; uint64_t start=0, end=0; };
Win run_window(const Result& r) {
    Win w;
    auto s = find_write(r.log, S_START, 0x0001);
    auto e = find_write(r.log, S_END, 0xE0D0);
    w.ok = s.found && e.found && s.cycle < e.cycle;
    w.start = s.cycle; w.end = e.cycle;
    return w;
}

// g_k: distinct guard words read inside the run window, among the
// guard_len words at W_k, W_k+2, .... guard_len is the guard's own
// assembled length (6 normally; SPECIFICATION.md section 8's own reference
// frame), never more: under P6.e's contrast (MUT_E, guard_len=1) the words
// after the single guard word are the next block's real, executed
// instructions, and counting reads there would count execution, not
// prefetch, so g_of counts guard words only.
int g_of(const Result& r, const Win& w, uint32_t x_k, int guard_len) {
    int g = 0;
    for (int i = 0; i < guard_len; ++i) {
        uint32_t addr = x_k + 2 + 2u*i;
        if (read_count_between(r.log, addr, w.start, w.end) > 0) ++g;
    }
    return g;
}

// The guard's own assembled length, taken from the image (not from which
// NMH_MUTATE variant was selected): block 1's guard opcode word is either
// $33FC (MOVE.W #imm,(abs).L -- the 6-word guard) or $4AFC (ILLEGAL -- the
// 1-word P6.e contrast).
int guard_len_of(const Result& r) { return (r.guard_opcode == 0x4AFC) ? 1 : 6; }

// P3.a-P3.d, reused for every P6 run (SPECIFICATION.md section 8: "the
// contrasts are the same as in P3"), graded over all 8 blocks.
struct Precond { bool p3a=true, p3b=true, p3c=true, p3d=true; ScanResult p3c_scan; };
Precond common_precond(const Result& r, const Win& win, const std::string& bin) {
    Precond p;
    if (!win.ok) { p.p3a = p.p3b = p.p3c = p.p3d = false; return p; }
    auto sx = find_write(r.log, S_X, 0xDEAD);
    auto sf = find_write(r.log, S_FAIL, 0xFA11);
    p.p3b = sx.count == 0 && sf.count == 0;
    p.p3a = markers_in_order(r.log, 8);
    for (int k = 1; k <= 8; ++k) p.p3d = p.p3d && p3::opcode_matches_table(kind_of(k), r.opcode[k]);
    p.p3c_scan = p3c_static(bin, r.x, 8);
    p.p3c = p.p3c_scan.ok();
    return p;
}

// P6's own alignment precondition: within each pair (blocks 2i-1,2i), the
// two copies' stop instructions lie at addresses of different parity mod 4
// (SPECIFICATION.md section 8: "In one copy, X_k lies at an address
// divisible by 4; in the other, at an address that leaves remainder 2").
bool alignment_ok(const Result& r) {
    for (int pair = 0; pair < 4; ++pair) {
        uint32_t xa = r.x[2*pair+1], xb = r.x[2*pair+2];
        if (xa % 4 == xb % 4) return false;
    }
    return true;
}

} // namespace p6

// ===========================================================================
// P7 -- MOVE SR,<dest> acts differently (SPECIFICATION.md section 9)
// ===========================================================================
namespace p7 {

// One entry of the port's record of a window: the time since the window
// opened, the word address, the data and the direction.
struct Entry {
    uint64_t rel; uint32_t addr; uint16_t data; bool wr;
    bool operator==(const Entry& o) const { return rel == o.rel && addr == o.addr && data == o.data && wr == o.wr; }
};

struct Result {
    bool loaded=false, released=false, done=false;
    std::vector<nmh::RamAccess> log;
    uint32_t xw = 0, xm = 0;                       // X_w, X_m from XTAB7
    uint16_t dest = 0, mrk_priv = 0, mrk_trap = 0; // result words after the run
    uint16_t fixture_peek = 0;
    ModeSeen seen;
};

Result run(const std::string& bin, uint8_t cpu_config) {
    Result res;
    nmh::Config cfg; cfg.cpu_config = driven_cfg(cpu_config, "P7");
    nmh::NanomigHarness h(cfg);
    res.loaded = h.load_program(bin) > 0;
    if (!res.loaded) return res;
    res.released = h.boot(kBootBudget).cpu_released;
    res.seen.boot = h.ports()->cpu_config;
    auto d = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    res.seen.end = h.ports()->cpu_config;
    res.done = d.has_value();
    res.log = nmh::coalesce(h.ram_log());
    res.fixture_peek = peekw(h, S_START);
    res.xw = peekl(h, XTAB7);
    res.xm = peekl(h, XTAB7 + 4);
    res.dest = peekw(h, DEST);
    res.mrk_priv = peekw(h, MRK_PRIV);
    res.mrk_trap = peekw(h, MRK_TRAP);
    return res;
}

// A window bounded by one CPU write of `open_val` to S_A and one of `close_val` to S_B.
struct Win { bool ok = false; uint64_t open = 0, close = 0; };
Win window(const Result& r, uint16_t open_val, uint16_t close_val) {
    Win w;
    auto a = find_write(r.log, S_A, open_val);
    auto b = find_write(r.log, S_B, close_val);
    w.ok = a.found && a.count == 1 && b.found && b.count == 1 && a.cycle < b.cycle;
    w.open = a.cycle; w.close = b.cycle;
    return w;
}

std::vector<Entry> sequence(const Result& r, const Win& w) {
    std::vector<Entry> q;
    for (const auto& x : r.log)
        if (x.cycle > w.open && x.cycle < w.close) q.push_back({x.cycle - w.open, x.word_addr, x.data, x.is_write});
    return q;
}

std::string writes_str(const Result& r, const Win& w) {
    std::string out;
    char b[48];
    for (const auto& x : r.log)
        if (x.is_write && x.cycle > w.open && x.cycle < w.close) {
            std::snprintf(b, sizeof b, "$%06X=%04X ", x.word_addr * 2u, x.data);
            out += b;
        }
    return out;
}

// The first entry at which two sequences differ, as text.
std::string first_diff(const std::vector<Entry>& a, const std::vector<Entry>& b) {
    size_t n = std::min(a.size(), b.size());
    for (size_t i = 0; i < n; ++i)
        if (!(a[i] == b[i])) return "entry " + std::to_string(i) + " of " + std::to_string(a.size()) + " and " + std::to_string(b.size());
    if (a.size() != b.size()) return "length " + std::to_string(a.size()) + " against " + std::to_string(b.size());
    return "none: equal, " + std::to_string(a.size()) + " entries";
}

} // namespace p7

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const bool is_tg68k = std::string(NMH_XSTR(NANOMIG_CORE_NAME)) == "tg68k";
    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";
    g_mutate = mv;

    const std::string core_label = is_tg68k ? "second core" : "first core";

    // C5 (SPECIFICATION.md 2.5): every run asserts that ports()->cpu_config shows
    // the value the test requested for it. That proves the input, not the
    // core's internal mode.
    checks.record("C5 note: ports()->cpu_config shows the requested input of each run (graded per run below); "
                  "it proves the input, not the mode the core runs in; the core label of every line is the "
                  "build's core and the mode label is the requested cpu_config value");

    // =======================================================================
    // P1 [p13] opus:1-guidelines.c60,sol:1-guidelines.c60: 68000 CLR reads
    // then writes. Graded on the first core, and the second core in 68000
    // mode (always cpu_config=0: a no-op on the first core's build).
    // =======================================================================
    {
        // Every precondition's mutant is a full copy of p1.bin with one
        // thing changed; the SAME capture and the SAME labels below are
        // used for the normal run and for every mutation -- the mutant
        // input makes its own clause go red through the unchanged
        // comparator, exactly as P1-MOVE already does for the main relation.
        std::string bin = dir + "/p1.bin";
        if (mv == "P1-MOVE") bin = dir + "/p1_move.bin";
        else if (mv == "P1C-MOVE") bin = dir + "/p1_move_mutc.bin";
        else if (mv == "P1C-KIND") bin = dir + "/p1_mutkind.bin";
        else if (mv == "P1A") bin = dir + "/p1_muta.bin";
        else if (mv == "P1A-ORD") bin = dir + "/p1_mutord.bin";
        else if (mv == "P1B") bin = dir + "/p1_mutb.bin";
        else if (mv == "P1C") bin = dir + "/p1_mutc.bin";
        bin = pick_macro_variant(mv, "P1", dir, "p1", bin);
        auto r = p1::run(bin, 0x00);
        bool fixture_ok = r.loaded && r.released && r.done;
        checks.expect(fixture_ok, "P1 fixture: p1.bin loaded, released and reached DONE on the " + core_label);
        bool peek_ok = r.fixture_peek == 0x0001;
        checks.expect(peek_ok, "P1 fixture: S_START reads back $0001 through peek_word, before any other "
                                "peeked value is trusted (" + core_label + ")");

        bool c1_ok = check_c1(r.log, r.r_int);
        bool c2_ok = check_c2(r.log, r.r_dma);
        bool c3_ok = check_c3(r.log);
        checks.expect(c1_ok, "C1 [p280]: interrupts off, R_INT bits 14-0 all zero (" + core_label + ")");
        checks.expect(c2_ok, "C2 [p278]: DMA off, R_DMA bits 10-0 all zero (" + core_label + ")");
        checks.expect(c3_ok, "C3: S_END written exactly once, S_FAIL never written (" + core_label + ")");
        bool c4_ok = c4_line(checks, r.log, pop_p1(), core_label);
        bool c5_ok = c5_line(checks, r.seen, 0x00, core_label);

        // The oracle (relation_ok with expect_single_write=false: read then
        // write) never changes for the mutation: under P1-MOVE, MOVE.x #0
        // makes no read at all, so this comparator goes red on its own,
        // through the exact same check as the normal run.
        // The expected opcode table is the one the program names: p1.s stores
        // a table marker (S_KIND) once, in its setup, and the test reads it
        // from that logged write, never from which variant was selected.
        const auto kind = first_write(r.log, S_KIND);
        const bool kind_clr = kind.found && kind.data == 0xC1A0;
        const bool kind_move = kind.found && kind.data == 0xC1A1;
        bool p1a_all = true, p1b_all = true, p1c_all = true, rel_all = true;
        // P1.a: each case's window is one ordered S_A/S_B pair, and the 21
        // windows follow in case order without overlap: open[c] < close[c] <
        // open[c+1] for every adjacent pair.
        uint64_t prev_close = 0;
        bool have_prev = false;
        for (auto& cs : p1::kCases) {
            auto w = p1::window(r, cs.c);
            p1a_all = p1a_all && w.ok && (!have_prev || prev_close < w.open);
            prev_close = w.close;
            have_prev = true;
            p1b_all = p1b_all && p1::precond_b_ok(r, cs);
            const uint16_t expected_op = kind_move ? p1::expected_move_opcode(cs.c) : p1::expected_clr_opcode(cs.c);
            p1c_all = p1c_all && (kind_clr || kind_move) && r.opcode[cs.c] == expected_op;
            rel_all = rel_all && w.ok && p1::relation_ok(r, cs, w, false);
        }
        checks.expect(p1a_all, "P1.a: every case's window is a single, ordered S_A/S_B pair, and the 21 windows follow "
                                "in case order without overlap (" + core_label + ")");
        checks.expect(p1b_all, "P1.b: every case's operand held the prefill before W_c and the readback (the CPU "
                                "write to R_c) matches the prefill/clear pattern (" + core_label + ")");
        if (!kind_move) {
            checks.expect(p1c_all, "P1.c: every case's last port read at X_c carries the opcode the table names; an address and data anchor, the record names no requester (" + core_label + ")");
        } else {
            // A program whose table names MOVE is graded against the MOVE
            // opcode assembled in place of CLR: were the Makefile's -m68010
            // to keep assembling CLR here, this line would be red.
            checks.expect(p1c_all, "P1.c (P1-MOVE contrast): every case's last port read at X_c carries the MOVE "
                                    "opcode assembled in place of CLR (" + core_label + ")");
        }

        bool p1_precond_ok = fixture_ok && peek_ok && p1a_all && p1b_all && p1c_all && c1_ok && c2_ok && c3_ok && c4_ok && c5_ok;
        checks.expect_gated(p1_precond_ok, rel_all,
            "P1 [p13,p263] opus:1-guidelines.c60,sol:1-guidelines.c60: on the 68000, every CLR with a "
            "memory operand makes a read access before the write that clears it, in all 21 cases (" + core_label + ")");
        if (kind_move) {
            auto w1 = p1::window(r, 1);
            auto acc1 = accesses_between(r.log, T_BASE+16, w1.open, w1.close);
            std::string s;
            for (auto& a : acc1) s += (a.is_write?"W":"R") + std::string("@") + std::to_string(a.cycle) + "=" + std::to_string(a.data) + " ";
            bool any_read = false; for (auto& a : acc1) if (!a.is_write) any_read = true;
            checks.record("P1-MOVE: measured (case 1, .B (An)) " + s + "-- MOVE.B #0,(a0) makes " +
                          std::string(any_read ? "a read then a write, the same pattern as CLR" : "a single write, no read (the page's own contrast)") +
                          "; P1's read-then-write comparator is graded through the unchanged relation above, and "
                          "goes " + std::string(any_read ? "green" : "red") + " here for that reason");
        }

        // ---- P4 [p13,p263]: on the 68020, CLR makes a single write access.
        // Second core, cpu_config=68020 only, P1's program on its own run.
        // The relation is graded on the byte and word cases with both c59
        // ids; the long-word cases get a line of their own without ids.
        // NMH_MUTATE: P4-68000 requests and drives cpu_config 00 (the equal
        // control), P4-MOVE loads P1's MOVE variant under 11 (the live
        // comparator), C5-P4 drives 00 while 11 is requested.
        if (is_tg68k) {
            const uint8_t cfg4 = (mv == "P4-68000") ? 0x00 : 0x03;
            std::string bin4 = dir + "/p1.bin";
            if (mv == "P4-MOVE") bin4 = dir + "/p1_move.bin";
            else if (mv == "P4-P1A") bin4 = dir + "/p1_muta.bin";
            else if (mv == "P4-P1B") bin4 = dir + "/p1_mutb.bin";
            else if (mv == "P4-P1C") bin4 = dir + "/p1_mutc.bin";
            bin4 = pick_macro_variant(mv, "P4", dir, "p1", bin4);
            auto r4 = p1::run(bin4, cfg4, "P4");
            const std::string run4 = "P4 run, second core, " + mode_name(cfg4);
            bool fixture4_ok = r4.loaded && r4.released && r4.done;
            checks.expect(fixture4_ok, "P4 fixture: p1.bin loaded, released and reached DONE (second core, " + mode_name(cfg4) + ")");
            bool peek4_ok = r4.fixture_peek == 0x0001;
            checks.expect(peek4_ok, "P4 fixture: S_START reads back $0001 through peek_word, before any other peeked "
                                    "value is trusted (second core, " + mode_name(cfg4) + ")");
            uint16_t ri4 = 0, rd4 = 0;
            bool c1_4 = check_c1(r4.log, ri4), c2_4 = check_c2(r4.log, rd4), c3_4 = check_c3(r4.log);
            checks.expect(c1_4, "C1 [p280]: interrupts off (" + run4 + ")");
            checks.expect(c2_4, "C2 [p278]: DMA off (" + run4 + ")");
            checks.expect(c3_4, "C3: S_END written exactly once, S_FAIL never written (" + run4 + ")");
            bool c4_4 = c4_line(checks, r4.log, pop_p1(), run4);
            bool c5_4 = c5_line(checks, r4.seen, cfg4, run4);

            // P1.a to P1.c on the P4 run's own log (SPECIFICATION.md section 6).
            const auto kind4 = first_write(r4.log, S_KIND);
            const bool kind4_clr = kind4.found && kind4.data == 0xC1A0;
            const bool kind4_move = kind4.found && kind4.data == 0xC1A1;
            bool p1a4 = true, p1b4 = true, p1c4 = true, rel_bw = true, rel_l = true;
            int n_bw = 0, n_l = 0;
            uint64_t prev_close4 = 0;
            bool have_prev4 = false;
            for (auto& cs : p1::kCases) {
                auto w = p1::window(r4, cs.c);
                p1a4 = p1a4 && w.ok && (!have_prev4 || prev_close4 < w.open);
                prev_close4 = w.close;
                have_prev4 = true;
                p1b4 = p1b4 && p1::precond_b_ok(r4, cs);
                const uint16_t expected_op = kind4_move ? p1::expected_move_opcode(cs.c) : p1::expected_clr_opcode(cs.c);
                p1c4 = p1c4 && (kind4_clr || kind4_move) && r4.opcode[cs.c] == expected_op;
                const bool one = w.ok && p1::relation_ok(r4, cs, w, true);
                if (cs.size == 'L') { rel_l = rel_l && one; if (one) ++n_l; }
                else { rel_bw = rel_bw && one; if (one) ++n_bw; }
            }
            checks.expect(p1a4, "P1.a (P4 run): every case's window is a single, ordered S_A/S_B pair, and the 21 windows follow "
                                "in case order without overlap (" + run4 + ")");
            checks.expect(p1b4, "P1.b (P4 run): every case's operand held the prefill before W_c and the readback matches the "
                                "prefill/clear pattern (" + run4 + ")");
            checks.expect(p1c4, "P1.c (P4 run): every case's last port read at X_c carries the opcode the table names; an "
                                "address and data anchor, the record names no requester (" + run4 + ")");

            const bool p4_pre = fixture4_ok && peek4_ok && c1_4 && c2_4 && c3_4 && c4_4 && c5_4 && p1a4 && p1b4 && p1c4;
            checks.expect_gated(p4_pre, rel_bw,
                "P4 [p13,p263] opus:1-guidelines.c59,sol:1-guidelines.c59: on the 68020, every CLR.B and CLR.W with a "
                "memory operand makes exactly one access at the operand's word, a write, in all 14 cases (second core, " +
                mode_name(cfg4) + ")");
            checks.expect_gated(p4_pre, rel_l,
                "P4.L [p13]: on the 68020, every CLR.L with a memory operand makes no read at the operand's two words and "
                "at least one write to each, in all 7 cases; this is not the claim of a single Write access (second core, " +
                mode_name(cfg4) + ")");

            auto w4_1 = p1::window(r4, 1);
            auto acc4_1 = accesses_between(r4.log, T_BASE+16, w4_1.open, w4_1.close);
            std::string s4;
            for (auto& a : acc4_1) s4 += (a.is_write?"W":"R") + std::string("@") + std::to_string(a.cycle) + "=" + std::to_string(a.data) + " ";
            checks.record("P4 measured (second core, " + mode_name(cfg4) + "): " + std::to_string(n_bw) + "/14 byte and word "
                "cases showed exactly one access at the operand, a write; " + std::to_string(n_l) + "/7 long-word cases "
                "showed no read and a write to both words; case 1 (.B (An)): " + s4 + "; this run's RAM-port log against "
                "P1's own run under cpu_config=00 (same program): " + compare_logs(r.log, r4.log) +
                (kind4_move ? " (this run loaded P1's MOVE variant, so the comparison is between two programs)" : ""));
        } else {
            checks.record("P4 [p13]: the first core has no 68020 mode; not run on this core (SPECIFICATION.md section 6)");
        }

    }

    // =======================================================================
    // P2 [p1,p2]: the 68000's data and address registers, and USP, hold 32
    // bits. Graded on the first core and the second core in 68000 mode.
    // =======================================================================
    {
        bool is_low = (mv == "P2-LOW");
        std::string bin = dir + "/p2.bin";
        if (is_low) bin = dir + "/p2_low.bin";
        else if (mv == "P2A") bin = dir + "/p2_muta.bin";
        else if (mv == "P2B") bin = dir + "/p2_mutb.bin";
        else if (mv == "P2C") bin = dir + "/p2_mutc.bin";
        else if (mv == "P2C-RD") bin = dir + "/p2_mutrd.bin";
        else if (mv == "P2C-VEC") bin = dir + "/p2_mutvec.bin";
        bin = pick_macro_variant(mv, "P2", dir, "p2", bin);
        auto r = p2::run(bin, 0x00);
        bool fixture_ok = r.loaded && r.released && r.done;
        checks.expect(fixture_ok, "P2 fixture: p2.bin loaded, released and reached DONE on the " + core_label);
        bool peek_ok = r.fixture_peek == 0x0001;
        checks.expect(peek_ok, "P2 fixture: S_START reads back $0001 through peek_word, before any other "
                                "peeked value is trusted (" + core_label + ")");

        uint16_t ri=0, rd=0;
        bool c1_ok = check_c1(r.log, ri), c2_ok = check_c2(r.log, rd);
        bool c3_ok = check_c3(r.log);
        checks.expect(c1_ok, "C1 [p280]: interrupts off (P2 run, " + core_label + ")");
        checks.expect(c2_ok, "C2 [p278]: DMA off (P2 run, " + core_label + ")");
        checks.expect(c3_ok, "C3: S_END written exactly once, S_FAIL never written (P2 run, " + core_label + ")");
        bool c4_ok = c4_line(checks, r.log, pop_p2(), "P2 run, " + core_label);
        bool c5_ok = c5_line(checks, r.seen, 0x00, "P2 run, " + core_label);

        auto w = p2::window(r);
        checks.expect(w.ok, "P2.a: exactly one ordered S_A/S_B pair (" + core_label + ")");

        // P2.b: exactly one CPU write at each of the 34 words M_r/M_r+2
        // inside W, for all 17 registers.
        bool p2b = true;
        for (int i = 0; i < 17; ++i) {
            uint32_t addr = (i < 16) ? (M_BASE + 4u*i) : M_USP;
            auto lo = accesses_between(r.log, addr, w.open, w.close);
            auto hi = accesses_between(r.log, addr+2, w.open, w.close);
            int lo_w=0, hi_w=0; for (auto&a:lo) if(a.is_write) ++lo_w; for (auto&a:hi) if(a.is_write) ++hi_w;
            if (lo_w != 1 || hi_w != 1) p2b = false;
        }
        checks.expect(p2b, "P2.b: every one of the 34 store words was written exactly once inside the window (" + core_label + ")");

        // P2.c: every CPU access inside W is one of: a read inside the
        // program image, a read of SAVE_SP (step 6), a write to one of the 34
        // store words, or the S_B write. An exception's stacking writes, its
        // vector read and any other data read fall outside this list. Reads of word 0 are not
        // covered: the baseline log shows four reads of word 0 inside W, 16
        // clk_sys apart, that the program does not issue, and a longer run
        // repeats them every 1816 clk_sys.
        bool p2c = true;
        std::string p2c_why;
        {
            std::vector<uint32_t> allowed_writes;
            for (int i=0;i<16;++i) { allowed_writes.push_back(wa(M_BASE+4u*i)); allowed_writes.push_back(wa(M_BASE+4u*i+2)); }
            allowed_writes.push_back(wa(M_USP)); allowed_writes.push_back(wa(M_USP+2));
            allowed_writes.push_back(wa(S_B));
            const uint32_t image_lo = wa(kProgramOrg);
            const uint32_t image_hi = wa(kProgramOrg + uint32_t(read_file_bytes(bin).size()));
            // the image is whatever the run loaded; an empty image allows no read
            for (auto& x : r.log) {
                if (!(x.cycle > w.open && x.cycle < w.close)) continue;
                if (x.is_write) {
                    if (std::find(allowed_writes.begin(), allowed_writes.end(), x.word_addr) == allowed_writes.end()) {
                        if (p2c) p2c_why = "write at byte address " + std::to_string(x.word_addr * 2u) + " cycle " + std::to_string(x.cycle);
                        p2c = false;
                    }
                } else {
                    const bool in_image = x.word_addr >= image_lo && x.word_addr < image_hi;
                    const bool save_sp = x.word_addr == wa(SAVE_SP) || x.word_addr == wa(SAVE_SP + 2);
                    // Word 0 is not covered: it is read inside W by a requester
                    // the program does not control (the record names none).
                    const bool word0 = x.word_addr == 0;
                    if (!in_image && !save_sp && !word0) {
                        if (p2c) p2c_why = "read at byte address " + std::to_string(x.word_addr * 2u) + " cycle " + std::to_string(x.cycle);
                        p2c = false;
                    }
                }
            }
        }
        checks.expect(p2c, "P2.c: every CPU access inside the window is a program-image read, a read of SAVE_SP, "
                           "a write to one of the 17 store slots or the S_B write; reads of word 0 are not covered (" + core_label + ")");
        if (!p2c) checks.note("P2.c first access outside the list (" + core_label + "): " + p2c_why);

        // The oracle is always the FULL pattern (kPatterns[i]), never
        // adjusted for the mutant: under P2-LOW the register only ever
        // held the lower half, so this must -- and does -- go red on its
        // own, through the same comparator as the normal run.
        bool rel = true;
        for (int i = 0; i < 17; ++i) {
            uint32_t pat = p2::kPatterns[i];
            uint32_t got = r.m[i];
            // order of halves is not graded (2.2's own note); the pair
            // {hi,lo} must equal the pattern's two halves in some order.
            bool halves_ok = (uint16_t(got>>16)==uint16_t(pat>>16) && uint16_t(got)==uint16_t(pat)) ||
                              (uint16_t(got>>16)==uint16_t(pat) && uint16_t(got)==uint16_t(pat>>16));
            rel = rel && halves_ok;
        }
        bool p2_precond_ok = fixture_ok && peek_ok && w.ok && p2b && p2c && c1_ok && c2_ok && c3_ok && c4_ok && c5_ok;
        checks.expect_gated(p2_precond_ok, rel,
            "P2 [p1,p2]: every one of the 17 registers (D0-D7,A0-A7,USP) stores back all 32 bits of its "
            "loaded pattern (" + core_label + ")");
        // opus/sol:1-components.c25 ("while the data bus and ALU are 16
        // bits") and opus/sol:1-components.c6 (processor identity) bundle a
        // clause this package does not test (SPECIFICATION.md section 1):
        // withdrawn whole, not printed on the graded line above.
        checks.record("opus:1-components.c25,sol:1-components.c25,opus:1-components.c6,sol:1-components.c6 "
                       "withdrawn (P2 run, " + core_label + "): bundle a clause not tested at this port -- the "
                       "data-bus/ALU width (c25) and the processor's identity (c6); see SPECIFICATION.md section 1");
        if (is_low) {
            char buf[96];
            std::snprintf(buf, sizeof(buf), "D0 observed {hi,lo}={$%04X,$%04X}, pattern {$%04X,$%04X}",
                          uint16_t(r.m[0]>>16), uint16_t(r.m[0]), uint16_t(p2::kPatterns[0]>>16), uint16_t(p2::kPatterns[0]));
            checks.note(std::string("P2-LOW: ") + buf + " -- compared against the SAME full-pattern oracle "
                        "above, which is why the assertion goes red under this mutation");
        }
    }

    // =======================================================================
    // P3 [p13]: a read at a guard word, which no executed instruction or
    // data access explains, occurs inside the run: every family member reads
    // instruction words ahead of the one it executes. Graded on the first
    // core, and the second core in 68000/68010/68020 mode, as the package's
    // property: the RAM-port log carries no requester, so the graded line
    // carries no claim id (opus/sol:1-guidelines.c65 is recorded below, and
    // c64, "All 68000 family processors", is withdrawn: only three of five
    // members are available).
    // =======================================================================
    {
        struct ModeRun { std::string label; uint8_t cfg; bool applicable; };
        std::vector<ModeRun> modes;
        if (is_tg68k) {
            modes = {{"second core, 68000 mode", 0x00, true},
                      {"second core, 68010 mode", 0x01, true},
                      {"second core, 68020 mode", 0x03, true}};
        } else {
            modes = {{"first core", 0x00, true}};
        }

        std::vector<nmh::RamAccess> log_mode0;
        for (auto& m : modes) {
            // P3's mutants are 68000 instruction sets, so every mode loads
            // the selected image and reuses every label below: the mutation
            // makes its own clause go red through the unchanged comparator.
            std::string bin = dir + "/p3.bin";
            if (mv == "P3A") bin = dir + "/p3_muta.bin";
            else if (mv == "P3A-ORD") bin = dir + "/p3_mutord.bin";
            else if (mv == "P3B") bin = dir + "/p3_mutb.bin";
            else if (mv == "P3C") bin = dir + "/p3_mutc.bin";
            else if (mv == "P3C-PC") bin = dir + "/p3_mutcpc.bin";
            else if (mv == "P3C-REG") bin = dir + "/p3_mutcreg.bin";
            else if (mv == "P3D") bin = dir + "/p3_mutd.bin";
            else if (mv == "P3C-IDX") bin = dir + "/p3_mutcidx.bin";
            else if (mv == "P3C-OP") bin = dir + "/p3_mutcop.bin";
            else if (mv == "P3C-UNSUP") bin = dir + "/p3_mutcunsup.bin";
            else if (mv == "P3-CAL") bin = dir + "/p3_mutcal.bin";
            else if (mv == "P3-WIN") bin = dir + "/p3_mutwin.bin";
            bin = pick_macro_variant(mv, "P3", dir, "p3", bin);
            auto r = p3::run(bin, m.cfg);
            bool fixture_ok = r.loaded && r.released && r.done;
            checks.expect(fixture_ok, "P3 fixture: p3.bin loaded, released and reached DONE (" + m.label + ")");
            bool peek_ok = r.fixture_peek == 0x0001;
            checks.expect(peek_ok, "P3 fixture: S_START reads back $0001 through peek_word, before any other "
                                    "peeked value is trusted (" + m.label + ")");

            auto win = p3::run_window(r);
            uint16_t ri=0, rd=0;
            bool c1_ok = check_c1(r.log, ri), c2_ok = check_c2(r.log, rd);
            bool c3_ok = check_c3(r.log);
            checks.expect(c1_ok, "C1 [p280]: interrupts off (P3 run, " + m.label + ")");
            checks.expect(c2_ok, "C2 [p278]: DMA off (P3 run, " + m.label + ")");
            checks.expect(c3_ok, "C3: S_END written exactly once, S_FAIL never written (P3 run, " + m.label + ")");
            bool c4_ok = c4_line(checks, r.log, pop_p3(), "P3 run, " + m.label);
            bool c5_ok = c5_line(checks, r.seen, m.cfg, "P3 run, " + m.label);

            bool p3a = markers_in_order(r.log, 4), p3b = true, p3d = true;
            int reads_at_w[5] = {0,0,0,0,0};
            for (int k = 1; k <= 4; ++k) {
                p3d = p3d && p3::opcode_matches_table(k, r.opcode[k]);
                uint32_t Wk = r.x[k] + 2;
                int n = win.ok ? read_count_between(r.log, Wk, win.start, win.end) : 0;
                reads_at_w[k] = n;
                if (win.ok) {
                    auto sx = find_write(r.log, S_X, 0xDEAD);
                    auto sf = find_write(r.log, S_FAIL, 0xFA11);
                    p3b = p3b && sx.count==0 && sf.count==0;
                }
            }
            checks.expect(p3a, "P3.a: every block's stop instruction executed and transferred control: exactly one S_P "
                                "and one S_L write per block, in the order P1,L1,P2,L2,... (" + m.label + ")");
            checks.expect(p3b, "P3.b: no guard word was executed, S_X and S_FAIL never written (" + m.label + ")");
            checks.expect(p3d, "P3.d: for every block, a port read at X_k inside the run window carries the opcode family the table names; "
                               "an address and data anchor, the record names no requester (" + m.label + ")");

            // P3.c: a static check on the program's assembler listing, which
            // must reproduce the loaded image byte for byte: no instruction's
            // effective-address path (absolute, PC-relative or through an
            // address register) accesses data in W_k to W_k+11. C2 holds DMA
            // off, above. The RAM-port log carries no requester, so which
            // requester made a read at the port is outside this check.
            bool p3c = p3c_line(checks, p3c_static(bin, r.x, 4),
                                "P3.c: no instruction of the assembled program accesses data in a guard word range "
                                "W_k to W_k+11, on any effective-address path (" + m.label + ")");

            int reads_at_z = win.ok ? read_count_between(r.log, FARZ, win.start, win.end) : -1;
            checks.record("P3 measured reads per guard word W_1..W_4 (" + m.label + "): " +
                std::to_string(reads_at_w[1]) + "," + std::to_string(reads_at_w[2]) + "," +
                std::to_string(reads_at_w[3]) + "," + std::to_string(reads_at_w[4]) +
                "; far word Z reads: " + std::to_string(reads_at_z) + " (calibration, not a page prediction)");

            if (m.cfg == 0x00) log_mode0 = r.log;
            else checks.record("P3 RAM-port log (" + m.label + ") against the same run under cpu_config=0 (second core, "
                               "68000 mode): " + compare_logs(log_mode0, r.log));

            bool any_read_somewhere = (reads_at_w[1]>0)||(reads_at_w[2]>0)||(reads_at_w[3]>0)||(reads_at_w[4]>0);
            bool p3_precond_ok = fixture_ok && peek_ok && win.ok && p3a && p3b && p3c && p3d &&
                                 c1_ok && c2_ok && c3_ok && c4_ok && c5_ok;
            checks.expect_gated(p3_precond_ok, any_read_somewhere,
                "P3 [p13]: at least one read at a guard word W_k inside the run, which no executed instruction "
                "and no data access explains (the port log has no requester field) (" + m.label + ")");
            checks.expect(reads_at_z == 0,
                "P3 calibration: the far word Z (never referenced, " + std::to_string(FARZ) +
                ") shows zero reads, so the counter can report absence (" + m.label + ")");
        }
        checks.record("opus:1-guidelines.c65,sol:1-guidelines.c65 recorded (P3): the RAM-port log carries no "
                       "requester, so a CPU instruction fetch cannot be told from another read at the port; "
                       "P3's graded line counts reads at guard words and is not the claim that the CPU loads "
                       "instructions ahead");
        checks.record("opus:1-guidelines.c64,sol:1-guidelines.c64 withdrawn (P3): \"All 68000 family processors\" "
                       "-- the 68000, 68010 and 68020 are graded above; the 68030 and 68040 are not available "
                       "(SPECIFICATION.md section 1)");
    }

    // =======================================================================
    // P5 [p13]: the exception stack frame differs between 68000-family
    // members. (opus/sol:1-guidelines.c52 is withdrawn below: universal
    // over "each member" with only three of five available; the 68010/68020
    // labels are cpu_config values and nothing at the ports shows which
    // setting a run used -- see C5.)
    // =======================================================================
    {
        std::string bin0 = dir + "/p5_68000.bin";
        if (mv == "P5A") bin0 = dir + "/p5_muta.bin";
        else if (mv == "P5C") bin0 = dir + "/p5_mutc.bin";
        bin0 = pick_macro_variant(mv, "P5", dir, "p5", bin0);
        // The pair contrasts of the second core run the 68000-mode image under
        // another member's cpu_config value (the images stay the genuine ones).
        uint8_t cfg0 = 0x00;
        if (is_tg68k && mv == "P5-PAIR-0010") cfg0 = 0x01;
        else if (is_tg68k && mv == "P5-PAIR-0020") cfg0 = 0x03;
        auto r0 = p5::run(bin0, cfg0);
        bool fixture0_ok = r0.loaded && r0.released && r0.done;
        checks.expect(fixture0_ok, "P5 fixture: the 68000-image loaded, released and reached DONE (" + core_label + ", " + mode_name(cfg0) + ")");
        bool peek0_ok = r0.fixture_peek == 0x0001;
        checks.expect(peek0_ok, "P5 fixture: S_START reads back $0001 through peek_word (" + core_label + ", " + mode_name(cfg0) + ")");
        uint16_t ri0=0, rd0=0;
        bool c1_0 = check_c1(r0.log, ri0), c2_0 = check_c2(r0.log, rd0);
        bool c3_0 = check_c3(r0.log);
        checks.expect(c1_0, "C1 [p280]: interrupts off (P5 run, " + core_label + ", " + mode_name(cfg0) + ")");
        checks.expect(c2_0, "C2 [p278]: DMA off (P5 run, " + core_label + ", " + mode_name(cfg0) + ")");
        checks.expect(c3_0, "C3: S_END written exactly once, S_FAIL never written (P5 run, " + core_label + ", " + mode_name(cfg0) + ")");
        bool c4_0 = c4_line(checks, r0.log, pop_p5(), "P5 run, " + core_label + ", " + mode_name(cfg0));
        bool c5_0 = c5_line(checks, r0.seen, cfg0, "P5 run, " + core_label + ", " + mode_name(cfg0));

        p5::Frame frames0[6];
        uint32_t pre_sp0[6] = {0};
        auto sfail0 = find_write(r0.log, S_FAIL, 0xFA11);
        bool p5a0 = sfail0.count == 0, p5c_normal = true;
        for (int e = 1; e <= 5; ++e) {
            auto w = p5::case_window(r0, e);
            p5a0 = p5a0 && w.ok;
            if (w.ok) {
                p5a0 = p5a0 && p5::vector_read_ok(r0, w, e);
                pre_sp0[e] = w.pre_sp;
                frames0[e] = p5::frame_of(r0, w);
                p5c_normal = p5c_normal && p5::writes_within_stack(frames0[e], w.pre_sp, r0.post_sp[e]);
            } else {
                p5c_normal = false;
            }
        }
        checks.expect_gated(fixture0_ok, p5a0, "P5.a: every case was taken through its installed vector, no write to "
                                               "S_FAIL (" + mode_name(cfg0) + ", " + core_label + ")");
        checks.expect_gated(fixture0_ok, p5c_normal,
            "P5.c: every CPU write inside each case's window lies between POST_SP_e and PRE_SP-2 (" + mode_name(cfg0) + ", " + core_label + ")");
        for (int e=1;e<=5;++e) checks.record("P5 F_" + std::to_string(e) + " (" + mode_name(cfg0) + ", " + core_label + "): " + p5::frame_str(frames0[e]));

        // ---- 68010/68020 runs: only meaningful on the second core ----
        p5::Frame frames1[6], frames2[6];
        bool have_00 = fixture0_ok && peek0_ok && p5a0 && p5c_normal && c1_0 && c2_0 && c3_0 && c4_0 && c5_0;
        bool have_10 = false, have_20 = false;
        if (is_tg68k) {
            // The 68010 and 68020 runs load the VBR image (the 68010 run's
            // image is the P5B contrast's own). The pair contrasts run one
            // of the two in another mode of the same instruction set, so
            // that two of the three runs write the same frames.
            std::string vbrbin = dir + "/p5_vbr.bin";
            std::string vbr20bin = dir + "/p5_vbr.bin";
            if (mv == "P5B") vbrbin = dir + "/p5_mutb.bin";
            else if (mv == "P5B-SPAN") vbrbin = dir + "/p5_mutspan.bin";
            else if (mv == "P5A") { vbrbin = dir + "/p5_vbr_muta.bin"; vbr20bin = vbrbin; }
            else if (mv == "P5C") { vbrbin = dir + "/p5_vbr_mutc.bin"; vbr20bin = vbrbin; }
            vbrbin = pick_macro_variant(mv, "P5", dir, "p5_vbr", vbrbin);
            vbr20bin = pick_macro_variant(mv, "P5", dir, "p5_vbr", vbr20bin);
            uint8_t cfg1 = 0x01, cfg2 = 0x03;
            if (mv == "P5-PAIR-1020") cfg1 = 0x03;
            auto r1 = p5::run(vbrbin, cfg1);
            auto r2 = p5::run(vbr20bin, cfg2);
            bool fixture1_ok = r1.loaded && r1.released && r1.done;
            bool fixture2_ok = r2.loaded && r2.released && r2.done;
            checks.expect(fixture1_ok, "P5 fixture: the VBR image loaded, released and reached DONE (second core, " + mode_name(cfg1) + ")");
            checks.expect(fixture2_ok, "P5 fixture: the VBR image loaded, released and reached DONE (second core, " + mode_name(cfg2) + ")");
            bool peek1_ok = r1.fixture_peek == 0x0001, peek2_ok = r2.fixture_peek == 0x0001;
            checks.expect(peek1_ok, "P5 fixture: S_START reads back $0001 through peek_word (second core, " + mode_name(cfg1) + ")");
            checks.expect(peek2_ok, "P5 fixture: S_START reads back $0001 through peek_word (second core, " + mode_name(cfg2) + ")");
            uint16_t ri1=0, rd1=0, ri2=0, rd2=0;
            bool c1_1=check_c1(r1.log,ri1), c2_1=check_c2(r1.log,rd1), c3_1=check_c3(r1.log);
            bool c1_2=check_c1(r2.log,ri2), c2_2=check_c2(r2.log,rd2), c3_2=check_c3(r2.log);
            checks.expect(c1_1, "C1 [p280]: interrupts off (P5 run, second core, " + mode_name(cfg1) + ")");
            checks.expect(c2_1, "C2 [p278]: DMA off (P5 run, second core, " + mode_name(cfg1) + ")");
            checks.expect(c3_1, "C3: S_END written exactly once, S_FAIL never written (P5 run, second core, " + mode_name(cfg1) + ")");
            bool c4_1 = c4_line(checks, r1.log, pop_p5(), "P5 run, second core, " + mode_name(cfg1));
            bool c5_1 = c5_line(checks, r1.seen, cfg1, "P5 run, second core, " + mode_name(cfg1));
            checks.expect(c1_2, "C1 [p280]: interrupts off (P5 run, second core, " + mode_name(cfg2) + ")");
            checks.expect(c2_2, "C2 [p278]: DMA off (P5 run, second core, " + mode_name(cfg2) + ")");
            checks.expect(c3_2, "C3: S_END written exactly once, S_FAIL never written (P5 run, second core, " + mode_name(cfg2) + ")");
            bool c4_2 = c4_line(checks, r2.log, pop_p5(), "P5 run, second core, " + mode_name(cfg2));
            bool c5_2 = c5_line(checks, r2.seen, cfg2, "P5 run, second core, " + mode_name(cfg2));

            bool p5a1=true, p5a2=true, p5c1=true, p5c2=true;
            auto sfail1 = find_write(r1.log, S_FAIL, 0xFA11);
            auto sfail2 = find_write(r2.log, S_FAIL, 0xFA11);
            p5a1 = sfail1.count == 0; p5a2 = sfail2.count == 0;
            uint32_t pre_sp1[6] = {0}, pre_sp2[6] = {0};
            for (int e = 1; e <= 5; ++e) {
                auto w1 = p5::case_window(r1, e);
                auto w2 = p5::case_window(r2, e);
                p5a1 = p5a1 && w1.ok;
                p5a2 = p5a2 && w2.ok;
                if (w1.ok) {
                    p5a1 = p5a1 && p5::vector_read_ok(r1, w1, e);
                    pre_sp1[e] = w1.pre_sp;
                    frames1[e] = p5::frame_of(r1, w1);
                    p5c1 = p5c1 && p5::writes_within_stack(frames1[e], w1.pre_sp, r1.post_sp[e]);
                } else p5c1 = false;
                if (w2.ok) {
                    p5a2 = p5a2 && p5::vector_read_ok(r2, w2, e);
                    pre_sp2[e] = w2.pre_sp;
                    frames2[e] = p5::frame_of(r2, w2);
                    p5c2 = p5c2 && p5::writes_within_stack(frames2[e], w2.pre_sp, r2.post_sp[e]);
                } else p5c2 = false;
            }
            checks.expect_gated(fixture1_ok, p5a1, "P5.a: every case was taken through its installed vector, no write "
                                                   "to S_FAIL (" + mode_name(cfg1) + ")");
            checks.expect_gated(fixture2_ok, p5a2, "P5.a: every case was taken through its installed vector, no write "
                                                   "to S_FAIL (" + mode_name(cfg2) + ")");
            checks.expect_gated(fixture1_ok, p5c1, "P5.c: every CPU write inside each case's window lies between "
                                                   "POST_SP_e and PRE_SP-2 (" + mode_name(cfg1) + ")");
            checks.expect_gated(fixture2_ok, p5c2, "P5.c: every CPU write inside each case's window lies between "
                                                   "POST_SP_e and PRE_SP-2 (" + mode_name(cfg2) + ")");
            for (int e=1;e<=5;++e) checks.record("P5 F_" + std::to_string(e) + " (" + mode_name(cfg1) + "): " + p5::frame_str(frames1[e]));
            for (int e=1;e<=5;++e) checks.record("P5 F_" + std::to_string(e) + " (" + mode_name(cfg2) + "): " + p5::frame_str(frames2[e]));
            have_10 = fixture1_ok && peek1_ok && p5a1 && p5c1 && c1_1 && c2_1 && c3_1 && c4_1 && c5_1;
            have_20 = fixture2_ok && peek2_ok && p5a2 && p5c2 && c1_2 && c2_2 && c3_2 && c4_2 && c5_2;

            // P5.b, dynamic half: PRE_SP equal across the 68000/68010/68020
            // runs, for every e = 1..5 (not just e=1). The SAME equality
            // oracle is used regardless of NMH_MUTATE: under P5B, r1 comes
            // from p5_mutb.bin (SUBQ.L #4,A7 inserted), so PRE_SP there
            // genuinely differs and this goes red on its own, for e=1 and
            // (SP stays offset) every later e too.
            bool p5b_dyn = true;
            for (int e = 1; e <= 5; ++e) {
                auto w0 = p5::case_window(r0, e);
                if (!(w0.ok)) { p5b_dyn = false; continue; }
                if (pre_sp0[e] != pre_sp1[e] || pre_sp0[e] != pre_sp2[e]) p5b_dyn = false;
            }
            // P5.b, static half: each image a VBR-side run loaded is equal to
            // the image the 68000-mode run loaded except inside step 3's four
            // bytes, which the listings locate (the dynamic half above is
            // exercised by the P5B contrast, this half by the P5B-SPAN
            // contrast).
            std::string p5b_why, p5b_why2;
            const bool p5b_static_10 = p5b_static_ok(bin0, vbrbin, p5b_why);
            const bool p5b_static_20 = p5b_static_ok(bin0, vbr20bin, p5b_why2);
            bool p5b_static = p5b_static_10 && p5b_static_20;
            if (p5b_why.empty()) p5b_why = p5b_why2;
            bool p5b = p5b_dyn && p5b_static;
            checks.expect(p5b, "P5.b: PRE_SP is the same in every run for every e=1..5, and the images differ only "
                               "inside the four bytes of step 3 (the two words of MOVEC or NOP,NOP)");
            if (!p5b_static) checks.note("P5.b static half: " + p5b_why);

            bool precond_00_10 = have_00 && have_10 && p5b;
            bool precond_00_20 = have_00 && have_20 && p5b;
            bool precond_10_20 = have_10 && have_20 && p5b;
            bool differ_00_10 = false, differ_00_20 = false, differ_10_20 = false;
            for (int e=1;e<=5;++e) {
                if (p5::frames_differ(frames0[e], frames1[e])) differ_00_10 = true;
                if (p5::frames_differ(frames0[e], frames2[e])) differ_00_20 = true;
                if (p5::frames_differ(frames1[e], frames2[e])) differ_10_20 = true;
            }
            checks.expect_gated(precond_00_10, differ_00_10,
                "P5 [p13]: for the pair (68000,68010), at least one exception writes a different frame (second core)");
            checks.expect_gated(precond_00_20, differ_00_20,
                "P5 [p13]: for the pair (68000,68020), at least one exception writes a different frame (second core)");
            checks.expect_gated(precond_10_20, differ_10_20,
                "P5 [p13]: for the pair (68010,68020), at least one exception writes a different frame (second core)");
            checks.record("opus:1-guidelines.c52,sol:1-guidelines.c52 withdrawn (P5, second core): \"different on "
                          "each member\" is universal over five members, three available (68030/68040 absent); "
                          "the 68010/68020 labels are cpu_config values, and the harness says that nothing at the "
                          "ports shows which setting a run used (see C5)");

        } else {
            checks.record("P5 CONTRAST data (first core, 68000 mode) recorded above as F_1..F_5; compare against "
                           "the second core's own 68000-mode F_1..F_5 once both outputs exist (both must be equal, "
                           "since two 68000s are predicted to write the same frame)");
        }

    }

    // =======================================================================
    // P6 [p13]: the 68020 prefetches more words than the 68000. Graded on
    // the second core only (68020 vs 68000); first core recorded for the
    // report-level contrast. (opus/sol:1-guidelines.c67 is declined below,
    // the same harness-option limit as P4/c59.)
    // =======================================================================
    {
        std::string bin = dir + "/p6.bin";
        if (mv == "P6E") bin = dir + "/p6_mute.bin";
        else if (mv == "P6ALIGN") bin = dir + "/p6_mutalign.bin";
        else if (mv == "P3A-P6") bin = dir + "/p6_muta.bin";
        else if (mv == "P3B-P6") bin = dir + "/p6_mutb.bin";
        else if (mv == "P3C-P6") bin = dir + "/p6_mutc.bin";
        else if (mv == "P3D-P6") bin = dir + "/p6_mutd.bin";
        bin = pick_macro_variant(mv, "P6", dir, "p6", bin);
        auto r = p6::run(bin, 0x00);
        bool fixture_ok = r.loaded && r.released && r.done;
        checks.expect(fixture_ok, "P6 fixture: p6.bin loaded, released and reached DONE (" + core_label + ", 68000 mode)");
        bool peek_ok = r.fixture_peek == 0x0001;
        checks.expect(peek_ok, "P6 fixture: S_START reads back $0001 through peek_word (" + core_label + ", 68000 mode)");
        uint16_t ri=0, rd=0;
        bool c1_ok = check_c1(r.log, ri), c2_ok = check_c2(r.log, rd);
        bool c3_ok = check_c3(r.log);
        checks.expect(c1_ok, "C1 [p280]: interrupts off (P6 run, " + core_label + ", 68000 mode)");
        checks.expect(c2_ok, "C2 [p278]: DMA off (P6 run, " + core_label + ", 68000 mode)");
        checks.expect(c3_ok, "C3: S_END written exactly once, S_FAIL never written (P6 run, " + core_label + ", 68000 mode)");
        bool c4_ok = c4_line(checks, r.log, pop_p6(), "P6 run, " + core_label + ", 68000 mode");
        bool c5_ok = c5_line(checks, r.seen, 0x00, "P6 run, " + core_label + ", 68000 mode");

        auto win = p6::run_window(r);
        auto pc = p6::common_precond(r, win, bin);
        checks.expect(pc.p3a, "P3.a (reused, SPECIFICATION.md section 8): every one of the 8 blocks' stop "
                              "instruction executed and transferred control: exactly one S_P and one S_L write "
                              "per block, in the order P1,L1,P2,L2,... (" + core_label + ", 68000 mode)");
        checks.expect(pc.p3b, "P3.b (reused): no guard word was executed, S_X and S_FAIL never written (" + core_label + ", 68000 mode)");
        pc.p3c = p3c_line(checks, pc.p3c_scan, "P3.c (reused): no instruction of the assembled program accesses data in a guard "
                              "word range W_k to W_k+11, on any effective-address path (" + core_label + ", 68000 mode)");
        checks.expect(pc.p3d, "P3.d (reused): for every block, a port read at X_k inside the run window carries the opcode "
                              "family the table names; an address and data anchor, the record names no requester (" + core_label + ", 68000 mode)");

        bool align_ok = p6::alignment_ok(r);
        checks.expect(align_ok, "P6 alignment: within each pair, the two copies' X_k lie at addresses of "
                                 "different parity mod 4 (" + core_label + ", 68000 mode)");

        int guard_len = p6::guard_len_of(r); // taken from the image, not from NMH_MUTATE
        int D0 = 0;
        for (int k = 1; k <= 8; ++k) D0 = std::max(D0, win.ok ? p6::g_of(r, win, r.x[k], guard_len) : 0);
        checks.record("P6 D(68000 mode, " + core_label + ") = " + std::to_string(D0) + " (guard length " + std::to_string(guard_len) + ")");

        // Alignment is not one of P6.e's own preconditions (SPECIFICATION.md
        // section 8 lists only P3.a-P3.d): it is asserted above on its own,
        // and does not gate P6.e, so P6.e's own contrast (P6E, which
        // deliberately shortens the guard) is not swallowed by a mutation
        // that -- as a side effect of the shorter guard -- also shifts
        // every X_k's parity.
        bool precond_e = fixture_ok && peek_ok && c1_ok && c2_ok && c3_ok && c4_ok && c5_ok &&
                         pc.p3a && pc.p3b && pc.p3c && pc.p3d;
        checks.expect_gated(precond_e, win.ok && D0 < guard_len,
            "P6.e: D(68000 mode) is less than the guard length (" + std::to_string(guard_len) + " words, " + core_label + ")");

        if (is_tg68k) {
            // The second run is the 68020-mode run; P6-68000 requests and drives
            // cpu_config 00 for it (the equal-68000 control), C5-P6 drives 00
            // while 11 is requested.
            const uint8_t cfg2 = (mv == "P6-68000") ? 0x00 : 0x03;
            const std::string md2 = "second core, " + mode_name(cfg2);
            auto r20 = p6::run(bin, cfg2);
            bool fixture20_ok = r20.loaded && r20.released && r20.done;
            checks.expect(fixture20_ok, "P6 fixture: p6.bin loaded, released and reached DONE (" + md2 + ")");
            bool peek20_ok = r20.fixture_peek == 0x0001;
            checks.expect(peek20_ok, "P6 fixture: S_START reads back $0001 through peek_word (" + md2 + ")");
            uint16_t ri20=0, rd20=0;
            bool c1_20=check_c1(r20.log,ri20), c2_20=check_c2(r20.log,rd20), c3_20=check_c3(r20.log);
            checks.expect(c1_20, "C1 [p280]: interrupts off (P6 run, " + md2 + ")");
            checks.expect(c2_20, "C2 [p278]: DMA off (P6 run, " + md2 + ")");
            checks.expect(c3_20, "C3: S_END written exactly once, S_FAIL never written (P6 run, " + md2 + ")");
            bool c4_20 = c4_line(checks, r20.log, pop_p6(), "P6 run, " + md2);
            bool c5_20 = c5_line(checks, r20.seen, cfg2, "P6 run, " + md2);

            auto win20 = p6::run_window(r20);
            auto pc20 = p6::common_precond(r20, win20, bin);
            checks.expect(pc20.p3a, "P3.a (reused): every one of the 8 blocks' stop instruction executed and "
                                    "transferred control: exactly one S_P and one S_L write per block, in the "
                                    "order P1,L1,P2,L2,... (" + md2 + ")");
            checks.expect(pc20.p3b, "P3.b (reused): no guard word was executed, S_X and S_FAIL never written (" + md2 + ")");
            pc20.p3c = p3c_line(checks, pc20.p3c_scan, "P3.c (reused): no instruction of the assembled program accesses data in a "
                                    "guard word range W_k to W_k+11, on any effective-address path (" + md2 + ")");
            checks.expect(pc20.p3d, "P3.d (reused): for every block, a port read at X_k inside the run window carries the opcode "
                                    "family the table names; an address and data anchor, the record names no requester (" + md2 + ")");
            bool align20_ok = p6::alignment_ok(r20);
            checks.expect(align20_ok, "P6 alignment: within each pair, the two copies' X_k lie at addresses of "
                                       "different parity mod 4 (" + md2 + ")");

            int guard_len20 = p6::guard_len_of(r20);
            int D20 = 0;
            for (int k = 1; k <= 8; ++k) D20 = std::max(D20, win20.ok ? p6::g_of(r20, win20, r20.x[k], guard_len20) : 0);
            checks.record("P6 D(" + mode_name(cfg2) + ", second core) = " + std::to_string(D20));

            // P6 [p13]: D(68020 mode) > D(68000 mode), the same image on the
            // second core. Gated on both runs' fixture, C1-C5, P3.a-P3.d and
            // alignment, on P6.e (68000-mode run) and on the asserted modes.
            // The line carries no claim id: opus/sol:1-guidelines.c67 stands
            // on the RECORDED line below (SPECIFICATION.md section 1).
            const bool p6e_ok = precond_e && win.ok && D0 < guard_len;
            const bool p6_pre = p6e_ok && align_ok &&
                                fixture20_ok && peek20_ok && c1_20 && c2_20 && c3_20 && c4_20 && c5_20 &&
                                pc20.p3a && pc20.p3b && pc20.p3c && pc20.p3d && align20_ok;
            checks.expect_gated(p6_pre, D20 > D0,
                "P6 [p13]: D(" + mode_name(cfg2) + ") > D(68000 mode), the same image on the second core: the 68020 "
                "prefetches more words than the 68000 (D(" + mode_name(cfg2) + ") = " + std::to_string(D20) +
                ", D(68000 mode) = " + std::to_string(D0) + ")");
            checks.record("opus:1-guidelines.c67,sol:1-guidelines.c67 withdrawn (P6): \"The more advanced processors "
                          "prefetch more words\" is universal over the more advanced processors, the 68030 and 68040 "
                          "are not available and the page does not say whether the 68010 counts; the graded line above "
                          "is the relation for the one pair the system can run");
            checks.record("P6 measured: D(" + mode_name(cfg2) + ") = " + std::to_string(D20) + ", D(68000 mode) = " +
                std::to_string(D0) + " (same image, second core); the " + mode_name(cfg2) + " RAM-port log against the "
                "68000-mode log of the same image: " + compare_logs(r.log, r20.log));
        } else {
            checks.record("P6 CONTRAST data (first core) recorded above as D(68000 mode); compare against the "
                           "second core's own 68000-mode D once both outputs exist (neither side is predicted "
                           "'more advanced', so 'more' must not hold)");
        }
    }


    // =======================================================================
    // P7 [p13]: MOVE SR,<dest> acts differently on the other members of the
    // family. Graded on the second core in its three modes: the outcome
    // (DEST, MRK_PRIV, MRK_TRAP) differs between the 68000 and each of the
    // 68010 and the 68020; the port's record of the moved-SR window also
    // holds the exception frames, p.13's separate difference, and is only
    // recorded. opus/sol:1-guidelines.c56 is withdrawn below
    // (it speaks of every other member; the 68030 and 68040 are not
    // available), so the graded lines carry no claim id.
    // NMH_MUTATE: P7A, P7B, P7C (the preconditions' contrasts), P7-CTL (the
    // same-mode control), P7-PAIR-0010 and P7-PAIR-0020 (the 68010 or 68020
    // run requests and is driven with 00), C5-P7, and the shared contrasts.
    // =======================================================================
    {
        struct Run7 {
            std::string label;      // "<core label>, <mode>"
            uint8_t req = 0;
            p7::Result r;
            p7::Win wm, ww;
            std::vector<p7::Entry> q;
            bool ok = false;        // every line of the run held
        };
        std::string bin7 = dir + "/p7.bin";
        if (mv == "P7A") bin7 = dir + "/p7_muta.bin";
        else if (mv == "P7B") bin7 = dir + "/p7_mutb.bin";
        else if (mv == "P7C") bin7 = dir + "/p7_mutc.bin";
        bin7 = pick_macro_variant(mv, "P7", dir, "p7", bin7);

        auto do_run = [&](Run7& x, const std::string& bin, uint8_t req, const std::string& suffix = "") {
            x.req = req;
            x.label = core_label + ", " + mode_name(req) + suffix;
            x.r = p7::run(bin, req);
            const std::string run = "P7 run, " + x.label;
            bool fixture_ok = x.r.loaded && x.r.released && x.r.done;
            checks.expect(fixture_ok, "P7 fixture: p7.bin loaded, released and reached DONE (" + x.label + ")");
            bool peek_ok = x.r.fixture_peek == 0x0001;
            checks.expect(peek_ok, "P7 fixture: S_START reads back $0001 through peek_word, before any other peeked "
                                   "value is trusted (" + x.label + ")");
            uint16_t ri = 0, rd = 0;
            bool c1 = check_c1(x.r.log, ri), c2 = check_c2(x.r.log, rd), c3 = check_c3(x.r.log);
            checks.expect(c1, "C1 [p280]: interrupts off (" + run + ")");
            checks.expect(c2, "C2 [p278]: DMA off (" + run + ")");
            checks.expect(c3, "C3: S_END written exactly once, S_FAIL never written (" + run + ")");
            bool c4 = c4_line(checks, x.r.log, pop_p7(), run);
            bool c5 = c5_line(checks, x.r.seen, req, run);

            x.wm = p7::window(x.r, 0xE702, 0xF702);
            x.ww = p7::window(x.r, 0xE701, 0xF701);
            auto sfail = find_write(x.r.log, S_FAIL, 0xFA11);
            bool p7a = x.wm.ok && sfail.count == 0;
            checks.expect(p7a, "P7.a: exactly one ordered S_A/S_B pair for the moved-SR window ($E702, $F702), no write "
                               "to S_FAIL (" + x.label + ")");
            auto ws = find_write(x.r.log, S_START, 0x0001);
            auto we = find_write(x.r.log, S_END, 0xE0D0);
            bool run_win = ws.found && we.found && ws.cycle < we.cycle;
            auto xm_read = run_win ? last_read_between(x.r.log, x.r.xm, ws.cycle, we.cycle) : WHitData();
            bool p7b = xm_read.found && xm_read.data == 0x40F8;
            checks.expect(p7b, "P7.b: a port read at X_m inside the run window carries $40F8 (MOVE.W SR,abs.W); an "
                               "address and data anchor, the record names no requester (" + x.label + ")");
            auto xw_read = run_win ? last_read_between(x.r.log, x.r.xw, ws.cycle, we.cycle) : WHitData();
            bool p7c = x.ww.ok && read_count_between(x.r.log, 0x20, x.ww.open, x.ww.close) > 0 &&
                       xw_read.found && xw_read.data == 0x46FC;
            checks.expect(p7c, "P7.c: user mode was entered: inside the witness window ($E701, $F701) a read at the "
                               "privilege-violation vector, and a port read at X_w carries $46FC (MOVE.W #imm,SR) (" + x.label + ")");
            x.ok = fixture_ok && peek_ok && c1 && c2 && c3 && c4 && c5 && p7a && p7b && p7c;
            x.q = p7::sequence(x.r, x.wm);
            char b[96];
            std::snprintf(b, sizeof b, "DEST=%04X MRK_PRIV=%04X MRK_TRAP=%04X", x.r.dest, x.r.mrk_priv, x.r.mrk_trap);
            checks.record("P7 measured (" + x.label + "): CPU writes inside the moved-SR window: " +
                          p7::writes_str(x.r, x.wm) + "; result words after the run: " + b + "; " +
                          std::to_string(x.q.size()) + " port accesses in the window");
        };

        if (is_tg68k) {
            Run7 a, bb, c, dd;
            do_run(a, bin7, 0x00);
            do_run(bb, bin7, (mv == "P7-PAIR-0010") ? 0x00 : 0x01);
            do_run(c, bin7, (mv == "P7-PAIR-0020") ? 0x00 : 0x03);
            // the same-mode control: a second run of the 68000 mode, through
            // the same per-run checks as the three primary runs
            std::string bin_ctl = (mv == "P7-CTL") ? dir + "/p7_mutctl.bin" : bin7;
            do_run(dd, bin_ctl, 0x00, ", control run");
            // the program-visible outcome of the instruction: the destination
            // word and the two handler markers
            auto outcome = [](const Run7& x) {
                return std::array<uint16_t, 3>{x.r.dest, x.r.mrk_priv, x.r.mrk_trap};
            };
            const bool ctl_pre = a.ok && dd.ok;
            const bool control = ctl_pre && outcome(a) == outcome(dd);
            checks.expect_gated(ctl_pre, control,
                "P7 control: two runs in the 68000 mode leave the same outcome (DEST, MRK_PRIV, MRK_TRAP), so a "
                "difference between modes is a property of the mode (" + core_label + ")");
            checks.record("P7 control: the port record of the moved-SR window of the control run against the first "
                          "68000-mode run: " + p7::first_diff(a.q, dd.q));

            // the pair lines use the two runs' outcomes only; the passing control gates them
            const bool pair_a_b = a.ok && bb.ok && dd.ok && control;
            const bool pair_a_c = a.ok && c.ok && dd.ok && control;
            checks.expect_gated(pair_a_b, outcome(a) != outcome(bb),
                "P7 [p13]: for the pair (68000," + mode_name(bb.req).substr(0, 5) + "), MOVE SR,<dest> executed in user mode "
                "leaves a different outcome (DEST, MRK_PRIV, MRK_TRAP) (second core)");
            checks.expect_gated(pair_a_c, outcome(a) != outcome(c),
                "P7 [p13]: for the pair (68000," + mode_name(c.req).substr(0, 5) + "), MOVE SR,<dest> executed in user mode "
                "leaves a different outcome (DEST, MRK_PRIV, MRK_TRAP) (second core)");
            checks.record("P7 pair (" + mode_name(bb.req) + ", " + mode_name(c.req) + ") is reported and not graded: first "
                          "difference of the port records " + p7::first_diff(bb.q, c.q) + "; outcomes " +
                          (outcome(bb) == outcome(c) ? "equal" : "different"));
            checks.record("P7 first difference between the 68000-mode port record and the " + mode_name(bb.req) + " record: " +
                          p7::first_diff(a.q, bb.q) + "; and the " + mode_name(c.req) + " record: " + p7::first_diff(a.q, c.q) +
                          " (the records also hold the exception frames and are not graded)");
            checks.record("opus:1-guidelines.c56,sol:1-guidelines.c56 withdrawn (P7): \"acts differently on other members "
                          "of the 68000 family\" covers every other member, the 68030 and 68040 are not available; the "
                          "graded lines compare the 68000 with the 68010 and with the 68020");
        } else {
            Run7 a;
            do_run(a, bin7, 0x00);
            checks.record("P7 CONTRAST data (first core, 68000 mode) recorded above; the first core has no 68010 or "
                          "68020 mode and takes no part in the pair lines");
        }
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
