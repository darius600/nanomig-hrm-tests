// test_appF_cia_system.cpp -- appF-cia-system. SPECIFICATION.md gives each
// assertion's proposition, program, stimulus and expected value; this file
// implements them.
//
// NMH_MUTATE selects a mutated program (or a mutated selection of the same
// programs) in place of its normal counterpart, one assertion or precondition
// at a time; a plain `make test` selects none. Every mutant changes the
// stimulus only: the comparator and the expected value stay. A mutant is
// built from the same body (p1_body.s, p2_body.s, ...) as its normal
// counterpart with one wrapper symbol changed, so only the one instance named
// in the record is substituted.
//
// A "// MUTATION <label> <kind> <result> <text>" record names the label an
// expect() call prints. A line that is red in the normal run has a record
// "stimulus green" that names its live control, <label>-GREENCTL; the control's
// own record is "stimulus red". NMH_MUTATE may name several selectors, joined by
// commas, when a run replays records whose selectors change different programs.
//
// Selectors that choose the same program take effect one per run (the P2-*-GREENCTL selectors; EP1-PER-RUN,
// EP2-PER-RUN, EP3-PER-RUN and F1-ANCHOR-PER-RUN), and P1-PRE14 removes CIAB/timer B from the population of the
// P2 controls, so it does not share a run with them.
//
// A P10 row joins the verdicts of its members and cannot print PASS on this
// implementation: CIA-A's PRB read-back is not live (Pre1.4/O1), so the row's
// members on its instances are never graded, and the INMODE=CNT and mode 11 members
// grade only part of their claim (FAIL or not graded). Its record is "none
// impossible"; it names, as the demonstration of the join, the selectors that falsify
// the preconditions of the members that fail, with which the row prints RECORDED, not
// graded, instead of FAIL, and the other lines that run moves. The result token
// "green" is kept for a printed PASS.
//
// MUTATION FIXTURE stimulus red the program stores $BEE0 as its fixture word (p1_ciaa_ta_fixture.bin): the delivered word is not $BEEF, via NMH_MUTATE=FIXTURE
// MUTATION EP1 stimulus red a write to CIAA CRA precedes SETUP's own read (ep1_contrast.bin): the read returns $0A instead of $00, via NMH_MUTATE=EP1
// MUTATION EP2 stimulus red Copper DMA started in the window (ep2_contrast.bin): the Copper's own fetch of $0C3000 appears at the Chip-memory port, via NMH_MUTATE=EP2
// MUTATION EP2 stimulus red Copper DMA started from a list in low Chip memory (ep2_contrast_low.bin): the Copper's own fetch lands below $1000 outside a refresh group (the run's MEASURED line gives the count), via NMH_MUTATE=EP2-LOW
// MUTATION EP3 stimulus red the program completes its work and stores $600E as its final marker (p1_ciaa_ta_ep3.bin): no DONE_VALUE write, via NMH_MUTATE=EP3
// MUTATION F1-ANCHOR stimulus red shorter lines (HTOTAL, BEAMCON0) set after the first marker (p1_ciaa_ta_f1.bin): consecutive refresh line marks are no longer 454*T_CPU apart, via NMH_MUTATE=F1-ANCHOR; F1-ANCHOR-CONTRAST is red with it, because it needs F1 to hold
// MUTATION F1-ANCHOR-CONTRAST none impossible unfalsifiable at this boundary: the line only compares the measured line marks against 1.25*T_CPU, and no stimulus makes line marks 454 or 456 apart at 1.25*T_CPU while they are not at T_CPU
// MUTATION EP1-PER-RUN stimulus red the CIAA/timer-B P1 program writes CIAA CRA before its first read of it (p1_ciaa_tb_ep1.bin): that capture's EP1 words are not all $00, via NMH_MUTATE=EP1-PER-RUN
// MUTATION EP2-PER-RUN stimulus red the CIAA/timer-B P1 program starts Copper DMA from a list in low Chip memory (p1_ciaa_tb_ep2.bin): a non-refresh low-address fetch in that capture's window, via NMH_MUTATE=EP2-PER-RUN; on TG68K the fetch falls inside a refresh group and F1-ANCHOR-PER-RUN is red with it
// MUTATION EP3-PER-RUN stimulus red the CIAA/timer-B P1 program stores $600E as its final marker (p1_ciaa_tb_ep3.bin): that capture has no DONE_VALUE write, via NMH_MUTATE=EP3-PER-RUN; EP2-PER-RUN is then not graded (that capture does not complete)
// MUTATION F1-ANCHOR-PER-RUN stimulus red the CIAA/timer-B P1 program sets shorter lines after the first marker (p1_ciaa_tb_f1.bin): that capture's line marks are off the cadence, via NMH_MUTATE=F1-ANCHOR-PER-RUN; EP2-PER-RUN is red with it on the same capture
// MUTATION P1-PRE1.1 stimulus red CIAB/timer B's pull-up reference read with DDRB bit n=1 and the data bit 0 beforehand (p1_ciab_tb_pre11.bin), via NMH_MUTATE=P1-PRE11
// MUTATION P1-PRE1.2 stimulus red CIAB/timer B's step-4 latch changed to $FFFF (p1_ciab_tb_pre12.bin): the poll limit runs out, via NMH_MUTATE=P1-PRE12; legs A1 and B are then not graded on that instance, and P1-MAIN-CRB and P1-MAIN stay FAIL through legs a2 and c1
// MUTATION P1-PRE1.3 stimulus red CIAB/timer B's step-7 latch changed to $0004 (p1_ciab_tb_pre13.bin): an underflow happens during leg C, via NMH_MUTATE=P1-PRE13
// MUTATION P1-PRE1.4/O1 stimulus red CIAB/timer B's O1-contrast leg left at data bit n=0 instead of 1 (p1_ciab_tb_pre14.bin): the "data 1 reads 1" half of the precondition no longer holds, via NMH_MUTATE=P1-PRE14; P1-MAIN-CRB is then not graded, because both timer-B instances are excluded; P10-opus:F-crb.c3 and P10-opus:F-crb.c13 are then FAIL -> not graded (CIAB/timer B leaves every member that fails there)
// MUTATION P1-MAIN-CRA stimulus green the PBON=0 runs through the same comparator (grade_p1): P1-CONTROL below
// MUTATION P1-MAIN-CRB stimulus green the PBON=0 runs through the same comparator (grade_p1): P1-CONTROL below
// MUTATION P1-MAIN stimulus green the PBON=0 runs through the same comparator (grade_p1): P1-CONTROL below
// MUTATION P1-CONTROL-PRE1.1 stimulus red CIAB/timer B's PBON=0 run with DDRB bit n set to output and the data bit 0 before the pull-up reference is read (p1_ciab_tb_pbon0_pre11.bin): r0 bit n reads 0, via NMH_MUTATE=P1-CONTROL-PRE11; P1-CONTROL is then not graded
// MUTATION P1-CONTROL-PRE1.2 stimulus red CIAB/timer B's PBON=0 run with step 4's latch changed to $FFFF (p1_ciab_tb_pbon0_pre12.bin): the poll limit runs out and the i1 flag reads 0, via NMH_MUTATE=P1-CONTROL-PRE12; P1-CONTROL is then not graded
// MUTATION P1-CONTROL-PRE1.3 stimulus red CIAB/timer B's PBON=0 run with step 7's latch changed to $0004 (p1_ciab_tb_pbon0_pre13.bin): an underflow happens during leg C and the i2 flag reads 1, via NMH_MUTATE=P1-CONTROL-PRE13; P1-CONTROL is then not graded
// MUTATION P1-OTHERBITS stimulus red CIAB/timer B's leg-B pattern bit 0 flipped (p1_ciab_tb_otherbits.bin): b1's bits 0-5 no longer match this instance's written pattern, via NMH_MUTATE=P1-OTHERBITS
// MUTATION P1-CONTROL stimulus red CIAB/timer B's PBON=0 run writes leg B's data bit n as 0 (p1_ciab_tb_pbon0_b0.bin): b1 bit n reads 0 where the page predicts 1, via NMH_MUTATE=P1-CONTROL
// MUTATION P2-LEGR stimulus green a CPU-driven low level before any start through the same comparator: P2-LEGR-GREENCTL below
// MUTATION P2-MAIN stimulus green a CPU-driven rise at every start through the same comparator: P2-MAIN-GREENCTL below
// MUTATION P2-LEGH stimulus green a CPU-driven high at each leg-H start through the same comparator: P2-LEGH-GREENCTL below
// MUTATION P2-TOGGLE stimulus green the CPU-driven level through legs R, S and H together: P2-TOGGLE-GREENCTL below
// MUTATION P2-PRE2.2 stimulus red CIAB/timer B's leg-S latch changed to $FFFF (p2_ciab_tb_pre22.bin): the poll count of start 1 hits the limit and its TB flag reads 0, via NMH_MUTATE=P2-PRE22; P2-MAIN then grades no start that follows a one-shot which had not ended on CIAB/timer B, and P2-LEGR, P2-MAIN, P2-LEGH and P2-TOGGLE stay FAIL through CIAB/timer A
// MUTATION P2-PRE2.3 stimulus red CIAB/timer B's start 1 written as $0D with DDRB bit n released for z_1's read only (p2_ciab_tb_pre23.bin): z_1 reads the pull-up's 1 and start 2 is not graded, via NMH_MUTATE=P2-PRE23
// MUTATION P2-PRE2.4 stimulus red CIAB/timer B's leg-H latch changed to $0008 (p2_ciab_tb_pre24.bin): the ih flag reads 1, via NMH_MUTATE=P2-PRE24; P2-LEGH then does not grade CIAB/timer B and stays FAIL through CIAB/timer A
// MUTATION P2-LEGR-GREENCTL stimulus red a start, with the CPU raising the line, before leg R's write (p2c_ciab_tb_legr.bin): r1 bit n reads 1 where the page predicts 0, via NMH_MUTATE=P2-LEGR-GREENCTL; P2-TOGGLE-GREENCTL is red with it
// MUTATION P2-MAIN-GREENCTL stimulus red start 4 written as $0C (START=0) with no CPU raise (p2c_ciab_tb_k4.bin): s_4 shows no rise, via NMH_MUTATE=P2-MAIN-GREENCTL; P2-TOGGLE-GREENCTL is red with it
// MUTATION P2-LEGH-GREENCTL stimulus red no CPU raise after leg H's first start (p2c_ciab_tb_h1.bin): h1 bit n reads 0, via NMH_MUTATE=P2-LEGH-GREENCTL; P2-TOGGLE-GREENCTL is red with it
// MUTATION P2-TOGGLE-GREENCTL stimulus red the P2-LEGR-GREENCTL mutant (p2c_ciab_tb_legr.bin), via NMH_MUTATE=P2-LEGR-GREENCTL
// MUTATION P2-GREENCTL-PRE2.2 stimulus red the CPU-driven leg-S latch changed to $FFFF (p2c_ciab_tb_pre22.bin): the poll count of start 1 hits the limit, via NMH_MUTATE=P2-GREENCTL-PRE22; the P2-*-GREENCTL lines are then not graded
// MUTATION P2-GREENCTL-PRE2.3 stimulus red DDRB bit n released for z_1's read in the CPU-driven run (p2c_ciab_tb_pre23.bin): z_1 reads the pull-up's 1, via NMH_MUTATE=P2-GREENCTL-PRE23; the P2-*-GREENCTL lines are then not graded
// MUTATION P2-GREENCTL-PRE2.4 stimulus red the CPU-driven leg-H latch changed to $0008 (p2c_ciab_tb_pre24.bin): the ih flag reads 1, via NMH_MUTATE=P2-GREENCTL-PRE24; the P2-*-GREENCTL lines are then not graded
// MUTATION P3-PRE stimulus red CIAB/timer B's latch changed to 2 (p3_ciab_tb_l2.bin): flags appear in consecutive reads, via NMH_MUTATE=P3-PRE31; P3 is then not graded
// MUTATION P3-PRE stimulus red CIAB/timer B's run with CR<-$06 (p3_ciab_tb_nostart.bin): no underflow, |Q|=0, via NMH_MUTATE=P3-PRE32; P3 is then not graded
// MUTATION P3 stimulus green a CPU-driven toggle through the same predicate: P3-GREENCTL below
// MUTATION P3-GREENCTL stimulus red the level before the first flag written as 0 instead of 1 (p3_ciab_tb_cputoggle_low.bin): the level conjunct of P3's predicate fails while the pairing holds, via NMH_MUTATE=P3-GREENCTL
// MUTATION P4-PRE stimulus red three of CIAB/timer B's four usable latch runs not started (p4_ciab_tb_l21_nostart.bin, l22, l24): fewer than two usable latches, via NMH_MUTATE=P4-PRE; P4 is then not graded
// MUTATION P4 stimulus green a CPU-driven pulse at one fixed phase of the timer's period through the same comparator: P4-GREENCTL below
// MUTATION P4-GREENCTL stimulus red CIAB/timer B's L=40 control table with a second high count value (p4c_ciab_tb_l40_two.bin): two residues read high and the single-residue comparator fails, via NMH_MUTATE=P4-GREENCTL
// MUTATION P9-LEGF-PRE stimulus red CIAB/timer B's L=40 run not started (p9_legf_ciab_tb_l40_nostart.bin): no flags, so no period is determined, via NMH_MUTATE=P10-NOPERIOD-L40; P9-LEGF and P9-LEGF-TRANSITIONS are then not graded
// MUTATION P9-LEGF stimulus red CIAB/timer B's L=41 run made with a latch of 42 (p9_legf_ciab_tb_l41_m.bin): the period offset is one larger than the other three, via NMH_MUTATE=P9-LEGF
// MUTATION P9-LEGF-TRANSITIONS stimulus green a CPU-driven transition train through the same comparator: P9-LEGF-TRANSITIONS-GREENCTL below
// MUTATION P9-LEGF-TRANSITIONS-GREENCTL stimulus red the CPU inverts PRB bit 7 after every second flagged read (p9_legf_ciab_tb_cputoggle_skip.bin): consecutive transitions are about 2*P(L) apart and the spacing comparator fails, via NMH_MUTATE=P9-LEGF-TRANSITIONS-GREENCTL
// MUTATION P9-LEGW-PRE2.2 stimulus red the L=40 leg-W latch changed to $FFFF (p9_legw_l40_nouf.bin): the one-shot has not ended when the sampler closes and the TB flag reads 0, via NMH_MUTATE=P9-LEGW-PRE22 (the line covers the L=40 and the L=100 run); P9-LEGW is then not graded
// MUTATION P9-LEGW-PRE2.3 stimulus red the L=40 leg-W run leaves DDRB bit 7 an input (p9_legw_l40_lvl1.bin): the level before the start reads the pull-up's 1, via NMH_MUTATE=P9-LEGW-PRE23 (the line covers the L=40 and the L=100 run); P9-LEGW is then not graded
// MUTATION P9-LEGW stimulus green a CPU-driven width through the same comparator: P9-LEGW-GREENCTL below
// MUTATION P9-LEGW-GREENCTL-PRE2.2 stimulus red the CPU-driven L=40 latch changed to $FFFF (p9_legw_ctl_l40_nouf.bin): the one-shot has not ended when the sampler closes and no TB flag is read, via NMH_MUTATE=P9-LEGW-GREENCTL-PRE22 (the line covers the L=40 and the L=100 run); P9-LEGW-GREENCTL is then not graded
// MUTATION P9-LEGW-GREENCTL-PRE2.3 stimulus red the CPU-driven L=40 run leaves DDRB bit 7 an input for the read before the start (p9_legw_ctl_l40_lvl1.bin): the level before the start reads the pull-up's 1, via NMH_MUTATE=P9-LEGW-GREENCTL-PRE23 (the line covers the L=40 and the L=100 run); P9-LEGW-GREENCTL is then not graded
// MUTATION P9-LEGW-GREENCTL stimulus red the CPU-driven L=40 run made with a latch of 20 (p9_legw_ctl_l20.bin): W(100)-W(20) is about 80, outside 60+-s, via NMH_MUTATE=P9-LEGW-GREENCTL
// MUTATION P5-PRE51 stimulus red CIAB/timer B's D2 run made with R-LATCH($1000) (p5a_ciab_tb_d2_pre51.bin): the TB flag is set in the window, via NMH_MUTATE=P5-PRE51; P5-LEGA is then not graded
// MUTATION P5-PRE52 stimulus red CIAB/timer B's D1 run made without the stop write (p5a_ciab_tb_d1_pre52.bin): the second read of the counter differs from the first, via NMH_MUTATE=P5-PRE52; P5-LEGA is then not graded
// MUTATION P5-LEGA stimulus red CIAB/timer B's D2 run with CR<-$00 as its start write (p5a_ciab_tb_d2_nostart.bin): the counter does not move, so Delta2-Delta1 is far from round((tau2-tau1)/(10*T_CPU)), via NMH_MUTATE=P5-LEGA
// MUTATION P5-PRE53 stimulus red CIAB/timer B's leg-(b) sampler reads a Chip-memory byte instead of the LO register (p5b_ciab_tb_chip.bin): its stores take five phases of the E cycle, via NMH_MUTATE=P5-PRE53; P5-LEGB is then not graded
// MUTATION P5-PRE53 stimulus red CIAB/timer B's leg-(b) sampler runs 255 iterations (p5b_ciab_tb_short.bin): 255 of the 256 samples, via NMH_MUTATE=P5-PRE53-POP; P5-LEGB is then not graded
// MUTATION P5-LEGB stimulus red CIAB/timer B's leg-(b) run with CR<-$00 as its start write (p5b_ciab_tb_nostart.bin): v_i is constant where e_(i+1)-e_i is not a multiple of 256, via NMH_MUTATE=P5-LEGB
// MUTATION P5-CONTRAST implementation green the private copy of the RTL described at P7.5-CIAB-GREENCTL, run with NMH_MUTATE=P5-CONTRAST: CIAB/timer B under CRB<-$21 holds and Delta1=Delta2=0, the line prints PASS on both cores; with the repository's RTL the line is red on both cores in its own selected run, because the timer counts phi2
// MUTATION P6-PRE61 stimulus red K3's loop body holds three or five NOPs, whichever makes the measured I3 a multiple of T_E on the core (p6_k123_n3.bin, p6_k123_n5.bin), via NMH_MUTATE=P6-PRE61; P6-K3 is then not graded
// MUTATION P6-PRE62 stimulus red K1 runs 199 iterations (p6_k123_short.bin), via NMH_MUTATE=P6-PRE62; P6-K1K2 and P6-K3 are then not graded
// MUTATION P6-K1K2 stimulus red K1 reads a Chip-memory byte instead of CIAA PRB (p6_k123_k1chip.bin): its stores take five phases of the E cycle, via NMH_MUTATE=P6-K1K2
// MUTATION P6-K3 stimulus red K3 alternates a read of CIAA PRB with a read of the Chip-memory byte (p6_k123_k3mix.bin): its stores take fewer than the five phases and the measured I3 is still not a multiple of T_E, via NMH_MUTATE=P6-K3
// MUTATION P6-P4LOOP stimulus red P4's ICR stores are Chip-memory reads taken after one NOP in every second iteration (p4_ciab_tb_l20_yspread.bin): the ICR stores take four phases while the PRB stores take two, via NMH_MUTATE=P6-P4LOOP-ICR
// MUTATION P6-P4LOOP stimulus red P4's PRB stores are Chip-memory reads taken after one NOP in every second iteration (p4_ciab_tb_l20_xspread.bin): the PRB stores take four phases while the ICR stores take two, via NMH_MUTATE=P6-P4LOOP-PRB
// MUTATION P6-P5BLOOP stimulus red P5 leg (b)'s sampler reads a Chip-memory byte instead of the LO register (p5b_ciab_tb_chip.bin): its stores take five phases, via NMH_MUTATE=P6-P5BLOOP
// MUTATION P6-P9LOOP stimulus red P9 leg W's sampler reads a Chip-memory byte instead of PRB (p9_legw_l40_chip.bin): its stores take five phases, via NMH_MUTATE=P6-P9LOOP
// MUTATION P70-PRE stimulus red CIAB/timer A's latch changed to $0100 (p7_ciab_start_stpre.bin): TA is set at the end, via NMH_MUTATE=P70-PRE; P7.0-STOP and P7.0-START are then not graded
// MUTATION P7.0-STOP stimulus red CIAB/timer A started during the first delay (p7_ciab_start_contrast.bin): u2 differs from u1, via NMH_MUTATE=P7.0-STOP; P7.0-START stays green
// MUTATION P7.0-START stimulus red CIAB/timer A's start write CR<-$00 in place of $01 (p7_ciab_start_nostart.bin): u3 equals u2, via NMH_MUTATE=P7.0-START; P7.0-STOP stays green
// MUTATION P7.3-PRE stimulus red CIAB's sampler loop padded beyond 20 E periods (p7_ciab_run_pad.bin, run with a raised budget): consecutive ICR stores are further apart, via NMH_MUTATE=P73-PRE; P7.3-ONESHOT, P7.3-CONT and P7.3-CONTRAST are then not graded
// MUTATION P7.3-PRE stimulus red CIAB's sampler runs 399 iterations (p7_ciab_run_short.bin): 399 of the 400 ICR stores, via NMH_MUTATE=P73-PRE-POP; P7.3-ONESHOT, P7.3-CONT and P7.3-CONTRAST are then not graded
// MUTATION P7.3-ONESHOT stimulus red CIAA's leg O started with $08 (one-shot, START=0) instead of $09, poked through the parameter block: leg O shows no TA flag, via NMH_MUTATE=P7.3-ONESHOT; P7.3-CONT and P7.3-CONTRAST stay green
// MUTATION P7.3-CONT stimulus red CIAA's leg C started with $09 (one-shot) instead of $01, poked through the parameter block: leg C shows exactly one TA flag, via NMH_MUTATE=P7.3-CONT; P7.3-CONTRAST is red with it and P7.3-ONESHOT stays green
// MUTATION P7.3-CONTRAST stimulus red CIAA's leg C started with $09 (one-shot) instead of $01, poked through the parameter block: the leg shows exactly one TA flag, so the line's check that leg C does not show exactly one flag fails, via NMH_MUTATE=P7.3-CONT; P7.3-CONT is red with it
// MUTATION P7.4-PRE stimulus red CIAB's stopped leg without a start write (p7_ciab_load_nostart.bin): v1 is $4000, via NMH_MUTATE=P74-PRE; P7.4 is then not graded
// MUTATION P7.4 stimulus red CIAB's stopped leg with CR<-$00 instead of $10 (p7_ciab_load_ld0.bin): v2 equals v1, via NMH_MUTATE=P74-LD0
// MUTATION P7.4 stimulus red CIAB's running leg with CR<-$01 instead of $11 (p7_ciab_load_run01.bin): $FFFF-v3 is about 5100, above (D-A)/T_E+1, via NMH_MUTATE=P74-CONTRAST
// MUTATION P7.7-PRE stimulus red CIAB's $81 run with a different delay count (p7_ciab_unused81_delay.bin): the bracket-to-bracket times of the two runs differ by more than one E period, via NMH_MUTATE=P77-PRE; P7.7 is then not graded
// MUTATION P7.7 stimulus red CIAB's $81 run with CR<-$80 (START=0) as its second write (p7_ciab_unused81_startoff.bin): c2 reads $00 and w stays $8000, via NMH_MUTATE=P77-STARTOFF
// MUTATION P7.7 stimulus red CIAB's $81 run with CR<-$02 (a used bit) in place of CR<-$80 as its first write (p7_ciab_unused_c1contrast.bin): c1 reads $02, via NMH_MUTATE=P77-C1CONTRAST
// MUTATION P7.5-WINDOW stimulus red CIAA's key code scheduled 3500 E periods after the ready marker, after any window of at most 3000 E periods: the run ends first and no key is sent, via NMH_MUTATE=P7.5-WINDOW; P7.6-CIAA is then not graded
// MUTATION P7.5-WINDOW stimulus red CIAA's wait of 200 dbra iterations instead of 2200 (p7_ciaa_inmode_short.bin): W is shorter than 1500 E periods, via NMH_MUTATE=P7.5-WINDOW-W; P7.6-CIAA is then not graded
// MUTATION P7.5-WINDOW-CIAB stimulus red CIAB's wait of 200 dbra iterations instead of 2200 (p7_ciab_inmode_short.bin): W is shorter than 1500 E periods, via NMH_MUTATE=P7.5-WINDOW-CIAB; P7.5 is then not graded
// MUTATION P7.5 stimulus green CIAB's start write CR<-$20 (INMODE=CNT, START=0) instead of $21 (p7_ciab_inmode_nostart.bin): the timer holds, v is $1000, via NMH_MUTATE=P7.5; the control through a started timer is P7.5-CIAB-GREENCTL below
// MUTATION P7.5 implementation green the private copy of the RTL described at P7.5-CIAB-GREENCTL below: CIAB's timer A holds v=$1000 in CRA INMODE=CNT, and the MEASURED line for CIAA shows the decrement of 8
// MUTATION P7.5-CONTRAST stimulus red CIAB's start write CR<-$20 (not started) instead of $01 (p7_ciab_inmode_nostart.bin): $1000-v is 0, below (stop_before-start_after)/T_E-1, via NMH_MUTATE=P75-CONTRAST-MUT
// MUTATION P7.5-CIAA-INMODE-PRE stimulus red the key code scheduled 100 E periods after the first interval's ready marker (NMH_MUTATE=P7.5-CIAA-INMODE-PRE): the key code is sent before the first interval ends, so the zero-edge precondition fails; P7.5-CIAA-INMODE is then not graded
// MUTATION P7.5-CIAA-INMODE stimulus green CIAA's first start write CR<-$20 (INMODE=CNT, START=0) instead of $21 (p7_ciaa_inmode_ctl_nostart1.bin): the timer holds, v1 is $1000, via NMH_MUTATE=P7.5-CIAA-INMODE; in the normal run the line is red on both cores because the started timer counts phi2 (v1 is $0DFE on fx68k and $0D9D on TG68K); with the selector of P7.5 applied too, P10-sol:F-cra.c13 is FAIL -> not graded
// MUTATION P7.5-CIAA-INMODE implementation green the private copy of the RTL described at P7.5-CIAB-GREENCTL below: CIAA's timer A holds v1=$1000 over the interval with no CNT edge, and the MEASURED line shows the decrement of 8 after the second start
// MUTATION P7.5-CIAB-GREENCTL-PRE stimulus red the key code scheduled 100 E periods after the first interval's ready marker (NMH_MUTATE=P75-GREENCTL-PRE): the ICR read after the first interval has SP=1 and the key is sent before that interval ends
// MUTATION P7.5-CIAB-GREENCTL implementation green a private copy of the RTL outside the repository in which CRA bit 5 selects, for CIA-A's timer A, one CNT pulse per KCLK pulse of a key code (eight) in place of phi2, and CIA-B's CNT stays low (cia_timera.v, ciaa.v, ciab.v; the same copy carries the change of cia_timerb.v described at P8.65-MODE01-CIAB-GREENCTL), built with make NANOMIG=<copy> and run with NMH_MUTATE=P75-GREENCTL: v1 is $1000 after the interval with no edge and v2 is $0FF8 after the interval with the key code; with the repository's RTL the line is red on both cores because the timer counts phi2 (v1 is $0DFE on fx68k and $0D9D on TG68K)
// MUTATION P7.5-CIAB-GREENCTL stimulus red on that copy, the key code left out of the second interval (NMH_MUTATE=P75-GREENCTL-NOKEY): v2 stays $1000, where v1-v2 should be 8
// MUTATION P7.5-WINDOW stimulus red CIAA's R-KEY left out (NMH_MUTATE=P76CIAA): no key code is sent and i has SP=0; P7.6-CIAA is then not graded
// MUTATION P7.6-CIAA implementation red a private copy of the RTL outside the repository in which a key code does not set CIA-A's SP flag (ciaa.v: the keystrobe no longer reaches the interrupt controller's serial input), built with make NANOMIG=<copy>: i has SP=0 while every timing clause holds, P7.6-CIAA prints FAIL and P7.5-WINDOW is red with it; no stimulus of this package makes SP=0 in a run whose timing clauses hold
// MUTATION P7.6-CIAA-CONTRAST stimulus red the key sent in the run whose comparator expects SP=0: SP=1, via NMH_MUTATE=P76CIAA-CONTRAST-MUT
// MUTATION P7.6-CIAB-PRE stimulus red CIAB's start write CR<-$40 (not started) instead of $41 (p76_ciab_pre.bin): no TA flag, via NMH_MUTATE=P76CIAB-PRE; P7.6-CIAB is then not graded
// MUTATION P7.6-CIAB stimulus green the same SPMODE-output program run on CIA-A (p76_ciaa.bin): at least one ICR sample has SP=1, the line P7.6-CIAB-GREENCTL below
// MUTATION P7.6-CIAB-CONTRAST stimulus red the output-mode program run on CIA-A (p76_ciaa.bin) in place of CIAB's input-mode run: SP is seen, via NMH_MUTATE=P76CIAB-CONTRAST-MUT
// MUTATION P7.6-CIAB-GREENCTL stimulus red CIA-A with CR<-$01 (SPMODE input) instead of $41 and no key (p76_ciaa_in.bin): no ICR sample has SP=1, via NMH_MUTATE=P76CIAB-GREENCTL-RED
// MUTATION P80-PRE stimulus red CIAB/timer B's latch changed to $0100 (p8_ciab_start_stpre.bin): TB is set at the end, via NMH_MUTATE=P80-PRE; P8.0-STOP and P8.0-START are then not graded
// MUTATION P8.0-STOP stimulus red CIAB/timer B started during the first delay (p8_ciab_start_contrast.bin): u2 differs from u1, via NMH_MUTATE=P8.0-STOP; P8.0-START stays green
// MUTATION P8.0-START stimulus red CIAB/timer B's start write CR<-$00 in place of $01 (p8_ciab_start_nostart.bin): u3 equals u2, via NMH_MUTATE=P8.0-START; P8.0-STOP stays green
// MUTATION P8.3-PRE stimulus red CIAB's sampler loop padded beyond 20 E periods (p8_ciab_run_pad.bin, run with a raised budget): consecutive ICR stores are further apart, via NMH_MUTATE=P83-PRE; P8.3-ONESHOT, P8.3-CONT and P8.3-CONTRAST are then not graded
// MUTATION P8.3-PRE stimulus red CIAB's sampler runs 399 iterations (p8_ciab_run_short.bin): 399 of the 400 ICR stores, via NMH_MUTATE=P83-PRE-POP; P8.3-ONESHOT, P8.3-CONT and P8.3-CONTRAST are then not graded
// MUTATION P8.3-ONESHOT stimulus red CIAA's leg O started with $08 (one-shot, START=0) instead of $09, poked through the parameter block: leg O shows no TB flag, via NMH_MUTATE=P8.3-ONESHOT; P8.3-CONT and P8.3-CONTRAST stay green
// MUTATION P8.3-CONT stimulus red CIAA's leg C started with $09 (one-shot) instead of $01, poked through the parameter block: leg C shows exactly one TB flag, via NMH_MUTATE=P8.3-CONT; P8.3-CONTRAST is red with it and P8.3-ONESHOT stays green
// MUTATION P8.3-CONTRAST stimulus red CIAA's leg C started with $09 (one-shot) instead of $01, poked through the parameter block: the leg shows exactly one TB flag, so the line's check that leg C does not show exactly one flag fails, via NMH_MUTATE=P8.3-CONT; P8.3-CONT is red with it
// MUTATION P8.4-PRE stimulus red CIAB's stopped leg without a start write (p8_ciab_load_nostart.bin): v1 is $4000, via NMH_MUTATE=P84-PRE; P8.4 is then not graded
// MUTATION P8.4 stimulus red CIAB's stopped leg with CR<-$00 instead of $10 (p8_ciab_load_ld0.bin): v2 equals v1, via NMH_MUTATE=P84-LD0
// MUTATION P8.4 stimulus red CIAB's running leg with CR<-$01 instead of $11 (p8_ciab_load_run01.bin): $FFFF-v3 is about 5100, above (D-A)/T_E+1, via NMH_MUTATE=P84-CONTRAST
// MUTATION P8.65-MODE01-WINDOW stimulus red CIAA's key code scheduled 3500 E periods after the ready marker, after any window of at most 3000 E periods: the run ends first and no key is sent, via NMH_MUTATE=P8.65-MODE01-WINDOW
// MUTATION P8.65-MODE01-WINDOW stimulus red CIAA's wait of 200 dbra iterations instead of 2200 (p8_ciaa_inmode01_short.bin): W is shorter than 1500 E periods, via NMH_MUTATE=P8.65-MODE01-WINDOW-W
// MUTATION P8.65-MODE01-WINDOW-CIAB stimulus red CIAB's wait of 200 dbra iterations instead of 2200 (p8_ciab_inmode01_short.bin): W is shorter than 1500 E periods, via NMH_MUTATE=P8.65-MODE01-WINDOW-CIAB; P8.65-MODE01 is then not graded
// MUTATION P8.65-MODE01 stimulus green CIAB's start write CRB<-$20 (INMODE=01, START=0) instead of $21 (p8_ciab_inmode01_nostart.bin): the timer holds, v is $1000, via NMH_MUTATE=P8.65-MODE01; the control through a started timer is P8.65-MODE01-CIAB-GREENCTL below; P10-sol:F-crb.c7 is then FAIL -> not graded
// MUTATION P8.65-MODE01 implementation green the private copy of the RTL described at P8.65-MODE01-CIAB-GREENCTL below: CIAB's timer B holds v=$1000 in CRB INMODE=01, and the MEASURED line for CIAA shows the decrement of 8
// MUTATION P8.65-MODE01-CONTRAST stimulus red CIAB's start write CRB<-$20 (not started) instead of $01 (p8_ciab_inmode01_nostart.bin): $1000-v is 0, below (stop_before-start_after)/T_E-1, via NMH_MUTATE=P8.65-MODE01-CONTRAST
// MUTATION P8.65-MODE01-CIAB-GREENCTL-PRE stimulus red the key code scheduled 100 E periods after the first interval's ready marker (NMH_MUTATE=P865M01-GREENCTL-PRE): the ICR read after the first interval has SP=1 and the key is sent before that interval ends
// MUTATION P8.65-MODE01-CIAB-GREENCTL implementation green a private copy of the RTL outside the repository in which CRB bits 6-5 = 01 select, for CIA-A's timer B, one CNT pulse per KCLK pulse of a key code (eight) in place of phi2, and CIA-B's CNT stays low (cia_timerb.v, ciaa.v, ciab.v), built with make NANOMIG=<copy> and run with NMH_MUTATE=P865M01-GREENCTL: v1 is $1000 after the interval with no edge and v2 is $0FF8 after the interval with the key code; with the repository's RTL the line is red on both cores because the timer counts phi2
// MUTATION P8.65-MODE01-CIAB-GREENCTL stimulus red on that copy, the key code left out of the second interval (NMH_MUTATE=P865M01-GREENCTL-NOKEY): v2 stays $1000, where v1-v2 should be 8
// MUTATION P8.65-MODE10-PRE stimulus red CIAB's R-LATCH_A(2) instead of 30 (p865_ciab_mode10_shortlatch.bin): flags appear in consecutive reads and no period is determined, via NMH_MUTATE=M10-PRE; P8.65-MODE10 is then not graded
// MUTATION P8.65-MODE10 stimulus red CIAB's CRB<-$01 (mode 00, phi2) in place of $41 (p865_ciab_mode10_contrast.bin): $FFFF-v counts the E cycles of the window, not N_A, via NMH_MUTATE=M10; P8.65-MODE10-PRE stays green
// MUTATION P8.65-MODE10-CONTRAST stimulus red the mode-10 run (p865_ciab_mode10.bin) in place of CRB<-$01 (mode 00): $FFFF-v is N_A, far below (stop_before-start_after)/T_E-1, via NMH_MUTATE=M10-CONTRAST
// MUTATION P8.65-MODE11-WINDOW stimulus red the key code of pair K scheduled 3500 E periods after the ready marker, after any window of at most 3000 E periods: the run ends first and no key is sent, via NMH_MUTATE=M11-WINDOW; P8.65-MODE11-PAIRN is then not graded
// MUTATION P8.65-MODE11-WINDOW stimulus red both runs of pair N with a wait of 200 dbra iterations instead of 2200 (p865_ciaa_mode11_short.bin): W is shorter than 1500 E periods and the two windows stay equal, via NMH_MUTATE=M11-WINDOW-W; P8.65-MODE11-WINPRE stays green and P8.65-MODE11-PAIRN is not graded
// MUTATION P8.65-MODE11-WINPRE stimulus red pair N's run Y with a wait of 2300 dbra iterations instead of 2200 (p865_ciaa_mode11_w2300.bin): the two windows differ, via NMH_MUTATE=M11-WINPRE; P8.65-MODE11-PAIRN is then not graded
// MUTATION P8.65-MODE11-PAIRN stimulus red pair N's run Y with CRB<-$01 (mode 00, phi2) in place of $61, poked through the parameter block: Y counts the E cycles of the window, so D_Y differs from D_X, via NMH_MUTATE=M11-PAIRN; P8.65-MODE11-WINDOW and P8.65-MODE11-WINPRE stay green
// MUTATION P8.7-PRE stimulus red run X's setup without the LSB write of TOD<-0 (p87_ciab_x_nolsb.bin): TOD stays stopped and T0 is 0, via NMH_MUTATE=P87-PRE; P8.7-RUNX and P8.7-RUNY are then not graded
// MUTATION P8.7-POP stimulus red run X's sampler of 449 iterations (p87_ciab_x_short.bin): 449 of the 450 TODLO and ICR stores, via NMH_MUTATE=P87-POP; P8.7-RUNX and P8.7-RUNY are then not graded
// MUTATION P8.7-RUNX stimulus red CIAB's run X with CRB<-$00 in place of $80 for its TOD writes, poked through the parameter block: the TOD is set to A and no alarm is set, via NMH_MUTATE=P8.7-RUNX; P8.7-RUNY stays green
// MUTATION P8.7-RUNY stimulus red CIAB's run Y with CRB<-$80 in place of $00 for its TOD writes, poked through the parameter block: the alarm is set to A and TOD keeps counting from its setup, via NMH_MUTATE=P8.7-RUNY; P8.7-RUNX stays green
// MUTATION P8.7-CIAA-RUNX stimulus red CIAA's run X with CRB<-$00 in place of $80 for its TOD writes, poked through the parameter block: the TOD is set to A and T1-T0 is far above 2, via NMH_MUTATE=P8.7-CIAA-RUNX; P8.7-CIAA-RUNY stays green
// MUTATION P8.7-CIAA-RUNY stimulus red CIAA's run Y with CRB<-$80 in place of $00 for its TOD writes, poked through the parameter block: the alarm is set to A and T1-A is far below 0, via NMH_MUTATE=P8.7-CIAA-RUNY; P8.7-CIAA-RUNX stays green
// MUTATION P10-sol:F-timers.c13 none impossible no stimulus of this package makes the row print PASS: CIA-A's PRB read-back is not live (Pre1.4/O1), so P2, P3 and P4 are never graded on CIAA/timer A and CIAA/timer B; the join leaves FAIL when the preconditions of the members that fail are falsified, via NMH_MUTATE=P2-PRE22,P3-PRE31,P4-PRE,P10-NOPERIOD-L40,P9-LEGW-PRE22,P1-PRE12,P7.5-WINDOW-CIAB,P7.5-CIAA-INMODE-PRE,P76CIAB-PRE,P8.65-MODE01-WINDOW-CIAB (CIAB/timer B's P2 leg-S latch $FFFF, its P3 latch 2, three of its four P4 latch runs not started, leg F's L=40 run not started, leg W's L=40 latch $FFFF, CIAB/timer B's P1 step-4 latch $FFFF, CIAB's P7.5 wait of 200 dbra iterations, the key code of the P7.5-CIAA-INMODE run sent in its first interval, CIAB's P7.6 timer not started, CIAB's P8.65 mode 01 wait of 200 dbra iterations): P2, P3 and P4 are not graded on CIAB/timer A and CIAB/timer B and the row prints RECORDED not graded instead of FAIL; that run also fails P1-PRE1.2, P2-PRE2.2, P3-PRE, P4-PRE, P9-LEGF-PRE, P9-LEGW-PRE2.2, P7.5-WINDOW-CIAB, P7.5-CIAA-INMODE-PRE, P8.65-MODE01-WINDOW-CIAB and P7.6-CIAB-PRE, and turns P3, P4, P9-LEGF, P9-LEGF-TRANSITIONS, P9-LEGW, P7.5, P7.5-CIAA-INMODE, P8.65-MODE01 and P7.6-CIAB into not graded
// MUTATION P10-opus:F-timers.c12 none impossible no stimulus of this package makes the row print PASS: CIA-A's PRB read-back is not live (Pre1.4/O1), so P3 and P4 are never graded on CIAA/timer A and CIAA/timer B; the join leaves FAIL when the preconditions of the members that fail are falsified, via NMH_MUTATE=P2-PRE22,P3-PRE31,P4-PRE,P10-NOPERIOD-L40,P9-LEGW-PRE22,P1-PRE12,P7.5-WINDOW-CIAB,P7.5-CIAA-INMODE-PRE,P76CIAB-PRE,P8.65-MODE01-WINDOW-CIAB (the list and the collateral lines of P10-sol:F-timers.c13): P3 and P4 are not graded on CIAB/timer A and CIAB/timer B and the row prints RECORDED not graded instead of FAIL
// MUTATION P10-sol:F-cra.c16 none impossible no stimulus of this package makes the row print PASS: CIA-A's PRB read-back is not live (Pre1.4/O1), so P3 and P4 are never graded on CIAA/timer A; the join leaves FAIL when the preconditions of the members that fail are falsified, via NMH_MUTATE=P2-PRE22,P3-PRE31,P4-PRE,P10-NOPERIOD-L40,P9-LEGW-PRE22,P1-PRE12,P7.5-WINDOW-CIAB,P7.5-CIAA-INMODE-PRE,P76CIAB-PRE,P8.65-MODE01-WINDOW-CIAB (the list and the collateral lines of P10-sol:F-timers.c13): P3 and P4 are not graded on CIAB/timer A and the row prints RECORDED not graded instead of FAIL
// MUTATION P10-opus:F-crb.c3 none impossible no stimulus of this package makes the row print PASS: CIA-A's PRB read-back is not live (Pre1.4/O1), so P3 and P4 are never graded on CIAA/timer B; the join leaves FAIL when the preconditions of the members that fail are falsified, via NMH_MUTATE=P2-PRE22,P3-PRE31,P4-PRE,P10-NOPERIOD-L40,P9-LEGW-PRE22,P1-PRE12,P7.5-WINDOW-CIAB,P7.5-CIAA-INMODE-PRE,P76CIAB-PRE,P8.65-MODE01-WINDOW-CIAB (the list and the collateral lines of P10-sol:F-timers.c13): P3 and P4 are not graded on CIAB/timer B and the row prints RECORDED not graded instead of FAIL
// MUTATION P10-sol:F-cra.c13 none impossible no stimulus of this package makes the row print PASS: P7.5 and P7.5-CIAA-INMODE grade only the zero-edge half of INMODE=CNT, so their members are FAIL or not graded; the join leaves FAIL when the preconditions of the members that fail are falsified, via NMH_MUTATE=P2-PRE22,P3-PRE31,P4-PRE,P10-NOPERIOD-L40,P9-LEGW-PRE22,P1-PRE12,P7.5-WINDOW-CIAB,P7.5-CIAA-INMODE-PRE,P76CIAB-PRE,P8.65-MODE01-WINDOW-CIAB (the list and the collateral lines of P10-sol:F-timers.c13): P7.5 and P7.5-CIAA-INMODE are not graded, P5 on the timer-A instances passes and the row prints RECORDED not graded instead of FAIL
// MUTATION P10-sol:F-crb.c7 none impossible no stimulus of this package makes the row print PASS: mode 11's member is never graded and mode 01's grades only the zero-edge half of INMODE=01; the join leaves FAIL when the preconditions of the members that fail are falsified, via NMH_MUTATE=P2-PRE22,P3-PRE31,P4-PRE,P10-NOPERIOD-L40,P9-LEGW-PRE22,P1-PRE12,P7.5-WINDOW-CIAB,P7.5-CIAA-INMODE-PRE,P76CIAB-PRE,P8.65-MODE01-WINDOW-CIAB (the list and the collateral lines of P10-sol:F-timers.c13): P8.65 mode 01 is not graded, P5 on the timer-B instances and mode 10 pass and the row prints RECORDED not graded instead of FAIL
// MUTATION P10-opus:F-cra.c12 none impossible no stimulus of this package makes the row print PASS: CIA-A's PRB read-back is not live (Pre1.4/O1), so P1-CONTROL and P4 are never graded on CIAA/timer A; the join leaves FAIL when the preconditions of the members that fail are falsified, via NMH_MUTATE=P2-PRE22,P3-PRE31,P4-PRE,P10-NOPERIOD-L40,P9-LEGW-PRE22,P1-PRE12,P7.5-WINDOW-CIAB,P7.5-CIAA-INMODE-PRE,P76CIAB-PRE,P8.65-MODE01-WINDOW-CIAB (the list and the collateral lines of P10-sol:F-timers.c13): P4 is not graded on CIAB/timer A, the other members of the 0-setting row pass or are not graded, and the row prints RECORDED not graded instead of FAIL
// MUTATION P10-opus:F-cra.c13 none impossible no stimulus of this package makes the row print PASS: CIA-A's PRB read-back is not live (Pre1.4/O1), so P1 PBON=1 and P3 are never graded on CIAA/timer A, and P7.5 and P7.5-CIAA-INMODE grade only the zero-edge half of INMODE=CNT; the join leaves FAIL when the preconditions of the members that fail are falsified, via NMH_MUTATE=P2-PRE22,P3-PRE31,P4-PRE,P10-NOPERIOD-L40,P9-LEGW-PRE22,P1-PRE12,P7.5-WINDOW-CIAB,P7.5-CIAA-INMODE-PRE,P76CIAB-PRE,P8.65-MODE01-WINDOW-CIAB (the list and the collateral lines of P10-sol:F-timers.c13): P1 PBON=1, P3 on CIAB/timer A, P7.5, P7.5-CIAA-INMODE and P7.6-CIAB are not graded and the row prints RECORDED not graded instead of FAIL
// MUTATION P10-opus:F-crb.c13 none impossible no stimulus of this package makes the row print PASS: CIA-A's PRB read-back is not live (Pre1.4/O1), so P1-CONTROL and P4 are never graded on CIAA/timer B; the join leaves FAIL when the preconditions of the members that fail are falsified, via NMH_MUTATE=P2-PRE22,P3-PRE31,P4-PRE,P10-NOPERIOD-L40,P9-LEGW-PRE22,P1-PRE12,P7.5-WINDOW-CIAB,P7.5-CIAA-INMODE-PRE,P76CIAB-PRE,P8.65-MODE01-WINDOW-CIAB (the list and the collateral lines of P10-sol:F-timers.c13): P4 is not graded on CIAB/timer B, the other members of the 0-setting row pass or are not graded, and the row prints RECORDED not graded instead of FAIL
// MUTATION P10-opus:F-crb.c14 none impossible no stimulus of this package makes the row print PASS: CIA-A's PRB read-back is not live (Pre1.4/O1), so P1 PBON=1 and P3 are never graded on CIAA/timer B, and P8.65 mode 01 grades only the zero-edge half of INMODE=01; the join leaves FAIL when the preconditions of the members that fail are falsified, via NMH_MUTATE=P2-PRE22,P3-PRE31,P4-PRE,P10-NOPERIOD-L40,P9-LEGW-PRE22,P1-PRE12,P7.5-WINDOW-CIAB,P7.5-CIAA-INMODE-PRE,P76CIAB-PRE,P8.65-MODE01-WINDOW-CIAB (the list and the collateral lines of P10-sol:F-timers.c13): P1 PBON=1, P3 on CIAB/timer B and P8.65 mode 01 are not graded and the row prints RECORDED not graded instead of FAIL
// MUTATION P10-opus:F-timers.c6 none impossible no stimulus of this package makes the row print PASS: CIA-A's PRB read-back is not live (Pre1.4/O1), so P4 is never graded on CIAA/timer A and CIAA/timer B; the join leaves FAIL when the preconditions of the members that fail are falsified, via NMH_MUTATE=P2-PRE22,P3-PRE31,P4-PRE,P10-NOPERIOD-L40,P9-LEGW-PRE22,P1-PRE12,P7.5-WINDOW-CIAB,P7.5-CIAA-INMODE-PRE,P76CIAB-PRE,P8.65-MODE01-WINDOW-CIAB (the list and the collateral lines of P10-sol:F-timers.c13): P4 on CIAB/timer A and CIAB/timer B, P9 leg F, P9-LEGF-TRANSITIONS and P9 leg W are not graded and the row prints RECORDED not graded instead of FAIL
//
// P1-LP, P1-O2, LED-R and P2-PRE2.1 are recorded, not asserted, and have no record.

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

// nanomig.i / ciaregs.i, restated: the harness side of the contract.
constexpr uint32_t kResults   = 0x0C0000;
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kRunBudget  = 4'000'000;

constexpr uint32_t kEp1CraA = kResults + 0;
constexpr uint32_t kEp1CrbA = kResults + 2;
constexpr uint32_t kEp1CraB = kResults + 4;
constexpr uint32_t kEp1CrbB = kResults + 6;

constexpr uint32_t kFixtureAddr  = 0x0C1FFC;
constexpr uint16_t kFixtureValue = 0xBEEF;

// P1 results (p1_body.s).
constexpr uint32_t kP1R0    = kResults + 8;
constexpr uint32_t kP1A2    = kResults + 10;
constexpr uint32_t kP1Poll1 = kResults + 12;
constexpr uint32_t kP1I1    = kResults + 14;
constexpr uint32_t kP1A1    = kResults + 16;
constexpr uint32_t kP1B1    = kResults + 18;
constexpr uint32_t kP1C1    = kResults + 20;
constexpr uint32_t kP1I2    = kResults + 22;
constexpr uint32_t kP1D1    = kResults + 24;
constexpr uint32_t kP1D2    = kResults + 26;
constexpr uint32_t kP1O1C   = kResults + 28;

// P1 peek layout: 0=R0 1=A2 2=Poll1 3=I1 4=A1 5=B1 6=C1 7=I2 8=D1 9=D2
// 10=O1C 11..14=EP1 CRA_A/CRB_A/CRA_B/CRB_B.
constexpr size_t kP1IdxD1  = 8;
constexpr size_t kP1IdxD2  = 9;
constexpr size_t kP1IdxO1C = 10;
constexpr size_t kP1IdxEp1 = 11;

// P1 O2 markers (p1_body.s).
constexpr uint32_t kMkStart      = 0x0C1000;
constexpr uint32_t kMkLpOnSel    = 0x0C1002;
constexpr uint32_t kMkLpOnEnd    = 0x0C1004;
constexpr uint32_t kMkLpOffSel   = 0x0C1006;
constexpr uint32_t kMkLpOffEnd   = 0x0C1008;
constexpr uint32_t kMkA1Sel      = 0x0C100A;
constexpr uint32_t kMkA1End      = 0x0C100C;
constexpr uint32_t kMkCSel       = 0x0C100E;
constexpr uint32_t kMkCEnd       = 0x0C1010;
constexpr uint32_t kMkD1Sel      = 0x0C1012;
constexpr uint32_t kMkD1End      = 0x0C1014;
constexpr uint32_t kMkD2Sel      = 0x0C1016;

// P2 results (p2_body.s).
constexpr uint32_t kP2R1  = kResults + 8;
constexpr uint32_t kP2S   = kResults + 10;          // 4*64 words
constexpr uint32_t kP2Z   = kP2S + 4 * 64 * 2;       // 4 words
constexpr uint32_t kP2I   = kP2Z + 4 * 2;            // 4 words
constexpr uint32_t kP2H1  = kP2I + 4 * 2;
constexpr uint32_t kP2H2  = kP2H1 + 2;
constexpr uint32_t kP2H3  = kP2H2 + 2;
constexpr uint32_t kP2Ih  = kP2H3 + 2;
constexpr uint32_t kP2Poll = kP2Ih + 2;              // 4 words
constexpr uint32_t kP2S0  = kP2Poll + 4 * 2;         // 1 word

// P2 peek layout: 0=R1 1-4=Z[0..3] 5-8=I[0..3] 9=H1 10=H2 11=H3 12=IH
// 13..268=S bits (4*64) 269..272=Poll[0..3] 273=S0 274..277=EP1.
constexpr size_t kP2IdxSBase   = 13;
constexpr size_t kP2IdxPoll    = 269;
constexpr size_t kP2IdxS0      = 273;
constexpr size_t kP2IdxEp1     = 274;

// ICR bits (ciaregs.i).
constexpr uint16_t kIcrTa = 0x01, kIcrTb = 0x02, kIcrAlrm = 0x04, kIcrSp = 0x08;

uint32_t word_addr(uint32_t byte_addr) { return byte_addr / 2; }

std::string hex2(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "$%02X", unsigned(v & 0xFF));
    return buf;
}

// A full 16-bit value, for timer/counter reads (SPECIFICATION.md section 5:
// "print 16-bit values in full") -- hex2 above is for the byte-wide CIA
// registers (CRA, ICR, PRB, DDRB) it was written for.
std::string hex4(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "$%04X", unsigned(v));
    return buf;
}

int64_t floordiv(int64_t a, int64_t b) {
    int64_t q = a / b;
    if ((a % b != 0) && ((a < 0) != (b < 0))) --q;
    return q;
}

struct Checks {
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

// A verdict over a population: NOTGRADED when the population the line quantifies
// over is empty (a universal line over nothing is neither PASS nor FAIL).
enum class V { PASS, FAIL, NOTGRADED };
V vof(bool graded, bool ok) { return !graded ? V::NOTGRADED : (ok ? V::PASS : V::FAIL); }
void emit(Checks& checks, bool graded, bool ok, const std::string& id, const std::string& label,
          const std::string& why) {
    if (graded) checks.expect(ok, label);
    else checks.record(id + ": not graded -- " + why);
}
// Several verdicts joined into one: FAIL when any member failed, NOTGRADED when none failed and
// some member was not graded, PASS when every member passed.
V join(std::initializer_list<V> vs) {
    bool ng = false;
    for (V v : vs) {
        if (v == V::FAIL) return V::FAIL;
        if (v == V::NOTGRADED) ng = true;
    }
    return ng ? V::NOTGRADED : V::PASS;
}

// ---- the common preconditions of every run (SPECIFICATION.md 1.2 EP1-EP3, 1.4 F1) --
// Every capture the package consumes is recorded here as it is made; the per-run
// assertions at the end of main grade the whole population. The dedicated EP and F1
// contrast runs switch the recording off (NoRegister), so that their mutations
// change only their own assertions.
struct RunRecord {
    std::string bin;
    bool loaded = false, done = false, ep1 = false;
    bool f1 = false;              // F1's anchor holds
    bool ep2_graded = false;      // EP2 is graded on a run that completed
    bool ep2 = false;
};
std::vector<RunRecord>& run_registry() { static std::vector<RunRecord> v; return v; }
bool& registering() { static bool b = true; return b; }
struct NoRegister {
    bool saved;
    NoRegister() : saved(registering()) { registering() = false; }
    ~NoRegister() { registering() = saved; }
};
using Ep1Words = std::array<std::optional<uint16_t>, 4>;
void register_run(const std::string& bin, bool loaded, bool done, const Ep1Words& ep1,
                  const std::vector<nmh::RamAccess>& log);
template <class H> Ep1Words ep1_of(H& h) {
    return {h.peek_logical(kEp1CraA), h.peek_logical(kEp1CrbA), h.peek_logical(kEp1CraB), h.peek_logical(kEp1CrbB)};
}

// ---- generic capture (peek_logical + full ram_log copy) ------------------

struct Capture {
    bool loaded = false;
    bool released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;
    std::vector<std::optional<uint16_t>> peeks; // aligned with peek_addrs
};

Capture run_capture(const std::string& bin, uint64_t run_budget,
                     const std::vector<uint32_t>& peek_addrs, nmh::Config cfg = nmh::Config{}) {
    Capture c;
    nmh::NanomigHarness h(cfg);
    c.loaded = h.load_program(bin) > 0;
    if (!c.loaded) { register_run(bin, false, false, Ep1Words{}, c.log); return c; }
    c.released = h.boot(kBootBudget).cpu_released;
    c.done = h.run_until_write(kDoneAddr, kDoneValue, run_budget);
    c.log = h.ram_log();
    for (auto a : peek_addrs) c.peeks.push_back(h.peek_logical(a));
    register_run(bin, true, c.done.has_value(), ep1_of(h), c.log);
    return c;
}

// ---- LED capture: fdd_led is a top-level port, not a CIA register, so it
// is only reachable by stepping the harness one clk_sys cycle at a time
// through run_cycles(1) and sampling ports() after each step
// (nanomig_harness.h: "reaches the top-level ports ... never use ->rootp").
struct LedCapture {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;
    std::vector<std::optional<uint16_t>> peeks;
    std::vector<uint8_t> led;   // led[i] = fdd_led sampled at cycle (origin+i)
    uint64_t origin = 0;
    bool led_before_boot = false;

    std::optional<uint8_t> led_at(uint64_t cycle) const {
        if (cycle < origin) return std::nullopt;
        uint64_t i = cycle - origin;
        if (i >= led.size()) return std::nullopt;
        return led[i];
    }
};

LedCapture run_capture_led(const std::string& bin, uint64_t budget,
                            const std::vector<uint32_t>& peek_addrs, nmh::Config cfg = nmh::Config{}) {
    LedCapture c;
    nmh::NanomigHarness h(cfg);
    c.loaded = h.load_program(bin) > 0;
    if (!c.loaded) { register_run(bin, false, false, Ep1Words{}, c.log); return c; }
    c.released = h.boot(kBootBudget).cpu_released;
    c.led_before_boot = h.ports()->fdd_led;
    c.origin = h.cycles();
    c.led.reserve(budget);
    size_t seen = h.ram_log().size();
    bool found_done = false;
    for (uint64_t i = 0; i < budget && !found_done; ++i) {
        h.run_cycles(1);
        c.led.push_back(h.ports()->fdd_led);
        const auto& lg = h.ram_log();
        for (; seen < lg.size(); ++seen)
            if (lg[seen].is_write && lg[seen].word_addr == word_addr(kDoneAddr) && lg[seen].data == kDoneValue) {
                c.done = lg[seen].cycle;
                found_done = true;
            }
    }
    c.log = h.ram_log();
    for (auto a : peek_addrs) c.peeks.push_back(h.peek_logical(a));
    register_run(bin, true, c.done.has_value(), ep1_of(h), c.log);
    return c;
}

Capture view_of(const LedCapture& lc) {
    Capture c;
    c.loaded = lc.loaded; c.released = lc.released; c.done = lc.done;
    c.peeks = lc.peeks; c.log = lc.log;
    return c;
}

// ---- small helpers ---------------------------------------------------

uint16_t pk(const Capture& c, size_t i) { return (i < c.peeks.size() && c.peeks[i]) ? *c.peeks[i] : 0xFFFF; }
bool pk_ok(const Capture& c, size_t i) { return i < c.peeks.size() && c.peeks[i].has_value(); }

// Pokes a parameter byte into the identity-mapped chip RAM parameter block
// (SPECIFICATION.md 1.2 E7/E9) before boot, in the low byte of the given
// word address (so the program reads it with `move.b addr+1,...`). Used
// only where the specification names "(E7)" explicitly (P7.3/P8.3's start
// value, P8.65 mode 11's CRB value): the same binary runs unmodified for
// both compared legs, and only the poked parameter differs.
Capture run_capture_param(const std::string& bin, uint32_t param_addr, uint8_t param_value,
                           uint64_t run_budget, const std::vector<uint32_t>& peek_addrs,
                           nmh::Config cfg = nmh::Config{}) {
    Capture c;
    nmh::NanomigHarness h(cfg);
    c.loaded = h.load_program(bin) > 0;
    if (!c.loaded) { register_run(bin, false, false, Ep1Words{}, c.log); return c; }
    h.poke_word(param_addr / 2, param_value);
    c.released = h.boot(kBootBudget).cpu_released;
    c.done = h.run_until_write(kDoneAddr, kDoneValue, run_budget);
    c.log = h.ram_log();
    for (auto a : peek_addrs) c.peeks.push_back(h.peek_logical(a));
    register_run(bin, true, c.done.has_value(), ep1_of(h), c.log);
    return c;
}

// ---- R-SAMPLE array extraction (SPECIFICATION.md 1.5 R-SAMPLE; F2/F3): a
// single pass over the log recovers each array slot's own store time and
// value from its own distinct word address (SPECIFICATION.md 1.2 E9), so
// per-iteration E-indices (F2) and period detection (F3) can be computed
// without a second harness run.
struct SampleSeries {
    std::vector<std::optional<uint64_t>> cycle;
    std::vector<std::optional<uint16_t>> value;
    size_t size() const { return cycle.size(); }
};

SampleSeries read_series(const std::vector<nmh::RamAccess>& log, uint32_t base_byte_addr, int count) {
    SampleSeries s;
    s.cycle.assign(count, std::nullopt);
    s.value.assign(count, std::nullopt);
    uint32_t base_w = word_addr(base_byte_addr);
    for (const auto& a : log) {
        if (!a.is_write) continue;
        if (a.word_addr < base_w) continue;
        uint32_t idx = a.word_addr - base_w;
        if (idx >= (uint32_t)count) continue;
        s.cycle[idx] = a.cycle;
        s.value[idx] = a.data;
    }
    return s;
}

// ---- F2: E-index and phase, against a T_CPU measured from the same run's
// own F1 anchor (SPECIFICATION.md 1.4 F1/F2).
double e_index(uint64_t t, uint64_t t0, uint64_t t_cpu) {
    double t_e = 10.0 * double(t_cpu);
    return std::round((double(t) - double(t0)) / t_e);
}

// (t - t0) mod T_E, folded into [-T_E/2, T_E/2) -- P6's own phi_i.
double fold_phase(uint64_t t, uint64_t t0, uint64_t t_cpu) {
    double t_e = 10.0 * double(t_cpu);
    double d = std::fmod(double(t) - double(t0), t_e);
    if (d < 0) d += t_e;
    if (d >= t_e / 2) d -= t_e;
    return d;
}

// ---- F3: underflow period P(L), fit exactly as SPECIFICATION.md 1.4 F3
// states it, not as the most frequent gap between flagged E-indices (that
// estimator is a different, weaker statistic: on a loop whose own iteration
// spacing is not a divisor of the true period, its gaps are multiples of the
// loop spacing, and "most frequent" picks one of those, not P).
//
// f_j are the E-indices of every ICR store in the series (flagged or not)
// and q_j their TA/TB flag. P(L) is the integer P for which one phase phi
// exists such that, for every j >= 1, q_j = 1 exactly when phi + kP lies in
// (f_(j-1), f_j] for some integer k -- i.e. the half-open interval crosses
// exactly one multiple of P from phi when q_j is set, and none when it is
// clear. "Exactly one P fits" (at least 16 flags, iteration spacing at most
// P/2) is checked by search: P(L) is unique when exactly one P in [2,400]
// admits such a phase.
struct PeriodResult {
    bool ok = false;               // exactly one P fits, over >=16 flags
    int64_t P = 0;
    size_t flag_count = 0;
};

PeriodResult measure_period(const SampleSeries& series, uint16_t mask, uint64_t t_cpu) {
    PeriodResult r;
    std::vector<int64_t> f;
    std::vector<bool> q;
    uint64_t t0 = 0; bool have = false;
    for (size_t i = 0; i < series.size(); ++i) {
        if (!series.cycle[i] || !series.value[i]) continue;
        if (!have) { t0 = *series.cycle[i]; have = true; }
        f.push_back((int64_t)e_index(*series.cycle[i], t0, t_cpu));
        bool fl = (*series.value[i] & mask) != 0;
        q.push_back(fl);
        if (fl) ++r.flag_count;
    }
    if (r.flag_count < 16 || f.size() < 2) return r;
    std::vector<int64_t> fits;
    for (int64_t P = 2; P <= 400; ++P) {
        bool anyphi = false;
        for (int64_t phi = 0; phi < P && !anyphi; ++phi) {
            bool ok = true;
            for (size_t j = 1; j < f.size() && ok; ++j) {
                int64_t cnt = floordiv(f[j] - phi, P) - floordiv(f[j - 1] - phi, P);
                if ((cnt > 0) != q[j]) ok = false;
            }
            if (ok) anyphi = true;
        }
        if (anyphi) fits.push_back(P);
        if (fits.size() > 1) break;   // already not unique
    }
    r.ok = (fits.size() == 1);
    if (!fits.empty()) r.P = fits.front();
    return r;
}

// Maximum gap between consecutive (present) E-indices of a whole series --
// used for "iteration spacing at most X E periods" preconditions.
int64_t max_gap_e(const std::vector<int64_t>& e_sorted) {
    int64_t m = 0;
    for (size_t i = 1; i < e_sorted.size(); ++i) m = std::max(m, e_sorted[i] - e_sorted[i - 1]);
    return m;
}

// The cycle of the (single) write to a marker address, or nullopt if the
// run never wrote it.
std::optional<uint64_t> mark(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr) {
    uint32_t w = word_addr(byte_addr);
    for (const auto& a : log) if (a.is_write && a.word_addr == w) return a.cycle;
    return std::nullopt;
}

// ---- F1's own anchor: refresh reads (word_addr<0x1000, four per line,
// two memory cycles -- kCck clk_sys -- apart; see ch6-blitter-cpu-sharing's
// own "How slots are numbered at the RAM port" for the same technique).
// kCck itself is measured here, not assumed, from the refresh groups'
// own internal spacing.
struct Anchor {
    bool ok = false;
    uint64_t t_cpu = 0;          // clk_sys per CPU clock period, measured
    std::vector<uint64_t> firsts; // each group's first refresh read
};

Anchor measure_anchor(const std::vector<nmh::RamAccess>& log) {
    Anchor a;
    std::vector<uint64_t> low_reads;
    for (const auto& x : log) if (!x.is_write && x.word_addr < 0x1000) low_reads.push_back(x.cycle);
    if (low_reads.empty()) return a;
    std::sort(low_reads.begin(), low_reads.end());
    std::vector<std::vector<uint64_t>> groups;
    groups.push_back({low_reads[0]});
    for (size_t i = 1; i < low_reads.size(); ++i) {
        if (low_reads[i] - low_reads[i - 1] > 100) groups.push_back({});
        groups.back().push_back(low_reads[i]);
    }
    std::vector<uint64_t> spacings;
    for (const auto& g : groups) {
        if (g.size() != 4) continue;
        uint64_t s = g[1] - g[0];
        bool uniform = true;
        for (size_t i = 1; i < g.size(); ++i) if (g[i] - g[i - 1] != s) { uniform = false; break; }
        if (!uniform) continue;
        spacings.push_back(s);
        a.firsts.push_back(g.front());
    }
    if (spacings.empty()) return a;
    // measured group-internal spacing = 2 memory cycles = 4*T_CPU
    uint64_t s0 = spacings[0];
    for (auto s : spacings) if (s != s0) { a.ok = false; return a; }
    if (s0 % 4 != 0) return a;
    a.t_cpu = s0 / 4;
    // F1's own anchor check: consecutive line marks are 454*T_CPU apart
    // (PAL), or alternately 456/454*T_CPU (NTSC) -- accept either.
    if (a.firsts.size() < 2) { a.ok = false; return a; }
    bool ok = true;
    for (size_t i = 1; i < a.firsts.size(); ++i) {
        uint64_t d = a.firsts[i] - a.firsts[i - 1];
        uint64_t ratio = a.t_cpu ? d / a.t_cpu : 0;
        bool exact = a.t_cpu && (d % a.t_cpu == 0);
        if (!exact || (ratio != 454 && ratio != 456)) { ok = false; break; }
    }
    a.ok = ok;
    return a;
}

// Same check, but against a caller-supplied (possibly wrong) T_CPU -- used
// only for F1's own contrast: "the same check evaluated with 1.25*T_CPU
// fails" (SPECIFICATION.md 1.4, F1).
bool anchor_holds_at(const Anchor& a, double t_cpu_assumed) {
    if (a.firsts.size() < 2) return false;
    for (size_t i = 1; i < a.firsts.size(); ++i) {
        double d = double(a.firsts[i] - a.firsts[i - 1]);
        double ratio = d / t_cpu_assumed;
        double nearest = std::round(ratio);
        if (std::fabs(ratio - nearest) > 0.01) return false;
        if (nearest != 454 && nearest != 456) return false;
    }
    return true;
}

// ---- P6's comparators (SPECIFICATION.md P6, F2), used by P6 and by P5's Pre5.3
// ("the E-index frame is valid for this loop"). A series' phases are taken
// against its first store, folded into [-T_E/2, T_E/2) and expressed in units of
// T_CPU. Store times lie on CPU clock edges (F1), so a comparison is made to the
// resolution of that unit: two phases are told apart, and a separation is
// checked, to within half a T_CPU.
struct PhaseShape {
    bool has_data = false;
    bool tight = true;                 // every cluster of equal phases is no wider than half a T_CPU
    std::vector<double> centers;       // one per cluster, ascending, in T_CPU
    std::set<int> residues;            // round(phase) mod 10, in T_CPU
    double band = 0;                   // (largest - smallest) phase, in T_CPU
};

PhaseShape phase_shape(const std::vector<double>& phases, uint64_t t_cpu) {
    PhaseShape r;
    if (phases.empty() || t_cpu == 0) return r;
    r.has_data = true;
    std::vector<double> v;
    for (double p : phases) v.push_back(p / double(t_cpu));
    std::sort(v.begin(), v.end());
    r.band = v.back() - v.front();
    std::vector<std::vector<double>> clusters;
    for (double x : v) {
        if (clusters.empty() || x - clusters.back().back() > 0.5) clusters.push_back({});
        clusters.back().push_back(x);
    }
    for (const auto& c : clusters) {
        double sum = 0;
        for (double x : c) sum += x;
        if (c.back() - c.front() > 0.5) r.tight = false;
        r.centers.push_back(sum / double(c.size()));
    }
    for (double x : v) r.residues.insert(((int)std::llround(x) % 10 + 10) % 10);
    return r;
}

// K1, K2 and every R-SAMPLE loop (SPECIFICATION.md P6): the phases take at most two values,
// 2*T_CPU apart.
bool frame_holds(const PhaseShape& p) {
    if (!p.has_data || !p.tight || p.centers.size() > 2) return false;
    return p.centers.size() < 2 || std::fabs((p.centers[1] - p.centers[0]) - 2.0) <= 0.5;
}

// K3 (SPECIFICATION.md P6): the phases take exactly the five values 0, 2, 4, 6 and 8*T_CPU mod T_E.
bool five_phases(const PhaseShape& p) {
    return p.has_data && p.residues == std::set<int>{0, 2, 4, 6, 8};
}

std::string shape_text(const PhaseShape& p) {
    std::string s = "values={";
    for (size_t i = 0; i < p.centers.size(); ++i) {
        char b[24];
        std::snprintf(b, sizeof b, "%s%.2f", i ? "," : "", p.centers[i]);
        s += b;
    }
    s += "}*T_CPU, residues mod T_E={";
    bool first = true;
    for (int r : p.residues) { s += (first ? "" : ",") + std::to_string(r); first = false; }
    return s + "}";
}

std::vector<double> series_phases(const SampleSeries& s, uint64_t t_cpu) {
    std::vector<double> ph;
    uint64_t t0 = 0;
    bool have = false;
    for (size_t i = 0; i < s.size(); ++i) {
        if (!s.cycle[i] || !s.value[i]) continue;
        if (!have) { t0 = *s.cycle[i]; have = true; }
        ph.push_back(fold_phase(*s.cycle[i], t0, t_cpu));
    }
    return ph;
}

size_t series_pop(const SampleSeries& s) {
    size_t n = 0;
    for (size_t i = 0; i < s.size(); ++i) if (s.cycle[i] && s.value[i]) ++n;
    return n;
}

// The iteration time of a sampling loop (P6's I3): the most frequent interval between consecutive stores,
// in clk_sys (the smaller on a tie). An interval that one iteration alone lengthens, by a refresh slot or a
// cache fill, does not decide it.
int64_t modal_interval(const SampleSeries& s) {
    std::map<int64_t, int> freq;
    for (size_t i = 0; i + 1 < s.size(); ++i)
        if (s.cycle[i] && s.cycle[i + 1]) ++freq[(int64_t)*s.cycle[i + 1] - (int64_t)*s.cycle[i]];
    int64_t best = 0;
    int best_n = 0;
    for (const auto& [iv, n] : freq) if (n > best_n) { best = iv; best_n = n; }
    return best;
}

// One sample loop's E-index frame: the run completed with F1's anchor, every one of the n specified
// samples is present, and the phases pass frame_holds.
struct LoopFrame {
    bool run_ok = false, full = false, holds = false;
    size_t pop = 0;
    PhaseShape shape;
};

LoopFrame loop_frame(bool loaded, bool done, const Anchor& anc, const SampleSeries& s, size_t n) {
    LoopFrame f;
    f.run_ok = loaded && done && anc.ok;
    f.pop = series_pop(s);
    f.full = f.pop == n;
    if (anc.t_cpu > 0 && f.pop > 0) f.shape = phase_shape(series_phases(s, anc.t_cpu), anc.t_cpu);
    f.holds = f.run_ok && f.full && frame_holds(f.shape);
    return f;
}

// The number of whole pulses (period T_E) a counter can count between a start event and a stop event,
// each known only to lie between a marker stored before its write and a marker stored after it (E5):
// at least (stop_before - start_after)/T_E - 1 and at most (stop_after - start_before)/T_E + 1. The 1
// is the counting of whole pulses in an interval.
struct CountBounds { double lo, hi; };
CountBounds count_bounds(uint64_t start_before, uint64_t start_after, uint64_t stop_before, uint64_t stop_after,
                         uint64_t t_cpu) {
    double t_e = 10.0 * double(t_cpu);
    return {(double(stop_before) - double(start_after)) / t_e - 1.0,
            (double(stop_after) - double(start_before)) / t_e + 1.0};
}

// ---- R-KEY (SPECIFICATION.md 1.5): runs `bin` cycle-by-cycle, waits for a
// write to `ready_addr` (the program's own "ready" marker), measures this
// run's own T_CPU from its own refresh captures, then -- if send_key is
// true -- at least `min_e_after_ready` E periods after the ready marker,
// puts one Amiga key code on the kbd_mouse_type/data/level port group for
// one clk_sys cycle, toggling kbd_mouse_level to mark a new code. No port
// carries KCLK/KDAT, so the test does not drive or time them (R-KEY).
// Continues until DONE or the cycle budget.
struct KeyCapture {
    bool loaded=false, released=false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;
    std::vector<std::optional<uint16_t>> peeks;
    uint64_t t_cpu = 0;
    bool key_sent = false;
    std::optional<uint64_t> send_cycle;   // clk_sys cycle at which the key code was put on the port group
};

KeyCapture run_capture_key(const std::string& bin, uint32_t ready_addr, bool send_key,
                            uint16_t keycode, uint64_t min_e_after_ready, uint64_t budget,
                            const std::vector<uint32_t>& peek_addrs, nmh::Config cfg = nmh::Config{},
                            int param = -1) {
    KeyCapture r;
    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) { register_run(bin, false, false, Ep1Words{}, r.log); return r; }
    if (param >= 0) h.poke_word(0x0C2000 / 2, uint16_t(param));   // the parameter block (E7), before boot
    r.released = h.boot(kBootBudget).cpu_released;
    uint32_t w_ready = word_addr(ready_addr);
    std::optional<uint64_t> ready_cycle;
    bool have_tcpu = false, key_pending = false, found_done = false;
    size_t seen = h.ram_log().size();
    for (uint64_t i = 0; i < budget && !found_done; ++i) {
        if (key_pending) {
            r.send_cycle = h.cycles();
            h.ports()->kbd_mouse_type = 2;
            h.ports()->kbd_mouse_data = keycode;
            h.ports()->kbd_mouse_level = !h.ports()->kbd_mouse_level;
        }
        h.run_cycles(1);
        if (key_pending) {
            h.ports()->kbd_mouse_type = 0;
            key_pending = false;
            r.key_sent = true;
        }
        const auto& lg = h.ram_log();
        for (; seen < lg.size(); ++seen) {
            const auto& a = lg[seen];
            if (a.is_write && a.word_addr == word_addr(kDoneAddr) && a.data == kDoneValue) { r.done = a.cycle; found_done = true; }
            if (a.is_write && a.word_addr == w_ready && !ready_cycle) ready_cycle = a.cycle;
        }
        if (!have_tcpu) {
            Anchor a = measure_anchor(h.ram_log());
            if (a.ok) { r.t_cpu = a.t_cpu; have_tcpu = true; }
        }
        if (send_key && !r.key_sent && !key_pending && ready_cycle && have_tcpu &&
            h.cycles() >= *ready_cycle + min_e_after_ready * 10 * r.t_cpu) {
            key_pending = true;
        }
    }
    r.log = h.ram_log();
    for (auto a : peek_addrs) r.peeks.push_back(h.peek_logical(a));
    register_run(bin, true, r.done.has_value(), ep1_of(h), r.log);
    return r;
}

Capture view_of(const KeyCapture& kc) {
    Capture c;
    c.loaded = kc.loaded; c.released = kc.released; c.done = kc.done;
    c.peeks = kc.peeks; c.log = kc.log;
    return c;
}

// ---- EP2: between the first marker (MK_START) and the last (DONE), the
// Chip-memory port shows only refresh accesses (F4's own shape: a group of
// four reads, two memory cycles apart, on F1's line cadence) or accesses to
// this package's own CPU-owned ranges (E9: the program's own code, its
// results, its markers, the parameter block, K3's byte). Any other access in
// the window is a stray DMA master -- exactly EP2's own contrast (the
// Copper's own fetch of $0C3000, or of a list in low Chip memory).
bool in_range(uint32_t w, uint32_t lo_byte, uint32_t hi_byte) {
    return w >= word_addr(lo_byte) && w < word_addr(hi_byte);
}

bool ep2_ok(const std::vector<nmh::RamAccess>& log, uint64_t lo, uint64_t hi, bool* saw_extra = nullptr,
            size_t* stray_low = nullptr, size_t* stray_other = nullptr) {
    // A read below $1000 is a refresh read only when it belongs to a group of exactly four reads
    // 4*T_CPU apart whose first read is a line mark on F1's cadence relative to a neighbouring group
    // (454 or 456 T_CPU after the previous group's first read, or before the next one's). T_CPU comes
    // from the uniform groups; F1's anchor over the whole run need not hold, because a stray read
    // inside a refresh group is what breaks it.
    Anchor anc = measure_anchor(log);
    if (anc.t_cpu == 0) return false;
    const uint64_t T = anc.t_cpu;
    std::vector<uint64_t> low_reads;
    for (const auto& x : log) if (!x.is_write && x.word_addr < 0x1000) low_reads.push_back(x.cycle);
    std::sort(low_reads.begin(), low_reads.end());
    std::vector<std::vector<uint64_t>> groups;
    for (size_t i = 0; i < low_reads.size(); ++i) {
        if (i == 0 || low_reads[i] - low_reads[i - 1] > 100) groups.push_back({});
        groups.back().push_back(low_reads[i]);
    }
    const size_t n = groups.size();
    std::vector<bool> four(n, false);
    for (size_t gi = 0; gi < n; ++gi) {
        const auto& g = groups[gi];
        bool uniform = g.size() == 4;
        for (size_t k = 1; k < g.size() && uniform; ++k) if (g[k] - g[k - 1] != 4 * T) uniform = false;
        four[gi] = uniform;
    }
    auto line_apart = [&](size_t i, size_t j) {
        uint64_t d = groups[j].front() - groups[i].front();
        return d == 454 * T || d == 456 * T;
    };
    const uint64_t log_end = log.empty() ? 0 : log.back().cycle;
    std::set<uint64_t> refresh;
    for (size_t gi = 0; gi < n; ++gi) {
        const auto& g = groups[gi];
        bool accept = four[gi] && ((gi > 0 && four[gi - 1] && line_apart(gi - 1, gi)) ||
                                   (gi + 1 < n && four[gi + 1] && line_apart(gi, gi + 1)));
        // A run ends at its DONE write, which can fall inside a refresh group: a last group of one to
        // three reads, 4*T_CPU apart, whose next read would come after the end of the log and whose
        // first read is a line mark on the cadence of the group before it, is a refresh group cut short.
        if (!accept && gi > 0 && gi + 1 == n && g.size() < 4 && four[gi - 1] && line_apart(gi - 1, gi) &&
            log_end < g.back() + 4 * T) {
            bool uniform = true;
            for (size_t k = 1; k < g.size(); ++k) if (g[k] - g[k - 1] != 4 * T) uniform = false;
            accept = uniform;
        }
        if (accept) for (uint64_t c : g) refresh.insert(c);
    }
    bool ok = true;
    for (const auto& a : log) {
        if (a.cycle < lo || a.cycle > hi) continue;
        if (!a.is_write && a.word_addr < 0x1000 && refresh.count(a.cycle)) continue;   // refresh
        if (in_range(a.word_addr, 0x080000, 0x081000)) continue;   // program code
        if (in_range(a.word_addr, 0x0C0000, 0x0C1000)) continue;   // results + DONE_ADDR
        if (in_range(a.word_addr, 0x0C1000, 0x0C1020)) continue;   // markers
        if (in_range(a.word_addr, 0x0C2000, 0x0C2020)) continue;   // parameter block
        if (in_range(a.word_addr, 0x0C4000, 0x0C4002)) continue;   // K3's byte
        ok = false;
        if (saw_extra) *saw_extra = true;
        if (!a.is_write && a.word_addr < 0x1000) { if (stray_low) ++*stray_low; }
        else if (stray_other) ++*stray_other;
    }
    return ok;
}

void register_run(const std::string& bin, bool loaded, bool done, const Ep1Words& ep1,
                  const std::vector<nmh::RamAccess>& log) {
    if (!registering()) return;
    RunRecord r;
    r.bin = bin; r.loaded = loaded; r.done = done;
    if (!loaded) { run_registry().push_back(r); return; }
    r.ep1 = true;
    for (const auto& w : ep1) if (!w.has_value() || *w != 0) r.ep1 = false;
    r.f1 = measure_anchor(log).ok;
    r.ep2_graded = done;
    if (r.ep2_graded) {
        auto s = mark(log, kMkStart);
        uint64_t last = log.empty() ? 0 : log.back().cycle;
        r.ep2 = s.has_value() && ep2_ok(log, *s, last);
    }
    run_registry().push_back(r);
}

// ---- P1: per-instance leg grading, gated by Pre1.1 (a2,d2), Pre1.2 (a1,b1), Pre1.3
// (c1) and Pre1.4/O1 (the whole instance -- SPECIFICATION.md P1
// Preconditions). d1/O1C together ARE Pre1.4/O1: DDRB bit n=1, data 0,
// PBON=0 reads 0 (d1); the contrast, data 1 reads 1 (O1C).
struct P1Legs {
    bool o1_live = false;
    bool a2_ok = true, a1_ok = true, b1_ok = true, c1_ok = true, d1_ok = true, d2_ok = true;
    bool otherbits_ok = true;
};

// pbon selects the page's prediction for the legs: with PBON=1 the timer's
// output overrides DDRB and the data (a2, a1, b1 bit n = 0; c1 = 1); with
// PBON=0 the line is in normal operation (a2, a1, b1 bit n = 1, the pull-up or
// the data; c1 = 0). d1 = 0 and d2 = 1 hold for both. The same function grades
// the PBON=1 runs and P1-CONTROL's PBON=0 runs.
P1Legs grade_p1(const Capture& c, uint16_t n_mask, uint16_t icr_mask, uint16_t Q1, uint16_t Q2, bool pbon = true) {
    P1Legs g;
    if (!c.loaded || !c.done || c.peeks.size() <= kP1IdxO1C) return g;
    uint16_t r0 = pk(c,0), a2 = pk(c,1), i2v = pk(c,7), a1 = pk(c,4), b1 = pk(c,5), c1 = pk(c,6);
    uint16_t d1 = pk(c,kP1IdxD1), d2 = pk(c,kP1IdxD2), o1c = pk(c,kP1IdxO1C);
    bool pre11 = pk_ok(c,0) && (r0 & n_mask);
    bool pre13 = pk_ok(c,7) && !(i2v & icr_mask);
    // Pre1.2: step 4's one-shot ended within the poll limit and its own flag is set, so exactly one underflow
    // preceded legs A1 and B (the timer output is low after it).
    bool pre12 = pk_ok(c,2) && pk(c,2) < 1000 && pk_ok(c,3) && (pk(c,3) & icr_mask);
    bool o1_pos = pk_ok(c,kP1IdxD1) && !(d1 & n_mask);
    bool o1_con = pk_ok(c,kP1IdxO1C) && (o1c & n_mask);
    g.o1_live = o1_pos && o1_con;
    if (!g.o1_live) return g;   // instance excluded; CIA-A's read-back is recorded separately
    if (pre11) {
        g.a2_ok = pbon ? !(a2 & n_mask) : (a2 & n_mask) != 0;
        if (pre12) g.a1_ok = pbon ? !(a1 & n_mask) : (a1 & n_mask) != 0;
        g.d2_ok = (d2 & n_mask) != 0;
    }
    if (pre12) g.b1_ok = pbon ? !(b1 & n_mask) : (b1 & n_mask) != 0;
    if (pre13) g.c1_ok = pbon ? (c1 & n_mask) != 0 : !(c1 & n_mask);
    g.d1_ok = !(d1 & n_mask);
    uint16_t bit_o = (n_mask == 0x40) ? 0x80 : 0x40;
    bool ok_a1o = ((a1 ^ r0) & uint16_t(~n_mask & 0xFF)) == 0;
    bool ok_a2o = ((a2 ^ r0) & uint16_t(~n_mask & 0xFF)) == 0;
    bool ok_b1o = ((b1 & 0x3F) == Q1) && (((b1 ^ r0) & bit_o) == 0);
    bool ok_c1o = ((c1 & 0x3F) == Q2) && (((c1 ^ r0) & bit_o) == 0);
    g.otherbits_ok = ok_a1o && ok_a2o && ok_b1o && ok_c1o;
    return g;
}

bool p1_main_ok(const P1Legs& g) { return g.a2_ok && g.a1_ok && g.b1_ok && g.c1_ok && g.d1_ok && g.d2_ok; }

// ---- P2: leg S's rise check, with the level before each start prepended
// (SPECIFICATION.md P2) and Pre2.3's own gating (a start whose level-before
// is not 0 is not graded, its value reported).
struct P2LegSResult {
    bool graded_any = false;
    bool all_ok = true;
    std::vector<std::string> notes;
};

// Start k+1 (k >= 1, 0-based) is graded only where Pre2.2 holds for start k (its one-shot ended before the
// restart): pre22_of(c, k) is Pre2.2 of the 0-based start k.
P2LegSResult grade_p2_legs(const Capture& c, uint16_t n_mask, const char* tag,
                            std::function<std::optional<bool>(const Capture&, int, int)> s_bit,
                            std::function<std::optional<uint16_t>(const Capture&, int)> z_of,
                            std::function<bool(const Capture&, int)> pre22_of) {
    P2LegSResult r;
    if (!c.loaded || !c.done || c.peeks.size() <= kP2IdxS0) return r;
    std::optional<bool> before;
    if (pk_ok(c, kP2IdxS0)) before = (pk(c, kP2IdxS0) & n_mask) != 0;
    for (int k = 0; k < 4; ++k) {
        bool pre23 = before.has_value() && !*before;
        const bool pre22_prev = k == 0 || pre22_of(c, k - 1);
        if (pre23 && pre22_prev) {
            r.graded_any = true;
            int rises = 0; bool any1 = false;
            bool prev = false; // the level before, known 0 here (Pre2.3)
            for (int j = 0; j < 64; ++j) {
                auto b = s_bit(c,k,j);
                if (!b) continue;
                if (*b) any1 = true;
                if (!prev && *b) ++rises;
                prev = *b;
            }
            if (!(rises == 1 && any1)) r.all_ok = false;
        } else {
            std::string state = before.has_value() ? (*before ? "1" : "?") : "unknown";
            r.notes.push_back(std::string(tag) + " start " + std::to_string(k+1) +
                (pre23 ? std::string(" not graded (Pre2.2: start ") + std::to_string(k) + "'s one-shot had not ended before the restart)"
                       : " not graded (Pre2.3: level before is " + state + ", not 0)"));
        }
        auto z = z_of(c,k);
        before = z ? std::optional<bool>((*z & n_mask) != 0) : std::optional<bool>();
    }
    return r;
}

// ---- P4: one latch run, and one instance's six latch runs ------------------
// SPECIFICATION.md P4. The same two functions grade the timer's own runs and
// P4-GREENCTL's CPU-driven pulse source.
struct P4LatchResult {
    bool loaded=false, pre43_ok=false, coverage_ok=false, pulse_ok=false; int64_t P=0;
    int samples=0, high=0; uint16_t vmin=0xFFFF, vmax=0;   // the PRB samples as read
};

P4LatchResult grade_p4_latch(const std::string& bin, uint16_t n_mask, uint16_t icrmask) {
    P4LatchResult r;
    Capture c = run_capture(bin, kRunBudget, {});
    SampleSeries prb = read_series(c.log, kResults+8, 400);
    SampleSeries icr = read_series(c.log, kResults+8+400*2, 400);
    Anchor anc = measure_anchor(c.log);
    for (int i=0;i<400;++i) if (prb.value[i]) {
        ++r.samples;
        r.vmin = std::min<uint16_t>(r.vmin, *prb.value[i] & 0xFF);
        r.vmax = std::max<uint16_t>(r.vmax, *prb.value[i] & 0xFF);
        if (*prb.value[i] & n_mask) ++r.high;
    }
    r.loaded = c.loaded && c.done.has_value() && anc.ok;
    if (!r.loaded) return r;
    uint64_t t0 = 0; bool have = false;
    std::vector<int64_t> e_all; std::vector<bool> bit_all;
    for (int i=0;i<400;++i) {
        if (!prb.cycle[i] || !prb.value[i]) continue;
        if (!have) { t0 = *prb.cycle[i]; have = true; }
        int64_t e = (int64_t)e_index(*prb.cycle[i], t0, anc.t_cpu);
        e_all.push_back(e);
        bit_all.push_back((*prb.value[i] & n_mask) != 0);
    }
    // F3's own f_j/q_j are the ICR stores' own E-indices (not PRB's).
    PeriodResult pr = measure_period(icr, icrmask, anc.t_cpu);
    r.pre43_ok = pr.ok;
    r.P = pr.P;
    if (!pr.ok || pr.P <= 0) return r;
    bool found = false;
    for (int64_t cand=0; cand<pr.P; ++cand) {
        bool ok = true; int highcount = 0;
        for (size_t i=0;i<e_all.size();++i) {
            int64_t res = ((e_all[i]-cand)%pr.P+pr.P)%pr.P;
            bool ishigh = (res==0);
            if (ishigh) ++highcount;
            if (bit_all[i] != ishigh) { ok = false; break; }
        }
        if (ok && highcount >= 3) { found = true; break; }
    }
    r.pulse_ok = found;
    std::vector<int> covcount(pr.P, 0);
    for (auto e : e_all) covcount[((e%pr.P)+pr.P)%pr.P]++;
    bool cov = true; for (auto cnt : covcount) if (cnt < 2) { cov = false; break; }
    r.coverage_ok = cov;
    return r;
}

struct P4InstanceSummary { bool ok=false; int usable=0; int samples=0, high=0; uint16_t vmin=0xFFFF, vmax=0; };

// bins[i] is the run for latch Ls[i]; note_tag names the measured lines.
P4InstanceSummary grade_p4_instance(Checks& checks, const std::string& note_tag, const std::string& tag,
                                    const std::vector<std::string>& bins, const std::vector<int>& Ls,
                                    uint16_t n_mask, uint16_t icrmask) {
    P4InstanceSummary s;
    std::vector<P4LatchResult> rs;
    for (size_t i=0;i<bins.size();++i) rs.push_back(grade_p4_latch(bins[i], n_mask, icrmask));
    for (const auto& r : rs) {
        s.samples += r.samples; s.high += r.high;
        if (r.samples > 0) { s.vmin = std::min(s.vmin, r.vmin); s.vmax = std::max(s.vmax, r.vmax); }
    }
    for (size_t i=0;i<rs.size();++i)
        checks.note(note_tag + " " + tag + " L=" + std::to_string(Ls[i]) + ": measured P(L)=" + std::to_string(rs[i].P) +
                    (rs[i].pre43_ok ? " (uniquely determined)" : " (not uniquely determined)") +
                    ", coverage " + (rs[i].coverage_ok ? "met" : "not met"));
    std::vector<int64_t> deltas; bool pulse_all = true;
    for (size_t i=0;i<rs.size();++i) {
        if (!rs[i].loaded || !rs[i].pre43_ok || !rs[i].coverage_ok) continue;
        ++s.usable;
        deltas.push_back(rs[i].P - Ls[i]);
        if (!rs[i].pulse_ok) pulse_all = false;
    }
    bool delta_ok = !deltas.empty();
    for (auto d : deltas) if (d != deltas[0] || (d != 0 && d != 1)) delta_ok = false;
    s.ok = (s.usable >= 2) && pulse_all && delta_ok;
    return s;
}

} // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    // P10's own aggregate rows need the verdict of every member proposition after
    // its own block has gone out of scope; each block below sets its fields here
    // as it computes its own verdict, already conjoined with the proposition's own
    // preconditions: NOTGRADED when a precondition or the population fails, never
    // PASS or FAIL (SPECIFICATION.md P10). A proposition about timer instances has
    // one verdict per instance, in the order CIAA/timer A, CIAA/timer B, CIAB/timer A,
    // CIAB/timer B; an instance whose PRB read-back is not live (Pre1.4/O1) is
    // NOTGRADED, never PASS. A proposition about the two settings of a bit has one
    // verdict per setting.
    struct PropVerdicts {
        using V4 = std::array<V, 4>;
        V4 p1_main{V::NOTGRADED, V::NOTGRADED, V::NOTGRADED, V::NOTGRADED};
        V4 p1_control{V::NOTGRADED, V::NOTGRADED, V::NOTGRADED, V::NOTGRADED};
        V4 p2{V::NOTGRADED, V::NOTGRADED, V::NOTGRADED, V::NOTGRADED};
        V4 p3{V::NOTGRADED, V::NOTGRADED, V::NOTGRADED, V::NOTGRADED};
        V4 p4{V::NOTGRADED, V::NOTGRADED, V::NOTGRADED, V::NOTGRADED};
        V4 p5{V::NOTGRADED, V::NOTGRADED, V::NOTGRADED, V::NOTGRADED};
        V p70_stop = V::NOTGRADED, p70_start = V::NOTGRADED;
        V p73_oneshot = V::NOTGRADED, p73_cont = V::NOTGRADED;
        V p74 = V::NOTGRADED, p75 = V::NOTGRADED, p75_ciaa = V::NOTGRADED;
        V p74_read = V::NOTGRADED, p84_read = V::NOTGRADED;   // LOAD reads 0 (c bit 4), the 0-setting rows' member
        V p76_ciaa = V::NOTGRADED, p76_ciab = V::NOTGRADED, p77 = V::NOTGRADED;
        V p80_stop = V::NOTGRADED, p80_start = V::NOTGRADED;
        V p83_oneshot = V::NOTGRADED, p83_cont = V::NOTGRADED, p84 = V::NOTGRADED;
        V p865_01 = V::NOTGRADED, p865_10 = V::NOTGRADED, p865_11 = V::NOTGRADED;
        V p87_x = V::NOTGRADED, p87_y = V::NOTGRADED;
        V p9_legf = V::NOTGRADED, p9_legf_trans = V::NOTGRADED, p9_legw = V::NOTGRADED;
        // inputs of the preconditions P10 conjoins that are printed before the section that needs them
        bool p1_pre_ok = false;
        bool p6_p9loop_ok = false;
    } pv;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";
    // NMH_MUTATE names one selector. It may also name several, joined by commas, when a run replays
    // records whose selectors change different programs and different lines.
    auto sel = [&](const char* name) {
        const std::string list = "," + mv + ",";
        return list.find("," + std::string(name) + ",") != std::string::npos;
    };

    // ---- fixture, asserted first (SPECIFICATION.md 1.2 E7) ------------
    {
        NoRegister dedicated;
        Capture f = run_capture(dir + (sel("FIXTURE") ? "/p1_ciaa_ta_fixture.bin" : "/p1_ciaa_ta.bin"),
                                kRunBudget, {kFixtureAddr});
        checks.expect(f.loaded && f.done.has_value() && pk_ok(f,0) && pk(f,0) == kFixtureValue,
            "FIXTURE [SPECIFICATION.md 1.2]: the program can deliver a known stored constant "
            "before any other result is graded");
    }

    // ---- P1 instances -----------------------------------------------------
    std::string p1_ciaa_ta = dir + "/p1_ciaa_ta.bin";
    std::string p1_ciaa_tb = dir + (sel("EP1-PER-RUN") ? "/p1_ciaa_tb_ep1.bin"
                                    : sel("EP2-PER-RUN") ? "/p1_ciaa_tb_ep2.bin"
                                    : sel("EP3-PER-RUN") ? "/p1_ciaa_tb_ep3.bin"
                                    : sel("F1-ANCHOR-PER-RUN") ? "/p1_ciaa_tb_f1.bin"
                                    : "/p1_ciaa_tb.bin");
    std::string p1_ciab_ta = dir + "/p1_ciab_ta.bin";
    std::string p1_ciab_tb = dir + (sel("P1-PRE11") ? "/p1_ciab_tb_pre11.bin"
                                    : sel("P1-PRE12") ? "/p1_ciab_tb_pre12.bin"
                                    : sel("P1-PRE13") ? "/p1_ciab_tb_pre13.bin"
                                    : sel("P1-PRE14") ? "/p1_ciab_tb_pre14.bin"
                                    : sel("P1-OTHERBITS") ? "/p1_ciab_tb_otherbits.bin"
                                    : "/p1_ciab_tb.bin");

    std::vector<uint32_t> p1_addrs = {kP1R0,kP1A2,kP1Poll1,kP1I1,kP1A1,kP1B1,kP1C1,kP1I2,kP1D1,kP1D2,
                                       kP1O1C,kEp1CraA,kEp1CrbA,kEp1CraB,kEp1CrbB};
    Capture p1a = run_capture(p1_ciaa_ta, kRunBudget, p1_addrs);
    Capture p1b = run_capture(p1_ciaa_tb, kRunBudget, p1_addrs);
    Capture p1c = run_capture(p1_ciab_ta, kRunBudget, p1_addrs);
    // CIAB/timer-B needs the LED trace too (O2), so it uses run_capture_led
    // instead of the plain capture.
    LedCapture p1d = run_capture_led(p1_ciab_tb, kRunBudget, p1_addrs);
    Capture p1d_view = view_of(p1d);

    // ---- P1-PRE1.1 - P1-PRE1.3 (unchanged channels; O1C does not move
    // their own indices) --------------------------------------------------
    bool pre11 = pk_ok(p1a,0) && (pk(p1a,0)&0x40) && pk_ok(p1b,0) && (pk(p1b,0)&0x80) &&
                 pk_ok(p1c,0) && (pk(p1c,0)&0x40) && pk_ok(p1d_view,0) && (pk(p1d_view,0)&0x80);
    checks.expect(pre11, "P1-PRE1.1 [p341,p354]: the pull-up reference r0 has bit n=1, for all four instances");

    auto poll_converged = [](const Capture& c) { return pk_ok(c,2) && pk(c,2) < 1000; };
    bool pre12 = poll_converged(p1a) && (pk(p1a,3)&kIcrTa) && poll_converged(p1b) && (pk(p1b,3)&kIcrTb) &&
                 poll_converged(p1c) && (pk(p1c,3)&kIcrTa) && poll_converged(p1d_view) && (pk(p1d_view,3)&kIcrTb);
    checks.expect(pre12, "P1-PRE1.2 [p342]: exactly one underflow happens before legs A1 and B on all four instances "
                         "(CR bit 0 read 0 within the poll limit, i1's own flag set)");

    bool pre13 = !(pk(p1a,7)&kIcrTa) && !(pk(p1b,7)&kIcrTb) && !(pk(p1c,7)&kIcrTa) && !(pk(p1d_view,7)&kIcrTb);
    checks.expect(pre13, "P1-PRE1.3 [p342]: no underflow happens during leg C on all four instances (i2's own flag clear)");

    // ---- P1-PRE1.4/O1: DDRB bit n=1, data 0, PBON=0 reads 0 (d1); data 1
    // reads 1 (O1C's own contrast leg). Graded only where a live channel is
    // expected (CIAB); CIA-A's read-back is recorded, not asserted as a failed
    // precondition -- it is the fact P1's own legs are gated on.
    P1Legs ga = grade_p1(p1a, 0x40, kIcrTa, 0x2A, 0x15);       // CIAA/TA
    P1Legs gb = grade_p1(p1b, 0x80, kIcrTb, 0x2A, 0x15);       // CIAA/TB
    P1Legs gc = grade_p1(p1c, 0x40, kIcrTa, 0x3D, 0x3B);       // CIAB/TA
    P1Legs gd = grade_p1(p1d_view, 0x80, kIcrTb, 0x3D, 0x3B);  // CIAB/TB

    checks.expect(gc.o1_live && gd.o1_live, "P1-PRE1.4/O1 [p341]: DDRB bit n=1, data 0, PBON=0 reads 0, and data 1 "
                              "reads 1 (CIAB/timer A and CIAB/timer B)");
    checks.record("P1-PRE1.4/O1 [p341], CIA-A: not graded as a live read-back -- measured " +
                   hex2(pk(p1a,kP1IdxD1)) + " (d1) and " + hex2(pk(p1a,kP1IdxO1C)) + " (O1 contrast) on "
                   "CIAA/timer A, " + hex2(pk(p1b,kP1IdxD1)) + " and " + hex2(pk(p1b,kP1IdxO1C)) +
                   " on CIAA/timer B; CIA-A's instances are not graded on P1's PBON legs or on "
                   "P1-OTHERBITS' pattern-bit clause");

    // ---- P1-MAIN, split by id: timer-A instances carry the CRA id,
    // timer-B instances the CRB ids, and the p341/p342 ids sit on the
    // conjunction of all four plus the other-bits clause.
    // Each instance's legs are gated by Pre1.1, Pre1.3 and Pre1.4/O1 above;
    // an excluded instance (O1 not live) contributes vacuously.
    // The population of each line below is the set of instances whose PRB
    // read-back is live (Pre1.4/O1). An excluded instance is not part of it;
    // a line whose population is empty is not graded.
    const int n_live_cra = int(ga.o1_live) + int(gc.o1_live);
    const int n_live_crb = int(gb.o1_live) + int(gd.o1_live);
    const int n_live_all = n_live_cra + n_live_crb;
    const std::string no_live = "no instance has a live read-back (Pre1.4/O1)";
    bool cra_ok = p1_main_ok(ga) && p1_main_ok(gc);
    bool crb_ok = p1_main_ok(gb) && p1_main_ok(gd);
    bool otherbits_all = ga.otherbits_ok && gb.otherbits_ok && gc.otherbits_ok && gd.otherbits_ok;
    bool main_ok = cra_ok && crb_ok && otherbits_all;
    {
        const P1Legs* gs[4] = {&ga, &gb, &gc, &gd};
        pv.p1_pre_ok = pre11 && pre12 && pre13;
        for (int i = 0; i < 4; ++i)
            pv.p1_main[i] = (!gs[i]->o1_live || !pv.p1_pre_ok) ? V::NOTGRADED : vof(true, p1_main_ok(*gs[i]));
    }

    emit(checks, n_live_cra > 0, cra_ok, "P1-MAIN-CRA",
         "P1-MAIN-CRA [p349: sol:F-cra.c17]: PBON=1 puts timer A's output on PB6, "
         "forcing an output over DDRB and the PR data, for the timer-A instances "
         "whose read-back is live (Pre1.4/O1)", "no timer-A instance has a live read-back (Pre1.4/O1)");
    emit(checks, n_live_crb > 0, crb_ok, "P1-MAIN-CRB",
         "P1-MAIN-CRB [p349: sol:F-crb.c3, opus:F-crb.c2; p350: sol:F-crb.c14]: PBON=1 "
         "puts timer B's output on PB7, forcing an output over DDRB and the PR data, "
         "for the timer-B instances whose read-back is live (Pre1.4/O1)",
         "no timer-B instance has a live read-back (Pre1.4/O1)");
    emit(checks, n_live_all > 0, main_ok, "P1-MAIN",
        "P1-MAIN [p342: opus:F-timers.c10, opus:F-timers.c11, sol:F-timers.c12; "
        "p341: opus:F-ports.c7, sol:F-ports.c7]: PBON=1 puts the timer's output on PB6 (timer A) or "
        "PB7 (timer B) only, forcing an output over DDRB and the PR data, for the instances whose "
        "read-back is live, including the other-bits clause", no_live);

    emit(checks, n_live_all > 0, otherbits_all, "P1-OTHERBITS",
        "P1-OTHERBITS [p342]: bits 0-5 of b1 and c1 match this instance's own written pattern, and bit "
        "o matches r0, for the instances whose read-back is live (Pre1.4/O1)", no_live);
    checks.note("P1-OTHERBITS, CIA-A (not graded): CIAA/timer A b1=" + hex2(pk(p1a,5)) + " c1=" + hex2(pk(p1a,6)) +
                " (pattern $2A/$15 expected in bits 0-5); CIAA/timer B b1=" + hex2(pk(p1b,5)) + " c1=" +
                hex2(pk(p1b,6)) + " (pattern $2A/$15 expected)");

    // Per-instance values, for the record (SPECIFICATION.md section 5).
    auto p1_dump = [&](const Capture& c, const char* tag) {
        checks.note(std::string(tag) + ": r0=" + hex2(pk(c,0)) + " a2=" + hex2(pk(c,1)) +
            " a1=" + hex2(pk(c,4)) + " b1=" + hex2(pk(c,5)) + " c1=" + hex2(pk(c,6)) +
            " d1=" + hex2(pk(c,kP1IdxD1)) + " o1c=" + hex2(pk(c,kP1IdxO1C)) + " d2=" + hex2(pk(c,kP1IdxD2)));
    };
    p1_dump(p1a, "P1 CIAA/TA"); p1_dump(p1b, "P1 CIAA/TB");
    p1_dump(p1c, "P1 CIAB/TA"); p1_dump(p1d_view, "P1 CIAB/TB");

    // ---- P1-LP / P1-O2: recorded, not graded; nothing is graded on the
    // light. The three readings below are the raw level fdd_led read at each
    // point, not a comparison against the page's own prediction.
    auto mk_cycle = [&](uint32_t addr) { return mark(p1d.log, addr); };
    auto lc2 = mk_cycle(kMkLpOnEnd), lc3 = mk_cycle(kMkLpOffEnd);
    std::optional<uint8_t> led_on_leg, led_off_leg;
    if (lc2) led_on_leg = p1d.led_at(*lc2);
    if (lc3) led_off_leg = p1d.led_at(*lc3);
    auto led_str = [](std::optional<uint8_t> v) { return v ? (*v != 0 ? "on" : "off") : "?"; };
    const bool led_same = led_on_leg.has_value() && led_off_leg.has_value() &&
                          *led_on_leg == *led_off_leg && (*led_on_leg != 0) == p1d.led_before_boot;
    checks.record("P1-LP/P1-O2 [p244]: not graded -- measured: LED when the CPU leaves reset=" +
                   std::string(p1d.led_before_boot?"on":"off") +
                   ", after R-SELECT with MTR*=0 (on leg)=" + led_str(led_on_leg) +
                   ", after R-SELECT with MTR*=1 (off leg)=" + led_str(led_off_leg) +
                   (led_same ? " -- the same level at all three points, so no input of this package "
                               "is shown to move the light at this port"
                             : " -- the level differs between the points") +
                   "; O2's own legs (LED-A1, LED-C, LED-D1, LED-D2) are not graded");

    // ---- P1-CONTROL: P1's own comparator (grade_p1, with the PBON=0
    // prediction) on the PBON=0 runs, all four instances, one per instance
    // whose read-back is live: the green control P1-MAIN's own red needs
    // (SPECIFICATION.md P1, "P1's control").
    {
        std::string cd_bin = dir + (sel("P1-CONTROL") ? "/p1_ciab_tb_pbon0_b0.bin"
                                    : sel("P1-CONTROL-PRE11") ? "/p1_ciab_tb_pbon0_pre11.bin"
                                    : sel("P1-CONTROL-PRE12") ? "/p1_ciab_tb_pbon0_pre12.bin"
                                    : sel("P1-CONTROL-PRE13") ? "/p1_ciab_tb_pbon0_pre13.bin"
                                    : "/p1_ciab_tb_pbon0.bin");
        Capture ca = run_capture(dir + "/p1_ciaa_ta_pbon0.bin", kRunBudget, p1_addrs);
        Capture cb = run_capture(dir + "/p1_ciaa_tb_pbon0.bin", kRunBudget, p1_addrs);
        Capture cc = run_capture(dir + "/p1_ciab_ta_pbon0.bin", kRunBudget, p1_addrs);
        Capture cd = run_capture(cd_bin, kRunBudget, p1_addrs);
        // Pre1.1-Pre1.3 on the control's own runs, printed before the line they gate.
        const Capture* kcs[4] = {&ca, &cb, &cc, &cd};
        const uint16_t kn[4] = {0x40, 0x80, 0x40, 0x80};
        const uint16_t kic[4] = {kIcrTa, kIcrTb, kIcrTa, kIcrTb};
        bool cpre11 = true, cpre12 = true, cpre13 = true;
        for (int i = 0; i < 4; ++i) {
            const Capture& c = *kcs[i];
            cpre11 = cpre11 && pk_ok(c,0) && (pk(c,0) & kn[i]);
            cpre12 = cpre12 && poll_converged(c) && (pk(c,3) & kic[i]);
            cpre13 = cpre13 && !(pk(c,7) & kic[i]);
        }
        checks.expect(cpre11, "P1-CONTROL-PRE1.1 [p341,p354]: the pull-up reference r0 has bit n=1, on the "
                              "PBON=0 runs of all four instances");
        checks.expect(cpre12, "P1-CONTROL-PRE1.2 [p342]: exactly one underflow happens before leg A1 on the "
                              "PBON=0 runs of all four instances (CR bit 0 read 0 within the poll limit, i1's own flag set)");
        checks.expect(cpre13, "P1-CONTROL-PRE1.3 [p342]: no underflow happens during leg C on the PBON=0 runs of "
                              "all four instances (i2's own flag clear)");
        const bool cpre_ok = cpre11 && cpre12 && cpre13;
        P1Legs ka = grade_p1(ca, 0x40, kIcrTa, 0x2A, 0x15, false), kb = grade_p1(cb, 0x80, kIcrTb, 0x2A, 0x15, false);
        P1Legs kc = grade_p1(cc, 0x40, kIcrTa, 0x3D, 0x3B, false), kd = grade_p1(cd, 0x80, kIcrTb, 0x3D, 0x3B, false);
        int n_live_ctl = int(ka.o1_live) + int(kb.o1_live) + int(kc.o1_live) + int(kd.o1_live);
        bool control_ok = p1_main_ok(ka) && p1_main_ok(kb) && p1_main_ok(kc) && p1_main_ok(kd);
        {
            const P1Legs* ks[4] = {&ka, &kb, &kc, &kd};
            for (int i = 0; i < 4; ++i)
                pv.p1_control[i] = (!ks[i]->o1_live || !cpre_ok) ? V::NOTGRADED : vof(true, p1_main_ok(*ks[i]));
        }
        emit(checks, n_live_ctl > 0 && cpre_ok, control_ok, "P1-CONTROL",
            "P1-CONTROL [p342]: P1's own comparator on the PBON=0 runs: every leg reads the page's own "
            "PBON=0 values (a2,a1,b1 bit n=1; c1=0; d1=0; d2=1), for the instances whose read-back is live "
            "(Pre1.4/O1)", cpre_ok ? no_live : "P1-CONTROL-PRE1.1, -PRE1.2 or -PRE1.3 failed");
    }

    // ---- P2 instances -------------------------------------------------
    std::string p2_ciaa_ta = dir + "/p2_ciaa_ta.bin";
    std::string p2_ciaa_tb = dir + "/p2_ciaa_tb.bin";
    std::string p2_ciab_ta = dir + "/p2_ciab_ta.bin";
    std::string p2_ciab_tb_bin = dir + (sel("P2-PRE22") ? "/p2_ciab_tb_pre22.bin"
                                        : sel("P2-PRE23") ? "/p2_ciab_tb_pre23.bin"
                                        : sel("P2-PRE24") ? "/p2_ciab_tb_pre24.bin"
                                        : "/p2_ciab_tb.bin");

    std::vector<uint32_t> p2_addrs = {kP2R1, kP2Z+0,kP2Z+2,kP2Z+4,kP2Z+6, kP2I+0,kP2I+2,kP2I+4,kP2I+6,
                                       kP2H1,kP2H2,kP2H3,kP2Ih};
    for (int k=0;k<4;++k) for (int i=0;i<64;++i) p2_addrs.push_back(kP2S + k*128 + i*2);
    for (int k=0;k<4;++k) p2_addrs.push_back(kP2Poll + k*2);
    p2_addrs.push_back(kP2S0);
    p2_addrs.push_back(kEp1CraA); p2_addrs.push_back(kEp1CrbA);
    p2_addrs.push_back(kEp1CraB); p2_addrs.push_back(kEp1CrbB);

    Capture p2a = run_capture(p2_ciaa_ta, kRunBudget, p2_addrs);
    Capture p2b = run_capture(p2_ciaa_tb, kRunBudget, p2_addrs);
    Capture p2c = run_capture(p2_ciab_ta, kRunBudget, p2_addrs);
    LedCapture p2d = run_capture_led(p2_ciab_tb_bin, kRunBudget, p2_addrs);
    Capture p2d_view = view_of(p2d);

    // s_k[j] as bit n, from the flat peek array (index 13 + k*64 + j).
    auto p2_s_bit = [](const Capture& c, int k, int j, uint16_t n_mask) -> std::optional<bool> {
        size_t idx = kP2IdxSBase + size_t(k)*64 + size_t(j);
        if (idx >= c.peeks.size() || !c.peeks[idx]) return std::nullopt;
        return (*c.peeks[idx] & n_mask) != 0;
    };
    auto p2_z = [](const Capture& c, int k) -> std::optional<uint16_t> {
        size_t idx = 1 + size_t(k);
        if (idx >= c.peeks.size() || !c.peeks[idx]) return std::nullopt;
        return *c.peeks[idx];
    };
    auto p2_i = [](const Capture& c, int k) -> std::optional<uint16_t> {
        size_t idx = 5 + size_t(k);
        if (idx >= c.peeks.size() || !c.peeks[idx]) return std::nullopt;
        return *c.peeks[idx];
    };

    // ---- Pre2.1-Pre2.4, printed before the lines they gate ---------------------
    // Pre2.1 is recorded, not graded: no output of this package shows a write that precedes leg R's own
    // beyond EP1's first reads. The page's contrast (a start before leg R's write gives r1 = 1) is
    // P2-LEGR-GREENCTL's mutation.
    checks.record("P2-PRE2.1 [p342]: the program's first write to this CR is leg R's own; the only "
                   "observation of an earlier write is EP1's first read of CRA and CRB, graded by "
                   "EP1-PER-RUN");

    // Pre2.2: before each restart, the previous one-shot has ended (CR bit 0 read 0 within the poll limit --
    // stored -- and the flag set), for starts 1 to 3 of all four instances (start 4 is not followed by a
    // restart). p2_pre22_of is the precondition of the 0-based start k of one instance.
    auto p2_pre22_of = [&](const Capture& c, int k, uint16_t icr_mask) {
        if (!c.loaded || !c.done) return false;
        size_t idx = kP2IdxPoll + k;
        if (idx >= c.peeks.size() || !c.peeks[idx]) return false;
        auto ik = p2_i(c,k);
        return *c.peeks[idx] < 1000 && ik.has_value() && (*ik & icr_mask);
    };
    auto p2_poll_ok = [&](const Capture& c, uint16_t icr_mask) {
        return p2_pre22_of(c, 0, icr_mask) && p2_pre22_of(c, 1, icr_mask) && p2_pre22_of(c, 2, icr_mask);
    };
    bool pre22_all = p2_poll_ok(p2a,kIcrTa) && p2_poll_ok(p2b,kIcrTb) && p2_poll_ok(p2c,kIcrTa) && p2_poll_ok(p2d_view,kIcrTb);
    checks.expect(pre22_all, "P2-PRE2.2 [p342]: before each restart, the previous one-shot has ended "
                             "(CR bit 0 read 0 within the poll limit, i_k's own flag set), for starts 1 to 3 "
                             "of all four instances");

    // Pre2.3: the level before start 2 is 0 (z_1's own bit n clear), so start 2 is graded. Mutated via the
    // top-level selection (start 1 written as $0D with DDRB left an input).
    bool z1_ok_p2d = false;
    {
        auto z1 = p2_z(p2d_view, 0);
        bool ok = z1.has_value() && !(*z1 & 0x80);
        z1_ok_p2d = ok;
        checks.expect(ok, "P2-PRE2.3 [p342]: the level before start 2 is 0 (z_1's own bit n clear, "
                          "CIAB/timer B), so start 2 is graded");
    }

    // Pre2.4: no underflow happens in leg H (ih's own flag clear), for all four instances.
    auto p2_ih_ok = [](const Capture& c, uint16_t icr_mask) {
        return c.loaded && c.done.has_value() && pk_ok(c,12) && !(pk(c,12)&icr_mask);
    };
    bool pre24_all = p2_ih_ok(p2a,kIcrTa) && p2_ih_ok(p2b,kIcrTb) && p2_ih_ok(p2c,kIcrTa) && p2_ih_ok(p2d_view,kIcrTb);
    checks.expect(pre24_all, "P2-PRE2.4 [p342]: no underflow happens in leg H (ih's own flag clear), "
                             "for all four instances");

    // ---- component lines (no claim id: SPECIFICATION.md P2 puts
    // opus:F-timers.c14 on one assertion over legs R, S and H together,
    // with each leg's own line carrying no id) --------------------------
    // Gated the same way as P1-MAIN (Pre1.4/O1, reused from the P1 captures
    // above: it is a fact of the instance's own read-back channel, not of
    // which program runs it) -- CIA-A's instances are excluded, as they are
    // from P1-MAIN; P2-MAIN gets the same exclusion for free through its
    // own Pre2.3 (s0 never reads 0 on CIA-A). P2-MAIN grades start k+1 only
    // where Pre2.2 holds for start k; P2-LEGH grades an instance only where
    // Pre2.4 holds for it.
    auto p2_legr_component = [](const Capture& c, uint16_t n_mask) {
        return c.loaded && c.done.has_value() && pk_ok(c,0) && !(pk(c,0)&n_mask);
    };
    bool p2_legr_all = (!ga.o1_live || p2_legr_component(p2a,0x40)) &&
                        (!gb.o1_live || p2_legr_component(p2b,0x80)) &&
                        (!gc.o1_live || p2_legr_component(p2c,0x40)) &&
                        (!gd.o1_live || p2_legr_component(p2d_view,0x80));
    const int n_live_p2 = int(ga.o1_live) + int(gb.o1_live) + int(gc.o1_live) + int(gd.o1_live);
    const std::string no_live_p2 = "no instance has a live read-back (Pre1.4/O1)";
    emit(checks, n_live_p2 > 0, p2_legr_all, "P2-LEGR",
         "P2-LEGR [p342]: the toggle output is low after RES (before any start), "
         "for the instances whose read-back is live (Pre1.4/O1)", no_live_p2);

    auto p2_pre22_ta = [&](const Capture& c, int k) { return p2_pre22_of(c, k, kIcrTa); };
    auto p2_pre22_tb = [&](const Capture& c, int k) { return p2_pre22_of(c, k, kIcrTb); };
    P2LegSResult r_a = grade_p2_legs(p2a, 0x40, "CIAA/TA",
        [&](const Capture& c, int k, int j){ return p2_s_bit(c,k,j,0x40); }, p2_z, p2_pre22_ta);
    P2LegSResult r_b = grade_p2_legs(p2b, 0x80, "CIAA/TB",
        [&](const Capture& c, int k, int j){ return p2_s_bit(c,k,j,0x80); }, p2_z, p2_pre22_tb);
    P2LegSResult r_c = grade_p2_legs(p2c, 0x40, "CIAB/TA",
        [&](const Capture& c, int k, int j){ return p2_s_bit(c,k,j,0x40); }, p2_z, p2_pre22_ta);
    P2LegSResult r_d = grade_p2_legs(p2d_view, 0x80, "CIAB/TB",
        [&](const Capture& c, int k, int j){ return p2_s_bit(c,k,j,0x80); }, p2_z, p2_pre22_tb);
    bool p2_legs_all = r_a.all_ok && r_b.all_ok && r_c.all_ok && r_d.all_ok;
    const bool p2_main_graded = r_a.graded_any || r_b.graded_any || r_c.graded_any || r_d.graded_any;
    emit(checks, p2_main_graded, p2_legs_all, "P2-MAIN",
         "P2-MAIN [p342]: every start whose own level-before is 0 (Pre2.3) and whose predecessor's one-shot "
         "had ended (Pre2.2) sets the "
         "toggle output high (the level before, prepended, followed by exactly one "
         "rise and at least one 1), for all four instances",
         "no start of any instance has Pre2.2 and Pre2.3");
    for (auto& n : r_a.notes) checks.note(n);
    for (auto& n : r_b.notes) checks.note(n);
    for (auto& n : r_c.notes) checks.note(n);
    for (auto& n : r_d.notes) checks.note(n);

    auto p2_legh_component = [](const Capture& c, uint16_t n_mask) {
        return c.loaded && c.done.has_value() && pk_ok(c,9) && pk_ok(c,11) && (pk(c,9)&n_mask) && (pk(c,11)&n_mask);
    };
    const bool legh_live[4] = {ga.o1_live && p2_ih_ok(p2a,kIcrTa), gb.o1_live && p2_ih_ok(p2b,kIcrTb),
                               gc.o1_live && p2_ih_ok(p2c,kIcrTa), gd.o1_live && p2_ih_ok(p2d_view,kIcrTb)};
    const int n_legh = int(legh_live[0]) + int(legh_live[1]) + int(legh_live[2]) + int(legh_live[3]);
    bool p2_legh_all = (!legh_live[0] || p2_legh_component(p2a,0x40)) &&
                        (!legh_live[1] || p2_legh_component(p2b,0x80)) &&
                        (!legh_live[2] || p2_legh_component(p2c,0x40)) &&
                        (!legh_live[3] || p2_legh_component(p2d_view,0x80));
    emit(checks, n_legh > 0, p2_legh_all, "P2-LEGH",
         "P2-LEGH [p342]: h1=1 (set high on start) and h3=1 (set high again after a "
         "stop, whether or not h2 separated it from toggling), for the instances whose "
         "read-back is live (Pre1.4/O1) and whose leg H had no underflow (Pre2.4)",
         n_live_p2 > 0 ? "no instance with a live read-back has Pre2.4" : no_live_p2);

    // Per-instance values, for the record (SPECIFICATION.md section 5).
    auto p2_dump = [&](const Capture& c, const char* tag) {
        auto z1=p2_z(c,0), z2=p2_z(c,1), z3=p2_z(c,2), z4=p2_z(c,3);
        checks.note(std::string(tag) + ": r1=" + hex2(pk(c,0)) +
            " z1=" + (z1?hex2(*z1):"?") + " z2=" + (z2?hex2(*z2):"?") +
            " z3=" + (z3?hex2(*z3):"?") + " z4=" + (z4?hex2(*z4):"?") +
            " h1=" + hex2(pk(c,9)) + " h2=" + hex2(pk(c,10)) + " h3=" + hex2(pk(c,11)) +
            " s0=" + (pk_ok(c,kP2IdxS0) ? hex2(pk(c,kP2IdxS0)) : "?"));
    };
    p2_dump(p2a, "P2 CIAA/TA"); p2_dump(p2b, "P2 CIAA/TB");
    p2_dump(p2c, "P2 CIAB/TA"); p2_dump(p2d_view, "P2 CIAB/TB");

    {
        const V vr = vof(n_live_p2 > 0, p2_legr_all), vs = vof(p2_main_graded, p2_legs_all),
                vh = vof(n_legh > 0, p2_legh_all);
        const bool any_fail = vr == V::FAIL || vs == V::FAIL || vh == V::FAIL;
        const bool any_ng = vr == V::NOTGRADED || vs == V::NOTGRADED || vh == V::NOTGRADED;
        const V p2_toggle = any_fail ? V::FAIL : any_ng ? V::NOTGRADED : V::PASS;
        emit(checks, p2_toggle != V::NOTGRADED, p2_toggle == V::PASS, "P2-TOGGLE",
             "P2-TOGGLE [p342] opus:F-timers.c14: the toggle output is set low by RES and set high whenever "
             "the timer is started, whether it was low or high before (legs R, S and H together)",
             "a leg (R, S or H) has no graded population");
    }

    checks.record("LED-R [p244]: not graded -- O2 is not graded at this port; see P1-LP/P1-O2");

    {
        // One verdict per instance, conjoined with Pre1.4/O1 (the instance's read-back is live) and
        // Pre2.2, Pre2.3 and Pre2.4 (printed above): legs R, S and H of that instance.
        const bool p2_pre_ok = pre22_all && pre24_all && z1_ok_p2d;
        const Capture* cs[4] = {&p2a, &p2b, &p2c, &p2d_view};
        const uint16_t ms[4] = {0x40, 0x80, 0x40, 0x80};
        const bool live[4] = {ga.o1_live, gb.o1_live, gc.o1_live, gd.o1_live};
        const P2LegSResult* rs[4] = {&r_a, &r_b, &r_c, &r_d};
        for (int i = 0; i < 4; ++i) {
            if (!live[i] || !p2_pre_ok || !rs[i]->graded_any) { pv.p2[i] = V::NOTGRADED; continue; }
            pv.p2[i] = vof(true, p2_legr_component(*cs[i], ms[i]) && rs[i]->all_ok && p2_legh_component(*cs[i], ms[i]));
        }
    }

    // ---- P2 live controls (SPECIFICATION.md P2, "Live controls"): the same
    // comparators (p2_legr_component, grade_p2_legs, p2_legh_component) on a
    // CPU-driven level in place of the timer's output: PBON=0, DDRB bit n an
    // output, and the CPU writes PRB bit n low before any start, high right after
    // every start and low again after a one-shot has ended. CIAB timer A and
    // timer B (the instances whose read-back is live). The start and underflow
    // preconditions are asserted on these runs too.
    {
        std::string q_tb_bin = dir + (sel("P2-LEGR-GREENCTL") ? "/p2c_ciab_tb_legr.bin"
                                      : sel("P2-MAIN-GREENCTL") ? "/p2c_ciab_tb_k4.bin"
                                      : sel("P2-LEGH-GREENCTL") ? "/p2c_ciab_tb_h1.bin"
                                      : sel("P2-GREENCTL-PRE22") ? "/p2c_ciab_tb_pre22.bin"
                                      : sel("P2-GREENCTL-PRE23") ? "/p2c_ciab_tb_pre23.bin"
                                      : sel("P2-GREENCTL-PRE24") ? "/p2c_ciab_tb_pre24.bin"
                                      : "/p2c_ciab_tb.bin");
        Capture q_a = run_capture(dir + "/p2c_ciab_ta.bin", kRunBudget, p2_addrs);
        Capture q_b = run_capture(q_tb_bin, kRunBudget, p2_addrs);
        const int n_live_ctl2 = int(gc.o1_live) + int(gd.o1_live);
        bool cpre22 = p2_poll_ok(q_a, kIcrTa) && p2_poll_ok(q_b, kIcrTb);
        auto z1_clear = [&](const Capture& c, uint16_t m) { auto z = p2_z(c, 0); return z.has_value() && !(*z & m); };
        bool cpre23 = z1_clear(q_a, 0x40) && z1_clear(q_b, 0x80);
        bool cpre24 = p2_ih_ok(q_a, kIcrTa) && p2_ih_ok(q_b, kIcrTb);
        checks.expect(cpre22, "P2-GREENCTL-PRE2.2 [p342]: on the CPU-driven runs, before each restart the previous "
                              "one-shot has ended (CR bit 0 read 0 within the poll limit, i_k's own flag set), "
                              "starts 1 to 3 (CIAB timer A and timer B)");
        checks.expect(cpre23, "P2-GREENCTL-PRE2.3 [p342]: on the CPU-driven runs, the level before start 2 is 0 "
                              "(z_1's own bit n clear; CIAB timer A and timer B)");
        checks.expect(cpre24, "P2-GREENCTL-PRE2.4 [p342]: on the CPU-driven runs, no underflow happens in leg H "
                              "(ih's own flag clear; CIAB timer A and timer B)");
        const bool cpre = cpre22 && cpre23 && cpre24;
        const std::string why_pre = "a P2-GREENCTL precondition (2.2, 2.3 or 2.4) failed";
        const std::string why_live = "no CIAB instance has a live read-back (Pre1.4/O1)";

        bool c_legr = (!gc.o1_live || p2_legr_component(q_a, 0x40)) && (!gd.o1_live || p2_legr_component(q_b, 0x80));
        P2LegSResult kr_a = grade_p2_legs(q_a, 0x40, "CPU CIAB/TA",
            [&](const Capture& c, int k, int j){ return p2_s_bit(c,k,j,0x40); }, p2_z, p2_pre22_ta);
        P2LegSResult kr_b = grade_p2_legs(q_b, 0x80, "CPU CIAB/TB",
            [&](const Capture& c, int k, int j){ return p2_s_bit(c,k,j,0x80); }, p2_z, p2_pre22_tb);
        bool c_main = kr_a.all_ok && kr_b.all_ok;
        bool c_main_graded = kr_a.graded_any || kr_b.graded_any;
        bool c_legh = (!gc.o1_live || p2_legh_component(q_a, 0x40)) && (!gd.o1_live || p2_legh_component(q_b, 0x80));
        for (auto& n : kr_a.notes) checks.note(n);
        for (auto& n : kr_b.notes) checks.note(n);
        checks.note("P2-GREENCTL CIAB/TA: r1=" + hex2(pk(q_a,0)) + " h1=" + hex2(pk(q_a,9)) + " h3=" + hex2(pk(q_a,11)) +
                    " s0=" + hex2(pk(q_a,kP2IdxS0)) + "; CIAB/TB: r1=" + hex2(pk(q_b,0)) + " h1=" + hex2(pk(q_b,9)) +
                    " h3=" + hex2(pk(q_b,11)) + " s0=" + hex2(pk(q_b,kP2IdxS0)));
        emit(checks, cpre && n_live_ctl2 > 0, c_legr, "P2-LEGR-GREENCTL",
             "P2-LEGR-GREENCTL [p342]: the same leg-R comparator passes on a CPU-driven low level before any "
             "start (CIAB timer A and timer B, PBON=0)", cpre ? why_live : why_pre);
        emit(checks, cpre && c_main_graded, c_main, "P2-MAIN-GREENCTL",
             "P2-MAIN-GREENCTL [p342]: the same leg-S comparator passes on a CPU-driven rise at every start "
             "(CIAB timer A and timer B, PBON=0)", cpre ? "no start has Pre2.3 (level before 0)" : why_pre);
        emit(checks, cpre && n_live_ctl2 > 0, c_legh, "P2-LEGH-GREENCTL",
             "P2-LEGH-GREENCTL [p342]: the same leg-H comparator passes on a CPU-driven high at each start "
             "(CIAB timer A and timer B, PBON=0)", cpre ? why_live : why_pre);
        bool c_all = c_legr && c_main && c_legh;
        emit(checks, cpre && n_live_ctl2 > 0 && c_main_graded, c_all, "P2-TOGGLE-GREENCTL",
             "P2-TOGGLE-GREENCTL [p342]: the same legs R, S and H together pass on the CPU-driven level "
             "(CIAB timer A and timer B, PBON=0)", cpre ? "a leg has no graded population" : why_pre);
    }

    // ---- environment preconditions (EP1-EP3, F1 anchor), on one dedicated run
    // each (the per-run assertions at the end of main grade every capture) ---
    {
        NoRegister dedicated;
        std::string bin = dir + (sel("EP1") ? "/ep1_contrast.bin" : "/p1_ciaa_ta.bin");
        Capture e = run_capture(bin, kRunBudget, {kEp1CraA,kEp1CrbA,kEp1CraB,kEp1CrbB});
        bool ok = e.loaded && e.done.has_value() && pk_ok(e,0) && pk_ok(e,1) && pk_ok(e,2) && pk_ok(e,3) &&
                  pk(e,0)==0 && pk(e,1)==0 && pk(e,2)==0 && pk(e,3)==0;
        checks.expect(ok, "EP1 [p354]: the first reads of CRA and CRB on both CIAs, made before any write "
                          "to them, return $00");
    }
    {
        NoRegister dedicated;
        std::string bin = dir + (sel("EP2") ? "/ep2_contrast.bin" : sel("EP2-LOW") ? "/ep2_contrast_low.bin"
                                                                                      : "/p1_ciaa_ta.bin");
        Capture e = run_capture(bin, kRunBudget, {});
        auto s = mark(e.log, kMkStart);
        size_t stray_low = 0, stray_other = 0;
        bool ok = e.loaded && e.done.has_value() && s.has_value() &&
                  ep2_ok(e.log, *s, *e.done, nullptr, &stray_low, &stray_other);
        checks.note("EP2 dedicated run: reads below $1000 outside a refresh group=" + std::to_string(stray_low) +
                    ", other accesses outside the CPU ranges=" + std::to_string(stray_other) +
                    ", F1 anchor holds=" + std::string(measure_anchor(e.log).ok ? "yes" : "no"));
        checks.expect(ok,
            "EP2 [SPECIFICATION.md 1.2]: between the first and last marker, the Chip-memory port "
            "shows only CPU accesses and memory-refresh accesses");
    }
    {
        NoRegister dedicated;
        std::string bin = dir + (sel("EP3") ? "/p1_ciaa_ta_ep3.bin" : "/p1_ciaa_ta.bin");
        Capture e = run_capture(bin, kRunBudget, {});
        checks.expect(e.loaded && e.done.has_value(),
            "EP3 [SPECIFICATION.md 1.2]: the program reaches DONE and has a final marker");
    }
    {
        NoRegister dedicated;
        Capture e = run_capture(dir + (sel("F1-ANCHOR") ? "/p1_ciaa_ta_f1.bin" : "/p1_ciaa_ta.bin"), kRunBudget, {});
        Anchor anc = measure_anchor(e.log);
        checks.expect(anc.ok, "F1-ANCHOR [p194,p195,p23]: consecutive refresh line-marks are 454*T_CPU apart "
                              "(or alternately 456/454*T_CPU), measured from the run's own refresh reads");
        bool contrast_fails = anc.ok && !anchor_holds_at(anc, double(anc.t_cpu) * 1.25);
        checks.expect(contrast_fails, "F1-ANCHOR-CONTRAST [p194,p195,p23]: the same comparator, evaluated "
                                      "against 1.25x the measured T_CPU, fails");
    }

    // ---- P3 (toggle mode inverts on every underflow, and at no other time) -
    struct P3Result {
        bool loaded=false, pairing_ok=false, level_ok=false, pre31_ok=false, pre32_ok=false; int nT=0,nQ=0;
        int n_prb=0, n_high=0; uint16_t prb_min=0xFFFF, prb_max=0;   // the PRB samples as read
    };
    auto p3_predicate = [](const P3Result& r) { return r.pairing_ok && r.level_ok; };
    auto grade_p3 = [&](const std::string& bin, uint16_t n_mask, uint16_t icrmask) {
        P3Result s;
        Capture c = run_capture(bin, kRunBudget, {});
        SampleSeries prb = read_series(c.log, kResults+8, 600);
        SampleSeries icr = read_series(c.log, kResults+8+600*2, 600);
        s.loaded = c.loaded && c.done.has_value();
        if (!s.loaded) return s;
        std::vector<bool> bits(600,false), flags(600,false);
        for (int i=0;i<600;++i) {
            if (prb.value[i]) {
                ++s.n_prb;
                s.prb_min = std::min<uint16_t>(s.prb_min, *prb.value[i] & 0xFF);
                s.prb_max = std::max<uint16_t>(s.prb_max, *prb.value[i] & 0xFF);
                if (*prb.value[i] & n_mask) ++s.n_high;
            }
            bits[i] = prb.value[i] && (*prb.value[i] & n_mask);
            flags[i] = icr.value[i] && (*icr.value[i] & icrmask);
        }
        std::vector<int> T, Q;
        for (int i=0;i+1<600;++i) if (bits[i] != bits[i+1]) T.push_back(i);
        for (int i=0;i<600;++i) if (flags[i]) Q.push_back(i);
        s.nT = (int)T.size(); s.nQ = (int)Q.size();
        std::vector<bool> usedT(T.size(), false);
        bool pairing_ok = true;
        for (int q : Q) {
            if (q == 599) continue;   // the last sample's own flag has no later PRB read to pair with
            bool found = false;
            for (size_t k=0;k<T.size();++k) if (!usedT[k] && std::abs(T[k]-q) <= 1) { usedT[k]=true; found=true; break; }
            if (!found) { pairing_ok = false; break; }
        }
        for (size_t k=0;k<T.size();++k) if (!usedT[k]) pairing_ok = false;
        s.pairing_ok = pairing_ok;
        s.level_ok = !Q.empty() && bits[0];
        bool pre31 = true;
        for (size_t i=1;i<Q.size();++i) if (Q[i]-Q[i-1] < 3) { pre31=false; break; }
        s.pre31_ok = pre31;
        s.pre32_ok = Q.size() >= 16;
        return s;
    };
    {
        // Mutations of Pre3.1 and Pre3.2 replace CIAB/timer B's own run.
        std::string d_bin = dir + (sel("P3-PRE31") ? "/p3_ciab_tb_l2.bin"
                                   : sel("P3-PRE32") ? "/p3_ciab_tb_nostart.bin" : "/p3_ciab_tb.bin");
        P3Result r_a = grade_p3(dir+"/p3_ciaa_ta.bin", 0x40, kIcrTa);
        P3Result r_b = grade_p3(dir+"/p3_ciaa_tb.bin", 0x80, kIcrTb);
        P3Result r_c = grade_p3(dir+"/p3_ciab_ta.bin", 0x40, kIcrTa);
        P3Result r_d = grade_p3(d_bin, 0x80, kIcrTb);
        // Preconditions first: P3 is graded only where they hold.
        bool p3_pre_ok = r_a.pre31_ok && r_a.pre32_ok && r_b.pre31_ok && r_b.pre32_ok &&
                         r_c.pre31_ok && r_c.pre32_ok && r_d.pre31_ok && r_d.pre32_ok;
        checks.expect(p3_pre_ok, "P3-PRE [SPECIFICATION.md P3]: underflows are separated by at least 3 "
                      "iterations, and there are at least 16 of them, for all four instances");
        checks.note("P3 CIAA/TA T=" + std::to_string(r_a.nT) + " Q=" + std::to_string(r_a.nQ) +
                    "; CIAA/TB T=" + std::to_string(r_b.nT) + " Q=" + std::to_string(r_b.nQ) +
                    "; CIAB/TA T=" + std::to_string(r_c.nT) + " Q=" + std::to_string(r_c.nQ) +
                    "; CIAB/TB T=" + std::to_string(r_d.nT) + " Q=" + std::to_string(r_d.nQ));
        // Gated by Pre1.4/O1 (the same fact P1/P2 reuse): CIA-A's own PRB
        // read never shows a driven level, so it is not graded and its own
        // read-back is recorded instead (SPECIFICATION.md P1 Pre1.4).
        const int n3_ta = int(ga.o1_live) + int(gc.o1_live), n3_tb = int(gb.o1_live) + int(gd.o1_live);
        bool p3_ta_ok = (!ga.o1_live || p3_predicate(r_a)) && (!gc.o1_live || p3_predicate(r_c));
        bool p3_tb_ok = (!gb.o1_live || p3_predicate(r_b)) && (!gd.o1_live || p3_predicate(r_d));
        {
            const P3Result* rs[4] = {&r_a, &r_b, &r_c, &r_d};
            const bool live[4] = {ga.o1_live, gb.o1_live, gc.o1_live, gd.o1_live};
            for (int i = 0; i < 4; ++i)
                pv.p3[i] = (!live[i] || !p3_pre_ok) ? V::NOTGRADED : vof(true, p3_predicate(*rs[i]));
        }
        emit(checks, p3_pre_ok && n3_ta + n3_tb > 0, p3_ta_ok && p3_tb_ok, "P3",
             "P3 [p342,p348,p349,p350]: every underflow has its transition and every "
             "transition has its underflow (paired within one iteration), and the level before "
             "the first flag is 1, for the instances whose read-back is live (Pre1.4/O1)",
             p3_pre_ok ? "no instance has a live read-back (Pre1.4/O1)" : "P3-PRE failed");
        auto p3_stat = [](const P3Result& r) {
            if (r.n_prb == 0) return std::string("no PRB sample stored");
            return "PRB " + hex2(r.prb_min) + (r.prb_min == r.prb_max ? "" : ".." + hex2(r.prb_max)) + " in " +
                   std::to_string(r.n_prb) + " samples, bit n set in " + std::to_string(r.n_high);
        };
        checks.record("P3 [p342], CIA-A: not graded (read-back not live, Pre1.4/O1) -- measured CIAA/TA: " +
                      p3_stat(r_a) + ", Q=" + std::to_string(r_a.nQ) + "; CIAA/TB: " + p3_stat(r_b) +
                      ", Q=" + std::to_string(r_b.nQ));

        // P3-GREENCTL: a CPU-driven toggle (PBON=0, DDRB bit 7 an output, the
        // CPU inverts PRB bit 7 after every flagged ICR read) through the whole
        // predicate P3's line grades: the pairing and the level before the
        // first flag.
        {
            P3Result r_cpu = grade_p3(dir + (sel("P3-GREENCTL") ? "/p3_ciab_tb_cputoggle_low.bin"
                                                                 : "/p3_ciab_tb_cputoggle.bin"), 0x80, kIcrTb);
            checks.expect(r_cpu.loaded && p3_predicate(r_cpu), "P3-GREENCTL [SPECIFICATION.md P3]: the CPU "
                          "inverts PRB bit 7 once per flagged ICR read (PBON=0, DDRB bit 7 an output) -- "
                          "the pairing and the level before the first flag both hold through the same "
                          "predicate P3's own line grades, once the last sample's own unpaired flag is "
                          "excluded (CIAB/timer B)");
        }
    }

    // ---- P4 (pulse mode: low except one phi2 cycle at every underflow) ----
    {
        const std::vector<int> p4_ls = {20,21,22,23,24,25};
        auto p4_bins = [&](const std::string& prefix, const std::string& tag, const std::vector<int>& ls) {
            std::vector<std::string> v;
            for (int L : ls) v.push_back(dir+"/"+prefix+"_"+tag+"_l"+std::to_string(L)+".bin");
            return v;
        };
        P4InstanceSummary s_a = grade_p4_instance(checks, "P4", "ciaa_ta", p4_bins("p4","ciaa_ta",p4_ls), p4_ls, 0x40, kIcrTa);
        P4InstanceSummary s_b = grade_p4_instance(checks, "P4", "ciaa_tb", p4_bins("p4","ciaa_tb",p4_ls), p4_ls, 0x80, kIcrTb);
        P4InstanceSummary s_c = grade_p4_instance(checks, "P4", "ciab_ta", p4_bins("p4","ciab_ta",p4_ls), p4_ls, 0x40, kIcrTa);
        auto d_bins = p4_bins("p4","ciab_tb",p4_ls);
        if (sel("P4-PRE"))   // three of the four usable latches of CIAB/timer B are not started
            for (size_t i = 0; i < p4_ls.size(); ++i)
                if (p4_ls[i] == 21 || p4_ls[i] == 22 || p4_ls[i] == 24)
                    d_bins[i] = dir + "/p4_ciab_tb_l" + std::to_string(p4_ls[i]) + "_nostart.bin";
        P4InstanceSummary s_d = grade_p4_instance(checks, "P4", "ciab_tb", d_bins, p4_ls, 0x80, kIcrTb);
        // P4's own pulse-shape clause is gated by Pre1.4/O1 (CIA-A's PRB
        // read-back is not live); P4-PRE (the usable-latch count, F3-based)
        // does not depend on port B and is not gated.
        bool p4_pre_ok = s_a.usable>=2 && s_b.usable>=2 && s_c.usable>=2 && s_d.usable>=2;
        checks.expect(p4_pre_ok, "P4-PRE [SPECIFICATION.md P4]: at least 2 of the 6 latch values are "
                      "usable (coverage and a unique period), for all four instances");
        const int n4_ta = int(ga.o1_live) + int(gc.o1_live), n4_tb = int(gb.o1_live) + int(gd.o1_live);
        bool p4_ta_ok = (!ga.o1_live || s_a.ok) && (!gc.o1_live || s_c.ok);
        bool p4_tb_ok = (!gb.o1_live || s_b.ok) && (!gd.o1_live || s_d.ok);
        {
            const P4InstanceSummary* ss[4] = {&s_a, &s_b, &s_c, &s_d};
            const bool live[4] = {ga.o1_live, gb.o1_live, gc.o1_live, gd.o1_live};
            for (int i = 0; i < 4; ++i)
                pv.p4[i] = (!live[i] || !p4_pre_ok) ? V::NOTGRADED : vof(true, ss[i]->ok);
        }
        // SPECIFICATION.md P4: a run whose own Pre4.2/Pre4.3 fails "is not
        // graded", reported rather than printed FAIL.
        emit(checks, p4_pre_ok && n4_ta + n4_tb > 0, p4_ta_ok && p4_tb_ok, "P4",
             "P4 [p341,p342]: P(L) is L or L+1 (constant offset), with one residue "
             "high (every sample in it, no other residue ever high, >=3 high samples), for "
             "the instances whose read-back is live (Pre1.4/O1), on the usable latches "
             "(Pre4.2/Pre4.3)",
             p4_pre_ok ? "no instance has a live read-back (Pre1.4/O1)"
                       : "P4-PRE (at least 2 usable latches) failed on at least one instance");
        auto p4_stat = [](const P4InstanceSummary& s) {
            if (s.samples == 0) return std::string("no PRB sample stored");
            return "PRB " + hex2(s.vmin) + (s.vmin == s.vmax ? "" : ".." + hex2(s.vmax)) + ", bit n set in " +
                   std::to_string(s.high) + " of " + std::to_string(s.samples) + " samples";
        };
        checks.record("P4 [p341,p342], CIA-A: not graded (read-back not live, Pre1.4/O1) -- measured over the "
                      "six latch runs of CIAA/TA: " + p4_stat(s_a) + "; CIAA/TB: " + p4_stat(s_b));
        checks.note("P4 [p341,p342], graded instances: measured over the six latch runs of CIAB/TA: " + p4_stat(s_c) +
                    "; CIAB/TB: " + p4_stat(s_d));
        checks.note("P4 usable latches: CIAA/TA=" + std::to_string(s_a.usable) + " CIAA/TB=" + std::to_string(s_b.usable) +
                    " CIAB/TA=" + std::to_string(s_c.usable) + " CIAB/TB=" + std::to_string(s_d.usable));

        // P4-GREENCTL: the unchanged P4 comparator (grade_p4_instance) on a live
        // pulse source. The CPU, not the timer, drives DDRB bit n's data level
        // (PBON=0): each iteration reads the running timer's own count and
        // writes a level that is high only when the count equals one value, so
        // the pulse recurs at one fixed phase of the timer's own period. The
        // latch values are those with P(L) at least twice the loop's 16 E
        // spacing (F3).
        {
            const std::vector<int> ctl_ls = {40,41,42,43,44,45};
            auto ctl_tb = p4_bins("p4c","ciab_tb",ctl_ls);
            if (sel("P4-GREENCTL")) ctl_tb[0] = dir + "/p4c_ciab_tb_l40_two.bin";
            P4InstanceSummary g_a = grade_p4_instance(checks, "P4-GREENCTL", "ciab_ta", p4_bins("p4c","ciab_ta",ctl_ls), ctl_ls, 0x40, kIcrTa);
            P4InstanceSummary g_b = grade_p4_instance(checks, "P4-GREENCTL", "ciab_tb", ctl_tb, ctl_ls, 0x80, kIcrTb);
            checks.note("P4-GREENCTL usable latches: CIAB/TA=" + std::to_string(g_a.usable) + " CIAB/TB=" + std::to_string(g_b.usable));
            checks.expect(g_a.ok && g_b.ok, "P4-GREENCTL [SPECIFICATION.md P4]: the same P4 comparator passes "
                          "on a CPU-driven pulse at one fixed phase of the timer's period (PBON=0, DDRB bit n "
                          "an output, latches 40-45; the timer-A and timer-B instances of CIAB)");
        }
    }

    // ---- P9 (the latch is the divider of the input clock) -----------------
    {
        // Leg F: reuses P3's own mechanism (toggle mode, R-SAMPLE(600;PRB,ICR)),
        // CIAB/timer B, for L in {40,41,64,100} (SPECIFICATION.md P9 leg F).
        auto period_of = [&](const std::string& bin, uint16_t icrmask) -> PeriodResult {
            Capture c = run_capture(bin, kRunBudget, {});
            SampleSeries icr = read_series(c.log, kResults+8+600*2, 600);
            Anchor anc = measure_anchor(c.log);
            if (!c.loaded || !c.done.has_value() || !anc.ok) return PeriodResult{};
            return measure_period(icr, icrmask, anc.t_cpu);
        };
        // P10-NOPERIOD-L40: the L=40 run with its timer not started shows no flags, so no period is determined.
        const std::string l40bin = dir + (sel("P10-NOPERIOD-L40") ? "/p9_legf_ciab_tb_l40_nostart.bin"
                                                                         : "/p9_legf_ciab_tb_l40.bin");
        PeriodResult p40 = period_of(l40bin, kIcrTb);
        PeriodResult p41 = period_of(dir + (sel("P9-LEGF") ? "/p9_legf_ciab_tb_l41_m.bin"
                                                             : "/p9_legf_ciab_tb_l41.bin"), kIcrTb);
        PeriodResult p64 = period_of(dir+"/p9_legf_ciab_tb_l64.bin", kIcrTb);
        PeriodResult p100 = period_of(dir+"/p9_legf_ciab_tb_l100.bin", kIcrTb);
        // P9-LEGF-PRE: all four runs completed with F1's anchor and each period is uniquely determined (F3: at
        // least 16 flags, iteration spacing at most P/2); P9-LEGF and P9-LEGF-TRANSITIONS are graded only where
        // it holds (the transitions' own L=40 run is the same program as the first of the four).
        const bool legf_pre = p40.ok && p41.ok && p64.ok && p100.ok;
        checks.expect(legf_pre, "P9-LEGF-PRE [SPECIFICATION.md P9, F3]: each of the L=40, 41, 64 and 100 runs of "
                      "CIAB/timer B completed with F1's anchor and its period P(L) is uniquely determined (at least "
                      "16 flags, iteration spacing at most P/2)");
        int64_t d40=p40.P-40, d41=p41.P-41, d64=p64.P-64, d100=p100.P-100;
        bool delta_ok = legf_pre && d40==d41 && d41==d64 && d64==d100 && (d40==0||d40==1);
        pv.p9_legf = vof(legf_pre, delta_ok);
        // opus:F-timers.c6/sol:F-timers.c8 bundle more than one proposition
        // (P4, leg F, leg W and P8.65 mode 10); P10 carries them (its own
        // row), not this component line (SPECIFICATION.md P10: "None
        // graded whole here").
        emit(checks, legf_pre, delta_ok, "P9-LEGF", "P9-LEGF [p341]: delta = P(L)-L is the same for L=40,41,64,100 "
             "and is 0 or 1 (CIAB/timer B)", "P9-LEGF-PRE failed");
        checks.note("P9-LEGF P(40)=" + std::to_string(p40.P) + " P(41)=" + std::to_string(p41.P) +
                    " P(64)=" + std::to_string(p64.P) + " P(100)=" + std::to_string(p100.P));

        // "Consecutive PRB transitions are P(L) +- s E periods apart"
        // (SPECIFICATION.md P9), s the PRB sampling spacing (leg F's own
        // R-SAMPLE2 iteration time in E periods, measured in the run).
        struct TransResult { bool run_ok = false, ok = false; size_t n = 0; double s_e = 0; };
        auto transitions_of = [&](const std::string& bin, const PeriodResult& pr) {
            TransResult r;
            Capture c = run_capture(bin, kRunBudget, {});
            SampleSeries prb = read_series(c.log, kResults+8, 600);
            Anchor anc = measure_anchor(c.log);
            r.run_ok = c.loaded && c.done.has_value() && anc.ok;
            if (!r.run_ok || !pr.ok) return r;
            uint64_t t0 = 0; bool have = false; std::vector<int64_t> e_all; std::vector<bool> bit_all;
            double sum = 0; int n = 0; std::optional<uint64_t> prev;
            for (int i=0;i<600;++i) {
                if (!prb.cycle[i] || !prb.value[i]) continue;
                if (!have) { t0 = *prb.cycle[i]; have = true; }
                e_all.push_back((int64_t)e_index(*prb.cycle[i], t0, anc.t_cpu));
                bit_all.push_back((*prb.value[i] & 0x80) != 0);
                if (prev) { sum += double(*prb.cycle[i] - *prev) / (10.0*double(anc.t_cpu)); ++n; }
                prev = prb.cycle[i];
            }
            r.s_e = n ? sum / n : 0;
            std::vector<int64_t> trans;
            for (size_t i=0;i+1<bit_all.size();++i) if (bit_all[i] != bit_all[i+1]) trans.push_back(e_all[i+1]);
            r.n = trans.size();
            if (trans.size() < 2) return r;
            r.ok = true;
            for (size_t i=1;i<trans.size();++i)
                if (std::fabs(double(trans[i]-trans[i-1]) - double(pr.P)) > r.s_e + 0.5) { r.ok = false; break; }
            return r;
        };
        auto trans_text = [](const TransResult& r, const PeriodResult& pr) {
            char b[32];
            std::snprintf(b, sizeof b, "%.2f", r.s_e);
            return std::to_string(r.n) + " PRB transitions in the 600 samples, P(40)=" + std::to_string(pr.P) +
                   ", s=" + b + "E";
        };
        TransResult tr = transitions_of(l40bin, p40);
        pv.p9_legf_trans = vof(legf_pre && tr.run_ok, tr.ok);
        emit(checks, legf_pre && tr.run_ok, tr.ok, "P9-LEGF-TRANSITIONS", "P9-LEGF-TRANSITIONS [p341,p342]: "
             "consecutive PRB transitions are P(L) +-s E periods apart, s the sampling spacing (CIAB/timer B, L=40)",
             legf_pre ? "the L=40 transition run did not complete with F1's anchor" : "P9-LEGF-PRE failed");
        checks.note("P9-LEGF-TRANSITIONS CIAB/TB L=40: " + trans_text(tr, p40));

        // P9-LEGF-TRANSITIONS-GREENCTL: the same period measurement and the same
        // transition-spacing comparator on a live transition train. The CPU
        // (PBON=0, DDRB bit 7 an output) inverts PRB bit 7 after every flagged
        // ICR read, so its transitions follow the underflows, L=40.
        {
            std::string cbin = dir + (sel("P9-LEGF-TRANSITIONS-GREENCTL") ? "/p9_legf_ciab_tb_cputoggle_skip.bin"
                                                                           : "/p9_legf_ciab_tb_cputoggle.bin");
            PeriodResult pc = period_of(cbin, kIcrTb);
            TransResult ctl = transitions_of(cbin, pc);
            checks.note("P9-LEGF-TRANSITIONS-GREENCTL P(40)=" + std::to_string(pc.P) +
                        (pc.ok ? " (uniquely determined)" : " (not uniquely determined)") + ", " + trans_text(ctl, pc));
            emit(checks, pc.ok && ctl.run_ok, ctl.ok, "P9-LEGF-TRANSITIONS-GREENCTL", "P9-LEGF-TRANSITIONS-GREENCTL "
                 "[p341,p342]: the same comparator passes on "
                 "a CPU-driven transition train that follows the underflows (CIAB/timer B, L=40, "
                 "PBON=0)", "the CPU-driven run's period is not uniquely determined or the run did not complete");
        }

        // Leg W: P2 leg S's own mechanics, CIAB/timer B, for L in {40,100}.
        // Each run also stores the level before its start (Pre2.3) and, after
        // the sampler, the stop-poll count and the ICR value (Pre2.2).
        constexpr uint32_t kP9wPoll = kResults + 8 + 300*2;
        constexpr uint32_t kP9wI    = kP9wPoll + 2;
        constexpr uint32_t kP9wS0   = kP9wPoll + 4;
        struct WidthRun {
            bool loaded=false;
            std::optional<double> w;
            double s_e=0;
            bool underflow_ok=false;   // Pre2.2: the one-shot had ended when the sampler closed, TB flag read set
            bool level_ok=false;       // Pre2.3: the level before the start is 0
            uint16_t poll=0, icr=0, s0=0;
        };
        auto width_of = [&](const std::string& bin) -> WidthRun {
            WidthRun r;
            Capture c = run_capture(bin, kRunBudget, {kP9wPoll, kP9wI, kP9wS0});
            Anchor anc = measure_anchor(c.log);
            auto mpost = mark(c.log, 0x0C1004);
            r.loaded = c.loaded && c.done.has_value();
            if (pk_ok(c,0)) r.poll = pk(c,0);
            if (pk_ok(c,1)) r.icr = pk(c,1);
            if (pk_ok(c,2)) r.s0 = pk(c,2);
            r.underflow_ok = r.loaded && pk_ok(c,0) && pk_ok(c,1) && r.poll == 1 && (r.icr & kIcrTb);
            r.level_ok = r.loaded && pk_ok(c,2) && !(r.s0 & 0x80);
            if (!r.loaded || !anc.ok || !mpost) return r;
            // NSAMP differs per L (200 for 40, 300 for 100); read a generous 300-slot window --
            // slots beyond the program's own NSAMP simply have no value and are skipped.
            SampleSeries s = read_series(c.log, kResults+8, 300);
            bool prev_high = false; bool have_prev = false;
            double sum = 0; int n = 0; std::optional<uint64_t> prev_cyc;
            for (size_t i=0;i<s.size();++i) {
                if (!s.cycle[i] || !s.value[i]) continue;
                if (prev_cyc) { sum += double(*s.cycle[i] - *prev_cyc) / (10.0*double(anc.t_cpu)); ++n; }
                prev_cyc = s.cycle[i];
                bool high = (*s.value[i] & 0x80) != 0;
                if (have_prev && prev_high && !high && !r.w) r.w = e_index(*s.cycle[i], *mpost, anc.t_cpu);
                prev_high = high; have_prev = true;
            }
            r.s_e = n ? sum / n : 0;
            return r;
        };
        // The width comparator, shared by the leg-W runs and P9-LEGW-GREENCTL.
        auto width_ok = [](const WidthRun& a, const WidthRun& b) {
            double s_meas = std::max(a.s_e, b.s_e);
            return a.w.has_value() && b.w.has_value() && std::fabs((*b.w - *a.w) - 60.0) <= s_meas + 0.5;
        };
        auto width_str = [](const WidthRun& r) { return r.w ? std::to_string(*r.w) : std::string("?"); };

        std::string w40bin = dir + (sel("P9-LEGW-PRE22") ? "/p9_legw_l40_nouf.bin"
                                    : sel("P9-LEGW-PRE23") ? "/p9_legw_l40_lvl1.bin"
                                    : "/p9_legw_l40.bin");
        WidthRun w40 = width_of(w40bin);
        WidthRun w100 = width_of(dir+"/p9_legw_l100.bin");
        bool w_pre22 = w40.underflow_ok && w100.underflow_ok;
        bool w_pre23 = w40.level_ok && w100.level_ok;
        checks.expect(w_pre22, "P9-LEGW-PRE2.2 [p342]: the one-shot has ended when the sampler "
                      "closes (CR bit 0 read 0 at the first poll) and the ICR read shows the TB flag "
                      "(CIAB/timer B, L=40 and L=100)");
        checks.expect(w_pre23, "P9-LEGW-PRE2.3 [p342]: the level before the start is 0 "
                      "(CIAB/timer B, L=40 and L=100)");
        checks.note("P9-LEGW polls=" + std::to_string(w40.poll) + "/" + std::to_string(w100.poll) +
                    " icr=" + hex2(w40.icr) + "/" + hex2(w100.icr) +
                    " s0=" + hex2(w40.s0) + "/" + hex2(w100.s0) + " (L=40 / L=100)");
        double s_meas = std::max(w40.s_e, w100.s_e);
        bool w_graded = w_pre22 && w_pre23;
        bool w_ok = width_ok(w40, w100);
        pv.p9_legw = w_graded ? vof(true, w_ok) : V::NOTGRADED;
        if (w_graded) {
            checks.expect(w_ok, "P9-LEGW [p341]: W(100)-W(40) = 60+-s, s the run's own measured PRB sampling "
                          "spacing, CIAB/timer B");
        } else {
            checks.record("P9-LEGW [p341]: not graded -- P9-LEGW-PRE2.2 or P9-LEGW-PRE2.3 failed");
        }
        checks.note("P9-LEGW W(40)=" + width_str(w40) + " W(100)=" + width_str(w100) +
                    " s=" + std::to_string(s_meas) + "E");

        // P9-LEGW-GREENCTL: the same width comparator on a live width source.
        // The CPU (PBON=0) sets PRB bit 7 right after the start marker and
        // clears it after the ICR read that shows the underflow; the same
        // underflow and level-before preconditions are asserted on these runs.
        {
            std::string c40bin = dir + (sel("P9-LEGW-GREENCTL") ? "/p9_legw_ctl_l20.bin"
                                        : sel("P9-LEGW-GREENCTL-PRE22") ? "/p9_legw_ctl_l40_nouf.bin"
                                        : sel("P9-LEGW-GREENCTL-PRE23") ? "/p9_legw_ctl_l40_lvl1.bin"
                                        : "/p9_legw_ctl_l40.bin");
            WidthRun c40 = width_of(c40bin);
            WidthRun c100 = width_of(dir+"/p9_legw_ctl_l100.bin");
            checks.expect(c40.underflow_ok && c100.underflow_ok, "P9-LEGW-GREENCTL-PRE2.2 [p342]: the one-shot has "
                          "ended when the sampler closes and the sampler read the TB flag (CPU-driven, "
                          "CIAB/timer B, L=40 and L=100)");
            checks.expect(c40.level_ok && c100.level_ok, "P9-LEGW-GREENCTL-PRE2.3 [p342]: the level before the "
                          "start is 0 (CPU-driven, CIAB/timer B, L=40 and L=100)");
            bool c_pre = c40.underflow_ok && c100.underflow_ok && c40.level_ok && c100.level_ok;
            checks.note("P9-LEGW-GREENCTL W(40)=" + width_str(c40) + " W(100)=" + width_str(c100) +
                        " s=" + std::to_string(std::max(c40.s_e, c100.s_e)) + "E");
            if (c_pre) {
                checks.expect(width_ok(c40, c100), "P9-LEGW-GREENCTL [p341]: the same comparator passes on a "
                              "CPU-driven width that follows the latch: W(100)-W(40) = 60+-s (CIAB/timer B, "
                              "PBON=0)");
            } else {
                checks.record("P9-LEGW-GREENCTL [p341]: not graded -- P9-LEGW-GREENCTL-PRE2.2 or "
                              "P9-LEGW-GREENCTL-PRE2.3 failed");
            }
        }

        // Contrasting cases (SPECIFICATION.md P9): leg F's is the L=41 run made
        // with a latch of 42 (NMH_MUTATE=P9-LEGF); leg W's is the CPU-driven L=40
        // run made with a latch of 20 (NMH_MUTATE=P9-LEGW-GREENCTL).
        checks.record("P9-CONTRAST [SPECIFICATION.md P9]: leg F's contrast is the L=41 run made with "
                      "a latch of 42 (P9-LEGF), leg W's the CPU-driven L=40 run made with a latch of 20 "
                      "(P9-LEGW-GREENCTL); both are selected through NMH_MUTATE");
    }

    // ========================================================================
    // P7 (CRA bit layout) / P8 (CRB bit layout). None of P7.0-P7.7/P8.0-P8.7's
    // own assertions carries a claim id (SPECIFICATION.md P7/P8: "None is
    // graded whole by one sub-proposition"); the ids sit on P10's own rows.
    // ========================================================================

    // ---- P7.0 / P8.0 (bit 0, START) ---------------------------------------
    // One line per setting: START=0 stops the timer (u1=u2=$8000), START=1 starts it (u3<u2). The
    // precondition is printed first and both lines are graded only when it holds.
    {
        struct SI { const char* tag; std::string bin; uint16_t icrmask; };
        std::vector<SI> insts = {
            {"CIAA/CRA", dir + "/p7_ciaa_start.bin", kIcrTa},
            {"CIAB/CRA", dir + (sel("P70-PRE") ? "/p7_ciab_start_stpre.bin"
                               : sel("P7.0-STOP") ? "/p7_ciab_start_contrast.bin"
                               : sel("P7.0-START") ? "/p7_ciab_start_nostart.bin"
                               : "/p7_ciab_start.bin"), kIcrTa},
        };
        std::vector<SI> insts8 = {
            {"CIAA/CRB", dir + "/p8_ciaa_start.bin", kIcrTb},
            {"CIAB/CRB", dir + (sel("P80-PRE") ? "/p8_ciab_start_stpre.bin"
                               : sel("P8.0-STOP") ? "/p8_ciab_start_contrast.bin"
                               : sel("P8.0-START") ? "/p8_ciab_start_nostart.bin"
                               : "/p8_ciab_start.bin"), kIcrTb},
        };
        struct StartV { V stop = V::NOTGRADED, start = V::NOTGRADED; };
        auto grade_start = [&](std::vector<SI>& list, const char* propid, const char* lbl, const char* cite) {
            bool stop_ok = true, start_ok = true, pre_ok = true;
            for (auto& s : list) {
                Capture c = run_capture(s.bin, kRunBudget, {kResults+8, kResults+10, kResults+12, kResults+14});
                bool run = c.loaded && c.done.has_value();
                stop_ok = stop_ok && run && pk_ok(c,0) && pk_ok(c,1) && pk(c,0) == 0x8000 && pk(c,1) == 0x8000;
                start_ok = start_ok && run && pk_ok(c,1) && pk_ok(c,2) && pk(c,2) < pk(c,1);
                pre_ok = pre_ok && run && pk_ok(c,3) && !(pk(c,3) & s.icrmask);
                checks.note(std::string(propid) + " " + s.tag + ": u1=" + hex4(pk(c,0)) + " u2=" + hex4(pk(c,1)) +
                            " u3=" + hex4(pk(c,2)));
            }
            checks.expect(pre_ok, std::string(propid) + "-PRE [p342]: no underflow happens before the stop "
                          "read, on both CIAs");
            const std::string why = std::string(propid) + "-PRE failed";
            emit(checks, pre_ok, stop_ok, std::string(lbl) + "-STOP", std::string(lbl) + "-STOP [" + cite + "]: "
                 "u1=u2=$8000 (START=0 stops the timer: the counter holds, and the latch write loaded it), on both CIAs", why);
            emit(checks, pre_ok, start_ok, std::string(lbl) + "-START", std::string(lbl) + "-START [" + cite + "]: "
                 "u3<u2 (START=1 starts the timer: started, then stopped, it has decremented), on both CIAs", why);
            StartV v;
            v.stop = vof(pre_ok, stop_ok);
            v.start = vof(pre_ok, start_ok);
            return v;
        };
        StartV s7 = grade_start(insts, "P70", "P7.0", "p348");
        pv.p70_stop = s7.stop; pv.p70_start = s7.start;
        StartV s8 = grade_start(insts8, "P80", "P8.0", "p349");
        pv.p80_stop = s8.stop; pv.p80_start = s8.start;
    }

    // ---- P7.3 / P8.3 (bit 3, RUNMODE) --------------------------------------
    // One line per setting: RUNMODE=1 (one-shot, leg O) and RUNMODE=0 (continuous, leg C). The precondition
    // (all 400 ICR stores of each leg, consecutive stores at most 20 E periods apart) is printed first and
    // both lines and the contrast are graded only when it holds.
    {
        auto max_gap_cycles = [](const SampleSeries& s) {
            uint64_t prev = 0; bool have = false; uint64_t mx = 0;
            for (auto& c : s.cycle) { if (!c) continue; if (have) mx = std::max(mx, *c - prev); prev = *c; have = true; }
            return mx;
        };
        auto count_flags = [](const SampleSeries& s, uint16_t mask) {
            int n = 0; for (auto& v : s.value) if (v && (*v & mask)) ++n; return n;
        };
        // one instance's own pair of legs (O: one-shot $09, C: continuous $01),
        // same binary, differing only in the E7 parameter poked before boot.
        struct RunLegs { bool oneshot_ok=false, cont_ok=false, contrast_ok=false, pre_ok=false; int flagsO=0, flagsC=0; size_t popO=0, popC=0; };
        // The padded contrast (P73-PRE/P83-PRE) needs a budget beyond kRunBudget to reach DONE
        // (its loop runs about 4.8-5.3M clk_sys against kRunBudget's 4.0M).
        auto grade_run_instance = [&](const std::string& bin, uint16_t icrmask, uint64_t budget,
                                      uint8_t start_o, uint8_t start_c) {
            RunLegs r;
            Capture co = run_capture_param(bin, 0x0C2000, start_o, budget, {kResults+808, kResults+810});
            Capture cc = run_capture_param(bin, 0x0C2000, start_c, budget, {kResults+808, kResults+810});
            SampleSeries so = read_series(co.log, kResults+8, 400);
            SampleSeries sc = read_series(cc.log, kResults+8, 400);
            r.flagsO = count_flags(so, icrmask);
            r.flagsC = count_flags(sc, icrmask);
            r.popO = series_pop(so);
            r.popC = series_pop(sc);
            bool cO = co.loaded && co.done.has_value() && pk_ok(co,0) && (pk(co,0)&0x01)==0 && pk_ok(co,1) && pk(co,1)==40;
            bool cC = cc.loaded && cc.done.has_value() && pk_ok(cc,0) && (pk(cc,0)&0x01)!=0;
            r.oneshot_ok = (r.flagsO == 1) && cO;
            r.cont_ok = (r.flagsC > 1) && cC;
            r.contrast_ok = !(r.flagsC == 1) && !(r.flagsO > 1);
            Anchor ancO = measure_anchor(co.log), ancC = measure_anchor(cc.log);
            r.pre_ok = ancO.ok && ancC.ok && r.popO == 400 && r.popC == 400 &&
                       double(max_gap_cycles(so)) <= 20.0*10.0*double(ancO.t_cpu) + 1 &&
                       double(max_gap_cycles(sc)) <= 20.0*10.0*double(ancC.t_cpu) + 1;
            return r;
        };
        struct RunV { V oneshot = V::NOTGRADED, cont = V::NOTGRADED; };
        // lbl is "P7.3" or "P8.3", flag the TA or TB flag; a and b are the two instances' own results.
        auto emit_run = [&](const char* lbl, const char* flag, const char* cite, const RunLegs& a, const RunLegs& b) {
            const bool pre = a.pre_ok && b.pre_ok;
            const std::string L = lbl;
            checks.expect(pre, L + "-PRE [SPECIFICATION.md " + L + "]: all 400 ICR stores of each leg are present, and "
                          "consecutive ICR stores are at most 20 E periods apart, on both CIAs");
            const std::string why = L + "-PRE failed";
            emit(checks, pre, a.oneshot_ok && b.oneshot_ok, L + "-ONESHOT", L + "-ONESHOT [" + cite + "]: leg O (one-shot, "
                 "RUNMODE=1) shows exactly one " + flag + " flag and START clears itself, on both CIAs", why);
            emit(checks, pre, a.cont_ok && b.cont_ok, L + "-CONT", L + "-CONT [" + cite + "]: leg C (continuous, RUNMODE=0) "
                 "shows more than one " + flag + " flag and START stays set, on both CIAs", why);
            emit(checks, pre, a.contrast_ok && b.contrast_ok, L + "-CONTRAST", L + "-CONTRAST [" + cite + "]: leg O's own "
                 "\"exactly one\" comparator fails on leg C's data, and leg C's own \"more than one\" comparator "
                 "fails on leg O's data, on both CIAs", why);
            checks.note(L + " CIAA flags O=" + std::to_string(a.flagsO) + " C=" + std::to_string(a.flagsC) +
                        " stores O=" + std::to_string(a.popO) + " C=" + std::to_string(a.popC) +
                        "; CIAB flags O=" + std::to_string(b.flagsO) + " C=" + std::to_string(b.flagsC) +
                        " stores O=" + std::to_string(b.popO) + " C=" + std::to_string(b.popC));
            RunV v;
            v.oneshot = vof(pre, a.oneshot_ok && b.oneshot_ok);
            v.cont = vof(pre, a.cont_ok && b.cont_ok);
            return v;
        };
        // P7.3's stimulus changes: leg O started with $08 (P7.3-ONESHOT), leg C started with $09 (P7.3-CONT);
        // the padded sampler (P73-PRE) and the 399-iteration sampler (P73-PRE-POP) on CIAB. P8.3's likewise.
        RunLegs r7a = grade_run_instance(dir + "/p7_ciaa_run.bin", kIcrTa, kRunBudget,
                                         sel("P7.3-ONESHOT") ? 0x08 : 0x09, sel("P7.3-CONT") ? 0x09 : 0x01);
        RunLegs r7b = grade_run_instance(dir + (sel("P73-PRE") ? "/p7_ciab_run_pad.bin"
                                                : sel("P73-PRE-POP") ? "/p7_ciab_run_short.bin"
                                                : "/p7_ciab_run.bin"),
                                         kIcrTa, sel("P73-PRE") ? 10'000'000 : kRunBudget, 0x09, 0x01);
        RunV v7 = emit_run("P7.3", "TA", "p342,p348", r7a, r7b);
        pv.p73_oneshot = v7.oneshot; pv.p73_cont = v7.cont;

        RunLegs r8a = grade_run_instance(dir + "/p8_ciaa_run.bin", kIcrTb, kRunBudget,
                                         sel("P8.3-ONESHOT") ? 0x08 : 0x09, sel("P8.3-CONT") ? 0x09 : 0x01);
        RunLegs r8b = grade_run_instance(dir + (sel("P83-PRE") ? "/p8_ciab_run_pad.bin"
                                                : sel("P83-PRE-POP") ? "/p8_ciab_run_short.bin"
                                                : "/p8_ciab_run.bin"),
                                         kIcrTb, sel("P83-PRE") ? 10'000'000 : kRunBudget, 0x09, 0x01);
        RunV v8 = emit_run("P8.3", "TB", "p342,p349", r8a, r8b);
        pv.p83_oneshot = v8.oneshot; pv.p83_cont = v8.cont;
    }

    // ---- P7.4 / P8.4 (bit 4, LOAD) -----------------------------------------
    {
        struct LoadResult { bool loaded=false; uint16_t v1=0,v2=0,c=0,v3=0; uint64_t mkA=0,mkB=0,mkC=0,mkD=0,mkRun=0; uint64_t t_cpu=0; bool anc_ok=false; };
        auto run_load = [&](const std::string& bin) {
            LoadResult r;
            Capture c = run_capture(bin, kRunBudget, {kResults+8,kResults+10,kResults+12,kResults+14});
            r.loaded = c.loaded && c.done.has_value();
            if (!r.loaded) return r;
            r.v1 = pk(c,0); r.v2 = pk(c,1); r.c = pk(c,2); r.v3 = pk(c,3);
            auto a = mark(c.log, 0x0C1002), b = mark(c.log, 0x0C1004), cc = mark(c.log, 0x0C1006), d = mark(c.log, 0x0C1008);
            auto rs = mark(c.log, 0x0C100A);
            if (a) r.mkA = *a; if (b) r.mkB = *b; if (cc) r.mkC = *cc; if (d) r.mkD = *d; if (rs) r.mkRun = *rs;
            Anchor anc = measure_anchor(c.log);
            r.anc_ok = anc.ok; r.t_cpu = anc.t_cpu;
            return r;
        };
        // The LOAD strobe's count bound comes from the markers that bracket the LOAD write and the stop write
        // (count_bounds); the precondition is printed first and the line is graded only when it holds.
        struct LoadV { V whole = V::NOTGRADED, read = V::NOTGRADED; };
        auto grade_load = [&](const std::string& base_a, const std::string& base_b, const char* propid, const char* pagecite) {
            LoadResult la = run_load(base_a), lb = run_load(base_b);
            auto ok_one = [](const LoadResult& r) {
                if (!r.loaded || !r.anc_ok) return false;
                if (!(r.mkA && r.mkA < r.mkB && r.mkB < r.mkC && r.mkC < r.mkD)) return false;
                double hi = count_bounds(r.mkA, r.mkB, r.mkC, r.mkD, r.t_cpu).hi;
                return r.v2 == 0x4000 && (r.c & 0x10) == 0 && (double(0xFFFF - r.v3) <= hi);
            };
            auto pre_one = [](const LoadResult& r) { return r.loaded && r.v1 != 0x4000; };
            bool ok = ok_one(la) && ok_one(lb);
            bool pre_ok = pre_one(la) && pre_one(lb);
            checks.expect(pre_ok, std::string(propid) + "-PRE [p342,p348]: the counter has moved before "
                          "leg 1's read (v1<>$4000), on both CIAs");
            emit(checks, pre_ok, ok, propid, std::string(propid) + std::string(" [") + pagecite + "]: v2=$4000 (a force "
                 "load loads the latch), c bit4=0 (always reads 0), and $FFFF-v3, the number of E cycles counted "
                 "after the LOAD write, is at most (D-A)/T_E+1, A the marker before the LOAD write and D the marker "
                 "after the stop write (the LOAD strobe reloaded the counter), on both CIAs",
                 std::string(propid) + "-PRE failed");
            auto bound_text = [&](const LoadResult& r) {
                if (!r.loaded || !r.anc_ok || !r.mkA || !r.mkD) return std::string("?");
                char b[48];
                std::snprintf(b, sizeof b, "%.2f", count_bounds(r.mkA, r.mkB, r.mkC, r.mkD, r.t_cpu).hi);
                return std::string(b);
            };
            checks.note(std::string(propid) + " CIAA v1=" + hex4(la.v1) + " v2=" + hex4(la.v2) +
                        " c=" + hex2(la.c) + " v3=" + hex4(la.v3) + " (counted " + std::to_string(0xFFFF - la.v3) +
                        ", bound " + bound_text(la) + ")" +
                        "; CIAB v1=" + hex4(lb.v1) + " v2=" + hex4(lb.v2) +
                        " c=" + hex2(lb.c) + " v3=" + hex4(lb.v3) + " (counted " + std::to_string(0xFFFF - lb.v3) +
                        ", bound " + bound_text(lb) + ")");
            LoadV lv;
            lv.whole = vof(pre_ok, ok);
            // LOAD has no 0 setting (Section 0): the 0-setting rows of P10 join only "LOAD reads 0" (c bit 4 = 0 on both
            // CIAs, read after the LOAD write), not the force load and its count bound.
            lv.read = vof(la.loaded && lb.loaded, (la.c & 0x10) == 0 && (lb.c & 0x10) == 0);
            return lv;
        };
        // The contrasts change the CIAB program: no start in the stopped leg (PRE), CR<-$00 instead of
        // $10 (LD0), CR<-$01 instead of $11 in the running leg (CONTRAST).
        LoadV l7 = grade_load(dir+"/p7_ciaa_load.bin",
                   dir + (sel("P74-PRE") ? "/p7_ciab_load_nostart.bin"
                          : sel("P74-LD0") ? "/p7_ciab_load_ld0.bin"
                          : sel("P74-CONTRAST") ? "/p7_ciab_load_run01.bin"
                          : "/p7_ciab_load.bin"),
                   "P7.4", "p342,p348");
        pv.p74 = l7.whole; pv.p74_read = l7.read;
        LoadV l8 = grade_load(dir+"/p8_ciaa_load.bin",
                   dir + (sel("P84-PRE") ? "/p8_ciab_load_nostart.bin"
                          : sel("P84-LD0") ? "/p8_ciab_load_ld0.bin"
                          : sel("P84-CONTRAST") ? "/p8_ciab_load_run01.bin"
                          : "/p8_ciab_load.bin"),
                   "P8.4", "p342,p349");
        pv.p84 = l8.whole; pv.p84_read = l8.read;
    }

    // ---- P7.7 (CRA bit 7, unused) -------------------------------------------
    {
        struct UnResult { bool loaded=false; uint16_t c1=0,c2=0,w=0; uint64_t mkA=0,mkD=0; uint64_t t_cpu=0; bool anc_ok=false; };
        auto run_un = [&](const std::string& bin) {
            UnResult r;
            Capture c = run_capture(bin, kRunBudget, {kResults+8,kResults+10,kResults+12});
            r.loaded = c.loaded && c.done.has_value();
            if (!r.loaded) return r;
            r.c1 = pk(c,0); r.c2 = pk(c,1); r.w = pk(c,2);
            auto a = mark(c.log, 0x0C1002), d = mark(c.log, 0x0C1008);
            if (a) r.mkA = *a; if (d) r.mkD = *d;
            Anchor anc = measure_anchor(c.log);
            r.anc_ok = anc.ok; r.t_cpu = anc.t_cpu;
            return r;
        };
        // The CIAB $81 run is replaced by one stimulus change at a time: START written as 0 (P77-STARTOFF),
        // a used bit (PBON) in place of bit 7 in the first write (P77-C1CONTRAST), a different delay (P77-PRE).
        std::string b81b_bin = dir + (sel("P77-STARTOFF") ? "/p7_ciab_unused81_startoff.bin"
                                      : sel("P77-C1CONTRAST") ? "/p7_ciab_unused_c1contrast.bin"
                                      : sel("P77-PRE") ? "/p7_ciab_unused81_delay.bin"
                                      : "/p7_ciab_unused81.bin");
        auto b81a = run_un(dir+"/p7_ciaa_unused81.bin"), b01a = run_un(dir+"/p7_ciaa_unused01.bin");
        auto b81b = run_un(b81b_bin), b01b = run_un(dir+"/p7_ciab_unused01.bin");
        // The two runs' counts differ by at most the whole pulses their bracket-to-bracket times differ by, plus
        // one (counting whole pulses in an interval, as count_bounds does): |tau81-tau01|/T_E + 1.
        auto w_tolerance = [](const UnResult& r81, const UnResult& r01) {
            if (!r81.anc_ok || !r01.anc_ok || r81.t_cpu == 0) return 1.0;
            double d = std::fabs(double(int64_t(r81.mkD) - int64_t(r81.mkA)) - double(int64_t(r01.mkD) - int64_t(r01.mkA)));
            return d / (10.0 * double(r81.t_cpu)) + 1.0;
        };
        auto ok_pair = [&](const UnResult& r81, const UnResult& r01) {
            if (!r81.loaded || !r01.loaded) return false;
            return r81.c1 == 0 && r01.c1 == 0 && r81.c2 == 0x01 && r01.c2 == 0x01 &&
                   std::fabs(double(int(r81.w) - int(r01.w))) <= w_tolerance(r81, r01);
        };
        bool ok = ok_pair(b81a,b01a) && ok_pair(b81b,b01b);
        // P7.7-PRE's own margin is the run's own T_E (F2: 10*T_CPU), not a
        // constant clk_sys figure.
        auto bracket_ok = [](const UnResult& r81, const UnResult& r01) {
            if (!r81.loaded || !r01.loaded || !r81.anc_ok || !r01.anc_ok) return false;
            int64_t tau81 = int64_t(r81.mkD) - int64_t(r81.mkA);
            int64_t tau01 = int64_t(r01.mkD) - int64_t(r01.mkA);
            return std::llabs(tau81 - tau01) <= (int64_t)(10 * r81.t_cpu);
        };
        bool p77_pre = bracket_ok(b81a,b01a) && bracket_ok(b81b,b01b);
        pv.p77 = vof(p77_pre, ok);
        checks.expect(p77_pre,
                      "P7.7-PRE [SPECIFICATION.md P7.7]: both runs have the same bracket-to-bracket time "
                      "within about 1 E period (the run's own T_E), on both CIAs");
        emit(checks, p77_pre, ok, "P7.7", "P7.7 [p349]: c1=$00 (bit7 forced to 0 on a read), c2=$01 (bit0 read back, "
             "unaffected by bit7), and |w81-w01|<=|tau81-tau01|/T_E+1, tau the bracket-to-bracket time of a run "
             "(bit7 does not affect operation), on both "
             "CIAs, in a $81-run and a $01-run of the same instance", "P7.7-PRE failed");
        checks.note("P7.7 CIAA c1(81)=" + hex2(b81a.c1) + " c2(81)=" + hex2(b81a.c2) + " w81=" + hex4(b81a.w) +
                    " c2(01)=" + hex2(b01a.c2) + " w01=" + hex4(b01a.w) +
                    "; CIAB c1(81)=" + hex2(b81b.c1) + " c2(81)=" + hex2(b81b.c2) + " w81=" + hex4(b81b.w) +
                    " c2(01)=" + hex2(b01b.c2) + " w01=" + hex4(b01b.w));
    }

    // ---- P7.5 / P8.65 mode 01 (bit 5, INMODE: CNT vs phi2) + KP -----------
    struct InmodeResult {
        bool loaded=false; uint16_t i=0, v=0; double w_e=0; bool have_tcpu=false; uint64_t t_cpu=0;
        std::optional<uint64_t> start_a, ready, icr_store, stop_a, stop_b, send;
    };
    auto grade_inmode = [&](const std::string& bin, bool do_key, uint64_t min_e_after_ready = 100) {
        InmodeResult r;
        KeyCapture kc = run_capture_key(bin, 0x0C1002, do_key, 0x35, min_e_after_ready, kRunBudget, {kResults+8,kResults+10});
        r.loaded = kc.loaded && kc.done.has_value();
        if (!r.loaded) return r;
        r.i = pk(view_of(kc),0); r.v = pk(view_of(kc),1);
        // W (SPECIFICATION.md P7.5): the marker after the start write (ready)
        // to the marker after the stop write -- not the wait loop's own end
        // marker, which the counter keeps running past.
        r.ready = mark(kc.log, 0x0C1002); r.stop_a = mark(kc.log, 0x0C1006); r.stop_b = mark(kc.log, 0x0C1008);
        r.start_a = mark(kc.log, 0x0C100A);        // written by the P7.5 programs only
        r.icr_store = mark(kc.log, kResults+8);
        r.send = kc.send_cycle;
        r.t_cpu = kc.t_cpu;
        r.have_tcpu = kc.t_cpu > 0;
        if (r.ready && r.stop_b && r.have_tcpu) r.w_e = double(*r.stop_b - *r.ready) / (10.0*double(kc.t_cpu));
        return r;
    };
    InmodeResult p75a, p75b;
    InmodeResult p865_01a, p865_01b;
    // The reason the counting half of INMODE=CNT is not graded (P7.5, P7.5-CIAA-INMODE, P8.65-MODE01).
    const std::string counting_why =
        "the counting half of INMODE=CNT (\"counts positive CNT transitions\", p348, p349) needs a CNT source whose "
        "edges are known to reach the timer; CIAB's CNT is undriven and no output of the system shows KCLK at CIAA's "
        "CNT input (Section 3, item 11), so the normal run grades only the zero-edge half and the P10 members of "
        "INMODE=CNT are FAIL or not graded, never PASS";
    // The window precondition (SPECIFICATION.md P7.5): 1500 <= W <= 3000 E periods; where a key is sent, the key
    // code is put on the port group at least 100 E periods after the ready marker, which follows the start write,
    // and before the ICR read; the ICR read (i) shows SP=1; the stop write comes after.
    // The timing clauses (marker order, W in [1500,3000] E periods, and where a key is sent, the key code put on the
    // port group at least 100 E periods after the ready marker and before the ICR read, itself before the stop write)
    // are what P7.6-CIAA is graded under; the window adds i's SP=1, which is P7.6-CIAA's own result.
    auto window_timing = [](const InmodeResult& r, bool with_key) {
        if (!r.loaded || !r.have_tcpu || !r.ready || !r.stop_a || !r.stop_b) return false;
        bool order = *r.ready < *r.stop_a && *r.stop_a < *r.stop_b;
        bool w_in = r.w_e >= 1500.0 && r.w_e <= 3000.0;
        if (!with_key) return order && w_in;
        if (!r.send || !r.icr_store) return false;
        bool sent = double(*r.send) >= double(*r.ready) + 100.0*10.0*double(r.t_cpu) &&
                    *r.send < *r.icr_store && *r.icr_store < *r.stop_a;
        return order && w_in && sent;
    };
    auto window_ok = [&](const InmodeResult& r, bool with_key) {
        return window_timing(r, with_key) && (!with_key || (r.i & kIcrSp));
    };
    // CR<-$01 (phi2) counts the E cycles between the start write and the stop write, each bounded by the markers
    // around it (count_bounds): $1000-v lies between the two bounds.
    auto counted_ok = [](const InmodeResult& r) {
        if (!r.loaded || !r.have_tcpu || !r.start_a || !r.ready || !r.stop_a || !r.stop_b) return false;
        CountBounds b = count_bounds(*r.start_a, *r.ready, *r.stop_a, *r.stop_b, r.t_cpu);
        double n = double(0x1000 - int(r.v));
        return n >= b.lo && n <= b.hi;
    };
    // The two-phase run on CIAA (p75ctl_body.s): one timer started twice in INMODE=CNT with the same code. The
    // first interval holds no CNT edge (the key is not sent before it ends); the second holds the eight edges of
    // one key code. Each interval ends with a bracketed stop and a read of the stopped counter.
    struct CtlResult {
        bool loaded = false, zero_edge = false, long_enough = false;
        uint16_t v1 = 0, i1 = 0, i2 = 0, v2 = 0;
    };
    auto run_ctl = [&](const std::string& bin, uint32_t ready_addr, bool send_key) {
        CtlResult r;
        KeyCapture kc = run_capture_key(bin, ready_addr, send_key, 0x35, 100, kRunBudget,
                                        {kResults+8, kResults+10, kResults+12, kResults+14});
        Capture cv = view_of(kc);
        auto start1 = mark(kc.log, 0x0C1002), ready1 = mark(kc.log, 0x0C1004), stop1a = mark(kc.log, 0x0C1006),
             stop1b = mark(kc.log, 0x0C1008), ready2 = mark(kc.log, 0x0C100C);
        r.loaded = kc.loaded && kc.done.has_value() && start1 && ready1 && stop1a && stop1b && ready2 &&
                   kc.t_cpu > 0 && pk_ok(cv,0) && pk_ok(cv,1) && pk_ok(cv,2) && pk_ok(cv,3);
        if (!r.loaded) return r;
        r.v1 = pk(cv,0); r.i1 = pk(cv,1); r.i2 = pk(cv,2); r.v2 = pk(cv,3);
        // no CNT edge in the first interval: the ICR read after it has SP=0 and the key code is not sent before it ends
        r.zero_edge = !(r.i1 & kIcrSp) && (!kc.send_cycle || *kc.send_cycle > *stop1b);
        // the first interval is long enough that a timer counting phi2 would have counted at least one pulse
        r.long_enough = count_bounds(*start1, *ready1, *stop1a, *stop1b, kc.t_cpu).lo >= 1.0;
        return r;
    };
    {
        // Stimulus changes, one selector each: the CIAA run without a key (P76CIAA), with the key scheduled after
        // the stop write (P7.5-WINDOW) or with a wait that makes W shorter than 1500 E periods (P7.5-WINDOW-W);
        // the CIAB run with START written as 0 (P7.5) or with the short wait (P7.5-WINDOW-CIAB).
        p75a = grade_inmode(dir + (sel("P7.5-WINDOW-W") ? "/p7_ciaa_inmode_short.bin" : "/p7_ciaa_inmode.bin"),
                            !sel("P76CIAA"), sel("P7.5-WINDOW") ? 3500 : 100);
        p75b = grade_inmode(dir + (sel("P7.5") ? "/p7_ciab_inmode_nostart.bin"
                                   : sel("P7.5-WINDOW-CIAB") ? "/p7_ciab_inmode_short.bin"
                                   : "/p7_ciab_inmode.bin"), false);
        bool win_a = window_ok(p75a, true), win_b = window_ok(p75b, false);
        checks.expect(win_a, "P7.5-WINDOW [p345,p357]: on CIAA the key code is sent at least 100 E periods after "
                      "the ready marker and before the ICR read, i has SP=1 (the transmission finished before the "
                      "stop write), and W lies in [1500,3000] E periods");
        checks.expect(win_b, "P7.5-WINDOW-CIAB [SPECIFICATION.md P7.5]: on CIAB the stop write follows the ready "
                      "marker and W lies in [1500,3000] E periods");
        bool ciab_ok = p75b.loaded && p75b.v == 0x1000;
        // Only the zero-edge half of INMODE=CNT is graded, so the P10 member is FAIL or not graded, never PASS.
        pv.p75 = (win_b && !ciab_ok) ? V::FAIL : V::NOTGRADED;
        emit(checks, win_b, ciab_ok, "P7.5", "P7.5 [p348,p351]: INMODE=CNT, zero-edge half: with CRA INMODE=CNT, CIAB's "
             "timer A (CNT undriven, no positive transition) does not move over the window: v=$1000",
             "P7.5-WINDOW-CIAB failed");
        checks.record("P7.5-COUNTING [p348,p351]: not graded -- " + counting_why);
        checks.note("P7.5-CIAA [p348,p351,p357]: not graded as a count -- CIAA's timer A, CRA INMODE=CNT, one key code (eight KCLK "
                    "pulses) in the window, decremented by " + std::to_string(0x1000 - int(p75a.v)) + " (the page's "
                    "count is 8) over W=" + std::to_string(p75a.w_e) + "E, i=" + hex2(p75a.i) + ", v=" + hex4(p75a.v) +
                    "; no port shows KCLK at the timer's CNT input, so this decrement cannot separate an ignored INMODE "
                    "bit from a KCLK pulse that does not reach CNT; P7.5-CIAA-INMODE grades the interval with no CNT edge");
        checks.note("P7.5 CIAB v=" + hex4(p75b.v) + " W=" + std::to_string(p75b.w_e) + "E");

        // P7.5-CONTRAST is the page's own contrasting case: CR<-$01 (phi2) counts the E cycles between the
        // start write and the stop write, each bounded by the markers around it (count_bounds). Its mutation
        // is the CIAB run with START written as 0 (P75-CONTRAST-MUT).
        {
            InmodeResult ca = grade_inmode(dir+"/p7_ciaa_inmode_phi2.bin", true);
            InmodeResult cb = grade_inmode(dir + (sel("P75-CONTRAST-MUT") ? "/p7_ciab_inmode_nostart.bin"
                                                  : "/p7_ciab_inmode_phi2.bin"), false);
            checks.expect(counted_ok(ca) && counted_ok(cb), "P7.5-CONTRAST [p348,p353]: CR<-$01 (phi2) instead of $21 "
                          "counts the E cycles between the start write and the stop write: $1000-v lies between "
                          "(stop_before-start_after)/T_E-1 and (stop_after-start_before)/T_E+1, the markers around each "
                          "write, on both CIAs");
        }

        // P7.5-CIAA-INMODE: CIAA's timer A, CR<-$21 (INMODE=CNT, START), over an interval in which no CNT edge occurs.
        // p348: "1 = Timer A counts positive CNT transitions, 0 = Timer A counts 02 pulses": with no edge the timer
        // does not move. The one run (p7_ciaa_inmode_ctl.bin) also holds the second interval with one key code.
        // Selectors: P7.5-CIAA-INMODE (start write $20, the timer not started), P7.5-CIAA-INMODE-PRE (the key code
        // sent in the first interval), and the control's own (P75-GREENCTL-PRE, P75-GREENCTL-NOKEY).
        {
            const bool early = sel("P75-GREENCTL-PRE") || sel("P7.5-CIAA-INMODE-PRE");
            CtlResult cr = run_ctl(dir + (sel("P7.5-CIAA-INMODE") ? "/p7_ciaa_inmode_ctl_nostart1.bin"
                                          : "/p7_ciaa_inmode_ctl.bin"),
                                   early ? 0x0C1004 : 0x0C100C, !sel("P75-GREENCTL-NOKEY"));
            const bool pre = cr.loaded && cr.zero_edge && cr.long_enough;
            checks.expect(pre, "P7.5-CIAA-INMODE-PRE [SPECIFICATION.md P7.5]: no CNT edge reaches CIAA's first interval "
                          "(the ICR read after it has SP=0 and the key code is not sent before it ends), and the interval is "
                          "long enough that a timer counting phi2 would have counted at least one pulse "
                          "(count_bounds' lower bound is at least 1)");
            const bool ok = cr.loaded && cr.v1 == 0x1000;
            pv.p75_ciaa = (pre && !ok) ? V::FAIL : V::NOTGRADED;   // the zero-edge half only: FAIL or not graded
            emit(checks, pre, ok, "P7.5-CIAA-INMODE", "P7.5-CIAA-INMODE [p348,p351]: INMODE=CNT, zero-edge half: with CRA "
                 "INMODE=CNT (CR<-$21, started), CIAA's timer A does not move over an interval in which no CNT edge "
                 "occurs: v1=$1000", "P7.5-CIAA-INMODE-PRE failed");
            checks.record("P7.5-CIAA-INMODE-COUNTING [p348,p351]: not graded -- " + counting_why);
            checks.note("P7.5-CIAA-INMODE CIAA v1=" + hex4(cr.v1) + " i1=" + hex2(cr.i1) + " (first interval, no CNT edge); "
                        "after the second start, with one key code: i2=" + hex2(cr.i2) + " v2=" + hex4(cr.v2) +
                        " (the page's count is 8)");
            // Green control for P7.5 (CIAB leg), selector only: the same equality comparator on a started timer
            // in INMODE=CNT whose count input is known: v1 counts 0 and v1-v2 counts 8, i2 has SP=1.
            if (sel("P75-GREENCTL") || sel("P75-GREENCTL-PRE") || sel("P75-GREENCTL-NOKEY")) {
                checks.expect(cr.loaded && cr.zero_edge, "P7.5-CIAB-GREENCTL-PRE [SPECIFICATION.md P7.5]: no CNT edge reaches the first "
                              "interval: the ICR read after it has SP=0 and the key code is not sent before it ends (CIAA)");
                auto counts = [](uint16_t from, uint16_t to, int edges) { return int(from) - int(to) == edges; };
                bool control_ok = cr.loaded && counts(0x1000, cr.v1, 0) && counts(cr.v1, cr.v2, 8) && (cr.i2 & kIcrSp);
                emit(checks, cr.loaded && cr.zero_edge, control_ok, "P7.5-CIAB-GREENCTL", "P7.5-CIAB-GREENCTL [SPECIFICATION.md P7.5]: a started "
                     "timer in CRA INMODE=CNT counts no pulse over an interval with no CNT edge ($1000-v=0) and one per edge "
                     "over an interval with one key code (v1-v2=8, i has SP=1), through one equality comparator (CIAA)",
                     "P7.5-CIAB-GREENCTL-PRE failed");
                checks.note("P7.5-CIAB-GREENCTL CIAA v1=" + hex4(cr.v1) + " i1=" + hex2(cr.i1) + " i2=" + hex2(cr.i2) +
                            " v2=" + hex4(cr.v2));
            }
        }

        // P8.65 mode 01 (CRB INMODE=01, CNT). Selectors: the CIAA key code after the stop write (P8.65-MODE01-WINDOW) or
        // the short wait (P8.65-MODE01-WINDOW-W); the CIAB short wait (P8.65-MODE01-WINDOW-CIAB); the CIAB timer not
        // started (P8.65-MODE01; P8.65-MODE01-CONTRAST puts the same run through the contrast's comparator).
        p865_01a = grade_inmode(dir + (sel("P8.65-MODE01-WINDOW-W") ? "/p8_ciaa_inmode01_short.bin" : "/p8_ciaa_inmode01.bin"),
                                true, sel("P8.65-MODE01-WINDOW") ? 3500 : 100);
        p865_01b = grade_inmode(dir + (sel("P8.65-MODE01") ? "/p8_ciab_inmode01_nostart.bin"
                                       : sel("P8.65-MODE01-WINDOW-CIAB") ? "/p8_ciab_inmode01_short.bin"
                                       : "/p8_ciab_inmode01.bin"), false);
        bool m01_win_a = window_ok(p865_01a, true), m01_win_b = window_ok(p865_01b, false);
        checks.expect(m01_win_a, "P8.65-MODE01-WINDOW [p345,p357]: on CIAA the key code is sent at least 100 E periods "
                      "after the ready marker and before the ICR read, i has SP=1 (the transmission finished before the "
                      "stop write), and W lies in [1500,3000] E periods");
        checks.expect(m01_win_b, "P8.65-MODE01-WINDOW-CIAB [SPECIFICATION.md P8.65]: on CIAB the stop write follows the "
                      "ready marker and W lies in [1500,3000] E periods");
        bool m01_ciab_ok = p865_01b.loaded && p865_01b.v == 0x1000;
        pv.p865_01 = (m01_win_b && !m01_ciab_ok) ? V::FAIL : V::NOTGRADED;   // the zero-edge half only
        emit(checks, m01_win_b, m01_ciab_ok, "P8.65-MODE01", "P8.65-MODE01 [p349,p351]: INMODE=01 (CNT), zero-edge half: "
             "with CRB INMODE=01, CIAB's timer B (CNT undriven, no positive transition) does not move over the window: "
             "v=$1000", "P8.65-MODE01-WINDOW-CIAB failed");
        checks.record("P8.65-MODE01-COUNTING [p349,p351]: not graded -- " + counting_why);
        checks.note("P8.65-MODE01 CIAA [p349,p351,p357]: not graded -- CIAA's timer B, CRB INMODE=01, one key code (eight KCLK "
                    "pulses) in the window, decremented by " + std::to_string(0x1000 - int(p865_01a.v)) + " (the page's "
                    "count is 8) over W=" + std::to_string(p865_01a.w_e) + "E, i=" + hex2(p865_01a.i) + ", v=" +
                    hex4(p865_01a.v) + "; no port shows KCLK at the timer's CNT input, so this decrement cannot "
                    "separate an ignored INMODE field from a KCLK pulse that does not reach CNT");
        checks.note("P8.65-MODE01 CIAB v=" + hex4(p865_01b.v) + " W=" + std::to_string(p865_01b.w_e) + "E");

        // P8.65-MODE01-CONTRAST is the page's own contrasting case: CRB<-$01 (mode 00, phi2) counts the E cycles between
        // the start write and the stop write, each bounded by the markers around it (count_bounds).
        {
            InmodeResult ca = grade_inmode(dir+"/p8_ciaa_inmode01_phi2.bin", true);
            InmodeResult cb = grade_inmode(dir + (sel("P8.65-MODE01-CONTRAST") ? "/p8_ciab_inmode01_nostart.bin"
                                                  : "/p8_ciab_inmode01_phi2.bin"), false);
            checks.expect(counted_ok(ca) && counted_ok(cb), "P8.65-MODE01-CONTRAST [p349,p353]: CRB<-$01 (mode 00, phi2) "
                          "instead of $21 counts the E cycles between the start write and the stop write: $1000-v lies between "
                          "(stop_before-start_after)/T_E-1 and (stop_after-start_before)/T_E+1, the markers around each "
                          "write, on both CIAs");
        }

        // Green control for P8.65 mode 01 (CIAB leg), selector only: the two-phase run on CIAA's timer B.
        if (sel("P865M01-GREENCTL") || sel("P865M01-GREENCTL-PRE") || sel("P865M01-GREENCTL-NOKEY")) {
            CtlResult cr = run_ctl(dir + "/p8_ciaa_inmode01_ctl.bin", sel("P865M01-GREENCTL-PRE") ? 0x0C1004 : 0x0C100C,
                                   !sel("P865M01-GREENCTL-NOKEY"));
            checks.expect(cr.loaded && cr.zero_edge, "P8.65-MODE01-CIAB-GREENCTL-PRE [SPECIFICATION.md P8.65]: no CNT edge "
                          "reaches the first interval: the ICR read after it has SP=0 and the key code is not sent before it "
                          "ends (CIAA)");
            auto counts = [](uint16_t from, uint16_t to, int edges) { return int(from) - int(to) == edges; };
            bool control_ok = cr.loaded && counts(0x1000, cr.v1, 0) && counts(cr.v1, cr.v2, 8) && (cr.i2 & kIcrSp);
            emit(checks, cr.loaded && cr.zero_edge, control_ok, "P8.65-MODE01-CIAB-GREENCTL", "P8.65-MODE01-CIAB-GREENCTL "
                 "[SPECIFICATION.md P8.65]: a started timer B in CRB INMODE=01 counts no pulse over an interval with no CNT "
                 "edge ($1000-v=0) and one per edge over an interval with one key code (v1-v2=8, i has SP=1), through one "
                 "equality comparator (CIAA)", "P8.65-MODE01-CIAB-GREENCTL-PRE failed");
            checks.note("P8.65-MODE01-CIAB-GREENCTL CIAA v1=" + hex4(cr.v1) + " i1=" + hex2(cr.i1) + " i2=" + hex2(cr.i2) +
                        " v2=" + hex4(cr.v2));
        }
    }

    // ---- KP (keyboard idle): recorded, not a gating precondition (KP's own
    // probe counts CNT, the clause under test -- see SPECIFICATION.md 1.5
    // KP, "Not usable as a gating precondition").
    {
        InmodeResult kpa_noKey, kpa_key, kpb_noKey, kpb_key;
        auto grade_kp = [&](const std::string& bin, bool send_key) {
            InmodeResult r;
            KeyCapture kc = run_capture_key(bin, 0x0C1002, send_key, 0x35, 100, kRunBudget, {kResults+8});
            r.loaded = kc.loaded && kc.done.has_value();
            if (r.loaded) r.v = pk(view_of(kc),0);
            return r;
        };
        kpa_noKey = grade_kp(dir+"/kp_a.bin", false);
        kpa_key   = grade_kp(dir+"/kp_a.bin", true);
        kpb_noKey = grade_kp(dir+"/kp_b.bin", false);
        kpb_key   = grade_kp(dir+"/kp_b.bin", true);
        checks.record("KP-A [SPECIFICATION.md 1.5 KP]: not a gating precondition (it depends on the "
                      "clause under test, INMODE=CNT). Measured: no-key v=" + hex4(kpa_noKey.v) +
                      ", with-key v=" + hex4(kpa_key.v) + " -- both CNT, so the two cannot be told apart "
                      "here; P7.5's own CIAB leg carries the red without it");
        checks.record("KP-B [SPECIFICATION.md 1.5 KP]: not a gating precondition, the same reason. "
                      "Measured: no-key v=" + hex4(kpb_noKey.v) + ", with-key v=" + hex4(kpb_key.v) +
                      " -- P8.65 mode 01's own CIAB leg carries the red without it");
    }

    // ---- P7.6 (CRA bit 6, SPMODE) ------------------------------------------
    {
        // CIAA leg reuses P7.5's own run (i, made before its stop write) --
        // this leg's own positive result and its own window precondition.
        // Graded under P7.5-WINDOW's timing clauses (the key code sent in time, W in its window); SP=1 is its result.
        const bool ciaa_timing = window_timing(p75a, true);
        bool ciaa_ok = p75a.loaded && (p75a.i & kIcrSp);
        pv.p76_ciaa = vof(ciaa_timing, ciaa_ok);
        emit(checks, ciaa_timing, ciaa_ok, "P7.6-CIAA", "P7.6-CIAA [p345,p351,p357]: SP=1 in i, the ICR read P7.5's CIAA run "
             "makes before its stop write (input mode, CRA bit6=0)",
             "the timing clauses of P7.5-WINDOW (the key code sent in time, W in its window) failed");
        // The contrast is the same run without R-KEY: SP=0. Its mutation sends the key (P76CIAA-CONTRAST-MUT).
        {
            InmodeResult ciaa_noKey = grade_inmode(dir+"/p7_ciaa_inmode.bin", sel("P76CIAA-CONTRAST-MUT"));
            bool ciaa_contrast_ok = ciaa_noKey.loaded && !(ciaa_noKey.i & kIcrSp);
            checks.expect(ciaa_contrast_ok, "P7.6-CIAA-CONTRAST [SPECIFICATION.md P7.6]: the same run "
                          "without R-KEY gives SP=0");
        }

        // CIAB: the precondition first (P76CIAB-PRE starts no timer), then the line.
        Capture cb = run_capture(dir + (sel("P76CIAB-PRE") ? "/p76_ciab_pre.bin" : "/p76_ciab.bin"), kRunBudget, {});
        SampleSeries icrb = read_series(cb.log, kResults+8, 300);
        int taCount = 0; bool anySp = false;
        for (auto& v : icrb.value) if (v) { if (*v & kIcrTa) ++taCount; if (*v & kIcrSp) anySp = true; }
        bool ciab_ok = cb.loaded && cb.done.has_value() && anySp;
        bool ciab_pre_ok = taCount >= 20;
        pv.p76_ciab = vof(ciab_pre_ok, ciab_ok);
        checks.expect(ciab_pre_ok, "P7.6-CIAB-PRE [SPECIFICATION.md P7.6]: timer A underflows during the "
                      "window (at least 20 samples have TA=1)");
        emit(checks, ciab_pre_ok, ciab_ok, "P7.6-CIAB", "P7.6-CIAB [p345,p348]: at least one ICR sample has SP=1 "
             "(CRA<-$41, SPMODE output, SDR<-$A5)", "P7.6-CIAB-PRE failed");
        checks.note("P7.6-CIAB TA samples=" + std::to_string(taCount) + " SP seen=" + std::string(anySp?"yes":"no"));

        // The contrast is CR<-$01 (input), SP=0 in every sample. Its mutation runs the output-mode program
        // on CIA-A, which shows SP (P76CIAB-CONTRAST-MUT).
        {
            Capture cbc = run_capture(dir + (sel("P76CIAB-CONTRAST-MUT") ? "/p76_ciaa.bin" : "/p76_ciab_contrast.bin"),
                                      kRunBudget, {});
            SampleSeries icrc = read_series(cbc.log, kResults+8, 300);
            bool anySpC = false; for (auto& v : icrc.value) if (v && (*v & kIcrSp)) anySpC = true;
            checks.expect(cbc.loaded && cbc.done.has_value() && !anySpC,
                          "P7.6-CIAB-CONTRAST [p345,p348]: CR<-$01 (bit6=0, input) gives SP=0 in every "
                          "sample (CIAB's CNT/POUT is not driven)");
        }
        // Green control: the same SPMODE=output program, run on CIA-A
        // instead: a proven-live input through the same "at least one
        // sample has SP=1" comparator P7.6-CIAB's red uses. Its mutation is the same program with CR<-$01
        // (input) and no key (P76CIAB-GREENCTL-RED).
        {
            Capture ca = run_capture(dir + (sel("P76CIAB-GREENCTL-RED") ? "/p76_ciaa_in.bin" : "/p76_ciaa.bin"),
                                     kRunBudget, {});
            SampleSeries icra = read_series(ca.log, kResults+8, 300);
            bool anySpA = false; for (auto& v : icra.value) if (v && (*v & kIcrSp)) anySpA = true;
            checks.expect(ca.loaded && ca.done.has_value() && anySpA,
                          "P7.6-CIAB-GREENCTL [SPECIFICATION.md P7.6]: the same program run on CIA-A "
                          "gives at least one SP=1 sample, through the same comparator P7.6-CIAB's red "
                          "uses");
        }
    }

    // ---- P8.65 mode 10 (CRB counts timer A underflow pulses) --------------
    {
        struct M10 {
            bool loaded = false, ok = false, pre_ok = false;
            uint16_t v = 0; int nA = 0; uint64_t t_cpu = 0;
            std::optional<uint64_t> start_a, start_b, stop_a, stop_b;   // the markers around CRB's start and stop writes
        };
        auto grade_mode10 = [&](const std::string& bin, const char* tag) {
            M10 m;
            Capture c = run_capture(bin, kRunBudget, {kResults+608, kResults+610});
            SampleSeries icr = read_series(c.log, kResults+8, 300);
            m.loaded = c.loaded && c.done.has_value();
            for (auto& v : icr.value) if (v && (*v & kIcrTa)) ++m.nA;
            if (pk_ok(c,0) && (pk(c,0) & kIcrTa)) ++m.nA;
            m.v = pk(c,1);
            m.ok = m.loaded && pk_ok(c,0) && pk_ok(c,1) && (0xFFFF - m.v) == m.nA;
            m.start_a = mark(c.log, 0x0C1002); m.start_b = mark(c.log, 0x0C1004);
            m.stop_a = mark(c.log, 0x0C1006);  m.stop_b = mark(c.log, 0x0C1008);
            // Pre8.65-M10: every sample is present, and the iteration spacing is at most P(30)/2 (F3), from the
            // array's own E-indices.
            Anchor anc = measure_anchor(c.log);
            m.t_cpu = anc.t_cpu;
            PeriodResult pr;
            if (anc.ok) {
                int64_t max_gap = 0; std::optional<uint64_t> prev;
                for (size_t i = 0; i < icr.size(); ++i) {
                    if (!icr.cycle[i]) continue;
                    if (prev) max_gap = std::max(max_gap, (int64_t)*icr.cycle[i] - (int64_t)*prev);
                    prev = icr.cycle[i];
                }
                pr = measure_period(icr, kIcrTa, anc.t_cpu);
                if (pr.ok)
                    m.pre_ok = m.loaded && pk_ok(c,0) && series_pop(icr) == 300 &&
                               double(max_gap) <= double(pr.P) * 10.0 * double(anc.t_cpu) / 2.0 + 1.0;
                checks.note(std::string(tag) + ": measured P(30)=" + std::to_string(pr.P) +
                            (pr.ok ? " (uniquely determined)" : " (not uniquely determined)"));
            }
            checks.note(std::string(tag) + ": nA=" + std::to_string(m.nA) + " v=" + hex4(m.v) +
                        " flagged=" + std::to_string(pr.flag_count));
            return m;
        };
        // One stimulus change at a time on the CIAB run: R-LATCH_A(2) instead of 30 (M10-PRE, Pre8.65-M10's own
        // contrast), or CRB<-$01 (mode 00) in place of $41 (M10).
        M10 ma = grade_mode10(dir+"/p865_ciaa_mode10.bin", "P8.65-M10 CIAA");
        M10 mb = grade_mode10(dir + (sel("M10-PRE") ? "/p865_ciab_mode10_shortlatch.bin"
                                     : sel("M10") ? "/p865_ciab_mode10_contrast.bin"
                                     : "/p865_ciab_mode10.bin"), "P8.65-M10 CIAB");
        const bool m10_pre = ma.pre_ok && mb.pre_ok;
        checks.expect(m10_pre, "P8.65-MODE10-PRE [SPECIFICATION.md F3]: all 300 ICR samples are present and the iteration "
                      "spacing is at most P(30)/2, so each TA flag is exactly one underflow, on both CIAs");
        pv.p865_10 = vof(m10_pre, ma.ok && mb.ok);
        emit(checks, m10_pre, ma.ok && mb.ok, "P8.65-MODE10", "P8.65-MODE10 [p349,p350]: $FFFF-v equals N_A, the number "
             "of ICR reads after CRA's start write (including the final one) that have TA=1, on both CIAs",
             "P8.65-MODE10-PRE failed");

        // P8.65-MODE10-CONTRAST is the page's own contrasting case: CRB<-$01 (mode 00, phi2) counts the E cycles
        // between CRB's start write and its stop write, each bounded by the markers around it (count_bounds), far more
        // than N_A. Its mutation puts the mode-10 run in its place (M10-CONTRAST).
        {
            M10 mc = grade_mode10(dir + (sel("M10-CONTRAST") ? "/p865_ciab_mode10.bin" : "/p865_ciab_mode10_contrast.bin"),
                                  "P8.65-M10 contrast CIAB");
            bool cok = false;
            if (mc.loaded && mc.t_cpu > 0 && mc.start_a && mc.start_b && mc.stop_a && mc.stop_b) {
                CountBounds b = count_bounds(*mc.start_a, *mc.start_b, *mc.stop_a, *mc.stop_b, mc.t_cpu);
                double n = double(0xFFFF - mc.v);
                cok = n >= b.lo && n <= b.hi;
            }
            checks.expect(cok, "P8.65-MODE10-CONTRAST [p349]: CRB<-$01 (mode 00) counts the E cycles between CRB's start "
                          "write and its stop write: $FFFF-v lies between (stop_before-start_after)/T_E-1 and "
                          "(stop_after-start_before)/T_E+1, the markers around each write, far more than N_A (CIAB)");
        }
    }

    // ---- P8.65 mode 11 (CIAA only) -----------------------------------------
    {
        struct M11 {
            bool loaded = false; uint16_t i = 0, v = 0; uint64_t window = 0; uint64_t t_cpu = 0; double w_e = 0;
            std::optional<uint64_t> ready, icr_store, stop_a, stop_b, send;
        };
        // The CRB value (mode 10 $41, mode 11 $61) is read from the parameter block (E7); the key code, where one
        // is sent, goes out at least min_e E periods after the ready marker (R-KEY).
        auto run_m11 = [&](const std::string& bin, uint8_t crbval, bool send_key, uint64_t min_e = 100) {
            M11 r;
            KeyCapture kc = run_capture_key(bin, 0x0C1004, send_key, 0x35, min_e, kRunBudget,
                                            {kResults+8, kResults+10}, nmh::Config{}, crbval);
            r.loaded = kc.loaded && kc.done.has_value();
            if (!r.loaded) return r;
            Capture cv = view_of(kc);
            r.i = pk(cv,0); r.v = pk(cv,1);
            r.ready = mark(kc.log, 0x0C1004); r.stop_a = mark(kc.log, 0x0C1008); r.stop_b = mark(kc.log, 0x0C100A);
            r.icr_store = mark(kc.log, kResults+8);
            r.send = kc.send_cycle;
            r.t_cpu = kc.t_cpu;
            if (r.ready && r.stop_b) {
                r.window = *r.stop_b - *r.ready;
                if (kc.t_cpu > 0) r.w_e = double(r.window) / (10.0*double(kc.t_cpu));
            }
            return r;
        };
        // SPECIFICATION.md P8.65 mode 11: every run waits W E periods, 1500 <= W <= 3000, from the marker after CRA's
        // start write to the marker after its stop write; in pair K the code is sent at least 100 E periods after the
        // ready marker and before the ICR read, and i, read before CRA's stop write, has SP=1.
        auto m11_window_ok = [](const M11& r, bool with_key) {
            if (!r.loaded || r.t_cpu == 0 || !r.ready || !r.stop_a || !r.stop_b || !r.icr_store) return false;
            bool order = *r.ready < *r.icr_store && *r.icr_store < *r.stop_a && *r.stop_a < *r.stop_b;
            bool w_in = r.w_e >= 1500.0 && r.w_e <= 3000.0;
            if (!with_key) return order && w_in;
            if (!r.send) return false;
            bool sent = double(*r.send) >= double(*r.ready) + 100.0*10.0*double(r.t_cpu) && *r.send < *r.icr_store;
            return order && w_in && sent && (r.i & kIcrSp);
        };
        const std::string b11 = dir + "/p865_ciaa_mode11.bin";
        // One stimulus change at a time: the key code after CRA's stop write in pair K (M11-WINDOW), a wait of 200
        // dbra iterations in both runs of pair N (M11-WINDOW-W), a wait of 2300 in pair N's run Y (M11-WINPRE), CRB<-$01
        // (mode 00) in place of $61 in pair N's run Y (M11-PAIRN).
        const std::string bshort = dir + "/p865_ciaa_mode11_short.bin";
        M11 nx = run_m11(sel("M11-WINDOW-W") ? bshort : b11, 0x41, false);
        M11 ny = run_m11(sel("M11-WINDOW-W") ? bshort : sel("M11-WINPRE") ? dir + "/p865_ciaa_mode11_w2300.bin" : b11,
                         sel("M11-PAIRN") ? 0x01 : 0x61, false);
        M11 kx = run_m11(b11, 0x41, true, sel("M11-WINDOW") ? 3500 : 100);
        M11 ky = run_m11(b11, 0x61, true, sel("M11-WINDOW") ? 3500 : 100);
        bool windows_ok = m11_window_ok(nx, false) && m11_window_ok(ny, false) && m11_window_ok(kx, true) && m11_window_ok(ky, true);
        checks.expect(windows_ok, "P8.65-MODE11-WINDOW [p345,p357]: in all four runs the stop write follows the ready marker "
                      "and W lies in [1500,3000] E periods; in pair K the key code is sent at least 100 E periods after the "
                      "ready marker and before the ICR read, and i has SP=1 (the transmission finished before CRA's stop write)");
        bool windows_eq = nx.loaded && ny.loaded && kx.loaded && ky.loaded && nx.window > 0 && kx.window > 0 &&
                          nx.window == ny.window && kx.window == ky.window;
        checks.expect(windows_eq, "P8.65-MODE11-WINPRE [SPECIFICATION.md P8.65 mode 11]: in each pair, "
                      "the two runs' windows (marker after CRA's start write to marker after its stop "
                      "write) are equally long in system clock cycles, so modes 10 and 11 are offered "
                      "the same timer A underflows");
        const bool m11_pre = windows_ok && windows_eq;
        bool pairN_ok = nx.loaded && ny.loaded && nx.v == ny.v;
        emit(checks, m11_pre, pairN_ok, "P8.65-MODE11-PAIRN", "P8.65-MODE11-PAIRN [p349,p357]: with no key sent, D_Y=D_X (mode 11 "
             "counts every underflow mode 10 counts, CNT idle high)", "P8.65-MODE11-WINDOW or P8.65-MODE11-WINPRE failed");
        // Pair K's "D_Y<D_X" is measured, not graded: no port shows KCLK at timer B's CNT input, so a D_Y equal to D_X
        // is as well the answer of an ignored mode-11 selection as of a KCLK that does not reach CNT.
        const bool pairK_lt = kx.loaded && ky.loaded && (0xFFFF - ky.v) < (0xFFFF - kx.v);
        checks.note("P8.65-MODE11-PAIRK [p349,p357]: not graded -- with a key sent, D_Y(K)=" + std::to_string(0xFFFF - ky.v) +
                    (pairK_lt ? " <" : " >=") + " D_X(K)=" + std::to_string(0xFFFF - kx.v) +
                    "; no port shows KCLK at timer B's CNT input, so D_Y=D_X cannot separate an ignored CRB mode-11 selection "
                    "from a KCLK that does not reach CNT");
        // Mode 11's claim has two halves (counts while CNT is high, not while CNT is low); only the first is graded.
        pv.p865_11 = (m11_pre && !pairN_ok) ? V::FAIL : V::NOTGRADED;
        checks.note("P8.65-MODE11 D_X(N)=" + std::to_string(0xFFFF-nx.v) + " D_Y(N)=" + std::to_string(0xFFFF-ny.v) +
                    " D_X(K)=" + std::to_string(0xFFFF-kx.v) + " D_Y(K)=" + std::to_string(0xFFFF-ky.v) +
                    " window(N)=" + std::to_string(nx.window) + "/" + std::to_string(ny.window) +
                    " window(K)=" + std::to_string(kx.window) + "/" + std::to_string(ky.window));
    }

    // ---- P8.7 (CRB bit 7, ALARM) -------------------------------------------
    // Run X (ALARM=1) and run Y (ALARM=0) are separate runs from reset, on each CIA; the CRB value of the TOD
    // writes comes from the parameter block (E7).
    {
        auto count_lines_between = [](const Anchor& a, uint64_t c1, uint64_t c2) {
            int n = 0; for (auto f : a.firsts) if (f > c1 && f <= c2) ++n; return n;
        };
        const std::vector<uint32_t> pk8 = {kResults+8,kResults+10,kResults+12,kResults+14,kResults+16,kResults+18};

        // CIAB. One stimulus change at a time: the LSB of the TOD<-0 write left out of run X's setup (P87-PRE), run X's
        // sampler of 449 iterations (P87-POP), CRB<-$00 for run X's TOD writes (P8.7-RUNX), CRB<-$80 for run Y's (P8.7-RUNY).
        Capture cx = run_capture_param(dir + (sel("P87-PRE") ? "/p87_ciab_x_nolsb.bin" : sel("P87-POP") ? "/p87_ciab_x_short.bin"
                                              : "/p87_ciab_x.bin"),
                                       0x0C2000, sel("P8.7-RUNX") ? 0x00 : 0x80, kRunBudget, pk8);
        Capture cy = run_capture_param(dir + "/p87_ciab_y.bin", 0x0C2000, sel("P8.7-RUNY") ? 0x80 : 0x00, kRunBudget, pk8);
        Anchor ancx = measure_anchor(cx.log), ancy = measure_anchor(cy.log);
        auto long_of = [](const Capture& c, size_t hi, size_t lo) { return (uint32_t(pk(c,hi))<<16)|pk(c,lo); };
        uint32_t t0x = long_of(cx,0,1), t1x = long_of(cx,2,3), t0y = long_of(cy,0,1), t1y = long_of(cy,4,5);
        // Pre8.7: TOD counts lines -- T0, read after TOD<-0 and a wait, matches the lines between the write and the read.
        auto setup_ok = [&](const Capture& c, const Anchor& a, uint32_t t0, const char* tag) {
            auto mwr = mark(c.log, 0x0C1010), mt0a = mark(c.log, 0x0C1002);
            bool ok = false;
            if (c.loaded && c.done.has_value() && a.ok && mwr && mt0a) {
                int lines = count_lines_between(a, *mwr, *mt0a);
                ok = std::abs(int(t0) - lines) <= 1;
                checks.note(std::string("P8.7-PRE CIAB ") + tag + ": T0=" + std::to_string(t0) + " lines-measured=" + std::to_string(lines));
            }
            return ok;
        };
        bool pre_ok = setup_ok(cx, ancx, t0x, "run X") && setup_ok(cy, ancy, t0y, "run Y");
        checks.expect(pre_ok, "P8.7-PRE [p340]: TOD counts lines -- T0 (from TOD<-0) matches the number "
                      "of lines elapsed (F4), within 1, in run X's setup and in run Y's (CIAB; the LSB never written keeps "
                      "TOD stopped)");

        SampleSeries xtod = read_series(cx.log, kResults+20, 450);
        SampleSeries xicr = read_series(cx.log, kResults+20+450*2, 450);
        SampleSeries ytod = read_series(cy.log, kResults+20+450*4, 450);
        SampleSeries yicr = read_series(cy.log, kResults+20+450*6, 450);
        bool pop_ok = series_pop(xtod) == 450 && series_pop(xicr) == 450 && series_pop(ytod) == 450 && series_pop(yicr) == 450;
        checks.expect(pop_ok, "P8.7-POP [SPECIFICATION.md P8.7]: all 450 TODLO stores and all 450 ICR stores of run X's "
                      "sampler and of run Y's are present (CIAB)");
        const bool gated = pre_ok && pop_ok;
        const std::string why_gate = "P8.7-PRE or P8.7-POP failed";

        bool alrmSeenX = false; std::optional<size_t> firstMatchIdx;
        uint8_t targetLo = uint8_t((t0x+12) & 0xFF);
        for (size_t i = 0; i < xtod.size(); ++i) {
            if (!firstMatchIdx && xtod.value[i] && uint8_t(*xtod.value[i] & 0xFF) == targetLo) firstMatchIdx = i;
            if (xicr.value[i] && (*xicr.value[i] & kIcrAlrm)) { if (!alrmSeenX) alrmSeenX = true; }
        }
        // The sampler reads TODLO before ICR every iteration, so the alarm
        // may already show in the (m-1)th ICR read (SPECIFICATION.md P8.7):
        // the first ALRM sample is ICR_(m-1) or ICR_m, none
        // earlier.
        std::optional<size_t> firstAlrmIdx;
        for (size_t i = 0; i < xicr.size(); ++i) if (xicr.value[i] && (*xicr.value[i] & kIcrAlrm)) { firstAlrmIdx = i; break; }
        bool alrmOrder_ok = firstMatchIdx && firstAlrmIdx &&
            (*firstAlrmIdx == *firstMatchIdx || *firstAlrmIdx + 1 == *firstMatchIdx);
        auto mt0a = mark(cx.log, 0x0C1002), mt1x = mark(cx.log, 0x0C1006);
        int linesX = (ancx.ok && mt0a && mt1x) ? count_lines_between(ancx, *mt0a, *mt1x) : -1;
        bool runX_ok = cx.loaded && cx.done.has_value() && linesX >= 0 &&
                       std::abs(int(t1x - t0x) - linesX) <= 1 && alrmSeenX && alrmOrder_ok;
        emit(checks, gated, runX_ok, "P8.7-RUNX", "P8.7-RUNX [p344,p349]: T1-T0 equals the lines elapsed (+-1); the first "
             "ALRM sample is ICR_(m-1) or ICR_m, where m is the first TODLO sample equal to "
             "A&$FF, and none earlier has it (CIAB)", why_gate);
        checks.note("P8.7-RUNX CIAB: firstMatch=" + (firstMatchIdx?std::to_string(*firstMatchIdx):"?") +
                    " firstALRM=" + (firstAlrmIdx?std::to_string(*firstAlrmIdx):"?"));

        bool alrmSeenY = false;
        for (auto& v : yicr.value) if (v && (*v & kIcrAlrm)) alrmSeenY = true;
        // F4: the line marks between the marker after the TODLO write (MK_87_T1Y) and the store of the T1Y read that
        // follows it immediately (no delay loop between them): a count of the lines independent of T1-A itself.
        auto mt1y = mark(cy.log, 0x0C1008), t1y_store = mark(cy.log, kResults+16);
        int linesY = (ancy.ok && mt1y && t1y_store) ? count_lines_between(ancy, *mt1y, *t1y_store) : -1;
        bool runY_ok = cy.loaded && cy.done.has_value() && linesY >= 0 &&
                       std::abs(int(t1y - 0x001000) - linesY) <= 1 && !alrmSeenY;
        emit(checks, gated, runY_ok, "P8.7-RUNY", "P8.7-RUNY [p344,p349,p354]: T1-A equals the number of line marks (F4) "
             "between the marker after the TODLO write and the T1 read's own store, +-1; no "
             "ALRM flag appears (CIAB, from reset: the alarm holds its reset value)", why_gate);
        checks.note("P8.7 CIAB T0(X)=" + std::to_string(t0x) + " T1X=" + std::to_string(t1x) + " T0(Y)=" + std::to_string(t0y) +
                    " T1Y=" + std::to_string(t1y) + " linesX=" + std::to_string(linesX) +
                    " linesY=" + std::to_string(linesY));

        // CIAA: each run from reset (SPECIFICATION.md P8.7, "Stimulus, CIAA"); the CRB value of the TOD writes from the
        // parameter block. Selectors: CRB<-$00 for run X's writes (P8.7-CIAA-RUNX), CRB<-$80 for run Y's (P8.7-CIAA-RUNY).
        Capture ax = run_capture_param(dir+"/p87_ciaa_x.bin", 0x0C2000, sel("P8.7-CIAA-RUNX") ? 0x00 : 0x80, kRunBudget, pk8);
        Capture ay = run_capture_param(dir+"/p87_ciaa_y.bin", 0x0C2000, sel("P8.7-CIAA-RUNY") ? 0x80 : 0x00, kRunBudget, pk8);
        uint32_t t0a = long_of(ax,0,1), t1xa = long_of(ax,2,3), t0ya = long_of(ay,0,1), t1ya = long_of(ay,4,5);
        bool ciaa_x_ok = ax.loaded && ax.done.has_value() &&
                         (int64_t(t1xa) - int64_t(t0a)) >= 0 && (int64_t(t1xa) - int64_t(t0a)) <= 2;
        bool ciaa_y_ok = ay.loaded && ay.done.has_value() &&
                         (int64_t(t1ya) - int64_t(0x001000)) >= 0 && (int64_t(t1ya) - int64_t(0x001000)) <= 2;
        checks.expect(ciaa_x_ok, "P8.7-CIAA-RUNX [p339,p344,p349]: run X (CRB<-$80), T1-T0 in [0,2]");
        checks.expect(ciaa_y_ok, "P8.7-CIAA-RUNY [p339,p344,p349]: run Y (CRB<-$00), T1-A in [0,2]");
        checks.note("P8.7 CIAA T0(X)=" + std::to_string(t0a) + " T1X=" + std::to_string(t1xa) + " T0(Y)=" + std::to_string(t0ya) +
                    " T1Y=" + std::to_string(t1ya));
        pv.p87_x = join({vof(gated, runX_ok), vof(true, ciaa_x_ok)});
        pv.p87_y = join({vof(gated, runY_ok), vof(true, ciaa_y_ok)});
    }

    // ---- P5 (phi2 is the E clock, one tenth of the CPU clock) -------------
    struct P5aResult { bool loaded=false; uint16_t v=0,v2=0,i=0; uint64_t mkB=0,mkD=0; uint64_t t_cpu=0; bool anc_ok=false; };
    auto run_p5a = [&](const std::string& bin) {
        P5aResult r;
        // D2's own delay (SPECIFICATION.md P5, tau2~=550000*T_CPU) needs a
        // budget well above the package's usual kRunBudget.
        Capture c = run_capture(bin, 20'000'000, {kResults+8,kResults+10,kResults+12});
        r.loaded = c.loaded && c.done.has_value();
        if (!r.loaded) return r;
        r.v = pk(c,0); r.v2 = pk(c,1); r.i = pk(c,2);
        auto b = mark(c.log, 0x0C1004), d = mark(c.log, 0x0C1008);
        if (b) r.mkB = *b; if (d) r.mkD = *d;
        Anchor a = measure_anchor(c.log);
        r.anc_ok = a.ok; r.t_cpu = a.t_cpu;
        return r;
    };
    struct P5Summary { bool delta_ok=false; bool pre51_ok=false, pre52_ok=false; double delta1=0, delta2=0, tau1=0, tau2=0; };
    P5Summary p5_ta_summary, p5_tb_summary;
    std::array<bool, 4> p5_delta_ok{}, p5_pre_ok{};   // per instance: leg (a)'s comparator, and Pre5.1 and Pre5.2
    {
        auto grade_p5a_instance = [&](const std::string& d1bin, const std::string& d2bin, uint16_t icrmask) {
            P5Summary s;
            P5aResult r1 = run_p5a(d1bin), r2 = run_p5a(d2bin);
            if (!r1.loaded || !r2.loaded || !r1.anc_ok || !r2.anc_ok) return s;
            double t_e = 10.0 * double(r2.t_cpu);
            s.tau1 = double(r1.mkD) - double(r1.mkB);
            s.tau2 = double(r2.mkD) - double(r2.mkB);
            s.delta1 = double(0xFFFF - r1.v);
            s.delta2 = double(0xFFFF - r2.v);
            double predicted = std::round((s.tau2 - s.tau1) / t_e);
            s.delta_ok = std::fabs((s.delta2 - s.delta1) - predicted) <= 1.0;
            s.pre51_ok = !(r1.i & icrmask) && !(r2.i & icrmask);
            s.pre52_ok = (r1.v2 == r1.v) && (r2.v2 == r2.v);
            return s;
        };
        // One stimulus change at a time on the CIAB/timer B instance: R-LATCH($1000) in the D2 run
        // (P5-PRE51), no stop write in the D1 run (P5-PRE52), no start write in the D2 run (P5-LEGA).
        std::string ciab_tb_d1 = dir + (sel("P5-PRE52") ? "/p5a_ciab_tb_d1_pre52.bin" : "/p5a_ciab_tb_d1.bin");
        std::string ciab_tb_d2 = dir + (sel("P5-PRE51") ? "/p5a_ciab_tb_d2_pre51.bin"
                                        : sel("P5-LEGA") ? "/p5a_ciab_tb_d2_nostart.bin"
                                        : "/p5a_ciab_tb_d2.bin");
        P5Summary a_ciaa_ta = grade_p5a_instance(dir+"/p5a_ciaa_ta_d1.bin", dir+"/p5a_ciaa_ta_d2.bin", kIcrTa);
        P5Summary a_ciaa_tb = grade_p5a_instance(dir+"/p5a_ciaa_tb_d1.bin", dir+"/p5a_ciaa_tb_d2.bin", kIcrTb);
        P5Summary a_ciab_ta = grade_p5a_instance(dir+"/p5a_ciab_ta_d1.bin", dir+"/p5a_ciab_ta_d2.bin", kIcrTa);
        P5Summary a_ciab_tb = grade_p5a_instance(ciab_tb_d1, ciab_tb_d2, kIcrTb);
        p5_ta_summary = a_ciaa_ta; // representative for the TA-instance id row (P10)
        p5_tb_summary = a_ciab_tb;

        {
            const P5Summary* as[4] = {&a_ciaa_ta, &a_ciaa_tb, &a_ciab_ta, &a_ciab_tb};
            for (int i = 0; i < 4; ++i) { p5_delta_ok[i] = as[i]->delta_ok; p5_pre_ok[i] = as[i]->pre51_ok && as[i]->pre52_ok; }
        }
        bool delta_ok_all = a_ciaa_ta.delta_ok && a_ciaa_tb.delta_ok && a_ciab_ta.delta_ok && a_ciab_tb.delta_ok;
        bool pre51_ok = a_ciaa_ta.pre51_ok && a_ciaa_tb.pre51_ok && a_ciab_ta.pre51_ok && a_ciab_tb.pre51_ok;
        checks.expect(pre51_ok, "P5-PRE51 [SPECIFICATION.md P5]: no underflow happens in the window (i "
                      "flag clear), for all four timers, in both D1 and D2 runs");
        bool pre52_ok = a_ciaa_ta.pre52_ok && a_ciaa_tb.pre52_ok && a_ciab_ta.pre52_ok && a_ciab_tb.pre52_ok;
        checks.expect(pre52_ok, "P5-PRE52 [SPECIFICATION.md P5]: the counter reads the same value twice "
                      "while stopped, for all four timers, in both D1 and D2 runs");
        emit(checks, pre51_ok && pre52_ok, delta_ok_all, "P5-LEGA", "P5-LEGA [p348,p349,p353]: Delta2-Delta1 "
             "= round((tau2-tau1)/(10*T_CPU)) +-1, for all four timers", "P5-PRE51 or P5-PRE52 failed");
        checks.note("P5-LEGA CIAA/TA tau1=" + std::to_string(a_ciaa_ta.tau1) + " tau2=" + std::to_string(a_ciaa_ta.tau2) +
                    " delta1=" + std::to_string(a_ciaa_ta.delta1) + " delta2=" + std::to_string(a_ciaa_ta.delta2));
        checks.note("P5-LEGA CIAA/TB tau1=" + std::to_string(a_ciaa_tb.tau1) + " tau2=" + std::to_string(a_ciaa_tb.tau2) +
                    " delta1=" + std::to_string(a_ciaa_tb.delta1) + " delta2=" + std::to_string(a_ciaa_tb.delta2));
        checks.note("P5-LEGA CIAB/TA tau1=" + std::to_string(a_ciab_ta.tau1) + " tau2=" + std::to_string(a_ciab_ta.tau2) +
                    " delta1=" + std::to_string(a_ciab_ta.delta1) + " delta2=" + std::to_string(a_ciab_ta.delta2));
        checks.note("P5-LEGA CIAB/TB tau1=" + std::to_string(a_ciab_tb.tau1) + " tau2=" + std::to_string(a_ciab_tb.tau2) +
                    " delta1=" + std::to_string(a_ciab_tb.delta1) + " delta2=" + std::to_string(a_ciab_tb.delta2));
        checks.record("opus:F-signals.c2 declined: the claim says that phi2 is connected to the 680x0 \"E\" clock; P5-LEGA and P5-LEGB measure the rate at which a timer counting phi2 decrements, which a timer clock of its own, divided by ten, would also give; whether phi2 is the E clock itself is not observable at this boundary");
        checks.record("sol:F-signals.c70 declined: the claim says that phi2 is connected to the 680x0 \"E\" clock; P5-LEGA and P5-LEGB measure the rate at which a timer counting phi2 decrements, which a timer clock of its own, divided by ten, would also give; whether phi2 is the E clock itself is not observable at this boundary");

        // The CNT contrasting case (CIAB/timer B only) is the page's own, selected through NMH_MUTATE.
        if (sel("P5-CONTRAST")) {
            P5aResult cnt1 = run_p5a(dir+"/p5a_ciab_tb_d1_cnt.bin"), cnt2 = run_p5a(dir+"/p5a_ciab_tb_d2_cnt.bin");
            bool cnt_ok = cnt1.loaded && cnt2.loaded && (0xFFFF-cnt1.v) == 0 && (0xFFFF-cnt2.v) == 0;
            checks.expect(cnt_ok, "P5-CONTRAST [p348,p357]: counting CNT instead of phi2, with no key "
                          "sent, gives Delta1=Delta2=0 (CIAB/timer B)");
        }
    }

    // ---- P5 leg (b) ---------------------------------------------------------
    {
        struct P5bRun { LoopFrame frame; bool diffs_ok = false; };
        auto run_p5b = [&](const std::string& bin) {
            P5bRun r;
            Capture c = run_capture(bin, kRunBudget, {});
            SampleSeries s = read_series(c.log, kResults+8, 256);
            Anchor anc = measure_anchor(c.log);
            r.frame = loop_frame(c.loaded, c.done.has_value(), anc, s, 256);
            if (!r.frame.run_ok) return r;
            uint64_t t0 = 0; bool have = false;
            std::vector<std::pair<int64_t,uint16_t>> pts;
            for (size_t i = 0; i < s.size(); ++i) {
                if (!s.cycle[i] || !s.value[i]) continue;
                if (!have) { t0 = *s.cycle[i]; have = true; }
                pts.push_back({(int64_t)e_index(*s.cycle[i], t0, anc.t_cpu), *s.value[i]});
            }
            if (pts.size() != 256) return r;
            bool ok = true;
            for (size_t i = 0; i+1 < pts.size(); ++i) {
                int expect_diff = int(pts[i+1].first - pts[i].first);
                int actual_diff = (int(pts[i].second) - int(pts[i+1].second)) & 0xFF;
                if (actual_diff != (expect_diff & 0xFF)) { ok = false; break; }
            }
            r.diffs_ok = ok;
            return r;
        };
        // One stimulus change at a time on the CIAB/timer B run: a sampler with no CIA access (P5-PRE53),
        // 255 iterations (P5-PRE53-POP), a timer that is not started (P5-LEGB).
        std::string ciab_tb_bin = dir + (sel("P5-PRE53") ? "/p5b_ciab_tb_chip.bin"
                                         : sel("P5-PRE53-POP") ? "/p5b_ciab_tb_short.bin"
                                         : sel("P5-LEGB") ? "/p5b_ciab_tb_nostart.bin"
                                         : "/p5b_ciab_tb.bin");
        const char* b_names[4] = {"CIAA/TA", "CIAA/TB", "CIAB/TA", "CIAB/TB"};
        P5bRun b_runs[4] = {run_p5b(dir+"/p5b_ciaa_ta.bin"), run_p5b(dir+"/p5b_ciaa_tb.bin"),
                            run_p5b(dir+"/p5b_ciab_ta.bin"), run_p5b(ciab_tb_bin)};
        for (int i = 0; i < 4; ++i)   // one verdict per instance: legs (a) and (b) with Pre5.1, Pre5.2 and Pre5.3
            pv.p5[i] = vof(p5_pre_ok[i] && b_runs[i].frame.run_ok && b_runs[i].frame.full && b_runs[i].frame.holds,
                           p5_delta_ok[i] && b_runs[i].diffs_ok);
        bool pre53 = true, diffs_all = true;
        for (const auto& r : b_runs) { pre53 = pre53 && r.frame.run_ok && r.frame.full && r.frame.holds; diffs_all = diffs_all && r.diffs_ok; }
        checks.expect(pre53, "P5-PRE53 [SPECIFICATION.md P5]: for each of the four leg-(b) runs, F1's anchor holds, all "
                      "256 samples are present and the E-index frame is valid for the LO-read loop (P6: at most two "
                      "phases, 2*T_CPU apart)");
        for (int k = 0; k < 4; ++k)
            checks.note(std::string("P5-PRE53 ") + b_names[k] + ": samples=" + std::to_string(b_runs[k].frame.pop) + " " +
                        shape_text(b_runs[k].frame.shape));
        emit(checks, pre53, diffs_all, "P5-LEGB", "P5-LEGB [p348,p349,p353]: (v_i-v_(i+1)) mod 256 = "
             "e_(i+1)-e_i for every one of the 256 samples, for all four timers", "P5-PRE53 failed");
    }

    // ---- P6 (phi2 is the timing reference for bus transfers to a CIA) -----
    {
        // One stimulus change at a time: K1 runs 199 iterations (P6-PRE62), K1 reads a Chip-memory byte
        // (P6-K1K2), K3 alternates a read of CIAA PRB with a read of the Chip-memory byte (P6-K3), K3's body holds enough NOPs to make I3 a multiple of T_E
        // (P6-PRE61: three NOPs or five, whichever makes the measured I3 a multiple on this core).
        Capture c;
        if (sel("P6-PRE61")) {
            Capture c3 = run_capture(dir+"/p6_k123_n3.bin", kRunBudget, {});
            Capture c5 = run_capture(dir+"/p6_k123_n5.bin", kRunBudget, {});
            auto i3_of = [](const Capture& cc) -> std::pair<int64_t, uint64_t> {
                SampleSeries k3 = read_series(cc.log, kResults+808, 200);
                Anchor a = measure_anchor(cc.log);
                return {modal_interval(k3), a.t_cpu};
            };
            auto [i3a, ta] = i3_of(c3);
            c = (ta > 0 && i3a % (10 * (int64_t)ta) == 0) ? c3 : c5;
        } else {
            c = run_capture(dir + (sel("P6-PRE62") ? "/p6_k123_short.bin"
                                   : sel("P6-K1K2") ? "/p6_k123_k1chip.bin"
                                   : sel("P6-K3") ? "/p6_k123_k3mix.bin"
                                   : "/p6_k123.bin"), kRunBudget, {});
        }
        Anchor anc = measure_anchor(c.log);
        SampleSeries k1 = read_series(c.log, kResults+8, 200);
        SampleSeries k2 = read_series(c.log, kResults+408, 200);
        SampleSeries k3 = read_series(c.log, kResults+808, 200);
        int64_t i3 = modal_interval(k3);
        bool pre61_ok = anc.ok && i3 > 0 && (i3 % (10*(int64_t)anc.t_cpu) != 0);
        checks.expect(pre61_ok, "P6-PRE61 [SPECIFICATION.md P6]: the measured I3 (K3's own iteration "
                      "time) is not a multiple of T_E");

        LoopFrame f1 = loop_frame(c.loaded, c.done.has_value(), anc, k1, 200);
        LoopFrame f2 = loop_frame(c.loaded, c.done.has_value(), anc, k2, 200);
        LoopFrame f3 = loop_frame(c.loaded, c.done.has_value(), anc, k3, 200);

        // The other loops P6 checks, each from one representative run: P4's PRB and ICR sample streams
        // (CIAB/timer B, L=20), P5 leg (b)'s LO-read loop (CIAB/timer B), P9 leg W's sampler (CIAB/timer B, L=40).
        // One stimulus change at a time: P4's ICR stores or its PRB stores spread over four phases
        // (P6-P4LOOP-ICR, P6-P4LOOP-PRB), a loop with no CIA access in P5 leg (b) (P6-P5BLOOP) and in P9 (P6-P9LOOP).
        Capture p4c = run_capture(dir + (sel("P6-P4LOOP-ICR") ? "/p4_ciab_tb_l20_yspread.bin"
                                         : sel("P6-P4LOOP-PRB") ? "/p4_ciab_tb_l20_xspread.bin"
                                         : "/p4_ciab_tb_l20.bin"), kRunBudget, {});
        Anchor anc4 = measure_anchor(p4c.log);
        SampleSeries p4prb = read_series(p4c.log, kResults+8, 400);
        SampleSeries p4icr = read_series(p4c.log, kResults+8+400*2, 400);
        LoopFrame f4a = loop_frame(p4c.loaded, p4c.done.has_value(), anc4, p4prb, 400);
        LoopFrame f4b = loop_frame(p4c.loaded, p4c.done.has_value(), anc4, p4icr, 400);

        Capture p5bc = run_capture(dir + (sel("P6-P5BLOOP") ? "/p5b_ciab_tb_chip.bin" : "/p5b_ciab_tb.bin"), kRunBudget, {});
        Anchor anc5 = measure_anchor(p5bc.log);
        SampleSeries p5blo = read_series(p5bc.log, kResults+8, 256);
        LoopFrame f5 = loop_frame(p5bc.loaded, p5bc.done.has_value(), anc5, p5blo, 256);

        Capture p9c = run_capture(dir + (sel("P6-P9LOOP") ? "/p9_legw_l40_chip.bin" : "/p9_legw_l40.bin"), kRunBudget, {});
        Anchor anc9 = measure_anchor(p9c.log);
        SampleSeries p9s = read_series(p9c.log, kResults+8, 200);
        LoopFrame f9 = loop_frame(p9c.loaded, p9c.done.has_value(), anc9, p9s, 200);

        bool pre62_k = f1.run_ok && f1.full && f2.full && f3.full;
        bool pre62_4 = f4a.run_ok && f4a.full && f4b.full;
        bool pre62_5 = f5.run_ok && f5.full;
        bool pre62_9 = f9.run_ok && f9.full;
        pv.p6_p9loop_ok = pre62_9 && f9.holds;   // leg W's E-index frame (P9 Preconditions)
        checks.expect(pre62_k && pre62_4 && pre62_5 && pre62_9,
                      "P6-PRE62 [SPECIFICATION.md P6]: every loop run completed with F1's anchor and stored its full "
                      "sample population: K1, K2 and K3 200 each, P4's PRB and ICR streams 400 each, P5 leg (b)'s LO "
                      "loop 256, P9 leg W's sampler 200");

        emit(checks, pre62_k, f1.holds && f2.holds, "P6-K1K2", "P6-K1K2 [p353,p354,p196]: K1 (CIAA PRB) and K2 (CIAB PRB) "
             "each take at most two phases, 2*T_CPU apart", "P6-PRE62 failed for the K run");
        emit(checks, pre62_k && pre61_ok, five_phases(f3.shape), "P6-K3", "P6-K3 [p196]: K3 (a Chip-memory read, no CIA access) "
             "takes exactly the five phases 0, 2, 4, 6 and 8*T_CPU mod T_E, so the 2*T_CPU band does not hold for it",
             "P6-PRE61 or P6-PRE62 failed for the K run");
        checks.note("P6 K1 " + shape_text(f1.shape) + "; K2 " + shape_text(f2.shape) + "; K3 " + shape_text(f3.shape) +
                    "; I3=" + std::to_string(i3) + " clk_sys, T_E=" + std::to_string(10*anc.t_cpu));

        emit(checks, pre62_4, f4a.holds && f4b.holds, "P6-P4LOOP", "P6-P4LOOP [SPECIFICATION.md P6]: P4's own sample loop "
             "(CIAB/timer B, L=20) meets the same frame in both of its store streams, PRB and ICR",
             "P6-PRE62 failed for the P4 run");
        checks.note("P6-P4LOOP PRB " + shape_text(f4a.shape) + "; ICR " + shape_text(f4b.shape));
        emit(checks, pre62_5, f5.holds, "P6-P5BLOOP", "P6-P5BLOOP [SPECIFICATION.md P6]: P5 leg (b)'s own LO-read loop "
             "(CIAB/timer B) meets the same frame", "P6-PRE62 failed for the P5 run");
        emit(checks, pre62_9, f9.holds, "P6-P9LOOP", "P6-P9LOOP [SPECIFICATION.md P6]: P9 leg W's own sample loop (P2 leg "
             "S's mechanics, CIAB/timer B, L=40) meets the same frame", "P6-PRE62 failed for the P9 run");
    }

    // ---- P10 (claims that several propositions grade together) ------------
    // Each row is its own assertion, one per claim id (or id group), the join of the
    // verdicts SPECIFICATION.md's own P10 table names for it (SPECIFICATION.md P10: "no
    // one of them carries its id"). Every member is a verdict already conjoined with its
    // own preconditions (a member whose precondition failed, or whose instance has no
    // live read-back, is NOTGRADED), one per timer instance where the row names
    // instances and one per setting where it names settings. The row "passes when every
    // assertion it lists passed ...; it fails when any of them failed; otherwise it is
    // not graded" (SPECIFICATION.md P10).
    {
        using Members = std::vector<std::pair<V, std::string>>;
        static const char* const inst_tag[4] = {"CIAA/TA", "CIAA/TB", "CIAB/TA", "CIAB/TB"};
        auto add4 = [&](Members& m, const PropVerdicts::V4& a, const std::string& name, std::initializer_list<int> idx) {
            for (int i : idx) m.push_back({a[i], name + "(" + inst_tag[i] + ")"});
        };
        auto row = [&](const std::string& label, const Members& members) {
            bool any_fail = false; std::string notgraded, listing;
            for (const auto& [v, name] : members) {
                if (v == V::FAIL) any_fail = true;
                else if (v == V::NOTGRADED) { if (!notgraded.empty()) notgraded += ", "; notgraded += name; }
                listing += (listing.empty() ? "" : "; ") + name + (v == V::PASS ? " PASS" : v == V::FAIL ? " FAIL" : " NOT GRADED");
            }
            checks.note(label.substr(0, label.find(' ')) + " members: " + listing);
            if (any_fail) { checks.expect(false, label); return; }
            if (!notgraded.empty()) { checks.record(label + ": not graded -- " + notgraded + " not graded"); return; }
            checks.expect(true, label);
        };
        const std::initializer_list<int> all4 = {0, 1, 2, 3}, ta = {0, 2}, tb = {1, 3};
        // the leg-W width, conjoined with P6's E-index frame for leg S's loop (P9 Preconditions)
        const V p9_legw = pv.p6_p9loop_ok ? pv.p9_legw : V::NOTGRADED;

        Members m1;
        add4(m1, pv.p2, "P2", all4); add4(m1, pv.p3, "P3", all4); add4(m1, pv.p4, "P4", all4);
        row("P10-sol:F-timers.c13 [p342: sol:F-timers.c13]: one bit selects toggle or a one-cycle positive pulse on every "
            "underflow; the toggle output is set high on start and low by RES (P2, P3, P4, all four "
            "instances each)", m1);
        Members m2;
        add4(m2, pv.p3, "P3", all4); add4(m2, pv.p4, "P4", all4);
        row("P10-opus:F-timers.c12 [p342: opus:F-timers.c12, opus:F-timers.c13]: one bit selects the output while PB on/off "
            "is on; every underflow toggles it or gives a one-cycle positive pulse (P3, P4, all four "
            "instances each)", m2);
        Members m3;
        add4(m3, pv.p3, "P3", ta); add4(m3, pv.p4, "P4", ta);
        row("P10-sol:F-cra.c16 [p349: sol:F-cra.c16]: CRA OUTMODE: 0 pulse, 1 toggle (P3, P4, timer-A instances)", m3);
        Members m4;
        add4(m4, pv.p3, "P3", tb); add4(m4, pv.p4, "P4", tb);
        row("P10-opus:F-crb.c3 [p349,p350: opus:F-crb.c3, sol:F-crb.c4, sol:F-crb.c13]: CRB OUTMODE: 0 pulse, 1 toggle "
            "(P3, P4, timer-B instances)", m4);
        Members m5;
        add4(m5, pv.p5, "P5", ta); m5.push_back({pv.p75, "P7.5"}); m5.push_back({pv.p75_ciaa, "P7.5-CIAA-INMODE"});
        row("P10-sol:F-cra.c13 [p349: sol:F-cra.c13]: CRA INMODE: 0 phi2, 1 CNT (P5, timer-A instances, P7.5 on CIAB and "
            "P7.5-CIAA-INMODE)", m5);
        Members m6;
        add4(m6, pv.p5, "P5", tb); m6.push_back({pv.p865_01, "P8.65-01"}); m6.push_back({pv.p865_10, "P8.65-10"});
        m6.push_back({pv.p865_11, "P8.65-11"});
        row("P10-sol:F-crb.c7 [p349,p350: sol:F-crb.c7, sol:F-crb.c10]: CRB INMODE: 00 phi2, 01 CNT, 10 timer A "
            "underflows, 11 timer A underflows while CNT is high (P5, timer-B instances, and P8.65 "
            "modes 01, 10 and 11)", m6);

        // The bitmap claims split one row per id (each over its own settings), not one row over "every field"
        // (SPECIFICATION.md P10: LOAD has no 0-setting, Section 0). A setting's verdict joins only what grades that setting.
        Members m7;
        m7.push_back({pv.p70_stop, "P7.0-STOP"}); add4(m7, pv.p1_control, "P1-CONTROL", ta); add4(m7, pv.p4, "P4", ta);
        m7.push_back({pv.p73_cont, "P7.3-CONT"}); m7.push_back({pv.p74_read, "P7.4-READ"}); add4(m7, pv.p5, "P5", ta);
        m7.push_back({pv.p76_ciaa, "P7.6-CIAA"}); m7.push_back({pv.p77, "P7.7"});
        row("P10-opus:F-cra.c12 [p349: opus:F-cra.c12]: every 0-setting field of the CRA bitmap (P7.0 START=0; P1-CONTROL "
            "PBON=0; P4 OUTMODE=0, timer-A instances; P7.3 RUNMODE=0; P7.4 LOAD reads 0; P5 INMODE=0, "
            "timer-A instances; P7.6-CIAA SPMODE=0; P7.7 bit 7 unused)", m7);
        Members m8;
        m8.push_back({pv.p70_start, "P7.0-START"}); add4(m8, pv.p1_main, "P1-MAIN", ta); add4(m8, pv.p3, "P3", ta);
        m8.push_back({pv.p73_oneshot, "P7.3-ONESHOT"}); m8.push_back({pv.p74, "P7.4"}); m8.push_back({pv.p75, "P7.5"});
        m8.push_back({pv.p75_ciaa, "P7.5-CIAA-INMODE"}); m8.push_back({pv.p76_ciab, "P7.6-CIAB"});
        row("P10-opus:F-cra.c13 [p349: opus:F-cra.c13]: every 1-setting field of the CRA bitmap (P7.0 START=1; P1 PBON=1, "
            "timer-A instances; P3 OUTMODE=1, timer-A instances; P7.3 RUNMODE=1; P7.4 LOAD=1; P7.5 and "
            "P7.5-CIAA-INMODE INMODE=1; P7.6-CIAB SPMODE=1)", m8);
        Members m9;
        m9.push_back({pv.p80_stop, "P8.0-STOP"}); add4(m9, pv.p1_control, "P1-CONTROL", tb); add4(m9, pv.p4, "P4", tb);
        m9.push_back({pv.p83_cont, "P8.3-CONT"}); m9.push_back({pv.p84_read, "P8.4-READ"}); add4(m9, pv.p5, "P5", tb);
        m9.push_back({pv.p87_y, "P8.7-RUNY"});
        row("P10-opus:F-crb.c13 [p350: opus:F-crb.c13]: every 0-setting field of the CRB bitmap (P8.0 START=0; P1-CONTROL "
            "PBON=0; P4 OUTMODE=0, timer-B instances; P8.3 RUNMODE=0; P8.4 LOAD reads 0; P5 INMODE=00, "
            "timer-B instances; P8.7 run Y ALARM=0)", m9);
        Members m10;
        m10.push_back({pv.p80_start, "P8.0-START"}); add4(m10, pv.p1_main, "P1-MAIN", tb); add4(m10, pv.p3, "P3", tb);
        m10.push_back({pv.p83_oneshot, "P8.3-ONESHOT"}); m10.push_back({pv.p84, "P8.4"});
        m10.push_back({pv.p865_01, "P8.65-01"}); m10.push_back({pv.p87_x, "P8.7-RUNX"});
        row("P10-opus:F-crb.c14 [p350: opus:F-crb.c14]: every 1-setting field of the CRB bitmap (P8.0 START=1; P1 PBON=1, "
            "timer-B instances; P3 OUTMODE=1, timer-B instances; P8.3 RUNMODE=1; P8.4 LOAD=1; P8.65 "
            "mode 01 (INMODE=01, CNT); P8.7 run X ALARM=1)", m10);

        Members m11;
        add4(m11, pv.p4, "P4", all4); m11.push_back({pv.p9_legf, "P9-LEGF"});
        m11.push_back({pv.p9_legf_trans, "P9-LEGF-TRANSITIONS"}); m11.push_back({p9_legw, "P9-LEGW"});
        m11.push_back({pv.p865_10, "P8.65-10"});
        row("P10-opus:F-timers.c6 [p341: opus:F-timers.c6, sol:F-timers.c8]: the modes allow long time delays, variable "
            "width pulses, pulse trains, and variable frequency waveforms; the latch is a divider of the "
            "input clocking frequency (P4 pulse trains; P9 leg F frequency/divider and its transitions, and leg W width; "
            "P8.65 mode 10 long delays)", m11);
    }

    // ---- Section 0 declines, printed as RECORDED lines -------------------
    checks.record("opus:F-portsig.c47 declined: \"PC...not used\" (CIAB): no output this package sees shows a PC pin on either CIA, and \"not used\" describes no behaviour");
    checks.record("opus:F-signals.c1 declined: the claim says phi2 is a \"TTL compatible input\", an electrical property of an input pin that nothing a test sees depends on");
    checks.record("sol:F-signals.c69 declined: the claim says phi2 is a \"TTL compatible input\", an electrical property of an input pin that nothing a test sees depends on");

    // ---- EP1, EP2, EP3 and F1's anchor, asserted on every capture the package
    // consumed (SPECIFICATION.md 1.2: "Every run asserts all three"). The
    // captures are recorded as they are made (register_run). EP2 is graded on
    // the runs that completed.
    {
        const auto& runs = run_registry();
        std::set<std::string> bins;
        size_t n_ep1 = 0, n_done = 0, n_f1 = 0, n_ep2_graded = 0, n_ep2 = 0, n_loaded = 0;
        std::string bad_ep1, bad_done, bad_f1, bad_ep2;
        auto add = [](std::string& list, const std::string& bin) {
            std::string shortname = bin.substr(bin.find_last_of('/') + 1);
            if (list.find(shortname) == std::string::npos) list += (list.empty() ? "" : ", ") + shortname;
        };
        for (const auto& r : runs) {
            bins.insert(r.bin);
            if (r.loaded) ++n_loaded;
            if (r.ep1) ++n_ep1; else add(bad_ep1, r.bin);
            if (r.done) ++n_done; else add(bad_done, r.bin);
            if (r.f1) ++n_f1; else add(bad_f1, r.bin);
            if (r.ep2_graded) { ++n_ep2_graded; if (r.ep2) ++n_ep2; else add(bad_ep2, r.bin); }
        }
        checks.note("per-run population: " + std::to_string(runs.size()) + " captures of " +
                    std::to_string(bins.size()) + " programs, " + std::to_string(n_loaded) + " loaded");
        checks.expect(!runs.empty() && n_ep1 == runs.size(),
            "EP1-PER-RUN [SPECIFICATION.md 1.2]: EP1 holds on every capture the package consumed");
        if (!bad_ep1.empty()) checks.note("EP1-PER-RUN failed on: " + bad_ep1);
        checks.expect(!runs.empty() && n_done == runs.size(),
            "EP3-PER-RUN [SPECIFICATION.md 1.2]: every capture the package consumed reaches DONE and has a final marker");
        if (!bad_done.empty()) checks.note("EP3-PER-RUN failed on: " + bad_done);
        checks.expect(!runs.empty() && n_f1 == runs.size(),
            "F1-ANCHOR-PER-RUN [p194,p195,p23]: F1's own anchor holds on every capture the package consumed");
        if (!bad_f1.empty()) checks.note("F1-ANCHOR-PER-RUN failed on: " + bad_f1);
        if (n_ep2_graded == runs.size() && !runs.empty()) {
            checks.expect(n_ep2 == n_ep2_graded,
                "EP2-PER-RUN [SPECIFICATION.md 1.2]: between the first and last marker, every capture the package "
                "consumed shows only CPU accesses and memory-refresh accesses");
            if (!bad_ep2.empty()) checks.note("EP2-PER-RUN failed on: " + bad_ep2);
        } else {
            checks.record("EP2-PER-RUN [SPECIFICATION.md 1.2]: not graded -- " + std::to_string(runs.size() - n_ep2_graded) +
                          " of " + std::to_string(runs.size()) + " captures did not complete");
        }
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
