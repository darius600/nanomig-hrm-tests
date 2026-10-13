// test_ch8_ecs_sync_registers.cpp -- ch8-ecs-sync-registers. See
// SPECIFICATION.md for every assertion's proposition, program, observed
// value and expected value. This file only implements what
// SPECIFICATION.md already committed to.
//
// NMH_MUTATE selects a mutated program, configuration or comparator input
// in place of its normal counterpart, one assertion or precondition at a
// time; none is exercised by a plain `make test`. The contrasting runs that
// SPECIFICATION.md's Runs table lists (H1c/H2c for P1/P2, SA0/SB0/SC0 for
// P3/P4) are the pages' contrasting case (VARBEAMEN=0 for P1/P2,
// VARHSYEN=0 for P3/P4); they run on every `make test` and are graded as
// lines of their own.
//
// Two kinds of selector exist. A precondition selector replaces the normal
// capture of one run with the mutant's capture at the source, so every line
// that consumes that run sees the mutant: the precondition's own line goes
// red and every line gated on it prints "precondition not met". A
// proposition selector changes the input of one proposition line only (a
// different capture, or another run's measured values, fed to the
// unchanged comparator). Every capture, normal or mutant, prints PC-ID,
// PC-MARK and DONE under its run's own label, so a mutant that goes red from
// a dead setup (a crash, a halted CPU, an emptied population) is
// distinguishable from one that goes red through the clause under test.
//
// Every record below names the line it turns red, the selector, the stimulus it changes,
// and the result of the replay on each core. A selector changes a stimulus only: the
// comparators, the expected values and the populations are the normal run's. A line that
// consumes a failed precondition prints "precondition not met" and is neither green nor red.
//
// MUTATION FIXTURE stimulus red NMH_MUTATE=FIXTURE: FIXTURE: H1 with FIXCONST holding $DEAD
//   instead of $C3A5 (prog_h_h1_fixture.bin, FIXTURE_MUTANT); the markers, the samples and DONE
//   are the normal run's. Red on fx68k and tg68k (81 passed, 1 failed): FIXTURE. Not graded:
//   none.
// MUTATION PC-ID stimulus red NMH_MUTATE=X-OCS: PC-ID [H1]: H1 with the harness set to OCS
//   (chipset_config clears the ECS bit); VPOSR bits 14-8 read the original-chipset value
//   instead of the enhanced-Agnus PAL value. Red on fx68k and tg68k (72 passed, 1 failed):
//   PC-ID [H1]. Not graded: P1-LINES [H1], P1-COVER [H1], P2-LINES [H1], A2 [H1], P1 [H1],
//   P2(a) [H1], P2(b), P2(c), P2. Measured: PC-ID [H1]: VPOSR bits 14-8 = $00.
// MUTATION PC-ID stimulus red NMH_MUTATE=X-NTSC: PC-ID [H1]: H1 with the harness set to NTSC
//   (chipset_config sets the NTSC bit); VPOSR bits 14-8 read the enhanced-Agnus NTSC value
//   instead of the PAL value. Red on fx68k and tg68k (72 passed, 1 failed): PC-ID [H1]. Not
//   graded: P1-LINES [H1], P1-COVER [H1], P2-LINES [H1], A2 [H1], P1 [H1], P2(a) [H1], P2(b),
//   P2(c), P2. Measured: PC-ID [H1]: VPOSR bits 14-8 = $30.
// MUTATION PC-MARK stimulus red NMH_MUTATE=X-HALT-H: PC-MARK [H1]: H1 with MARK1's write
//   omitted (prog_h_h1_halt.bin, MARK_OMIT); no marker appears, while the rest of the program,
//   including DONE, still runs. Red on fx68k and tg68k (72 passed, 1 failed): PC-MARK [H1]. Not
//   graded: P1-LINES [H1], P1-COVER [H1], P2-LINES [H1], A2 [H1], P1 [H1], P2(a) [H1], P2(b),
//   P2(c), P2.
// MUTATION PC-MARK stimulus red NMH_MUTATE=X-NOMARK2: PC-MARK [H1]: H1 with MARK2's write
//   omitted (prog_h_h1_nomark2.bin, MARK2_OMIT); MARK1 appears and nothing follows it, so the
//   clause 'MARK2 later' fails alone. Red on fx68k and tg68k (72 passed, 1 failed): PC-MARK
//   [H1]. Not graded: P1-LINES [H1], P1-COVER [H1], P2-LINES [H1], A2 [H1], P1 [H1], P2(a)
//   [H1], P2(b), P2(c), P2.
// MUTATION PC-MARK stimulus red NMH_MUTATE=X-HALT-S: PC-MARK [SA]: SA with MARK1's write
//   omitted (prog_s_sa_halt.bin, MARK_OMIT); no marker appears, while the rest of the program,
//   including DONE, still runs. Red on fx68k and tg68k (64 passed, 1 failed): PC-MARK [SA]. Not
//   graded: P3/P4-LINES [SA], A4, P3/P4(a) [SA], P3/P4(a) [SB], P3/P4(a) [SC], P3/P4(a) [SA0],
//   P3/P4(a) [SB0], P3/P4(a) [SC0], A5, P3(b), P3(c), P3-CONTRAST, P3, P4(b), P4(c),
//   P4-CONTRAST, P4.
// MUTATION DONE stimulus red NMH_MUTATE=X-NODONE: DONE [H1]: H1 whose program stops in place
//   after MARK2 without storing the DONE value (prog_h_h1_nodone.bin, NO_DONE); PC-ID and
//   PC-MARK hold. Red on fx68k and tg68k (81 passed, 1 failed): DONE [H1]. Not graded: none.
// MUTATION P1-LINES stimulus red NMH_MUTATE=X-SHORT: P1-LINES [H1]: H1's sampling loop
//   shortened to 1024 samples (prog_h_h1_short.bin, SHORT_LOOP), followed by a delay loop that
//   keeps the MARK1..MARK2 window as long as the normal run's. Red on fx68k and tg68k (80
//   passed, 1 failed): P1-LINES [H1]. Not graded: P1 [H1]. Measured: P1-LINES [H1]: 34 graded
//   wraps, 1024 samples (fx68k), 35 graded wraps, 1024 samples (tg68k); P2-LINES [H1]: 719
//   graded periods (fx68k), 852 graded periods (tg68k).
// MUTATION P1-LINES stimulus red NMH_MUTATE=X-SHORT-H2: P1-LINES [H2]: as X-SHORT, for H2
//   (prog_h_h2_short.bin). Red on fx68k and tg68k (80 passed, 1 failed): P1-LINES [H2]. Not
//   graded: P1 [H2]. Measured: P1-LINES [H2]: 31 graded wraps, 1024 samples (fx68k), 32 graded
//   wraps, 1024 samples (tg68k); P2-LINES [H2]: 657 graded periods (fx68k), 781 graded periods
//   (tg68k).
// MUTATION P1-LINES stimulus red NMH_MUTATE=X-SHORT-H1C: P1-LINES [H1c]: as X-SHORT, for H1c
//   (prog_h_h1c_short.bin). Red on fx68k and tg68k (79 passed, 1 failed): P1-LINES [H1c]. Not
//   graded: A1 [H1c], P1-CONTRAST [H1c]. Measured: P1-LINES [H1c]: 26 graded wraps, 1024
//   samples (fx68k), 27 graded wraps, 1024 samples (tg68k); P2-LINES [H1c]: 559 graded periods
//   (fx68k), 663 graded periods (tg68k).
// MUTATION P1-LINES stimulus red NMH_MUTATE=X-SHORT-H2C: P1-LINES [H2c]: as X-SHORT, for H2c
//   (prog_h_h2c_short.bin). Red on fx68k and tg68k (79 passed, 1 failed): P1-LINES [H2c]. Not
//   graded: A1 [H2c], P1-CONTRAST [H2c]. Measured: P1-LINES [H2c]: 26 graded wraps, 1024
//   samples (fx68k), 27 graded wraps, 1024 samples (tg68k); P2-LINES [H2c]: 559 graded periods
//   (fx68k), 663 graded periods (tg68k).
// MUTATION P2-LINES stimulus red NMH_MUTATE=X-LATE: P2-LINES [H1]: H1 with all 16384 samples
//   but MARK1 written only after the last of them, followed by a short delay loop
//   (prog_h_h1_late.bin, LATE_MARK): the MARK1..MARK2 window holds far fewer than 320 line
//   periods while the sampled population is the normal run's. Red on fx68k and tg68k (76
//   passed, 1 failed): P2-LINES [H1]. Not graded: A2 [H1], P2(a) [H1], P2(b), P2(c), P2.
//   Measured: P1-LINES [H1]: 564 graded wraps, 16384 samples (fx68k), 566 graded wraps, 16384
//   samples (tg68k); P2-LINES [H1]: 83 graded periods (fx68k), 101 graded periods (tg68k).
// MUTATION P2-LINES stimulus red NMH_MUTATE=X-LATE-H2: P2-LINES [H2]: as X-LATE, for H2
//   (prog_h_h2_late.bin). Red on fx68k and tg68k (76 passed, 1 failed): P2-LINES [H2]. Not
//   graded: A2 [H2], P2(a) [H2], P2(b), P2(c), P2. Measured: P1-LINES [H2]: 516 graded wraps,
//   16384 samples (fx68k), 519 graded wraps, 16384 samples (tg68k); P2-LINES [H2]: 77 graded
//   periods (fx68k), 92 graded periods (tg68k).
// MUTATION P2-LINES stimulus red NMH_MUTATE=X-LATE-H1C: P2-LINES [H1c]: as X-LATE, for H1c
//   (prog_h_h1c_late.bin). Red on fx68k and tg68k (79 passed, 1 failed): P2-LINES [H1c]. Not
//   graded: A2 [H1c], P2-CONTRAST. Measured: P1-LINES [H1c]: 439 graded wraps, 16384 samples
//   (fx68k), 441 graded wraps, 16384 samples (tg68k); P2-LINES [H1c]: 64 graded periods
//   (fx68k), 77 graded periods (tg68k).
// MUTATION P2-LINES stimulus red NMH_MUTATE=X-LATE-H2C: P2-LINES [H2c]: as X-LATE, for H2c
//   (prog_h_h2c_late.bin). Red on fx68k and tg68k (79 passed, 1 failed): P2-LINES [H2c]. Not
//   graded: A2 [H2c], P2-CONTRAST. Measured: P1-LINES [H2c]: 439 graded wraps, 16384 samples
//   (fx68k), 441 graded wraps, 16384 samples (tg68k); P2-LINES [H2c]: 64 graded periods
//   (fx68k), 77 graded periods (tg68k).
// MUTATION P1-COVER stimulus red NMH_MUTATE=X-BURST: P1-COVER [H1]: H1's sampling loop replaced
//   with a read-until-wrap-then-store-4 burst (prog_h_h1_burst.bin, BURST_MODE): consecutive
//   graded sample stores are a whole line apart between bursts. Red on fx68k and tg68k (80
//   passed, 1 failed): P1-COVER [H1]. Not graded: P1 [H1]. Measured: P1-COVER [H1]: largest
//   consecutive-sample gap in the graded range = 148 CCK (fx68k), largest consecutive-sample
//   gap in the graded range = 153 CCK (tg68k); P1-LINES [H1]: 398 graded wraps, 1600 samples.
// MUTATION P1-COVER stimulus red NMH_MUTATE=X-BURST-H2: P1-COVER [H2]: as X-BURST, for H2
//   (prog_h_h2_burst.bin). Red on fx68k and tg68k (80 passed, 1 failed): P1-COVER [H2]. Not
//   graded: P1 [H2]. Measured: P1-COVER [H2]: largest consecutive-sample gap in the graded
//   range = 164 CCK (fx68k), largest consecutive-sample gap in the graded range = 169 CCK
//   (tg68k); P1-LINES [H2]: 398 graded wraps, 1600 samples.
// MUTATION A2 stimulus red NMH_MUTATE=X-AUD0: A2 [H1]: H1 with audio channel 0 DMA started
//   (prog_h_h1_aud0.bin, AUD0_ON: location, length 8 words, volume 64, period 124, enable;
//   p.145, p.140, p.266): a non-refresh DMA access appears at the port, so a graded period's
//   group no longer has exactly 4 members. Red on fx68k and tg68k (81 passed, 1 failed): A2
//   [H1]. Not graded: none. Measured: P2-LINES [H1]: 563 graded periods (fx68k), 566 graded
//   periods (tg68k).
// MUTATION A2 stimulus red NMH_MUTATE=X-AUD0-H2: A2 [H2]: as X-AUD0, for H2
//   (prog_h_h2_aud0.bin). Red on fx68k and tg68k (81 passed, 1 failed): A2 [H2]. Not graded:
//   none. Measured: P2-LINES [H2]: 516 graded periods (fx68k), 518 graded periods (tg68k).
// MUTATION A2 stimulus red NMH_MUTATE=X-AUD0-H1C: A2 [H1c]: as X-AUD0, for H1c
//   (prog_h_h1c_aud0.bin). Red on fx68k and tg68k (81 passed, 1 failed): A2 [H1c]. Not graded:
//   none. Measured: P2-LINES [H1c]: 438 graded periods (fx68k), 440 graded periods (tg68k).
// MUTATION A2 stimulus red NMH_MUTATE=X-AUD0-H2C: A2 [H2c]: as X-AUD0, for H2c
//   (prog_h_h2c_aud0.bin). Red on fx68k and tg68k (81 passed, 1 failed): A2 [H2c]. Not graded:
//   none. Measured: P2-LINES [H2c]: 438 graded periods (fx68k), 440 graded periods (tg68k).
// MUTATION A1 stimulus red NMH_MUTATE=A1: A1 [H1c]: H1's graded samples (VARBEAMEN=1, bounded
//   by N) in A1 [H1c]'s unchanged comparator, which expects the free-running range only
//   VARBEAMEN=0 produces. Red on fx68k and tg68k (81 passed, 1 failed): A1 [H1c]. Not graded:
//   none. Measured: A1 [H1c]: max graded H=$00B0.
// MUTATION A1 stimulus red NMH_MUTATE=A1-H2: A1 [H2c]: as A1, H2's graded samples in A1 [H2c].
//   Red on fx68k and tg68k (81 passed, 1 failed): A1 [H2c]. Not graded: none. Measured: A1
//   [H2c]: max graded H=$00C0 (fx68k), max graded H=$00BE (tg68k).
// MUTATION A3 stimulus red NMH_MUTATE=A3: A3 [H1c]: H1's graded periods (VARBEAMEN=1, 177 CCK)
//   in A3 [H1c]'s unchanged 227/228-CCK comparator. Red on fx68k and tg68k (61 passed, 1
//   failed): A3 [H1c]. Not graded: P1-COVER [H1], P1-COVER [H2], P2-LINES [H1], P2-LINES [H2],
//   P2-LINES [H1c], P2-LINES [H2c], A2 [H1], A2 [H2], A2 [H1c], A2 [H2c], P1 [H1], P1 [H2],
//   P2(a) [H1], P2(a) [H2], P2(b), P2(c), P2, P2-CONTRAST, A5-REF, A5.
// MUTATION A3 stimulus red NMH_MUTATE=A3-H2: A3 [H2c]: as A3, H2's graded periods in A3 [H2c].
//   Red on fx68k and tg68k (61 passed, 1 failed): A3 [H2c]. Not graded: P1-COVER [H1], P1-COVER
//   [H2], P2-LINES [H1], P2-LINES [H2], P2-LINES [H1c], P2-LINES [H2c], A2 [H1], A2 [H2], A2
//   [H1c], A2 [H2c], P1 [H1], P1 [H2], P2(a) [H1], P2(a) [H2], P2(b), P2(c), P2, P2-CONTRAST,
//   A5-REF, A5.
// MUTATION P1 stimulus red NMH_MUTATE=P1: P1 [H1]: H1c's graded samples (VARBEAMEN=0,
//   unbounded) in P1 [H1]'s unchanged any-exceeds comparator. Red on fx68k and tg68k (81
//   passed, 1 failed): P1 [H1]. Not graded: none. Measured: P1 [H1]: max graded H=$00E2,
//   N=$00B0.
// MUTATION P1 stimulus red NMH_MUTATE=P1-H2: P1 [H2]: as P1, H2c's graded samples in P1 [H2].
//   Red on fx68k and tg68k (81 passed, 1 failed): P1 [H2]. Not graded: none. Measured: P1 [H2]:
//   max graded H=$00E2, N=$00C0.
// MUTATION P1-CONTRAST stimulus red NMH_MUTATE=P1C: P1-CONTRAST [H1c]: H1's graded samples
//   (VARBEAMEN=1) in P1-CONTRAST [H1c]'s unchanged comparator, which expects a sample above N.
//   Red on fx68k and tg68k (81 passed, 1 failed): P1-CONTRAST [H1c]. Not graded: none.
//   Measured: P1-CONTRAST [H1c]: max graded H=$00B0, N=$00B0.
// MUTATION P1-CONTRAST stimulus red NMH_MUTATE=P1C-H2: P1-CONTRAST [H2c]: as P1C, H2's graded
//   samples in P1-CONTRAST [H2c]. Red on fx68k and tg68k (81 passed, 1 failed): P1-CONTRAST
//   [H2c]. Not graded: none. Measured: P1-CONTRAST [H2c]: max graded H=$00C0, N=$00C0 (fx68k),
//   max graded H=$00BE, N=$00C0 (tg68k).
// MUTATION P2(a) stimulus red NMH_MUTATE=P2A: P2(a) [H1]: H1 with HTOTAL written again
//   mid-loop, to $00C0 in place of $00B0 (prog_h_h1_retotal.bin, RETOTAL_MID), in P2(a) [H1]
//   only; p.280 states no exception for a second write, so the graded periods after that point
//   are page-predicted to differ from those before it. The conjunction P2 requires P2(a) [H1]
//   and goes red with it. The capture prints its own PC-ID, PC-MARK, DONE and P2-LINES under
//   the tag [H1,P2A]. Red on fx68k and tg68k (84 passed, 2 failed): P2(a) [H1], P2. Not graded:
//   none. Measured: P2-LINES [H1,P2A]: 553 graded periods.
// MUTATION P2 stimulus red NMH_MUTATE=P2A: the line requires P2(a) [H1] and goes red with it
//   (above).
// MUTATION P2(a) stimulus red NMH_MUTATE=P2A-H2: P2(a) [H2]: as P2A for H2: HTOTAL written
//   again to $00D0 in place of $00C0 (prog_h_h2_retotal.bin), in P2(a) [H2] only; tag
//   [H2,P2A-H2]. Red on fx68k and tg68k (84 passed, 2 failed): P2(a) [H2], P2. Not graded:
//   none. Measured: P2-LINES [H2,P2A-H2]: 509 graded periods.
// MUTATION P2 stimulus red NMH_MUTATE=P2A-H2: the line requires P2(a) [H2] and goes red with it
//   (above).
// MUTATION P2(b) stimulus red NMH_MUTATE=P2B: P2(b): H1c's and H2c's first periods
//   (VARBEAMEN=0, d=0) in P2(b)'s unchanged delta comparator. Red on fx68k and tg68k (80
//   passed, 2 failed): P2(b), P2. Not graded: none.
// MUTATION P2 stimulus red NMH_MUTATE=P2B: the line requires P2(b) and goes red with it
//   (above).
// MUTATION P2(c) stimulus red NMH_MUTATE=P2C: P2(c): H1c's first period (VARBEAMEN=0, about 227
//   CCK) in P2(c)'s unchanged N/N+1 comparator. Red on fx68k and tg68k (80 passed, 2 failed):
//   P2(c), P2. Not graded: none.
// MUTATION P2 stimulus red NMH_MUTATE=P2C: the line requires P2(c) and goes red with it
//   (above).
// MUTATION P2-CONTRAST stimulus red NMH_MUTATE=P2CON: P2-CONTRAST: H1's periods and H2's
//   periods (VARBEAMEN=1) in the pooled-equality comparator of P2-CONTRAST in place of H1c's
//   and H2c's; the pooled periods are no longer all equal. Red on fx68k and tg68k (81 passed, 1
//   failed): P2-CONTRAST. Not graded: none.
// MUTATION P3/P4-LINES stimulus red NMH_MUTATE=X-NOBPL: P3/P4-LINES [SA]: SA with PLANE holding
//   only 32 lines of pattern instead of 256 (prog_s_sa_nobpl.bin, SHORT_PLANE); bitplane DMA
//   still runs, so the population is a live, reduced but nonempty one. Red on fx68k and tg68k
//   (65 passed, 1 failed): P3/P4-LINES [SA]. Not graded: A4, P3/P4(a) [SA], P3/P4(a) [SB],
//   P3/P4(a) [SC], P3/P4(a) [SA0], P3/P4(a) [SB0], P3/P4(a) [SC0], A5, P3(b), P3(c),
//   P3-CONTRAST, P3, P4(b), P4(c), P4-CONTRAST, P4. Measured: P3/P4-LINES [SA]: 32 class-0, 32
//   class-1, 64 total graded output lines.
// MUTATION P3/P4-LINES stimulus red NMH_MUTATE=X-NOBPL-SB: P3/P4-LINES [SB]: as X-NOBPL, for SB
//   (prog_s_sb_nobpl.bin). Red on fx68k and tg68k (65 passed, 1 failed): P3/P4-LINES [SB]. Not
//   graded: A4, P3/P4(a) [SA], P3/P4(a) [SB], P3/P4(a) [SC], P3/P4(a) [SA0], P3/P4(a) [SB0],
//   P3/P4(a) [SC0], A5, P3(b), P3(c), P3-CONTRAST, P3, P4(b), P4(c), P4-CONTRAST, P4. Measured:
//   P3/P4-LINES [SB]: 32 class-0, 32 class-1, 64 total graded output lines.
// MUTATION P3/P4-LINES stimulus red NMH_MUTATE=X-NOBPL-SC: P3/P4-LINES [SC]: as X-NOBPL, for SC
//   (prog_s_sc_nobpl.bin). Red on fx68k and tg68k (65 passed, 1 failed): P3/P4-LINES [SC]. Not
//   graded: A4, P3/P4(a) [SA], P3/P4(a) [SB], P3/P4(a) [SC], P3/P4(a) [SA0], P3/P4(a) [SB0],
//   P3/P4(a) [SC0], A5, P3(b), P3(c), P3-CONTRAST, P3, P4(b), P4(c), P4-CONTRAST, P4. Measured:
//   P3/P4-LINES [SC]: 32 class-0, 32 class-1, 64 total graded output lines.
// MUTATION P3/P4-LINES stimulus red NMH_MUTATE=X-NOBPL-SA0: P3/P4-LINES [SA0]: as X-NOBPL, for
//   SA0 (prog_s_sa0_nobpl.bin). Red on fx68k and tg68k (64 passed, 1 failed): P3/P4-LINES
//   [SA0]. Not graded: A4, P3/P4(a) [SA], P3/P4(a) [SB], P3/P4(a) [SC], P3/P4(a) [SA0],
//   P3/P4(a) [SB0], P3/P4(a) [SC0], A5-REF, A5, P3(b), P3(c), P3-CONTRAST, P3, P4(b), P4(c),
//   P4-CONTRAST, P4. Measured: P3/P4-LINES [SA0]: 32 class-0, 32 class-1, 64 total graded
//   output lines.
// MUTATION P3/P4-LINES stimulus red NMH_MUTATE=X-NOBPL-SB0: P3/P4-LINES [SB0]: as X-NOBPL, for
//   SB0 (prog_s_sb0_nobpl.bin). Red on fx68k and tg68k (64 passed, 1 failed): P3/P4-LINES
//   [SB0]. Not graded: A4, P3/P4(a) [SA], P3/P4(a) [SB], P3/P4(a) [SC], P3/P4(a) [SA0],
//   P3/P4(a) [SB0], P3/P4(a) [SC0], A5-REF, A5, P3(b), P3(c), P3-CONTRAST, P3, P4(b), P4(c),
//   P4-CONTRAST, P4. Measured: P3/P4-LINES [SB0]: 32 class-0, 32 class-1, 64 total graded
//   output lines.
// MUTATION P3/P4-LINES stimulus red NMH_MUTATE=X-NOBPL-SC0: P3/P4-LINES [SC0]: as X-NOBPL, for
//   SC0 (prog_s_sc0_nobpl.bin). Red on fx68k and tg68k (64 passed, 1 failed): P3/P4-LINES
//   [SC0]. Not graded: A4, P3/P4(a) [SA], P3/P4(a) [SB], P3/P4(a) [SC], P3/P4(a) [SA0],
//   P3/P4(a) [SB0], P3/P4(a) [SC0], A5-REF, A5, P3(b), P3(c), P3-CONTRAST, P3, P4(b), P4(c),
//   P4-CONTRAST, P4. Measured: P3/P4-LINES [SC0]: 32 class-0, 32 class-1, 64 total graded
//   output lines.
// MUTATION A4 stimulus red NMH_MUTATE=X-A4DOUBLE: A4: SA's plane data changed to 6 black, 6
//   white, 2 black and 6 white words per line (prog_s_sa_a4double.bin, A4_DOUBLE): every line
//   carries two transitions into white and two out of white, the first white span as wide as
//   the normal one, so the transition clause fails alone. Red on fx68k and tg68k (66 passed, 1
//   failed): A4. Not graded: P3/P4(a) [SA], P3/P4(a) [SB], P3/P4(a) [SC], P3/P4(a) [SA0],
//   P3/P4(a) [SB0], P3/P4(a) [SC0], A5, P3(b), P3(c), P3-CONTRAST, P3, P4(b), P4(c),
//   P4-CONTRAST, P4. Measured: P3/P4-LINES [SA]: 256 class-0, 256 class-1, 512 total graded
//   output lines; A4 class 0: 1536 lines, 256 without exactly one transition into and out of
//   white; white run per run SA=192 SB=192 SC=192 SA0=192 SB0=192 SC0=192 .
// MUTATION A4 stimulus red NMH_MUTATE=X-A4WIDTH: A4: SA's plane data changed to 14 black, 5
//   white and 1 black words per line (prog_s_sa_a4width.bin, A4_WIDTH): one transition into and
//   out of white per line, the white span a data word narrower, so the width clause fails
//   alone. Red on fx68k and tg68k (66 passed, 1 failed): A4. Not graded: P3/P4(a) [SA],
//   P3/P4(a) [SB], P3/P4(a) [SC], P3/P4(a) [SA0], P3/P4(a) [SB0], P3/P4(a) [SC0], A5, P3(b),
//   P3(c), P3-CONTRAST, P3, P4(b), P4(c), P4-CONTRAST, P4. Measured: P3/P4-LINES [SA]: 256
//   class-0, 256 class-1, 512 total graded output lines; A4 class 0: 1536 lines, 0 without
//   exactly one transition into and out of white; white run per run SA=160 SB=192 SC=192
//   SA0=192 SB0=192 SC0=192 .
// MUTATION P3/P4(a) stimulus red NMH_MUTATE=X-SHIFT-SA: P3/P4(a) [SA]: SA's plane data from
//   line 128 on carries the white span one data word earlier, same width (prog_s_sa_shift.bin,
//   SHIFT_ROWS): dL and dT are no longer the same in every graded line. P3 and P4 require
//   P3/P4(a) [SA] and go red with it. Red on fx68k and tg68k (75 passed, 3 failed): P3/P4(a)
//   [SA], P3, P4. Not graded: P3(b), P3(c), P4(b), P4(c). Measured: P3/P4(a) [SA] class 0:
//   dL=633..665 dT=505..537.
// MUTATION P3 stimulus red NMH_MUTATE=X-SHIFT-SA: the line requires P3/P4(a) [SA] and goes red
//   with it (above).
// MUTATION P4 stimulus red NMH_MUTATE=X-SHIFT-SA: the line requires P3/P4(a) [SA] and goes red
//   with it (above).
// MUTATION P3/P4(a) stimulus red NMH_MUTATE=X-SHIFT-SB: P3/P4(a) [SB]: as X-SHIFT-SA, for SB
//   (prog_s_sb_shift.bin). P3 requires P3/P4(a) [SB]. Red on fx68k and tg68k (78 passed, 2
//   failed): P3/P4(a) [SB], P3. Not graded: P3(b), P3(c). Measured: P3/P4(a) [SB] class 0:
//   dL=569..601 dT=505..537.
// MUTATION P3 stimulus red NMH_MUTATE=X-SHIFT-SB: the line requires P3/P4(a) [SB] and goes red
//   with it (above).
// MUTATION P3/P4(a) stimulus red NMH_MUTATE=X-SHIFT-SC: P3/P4(a) [SC]: as X-SHIFT-SA, for SC
//   (prog_s_sc_shift.bin). P4 requires P3/P4(a) [SC]. Red on fx68k and tg68k (78 passed, 2
//   failed): P3/P4(a) [SC], P4. Not graded: P4(b), P4(c). Measured: P3/P4(a) [SC] class 0:
//   dL=633..665 dT=441..473.
// MUTATION P4 stimulus red NMH_MUTATE=X-SHIFT-SC: the line requires P3/P4(a) [SC] and goes red
//   with it (above).
// MUTATION P3/P4(a) stimulus red NMH_MUTATE=X-SHIFT-SA0: P3/P4(a) [SA0]: as X-SHIFT-SA, for SA0
//   (prog_s_sa0_shift.bin). P3 and P4 require P3/P4(a) [SA0]; A5 is gated on it. Red on fx68k
//   and tg68k (76 passed, 3 failed): P3/P4(a) [SA0], P3, P4. Not graded: A5, P3-CONTRAST,
//   P4-CONTRAST. Measured: P3/P4(a) [SA0] class 0: dL=623..655 dT=557..589.
// MUTATION P3 stimulus red NMH_MUTATE=X-SHIFT-SA0: the line requires P3/P4(a) [SA0] and goes
//   red with it (above).
// MUTATION P4 stimulus red NMH_MUTATE=X-SHIFT-SA0: the line requires P3/P4(a) [SA0] and goes
//   red with it (above).
// MUTATION P3/P4(a) stimulus red NMH_MUTATE=X-SHIFT-SB0: P3/P4(a) [SB0]: as X-SHIFT-SA, for SB0
//   (prog_s_sb0_shift.bin). P3 requires P3/P4(a) [SB0]; A5 is gated on it. Red on fx68k and
//   tg68k (78 passed, 2 failed): P3/P4(a) [SB0], P3. Not graded: A5, P3-CONTRAST. Measured:
//   P3/P4(a) [SB0] class 0: dL=623..655 dT=557..589.
// MUTATION P3 stimulus red NMH_MUTATE=X-SHIFT-SB0: the line requires P3/P4(a) [SB0] and goes
//   red with it (above).
// MUTATION P3/P4(a) stimulus red NMH_MUTATE=X-SHIFT-SC0: P3/P4(a) [SC0]: as X-SHIFT-SA, for SC0
//   (prog_s_sc0_shift.bin). P4 requires P3/P4(a) [SC0]; A5 is gated on it. Red on fx68k and
//   tg68k (78 passed, 2 failed): P3/P4(a) [SC0], P4. Not graded: A5, P4-CONTRAST. Measured:
//   P3/P4(a) [SC0] class 0: dL=623..655 dT=557..589.
// MUTATION P4 stimulus red NMH_MUTATE=X-SHIFT-SC0: the line requires P3/P4(a) [SC0] and goes
//   red with it (above).
// MUTATION A5-REF stimulus red NMH_MUTATE=X-A5REF: A5-REF: SA0 with audio channel 0 DMA started
//   on a one-word buffer at Chip address 0 (prog_s_sa0_a5ref.bin, AUD0_AT0; p.145, period 124
//   per p.140/p.266): reads of word 0 join the groups, which no longer have exactly 4 accesses.
//   Red on fx68k and tg68k (80 passed, 1 failed): A5-REF. Not graded: A5. Measured: A5-REF
//   [SA0]: 314 refresh groups in the graded field, 288 with other than 4 accesses, 0
//   first-access distances other than 227 or 228 CCK (fx68k), 314 refresh groups in the graded
//   field, 287 with other than 4 accesses, 0 first-access distances other than 227 or 228 CCK
//   (tg68k).
// MUTATION A5 stimulus red NMH_MUTATE=A5: A5: SB's graded lines (VARHSYEN=1, HSSTRT moved) in
//   place of SB0's in A5's unchanged exact-equality comparator; the landmark's distance to the
//   nearest refresh access then differs from SA0's and SC0's in both parity classes. Red on
//   fx68k and tg68k (81 passed, 1 failed): A5. Not graded: none. Measured: A5 class 0
//   (VARHSYEN=0): dMR SA0=1681 SB0=1735 SC0=1681.
// MUTATION P3(c) stimulus red NMH_MUTATE=P3C: P3(c): SB replaced by a run with HSSTOP changed
//   as well (prog_s_sbc.bin, s=$0020, e=$0040): the trailing edge's distance moves, so clause
//   (c) fails alone. P3 requires P3(c). Red on fx68k and tg68k (80 passed, 2 failed): P3(c),
//   P3. Not graded: none. Measured: P3 class 0: dL(SA)=665 dL(SB)=601 dT(SA)=537 dT(SB)=473
//   dL(SA0)=655 dL(SB0)=655 dT(SA0)=589 dT(SB0)=589.
// MUTATION P3 stimulus red NMH_MUTATE=P3C: the line requires P3(c) and goes red with it
//   (above).
// MUTATION P4(c) stimulus red NMH_MUTATE=P4C: P4(c): SC replaced by a run with HSSTRT changed
//   as well (prog_s_scc.bin, s=$0020, e=$0040): the leading edge's distance moves, so clause
//   (c) fails alone. P4 requires P4(c). Red on fx68k and tg68k (80 passed, 2 failed): P4(c),
//   P4. Not graded: none. Measured: P4 class 0: dT(SA)=537 dT(SC)=473 dL(SA)=665 dL(SC)=601
//   dL(SA0)=655 dL(SC0)=655 dT(SA0)=589 dT(SC0)=589.
// MUTATION P4 stimulus red NMH_MUTATE=P4C: the line requires P4(c) and goes red with it
//   (above).
// MUTATION P3(b) stimulus red NMH_MUTATE=P3-NOMOVE: P3(b): SA's graded lines in place of SB's
//   in P3(b) (s equal to SA's, so no move). P3 requires P3(b). Red on fx68k and tg68k (80
//   passed, 2 failed): P3(b), P3. Not graded: none.
// MUTATION P3 stimulus red NMH_MUTATE=P3-NOMOVE: the line requires P3(b) and goes red with it
//   (above).
// MUTATION P4(b) stimulus red NMH_MUTATE=P4-NOMOVE: P4(b): SA's graded lines in place of SC's
//   in P4(b) (e equal to SA's, so no move). P4 requires P4(b). Red on fx68k and tg68k (80
//   passed, 2 failed): P4(b), P4. Not graded: none.
// MUTATION P4 stimulus red NMH_MUTATE=P4-NOMOVE: the line requires P4(b) and goes red with it
//   (above).
// MUTATION P3-CONTRAST stimulus red NMH_MUTATE=P3CON: P3-CONTRAST: SB's graded lines
//   (VARHSYEN=1) in place of SB0's in P3-CONTRAST. P3 requires P3-CONTRAST. Red on fx68k and
//   tg68k (80 passed, 2 failed): P3-CONTRAST, P3. Not graded: none.
// MUTATION P3 stimulus red NMH_MUTATE=P3CON: the line requires P3-CONTRAST and goes red with it
//   (above).
// MUTATION P4-CONTRAST stimulus red NMH_MUTATE=P4CON: P4-CONTRAST: SC's graded lines
//   (VARHSYEN=1) in place of SC0's in P4-CONTRAST. P4 requires P4-CONTRAST. Red on fx68k and
//   tg68k (80 passed, 2 failed): P4-CONTRAST, P4. Not graded: none.
// MUTATION P4 stimulus red NMH_MUTATE=P4CON: the line requires P4-CONTRAST and goes red with it
//   (above).
// MUTATION NMH_MUTATE stimulus red NMH_MUTATE=BOGUS: a selector name no branch of the test
//   knows (NMH_MUTATE: unknown selector); every other line is the normal run's. Red on fx68k
//   (82 passed, 1 failed): NMH_MUTATE; on tg68k (82 passed, 1 failed): NMH_MUTATE.

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

struct Checks {
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

// A graded line runs only when every precondition it names holds. Otherwise
// one "precondition not met" line names the ones that do not, and the line
// is neither green nor red.
bool gate(Checks& checks, const std::string& label,
          const std::vector<std::pair<std::string, bool>>& pre) {
    std::string missing;
    for (const auto& p : pre)
        if (!p.second) { if (!missing.empty()) missing += ", "; missing += p.first; }
    if (missing.empty()) return true;
    checks.record(label + ": precondition not met (" + missing + ")");
    return false;
}

// One colour clock ("memory access cycle", p.194) is 8 clk_sys cycles.
// Neither the page nor the harness header defines this ratio; it is
// measured, and it is checked at runtime by anchor A3 (its measured periods
// must convert to 227 or 228, the page's own PAL line length,
// SPECIFICATION.md sec.3). Every other check that consumes this ratio
// (P1-COVER, P2-LINES, A2, P2(a)/(b)/(c), A5-REF) is gated on A3.
constexpr uint64_t kCck = 8;

// nanomig.i's DONE macro: the last thing every program does, after the
// measured work, is store DONE_VALUE at DONE_ADDR and hold. Every capture
// below asserts this write happened.
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;

// ---- Program H addresses (prog_h.s) -----------------------------------
constexpr uint32_t kHMark1   = 0x0C0100;
constexpr uint32_t kHMark2   = 0x0C0102;
constexpr uint32_t kHId      = 0x0C0104;
constexpr uint32_t kHFix     = 0x0C0106;
constexpr uint16_t kHFixValue = 0xC3A5;
constexpr uint32_t kHSamples = 0x0C4000;
constexpr size_t   kHSampleCount = 16384;

// ---- Program S addresses (prog_s.s) ------------------------------------
constexpr uint32_t kSMark1 = 0x0C0100;
constexpr uint32_t kSId    = 0x0C0102;

// Every program in this package is well under 512 bytes; PROGRAM_ORG is
// fixed at $080400 (nanomig.i). A read in this range is a CPU instruction
// fetch, not a DMA access.
constexpr uint32_t kProgLoW = 0x080400 / 2;
constexpr uint32_t kProgHiW = (0x080400 + 0x800) / 2;

constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kHRunBudget = 4'000'000;
constexpr uint64_t kSRunBudget = 4'000'000;

nmh::Config default_cfg() { return nmh::Config{}; }               // ECS, PAL (chipset_config=0x08)
nmh::Config ocs_cfg()     { nmh::Config c; c.chipset_config = 0x00; return c; }  // OCS, PAL
nmh::Config ntsc_cfg()    { nmh::Config c; c.chipset_config = 0x0A; return c; }  // ECS, NTSC

// PC-ID: p.299's enhanced-Agnus PAL chip-identification value, accepted
// under either radix reading of the manual's un-prefixed digits (page
// note 9).
bool pc_id_pal(uint16_t id) { uint16_t f = (id >> 8) & 0x7F; return f == 0x20 || f == 0x14; }

std::string hex_word(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "%04X", v);
    return std::string(buf);
}

bool all_equal(const std::vector<uint64_t>& v) {
    if (v.empty()) return false;
    for (auto p : v) if (p != v[0]) return false;
    return true;
}

// Anchor A3 (SPECIFICATION.md P2 sec.7): every graded period, not just the
// first, must be exactly divisible by kCck and convert to 227 or 228 CCK
// (p.23's PAL count, page note 3).
bool a3_all_ok(const std::vector<uint64_t>& periods) {
    if (periods.empty()) return false;
    for (auto raw : periods) {
        if (raw % kCck != 0) return false;
        uint64_t v = raw / kCck;
        if (v != 227 && v != 228) return false;
    }
    return true;
}

// ==== Program H capture ===================================================

struct HCapture {
    bool loaded = false;
    uint16_t id = 0;
    std::optional<uint64_t> mark1, mark2, done;
    bool fix_ok = false;
    uint16_t fix = 0;
    std::vector<std::pair<uint64_t, uint16_t>> samples;  // (cycle, raw VHPOSR value), store order
    std::vector<nmh::RamAccess> log;
};

HCapture run_h(const std::string& bin, const nmh::Config& cfg) {
    HCapture c;
    nmh::NanomigHarness h(cfg);
    if (h.load_program(bin) == 0) return c;
    c.loaded = true;
    h.boot(kBootBudget);
    h.run_cycles(kHRunBudget);
    c.log = h.ram_log();
    if (auto w = h.peek_logical(kHId)) c.id = *w;
    if (auto w = h.peek_logical(kHFix)) { c.fix = *w; c.fix_ok = true; }

    const uint32_t mark1_w = kHMark1 / 2, mark2_w = kHMark2 / 2, done_w = kDoneAddr / 2;
    const uint32_t samp_lo = kHSamples / 2, samp_hi = samp_lo + (uint32_t)kHSampleCount;
    for (const auto& a : c.log) {
        if (!a.is_write) continue;
        if (a.word_addr == mark1_w && a.data == 0xA5A5 && !c.mark1) { c.mark1 = a.cycle; continue; }
        if (a.word_addr == mark2_w && a.data == 0x5A5A && !c.mark2) { c.mark2 = a.cycle; continue; }
        if (a.word_addr == done_w && a.data == kDoneValue && !c.done) { c.done = a.cycle; continue; }
        if (a.word_addr >= samp_lo && a.word_addr < samp_hi) c.samples.push_back({a.cycle, a.data});
    }
    return c;
}

bool is_cpu_access_h(const nmh::RamAccess& a) {
    if (!a.is_write) return a.word_addr >= kProgLoW && a.word_addr < kProgHiW;
    const uint32_t mark1_w = kHMark1 / 2, mark2_w = kHMark2 / 2, id_w = kHId / 2, fix_w = kHFix / 2,
                   done_w = kDoneAddr / 2;
    const uint32_t samp_lo = kHSamples / 2, samp_hi = samp_lo + (uint32_t)kHSampleCount;
    return a.word_addr == mark1_w || a.word_addr == mark2_w || a.word_addr == id_w || a.word_addr == fix_w ||
           a.word_addr == done_w || (a.word_addr >= samp_lo && a.word_addr < samp_hi);
}

// Refresh groups: a maximal run of non-CPU RAM-port accesses whose
// neighbours are less than 32 CCK apart (SPECIFICATION.md P2 sec.3).
std::vector<std::vector<uint64_t>> refresh_groups(const std::vector<nmh::RamAccess>& log,
                                                    uint64_t lo, uint64_t hi) {
    // Deliberately not coalesced: two accesses at the same address are two
    // accesses, and anchor A2's per-group count depends on counting each.
    std::vector<uint64_t> cycles;
    for (const auto& a : log)
        if (a.cycle > lo && a.cycle < hi && !is_cpu_access_h(a)) cycles.push_back(a.cycle);
    std::sort(cycles.begin(), cycles.end());
    std::vector<std::vector<uint64_t>> groups;
    for (auto c : cycles) {
        if (groups.empty() || c - groups.back().back() >= 32 * kCck) groups.push_back({c});
        else groups.back().push_back(c);
    }
    return groups;
}

// Result of P2's per-run analysis: graded periods (clk_sys cycles between
// consecutive graded groups' first access) and whether every graded group
// has exactly 4 members (anchor A2).
struct P2Data {
    bool ok = false;
    std::vector<uint64_t> periods;
    bool a2_all_four = false;
};

P2Data analyze_p2(const HCapture& c) {
    P2Data d;
    if (!c.loaded || !c.mark1 || !c.mark2) return d;
    auto groups = refresh_groups(c.log, *c.mark1, *c.mark2);
    // first group starting after MARK1:
    size_t idx0 = groups.size();
    for (size_t i = 0; i < groups.size(); ++i)
        if (groups[i].front() > *c.mark1) { idx0 = i; break; }
    if (idx0 == groups.size() || idx0 + 1 >= groups.size()) return d;
    size_t first = idx0 + 1;  // the *second* group that starts after MARK1
    size_t last = groups.size();
    for (size_t i = first; i < groups.size(); ++i)
        if (groups[i].front() < *c.mark2) last = i; else break;
    if (last == groups.size() || last <= first) return d;
    d.ok = true;
    d.a2_all_four = true;
    // groups[first..last-1] each start one graded period and their own members
    // populate it; groups[last] only marks where the last graded period ends
    // (SPECIFICATION.md P2 sec.3), so its own size is not itself graded.
    for (size_t i = first; i < last; ++i) if (groups[i].size() != 4) d.a2_all_four = false;
    for (size_t i = first; i < last; ++i) d.periods.push_back(groups[i + 1].front() - groups[i].front());
    return d;
}

// The graded samples of a Program H capture: H (bits 7-0 of VHPOSR), the
// store cycle, and the wraps (a sample whose H is smaller than the one
// before it); the graded range starts at the second wrap.
struct Extracted {
    bool ok = false;
    std::vector<uint16_t> hf;
    std::vector<uint64_t> cyc;
    std::vector<size_t> wraps;
};

Extracted extract_h(const HCapture& c) {
    Extracted e;
    if (!c.loaded || c.samples.empty()) return e;
    e.hf.reserve(c.samples.size());
    e.cyc.reserve(c.samples.size());
    for (auto& s : c.samples) { e.hf.push_back(s.second & 0x00FF); e.cyc.push_back(s.first); }
    for (size_t i = 1; i < e.hf.size(); ++i) if (e.hf[i] < e.hf[i - 1]) e.wraps.push_back(i);
    e.ok = e.wraps.size() >= 2;
    return e;
}

struct HRun {
    const char* label = "";
    HCapture cap;
    Extracted ex;
    P2Data d;
    bool pc = false;       // PC-ID and PC-MARK
    bool lines1 = false;   // P1-LINES
    bool lines2 = false;   // P2-LINES
    bool cover = false;    // P1-COVER
};

// ==== Program S capture ==================================================

struct SCapture {
    bool loaded = false;
    uint16_t id = 0;
    std::optional<uint64_t> mark1, done;
    std::vector<nmh::VideoSample> video;
    // Anchor A5-REF: the reads of Chip word 0, grouped; R is a group's first access.
    std::vector<std::vector<uint64_t>> refresh_groups;
    std::vector<uint64_t> refresh;
};

// The reads of Chip word 0 are this test's selection of the memory-refresh
// accesses of a Program S run, where bitplane and Copper DMA share the port
// and "every non-CPU access" is no longer refresh. Anchor A5-REF checks the
// selection against the pages: four accesses per line (p.194, p.285), the
// first accesses of consecutive lines 227 or 228 CCK apart (p.23, p.304).
// Grouped the way refresh_groups() groups, 32 CCK apart.
std::vector<std::vector<uint64_t>> refresh_groups_s(const std::vector<nmh::RamAccess>& log) {
    std::vector<uint64_t> cycles;
    for (const auto& a : log) if (!a.is_write && a.word_addr == 0) cycles.push_back(a.cycle);
    std::sort(cycles.begin(), cycles.end());
    std::vector<std::vector<uint64_t>> groups;
    for (auto c : cycles) {
        if (groups.empty() || c - groups.back().back() >= 32 * kCck) groups.push_back({c});
        else groups.back().push_back(c);
    }
    return groups;
}

SCapture run_s(const std::string& bin, const nmh::Config& cfg) {
    SCapture c;
    nmh::NanomigHarness h(cfg);
    if (h.load_program(bin) == 0) return c;
    c.loaded = true;
    h.boot(kBootBudget);
    h.run_cycles(kSRunBudget);
    if (auto w = h.peek_logical(kSId)) c.id = *w;
    const uint32_t mark1_w = kSMark1 / 2, done_w = kDoneAddr / 2;
    for (const auto& a : h.ram_log()) {
        if (!a.is_write) continue;
        if (a.word_addr == mark1_w && a.data == 0xA5A5 && !c.mark1) { c.mark1 = a.cycle; continue; }
        if (a.word_addr == done_w && a.data == kDoneValue && !c.done) { c.done = a.cycle; continue; }
    }
    c.refresh_groups = refresh_groups_s(h.ram_log());
    for (const auto& g : c.refresh_groups) c.refresh.push_back(g.front());
    c.video = h.video();
    return c;
}

bool is_white(const nmh::VideoSample& s) { return s.r == 15 && s.g == 15 && s.b == 15; }

// The graded field: the span (nmh::find_frames) between the second and
// third output VSYNC after MARK1 (SPECIFICATION.md P3/P4 sec.3).
std::optional<nmh::Span> graded_field(const std::vector<nmh::VideoSample>& v, uint64_t mark1) {
    auto frames = nmh::find_frames(v);
    size_t first = frames.size();
    for (size_t i = 0; i < frames.size(); ++i)
        if (frames[i].begin > mark1) { first = i; break; }
    if (first == frames.size() || first + 1 >= frames.size()) return std::nullopt;
    return frames[first + 1];
}

// The output line's own reference frame (SPECIFICATION.md P3 sec.7): "The
// output line runs from one HSYNC leading edge to the next." The pulse
// level is whichever level is held for less than half of the graded field
// (measured once over the whole field, since the polarity is a property of
// the run, not of one line). A leading edge is a transition INTO that
// level; for an active-low pulse that is a falling (1->0) transition, for
// an active-high pulse a rising (0->1) transition -- deliberately not
// assumed either way.
bool pulse_is_zero_over(const std::vector<nmh::VideoSample>& v, nmh::Span field) {
    size_t zeros = 0, ones = 0;
    for (size_t i = field.begin; i < field.end && i < v.size(); ++i) (v[i].hs ? ones : zeros)++;
    return zeros < ones;
}

std::vector<size_t> leading_edges(const std::vector<nmh::VideoSample>& v, nmh::Span field, bool pulse_is_zero) {
    std::vector<size_t> edges;
    size_t lo = field.begin > 0 ? field.begin - 1 : 0;
    size_t hi = std::min(field.end + 1, v.size());
    for (size_t i = std::max<size_t>(lo, 1); i < hi; ++i) {
        bool prev = v[i - 1].hs, cur = v[i].hs;
        if (pulse_is_zero ? (prev && !cur) : (!prev && cur)) edges.push_back(i);
    }
    return edges;
}

struct LineMetric {
    bool ok = false;
    // dL, dT: the landmark M's distance to L and to T (SPECIFICATION.md P3
    // sec.3). dMR: M's own distance to R, the nearest memory-refresh access
    // before M (anchor A5, sec.7).
    int64_t dL = 0, dT = 0, dMR = 0;
    int64_t white_run = -1;
    int into_white = 0, out_white = 0;
};

// One canonical output line: [L, nextL). T is the first opposite-polarity
// transition strictly between them (a two-level signal has exactly one
// between two consecutive leading edges); M is the first white pixel after
// L. R, passed in by the caller (graded_lines_of()), is the nearest
// memory-refresh access before M.
LineMetric measure_span(const std::vector<nmh::VideoSample>& v, size_t L, size_t nextL, bool pulse_is_zero,
                         uint64_t R) {
    LineMetric m;
    if (nextL <= L + 1 || nextL > v.size()) return m;
    size_t T = SIZE_MAX;
    for (size_t i = L + 1; i < nextL; ++i) {
        bool prev = v[i - 1].hs, cur = v[i].hs;
        bool trailing = pulse_is_zero ? (!prev && cur) : (prev && !cur);
        if (trailing) { T = i; break; }
    }
    if (T == SIZE_MAX) return m;

    size_t M = SIZE_MAX, white_end = SIZE_MAX;
    for (size_t i = L + 1; i < nextL; ++i) {
        bool w = is_white(v[i]), pw = is_white(v[i - 1]);
        if (!pw && w) { m.into_white++; if (M == SIZE_MAX) M = i; }
        if (pw && !w) { m.out_white++; if (M != SIZE_MAX && white_end == SIZE_MAX) white_end = i; }
    }
    if (M == SIZE_MAX) return m;

    m.dL = (int64_t)M - (int64_t)L;
    m.dT = (int64_t)M - (int64_t)T;
    m.dMR = (int64_t)M - (int64_t)R;
    if (white_end != SIZE_MAX) m.white_run = (int64_t)(white_end - M);
    m.ok = true;
    return m;
}

// Graded lines of one S run, split into the two doubled-line parity
// classes (SPECIFICATION.md P3 sec.7, "Doubled lines"). One entry per
// output line that carries white: the first white span of the line is its
// landmark. A white span whose line falls outside the graded field, or that
// has no refresh access before it, is not graded; a lost line shows as a
// population short of P3/P4-LINES' exact 256 per class.
struct GradedLines {
    bool ok = false;
    std::vector<LineMetric> cls[2];
    size_t total = 0;
};

GradedLines graded_lines_of(const SCapture& c) {
    GradedLines g;
    if (!c.loaded || !c.mark1) return g;
    auto field = graded_field(c.video, *c.mark1);
    if (!field) return g;
    bool pulse_is_zero = pulse_is_zero_over(c.video, *field);
    auto edges = leading_edges(c.video, *field, pulse_is_zero);

    // Anchor A5 (SPECIFICATION.md P3 sec.7): white content (M) is driven
    // by DDFSTRT/DIWSTRT/PLANE data alone, never by HSSTRT/HSSTOP, so its
    // absolute position is identical for "the same" chip line across all
    // six S runs -- unlike a count of L-edges from the field's own start,
    // which HSSTRT can shift enough to move a boundary line in or out of
    // the field differently between runs. M is therefore found first, by a
    // direct scan of the field's own video content, not by iterating per-L
    // windows; each M then independently selects R (the nearest refresh
    // access before M) and its own enclosing [L, nextL) window.
    size_t idx = 0;
    size_t last_L = SIZE_MAX;
    for (size_t i = std::max<size_t>(field->begin, 1); i < field->end && i < c.video.size(); ++i) {
        if (!is_white(c.video[i]) || is_white(c.video[i - 1])) continue;
        size_t M = i;

        auto lit = std::upper_bound(edges.begin(), edges.end(), M);
        if (lit == edges.begin() || lit == edges.end()) { ++idx; continue; }
        size_t L = *(lit - 1), nextL = *lit;
        if (L < field->begin || nextL > field->end) { ++idx; continue; }
        if (L == last_L) continue;  // a later white span of a line already taken
        last_L = L;

        auto rit = std::upper_bound(c.refresh.begin(), c.refresh.end(), (uint64_t)M);
        if (rit == c.refresh.begin()) { ++idx; continue; }
        uint64_t R = *(rit - 1);

        LineMetric m = measure_span(c.video, L, nextL, pulse_is_zero, R);
        if (m.ok) { g.cls[idx % 2].push_back(m); g.total++; }
        ++idx;
    }
    g.ok = true;
    return g;
}

// Anchor A5-REF: the accesses selected as refresh in a Program S run behave
// as the pages' refresh slots do, over the graded field.
struct RefAnchor {
    size_t groups = 0, bad_size = 0, bad_period = 0;
    bool ok = false;
};

RefAnchor ref_anchor_of(const SCapture& c) {
    RefAnchor a;
    if (!c.loaded || !c.mark1) return a;
    auto field = graded_field(c.video, *c.mark1);
    if (!field) return a;
    uint64_t prev = 0;
    bool have = false;
    for (const auto& g : c.refresh_groups) {
        uint64_t first = g.front();
        if (first < field->begin || first >= field->end) continue;
        ++a.groups;
        if (g.size() != 4) ++a.bad_size;
        if (have) {
            uint64_t p = first - prev;
            if (p % kCck != 0 || (p / kCck != 227 && p / kCck != 228)) ++a.bad_period;
        }
        prev = first;
        have = true;
    }
    a.ok = a.groups >= 256 && a.bad_size == 0 && a.bad_period == 0;
    return a;
}

int64_t median_of(std::vector<int64_t> v) {
    if (v.empty()) return 0;
    std::sort(v.begin(), v.end());
    return v[v.size() / 2];
}

// Per run and parity class: whether dL and dT are exactly the same in every
// graded line (P3/P4 clause (a): the pages give no tolerance band for a
// measured distance), and the value they have.
struct ClassStat {
    bool ok = false;        // the class has lines
    bool cons = false;      // dL and dT each take exactly one value
    int64_t dL = 0, dT = 0; // the class's median (the value when cons)
    int64_t dLlo = 0, dLhi = 0, dTlo = 0, dThi = 0;
    bool use() const { return ok && cons; }
};

ClassStat class_stat(const std::vector<LineMetric>& v) {
    ClassStat s;
    if (v.empty()) return s;
    std::vector<int64_t> dls, dts;
    for (const auto& m : v) { dls.push_back(m.dL); dts.push_back(m.dT); }
    auto l = std::minmax_element(dls.begin(), dls.end());
    auto t = std::minmax_element(dts.begin(), dts.end());
    s.ok = true;
    s.dLlo = *l.first; s.dLhi = *l.second; s.dTlo = *t.first; s.dThi = *t.second;
    s.cons = s.dLlo == s.dLhi && s.dTlo == s.dThi;
    s.dL = median_of(dls);
    s.dT = median_of(dts);
    return s;
}

// A5: the landmark M's distance to R within a class.
struct DmrStat {
    bool ok = false;
    int64_t v = 0;
};

DmrStat class_dmr(const std::vector<LineMetric>& cls) {
    DmrStat s;
    if (cls.empty()) return s;
    std::vector<int64_t> d;
    for (const auto& m : cls) d.push_back(m.dMR);
    auto mm = std::minmax_element(d.begin(), d.end());
    s.v = median_of(d);
    s.ok = *mm.first == *mm.second;
    return s;
}

// Anchor A4: every graded line of a class, in every run, has exactly one
// transition into white and one out of white, and a first white span of one
// and the same width. The check reads the white transitions and the width
// only, not the distances to the sync edges.
struct A4Class {
    bool ok = false;
    size_t lines = 0, bad = 0;
    int64_t wlo = INT64_MAX, whi = INT64_MIN;
};

A4Class a4_class(const std::vector<const GradedLines*>& runs, int cls) {
    A4Class a;
    for (const auto* g : runs)
        for (const auto& m : g->cls[cls]) {
            ++a.lines;
            if (m.into_white != 1 || m.out_white != 1 || m.white_run < 0) { ++a.bad; continue; }
            a.wlo = std::min(a.wlo, m.white_run);
            a.whi = std::max(a.whi, m.white_run);
        }
    a.ok = a.lines > 0 && a.bad == 0 && a.wlo == a.whi;
    return a;
}

std::string width_range(const GradedLines& g) {
    int64_t lo = INT64_MAX, hi = INT64_MIN;
    for (int c = 0; c < 2; ++c)
        for (const auto& m : g.cls[c]) {
            if (m.white_run < 0) continue;
            lo = std::min(lo, m.white_run);
            hi = std::max(hi, m.white_run);
        }
    if (lo > hi) return "n/a";
    return lo == hi ? std::to_string(lo) : std::to_string(lo) + ".." + std::to_string(hi);
}

std::string range_str(int64_t lo, int64_t hi) {
    return lo == hi ? std::to_string(lo) : std::to_string(lo) + ".." + std::to_string(hi);
}

// ---- Selector tables -------------------------------------------------------

// Precondition selectors: the named run's capture is replaced at the source
// by the mutant program (and configuration: 0 default, 1 OCS, 2 NTSC).
struct HSub { const char* sel; int run; const char* bin; int cfg; };
const HSub kHSubs[] = {
    {"FIXTURE",    0, "prog_h_h1_fixture.bin", 0},
    {"X-OCS",      0, "prog_h_h1.bin", 1},
    {"X-NTSC",     0, "prog_h_h1.bin", 2},
    {"X-HALT-H",   0, "prog_h_h1_halt.bin", 0},
    {"X-NOMARK2",  0, "prog_h_h1_nomark2.bin", 0},
    {"X-NODONE",   0, "prog_h_h1_nodone.bin", 0},
    {"X-SHORT",     0, "prog_h_h1_short.bin", 0},
    {"X-SHORT-H2",  1, "prog_h_h2_short.bin", 0},
    {"X-SHORT-H1C", 2, "prog_h_h1c_short.bin", 0},
    {"X-SHORT-H2C", 3, "prog_h_h2c_short.bin", 0},
    {"X-LATE",      0, "prog_h_h1_late.bin", 0},
    {"X-LATE-H2",   1, "prog_h_h2_late.bin", 0},
    {"X-LATE-H1C",  2, "prog_h_h1c_late.bin", 0},
    {"X-LATE-H2C",  3, "prog_h_h2c_late.bin", 0},
    {"X-BURST",     0, "prog_h_h1_burst.bin", 0},
    {"X-BURST-H2",  1, "prog_h_h2_burst.bin", 0},
    {"X-AUD0",      0, "prog_h_h1_aud0.bin", 0},
    {"X-AUD0-H2",   1, "prog_h_h2_aud0.bin", 0},
    {"X-AUD0-H1C",  2, "prog_h_h1c_aud0.bin", 0},
    {"X-AUD0-H2C",  3, "prog_h_h2c_aud0.bin", 0},
};

struct SSub { const char* sel; int run; const char* bin; };
const SSub kSSubs[] = {
    {"X-HALT-S",    0, "prog_s_sa_halt.bin"},
    {"X-NOBPL",     0, "prog_s_sa_nobpl.bin"},
    {"X-NOBPL-SB",  1, "prog_s_sb_nobpl.bin"},
    {"X-NOBPL-SC",  2, "prog_s_sc_nobpl.bin"},
    {"X-NOBPL-SA0", 3, "prog_s_sa0_nobpl.bin"},
    {"X-NOBPL-SB0", 4, "prog_s_sb0_nobpl.bin"},
    {"X-NOBPL-SC0", 5, "prog_s_sc0_nobpl.bin"},
    {"X-A4DOUBLE",  0, "prog_s_sa_a4double.bin"},
    {"X-A4WIDTH",   0, "prog_s_sa_a4width.bin"},
    {"X-SHIFT-SA",  0, "prog_s_sa_shift.bin"},
    {"X-SHIFT-SB",  1, "prog_s_sb_shift.bin"},
    {"X-SHIFT-SC",  2, "prog_s_sc_shift.bin"},
    {"X-SHIFT-SA0", 3, "prog_s_sa0_shift.bin"},
    {"X-SHIFT-SB0", 4, "prog_s_sb0_shift.bin"},
    {"X-SHIFT-SC0", 5, "prog_s_sc0_shift.bin"},
    {"X-A5REF",     3, "prog_s_sa0_a5ref.bin"},
    {"P3C",         1, "prog_s_sbc.bin"},
    {"P4C",         2, "prog_s_scc.bin"},
};

// Proposition selectors: one proposition line's input is replaced, nothing
// else. P2A and P2A-H2 also run a capture of their own (prog_h_h1_retotal,
// prog_h_h2_retotal).
const char* const kPropSels[] = {
    "A1", "A1-H2", "A3", "A3-H2", "P1", "P1-H2", "P1C", "P1C-H2",
    "P2A", "P2A-H2", "P2B", "P2C", "P2CON",
    "A5", "P3-NOMOVE", "P4-NOMOVE", "P3CON", "P4CON",
};

bool known_selector(const std::string& s) {
    if (s.empty()) return true;
    for (const auto& x : kHSubs) if (s == x.sel) return true;
    for (const auto& x : kSSubs) if (s == x.sel) return true;
    for (const auto* x : kPropSels) if (s == x) return true;
    return false;
}

}  // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";
    if (!known_selector(mv)) checks.expect(false, "NMH_MUTATE: unknown selector " + mv);

    // ======================================================================
    // Program H group: P1, P2. Runs H1, H2, H1c, H2c (SPECIFICATION.md
    // sec.5) always execute; H1c/H2c are the page's contrasting case
    // (VARBEAMEN=0), graded as lines of their own.
    // ======================================================================

    const char* const h_label[4] = {"H1", "H2", "H1c", "H2c"};
    const char* const h_bin[4] = {"prog_h_h1.bin", "prog_h_h2.bin", "prog_h_h1c.bin", "prog_h_h2c.bin"};
    const uint16_t h_n[4] = {0x00B0, 0x00C0, 0x00B0, 0x00C0};
    HRun hr[4];
    for (int i = 0; i < 4; ++i) {
        std::string bin = h_bin[i];
        nmh::Config cfg = default_cfg();
        for (const auto& s : kHSubs)
            if (mv == s.sel && s.run == i) {
                bin = s.bin;
                cfg = s.cfg == 1 ? ocs_cfg() : s.cfg == 2 ? ntsc_cfg() : default_cfg();
            }
        hr[i].label = h_label[i];
        hr[i].cap = run_h(dir + "/" + bin, cfg);
        hr[i].ex = extract_h(hr[i].cap);
        hr[i].d = analyze_p2(hr[i].cap);
    }
    HRun& H1 = hr[0]; HRun& H2 = hr[1]; HRun& H1c = hr[2]; HRun& H2c = hr[3];

    // ---- Fixture: a program-stored constant asserted first, through
    // peek_logical() -- the same call PC-ID's ID depends on -- before
    // anything else reads one. MUTATION FIXTURE: see header.
    checks.expect(H1.cap.loaded && H1.cap.fix_ok && H1.cap.fix == kHFixValue,
        "FIXTURE: peek_logical(FIXCONST) [H1] == $C3A5, the literal Program H stores before anything else is read back");

    // ---- Claims without a proposition (SPECIFICATION.md sec.1) ----------
    checks.record(
        "opus:8-appA-oos.c1-WITHDRAWN [p280]: HBSTOP -- no page states the "
        "condition under which the programmed position takes effect, or what "
        "HBLANK does to R/G/B; not testable at the video output.");
    checks.record(
        "opus:8-appA-oos.c2-WITHDRAWN [p280]: HBSTRT -- as c1.");
    checks.record(
        "opus:8-appA-oos.c3-WITHDRAWN [p280]: HCENTER -- no page names the "
        "enabling condition, the field or the VSYNC edge it positions; the "
        "effect lies in sub-line VSYNC timing the test sees only after the "
        "line doubler.");
    checks.record(
        "opus:8-display.c7-WITHDRAWN [p260]: \"analog RGB\" names a connector "
        "and a kind of monitor; the test sees digital R/G/B and separate H/V "
        "sync, never a connector, and the colour count follows from the "
        "harness's fixed 4-bit outputs, not from the chips.");
    checks.record(
        "opus:8-appA-oos.c4-WITHDRAWN [p280]: HSSTOP -- the claim bundles "
        "write-only, Agnus, ECS-mark and the address with the stop-position "
        "functional effect (SPECIFICATION.md sec.1, \"Parts of c4, c5 and c6 "
        "that have no proposition of their own\"); none of the bundled parts "
        "is independently observable. The claim also says which edge HSSTOP "
        "sets (sync stop); P4 cannot show that: the video output the test "
        "sees has passed a line doubler that re-times the chip's lines, so no "
        "page-anchored reference for which edge moved survives to this "
        "boundary. P4 grades, unlabelled, the move of dT = M - T relative to "
        "the picture (the landmark M), a stated limit.");
    checks.record(
        "opus:8-appA-oos.c5-WITHDRAWN [p280]: HSSTRT -- as c4, for the "
        "start-position effect. The claim also says which edge HSSTRT sets "
        "(sync start); P3 cannot show that, for the reason given at c4: the "
        "line doubler re-times the chip's lines, so no page-anchored "
        "reference for which edge moved survives to this boundary. P3 "
        "grades, unlabelled, the move of dL = M - L relative to the picture "
        "(the landmark M), a stated limit.");
    checks.record(
        "opus:8-appA-oos.c6-WITHDRAWN [p280]: HTOTAL -- as c4, for the "
        "highest-count effect. P1 and P2 grade the functional effect alone, "
        "unlabelled.");

    // ---- PC-ID, PC-MARK, DONE: shared preconditions, every H-group run --
    for (auto& r : hr) {
        const std::string tag = std::string("[") + r.label + "]";
        bool id_ok = r.cap.loaded && pc_id_pal(r.cap.id);
        if (r.cap.loaded)
            checks.note("PC-ID " + tag + ": VPOSR bits 14-8 = $" + hex_word((r.cap.id >> 8) & 0x7F).substr(2));
        checks.expect(id_ok,
            "PC-ID " + tag + " p.299: VPOSR bits 14-8 read the enhanced-Agnus "
            "PAL chip-identification value ($20 or 20 decimal)");
        bool mark_ok = r.cap.loaded && r.cap.mark1.has_value() &&
                       r.cap.mark2.has_value() && *r.cap.mark2 > *r.cap.mark1;
        checks.expect(mark_ok, "PC-MARK " + tag + ": MARK1 appears at the port, and MARK2 later");
        checks.expect(r.cap.loaded && r.cap.done.has_value(), "DONE " + tag + ": the program reaches DONE");
        r.pc = id_ok && mark_ok;
    }

    // ---- P1-LINES: the graded samples contain at least 320 wraps, in every
    // H-group run (the contrast runs' population gates their contrast).
    for (auto& r : hr) {
        const std::string tag = std::string("[") + r.label + "]";
        size_t wraps = r.ex.ok ? r.ex.wraps.size() - 1 : 0;
        checks.note("P1-LINES " + tag + ": " + std::to_string(wraps) + " graded wraps, " +
                    std::to_string(r.cap.samples.size()) + " samples");
        if (gate(checks, "P1-LINES " + tag, {{"PC-ID and PC-MARK " + tag, r.pc}})) {
            r.lines1 = r.ex.ok && wraps >= 320;
            checks.expect(r.lines1, "P1-LINES " + tag + ": the graded samples contain at least 320 wraps");
        }
    }

    // ---- Anchor A1: in H1c/H2c (VARBEAMEN=0), the largest graded H lies
    // between $E2-16 and $E3 (p.23). MUTATION A1/A1-H2: see header.
    for (int i = 2; i < 4; ++i) {
        HRun& r = hr[i];
        const std::string tag = std::string("[") + r.label + "]";
        const Extracted& ex = (i == 2 && mv == "A1") ? H1.ex : (i == 3 && mv == "A1-H2") ? H2.ex : r.ex;
        if (gate(checks, "A1 " + tag, {{"PC-ID and PC-MARK " + tag, r.pc}, {"P1-LINES " + tag, r.lines1}})) {
            uint16_t maxH = 0;
            if (ex.ok) for (size_t k = ex.wraps[1]; k < ex.hf.size(); ++k) maxH = std::max(maxH, ex.hf[k]);
            checks.note("A1 " + tag + ": max graded H=$" + hex_word(maxH));
            checks.expect(ex.ok && maxH >= (0xE2 - 16) && maxH <= 0xE3,
                "A1 " + tag + " [p23]: the largest graded H with VARBEAMEN=0 lies between $E2-16 and $E3");
        }
    }

    // ---- Anchor A3: establishes kCck. Every graded period (not just the
    // first) must be exactly divisible by kCck and convert to 227 or 228
    // CCK (p.23). MUTATION A3/A3-H2: see header.
    bool a3[2] = {false, false};  // [H1c], [H2c]
    for (int i = 2; i < 4; ++i) {
        HRun& r = hr[i];
        const std::string tag = std::string("[") + r.label + "]";
        const std::vector<uint64_t>& periods =
            (i == 2 && mv == "A3") ? H1.d.periods : (i == 3 && mv == "A3-H2") ? H2.d.periods : r.d.periods;
        if (gate(checks, "A3 " + tag, {{"PC-ID and PC-MARK " + tag, r.pc}})) {
            a3[i - 2] = a3_all_ok(periods);
            checks.expect(a3[i - 2],
                "A3 " + tag + " [p23,p194,p304]: every graded period (VARBEAMEN=0) is exactly divisible by "
                "the clk_sys-per-CCK ratio and converts to 227 or 228 CCK");
        }
    }
    const bool kcck_ok = a3[0] && a3[1];  // every kCck consumer is gated on both A3 lines

    // ---- P1-COVER [H1]/[H2]: consecutive graded sample stores are never
    // more than 16 CCK apart, and at least one graded sample has H >= N-16.
    for (int i = 0; i < 2; ++i) {
        HRun& r = hr[i];
        const std::string tag = std::string("[") + r.label + "]";
        const Extracted& ex = r.ex;
        uint64_t max_gap = 0;
        bool near_end = false;
        if (ex.ok) {
            size_t gs = ex.wraps[1];
            for (size_t k = gs + 1; k < ex.cyc.size(); ++k) max_gap = std::max(max_gap, ex.cyc[k] - ex.cyc[k - 1]);
            for (size_t k = gs; k < ex.hf.size(); ++k) if (ex.hf[k] >= (uint16_t)(h_n[i] - 16)) near_end = true;
            checks.note("P1-COVER " + tag + ": largest consecutive-sample gap in the graded range = " +
                        std::to_string(max_gap / kCck) + " CCK");
        }
        if (gate(checks, "P1-COVER " + tag,
                 {{"PC-ID and PC-MARK " + tag, r.pc}, {"A3 [H1c] and A3 [H2c]", kcck_ok}})) {
            r.cover = ex.ok && max_gap <= 16 * kCck && near_end;
            checks.expect(r.cover,
                "P1-COVER " + tag + ": consecutive graded sample stores are never more than 16 CCK "
                "apart, and at least one graded sample has H >= N-16");
        }
    }

    // ---- P2-LINES: at least 320 graded periods, in every H-group run.
    for (auto& r : hr) {
        const std::string tag = std::string("[") + r.label + "]";
        checks.note("P2-LINES " + tag + ": " + std::to_string(r.d.periods.size()) + " graded periods");
        if (gate(checks, "P2-LINES " + tag,
                 {{"PC-ID and PC-MARK " + tag, r.pc}, {"A3 [H1c] and A3 [H2c]", kcck_ok}})) {
            r.lines2 = r.d.ok && r.d.periods.size() >= 320;
            checks.expect(r.lines2, "P2-LINES " + tag + ": at least 320 graded periods");
        }
    }

    // ---- Anchor A2: every graded period has exactly 4 refresh accesses.
    // MUTATION A2 stimulus red NMH_MUTATE=X-AUD0*: see header.
    for (auto& r : hr) {
        const std::string tag = std::string("[") + r.label + "]";
        if (gate(checks, "A2 " + tag,
                 {{"PC-ID and PC-MARK " + tag, r.pc}, {"P2-LINES " + tag, r.lines2}})) {
            checks.expect(r.d.ok && r.d.a2_all_four,
                "A2 " + tag + " [p194,p285]: every graded period contains exactly 4 refresh accesses");
        }
    }

    // ---- P1: with VARBEAMEN set, no H readback exceeds HTOTAL ------------
    // opus:8-appA-oos.c6 bundles this with address/write-only/Agnus/
    // ECS-mark clauses SPECIFICATION.md sec.1 declines, so no id is
    // carried here. MUTATION P1/P1-H2 (the proposition lines) and
    // P1C/P1C-H2 (the contrast lines): see header.
    for (int i = 0; i < 4; ++i) {
        HRun& r = hr[i];
        const bool contrast = i >= 2;
        const std::string tag = std::string("[") + r.label + "]";
        const std::string name = contrast ? "P1-CONTRAST " + tag : "P1 " + tag;
        const Extracted* src = &r.ex;
        if (!contrast && i == 0 && mv == "P1") src = &H1c.ex;
        if (!contrast && i == 1 && mv == "P1-H2") src = &H2c.ex;
        if (contrast && i == 2 && mv == "P1C") src = &H1.ex;
        if (contrast && i == 3 && mv == "P1C-H2") src = &H2.ex;
        const Extracted& ex = *src;
        std::vector<std::pair<std::string, bool>> pre = {
            {"PC-ID and PC-MARK " + tag, r.pc}, {"P1-LINES " + tag, r.lines1}};
        if (!contrast) pre.push_back({"P1-COVER " + tag, r.cover});
        if (!gate(checks, name, pre)) continue;
        if (!ex.ok) {
            checks.expect(false, name + " [p280]: fewer than two wraps observed, no graded range");
            continue;
        }
        uint16_t maxH = 0;
        bool any_exceeds = false;
        for (size_t k = ex.wraps[1]; k < ex.hf.size(); ++k) {
            maxH = std::max(maxH, ex.hf[k]);
            if (ex.hf[k] > h_n[i]) any_exceeds = true;
        }
        checks.note(name + ": max graded H=$" + hex_word(maxH) + ", N=$" + hex_word(h_n[i]));
        if (!contrast) {
            checks.expect(!any_exceeds,
                name + " [p280]: while VARBEAMEN is 1, no graded horizontal-position readback exceeds HTOTAL");
        } else {
            checks.expect(any_exceeds,
                name + " [p280] (VARBEAMEN=0): at least one graded readback exceeds N, since HTOTAL's "
                "condition does not hold");
        }
    }

    // ---- P2: with VARBEAMEN set, every line lasts N or N+1 CCK -----------
    // No single graded line establishes the whole proposition (each of
    // P2(a)/(b)/(c) grades one clause only); none of them carries
    // opus:8-appA-oos.c6 (sec.1). The conjunction below is the whole
    // proposition, still unlabelled for the same reason.
    {
        // The captures P2A and P2A-H2 substitute: HTOTAL written again
        // mid-loop. They are the input of P2(a) only, and print their own
        // liveness and population lines.
        HCapture h1_retotal, h2_retotal;
        auto live_h = [&](const HCapture& c, const std::string& tag) {
            checks.expect(c.loaded && pc_id_pal(c.id), "PC-ID [" + tag + "]: VPOSR bits 14-8 read the enhanced-Agnus PAL value");
            checks.expect(c.loaded && c.mark1.has_value() && c.mark2.has_value() && *c.mark2 > *c.mark1,
                "PC-MARK [" + tag + "]: MARK1 appears at the port, and MARK2 later");
            checks.expect(c.loaded && c.done.has_value(), "DONE [" + tag + "]: the program reaches DONE");
        };
        if (mv == "P2A") {
            h1_retotal = run_h(dir + "/prog_h_h1_retotal.bin", default_cfg());
            live_h(h1_retotal, "H1,P2A");
            P2Data dd = analyze_p2(h1_retotal);
            checks.note("P2-LINES [H1,P2A]: " + std::to_string(dd.periods.size()) + " graded periods");
            if (gate(checks, "P2-LINES [H1,P2A]", {{"A3 [H1c] and A3 [H2c]", kcck_ok}}))
                checks.expect(dd.ok && dd.periods.size() >= 320, "P2-LINES [H1,P2A]: at least 320 graded periods");
        }
        if (mv == "P2A-H2") {
            h2_retotal = run_h(dir + "/prog_h_h2_retotal.bin", default_cfg());
            live_h(h2_retotal, "H2,P2A-H2");
            P2Data dd = analyze_p2(h2_retotal);
            checks.note("P2-LINES [H2,P2A-H2]: " + std::to_string(dd.periods.size()) + " graded periods");
            if (gate(checks, "P2-LINES [H2,P2A-H2]", {{"A3 [H1c] and A3 [H2c]", kcck_ok}}))
                checks.expect(dd.ok && dd.periods.size() >= 320, "P2-LINES [H2,P2A-H2]: at least 320 graded periods");
        }
        const P2Data d1_for_a = (mv == "P2A") ? analyze_p2(h1_retotal) : H1.d;
        const P2Data d2_for_a = (mv == "P2A-H2") ? analyze_p2(h2_retotal) : H2.d;

        for (const auto& r : hr)
            if (r.d.ok && !r.d.periods.empty())
                checks.note(std::string("P2 [") + r.label + "]: " + std::to_string(r.d.periods.size()) +
                            " graded periods, first=" + std::to_string(r.d.periods[0]) + " clk_sys cycles");
        if (mv == "P2A" && d1_for_a.ok && !d1_for_a.periods.empty())
            checks.note("P2 [H1,P2A]: " + std::to_string(d1_for_a.periods.size()) + " graded periods, first=" +
                        std::to_string(d1_for_a.periods[0]) + " clk_sys cycles");
        if (mv == "P2A-H2" && d2_for_a.ok && !d2_for_a.periods.empty())
            checks.note("P2 [H2,P2A-H2]: " + std::to_string(d2_for_a.periods.size()) + " graded periods, first=" +
                        std::to_string(d2_for_a.periods[0]) + " clk_sys cycles");

        // P2(a) [H1], [H2]. MUTATION P2(a) NMH_MUTATE=P2A / P2A-H2: see header.
        bool p2a[2] = {false, false};
        for (int i = 0; i < 2; ++i) {
            HRun& r = hr[i];
            const std::string tag = std::string("[") + r.label + "]";
            const P2Data& d = i == 0 ? d1_for_a : d2_for_a;
            if (gate(checks, "P2(a) " + tag,
                     {{"PC-ID and PC-MARK " + tag, r.pc}, {"P2-LINES " + tag, r.lines2}})) {
                p2a[i] = d.ok && !d.periods.empty() && all_equal(d.periods);
                checks.expect(p2a[i],
                    "P2(a) " + tag + " [p280,p304]: while VARBEAMEN is 1, every graded line period has one "
                    "and the same value");
            }
        }

        const uint64_t T1_raw = H1.d.ok && !H1.d.periods.empty() ? H1.d.periods[0] : 0;
        const uint64_t T2_raw = H2.d.ok && !H2.d.periods.empty() ? H2.d.periods[0] : 0;
        const uint64_t T1c_raw = H1c.d.ok && !H1c.d.periods.empty() ? H1c.d.periods[0] : 0;
        const uint64_t T2c_raw = H2c.d.ok && !H2c.d.periods.empty() ? H2c.d.periods[0] : 0;
        const std::vector<std::pair<std::string, bool>> pre_t = {
            {"PC-ID and PC-MARK [H1]", H1.pc}, {"PC-ID and PC-MARK [H2]", H2.pc},
            {"P2-LINES [H1]", H1.lines2}, {"P2-LINES [H2]", H2.lines2}};

        // P2(b). MUTATION P2(b) stimulus red NMH_MUTATE=P2B: see header.
        bool p2b = false;
        {
            uint64_t t1 = (mv == "P2B") ? T1c_raw : T1_raw;
            uint64_t t2 = (mv == "P2B") ? T2c_raw : T2_raw;
            if (gate(checks, "P2(b)", pre_t)) {
                p2b = (int64_t)(t2 - t1) == 16 * (int64_t)kCck;
                checks.expect(p2b, "P2(b) [p280,p304]: T2 - T1 = 16 CCK ($C0 - $B0)");
            }
        }

        // P2(c). MUTATION P2(c) stimulus red NMH_MUTATE=P2C: see header.
        bool p2c = false;
        {
            uint64_t t1 = (mv == "P2C") ? T1c_raw : T1_raw;
            uint64_t t1_cck = t1 / kCck;
            if (gate(checks, "P2(c)", pre_t)) {
                p2c = t1 != 0 && (t1_cck >= 0x00B0) && (t1_cck - 0x00B0 <= 1);
                checks.expect(p2c, "P2(c) [p280,p304]: T1 (converted to CCK) - $B0 is 0 or 1 (page note 2)");
            }
        }

        // Conjunction: the whole proposition, held only when P2(a) [H1],
        // P2(a) [H2], P2(b) and P2(c) all hold together.
        if (gate(checks, "P2", pre_t)) {
            checks.expect(p2a[0] && p2a[1] && p2b && p2c,
                "P2 [p280,p304]: while VARBEAMEN is 1, every line has one and the same length, that "
                "length is N or N+1 CCK for HTOTAL=N, and changing HTOTAL from $B0 to $C0 (d = 16, the "
                "one pair run) changes the length by exactly d (conjunction of P2(a) [H1], P2(a) "
                "[H2], P2(b), P2(c))");
        }

        // P2-CONTRAST: every graded period in H1c equals every graded period
        // in H2c (not just periods[0]), through the unenabled comparator.
        // MUTATION P2-CONTRAST NMH_MUTATE=P2CON: H1's and H2's periods
        // (VARBEAMEN=1) in place of H1c's and H2c's.
        if (gate(checks, "P2-CONTRAST",
                 {{"PC-ID and PC-MARK [H1c]", H1c.pc}, {"PC-ID and PC-MARK [H2c]", H2c.pc},
                  {"P2-LINES [H1c]", H1c.lines2}, {"P2-LINES [H2c]", H2c.lines2}})) {
            const std::vector<uint64_t>& pool1 = (mv == "P2CON") ? H1.d.periods : H1c.d.periods;
            const std::vector<uint64_t>& pool2 = (mv == "P2CON") ? H2.d.periods : H2c.d.periods;
            std::vector<uint64_t> pooled = pool1;
            pooled.insert(pooled.end(), pool2.begin(), pool2.end());
            checks.expect(H1c.d.ok && !H1c.d.periods.empty() && H2c.d.ok && !H2c.d.periods.empty() &&
                          all_equal(pooled),
                "P2-CONTRAST [p280,p304] (VARBEAMEN=0): every graded period in H1c equals every "
                "graded period in H2c -- HTOTAL has no effect");
        }
    }

    // ======================================================================
    // Program S group: P3, P4. Runs SA, SB, SC, SA0, SB0, SC0
    // (SPECIFICATION.md sec.5) always execute; SA0/SB0/SC0 are the page's
    // contrasting case (VARHSYEN=0).
    // ======================================================================

    enum { SA = 0, SB = 1, SC = 2, SA0 = 3, SB0 = 4, SC0 = 5 };
    const char* const s_label[6] = {"SA", "SB", "SC", "SA0", "SB0", "SC0"};
    const char* const s_bin[6] = {"prog_s_sa.bin", "prog_s_sb.bin", "prog_s_sc.bin",
                                  "prog_s_sa0.bin", "prog_s_sb0.bin", "prog_s_sc0.bin"};
    SCapture scap[6];
    GradedLines sg[6];
    bool s_pc[6] = {false, false, false, false, false, false};
    bool s_lines[6] = {false, false, false, false, false, false};
    for (int i = 0; i < 6; ++i) {
        std::string bin = s_bin[i];
        for (const auto& s : kSSubs) if (mv == s.sel && s.run == i) bin = s.bin;
        scap[i] = run_s(dir + "/" + bin, default_cfg());
        sg[i] = graded_lines_of(scap[i]);
    }

    // ---- PC-ID, PC-MARK, DONE: shared preconditions, every S-group run --
    for (int i = 0; i < 6; ++i) {
        const SCapture& c = scap[i];
        const std::string tag = std::string("[") + s_label[i] + "]";
        bool id_ok = c.loaded && pc_id_pal(c.id);
        if (c.loaded)
            checks.note("PC-ID " + tag + ": VPOSR bits 14-8 = $" + hex_word((c.id >> 8) & 0x7F).substr(2));
        checks.expect(id_ok,
            "PC-ID " + tag + " p.299: VPOSR bits 14-8 read the enhanced-Agnus PAL chip-identification "
            "value ($20 or 20 decimal)");
        bool mark_ok = c.loaded && c.mark1.has_value();
        checks.expect(mark_ok, "PC-MARK " + tag + ": MARK1 appears at the port");
        checks.expect(c.loaded && c.done.has_value(), "DONE " + tag + ": the program reaches DONE");
        s_pc[i] = id_ok && mark_ok;
    }

    // ---- P3/P4-LINES: p.64's programmed playfield is 256 chip lines, and
    // the harness doubles each into two output lines, so the exact
    // page/harness-anchored population is 256 lines in each parity class.
    for (int i = 0; i < 6; ++i) {
        const std::string tag = std::string("[") + s_label[i] + "]";
        checks.note("P3/P4-LINES " + tag + ": " + std::to_string(sg[i].cls[0].size()) + " class-0, " +
                    std::to_string(sg[i].cls[1].size()) + " class-1, " + std::to_string(sg[i].total) +
                    " total graded output lines");
        if (gate(checks, "P3/P4-LINES " + tag, {{"PC-ID and PC-MARK " + tag, s_pc[i]}})) {
            s_lines[i] = sg[i].ok && sg[i].cls[0].size() == 256 && sg[i].cls[1].size() == 256;
            checks.expect(s_lines[i],
                "P3/P4-LINES " + tag + " [p64]: exactly 256 graded output lines in each parity class "
                "(512 total)");
        }
    }
    auto pre_run = [&](int i) {
        std::vector<std::pair<std::string, bool>> v;
        const std::string tag = std::string("[") + s_label[i] + "]";
        v.push_back({"PC-ID and PC-MARK " + tag, s_pc[i]});
        v.push_back({"P3/P4-LINES " + tag, s_lines[i]});
        return v;
    };

    // ---- Anchor A4: line well-formedness and white-run-width consistency,
    // all six S runs together, both parity classes (SPECIFICATION.md P3
    // sec.7). Exact invariant: the pages give no tolerance band for the
    // white run's width, so it must match exactly, not to within an invented
    // +-1. It reads the white transitions and the width only, so it does not
    // depend on the clauses P3 and P4 grade. MUTATION A4 stimulus red
    // NMH_MUTATE=X-A4DOUBLE (the transition clause) and X-A4WIDTH (the width
    // clause): see header.
    bool a4_ok = false;
    {
        std::vector<std::pair<std::string, bool>> pre;
        for (int i = 0; i < 6; ++i) for (auto& p : pre_run(i)) pre.push_back(p);
        std::vector<const GradedLines*> all;
        for (int i = 0; i < 6; ++i) all.push_back(&sg[i]);
        A4Class a4c[2] = {a4_class(all, 0), a4_class(all, 1)};
        for (int cls = 0; cls < 2; ++cls) {
            std::string dump;
            for (int i = 0; i < 6; ++i) dump += std::string(s_label[i]) + "=" + width_range(sg[i]) + " ";
            checks.note("A4 class " + std::to_string(cls) + ": " + std::to_string(a4c[cls].lines) + " lines, " +
                        std::to_string(a4c[cls].bad) + " without exactly one transition into and out of "
                        "white; white run per run " + dump);
        }
        if (gate(checks, "A4", pre)) {
            a4_ok = a4c[0].ok && a4c[1].ok;
            checks.expect(a4_ok,
                "A4 [p59,p60]: every graded line has exactly one transition into white and one out of "
                "white, and the white run's length is exactly the same across all six S runs, in both "
                "parity classes");
        }
    }

    // ---- P3/P4 clause (a), per run: dL and dT are exactly the same in every
    // graded line of the run, in at least one parity class. MUTATION
    // P3/P4(a) stimulus red NMH_MUTATE=X-SHIFT-<run>: see header.
    ClassStat st[6][2];
    bool a_ok[6] = {false, false, false, false, false, false};
    for (int i = 0; i < 6; ++i) {
        for (int c = 0; c < 2; ++c) st[i][c] = class_stat(sg[i].cls[c]);
        const std::string tag = std::string("[") + s_label[i] + "]";
        std::vector<std::pair<std::string, bool>> pre = pre_run(i);
        pre.push_back({"A4", a4_ok});
        for (int c = 0; c < 2; ++c)
            checks.note("P3/P4(a) " + tag + " class " + std::to_string(c) + ": dL=" +
                        range_str(st[i][c].dLlo, st[i][c].dLhi) + " dT=" + range_str(st[i][c].dTlo, st[i][c].dThi));
        if (gate(checks, "P3/P4(a) " + tag, pre)) {
            a_ok[i] = st[i][0].use() || st[i][1].use();
            checks.expect(a_ok[i],
                "P3/P4(a) " + tag + " [p280,p304,p305]: dL (the landmark's distance to the leading edge) and "
                "dT (its distance to the trailing edge) are exactly the same in every graded line of "
                "the run, in at least one parity class");
        }
    }

    // ---- Anchor A5-REF: the accesses selected as refresh in the three
    // VARHSYEN=0 runs behave as the pages' refresh slots (p.194, p.285; p.23,
    // p.304). MUTATION A5-REF stimulus red NMH_MUTATE=X-A5REF: see header.
    bool a5ref_ok = false;
    {
        RefAnchor ra[3] = {ref_anchor_of(scap[SA0]), ref_anchor_of(scap[SB0]), ref_anchor_of(scap[SC0])};
        for (int k = 0; k < 3; ++k)
            checks.note(std::string("A5-REF [") + s_label[SA0 + k] + "]: " + std::to_string(ra[k].groups) +
                        " refresh groups in the graded field, " + std::to_string(ra[k].bad_size) +
                        " with other than 4 accesses, " + std::to_string(ra[k].bad_period) +
                        " first-access distances other than 227 or 228 CCK");
        std::vector<std::pair<std::string, bool>> pre;
        for (int i = SA0; i <= SC0; ++i) for (auto& p : pre_run(i)) pre.push_back(p);
        pre.push_back({"A3 [H1c] and A3 [H2c]", kcck_ok});
        if (gate(checks, "A5-REF", pre)) {
            a5ref_ok = ra[0].ok && ra[1].ok && ra[2].ok;
            checks.expect(a5ref_ok,
                "A5-REF [p194,p285,p23,p304]: in SA0, SB0 and SC0 the accesses selected as refresh form "
                "groups of exactly 4 per line, at least 256 lines in the graded field, and the first "
                "accesses of consecutive groups are 227 or 228 CCK apart");
        }
    }

    // ---- Anchor A5: the landmark M's own position relative to R, the
    // nearest memory-refresh access before it (p.194, p.285) -- a landmark
    // independent of DIWSTRT, DDFSTRT, HSSTRT and HSSTOP alike, used to
    // check that M itself does not move. Graded on the three VARHSYEN=0
    // contrast runs (SA0/SB0/SC0), where nothing is expected to move: if
    // dMR (M-R) is not identical there, the apparatus itself is not
    // precise enough to use as an independent reference at all. It is
    // exact, not to a tolerance the pages do not give, and both parity
    // classes must hold.
    {
        DmrStat dA0[2], dB0[2], dC0[2];
        bool both = true;
        for (int cls = 0; cls < 2; ++cls) {
            // MUTATION A5 NMH_MUTATE=A5: see header -- SB's live lines in place of SB0's.
            dA0[cls] = class_dmr(sg[SA0].cls[cls]);
            dB0[cls] = class_dmr((mv == "A5") ? sg[SB].cls[cls] : sg[SB0].cls[cls]);
            dC0[cls] = class_dmr(sg[SC0].cls[cls]);
            checks.note("A5 class " + std::to_string(cls) + " (VARHSYEN=0): dMR SA0=" +
                        (dA0[cls].ok ? std::to_string(dA0[cls].v) : std::string("n/a")) + " SB0=" +
                        (dB0[cls].ok ? std::to_string(dB0[cls].v) : std::string("n/a")) + " SC0=" +
                        (dC0[cls].ok ? std::to_string(dC0[cls].v) : std::string("n/a")));
            if (!(dA0[cls].ok && dB0[cls].ok && dC0[cls].ok && dA0[cls].v == dB0[cls].v &&
                  dB0[cls].v == dC0[cls].v))
                both = false;
        }
        std::vector<std::pair<std::string, bool>> pre;
        for (int i = SA0; i <= SC0; ++i) for (auto& p : pre_run(i)) pre.push_back(p);
        pre.push_back({"A5-REF", a5ref_ok});
        pre.push_back({"P3/P4(a) [SA0]", a_ok[SA0]});
        pre.push_back({"P3/P4(a) [SB0]", a_ok[SB0]});
        pre.push_back({"P3/P4(a) [SC0]", a_ok[SC0]});
        if (gate(checks, "A5", pre)) {
            checks.expect(both,
                "A5 [p280,p305,p194,p285]: with VARHSYEN=0 (p.280 \"(VARHSY)\", p.305 VARHSYEN: "
                "nothing expected to move), the landmark's distance to the nearest memory-refresh "
                "access (the reference, p.194, p.285) is exactly the same in SA0, SB0 and SC0, in "
                "both parity classes");
        }
        // With VARHSYEN=1, dMR is printed but not graded: the output has
        // passed a line doubler that re-times the chip's lines, so a
        // reference fixed in the chip's line cannot be used to attribute the
        // live SA/SB/SC move to a specific edge at this boundary
        // (SPECIFICATION.md P3 sec.7).
        for (int cls = 0; cls < 2; ++cls) {
            DmrStat mA = class_dmr(sg[SA].cls[cls]), mB = class_dmr(sg[SB].cls[cls]), mC = class_dmr(sg[SC].cls[cls]);
            checks.note("A5 class " + std::to_string(cls) + " (VARHSYEN=1, not graded): dMR SA=" +
                        (mA.ok ? std::to_string(mA.v) : std::string("n/a")) + " SB=" +
                        (mB.ok ? std::to_string(mB.v) : std::string("n/a")) + " SC=" +
                        (mC.ok ? std::to_string(mC.v) : std::string("n/a")));
        }
        checks.record(
            "A5-NOT-DISPOSITIVE: with VARHSYEN=1, dMR itself changes between SA/SB/SC even though "
            "DIWSTRT/DDFSTRT (hence M's chip-side position) are unchanged, because the video output "
            "the test sees has passed a line doubler that re-times the chip's lines; no "
            "page-anchored reference for which edge moved survives to this boundary. A5 cannot "
            "attribute P3's or P4's live move to a specific edge, only confirm the apparatus is "
            "exact when nothing moves (above).");
    }

    // ---- P3: HSSTRT alone sets the pulse's leading edge -------------------
    // dL, dT are the landmark M's own distance to L and to T
    // (SPECIFICATION.md P3 sec.3); anchor A5 (above) confirms the apparatus
    // is exact when nothing moves, but cannot attribute which of L or M
    // moved once HSSTRT genuinely changes the sync timing (recorded above,
    // A5-NOT-DISPOSITIVE); P3 is graded in the picture-relative frame, a
    // stated limit. The pages give no size or direction for the move, so it
    // is graded as exact inequality, and the untouched edge as exact
    // equality (sec.6 point 4). One line per clause, each graded in at least
    // one parity class; the conjunction requires one class in which all hold.
    // MUTATION P3-NOMOVE (b), P3C (c), P3CON (contrast): see header.
    // P4 is the same with HSSTOP and SC.
    struct Prop {
        const char* name;       // "P3" or "P4"
        int moved;              // SB or SC
        int contrast;           // SB0 or SC0
        bool moves_dL;          // P3: the leading edge (dL) moves; P4: dT
        const char* sel_nomove; const char* sel_con;
        const char* edge_moved; const char* edge_kept;
    };
    const Prop props[2] = {
        {"P3", SB, SB0, true,  "P3-NOMOVE", "P3CON", "dL (the landmark's distance to the leading edge)",
         "dT (its distance to the trailing edge)"},
        {"P4", SC, SC0, false, "P4-NOMOVE", "P4CON", "dT (the landmark's distance to the trailing edge)",
         "dL (its distance to the leading edge)"},
    };
    for (const auto& p : props) {
        const std::string name = p.name;
        const std::string mt = std::string("[") + s_label[SA] + "]", mm = std::string("[") + s_label[p.moved] + "]",
                          mc = std::string("[") + s_label[p.contrast] + "]", ma0 = std::string("[") + s_label[SA0] + "]";
        auto pick = [&](const ClassStat& s) { return p.moves_dL ? s.dL : s.dT; };       // the moved edge's distance
        auto keep = [&](const ClassStat& s) { return p.moves_dL ? s.dT : s.dL; };       // the untouched edge's distance
        bool b_cls[2], c_cls[2], con_cls[2];
        for (int c = 0; c < 2; ++c) {
            // (b): the moved edge's distance differs between SA and the moved run.
            const ClassStat& sm_b = (mv == p.sel_nomove) ? st[SA][c] : st[p.moved][c];
            b_cls[c] = st[SA][c].use() && sm_b.use() && pick(sm_b) != pick(st[SA][c]);
            // (c): the untouched edge's distance is exactly unchanged.
            c_cls[c] = st[SA][c].use() && st[p.moved][c].use() && keep(st[p.moved][c]) == keep(st[SA][c]);
            // contrast: with VARHSYEN=0 neither distance changes.
            const ClassStat& sm_c = (mv == p.sel_con) ? st[p.moved][c] : st[p.contrast][c];
            con_cls[c] = st[SA0][c].use() && sm_c.use() && sm_c.dL == st[SA0][c].dL && sm_c.dT == st[SA0][c].dT;
            checks.note(name + " class " + std::to_string(c) + ": " + (p.moves_dL ? "dL" : "dT") + "(SA)=" +
                        std::to_string(pick(st[SA][c])) + " " + (p.moves_dL ? "dL" : "dT") + "(" + s_label[p.moved] +
                        ")=" + std::to_string(pick(sm_b)) + " " + (p.moves_dL ? "dT" : "dL") + "(SA)=" +
                        std::to_string(keep(st[SA][c])) + " " + (p.moves_dL ? "dT" : "dL") + "(" + s_label[p.moved] +
                        ")=" + std::to_string(keep(st[p.moved][c])) + " dL(SA0)=" + std::to_string(st[SA0][c].dL) +
                        " dL(" + s_label[p.contrast] + ")=" + std::to_string(sm_c.dL) + " dT(SA0)=" +
                        std::to_string(st[SA0][c].dT) + " dT(" + s_label[p.contrast] + ")=" + std::to_string(sm_c.dT));
        }
        auto any = [](const bool* v) { return v[0] || v[1]; };

        const std::vector<std::pair<std::string, bool>> pre_move = {
            {"PC-ID and PC-MARK [SA]", s_pc[SA]}, {"P3/P4-LINES [SA]", s_lines[SA]},
            {std::string("PC-ID and PC-MARK ") + mm, s_pc[p.moved]}, {std::string("P3/P4-LINES ") + mm, s_lines[p.moved]},
            {"A4", a4_ok}, {"P3/P4(a) [SA]", a_ok[SA]}, {std::string("P3/P4(a) ") + mm, a_ok[p.moved]}};
        const std::vector<std::pair<std::string, bool>> pre_con = {
            {"PC-ID and PC-MARK [SA0]", s_pc[SA0]}, {"P3/P4-LINES [SA0]", s_lines[SA0]},
            {std::string("PC-ID and PC-MARK ") + mc, s_pc[p.contrast]}, {std::string("P3/P4-LINES ") + mc, s_lines[p.contrast]},
            {"A4", a4_ok}, {"P3/P4(a) [SA0]", a_ok[SA0]}, {std::string("P3/P4(a) ") + mc, a_ok[p.contrast]}};
        std::vector<std::pair<std::string, bool>> pre_all = {
            {"PC-ID and PC-MARK [SA]", s_pc[SA]}, {std::string("PC-ID and PC-MARK ") + mm, s_pc[p.moved]},
            {"PC-ID and PC-MARK [SA0]", s_pc[SA0]}, {std::string("PC-ID and PC-MARK ") + mc, s_pc[p.contrast]},
            {"P3/P4-LINES [SA]", s_lines[SA]}, {std::string("P3/P4-LINES ") + mm, s_lines[p.moved]},
            {"P3/P4-LINES [SA0]", s_lines[SA0]}, {std::string("P3/P4-LINES ") + mc, s_lines[p.contrast]},
            {"A4", a4_ok}};

        const std::string edge_phrase = std::string(p.edge_moved);
        const std::string moved_run = s_label[p.moved];
        if (gate(checks, name + "(b)", pre_move))
            checks.expect(any(b_cls),
                name + "(b) [p280,p304,p305]: while VARHSYEN and VARBEAMEN are both 1, " + edge_phrase +
                " differs between SA and " + moved_run + " (exact inequality), in at least one parity class");
        if (gate(checks, name + "(c)", pre_move))
            checks.expect(any(c_cls),
                name + "(c) [p280,p304,p305]: " + std::string(p.edge_kept) + " is exactly unchanged between SA and " +
                moved_run + ", in at least one parity class");
        if (gate(checks, name + "-CONTRAST", pre_con))
            checks.expect(any(con_cls),
                name + "-CONTRAST [p280,p305] (VARHSYEN=0): neither dL nor dT differs between SA0 and " +
                std::string(s_label[p.contrast]) + " (exact equality), in at least one parity class");
        if (gate(checks, name, pre_all)) {
            bool conj = false;
            for (int c = 0; c < 2; ++c)
                if (st[SA][c].use() && st[p.moved][c].use() && st[SA0][c].use() && st[p.contrast][c].use() &&
                    b_cls[c] && c_cls[c] && con_cls[c])
                    conj = true;
            checks.expect(conj,
                name + " [p280,p304,p305]: while VARHSYEN and VARBEAMEN are both 1, " + edge_phrase +
                " differs between SA and " + moved_run + " (exact inequality) and " + std::string(p.edge_kept) +
                " is exactly unchanged, and with VARHSYEN=0 neither changes, in at least one parity class; "
                "graded relative to the picture (the landmark M): the line doubler re-times the chip's lines, so "
                "this does not show which edge moved (A5-NOT-DISPOSITIVE) (conjunction of P3/P4(a) for the "
                "four runs, " + name + "(b), " + name + "(c), " + name + "-CONTRAST in one class)");
        }
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
