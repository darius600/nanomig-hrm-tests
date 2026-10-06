// test_aa_integ_s1.cpp
//
// aa-integ-s1: see SPECIFICATION.md. BPLCON1 horizontal scroll (P1-P5), the bit-plane fetch increment and the modulos (P7-P9), BYPASS (P6), LPEN, the joystick and mouse registers and the UART break (P10-P12) and the disk DMA pointer and the DUAL-mode secondary beam counter (P13-P15).
//
// Every run selects the AA chip set with chipset_config = $10 (SPECIFICATION.md
// section 0). Both builds, fx68k and TG68K, grade every proposition,
// precondition and anchor: the input does not depend on the CPU core.
//
// Programs (SPECIFICATION.md section 4): prog_bars.s and prog_bars_midrun.s take their configuration block from chip RAM, which the test pokes before boot() (P1-P5); prog_regs.s writes a register list the test pokes (P6-P9, the P15 setup). The files named *_nofix, *_k, *_c,
// *_same, *_late, *_var, *_early and *_pad* are the contrast builds the MUTATION records name.
//
// NMH_MUTATE=<selector> runs one contrast: only the proposition the selector
// names runs, with the one stimulus the record describes, through the same
// comparators; the record's label prints FAIL (red) or, for a live input
// through a comparator that is red without it, PASS (green). `make test`
// leaves NMH_MUTATE unset and runs none.
//
// MUTATION R1 stimulus red NMH_MUTATE=R1: the calibration run has no plane data, so no COLOR01 pixel appears to measure; TG68K FAIL, fx68k FAIL.
// MUTATION P2 stimulus red NMH_MUTATE=P2: the three reference runs have no plane-1 data; TG68K FAIL, fx68k FAIL.
// MUTATION P3 stimulus red NMH_MUTATE=P3-PRE: the reference run has no plane-1 data; TG68K FAIL, fx68k FAIL. Also printing FAIL: P3-0E0F, P3-0F0F, P3-AGG, P3-E0F0, P3-F0F0.
// MUTATION P4 stimulus red NMH_MUTATE=P4-PRE: runs (a) and (b) have no plane-1 data; TG68K FAIL, fx68k FAIL. Also printing FAIL: P4a-D1, P4a-D2.
// MUTATION P1 stimulus red NMH_MUTATE=P1: the test run writes $0000 to BPLCON1 ($DFF102) and $0033 to BPLCON4 ($DFF10C) in place of $0033 to BPLCON1; TG68K FAIL, fx68k FAIL.
// MUTATION P3-0E0F stimulus red NMH_MUTATE=P3: chipset_config = $08 (the ECS chip set) in the four test runs instead of $10; TG68K FAIL, fx68k FAIL.
// MUTATION P3-0F0F stimulus red NMH_MUTATE=P3: chipset_config = $08 (the ECS chip set) in the four test runs instead of $10; TG68K FAIL, fx68k FAIL.
// MUTATION P3-E0F0 stimulus red NMH_MUTATE=P3: chipset_config = $08 (the ECS chip set) in the four test runs instead of $10; TG68K FAIL, fx68k FAIL.
// MUTATION P3-F0F0 stimulus red NMH_MUTATE=P3: chipset_config = $08 (the ECS chip set) in the four test runs instead of $10; TG68K FAIL, fx68k FAIL.
// MUTATION P3-AGG stimulus red NMH_MUTATE=P3: chipset_config = $08 (the ECS chip set) in the four test runs instead of $10; TG68K FAIL, fx68k FAIL.
// MUTATION P4a-D1 stimulus red NMH_MUTATE=P4a-D1: the CPU writes BPLCON1 = $0200 (PF1H1) before F3 instead of leaving the reset state; F5 stays the reference run's; TG68K FAIL, fx68k FAIL.
// MUTATION P4a-D2 stimulus red NMH_MUTATE=P4a-D2: the CPU writes BPLCON1 = $2000 (PF2H1) before F3 instead of leaving the reset state; F5 stays the reference run's; TG68K FAIL, fx68k FAIL.
// MUTATION P4b-C1 stimulus green NMH_MUTATE=P4b-C1: live input through the same comparator: the CPU writes BPLCON1 = $0000 before F3 instead of leaving the reset state; TG68K PASS, fx68k PASS.
// MUTATION P4b-C2 stimulus green NMH_MUTATE=P4b-C2: live input through the same comparator: the CPU writes BPLCON1 = $0000 before F3 instead of leaving the reset state; TG68K PASS, fx68k PASS.
// MUTATION P5 stimulus green NMH_MUTATE=P5: live input through the same line: P4 runs (a) and (b) write BPLCON1 = $0000 before F3 instead of leaving the reset state; TG68K PASS, fx68k PASS.
// MUTATION UNIT-FIXTURE stimulus red NMH_MUTATE=UNIT-FIXTURE: the unit run loads prog_regs_nofix.bin, which stores no $BEEF; TG68K FAIL, fx68k FAIL.
// MUTATION UNIT-AA stimulus red NMH_MUTATE=UNIT-AA: chipset_config = $08 (the ECS chip set) in the unit run; TG68K FAIL, fx68k FAIL.
// MUTATION UNIT-R2 stimulus red NMH_MUTATE=UNIT-R2: the unit run writes BEAMCON0 = $00A0 (VARBEAMEN) and HTOTAL = $0040: lines of 65 CCK; TG68K FAIL, fx68k FAIL.
// MUTATION P6-R4 stimulus red NMH_MUTATE=P6-R4: COLOR01 = $0500 is written with LOCT=0 and $0A00 with LOCT=1, the two writes exchanged; TG68K FAIL, fx68k FAIL.
// MUTATION P6-FIXTURE stimulus red NMH_MUTATE=P6-FIXTURE: the B0/T_id run loads prog_regs_nofix.bin; TG68K FAIL, fx68k FAIL.
// MUTATION P6-F3 stimulus red NMH_MUTATE=P6-F3: the B0/T_id run ends 3,300,000 cycles after boot(), before F3 is complete; TG68K FAIL, fx68k FAIL.
// MUTATION P6-R1 stimulus red NMH_MUTATE=P6-R1: the B1/T_id run writes BEAMCON0 = $0000 (NTSC); TG68K FAIL, fx68k FAIL.
// MUTATION P6-POP stimulus red NMH_MUTATE=P6-POP: the B0/T_id run writes DMACON = $8280 (BPLEN clear); TG68K FAIL, fx68k FAIL. Also printing FAIL: P6-RED16.
// MUTATION P6-TBL stimulus red NMH_MUTATE=P6-TBL: the B0/T_x run loads table T_id; TG68K FAIL, fx68k FAIL.
// MUTATION P6-RED16 stimulus red NMH_MUTATE=P6-RED16: the B0/T_id run loads a T_id with the red nibble limited to 0-7; TG68K FAIL, fx68k FAIL.
// MUTATION P6-SPR stimulus red NMH_MUTATE=P6-SPR: the B0/T_id run writes DMACON = $8380 (SPREN clear); TG68K FAIL, fx68k FAIL.
// MUTATION P6-SCROLL stimulus red NMH_MUTATE=P6-SCROLL: the B0/T_id run writes BPLCON1 = $0000; TG68K FAIL, fx68k FAIL.
// MUTATION P6-I stimulus red NMH_MUTATE=P6-I: the B1/T_id run writes BPLCON1 = $0000 while B0/T_id keeps $0033 (the scroll clause); TG68K FAIL, fx68k FAIL.
// MUTATION P6-I stimulus red NMH_MUTATE=P6-I-PRIO: the B1/T_id run writes BPLCON2 = $0000 (playfields in front of the sprites) while B0/T_id keeps $0024 (the priority clause); TG68K FAIL, fx68k FAIL.
// MUTATION P6-II stimulus green NMH_MUTATE=P6-II: live input through the same comparator: the B1/T_x run is run as B0/T_id (table T_id, BYPASS clear); TG68K PASS, fx68k PASS.
// MUTATION P6-AGG stimulus green NMH_MUTATE=P6-AGG: live input through the same line: the B1/T_x run is run as B0/T_id (table T_id, BYPASS clear); TG68K PASS, fx68k PASS.
// MUTATION P7-F0000 stimulus red NMH_MUTATE=P7-F0000: the FMODE = $0000 run writes $0003; TG68K FAIL, fx68k FAIL.
// MUTATION P7-F0001 stimulus red NMH_MUTATE=P7-F0001: the FMODE = $0001 run writes $0003; TG68K FAIL, fx68k FAIL.
// MUTATION P7-F0002 stimulus red NMH_MUTATE=P7-F0002: the FMODE = $0002 run writes $0003; TG68K FAIL, fx68k FAIL.
// MUTATION P7-F0003 stimulus red NMH_MUTATE=P7-F0003: the FMODE = $0003 run writes $0000; TG68K FAIL, fx68k FAIL.
// MUTATION P7-F000C stimulus red NMH_MUTATE=P7-F000C: the FMODE = $000C run writes $0003; TG68K FAIL, fx68k FAIL.
// MUTATION P7-FMODE-W stimulus red NMH_MUTATE=P7-FMODE-W: the FMODE = $0001 run writes $0000 (the P7-F0001 line goes red with it); TG68K FAIL, fx68k FAIL. Also printing FAIL: P7-F0001.
// MUTATION P7-FIXTURE stimulus red NMH_MUTATE=P7-FIXTURE: the FMODE = $0000 run loads prog_regs_nofix.bin; TG68K FAIL, fx68k FAIL.
// MUTATION P7-R2 stimulus red NMH_MUTATE=P7-R2: the FMODE = $0000 run writes BEAMCON0 = $00A0, HTOTAL = $00E8 and VTOTAL = $0138: lines of 233 CCK; TG68K FAIL, fx68k FAIL.
// MUTATION P7-F3 stimulus red NMH_MUTATE=P7-F3: the FMODE = $0000 run ends 3,300,000 cycles after boot(); TG68K FAIL, fx68k FAIL.
// MUTATION P7-POP stimulus red NMH_MUTATE=P7-POP: the FMODE = $0000 run writes DMACON = $8280 (BPLEN clear); TG68K FAIL, fx68k FAIL.
// MUTATION P8-M stimulus red NMH_MUTATE=P8-M: run M writes BPL1MOD = $0018 and BPL2MOD = $0008 (run M's values) while the check keeps run M's expectation; TG68K FAIL, fx68k FAIL.
// MUTATION P8-Mp stimulus red NMH_MUTATE=P8-Mp: run M' writes BPL1MOD = $0008 and BPL2MOD = $0018 (run M's values); TG68K FAIL, fx68k FAIL.
// MUTATION P8-FIXTURE stimulus red NMH_MUTATE=P8-FIXTURE: run M loads prog_regs_nofix.bin; TG68K FAIL, fx68k FAIL.
// MUTATION P8-R2 stimulus red NMH_MUTATE=P8-R2: run M writes BEAMCON0 = $00A0, HTOTAL = $00E8 and VTOTAL = $0138: lines of 233 CCK; TG68K FAIL, fx68k FAIL.
// MUTATION P8-F3 stimulus red NMH_MUTATE=P8-F3: run M ends 3,300,000 cycles after boot(); TG68K FAIL, fx68k FAIL.
// MUTATION P8-R3 stimulus red NMH_MUTATE=P8-R3: run M places the second marker at line $81 instead of $80; TG68K FAIL, fx68k FAIL.
// MUTATION P8-POP stimulus red NMH_MUTATE=P8-POP: run M writes BPLCON0 = $4000 (4 planes); TG68K FAIL, fx68k FAIL.
// MUTATION P9-D0 stimulus red NMH_MUTATE=P9-D0: run D0 writes FMODE = $0000 (BSCAN2 clear); TG68K FAIL, fx68k FAIL.
// MUTATION P9-D1 stimulus red NMH_MUTATE=P9-D1: run D1 writes FMODE = $0000 (BSCAN2 clear); TG68K FAIL, fx68k FAIL.
// MUTATION P9-D2 stimulus red NMH_MUTATE=P9-D2: run D2 writes FMODE = $0000 (BSCAN2 clear); TG68K FAIL, fx68k FAIL.
// MUTATION P9-AGG stimulus red NMH_MUTATE=P9-AGG: run D1 writes BPL1MOD = $0018 and BPL2MOD = $0008 in place of $0008 and $0018; TG68K FAIL, fx68k FAIL. Also printing FAIL: P9-D1.
// MUTATION P9-FIXTURE stimulus red NMH_MUTATE=P9-FIXTURE: run D0 loads prog_regs_nofix.bin; TG68K FAIL, fx68k FAIL.
// MUTATION P9-R2 stimulus red NMH_MUTATE=P9-R2: run D0 writes BEAMCON0 = $00A0, HTOTAL = $00E8 and VTOTAL = $0138: lines of 233 CCK; TG68K FAIL, fx68k FAIL.
// MUTATION P9-F3 stimulus red NMH_MUTATE=P9-F3: run D0 ends 3,300,000 cycles after boot(); TG68K FAIL, fx68k FAIL.
// MUTATION P9-R3 stimulus red NMH_MUTATE=P9-R3: run D0 places the second marker at line $81 instead of $80; TG68K FAIL, fx68k FAIL.
// MUTATION P9-POP stimulus red NMH_MUTATE=P9-POP: run D0 writes BPLCON0 = $4000 (4 planes); TG68K FAIL, fx68k FAIL.
// MUTATION P9-FIRST stimulus red NMH_MUTATE=P9-FIRST: run D0 writes DIWSTRT = $2E81; TG68K FAIL, fx68k FAIL.
// MUTATION P10-MAIN stimulus red NMH_MUTATE=P10-MAIN: both runs load prog_lpen_same.bin, which reads VPOSR:VHPOSR once and stores the value as w0 and w1; TG68K FAIL, fx68k FAIL.
// MUTATION P10-FIXTURE stimulus red NMH_MUTATE=P10-FIXTURE: the main run loads prog_lpen_nofix.bin; TG68K FAIL, fx68k FAIL.
// MUTATION P10-R2 stimulus red NMH_MUTATE=P10-R2: the main run loads prog_lpen_var.bin (BEAMCON0 = $00A0, HTOTAL = $00E8, VTOTAL = $0138: lines of 233 CCK); TG68K FAIL, fx68k FAIL.
// MUTATION P10-WIN stimulus red NMH_MUTATE=P10-WIN: the main run loads prog_lpen_late.bin, whose first read follows the marker by about 110 lines; TG68K FAIL, fx68k FAIL.
// MUTATION P10-LATCH stimulus green NMH_MUTATE=P10-LATCH: live input through the same comparator: the contrast run loads prog_lpen_same.bin; TG68K PASS, fx68k PASS.
// MUTATION P11-JOY stimulus red NMH_MUTATE=P11-JOY: the state "right" drives the left switch ($02) in place of the right switch ($01) on both connectors; TG68K FAIL, fx68k FAIL. Also printing FAIL: P11-AGG.
// MUTATION P11-MOUSE stimulus red NMH_MUTATE=P11-MOUSE: the 40 counts of the first movement are -1 instead of +1 on X; TG68K FAIL, fx68k FAIL. Also printing FAIL: P11-AGG.
// MUTATION P11-MOUSE stimulus red NMH_MUTATE=P11-MOUSE-RET: the second movement brings 39 counts instead of 40; TG68K FAIL, fx68k FAIL. Also printing FAIL: P11-AGG.
// MUTATION P11-AGG stimulus red NMH_MUTATE=P11-AGG: the 40 counts of the third movement are -1 instead of +1 on Y; TG68K FAIL, fx68k FAIL. Also printing FAIL: P11-MOUSE.
// MUTATION P11-FIXTURE stimulus red NMH_MUTATE=P11-FIXTURE: the joystick run loads prog_joy_nofix.bin; TG68K FAIL, fx68k FAIL.
// MUTATION P11-UNIT stimulus red NMH_MUTATE=P11-UNIT: the unit run writes BEAMCON0 = $00A0 and HTOTAL = $0040, so no CCK unit is derived; TG68K FAIL, fx68k FAIL. Also printing FAIL: UNIT-R2.
// MUTATION P11-POP stimulus red NMH_MUTATE=P11-POP: each hold lasts 0.15 ms instead of 2 ms; TG68K FAIL, fx68k FAIL.
// MUTATION P11-STILL stimulus red NMH_MUTATE=P11-STILL: one X count is injected through the mouse inputs in the middle of a hold; TG68K FAIL, fx68k FAIL.
// MUTATION P12-MAIN stimulus red NMH_MUTATE=P12-MAIN: the main run loads prog_uart_k.bin: ADKCON = $0800 at step 5; TG68K FAIL, fx68k FAIL.
// MUTATION P12-PRE1 stimulus red NMH_MUTATE=P12-PRE1: the main run loads prog_uart_early.bin: ADKCON = $8800 in place of $0800 at step 2, so UARTBRK is set before M1; TG68K FAIL, fx68k FAIL.
// MUTATION P12-KPRE stimulus red NMH_MUTATE=P12-KPRE: the contrast run loads prog_uart.bin: the break stays set; TG68K FAIL, fx68k FAIL.
// MUTATION P12-FIXTURE stimulus red NMH_MUTATE=P12-FIXTURE: the main run loads prog_uart_nofix.bin; TG68K FAIL, fx68k FAIL.
// MUTATION P12-UNIT stimulus red NMH_MUTATE=P12-UNIT: the unit run writes BEAMCON0 = $00A0 and HTOTAL = $0040, so no CCK unit is derived; TG68K FAIL, fx68k FAIL. Also printing FAIL: UNIT-R2.
// MUTATION P13-MAIN stimulus red NMH_MUTATE=P13-MAIN: run Y writes DSKPTH = $0005 (3 bits of A20-A16) in place of $0015; TG68K FAIL, fx68k FAIL.
// MUTATION P13-RUN stimulus red NMH_MUTATE=P13-RUN: run X writes DMACON = $8200 (DSKEN clear); TG68K FAIL, fx68k FAIL.
// MUTATION P13-FIXTURE stimulus red NMH_MUTATE=P13-FIXTURE: run X loads prog_disk_nofix.bin; TG68K FAIL, fx68k FAIL.
// MUTATION P14-PAIRS stimulus red NMH_MUTATE=P14-PAIRS: run U1 loads prog_dual_pad121.bin: a loop body of 121 CCK; TG68K FAIL, fx68k FAIL.
// MUTATION P14-R2 stimulus red NMH_MUTATE=P14-R2: run U1 writes BEAMCON0 = $08E0 (VARBEAMEN added); TG68K FAIL, fx68k FAIL. Also printing FAIL: P14-PHASE.
// MUTATION P14-U3 stimulus red NMH_MUTATE=P14-U3: run U3 writes BEAMCON0 = $0820 and no VTOTAL (run U0's); TG68K FAIL, fx68k FAIL.
// MUTATION P14-FIXTURE stimulus red NMH_MUTATE=P14-FIXTURE: run U1 loads prog_dual_nofix.bin; TG68K FAIL, fx68k FAIL.
// MUTATION P14-UNIT stimulus red NMH_MUTATE=P14-UNIT: the unit run writes BEAMCON0 = $00A0 and HTOTAL = $0040, so no CCK unit is derived; TG68K FAIL, fx68k FAIL. Also printing FAIL: UNIT-R2.
// MUTATION P15-V1 stimulus green NMH_MUTATE=P15-V1: live inputs through the same comparators: run V0 loads prog_cpuuh_line.bin (one CPU read of the region per line in place of the UHRES pointer) and run V1 loads prog_cpuuh_57.bin (one per 57 CCK); TG68K PASS, fx68k PASS.
// MUTATION P15-V0 stimulus green NMH_MUTATE=P15-V0: live input through the same comparator: run V0 loads prog_cpuuh_line.bin (one CPU read of the region per line in place of the UHRES pointer); TG68K PASS, fx68k PASS. Also printing FAIL: P15-V1.
// MUTATION P15-FIXTURE stimulus red NMH_MUTATE=P15-FIXTURE: run V0 loads prog_uhres_nofix.bin; TG68K FAIL, fx68k FAIL.
// MUTATION P15-R2 stimulus red NMH_MUTATE=P15-R2: run V0 writes BEAMCON0 = $00A0, HTOTAL = $00E8 and VTOTAL = $0138: lines of 233 CCK; TG68K FAIL, fx68k FAIL.
// MUTATION P15-F3 stimulus red NMH_MUTATE=P15-F3: run V0 ends 3,300,000 cycles after boot(); TG68K FAIL, fx68k FAIL.
// MUTATION P15-R3 stimulus red NMH_MUTATE=P15-R3: run V0 places the second marker at line $99 instead of $98; TG68K FAIL, fx68k FAIL.
// MUTATION P2A1-PF1H2 stimulus red NMH_MUTATE=P2A1-PF1H2: the FMODE = $0000 (a1) test run writes the neighbouring bit $0002 in place of $0001; TG68K FAIL, fx68k FAIL.
// MUTATION P2A1-PF1H3 stimulus red NMH_MUTATE=P2A1-PF1H3: the FMODE = $0000 (a1) test run writes the neighbouring bit $0001 in place of $0002; TG68K FAIL, fx68k FAIL.
// MUTATION P2A1-PF1H4 stimulus red NMH_MUTATE=P2A1-PF1H4: the FMODE = $0000 (a1) test run writes the neighbouring bit $0002 in place of $0004; TG68K FAIL, fx68k FAIL.
// MUTATION P2A1-PF1H5 stimulus red NMH_MUTATE=P2A1-PF1H5: the FMODE = $0000 (a1) test run writes the neighbouring bit $0004 in place of $0008; TG68K FAIL, fx68k FAIL.
// MUTATION P2A1-PF2H2 stimulus red NMH_MUTATE=P2A1-PF2H2: the FMODE = $0000 (a1) test run writes the neighbouring bit $0008 in place of $0010; TG68K FAIL, fx68k FAIL.
// MUTATION P2A1-PF2H3 stimulus red NMH_MUTATE=P2A1-PF2H3: the FMODE = $0000 (a1) test run writes the neighbouring bit $0010 in place of $0020; TG68K FAIL, fx68k FAIL.
// MUTATION P2A1-PF2H4 stimulus red NMH_MUTATE=P2A1-PF2H4: the FMODE = $0000 (a1) test run writes the neighbouring bit $0020 in place of $0040; TG68K FAIL, fx68k FAIL.
// MUTATION P2A1-PF2H5 stimulus red NMH_MUTATE=P2A1-PF2H5: the FMODE = $0000 (a1) test run writes the neighbouring bit $0040 in place of $0080; TG68K FAIL, fx68k FAIL.
// MUTATION P2A1-PF1H1 stimulus red NMH_MUTATE=P2A1-PF1H1: the FMODE = $0000 (a1) test run selects chipset_config = $08 (the ECS chip set) in place of $10 while BPLCON1 = $0200 is written; TG68K FAIL, fx68k FAIL.
// MUTATION P2A1-PF2H1 stimulus red NMH_MUTATE=P2A1-PF2H1: the FMODE = $0000 (a1) test run selects chipset_config = $08 (the ECS chip set) in place of $10 while BPLCON1 = $2000 is written; TG68K FAIL, fx68k FAIL.
// MUTATION P2A2-PF1H2 stimulus red NMH_MUTATE=P2A2-PF1H2: the FMODE = $0003 (a2) test run writes the neighbouring bit $0002 in place of $0001; TG68K FAIL, fx68k FAIL.
// MUTATION P2A2-PF1H3 stimulus red NMH_MUTATE=P2A2-PF1H3: the FMODE = $0003 (a2) test run writes the neighbouring bit $0001 in place of $0002; TG68K FAIL, fx68k FAIL.
// MUTATION P2A2-PF1H4 stimulus red NMH_MUTATE=P2A2-PF1H4: the FMODE = $0003 (a2) test run writes the neighbouring bit $0002 in place of $0004; TG68K FAIL, fx68k FAIL.
// MUTATION P2A2-PF1H5 stimulus red NMH_MUTATE=P2A2-PF1H5: the FMODE = $0003 (a2) test run writes the neighbouring bit $0004 in place of $0008; TG68K FAIL, fx68k FAIL.
// MUTATION P2A2-PF2H2 stimulus red NMH_MUTATE=P2A2-PF2H2: the FMODE = $0003 (a2) test run writes the neighbouring bit $0008 in place of $0010; TG68K FAIL, fx68k FAIL.
// MUTATION P2A2-PF2H3 stimulus red NMH_MUTATE=P2A2-PF2H3: the FMODE = $0003 (a2) test run writes the neighbouring bit $0010 in place of $0020; TG68K FAIL, fx68k FAIL.
// MUTATION P2A2-PF2H4 stimulus red NMH_MUTATE=P2A2-PF2H4: the FMODE = $0003 (a2) test run writes the neighbouring bit $0020 in place of $0040; TG68K FAIL, fx68k FAIL.
// MUTATION P2A2-PF2H5 stimulus red NMH_MUTATE=P2A2-PF2H5: the FMODE = $0003 (a2) test run writes the neighbouring bit $0040 in place of $0080; TG68K FAIL, fx68k FAIL.
// MUTATION P2A2-PF1H1 stimulus red NMH_MUTATE=P2A2-PF1H1: the FMODE = $0003 (a2) test run selects chipset_config = $08 (the ECS chip set) in place of $10 while BPLCON1 = $0200 is written; TG68K FAIL, fx68k FAIL.
// MUTATION P2A2-PF1H6 stimulus red NMH_MUTATE=P2A2-PF1H6: the FMODE = $0003 (a2) test run selects chipset_config = $08 (the ECS chip set) in place of $10 while BPLCON1 = $0400 is written; TG68K FAIL, fx68k FAIL.
// MUTATION P2A2-PF1H7 stimulus red NMH_MUTATE=P2A2-PF1H7: the FMODE = $0003 (a2) test run selects chipset_config = $08 (the ECS chip set) in place of $10 while BPLCON1 = $0800 is written; TG68K FAIL, fx68k FAIL.
// MUTATION P2A2-PF2H1 stimulus red NMH_MUTATE=P2A2-PF2H1: the FMODE = $0003 (a2) test run selects chipset_config = $08 (the ECS chip set) in place of $10 while BPLCON1 = $2000 is written; TG68K FAIL, fx68k FAIL.
// MUTATION P2A2-PF2H6 stimulus red NMH_MUTATE=P2A2-PF2H6: the FMODE = $0003 (a2) test run selects chipset_config = $08 (the ECS chip set) in place of $10 while BPLCON1 = $4000 is written; TG68K FAIL, fx68k FAIL.
// MUTATION P2A2-PF2H7 stimulus red NMH_MUTATE=P2A2-PF2H7: the FMODE = $0003 (a2) test run selects chipset_config = $08 (the ECS chip set) in place of $10 while BPLCON1 = $8000 is written; TG68K FAIL, fx68k FAIL.
// MUTATION P2B-PF1H0 stimulus red NMH_MUTATE=P2B-PF1H0: the SHRES stripe (b) test run selects chipset_config = $08 (the ECS chip set) in place of $10 while BPLCON1 = $0100 is written; TG68K FAIL, fx68k FAIL.
// MUTATION P2B-PF2H0 stimulus red NMH_MUTATE=P2B-PF2H0: the SHRES stripe (b) test run selects chipset_config = $08 (the ECS chip set) in place of $10 while BPLCON1 = $1000 is written; TG68K FAIL, fx68k FAIL.
// MUTATION P2S-PF1H2 stimulus red NMH_MUTATE=P2S-PF1H2: the FMODE = $0003 (a2) test run writes the neighbouring bit $0002 in place of $0001, the (a1) run unchanged; TG68K FAIL, fx68k FAIL. Also printing FAIL: P2A2-PF1H2.
// MUTATION P2S-PF1H3 stimulus red NMH_MUTATE=P2S-PF1H3: the FMODE = $0003 (a2) test run writes the neighbouring bit $0001 in place of $0002, the (a1) run unchanged; TG68K FAIL, fx68k FAIL. Also printing FAIL: P2A2-PF1H3.
// MUTATION P2S-PF1H4 stimulus red NMH_MUTATE=P2S-PF1H4: the FMODE = $0003 (a2) test run writes the neighbouring bit $0002 in place of $0004, the (a1) run unchanged; TG68K FAIL, fx68k FAIL. Also printing FAIL: P2A2-PF1H4.
// MUTATION P2S-PF1H5 stimulus red NMH_MUTATE=P2S-PF1H5: the FMODE = $0003 (a2) test run writes the neighbouring bit $0004 in place of $0008, the (a1) run unchanged; TG68K FAIL, fx68k FAIL. Also printing FAIL: P2A2-PF1H5.
// MUTATION P2S-PF2H2 stimulus red NMH_MUTATE=P2S-PF2H2: the FMODE = $0003 (a2) test run writes the neighbouring bit $0008 in place of $0010, the (a1) run unchanged; TG68K FAIL, fx68k FAIL. Also printing FAIL: P2A2-PF2H2.
// MUTATION P2S-PF2H3 stimulus red NMH_MUTATE=P2S-PF2H3: the FMODE = $0003 (a2) test run writes the neighbouring bit $0010 in place of $0020, the (a1) run unchanged; TG68K FAIL, fx68k FAIL. Also printing FAIL: P2A2-PF2H3.
// MUTATION P2S-PF2H4 stimulus red NMH_MUTATE=P2S-PF2H4: the FMODE = $0003 (a2) test run writes the neighbouring bit $0020 in place of $0040, the (a1) run unchanged; TG68K FAIL, fx68k FAIL. Also printing FAIL: P2A2-PF2H4.
// MUTATION P2S-PF2H5 stimulus red NMH_MUTATE=P2S-PF2H5: the FMODE = $0003 (a2) test run writes the neighbouring bit $0040 in place of $0080, the (a1) run unchanged; TG68K FAIL, fx68k FAIL. Also printing FAIL: P2A2-PF2H5.
// MUTATION P2S-PF1H1 stimulus red NMH_MUTATE=P2S-PF1H1: the FMODE = $0003 (a2) test run selects chipset_config = $08 (the ECS chip set) in place of $10 while BPLCON1 = $0200 is written, the (a1) run unchanged; TG68K FAIL, fx68k FAIL. Also printing FAIL: P2A2-PF1H1.
// MUTATION P2S-PF2H1 stimulus red NMH_MUTATE=P2S-PF2H1: the FMODE = $0003 (a2) test run selects chipset_config = $08 (the ECS chip set) in place of $10 while BPLCON1 = $2000 is written, the (a1) run unchanged; TG68K FAIL, fx68k FAIL. Also printing FAIL: P2A2-PF2H1.
// MUTATION P4a-D1 implementation green NMH_MUTATE=P5-NONE: the test is built against a private copy of the design outside the repository in which the BPLCON1 register resets to $0000 (make test-<core> NANOMIG=<copy>, then NMH_MUTATE=P5-NONE); TG68K PASS, fx68k PASS.
// MUTATION P4a-D2 implementation green NMH_MUTATE=P5-NONE: the test is built against a private copy of the design outside the repository in which the BPLCON1 register resets to $0000 (make test-<core> NANOMIG=<copy>, then NMH_MUTATE=P5-NONE); TG68K PASS, fx68k PASS.
// MUTATION P4b-C1 implementation green NMH_MUTATE=P5-NONE: the test is built against a private copy of the design outside the repository in which the BPLCON1 register resets to $0000 (make test-<core> NANOMIG=<copy>, then NMH_MUTATE=P5-NONE); TG68K PASS, fx68k PASS.
// MUTATION P4b-C2 implementation green NMH_MUTATE=P5-NONE: the test is built against a private copy of the design outside the repository in which the BPLCON1 register resets to $0000 (make test-<core> NANOMIG=<copy>, then NMH_MUTATE=P5-NONE); TG68K PASS, fx68k PASS.
// MUTATION P5 implementation green NMH_MUTATE=P5-NONE: the test is built against a private copy of the design outside the repository in which the BPLCON1 register resets to $0000 (make test-<core> NANOMIG=<copy>, then NMH_MUTATE=P5-NONE); TG68K PASS, fx68k PASS.
// MUTATION P14-POS none impossible no stimulus makes the precondition green: a CPU sampler samples only 113 (TG68K) and 109 (fx68k) of the 227 line positions in run U1, and no sampling loop of the CPU reaches the others
// MUTATION P14-U1 none impossible not graded in any run: the precondition P14-POS prints FAIL on both cores, so this line prints NOT GRADED and no stimulus reaches its comparator
// MUTATION P14-U2 none impossible not graded in any run: the precondition P14-POS prints FAIL on both cores, so this line prints NOT GRADED and no stimulus reaches its comparator
// MUTATION P14-REL none impossible not graded in any run: the precondition P14-POS prints FAIL on both cores, so this line prints NOT GRADED and no stimulus reaches its comparator
// MUTATION P14-MAIN none impossible not graded in any run: the precondition P14-POS prints FAIL on both cores, so this line prints NOT GRADED and no stimulus reaches its comparator
// MUTATION P14-AGG none impossible not graded in any run: the precondition P14-POS prints FAIL on both cores, so this line prints NOT GRADED and no stimulus reaches its comparator
// MUTATION P14-PHASE none impossible unfalsifiable at this boundary: the line compares the phases of the sample times of a CPU loop, whose period varies by one colour clock between samples; a stimulus that confines the phases to fewer than HTOTAL+1 values would need a loop of fixed period, which a CPU loop does not give

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <optional>
#include <algorithm>
#include <cmath>
#include <map>
#include <tuple>
#include <iostream>
#include <functional>
#include <set>
#include <memory>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

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

// Every AA proposition, precondition and anchor is graded on both cores:
// the chip set is selected by the chipset_config input, not by the build.
using Grader = Checks;

std::string g_dir = ".";
const char* g_mutate = nullptr; // NMH_MUTATE: unset -> normal run never runs a mutant
bool run_mutation(const std::string& id) { return g_mutate && id == g_mutate; }

std::string num(double v) { char b[32]; std::snprintf(b, sizeof b, "%g", v); return b; }
std::string hex4(uint16_t v) { char b[8]; std::snprintf(b, sizeof b, "%04X", v); return b; }
std::string opt_num(std::optional<double> v) { return v ? num(*v) : std::string("n/a"); }

// ---- CFG mirror: the fields prog_bars.s / prog_bars_midrun.s read from
// chip RAM before writing them, via genuine CPU bus cycles, to the custom
// registers named in each field's comment. ------------------------------
constexpr uint32_t kCfgBplcon0   = 0x0B0000; // -> BPLCON0 ($100)
constexpr uint32_t kCfgBplcon1   = 0x0B0002; // -> BPLCON1 ($102)
constexpr uint32_t kCfgBplcon1Wr = 0x0B0004; // nonzero: the CPU performs the BPLCON1 write
constexpr uint32_t kCfgBplcon4   = 0x0B0006; // -> BPLCON4 ($10C)
constexpr uint32_t kCfgFmode     = 0x0B0008; // -> FMODE ($1FC)
constexpr uint32_t kCfgDdfstrt   = 0x0B000A; // -> DDFSTRT ($092)
constexpr uint32_t kCfgDdfstop   = 0x0B000C; // -> DDFSTOP ($094)
constexpr uint32_t kCfgDmacon    = 0x0B000E; // -> DMACON ($096)
constexpr uint32_t kCfg2Val      = 0x0B0010; // prog_bars_midrun.s only: -> BPLCON1 at kCfg2Edge
constexpr uint32_t kCfg2Edge     = 0x0B0012; // VERTB edge count (1-based) to write at
constexpr uint32_t kCfg2StopEdge = 0x0B0014; // VERTB edge count after which the program idles

constexpr uint32_t kBuf1 = 0x0B1000; // plane-1 line buffer, 192 bytes
constexpr uint32_t kBuf2 = 0x0B1100; // plane-2 line buffer, 192 bytes

constexpr uint32_t kResults = 0x0C0000;

constexpr uint64_t kBootBudget = 4'000'000;
// Cycles run after boot(). The internal reset takes about four frames before the CPU runs and F1 (located from the
// content) is frame 6 of the capture; a frame is about 568,400 cycles (PAL). P1-P3 read F1..F3 (the third successor ends
// at about 4.56M cycles); P4 also reads F5 (about 5.69M).
constexpr uint64_t kBarsBudget = 4'900'000;
constexpr uint64_t kRunBudget  = 6'000'000;

constexpr uint16_t kDdfstrtLores = 0x0038, kDdfstopLores = 0x00D0;
constexpr uint16_t kDdfstrtShres = 0x003C, kDdfstopShres = 0x00D4;

// ---- line-buffer content, S3 --------------------------------------------
using Buf = std::array<uint8_t, 192>;

Buf zero_buf() { Buf b{}; b.fill(0); return b; }

Buf bars_buf(int byte_start) {
    Buf b{}; b.fill(0);
    for (int i = 0; i < 4; ++i) b[byte_start + i] = 0xFF;
    return b;
}

Buf stripe_buf(int solid_start, int alt_start, int alt_len, bool cc_instead_of_aa = false) {
    Buf b{}; b.fill(0);
    b[solid_start] = 0xFF; b[solid_start + 1] = 0xFF;
    uint8_t pat = cc_instead_of_aa ? 0xCC : 0xAA;
    for (int i = 0; i < alt_len; ++i) b[alt_start + i] = pat;
    return b;
}

Buf scale_buf() {
    Buf b{};
    for (size_t i = 0; i < b.size(); i += 2) { b[i] = 0xAA; b[i + 1] = 0xAA; }
    return b;
}

void poke_buf(nmh::NanomigHarness& h, uint32_t byte_addr, const Buf& buf) {
    for (size_t i = 0; i < buf.size(); i += 2)
        h.poke_word(byte_addr / 2 + i / 2, uint16_t((buf[i] << 8) | buf[i + 1]));
}

// ---- run configuration and execution -------------------------------------

// Harness state of every run (SPECIFICATION.md section 0): the AA chip set
// is selected (chipset_config bit 4) and the 2 MB Chip memory of the
// harness default is kept.
nmh::Config aa_config() {
    nmh::Config c;
    c.chipset_config = 0x10;
    return c;
}

struct BarsConfig {
    uint16_t bplcon0;
    uint16_t bplcon1 = 0x0000;
    bool     write_bplcon1 = true;
    uint16_t bplcon4 = 0x0011;
    uint16_t fmode = 0x0000;
    uint16_t ddfstrt = kDdfstrtLores;
    uint16_t ddfstop = kDdfstopLores;
    uint16_t dmacon = 0x8380; // SET,DMAEN,BPLEN,COPEN
    uint8_t  chipset = 0x10;  // chipset_config: AA selected; 0x08 is the ECS chip set (the contrast)
    Buf buf1 = zero_buf();
    Buf buf2 = zero_buf();
};

struct RunOut {
    bool loaded = false;
    bool released = false;
    bool fixture_ok = false;
    std::vector<nmh::VideoSample> video;
    std::vector<nmh::RamAccess> ram;
};

RunOut run_bars(const BarsConfig& c, uint64_t run_budget = kBarsBudget) {
    RunOut r;
    nmh::Config cfg = aa_config();
    cfg.chipset_config = c.chipset;
    nmh::NanomigHarness h(cfg);

    h.poke_word(kCfgBplcon0 / 2, c.bplcon0);
    h.poke_word(kCfgBplcon1 / 2, c.bplcon1);
    h.poke_word(kCfgBplcon1Wr / 2, c.write_bplcon1 ? 1 : 0);
    h.poke_word(kCfgBplcon4 / 2, c.bplcon4);
    h.poke_word(kCfgFmode / 2, c.fmode);
    h.poke_word(kCfgDdfstrt / 2, c.ddfstrt);
    h.poke_word(kCfgDdfstop / 2, c.ddfstop);
    h.poke_word(kCfgDmacon / 2, c.dmacon);
    poke_buf(h, kBuf1, c.buf1);
    poke_buf(h, kBuf2, c.buf2);

    r.loaded = h.load_program(g_dir + "/prog_bars.bin") > 0;
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;
    h.run_cycles(run_budget);
    auto fx = h.peek_logical(kResults);
    r.fixture_ok = fx.has_value() && *fx == 0xBEEF;
    r.video = h.video();
    r.ram = h.ram_log();
    return r;
}

struct MidrunConfig {
    uint16_t bplcon0;
    uint16_t bplcon4 = 0x0011;
    uint16_t fmode = 0x0000;
    uint16_t ddfstrt = kDdfstrtLores;
    uint16_t ddfstop = kDdfstopLores;
    uint16_t dmacon = 0x8380;
    uint16_t mid_val = 0x0000;
    uint16_t mid_edge = 5;
    uint16_t stop_edge = 6;
    uint8_t  chipset = 0x10;
    Buf buf1 = zero_buf();
    Buf buf2 = zero_buf();
};

RunOut run_midrun(const MidrunConfig& c, uint64_t run_budget) {
    RunOut r;
    nmh::Config cfg = aa_config();
    cfg.chipset_config = c.chipset;
    nmh::NanomigHarness h(cfg);

    h.poke_word(kCfgBplcon0 / 2, c.bplcon0);
    h.poke_word(kCfgBplcon4 / 2, c.bplcon4);
    h.poke_word(kCfgFmode / 2, c.fmode);
    h.poke_word(kCfgDdfstrt / 2, c.ddfstrt);
    h.poke_word(kCfgDdfstop / 2, c.ddfstop);
    h.poke_word(kCfgDmacon / 2, c.dmacon);
    h.poke_word(kCfg2Val / 2, c.mid_val);
    h.poke_word(kCfg2Edge / 2, c.mid_edge);
    h.poke_word(kCfg2StopEdge / 2, c.stop_edge);
    poke_buf(h, kBuf1, c.buf1);
    poke_buf(h, kBuf2, c.buf2);

    r.loaded = h.load_program(g_dir + "/prog_bars_midrun.bin") > 0;
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;
    h.run_cycles(run_budget);
    auto fx = h.peek_logical(kResults);
    r.fixture_ok = fx.has_value() && *fx == 0xBEEF;
    r.video = h.video();
    return r;
}

// ---- R1: video reference frame -------------------------------------------

using nmh::VideoSample;
using nmh::Span;

bool is_color(const VideoSample& s, uint8_t r, uint8_t g, uint8_t b) {
    return s.r == r && s.g == g && s.b == b;
}

constexpr uint8_t kC00r=2, kC00g=2, kC00b=2;   // COLOR00 = $0222
constexpr uint8_t kC01r=0xF, kC01g=0, kC01b=0; // COLOR01 = $0F00
constexpr uint8_t kC02r=0, kC02g=0xF, kC02b=0; // COLOR02 = $00F0

std::optional<int> measure_r_l(const std::vector<VideoSample>& v, size_t frame_idx) {
    auto frames = nmh::find_frames(v);
    if (frame_idx >= frames.size()) return std::nullopt;
    Span fr = frames[frame_idx];
    auto lines = nmh::find_lines(v);
    std::map<int,int> hist;
    for (auto& ln : lines) {
        if (ln.begin < fr.begin || ln.end > fr.end) continue;
        int cur = -1, run_len = 0;
        for (size_t i = ln.begin; i < ln.end; ++i) {
            int cls = -1;
            if (is_color(v[i], kC01r,kC01g,kC01b)) cls = 1;
            else if (is_color(v[i], kC00r,kC00g,kC00b)) cls = 0;
            if (cls == cur) { ++run_len; continue; }
            if (cur != -1 && run_len > 1) ++hist[run_len];
            cur = cls; run_len = (cls == -1) ? 0 : 1;
        }
        if (cur != -1 && run_len > 1) ++hist[run_len];
    }
    if (hist.empty()) return std::nullopt;
    int best = -1, best_n = -1;
    for (auto& [len, n] : hist) if (n > best_n) { best = len; best_n = n; }
    return best;
}

std::vector<Span> window_lines(const std::vector<VideoSample>& v, size_t frame_idx,
                                uint8_t r, uint8_t g, uint8_t b) {
    std::vector<Span> out;
    auto frames = nmh::find_frames(v);
    if (frame_idx >= frames.size()) return out;
    Span fr = frames[frame_idx];
    for (auto& ln : nmh::find_lines(v)) {
        if (ln.begin < fr.begin || ln.end > fr.end) continue;
        for (size_t i = ln.begin; i < ln.end; ++i)
            if (is_color(v[i], r, g, b)) { out.push_back(ln); break; }
    }
    return out;
}

std::optional<size_t> find_edge(const std::vector<VideoSample>& v, Span ln,
                                 uint8_t r, uint8_t g, uint8_t b) {
    for (size_t i = ln.begin; i < ln.end && i < v.size(); ++i)
        if (is_color(v[i], r, g, b)) return i - ln.begin;
    return std::nullopt;
}

struct EdgeMeasure { bool ok; std::optional<size_t> offset; int lines_checked; };

EdgeMeasure measure_edge_all_lines(const std::vector<VideoSample>& v, size_t frame_idx,
                                    uint8_t plane_r, uint8_t plane_g, uint8_t plane_b) {
    auto lines = window_lines(v, frame_idx, plane_r, plane_g, plane_b);
    EdgeMeasure m{true, std::nullopt, (int)lines.size()};
    for (auto& ln : lines) {
        auto e = find_edge(v, ln, plane_r, plane_g, plane_b);
        if (!e) { m.ok = false; return m; }
        if (!m.offset) m.offset = e;
        else if (*e != *m.offset) { m.ok = false; return m; }
    }
    if (lines.empty()) m.ok = false;
    return m;
}

struct HsyncShape { size_t line_count; double avg_period; double avg_width; };

HsyncShape hsync_shape(const std::vector<VideoSample>& v, size_t frame_idx) {
    auto frames = nmh::find_frames(v);
    HsyncShape s{0, 0, 0};
    if (frame_idx >= frames.size()) return s;
    Span fr = frames[frame_idx];
    std::vector<Span> lines;
    for (auto& ln : nmh::find_lines(v)) if (ln.begin >= fr.begin && ln.end <= fr.end) lines.push_back(ln);
    s.line_count = lines.size();
    if (lines.empty()) return s;
    double period_sum = 0; for (auto& ln : lines) period_sum += double(ln.end - ln.begin);
    s.avg_period = period_sum / lines.size();
    double width_sum = 0; size_t width_n = 0;
    for (auto& ln : lines) {
        size_t w = 0;
        for (size_t i = ln.begin; i < ln.end && i < v.size() && v[i].hs; ++i) ++w;
        if (w > 0) { width_sum += w; ++width_n; }
    }
    if (width_n) s.avg_width = width_sum / width_n;
    return s;
}

int classify_offset(const std::vector<VideoSample>& v, Span ln, double offset_samples,
                     uint8_t r, uint8_t g, uint8_t b) {
    if (offset_samples < 0) return -1;
    size_t idx = ln.begin + (size_t)std::llround(offset_samples);
    if (idx >= ln.end || idx >= v.size()) return -1;
    return is_color(v[idx], r, g, b) ? 1 : 0;
}

// S3 stripe C1/C2 signature: classification (1 = shows plane colour, 0 =
// does not) at fixed op-offsets from a reference solid-part edge, required
// equal on every window line (S3b). nullopt if not found or inconsistent.
std::optional<std::vector<int>> stripe_signature(const std::vector<VideoSample>& v, size_t frame_idx,
        uint8_t plane_r, uint8_t plane_g, uint8_t plane_b,
        double ref_edge_samples, double op, const std::vector<double>& offsets_op) {
    auto lines = window_lines(v, frame_idx, plane_r, plane_g, plane_b);
    if (lines.empty()) return std::nullopt;
    std::vector<int> sig;
    for (double k : offsets_op)
        sig.push_back(classify_offset(v, lines[0], ref_edge_samples + k * op, plane_r, plane_g, plane_b));
    for (int c : sig) if (c < 0) return std::nullopt;
    for (auto& ln : lines) {
        for (size_t i = 0; i < offsets_op.size(); ++i) {
            int c = classify_offset(v, ln, ref_edge_samples + offsets_op[i] * op, plane_r, plane_g, plane_b);
            if (c != sig[i]) return std::nullopt;
        }
    }
    return sig;
}

bool near_int(double v, double target, double tol = 0.26) { return std::abs(v - target) <= tol; }

// boot() runs through several complete frames of NanoMig's own internal
// (multi-frame, SOF-counted) reset before releasing the CPU, and video
// capture starts at harness construction -- so a raw find_frames() index
// counted from the start of the capture is not "F1" (SPECIFICATION.md
// section 0, "Frames": "Fk is the k-th complete frame after the write
// that enables DMA"). F1 is located here from content instead: the first
// frame index whose window_lines() population (any sample of COLOR01,
// which every P1-P5 buffer places somewhere on every window line) is
// large and equal to the next frame's population, i.e. steady-state
// bitplane DMA is already running for the whole frame.
std::optional<size_t> find_f1(const std::vector<VideoSample>& v) {
    auto frames = nmh::find_frames(v);
    for (size_t i = 0; i + 1 < frames.size(); ++i) {
        size_t n0 = window_lines(v, i, kC01r, kC01g, kC01b).size();
        size_t n1 = window_lines(v, i + 1, kC01r, kC01g, kC01b).size();
        if (n0 >= 100 && n0 == n1) return i;
    }
    return std::nullopt;
}


// The proposition a mutation selector belongs to: "P7-F0001" -> "P7", "UNIT-R2" -> "UNIT".
std::string sel_prop() {
    if (!g_mutate) return "";
    std::string s = g_mutate;
    if (s.compare(0, 4, "UNIT") == 0) return "UNIT";
    if (s.compare(0, 2, "R1") == 0) return "R1";
    size_t i = 1;
    while (s.size() > 0 && s[0] == 'P' && i < s.size() && std::isdigit((unsigned char)s[i])) ++i;
    return s.substr(0, i);
}

// ---- register-list runs and port-level captures (P6-P15) -------------------
//
// prog_regs.bin reads (register offset, value) word pairs from chip RAM at
// kRList, terminated by $FFFF, and writes each value to its custom register
// with one MOVE.W: every register is written by a genuine CPU bus cycle, in
// list order. What the test pre-places in memory (Copper lists, bit-plane
// and sprite data, tables) is poked before boot().

struct RegList {
    std::vector<uint16_t> w;
    void add(uint16_t reg, uint16_t v) { w.push_back(reg); w.push_back(v); }
    void finish() { w.push_back(0xFFFF); }
};

constexpr uint32_t kRList = 0x0B8000;
constexpr uint32_t kDoneAddr = 0x0C0FFE;

struct Poke { uint32_t byte_addr; uint16_t value; };

struct Cap {
    bool loaded = false, released = false, fixture_ok = false, done = false;
    std::vector<VideoSample> video;
    std::vector<nmh::RamAccess> ram;
    std::vector<uint8_t> refresh;  // `refresh` output, one entry per clk_sys cycle from `base`
    std::vector<uint8_t> txd;      // `uart_tx`, same indexing, when requested
    uint64_t base = 0;             // harness cycle of refresh[0]; video[base + i] is the same cycle
    uint16_t results2 = 0;         // word at RESULTS+2 at the end of the run
    uint16_t results4 = 0;         // word at RESULTS+4 at the end of the run
};

struct RunSpec {
    std::string bin = "prog_regs.bin";
    RegList rl;
    std::vector<Poke> pokes;
    uint64_t cycles = 5'400'000;   // clk_sys cycles run after boot()
    bool sample_txd = false;
    uint8_t chipset = 0x10;        // chipset_config
    std::function<void(nmh::NanomigHarness&, uint64_t)> drive; // before cycle i (since base): set inputs
};

Cap run_spec(const RunSpec& s) {
    Cap c;
    nmh::Config cfg = aa_config();
    cfg.chipset_config = s.chipset;
    nmh::NanomigHarness h(cfg);
    for (size_t i = 0; i < s.rl.w.size(); ++i) h.poke_word(kRList / 2 + i, s.rl.w[i]);
    for (auto& p : s.pokes) h.poke_word(p.byte_addr / 2, p.value);
    c.loaded = h.load_program(g_dir + "/" + s.bin) > 0;
    if (!c.loaded) return c;
    c.released = h.boot(kBootBudget).cpu_released;
    c.base = h.cycles();
    c.refresh.reserve(s.cycles);
    if (s.sample_txd) c.txd.reserve(s.cycles);
    for (uint64_t i = 0; i < s.cycles; ++i) {
        if (s.drive) s.drive(h, i);
        h.run_cycles(1);
        c.refresh.push_back(h.ports()->refresh ? 1 : 0);
        if (s.sample_txd) c.txd.push_back(h.ports()->uart_tx ? 1 : 0);
    }
    auto fx = h.peek_logical(kResults);
    c.fixture_ok = fx.has_value() && *fx == 0xBEEF;
    c.results2 = h.peek_logical(kResults + 2).value_or(0);
    c.results4 = h.peek_logical(kResults + 4).value_or(0);
    auto dn = h.peek_logical(kDoneAddr);
    c.done = dn.has_value() && *dn == 0x600D;
    c.video = h.video();
    c.ram = h.ram_log();
    return c;
}

// Pokes a word table into chip RAM (byte address, big-endian words).
void poke_words(std::vector<Poke>& v, uint32_t byte_addr, const std::vector<uint16_t>& words) {
    for (size_t i = 0; i < words.size(); ++i) v.push_back({byte_addr + uint32_t(2 * i), words[i]});
}

// S1 step 1 and the S2 display baseline (SPECIFICATION.md section 4).
void s1_step1(RegList& rl, uint16_t beamcon0 = 0x0020) {
    rl.add(0x096, 0x7FFF); rl.add(0x09A, 0x7FFF); rl.add(0x09C, 0x7FFF);
    rl.add(0x1DC, beamcon0);
}
void s2_baseline(RegList& rl, uint16_t bplcon0, uint16_t fmode, uint16_t diwstrt = 0x2C81,
                 uint16_t ddfstrt = kDdfstrtLores, uint16_t ddfstop = kDdfstopLores,
                 uint16_t bpl1mod = 0, uint16_t bpl2mod = 0) {
    rl.add(0x100, bplcon0);
    rl.add(0x104, 0x0024);
    rl.add(0x106, 0x0C00);
    rl.add(0x10C, 0x0011);
    rl.add(0x08E, diwstrt); rl.add(0x090, 0xF0C1);
    rl.add(0x092, ddfstrt); rl.add(0x094, ddfstop);
    rl.add(0x108, bpl1mod); rl.add(0x10A, bpl2mod);
    rl.add(0x1FC, fmode);
    rl.add(0x180, 0x0222); rl.add(0x182, 0x0F00); rl.add(0x184, 0x00F0); rl.add(0x186, 0x000F);
    rl.add(0x1A2, 0x0FF0);
}

// A Copper list is built into a word vector at a chip-RAM byte address.
struct CopList {
    uint32_t base;
    std::vector<uint16_t> w;
    std::map<int, uint32_t> marker_move_word;  // marker line -> word address of its MOVE's first word
    explicit CopList(uint32_t b) : base(b) {}
    void move(uint16_t reg, uint16_t v) { w.push_back(reg); w.push_back(v); }
    void wait(int v, int hp = 0x06) { w.push_back(uint16_t((v << 8) | (hp & 0xFE) | 1)); w.push_back(0xFFFE); }
    void marker(int v) {            // R3: WAIT(V,$06) then MOVE $1FE,$0000
        wait(v);
        marker_move_word[v] = (base + uint32_t(2 * w.size())) / 2;
        move(0x1FE, 0x0000);
    }
    void ptr(uint16_t hi_reg, uint32_t addr) { move(hi_reg, uint16_t(addr >> 16)); move(uint16_t(hi_reg + 2), uint16_t(addr & 0xFFFF)); }
    void end() { w.push_back(0xFFFF); w.push_back(0xFFFE); }
    void poke_into(std::vector<Poke>& p) const { poke_words(p, base, w); }
};

constexpr uint32_t kCopList = 0x0B2000;

// R2 reference frame: line boundaries and the CCK unit from the `refresh` output.
struct R2 {
    bool ok = false;              // groups of exactly four equally spaced edges, 226..229 CCK apart
    uint64_t K = 0;               // clk_sys cycles per CCK (half the spacing within a group)
    std::vector<uint64_t> start;  // cycle of the first refresh access of each line
    size_t groups = 0;
    double gap_min = 0, gap_max = 0;  // consecutive line starts, in CCK
};

R2 analyse_r2(const Cap& c) {
    R2 r;
    std::vector<uint64_t> edges;
    for (size_t i = 1; i < c.refresh.size(); ++i)
        if (c.refresh[i] && !c.refresh[i - 1]) edges.push_back(c.base + i);
    std::vector<std::vector<uint64_t>> groups;
    for (size_t i = 0; i < edges.size(); ++i) {
        if (i == 0 || edges[i] - edges[i - 1] > 64) groups.push_back({});
        groups.back().push_back(edges[i]);
    }
    if (groups.size() > 2) { groups.erase(groups.begin()); groups.pop_back(); } // capture ends may cut a group
    r.groups = groups.size();
    if (groups.size() < 10) return r;
    uint64_t sp = groups[0].size() == 4 ? groups[0][1] - groups[0][0] : 0;
    bool ok = sp > 0 && sp % 2 == 0;
    r.K = sp / 2;
    for (auto& g : groups) {
        if (g.size() != 4) { ok = false; break; }
        for (size_t k = 1; k < 4; ++k) if (g[k] - g[k - 1] != sp) ok = false;
        r.start.push_back(g[0]);
    }
    r.gap_min = 1e9; r.gap_max = 0;
    for (size_t i = 1; ok && i < r.start.size(); ++i) {
        double gap = double(r.start[i] - r.start[i - 1]) / double(r.K);
        r.gap_min = std::min(r.gap_min, gap); r.gap_max = std::max(r.gap_max, gap);
        if (gap < 226.0 || gap > 229.0) ok = false;
    }
    r.ok = ok;
    return r;
}

// Index of the R2 line holding cycle t (-1 before the first boundary).
int64_t line_of(const R2& r, uint64_t t) {
    auto it = std::upper_bound(r.start.begin(), r.start.end(), t);
    return int64_t(it - r.start.begin()) - 1;
}

// The frame holding the first read of [lo_byte, hi_byte): the frame the
// DMA-enabling write falls in. Fk (k >= 1) is its k-th successor.
std::optional<size_t> enable_frame(const Cap& c, uint32_t lo_byte, uint32_t hi_byte) {
    auto frames = nmh::find_frames(c.video);
    for (auto& a : c.ram) {
        if (a.is_write || a.word_addr * 2 < lo_byte || a.word_addr * 2 >= hi_byte) continue;
        for (size_t i = 0; i < frames.size(); ++i)
            if (a.cycle >= frames[i].begin && a.cycle < frames[i].end) return i;
        return std::nullopt;
    }
    return std::nullopt;
}

// A fetch: the accesses of one channel in one slot; its address is the lowest.
struct Fetch { uint64_t cycle; uint32_t addr; int64_t line; };

std::vector<Fetch> fetches_in(const Cap& c, const R2& r, uint32_t lo_byte, uint32_t hi_byte, Span fr) {
    std::vector<Fetch> out;
    for (auto& a : c.ram) {
        if (a.is_write || a.cycle < fr.begin || a.cycle >= fr.end) continue;
        uint32_t byte = a.word_addr * 2;
        if (byte < lo_byte || byte >= hi_byte) continue;
        if (!out.empty() && a.cycle - out.back().cycle < r.K) {
            out.back().addr = std::min(out.back().addr, byte);
            continue;
        }
        out.push_back({a.cycle, byte, line_of(r, a.cycle)});
    }
    return out;
}

void not_graded(Checks& checks, const std::string& id, const std::string& unmet) {
    checks.record("NOT GRADED " + id + ": precondition(s) not met: " + unmet);
}

// A precondition that rests on others: graded only when they hold, NOT GRADED otherwise (never PASS or FAIL).
void dep_expect(Checks& checks, bool deps, bool cond, const std::string& label, const std::string& unmet) {
    if (!deps) { not_graded(checks, label.substr(0, label.find_first_of(" :")), unmet); return; }
    checks.expect(cond, label);
}

std::string dec(long long v) { return std::to_string(v); }
std::string hexw(uint32_t v, int digits = 4) { char b[16]; std::snprintf(b, sizeof b, "%0*X", digits, v); return b; }

bool mutating(const char* id) { return g_mutate && std::string(g_mutate) == id; }

// ---- P7-P9: bit-plane fetch runs -------------------------------------------

struct FetchRun {
    Cap cap;
    R2 r2;
    std::optional<size_t> x;       // frame holding the first fetch
    bool f3_ok = false;            // F3 exists and is complete
    Span f3{0, 0};
};

FetchRun run_fetch(const RunSpec& s, uint32_t lo, uint32_t hi) {
    FetchRun f;
    f.cap = run_spec(s);
    f.r2 = analyse_r2(f.cap);
    f.x = enable_frame(f.cap, lo, hi);
    auto frames = nmh::find_frames(f.cap.video);
    if (f.x && *f.x + 3 + 1 < frames.size()) { f.f3_ok = true; f.f3 = frames[*f.x + 3]; }
    return f;
}

// Most common difference between consecutive fetch addresses, and whether
// every difference equals it.
struct StepStat { long long step = 0; bool uniform = false; size_t pairs = 0; };
StepStat step_stat(const std::vector<Fetch>& v) {
    StepStat s;
    std::map<long long, size_t> hist;
    for (size_t i = 1; i < v.size(); ++i) ++hist[(long long)v[i].addr - (long long)v[i - 1].addr];
    s.pairs = v.size() > 0 ? v.size() - 1 : 0;
    size_t best = 0;
    for (auto& kv : hist) if (kv.second > best) { best = kv.second; s.step = kv.first; }
    s.uniform = s.pairs > 0 && best == s.pairs;
    return s;
}

// Number of R2 lines holding at least `min_per_line` fetches.
size_t lines_with(const std::vector<Fetch>& v, size_t min_per_line) {
    std::map<int64_t, size_t> per;
    for (auto& f : v) ++per[f.line];
    size_t n = 0;
    for (auto& kv : per) if (kv.second >= min_per_line) ++n;
    return n;
}

constexpr uint32_t kP7Plane1 = 0x120000, kP7Plane2 = 0x130000;

// S1 + S2, LORES with two planes, FMODE as given, Copper reloading both pointers.
RunSpec p7_spec(uint16_t fmode, bool bplen, uint16_t beamcon0, bool htotal) {
    RunSpec s;
    CopList cl(kCopList);
    cl.ptr(0x0E0, kP7Plane1);
    cl.ptr(0x0E4, kP7Plane2);
    cl.end();
    cl.poke_into(s.pokes);
    for (uint32_t pl = 0; pl < 2; ++pl)
        for (uint32_t o = 0; o < 0x8000; o += 2)
            s.pokes.push_back({(pl ? kP7Plane2 : kP7Plane1) + o, uint16_t(0x8000 | ((o / 2) & 0x7FFF))});
    s1_step1(s.rl, beamcon0);
    if (htotal) { s.rl.add(0x1C0, 0x00E8); s.rl.add(0x1C8, 0x0138); }   // VARBEAMEN: lines of 233 CCK in place of 227
    s2_baseline(s.rl, 0x2000, fmode);
    s.rl.add(0x102, 0x0000);
    s.rl.add(0x080, uint16_t(kCopList >> 16)); s.rl.add(0x082, uint16_t(kCopList & 0xFFFF)); s.rl.add(0x088, 0);
    s.rl.add(0x096, bplen ? 0x8380 : 0x8280);
    s.rl.finish();
    return s;
}

void prop_p7(Checks& checks, Grader& g) {
    struct Row { uint16_t fmode; int n; const char* ids; uint16_t mutated; const char* what; };
    const Row rows[] = {
        {0x0000, 2, "sol:AA-fmode-bitplane-fetch.c1", 0x0003, "bits 1-0 = 00, AA p.23 'by 2 bytes (as before)'"},
        {0x0001, 4, "sol:AA-fmode-bitplane-fetch.c4", 0x0003, "bits 1-0 = 01, AA p.23 'by 4 bytes'"},
        {0x0002, 4, "sol:AA-fmode-bitplane-fetch.c7", 0x0003, "bits 1-0 = 10, AA p.23 'by 4 bytes'"},
        {0x0003, 8, "sol:AA-fmode-bitplane-fetch.c10", 0x0000, "bits 1-0 = 11, AA p.23 'by 8 bytes'"},
        {0x000C, 2, "", 0x0003, "bits 3-2 set, bits 1-0 = 00, AA p.23 SPAGEM and SPR32 are bits 3 and 2"},
    };
    constexpr size_t N = sizeof(rows) / sizeof(rows[0]);
    FetchRun fr[N];
    uint16_t written[N];
    // A mutation names the run it changes; only the runs it needs are made.
    bool ran[N];
    for (size_t i = 0; i < N; ++i) ran[i] = true;
    if (g_mutate && std::string(g_mutate) != "P7-NONE") {
        const std::string ms = g_mutate;
        bool rowsel = false;
        for (size_t i = 0; i < N; ++i) { ran[i] = false; if (ms == "P7-F" + hexw(rows[i].fmode)) { ran[i] = true; rowsel = true; } }
        if (ms == "P7-FMODE-W") for (size_t i = 0; i < 4; ++i) ran[i] = true;
        else if (!rowsel) ran[0] = true;
    }
    bool all_ran = true;
    for (size_t i = 0; i < N; ++i) all_ran = all_ran && ran[i];
    for (size_t i = 0; i < N; ++i) {
        if (!ran[i]) continue;
        std::string mid = "P7-F" + hexw(rows[i].fmode);
        written[i] = mutating(mid.c_str()) ? rows[i].mutated : rows[i].fmode;
        if (i == 1 && mutating("P7-FMODE-W")) written[i] = 0x0000;
        bool bplen = !(i == 0 && mutating("P7-POP"));
        uint16_t bc = (i == 0 && mutating("P7-R2")) ? 0x00A0 : 0x0020;
        RunSpec s = p7_spec(written[i], bplen, bc, i == 0 && mutating("P7-R2"));
        if (i == 0 && mutating("P7-F3")) s.cycles = 3'300'000;
        if (i == 0 && mutating("P7-FIXTURE")) s.bin = "prog_regs_nofix.bin";
        fr[i] = run_fetch(s, kCopList, kCopList + 0x100);   // the frame holding the first Copper fetch is the one the DMA enable falls in
    }

    // ---- preconditions, once for all five runs ----
    bool fixture = true, r2ok = true, f3ok = true, pop = true;
    std::string pre_note;
    std::vector<Fetch> p1[N], p2[N];
    for (size_t i = 0; i < N; ++i) {
        if (!ran[i]) continue;
        fixture = fixture && fr[i].cap.loaded && fr[i].cap.released && fr[i].cap.fixture_ok;
        r2ok = r2ok && fr[i].r2.ok;
        f3ok = f3ok && fr[i].f3_ok;
        if (fr[i].f3_ok && fr[i].r2.ok) {
            p1[i] = fetches_in(fr[i].cap, fr[i].r2, kP7Plane1, kP7Plane1 + 0x8000, fr[i].f3);
            p2[i] = fetches_in(fr[i].cap, fr[i].r2, kP7Plane2, kP7Plane2 + 0x8000, fr[i].f3);
        }
        pop = pop && lines_with(p1[i], 2) >= 10 && lines_with(p2[i], 2) >= 10;
    }
    g.expect(fixture, "P7-FIXTURE: every run loaded, booted and stored the $BEEF fixture before any observation");
    g.expect(r2ok, "P7-R2: precondition R2 anchor: in every run the refresh output comes in groups of exactly four equally spaced accesses, consecutive groups 226 to 229 CCK apart");
    if (ran[0]) checks.note("P7 R2: K = " + dec((long long)fr[0].r2.K) + " clk_sys cycles per CCK, " + dec((long long)fr[0].r2.groups) +
                " lines, line spacing " + num(fr[0].r2.gap_min) + " to " + num(fr[0].r2.gap_max) + " CCK");
    g.expect(f3ok, "P7-F3: precondition frame F3: in every run the frame holding the first Copper-list fetch is found and its third successor F3 is complete");
    dep_expect(checks, r2ok && f3ok, pop, "P7-POP: precondition population: in every run both planes are fetched on at least 10 lines of F3 with at least 2 fetches per line", "R2 anchor or F3");
    bool pre = fixture && r2ok && f3ok && pop;

    // ---- assertions ----
    StepStat s1[N], s2[N];
    for (size_t i = 0; i < N; ++i) { s1[i] = step_stat(p1[i]); s2[i] = step_stat(p2[i]); }
    for (size_t i = 0; i < N; ++i) {
        if (!ran[i]) continue;
        std::string label = "P7-F" + hexw(rows[i].fmode) + " " + (rows[i].ids[0] ? std::string(rows[i].ids) + ": " : std::string()) +
            "FMODE=$" + hexw(rows[i].fmode) + " (" + rows[i].what + "): the first fetch of each plane in F3 is at the plane's buffer address and every pair of consecutive fetches of plane 1 and of plane 2 is " +
            dec(rows[i].n) + " bytes apart";
        if (!pre) { not_graded(checks, "P7 FMODE=$" + hexw(rows[i].fmode), "fixture, R2 anchor, F3 or population"); continue; }
        bool ok = !p1[i].empty() && !p2[i].empty() && p1[i][0].addr == kP7Plane1 && p2[i][0].addr == kP7Plane2 &&
                  s1[i].uniform && s2[i].uniform && s1[i].step == rows[i].n && s2[i].step == rows[i].n;
        g.expect(ok, label);
        checks.note("P7 measured FMODE=$" + hexw(written[i]) + " written: plane 1 first=$" + hexw(p1[i].empty() ? 0 : p1[i][0].addr, 6) +
                    " step=" + dec(s1[i].step) + (s1[i].uniform ? " (uniform)" : " (not uniform)") + " over " + dec((long long)s1[i].pairs) +
                    " pairs; plane 2 first=$" + hexw(p2[i].empty() ? 0 : p2[i][0].addr, 6) + " step=" + dec(s2[i].step) +
                    (s2[i].uniform ? " (uniform)" : " (not uniform)") + " over " + dec((long long)s2[i].pairs) + " pairs");
    }
    if (!(ran[0] && ran[1] && ran[2] && ran[3])) return;
    if (pre) {
        bool w = s1[1].step != s1[0].step && s1[2].step != s1[0].step && s1[3].step != s1[0].step;
        g.expect(w, "P7-FMODE-W sol:AA-fmode.c5: a CPU write to $DFF1FC (FMODE, AA p.23 'FMODE P 1FC W') takes effect: the plane-1 fetch step of each run written with $0001, $0002 and $0003 differs from that of the run written with $0000");
    } else {
        not_graded(checks, "P7 FMODE write", "fixture, R2 anchor, F3 or population");
    }
}

// ---- P8, P9: modulos -------------------------------------------------------

constexpr uint32_t kP89Plane = 0x100000, kP89Pitch = 0x4000;
constexpr int kLastWindowLine = 0xEF;   // DIWSTOP $F0C1: the window ends before line $F0

struct Cfg89 {
    uint16_t bplcon0 = 0x0010;          // BPU = 8
    uint16_t fmode = 0x0000;
    uint16_t bpl1mod = 0x0008, bpl2mod = 0x0018;
    uint16_t diwstrt = 0x2C81;          // as written by the CPU (S2)
    bool diw_copper = false;            // D2: the list writes DIWSTRT in vertical blank and again at line diw_line
    int diw_line = 0x60;
    uint16_t diw_second = 0x2D81;
    int marker_b = 0x80;
    uint16_t dmacon = 0x8380;
    uint16_t beamcon0 = 0x0020;
    bool htotal = false;
    uint64_t cycles = 4'700'000;
    std::string bin = "prog_regs.bin";
};

struct PlaneLines {
    std::map<int64_t, uint32_t> first, last;
    std::map<int64_t, size_t> count;
    bool inline_two = true;             // consecutive fetches of one line are 2 bytes apart
};

struct Run89 {
    FetchRun fr;
    CopList cl{kCopList};
    PlaneLines pl[8];
    std::optional<int64_t> idx_a, idx_b;  // R2 line index of the markers $30 and marker_b
    int marker_b = 0x80;
};

PlaneLines plane_lines(const std::vector<Fetch>& v) {
    PlaneLines p;
    for (size_t i = 0; i < v.size(); ++i) {
        int64_t ln = v[i].line;
        if (!p.first.count(ln)) p.first[ln] = v[i].addr;
        if (i > 0 && v[i - 1].line == ln && v[i].addr - v[i - 1].addr != 2) p.inline_two = false;
        p.last[ln] = v[i].addr;
        ++p.count[ln];
    }
    return p;
}

std::optional<int64_t> marker_line_idx(const FetchRun& f, uint32_t move_word) {
    for (auto& a : f.cap.ram)
        if (!a.is_write && a.word_addr == move_word && a.cycle >= f.f3.begin && a.cycle < f.f3.end)
            return line_of(f.r2, a.cycle);
    return std::nullopt;
}

Run89 run_89(const Cfg89& c) {
    Run89 r;
    r.marker_b = c.marker_b;
    RunSpec s;
    s.bin = c.bin;
    CopList& cl = r.cl;
    if (c.diw_copper) cl.move(0x08E, 0x2C81);
    int planes = (c.bplcon0 & 0x0010) ? 8 : (c.bplcon0 >> 12) & 7;
    for (int k = 0; k < planes; ++k) cl.ptr(uint16_t(0x0E0 + 4 * k), kP89Plane + k * kP89Pitch);
    cl.marker(0x30);
    if (c.diw_copper) { cl.wait(c.diw_line); cl.move(0x08E, c.diw_second); }
    cl.marker(c.marker_b);
    cl.end();
    cl.poke_into(s.pokes);
    for (int k = 0; k < 8; ++k)
        for (uint32_t o = 0; o < kP89Pitch; o += 2)
            s.pokes.push_back({kP89Plane + k * kP89Pitch + o, uint16_t(0x8000 | ((o / 2) & 0x7FFF))});
    s1_step1(s.rl, c.beamcon0);
    if (c.htotal) { s.rl.add(0x1C0, 0x00E8); s.rl.add(0x1C8, 0x0138); }   // VARBEAMEN: lines of 233 CCK in place of 227
    s2_baseline(s.rl, c.bplcon0, c.fmode, c.diwstrt, kDdfstrtLores, kDdfstopLores, c.bpl1mod, c.bpl2mod);
    s.rl.add(0x102, 0x0000);
    s.rl.add(0x080, uint16_t(kCopList >> 16)); s.rl.add(0x082, uint16_t(kCopList & 0xFFFF)); s.rl.add(0x088, 0);
    s.rl.add(0x096, c.dmacon);
    s.rl.finish();
    s.cycles = c.cycles;
    r.fr = run_fetch(s, kCopList, kCopList + 0x100);   // the frame holding the first Copper fetch is the one the DMA enable falls in
    if (r.fr.f3_ok && r.fr.r2.ok) {
        for (int k = 0; k < 8; ++k)
            r.pl[k] = plane_lines(fetches_in(r.fr.cap, r.fr.r2, kP89Plane + k * kP89Pitch,
                                             kP89Plane + (k + 1) * kP89Pitch, r.fr.f3));
        r.idx_a = marker_line_idx(r.fr, r.cl.marker_move_word.at(0x30));
        r.idx_b = marker_line_idx(r.fr, r.cl.marker_move_word.at(c.marker_b));
    }
    return r;
}

// Absolute beam line of an R2 line index, from marker $30.
long long abs_line(const Run89& r, int64_t idx) { return 0x30 + (idx - *r.idx_a); }

struct Pop89 {
    bool all_planes = false;     // every plane fetched on the same contiguous lines, equal counts >= 2
    int64_t first = 0, last = 0; // first and last fetch line index
    size_t lines = 0;
    size_t per_line = 0;
};

Pop89 population(const Run89& r) {
    Pop89 p;
    const PlaneLines& a = r.pl[0];
    if (a.first.empty()) return p;
    p.first = a.first.begin()->first;
    p.last = a.first.rbegin()->first;
    p.lines = a.first.size();
    p.per_line = a.count.begin()->second;
    bool ok = p.per_line >= 2 && size_t(p.last - p.first + 1) == p.lines;
    for (int k = 0; k < 8 && ok; ++k) {
        const PlaneLines& q = r.pl[k];
        if (q.first.size() != p.lines) { ok = false; break; }
        auto ia = a.first.begin(); auto iq = q.first.begin();
        for (; ia != a.first.end(); ++ia, ++iq) if (ia->first != iq->first) ok = false;
        for (auto& kv : q.count) if (kv.second != p.per_line) ok = false;
    }
    p.all_planes = ok;
    return p;
}

// Step added at the end of line idx for plane k: first(idx+1) - last(idx).
std::optional<long long> step_at(const Run89& r, int k, int64_t idx) {
    auto a = r.pl[k].last.find(idx), b = r.pl[k].first.find(idx + 1);
    if (a == r.pl[k].last.end() || b == r.pl[k].first.end()) return std::nullopt;
    return (long long)b->second - (long long)a->second;
}

void prop_p8(Checks& checks, Grader& g) {
    Cfg89 m, mp;
    mp.bpl1mod = 0x0018; mp.bpl2mod = 0x0008;
    if (mutating("P8-M")) { m.bpl1mod = 0x0018; m.bpl2mod = 0x0008; }
    if (mutating("P8-Mp")) { mp.bpl1mod = 0x0008; mp.bpl2mod = 0x0018; }
    if (mutating("P8-POP")) m.bplcon0 = 0x4000;
    if (mutating("P8-R2")) { m.beamcon0 = 0x00A0; m.htotal = true; }
    if (mutating("P8-F3")) m.cycles = 3'300'000;
    if (mutating("P8-FIXTURE")) m.bin = "prog_regs_nofix.bin";
    if (mutating("P8-R3")) m.marker_b = 0x81;
    // A mutation names the run it changes; only that run is made.
    const std::string ms = g_mutate ? g_mutate : "";
    const bool dev = ms.empty() || ms == "P8-NONE";
    const bool ran_m = dev || ms != "P8-Mp", ran_p = dev || ms == "P8-Mp";
    Run89 rm, rp;
    if (ran_m) rm = run_89(m);
    if (ran_p) rp = run_89(mp);
    // the contrast of P8-R3 moves the marker in the stimulus; the check keeps $80
    std::vector<Run89*> runs;
    if (ran_m) runs.push_back(&rm);
    if (ran_p) runs.push_back(&rp);

    bool fixture = true, r2ok = true, f3ok = true, r3ok = true, pop = true;
    for (Run89* r : runs) {
        fixture = fixture && r->fr.cap.loaded && r->fr.cap.released && r->fr.cap.fixture_ok;
        r2ok = r2ok && r->fr.r2.ok;
        f3ok = f3ok && r->fr.f3_ok;
        bool r3 = r->idx_a && r->idx_b && (*r->idx_b - *r->idx_a) == (0x80 - 0x30);
        r3ok = r3ok && r3;
        if (r->idx_a && r->idx_b) checks.note("P8 R3: markers $30 and $80 are " + dec(*r->idx_b - *r->idx_a) + " line boundaries apart");
        Pop89 p = r->fr.f3_ok && r->fr.r2.ok ? population(*r) : Pop89{};
        bool first_ok = p.all_planes && r->idx_a && p.first == *r->idx_a - 4 && p.lines == 196;
        pop = pop && first_ok;
        checks.note("P8 population: " + dec((long long)p.lines) + " lines, " + dec((long long)p.per_line) + " fetches per line per plane");
    }
    g.expect(fixture, "P8-FIXTURE: every run loaded, booted and stored the $BEEF fixture before any observation");
    g.expect(r2ok, "P8-R2: precondition R2 anchor: in every run the refresh output comes in groups of exactly four equally spaced accesses, consecutive groups 226 to 229 CCK apart");
    g.expect(f3ok, "P8-F3: precondition frame F3: in every run the frame holding the first Copper-list fetch is found and its third successor F3 is complete");
    dep_expect(checks, r2ok && f3ok, r3ok, "P8-R3: precondition R3 anchor: the markers at lines $30 and $80 fall 80 line boundaries apart in every run", "R2 anchor or F3");
    dep_expect(checks, r2ok && f3ok, pop, "P8-POP: precondition population: in every run all eight planes are fetched on the same 196 consecutive lines of F3, with the same number of at least 2 fetches on each line, the first of them 4 lines before the marker of line $30", "R2 anchor or F3");
    bool pre = fixture && r2ok && f3ok && r3ok && pop;

    struct R { const char* name; Run89* run; int odd, even; const char* mut; const char* what; };
    const R rs[2] = {
        {"M", &rm, 10, 26, "P8-M", "BPL1MOD=$0008, BPL2MOD=$0018"},
        {"M'", &rp, 26, 10, "P8-Mp", "BPL1MOD=$0018, BPL2MOD=$0008"},
    };
    for (const R& x : rs) {
        if ((x.run == &rm && !ran_m) || (x.run == &rp && !ran_p)) continue;
        std::string label = std::string(x.mut) + " opus:AA-bpl1mod.c5 sol:AA-bplmod.c1 sol:AA-bplmod.c2: run " + x.name + " (" + x.what +
            ", scan-doubling off, AA p.16 'Modulos for the odd and even bit planes'): on every pair of consecutive window lines every odd plane steps " +
            dec(x.odd) + " bytes and every even plane " + dec(x.even) + " bytes from the last fetch of the first line to the first of the next, and fetches within a line are 2 bytes apart";
        if (!pre) { not_graded(checks, std::string("P8 run ") + x.name, "fixture, R2, F3, R3 anchor or population"); continue; }
        bool ok = true; size_t pairs = 0; std::map<long long, size_t> seen;
        for (int k = 0; k < 8; ++k) {
            ok = ok && x.run->pl[k].inline_two;
            for (int64_t i = x.run->pl[k].first.begin()->first; i < x.run->pl[k].first.rbegin()->first; ++i) {
                auto st = step_at(*x.run, k, i);
                long long want = (k % 2 == 0) ? x.odd : x.even;   // planes 1,3,5,7 are k = 0,2,4,6
                ++pairs; if (st) ++seen[*st];
                ok = ok && st && *st == want;
            }
        }
        g.expect(ok, label);
        std::string hist;
        for (auto& kv : seen) hist += " " + dec(kv.first) + "x" + dec((long long)kv.second);
        checks.note(std::string("P8 run ") + x.name + " measured steps (bytes x count) over " + dec((long long)pairs) + " plane-line pairs:" + hist);
    }
}

void prop_p9(Checks& checks, Grader& g) {
    struct Run { const char* name; uint16_t diwstrt; uint16_t fmode; bool copper; int first_line; };
    // D0, D1, D2 and the contrast N
    Cfg89 cfg[3];
    cfg[0].fmode = 0x4000;
    cfg[1].fmode = 0x4000; cfg[1].diwstrt = 0x2D81;
    cfg[2].fmode = 0x4000; cfg[2].diw_copper = true;
    if (mutating("P9-D0")) cfg[0].fmode = 0x0000;
    if (mutating("P9-D1")) cfg[1].fmode = 0x0000;
    if (mutating("P9-D2")) cfg[2].fmode = 0x0000;
    if (mutating("P9-FIRST")) cfg[0].diwstrt = 0x2E81;
    if (mutating("P9-POP")) cfg[0].bplcon0 = 0x4000;
    if (mutating("P9-R2")) { cfg[0].beamcon0 = 0x00A0; cfg[0].htotal = true; }
    if (mutating("P9-F3")) cfg[0].cycles = 3'300'000;
    if (mutating("P9-FIXTURE")) cfg[0].bin = "prog_regs_nofix.bin";
    if (mutating("P9-R3")) cfg[0].marker_b = 0x81;
    if (mutating("P9-AGG")) { cfg[1].bpl1mod = 0x0018; cfg[1].bpl2mod = 0x0008; }
    // A mutation names the runs it changes; only the runs it needs are made.
    const std::string ms = g_mutate ? g_mutate : "";
    const bool dev = ms.empty() || ms == "P9-NONE";
    bool ran[3] = {true, true, true};
    if (!dev) {
        ran[0] = ran[1] = ran[2] = false;
        if (ms == "P9-D1") ran[1] = true;
        else if (ms == "P9-D2") ran[2] = true;
        else if (ms == "P9-D0") ran[0] = true;
        else if (ms == "P9-AGG") ran[0] = ran[1] = ran[2] = true;
        else ran[0] = true;
    }
    Run89 r[3];
    for (int i = 0; i < 3; ++i) if (ran[i]) r[i] = run_89(cfg[i]);
    const char* names[3] = {"D0", "D1", "D2"};
    const int first_line[3] = {0x2C, 0x2D, 0x2C};

    bool fixture = true, r2ok = true, f3ok = true, r3ok = true, pop = true, firstok = true;
    for (int i = 0; i < 3; ++i) {
        if (!ran[i]) continue;
        fixture = fixture && r[i].fr.cap.loaded && r[i].fr.cap.released && r[i].fr.cap.fixture_ok;
        r2ok = r2ok && r[i].fr.r2.ok;
        f3ok = f3ok && r[i].fr.f3_ok;
        bool r3 = r[i].idx_a && r[i].idx_b && (*r[i].idx_b - *r[i].idx_a) == (0x80 - 0x30);
        r3ok = r3ok && r3;
        Pop89 p = r[i].fr.f3_ok && r[i].fr.r2.ok ? population(r[i]) : Pop89{};
        bool pp = p.all_planes && r[i].idx_a;
        pop = pop && pp;
        firstok = firstok && pp && p.lines == size_t(kLastWindowLine - first_line[i] + 1) && p.first == *r[i].idx_a + (first_line[i] - 0x30);
        checks.note(std::string("P9 ") + names[i] + " population: " + dec((long long)p.lines) + " lines, " + dec((long long)p.per_line) + " fetches per line per plane");
    }
    g.expect(fixture, "P9-FIXTURE: every run loaded, booted and stored the $BEEF fixture before any observation");
    g.expect(r2ok, "P9-R2: precondition R2 anchor: in every run the refresh output comes in groups of exactly four equally spaced accesses, consecutive groups 226 to 229 CCK apart");
    g.expect(f3ok, "P9-F3: precondition frame F3: in every run the frame holding the first Copper-list fetch is found and its third successor F3 is complete");
    dep_expect(checks, r2ok && f3ok, r3ok, "P9-R3: precondition R3 anchor: the markers at lines $30 and $80 fall 80 line boundaries apart in every run", "R2 anchor or F3");
    dep_expect(checks, r2ok && f3ok, pop, "P9-POP: precondition population: in every run all eight planes are fetched on the same consecutive lines of F3, with the same number of at least 2 fetches on each line", "R2 anchor or F3");
    dep_expect(checks, r2ok && f3ok && pop, firstok, "P9-FIRST: precondition first fetch line (AA p.21 'The vertical Bit Plane DMA timing is identical to the Display windows'): the first fetch line is $2C in D0 and D2 and $2D in D1, and the fetches run through line $EF", "R2 anchor, F3 or population");
    bool pre = fixture && r2ok && f3ok && r3ok && pop && firstok;

    bool comp[3] = {false, false, false};
    for (int i = 0; i < 3; ++i) {
        if (!ran[i]) continue;
        std::string label = std::string("P9-") + names[i] + " run " + names[i] +
            (i == 0 ? " (DIWSTRT=$2C81, FMODE=$4000)" : i == 1 ? " (DIWSTRT=$2D81, FMODE=$4000)" : " (DIWSTRT=$2C81 then $2D81 from line $60 on, FMODE=$4000)") +
            ": with BSCAN2 set, on every window line n every plane steps by the same modulus, 10 bytes (BPL1MOD) when n and the DIWSTRT start line agree in bit 0 and 26 bytes (BPL2MOD) when they differ, the steps alternating between the two";
        if (!pre) { not_graded(checks, std::string("P9 run ") + names[i], "fixture, R2, F3, R3 anchor, population or first fetch line"); continue; }
        bool ok = true; size_t graded = 0; std::map<long long, size_t> seen;
        int diw_lsb0 = 0;   // DIWSTRT start line $2C: bit 0 = 0
        for (int k = 0; k < 8; ++k) {
            ok = ok && r[i].pl[k].inline_two;
            for (int64_t idx = r[i].pl[k].first.begin()->first; idx < r[i].pl[k].first.rbegin()->first; ++idx) {
                long long n = abs_line(r[i], idx);
                int lsb;
                if (i == 0) lsb = diw_lsb0; else if (i == 1) lsb = 1;
                else { if (n == 0x60) continue; lsb = (n <= 0x5F) ? 0 : 1; }
                bool primary = ((n & 1) == lsb);
                long long want = primary ? 10 : 26;
                auto st = step_at(r[i], k, idx);
                ++graded; if (st) ++seen[*st];
                ok = ok && st && *st == want;
            }
        }
        comp[i] = ok;
        g.expect(ok, label);
        std::string hist;
        for (auto& kv : seen) hist += " " + dec(kv.first) + "x" + dec((long long)kv.second);
        checks.note(std::string("P9 run ") + names[i] + " measured steps (bytes x count) over " + dec((long long)graded) + " graded plane-line pairs:" + hist);
    }
    if (!(ran[0] && ran[1] && ran[2])) return;
    if (pre) {
        g.expect(comp[0] && comp[1] && comp[2],
            "P9-AGG opus:AA-bpl1mod.c7 opus:AA-bpl1mod.c8 sol:AA-bplmod.c5 sol:AA-bplmod.c6 sol:AA-bplmod.c7 sol:AA-bplmod.c8 opus:AA-bitplanes.c6 sol:AA-dual-playfield.c3: "
            "with scan-doubling enabled the modulus of every plane on a line is BPL1MOD on a line whose beam-counter bit 0 equals DIWSTRT's and BPL2MOD on the others, in runs D0, D1 and D2 (AA p.16 and p.3)");
    } else {
        not_graded(checks, "P9 aggregate", "fixture, R2, F3, R3 anchor, population or first fetch line");
    }
}

// ---- P6: BYPASS ------------------------------------------------------------

constexpr uint32_t kP6Plane = 0x100000, kP6PlanePitch = 0x1000;
constexpr uint32_t kP6Sprite = 0x0B4000, kP6Zero = 0x0B4100;

struct P6Cfg {
    bool bypass = false;
    bool tx = false;                  // table T_x instead of T_id
    bool half_table = false;          // T_id with the red nibble limited to 0-7
    uint16_t bplcon1 = 0x0033;
    uint16_t bplcon2 = 0x0024;
    uint16_t dmacon = 0x83A0;
    uint16_t beamcon0 = 0x0020;
    uint64_t cycles = 4'700'000;
    std::string bin = "prog_regs.bin";
};

uint16_t p6_entry(int i, bool tx, bool half) {
    int hi = i >> 4, lo = i & 15;
    int r = tx ? 15 - hi : hi;
    if (half) r &= 7;
    int b = tx ? hi : 15 - hi;
    return uint16_t((r << 8) | (lo << 4) | b);
}

int p6_value(int g) { int m = g % 16; return (m << 4) | (15 - m); }

RunSpec p6_spec(const P6Cfg& c) {
    RunSpec s;
    s.bin = c.bin; s.cycles = c.cycles;
    CopList cl(kCopList);
    for (int k = 0; k < 8; ++k) cl.ptr(uint16_t(0x0E0 + 4 * k), kP6Plane + k * kP6PlanePitch);
    cl.ptr(0x120, kP6Sprite);
    for (int x = 1; x < 8; ++x) cl.ptr(uint16_t(0x120 + 4 * x), kP6Zero);
    cl.end();
    cl.poke_into(s.pokes);
    // plane k holds bit k-1 of v(g) for the 20 groups of every line; the
    // modulus of -40 bytes returns each pointer to the line's start
    for (int k = 0; k < 8; ++k)
        for (int g = 0; g < 20; ++g)
            s.pokes.push_back({kP6Plane + k * kP6PlanePitch + 2 * g, uint16_t(((p6_value(g) >> k) & 1) ? 0xFFFF : 0x0000)});
    std::vector<uint16_t> spr = {0x6060, 0x7000};
    for (int i = 0; i < 16; ++i) { spr.push_back(0xFFFF); spr.push_back(0x0000); }
    spr.push_back(0); spr.push_back(0);
    poke_words(s.pokes, kP6Sprite, spr);
    s1_step1(s.rl, c.beamcon0);
    s.rl.add(0x100, uint16_t(0x0010 | (c.bypass ? 0x0020 : 0)));
    s.rl.add(0x102, c.bplcon1);
    s.rl.add(0x104, c.bplcon2);
    s.rl.add(0x10C, 0x0011);
    s.rl.add(0x08E, 0x2C81); s.rl.add(0x090, 0xF0C1);
    s.rl.add(0x092, kDdfstrtLores); s.rl.add(0x094, kDdfstopLores);
    s.rl.add(0x108, 0xFFD8); s.rl.add(0x10A, 0xFFD8);
    s.rl.add(0x1FC, 0x0000);
    for (int b = 0; b < 8; ++b) {
        s.rl.add(0x106, uint16_t(0x0C00 | (b << 13)));
        for (int j = 0; j < 32; ++j) s.rl.add(uint16_t(0x180 + 2 * j), p6_entry(32 * b + j, c.tx, c.half_table));
    }
    s.rl.add(0x106, 0x0C00);
    s.rl.add(0x080, uint16_t(kCopList >> 16)); s.rl.add(0x082, uint16_t(kCopList & 0xFFFF)); s.rl.add(0x088, 0);
    s.rl.add(0x096, c.dmacon);
    s.rl.finish();
    return s;
}

// R4: one LORES plane of $AAAA, COLOR01 written with LOCT=0 and then LOCT=1.
RunSpec p6_r4_spec(bool swapped) {
    RunSpec s;
    s.cycles = 4'700'000;
    CopList cl(kCopList);
    cl.ptr(0x0E0, kP6Plane);
    cl.end();
    cl.poke_into(s.pokes);
    for (int g = 0; g < 20; ++g) s.pokes.push_back({kP6Plane + 2 * g, 0xAAAA});
    s1_step1(s.rl);
    s.rl.add(0x100, 0x1000);
    s.rl.add(0x102, 0x0000);
    s.rl.add(0x104, 0x0024);
    s.rl.add(0x10C, 0x0011);
    s.rl.add(0x08E, 0x2C81); s.rl.add(0x090, 0xF0C1);
    s.rl.add(0x092, kDdfstrtLores); s.rl.add(0x094, kDdfstopLores);
    s.rl.add(0x108, 0xFFD8); s.rl.add(0x10A, 0xFFD8);
    s.rl.add(0x1FC, 0x0000);
    s.rl.add(0x106, 0x0C00);
    s.rl.add(0x180, 0x0222);
    s.rl.add(0x182, swapped ? 0x0500 : 0x0A00);
    s.rl.add(0x106, 0x0E00);
    s.rl.add(0x182, swapped ? 0x0A00 : 0x0500);
    s.rl.add(0x106, 0x0C00);
    s.rl.add(0x080, uint16_t(kCopList >> 16)); s.rl.add(0x082, uint16_t(kCopList & 0xFFFF)); s.rl.add(0x088, 0);
    s.rl.add(0x096, 0x8380);
    s.rl.finish();
    return s;
}

// The lines of a frame, each as its samples.
using Img = std::vector<std::vector<VideoSample>>;

Img frame_img(const Cap& c, Span fr) {
    Img img;
    for (auto& ln : nmh::find_lines(c.video)) {
        if (ln.begin < fr.begin || ln.end > fr.end) continue;
        img.emplace_back(c.video.begin() + ln.begin, c.video.begin() + ln.end);
    }
    return img;
}

bool is_window_px(const VideoSample& s) {
    // neither the blanked level (0,0,0) nor COLOR00 of table T_id, whose entry 0 is red 0, green 0, blue $F
    return !(s.r == 0 && s.g == 0 && s.b == 0) && !(s.r == 0 && s.g == 0 && s.b == 15);
}

struct P6Run {
    Cap cap;
    std::optional<size_t> x;
    bool f3_ok = false;
    Span f3{0, 0};
    Img img;
    size_t shape_lines = 0; double shape_period = 0, shape_width = 0;
};

P6Run run_p6(const RunSpec& s, uint32_t plane_lo, uint32_t plane_hi) {
    P6Run r;
    r.cap = run_spec(s);
    r.x = enable_frame(r.cap, plane_lo, plane_hi);
    auto frames = nmh::find_frames(r.cap.video);
    if (r.x && *r.x + 3 + 1 < frames.size()) {
        r.f3_ok = true; r.f3 = frames[*r.x + 3];
        r.img = frame_img(r.cap, r.f3);
        HsyncShape h = hsync_shape(r.cap.video, *r.x + 3);
        r.shape_lines = h.line_count; r.shape_period = h.avg_period; r.shape_width = h.avg_width;
    }
    std::vector<VideoSample>().swap(r.cap.video);   // the frame's lines are kept in img
    return r;
}

bool same_shape(const P6Run& a, const P6Run& b) {
    return a.f3_ok && b.f3_ok && a.shape_lines > 0 && a.shape_lines == b.shape_lines &&
           near_int(a.shape_period, b.shape_period, 1.0) && near_int(a.shape_width, b.shape_width, 1.0);
}

// Compares the red channel of two images at the pixels of `mask`; returns the
// number of pixels compared and how many differ. `rows_ok` is false when the
// images do not have the same number of lines or a line is too short.
struct RedCmp { size_t compared = 0, differ = 0; bool rows_ok = true; };

RedCmp compare_red(const Img& a, const Img& b, const Img& mask_from) {
    RedCmp r;
    if (a.size() != b.size() || a.size() != mask_from.size()) { r.rows_ok = false; return r; }
    for (size_t l = 0; l < a.size(); ++l) {
        for (size_t c = 0; c < mask_from[l].size(); ++c) {
            if (!is_window_px(mask_from[l][c])) continue;
            if (c >= a[l].size() || c >= b[l].size()) { r.rows_ok = false; continue; }
            ++r.compared;
            if (a[l][c].r != b[l][c].r) ++r.differ;
        }
    }
    return r;
}

void prop_p6(Checks& checks, Grader& g) {
    // runs: B0 and B1 with table T_id and T_x, plus the three for the preconditions
    P6Cfg b0id, b1id, b0x, b1x, b0n, b0s0;
    b1id.bypass = true; b0x.tx = true; b1x.bypass = true; b1x.tx = true;
    b0n.dmacon = 0x8380;                // SPREN cleared: no sprite
    b0s0.bplcon1 = 0x0000;              // no scroll
    if (mutating("P6-FIXTURE")) b0id.bin = "prog_regs_nofix.bin";
    if (mutating("P6-R1")) b1id.beamcon0 = 0x0000;
    if (mutating("P6-F3")) b0id.cycles = 3'300'000;
    if (mutating("P6-TBL")) b0x.tx = false;
    if (mutating("P6-RED16")) b0id.half_table = true;
    if (mutating("P6-SPR")) b0id.dmacon = 0x8380;
    if (mutating("P6-SCROLL")) b0id.bplcon1 = 0x0000;
    if (mutating("P6-POP")) b0id.dmacon = 0x8280;
    if (mutating("P6-I")) b1id.bplcon1 = 0x0000;
    if (mutating("P6-I-PRIO")) b1id.bplcon2 = 0x0000;
    // P6-II and P6-AGG: a live input through the same comparators: the B1/T_x run is run as B0/T_id
    if (mutating("P6-II") || mutating("P6-AGG")) { b1x.bypass = false; b1x.tx = false; }
    const uint32_t lo = kCopList, hi = kCopList + 0x100;   // the frame holding the first Copper fetch is the one the DMA enable falls in
    // A mutation names the runs it changes; the runs it needs are the only ones made. B0/T_id is always made.
    const std::string ms = g_mutate ? g_mutate : "";
    const bool dev = ms.empty() || ms == "P6-NONE" || ms == "P6-II" || ms == "P6-AGG";
    const bool nb1 = dev || ms == "P6-R1" || ms == "P6-I" || ms == "P6-I-PRIO";
    const bool nx1 = dev;
    const bool nx0 = dev || ms == "P6-TBL";
    const bool nn = dev || ms == "P6-SPR";
    const bool ns0 = dev || ms == "P6-SCROLL";
    const bool nr4 = dev || ms == "P6-R4";
    P6Run rid0 = run_p6(p6_spec(b0id), lo, hi), rid1, rx0, rx1, rn, rs0, r4;
    std::vector<P6Run*> all = {&rid0};
    if (nb1) { rid1 = run_p6(p6_spec(b1id), lo, hi); all.push_back(&rid1); }
    if (nx0) { rx0 = run_p6(p6_spec(b0x), lo, hi); all.push_back(&rx0); }
    if (nx1) { rx1 = run_p6(p6_spec(b1x), lo, hi); all.push_back(&rx1); }
    if (nn) { rn = run_p6(p6_spec(b0n), lo, hi); all.push_back(&rn); }
    if (ns0) { rs0 = run_p6(p6_spec(b0s0), lo, hi); all.push_back(&rs0); }

    // ---- R4, run with COLOR01 written in two parts ----
    bool r4_ok = true;
    if (nr4) {
        r4 = run_p6(p6_r4_spec(mutating("P6-R4")), kCopList, kCopList + 0x100);
        std::set<int> r4_reds;
        if (r4.f3_ok)
            for (auto& row : r4.img) for (auto& px : row) if (px.g == 0 && px.b == 0 && px.r != 0) r4_reds.insert(px.r);
        r4_ok = r4_reds.size() == 1 && *r4_reds.begin() == 0xA;
        std::string r4_note;
        for (int v : r4_reds) r4_note += " $" + hexw(v, 1);
        g.expect(r4_ok, "P6-R4: precondition R4: a COLOR01 pixel written $0A00 with LOCT=0 and then $0500 with LOCT=1 (AA p.19) shows red $A, so the 4-bit output carries R7-R4");
        checks.note("P6 R4: red values of COLOR01 pixels:" + r4_note);
        if (ms == "P6-R4") return;
    }

    bool fixture = true, f3ok = !nr4 || r4.f3_ok;
    for (P6Run* r : all) { fixture = fixture && r->cap.loaded && r->cap.released && r->cap.fixture_ok; f3ok = f3ok && r->f3_ok; }
    g.expect(fixture, "P6-FIXTURE: all runs loaded, booted and stored the $BEEF fixture before any observation");
    g.expect(f3ok, "P6-F3: precondition frame F3: in every run the frame holding the first Copper-list fetch is found and its third successor F3 is complete");
    bool r1ok = f3ok;
    for (P6Run* r : all) r1ok = r1ok && same_shape(rid0, *r);
    dep_expect(checks, f3ok, r1ok, "P6-R1: precondition R1 anchor: hsync period, hsync width and output-line count per frame are equal in every compared run", "F3");
    if (!(fixture && f3ok && r1ok)) {
        not_graded(checks, "P6 preconditions and assertions", "fixture, F3 or R1 anchor");
        return;
    }

    // observed pixels: those of B0/T_id whose colour is not COLOR00's or the blanked level
    size_t px_n = 0; std::set<size_t> rows_with; std::map<size_t, size_t> per_row;
    for (size_t l = 0; l < rid0.img.size(); ++l)
        for (auto& px : rid0.img[l]) if (is_window_px(px)) { ++px_n; rows_with.insert(l); ++per_row[l]; }
    bool equal_cols = true;
    for (auto& kv : per_row) if (kv.second != per_row.begin()->second) equal_cols = false;
    bool pop = rows_with.size() >= 196 && equal_cols;
    g.expect(pop, "P6-POP: precondition population: the window of B0/T_id covers at least 196 lines of F3 with the same number of window columns on each");
    checks.note("P6 population: " + dec((long long)rows_with.size()) + " lines, " + dec(per_row.empty() ? 0 : (long long)per_row.begin()->second) + " window columns on a line");

    // table matters: red(B0/T_x) != red(B0/T_id) at every observed pixel
    bool tbl_ok = true;
    if (nx0) {
        RedCmp tbl = compare_red(rid0.img, rx0.img, rid0.img);
        tbl_ok = pop && tbl.rows_ok && tbl.compared > 0 && tbl.differ == tbl.compared;
        g.expect(tbl_ok, "P6-TBL: precondition table: the red of B0/T_x differs from that of B0/T_id at every observed pixel, so the colour table decides the red without BYPASS");
        checks.note("P6 table: " + dec((long long)tbl.differ) + " of " + dec((long long)tbl.compared) + " pixels differ between B0/T_x and B0/T_id");
    }

    // all 16 red values appear in B0/T_id
    std::set<int> reds;
    for (auto& row : rid0.img) for (auto& px : row) if (is_window_px(px)) reds.insert(px.r);
    g.expect(reds.size() == 16 && *reds.begin() == 0 && *reds.rbegin() == 15, "P6-RED16: precondition values: B0/T_id shows all 16 red values in the window");

    // the sprite's columns: where B0/T_id differs from the run without sprite DMA
    size_t spr_rows = 0; bool spr_ok = true;
    bool spr_over_other = false;
    if (nn) spr_ok = pop && rid0.img.size() == rn.img.size();
    if (nn && spr_ok) {
        for (size_t l = 0; l < rid0.img.size(); ++l) {
            size_t n = 0;
            for (size_t c = 0; c < rid0.img[l].size() && c < rn.img[l].size(); ++c) {
                if (rid0.img[l][c].r == rn.img[l][c].r) continue;
                ++n;
                if (rid0.img[l][c].r != 1) spr_ok = false;
                if (rn.img[l][c].r != 1) spr_over_other = true;
            }
            if (n) { ++spr_rows; if (n != 32) spr_ok = false; }
        }
        spr_ok = spr_ok && spr_rows > 0 && spr_rows % 16 == 0 && spr_over_other;
    }
    if (nn) {
        g.expect(spr_ok, "P6-SPR: precondition sprite: on lines $60-$6F sprite 0's 32 columns show red 1 in B0/T_id, including over groups whose high nibble is not 1");
        checks.note("P6 sprite: " + dec((long long)spr_rows) + " output lines carry sprite columns");
    }

    // scroll in effect: boundaries of B0/T_id lie 6 op right of those with BPLCON1 = $0000
    bool scroll_ok = true;
    if (ns0) {
        scroll_ok = false;
        size_t l = *rows_with.begin();
        auto bounds = [&](const P6Run& r) {
            std::vector<size_t> b;
            const auto& row = r.img[l];
            size_t first = 0, last = 0; bool seen = false;
            for (size_t c = 0; c < rid0.img[l].size() && c < row.size(); ++c)
                if (is_window_px(rid0.img[l][c])) { if (!seen) first = c; last = c; seen = true; }
            for (size_t c = first + 1; seen && c <= last && c < row.size(); ++c)
                if (row[c].r != row[c - 1].r) b.push_back(c);
            return b;
        };
        std::vector<size_t> ub = bounds(rs0), sb = bounds(rid0);
        scroll_ok = ub.size() >= 10 && !sb.empty();
        for (size_t u : ub) if (std::find(sb.begin(), sb.end(), u + 6) == sb.end()) scroll_ok = false;
        checks.note("P6 scroll: " + dec((long long)ub.size()) + " group boundaries without scroll, " + dec((long long)sb.size()) + " with BPLCON1=$0033, line index " + dec((long long)l));
        g.expect(scroll_ok, "P6-SCROLL: precondition scroll: every group boundary of B0/T_id lies 6 op (6 samples) to the right of the one with BPLCON1 = $0000");
    }

    bool pre = r4_ok && pop && tbl_ok && reds.size() == 16 && spr_ok && scroll_ok;
    if (!nb1) return;
    if (!pre) { not_graded(checks, "P6 assertions", "R4, population, table, values, sprite or scroll precondition"); return; }

    RedCmp i = compare_red(rid1.img, rid0.img, rid0.img);
    bool i_ok = i.rows_ok && i.compared > 0 && i.differ == 0;
    g.expect(i_ok, "P6-I: with BYPASS set (BPLCON0 = $0030) and table T_id the red of every observed pixel equals that of the run without BYPASS (BPLCON0 = $0010), at BPLCON1 = $0033 and with sprite 0 in front");
    checks.note("P6 (i): " + dec((long long)i.differ) + " of " + dec((long long)i.compared) + " pixels differ between B1/T_id and B0/T_id");
    if (!nx1) return;
    RedCmp ii = compare_red(rx1.img, rid1.img, rid0.img);
    bool ii_ok = ii.rows_ok && ii.compared > 0 && ii.differ == 0;
    g.expect(ii_ok, "P6-II: with BYPASS set the red of every observed pixel is the same with table T_x as with table T_id");
    checks.note("P6 (ii): " + dec((long long)ii.differ) + " of " + dec((long long)ii.compared) + " pixels differ between B1/T_x and B1/T_id");
    g.expect(i_ok && ii_ok, "P6-AGG sol:AA-bplcon0.c19: with BYPASS set the red output carries the address bits 7-4 chosen by scrolling and priority, as without BYPASS, and does not use the colour table (AA p.17 'bitplanes are scrolled and prioritized normally, but bypass color table and 8 bit wide data appear on R(7:0)')");
}

// ---- P10-P15 helpers -------------------------------------------------------

// Time units. One CCK is 280 ns (AA p.21: "memory cycles are 280 ns"); K, the
// clk_sys cycles per CCK, is measured by the unit run.
uint64_t g_K = 0;

uint64_t ns_to_cycles(double ns) { return (uint64_t)std::llround(ns / 280.0 * double(g_K)); }
uint64_t us_to_cycles(double us) { return ns_to_cycles(us * 1000.0); }
uint64_t ms_to_cycles(double ms) { return ns_to_cycles(ms * 1e6); }

// Writes of one word address, in order: (cycle, data).
std::vector<std::pair<uint64_t, uint16_t>> writes_to(const Cap& c, uint32_t byte_addr) {
    std::vector<std::pair<uint64_t, uint16_t>> v;
    for (auto& a : c.ram) if (a.is_write && a.word_addr * 2 == byte_addr) v.push_back({a.cycle, a.data});
    return v;
}

bool reached_done(const Cap& c) { return c.done; }

constexpr uint64_t kCpuStart = 2'450'000;  // cycles after boot() before the CPU has run its first instruction (measured, asserted by each run's fixture)

// ---- the unit run: K, from the refresh accesses of a run with no DMA --------

void prop_unit(Checks& checks, Grader& g) {
    RunSpec s;
    s.cycles = kCpuStart + 100'000;
    s.chipset = mutating("UNIT-AA") ? 0x08 : 0x10;
    const bool varb = mutating("UNIT-R2") || mutating("P11-UNIT") || mutating("P12-UNIT") || mutating("P14-UNIT");
    s1_step1(s.rl, varb ? 0x00A0 : 0x0020);
    if (varb) s.rl.add(0x1C0, 0x0040);
    s.rl.finish();
    if (mutating("UNIT-FIXTURE")) s.bin = "prog_regs_nofix.bin";
    Cap c = run_spec(s);
    R2 r = analyse_r2(c);
    bool fixture = c.loaded && c.released && c.fixture_ok;
    g.expect(fixture, "UNIT-FIXTURE: the unit run loaded, booted and stored the $BEEF fixture before any observation");
    dep_expect(checks, fixture, (c.results4 & 0xFF) == 0xF8, "UNIT-AA: LISAID ($DFF07C) reads $F8 in its low byte, which AA p.26 gives for the AA chip set ('LISA returns hex (F8)'; the ECS Denise returns $FC), so the AA chip set is selected", "fixture");
    checks.note("UNIT: LISAID reads $" + hexw(c.results4));
    dep_expect(checks, fixture, r.ok, "UNIT-R2: the refresh output of a run with no DMA enabled comes in groups of exactly four equally spaced accesses, consecutive groups 226 to 229 CCK apart (HRM p.194: 227.5 colour clocks per line; Figure 6-9)", "fixture");
    checks.note("UNIT: K = " + dec((long long)r.K) + " clk_sys cycles per CCK; " + dec((long long)r.groups) + " lines; line spacing " + num(r.gap_min) + " to " + num(r.gap_max) + " CCK");
    g_K = (fixture && r.ok) ? r.K : 0;
}

// ---- P10: LPEN is off after power-up ---------------------------------------

constexpr uint32_t kLpOut = 0x0B9000, kLpMarks = 0x0B9100;

struct LpRun {
    Cap cap;
    R2 r2;
    uint32_t w0[3] = {}, w1[3] = {};
    uint64_t t_v[3] = {}, t_w1[3] = {};
    bool have = false;
};

LpRun run_lpen(const std::string& bin, uint64_t cycles = 4'700'000) {
    LpRun l;
    RunSpec s;
    s.bin = bin; s.cycles = cycles;
    l.cap = run_spec(s);        // no register list: the program writes its own registers; the list area is left empty
    l.r2 = analyse_r2(l.cap);
    l.have = true;
    for (int i = 0; i < 3; ++i) {
        auto m = writes_to(l.cap, kLpMarks + 2 * i);
        auto hi0 = writes_to(l.cap, kLpOut + 8 * i), lo0 = writes_to(l.cap, kLpOut + 8 * i + 2);
        auto hi1 = writes_to(l.cap, kLpOut + 8 * i + 4), lo1 = writes_to(l.cap, kLpOut + 8 * i + 6);
        if (m.empty() || hi0.empty() || lo0.empty() || hi1.empty() || lo1.empty()) { l.have = false; continue; }
        l.t_v[i] = m.front().first;
        l.t_w1[i] = hi1.front().first;
        l.w0[i] = (uint32_t(hi0.front().second) << 16) | lo0.front().second;
        l.w1[i] = (uint32_t(hi1.front().second) << 16) | lo1.front().second;
    }
    return l;
}

unsigned lp_v(uint32_t w) { return ((w >> 16) & 1u) << 8 | ((w >> 8) & 0xFFu); }

void prop_p10(Checks& checks, Grader& g) {
    // P10-MAIN: both runs read once and store the value twice, so the check goes red while the latch precondition holds;
    // P10-LATCH: the contrast run is that same program, a live input through the latch comparator
    LpRun m = run_lpen(mutating("P10-MAIN") ? "prog_lpen_same.bin" : mutating("P10-FIXTURE") ? "prog_lpen_nofix.bin" :
                       mutating("P10-R2") ? "prog_lpen_var.bin" : mutating("P10-WIN") ? "prog_lpen_late.bin" : "prog_lpen.bin");
    LpRun c = run_lpen(mutating("P10-LATCH") || mutating("P10-MAIN") ? "prog_lpen_same.bin" : "prog_lpen_c.bin");
    bool fixture = m.cap.loaded && m.cap.released && m.cap.fixture_ok && m.cap.done && m.have &&
                   c.cap.loaded && c.cap.released && c.cap.fixture_ok && c.cap.done && c.have;
    g.expect(fixture, "P10-FIXTURE: both runs loaded, booted, stored the $BEEF fixture, read the registers three times and reached DONE");
    g.expect(m.r2.ok && c.r2.ok, "P10-R2: precondition R2 anchor: in both runs the refresh output comes in groups of exactly four equally spaced accesses, consecutive groups 226 to 229 CCK apart");
    bool win = fixture && m.r2.ok;
    for (int i = 0; win && i < 3; ++i) {
        int64_t a = line_of(m.r2, m.t_v[i]), b = line_of(m.r2, m.t_w1[i]);
        checks.note("P10 pair " + dec(i) + ": " + dec(b - a) + " line boundaries between the marker and the store of w1");
        if (!(b - a < 19)) win = false;
    }
    dep_expect(checks, fixture && m.r2.ok, win, "P10-WIN: precondition (i): in every pair fewer than 19 R2 line boundaries lie between the marker store (the start of vertical blank, AA p.25 VERTB) and the store of w1", "fixture or R2 anchor");
    bool latch = fixture && c.r2.ok;
    for (int i = 0; latch && i < 3; ++i) if (c.w1[i] != c.w0[i]) latch = false;
    dep_expect(checks, fixture, latch, "P10-LATCH: precondition (ii): with BPLCON0 = $0008 (LPEN = 1) written before the reads, w1 equals w0 in every pair, so the latch freezes the counter in this system", "fixture");
    checks.record("P10 precondition (iii): the program source of the main run contains no write to BPLCON0 ($DFF100); the contrast build adds that one write");
    for (int i = 0; fixture && i < 3; ++i)
        checks.note("P10 main pair " + dec(i) + ": w0=$" + hexw(m.w0[i], 8) + " (V=" + dec(lp_v(m.w0[i])) + ") w1=$" + hexw(m.w1[i], 8) +
                    "; LPEN run: w0=$" + hexw(c.w0[i], 8) + " w1=$" + hexw(c.w1[i], 8));
    if (!(fixture && m.r2.ok && win && latch)) { not_graded(checks, "P10 main check", "fixture, R2 anchor, vertical-blank window or latch precondition"); return; }
    bool ok = true;
    for (int i = 0; i < 3; ++i) ok = ok && m.w1[i] != m.w0[i] && lp_v(m.w0[i]) < 20;
    g.expect(ok, "P10-MAIN opus:AA-lpen.c1: after power-on and before any write to BPLCON0, in every one of three reads within vertical blank, VPOSR:VHPOSR reads differently 40 CCK later and its vertical position is below 20 (AA p.17 'LPEN =Light pen enable(reset on power up)')");
}

// ---- P11: JOY0DAT and JOY1DAT ----------------------------------------------

constexpr uint32_t kJoyA = 0x0D0000, kJoyB = 0x0D8000, kJoyCount = 7200;

// One entry per loop iteration: the store times of the JOY0DAT read (t) and of the JOY1DAT read (t2) and the two values.
// Both reads precede their stores, so an iteration lies before a time T when t2 < T.
struct Reads { std::vector<uint64_t> t, t2; std::vector<uint16_t> v0, v1; };

Reads joy_reads(const Cap& c) {
    Reads r;
    std::vector<std::pair<uint64_t, uint16_t>> a(kJoyCount, {0, 0}), b(kJoyCount, {0, 0});
    std::vector<uint8_t> ha(kJoyCount, 0), hb(kJoyCount, 0);
    for (auto& x : c.ram) {
        if (!x.is_write) continue;
        uint32_t byte = x.word_addr * 2;
        if (byte >= kJoyA && byte < kJoyA + 2 * kJoyCount) { size_t k = (byte - kJoyA) / 2; a[k] = {x.cycle, x.data}; ha[k] = 1; }
        if (byte >= kJoyB && byte < kJoyB + 2 * kJoyCount) { size_t k = (byte - kJoyB) / 2; b[k] = {x.cycle, x.data}; hb[k] = 1; }
    }
    for (size_t k = 0; k < kJoyCount; ++k) {
        if (!ha[k] || !hb[k]) break;
        r.t.push_back(a[k].first); r.t2.push_back(b[k].first); r.v0.push_back(a[k].second); r.v1.push_back(b[k].second);
    }
    return r;
}

struct JoyState { const char* name; uint8_t bits; int f, l, b, r; };
// joystick0/joystick1 bits: fire4, fire3, fire2, fire, up, down, left, right
const JoyState kJoyStates[9] = {
    {"none", 0x00, 0, 0, 0, 0}, {"up", 0x08, 1, 0, 0, 0}, {"down", 0x04, 0, 0, 1, 0},
    {"left", 0x02, 0, 1, 0, 0}, {"right", 0x01, 0, 0, 0, 1}, {"up+left", 0x0A, 1, 1, 0, 0},
    {"up+right", 0x09, 1, 0, 0, 1}, {"down+left", 0x06, 0, 1, 1, 0}, {"down+right", 0x05, 0, 0, 1, 1},
};

struct Decoded { int f, l, b, r; };
Decoded decode_joy(uint16_t v) {
    int b9 = (v >> 9) & 1, b8 = (v >> 8) & 1, b1 = (v >> 1) & 1, b0 = v & 1;
    return {b9 ^ b8, b9, b1 ^ b0, b1};
}

struct JoyRun {
    Cap cap;
    Reads rd;
    uint64_t t0 = 0;
    std::vector<uint64_t> change;   // absolute cycle of each of the 18 state changes
};

void prop_p11(Checks& checks, Grader& g) {
    g.expect(g_K > 0, "P11-UNIT: precondition: the CCK unit K was measured by the unit run before the 2 ms, 0.2 ms and 50 us durations were derived");
    if (g_K == 0) { not_graded(checks, "P11 checks", "unit"); return; }
    const uint64_t hold = ms_to_cycles(2.0), settle = ms_to_cycles(0.2);
    // ---- (a) joystick ----
    JoyRun ja;
    {
        RunSpec s;
        s.bin = mutating("P11-FIXTURE") ? "prog_joy_nofix.bin" : "prog_joy.bin";
        s.cycles = 3'900'000;
        uint64_t holdc = mutating("P11-POP") ? ms_to_cycles(0.15) : hold;
        auto t0 = std::make_shared<int64_t>(-1);
        auto changes = std::make_shared<std::vector<uint64_t>>();
        s.drive = [=, &ja](nmh::NanomigHarness& h, uint64_t i) {
            if (*t0 < 0) { auto fx = h.peek_logical(kResults); if (fx && *fx == 0xBEEF) { *t0 = (int64_t)i; } else return; }
            uint64_t rel = i - (uint64_t)*t0;
            if (rel % holdc != 0) {
                if (mutating("P11-STILL") && rel == 3 * holdc + holdc / 2) {
                    h.ports()->kbd_mouse_type = 0; h.ports()->kbd_mouse_data = 1; h.ports()->kbd_mouse_level = !h.ports()->kbd_mouse_level;
                }
                return;
            }
            uint64_t k = rel / holdc;
            if (k >= 18) return;
            const JoyState& st = kJoyStates[k % 9];
            uint8_t bits = st.bits;
            if (mutating("P11-JOY") && std::string(st.name) == "right") bits = 0x02;
            // connector 0 (JOY0DAT, the left controller pair, AA p.25) is fed through `joystick1`, connector 1 (JOY1DAT) through `joystick0`
            h.ports()->joystick1 = (k < 9) ? bits : 0;
            h.ports()->joystick0 = (k >= 9) ? bits : 0;
            changes->push_back(i);
        };
        ja.cap = run_spec(s);
        ja.rd = joy_reads(ja.cap);
        ja.change = *changes;
        ja.t0 = *t0 >= 0 ? (uint64_t)*t0 : 0;
        // changes hold run-relative cycles; make them absolute
        for (auto& x : ja.change) x += ja.cap.base;
    }
    bool fixture = ja.cap.loaded && ja.cap.released && ja.cap.fixture_ok && ja.cap.done && ja.rd.t.size() == kJoyCount && ja.change.size() == 18;
    g.expect(fixture, "P11-FIXTURE: the joystick run loaded, booted, stored the $BEEF fixture, stored all 7200 reads of both registers and reached DONE");
    bool still = fixture;
    bool pop = fixture;
    std::vector<std::pair<size_t, size_t>> win(18);   // read index range of each graded window
    if (fixture) {
        for (int k = 0; k < 18; ++k) {
            uint64_t a = ja.change[k] + settle, b = (k + 1 < 18) ? ja.change[k + 1] : ja.change[k] + hold;
            size_t lo = 0, hi = 0; bool any = false;
            for (size_t i = 0; i < ja.rd.t.size(); ++i)
                if (ja.rd.t[i] >= a && ja.rd.t2[i] < b) { if (!any) lo = i; hi = i + 1; any = true; }
            win[k] = {lo, hi};
            if (!any || hi - lo < 10) pop = false;
            std::map<uint16_t, size_t> h0, h1;
            for (size_t i = lo; any && i < hi; ++i) { ++h0[ja.rd.v0[i]]; ++h1[ja.rd.v1[i]]; }
            for (size_t i = lo + 1; any && i < hi; ++i)
                if (ja.rd.v0[i] != ja.rd.v0[lo] || ja.rd.v1[i] != ja.rd.v1[lo]) still = false;
            std::string vals;
            for (auto& kv : h0) vals += " $" + hexw(kv.first) + "x" + dec((long long)kv.second);
            vals += " |";
            for (auto& kv : h1) vals += " $" + hexw(kv.first) + "x" + dec((long long)kv.second);
            checks.note("P11 hold " + dec(k) + ": " + dec((long long)(hi - lo)) + " reads; JOY0DAT values x count, then JOY1DAT:" + vals);
        }
    }
    dep_expect(checks, fixture, pop, "P11-POP: precondition population: each of the 18 holds has at least 10 reads of both registers between 0.2 ms after its change and the next change", "fixture");
    dep_expect(checks, fixture, still, "P11-STILL: precondition: while nothing changes, consecutive reads of both registers are equal within every hold", "fixture");
    if (!(fixture && pop && still)) { not_graded(checks, "P11 joystick decode", "fixture, population or stillness precondition"); }
    bool joy_ok = false;
    if (fixture && pop && still) {
        joy_ok = true;
        for (int k = 0; k < 18; ++k) {
            const JoyState& st = kJoyStates[k % 9];
            Decoded d0 = decode_joy(ja.rd.v0[win[k].first]), d1 = decode_joy(ja.rd.v1[win[k].first]);
            Decoded want{st.f, st.l, st.b, st.r}, zero{0, 0, 0, 0};
            Decoded w0 = k < 9 ? want : zero, w1 = k < 9 ? zero : want;
            bool a = d0.f == w0.f && d0.l == w0.l && d0.b == w0.b && d0.r == w0.r;
            bool b = d1.f == w1.f && d1.l == w1.l && d1.b == w1.b && d1.r == w1.r;
            joy_ok = joy_ok && a && b;
            checks.note(std::string("P11 connector ") + (k < 9 ? "0" : "1") + " " + st.name + ": JOY0DAT=$" + hexw(ja.rd.v0[win[k].first]) +
                        " JOY1DAT=$" + hexw(ja.rd.v1[win[k].first]) + (a && b ? "" : "  (differs from the expected F/L/B/R)"));
        }
        g.expect(joy_ok, "P11-JOY: for each of the nine switch states on each port, F = bit 9 xor bit 8, L = bit 9, B = bit 1 xor bit 0 and R = bit 1 of the port's register are 1 exactly for the closed switches, and all four are 0 in the other port's register (AA p.25-26 joystick decode)");
    }

    // ---- (b) mouse ----
    JoyRun jm;
    const int counts = mutating("P11-MOUSE-RET") ? 39 : 40;
    const uint64_t step = us_to_cycles(50.0), mgap = ms_to_cycles(4.0), mstart = ms_to_cycles(2.0);
    auto mt0 = std::make_shared<int64_t>(-1);
    {
        RunSpec s;
        s.bin = "prog_joy.bin";
        s.cycles = 3'900'000;
        s.drive = [=](nmh::NanomigHarness& h, uint64_t i) {
            if (*mt0 < 0) { auto fx = h.peek_logical(kResults); if (fx && *fx == 0xBEEF) { *mt0 = (int64_t)i; } else return; }
            uint64_t rel = i - (uint64_t)*mt0;
            if (rel < mstart) return;
            uint64_t q = rel - mstart;
            uint64_t m = q / mgap;
            uint64_t qm = q % mgap;
            if (m >= 4 || qm >= (uint64_t)40 * step) return;
            uint64_t sub = qm % step;
            const bool yaxis = m >= 2;
            // a Y count is a type-1 datum; the input takes a type-1 datum for Y after a type-0 datum (an X count of 0)
            if (sub != 0 && !(yaxis && sub == 16)) return;
            uint64_t j = qm / step;
            // movement 0 right (+1 on X), 1 left (-1 on X), 2 down (+1 on Y), 3 up (-1 on Y)
            int dir = (m == 0 || m == 2) ? 1 : -1;
            if (mutating("P11-MOUSE") && m == 0) dir = -1;
            if (mutating("P11-AGG") && m == 2) dir = -1;
            // a movement of `counts` counts: the last count of movement 1 is left out for P11-MOUSE-RET
            if (m == 1 && j >= (uint64_t)counts) return;
            if (yaxis && sub == 0) { h.ports()->kbd_mouse_type = 0; h.ports()->kbd_mouse_data = 0; }
            else { h.ports()->kbd_mouse_type = yaxis ? 1 : 0; h.ports()->kbd_mouse_data = (uint8_t)(int8_t)dir; }
            h.ports()->kbd_mouse_level = !h.ports()->kbd_mouse_level;
        };
        jm.cap = run_spec(s);
        jm.rd = joy_reads(jm.cap);
        jm.t0 = *mt0 >= 0 ? (uint64_t)*mt0 + jm.cap.base : 0;
    }
    bool mfix = jm.cap.loaded && jm.cap.released && jm.cap.fixture_ok && jm.cap.done && jm.rd.t.size() == kJoyCount;
    bool mouse_ok = false;
    if (!mfix) {
        not_graded(checks, "P11 mouse", "fixture");
    } else {
        // reads before the first movement, between and after
        auto last_before = [&](uint64_t t) { size_t k = 0; for (size_t i = 0; i < jm.rd.t.size(); ++i) if (jm.rd.t[i] < t) k = i; return k; };
        mouse_ok = true;
        bool moved[4] = {false, false, false, false};
        uint16_t x_before = 0, y_before = 0;
        for (int m = 0; m < 4; ++m) {
            uint64_t s0 = jm.t0 + mstart + m * mgap, s1 = s0 + 40 * step + settle;
            size_t k0 = last_before(s0), k1 = last_before(s1);
            const bool xaxis = m < 2;
            bool sign_ok = true;
            for (size_t i = k0 + 1; i <= k1; ++i) {
                int dx = (int8_t)((jm.rd.v0[i] & 0xFF) - (jm.rd.v0[i - 1] & 0xFF));
                int dy = (int8_t)(((jm.rd.v0[i] >> 8) & 0xFF) - ((jm.rd.v0[i - 1] >> 8) & 0xFF));
                int dm = xaxis ? dx : dy, doth = xaxis ? dy : dx;
                int want = (m == 0 || m == 2) ? 1 : -1;
                if (doth != 0) sign_ok = false;
                if (dm != 0) { moved[m] = true; if ((dm > 0) != (want > 0)) sign_ok = false; }
            }
            mouse_ok = mouse_ok && sign_ok && moved[m];
            checks.note("P11 mouse movement " + dec(m) + ": counter bits at the start $" + hexw(jm.rd.v0[k0]) + ", at the end $" + hexw(jm.rd.v0[k1]) +
                        (sign_ok && moved[m] ? "" : "  (direction, axis or movement differs from the expected)"));
            if (m == 0) { x_before = jm.rd.v0[k0]; }
            if (m == 2) { y_before = jm.rd.v0[k0]; }
            if (m == 1) mouse_ok = mouse_ok && (jm.rd.v0[k1] & 0xFF) == (x_before & 0xFF);
            if (m == 3) mouse_ok = mouse_ok && ((jm.rd.v0[k1] >> 8) & 0xFF) == ((y_before >> 8) & 0xFF);
        }
        g.expect(mouse_ok, "P11-MOUSE: through the mouse inputs, 40 counts right and then left change bits 7-0 of the mouse register upward and then downward, and 40 down and up change bits 15-8 upward and downward, the other axis unchanged, each pair returning the counter to its start (AA p.25 JOY0DAT, HRM p.229)");
    }
    if (fixture && pop && still && mfix)
        g.expect(joy_ok && mouse_ok, "P11-AGG sol:AA-mouse.c2: the joystick switches decode through bits 9, 8, 1 and 0 and the mouse counters count right, left, down and up through bits 7-0 and 15-8, as AA p.25-26 tabulates for the registers (AA p.5: the serial mouse interface is 'transparent to the programmer')");
    else not_graded(checks, "P11 aggregate", "fixture, population or stillness precondition");
}

// ---- P12: UARTBRK ------------------------------------------------------------

constexpr uint32_t kUartMarks = 0x0B9200;

struct UartRun { Cap cap; uint64_t t[4] = {0, 0, 0, 0}; bool have = false; };

UartRun run_uart(const std::string& bin) {
    UartRun u;
    RunSpec s;
    s.bin = bin; s.cycles = 2'900'000; s.sample_txd = true;
    u.cap = run_spec(s);
    u.have = true;
    for (int i = 0; i < 4; ++i) {
        auto w = writes_to(u.cap, kUartMarks + 2 * i);
        if (w.empty()) { u.have = false; continue; }
        u.t[i] = w.front().first;
    }
    return u;
}

void prop_p12(Checks& checks, Grader& g) {
    UartRun m = run_uart(mutating("P12-MAIN") ? "prog_uart_k.bin" : mutating("P12-PRE1") ? "prog_uart_early.bin" :
                         mutating("P12-FIXTURE") ? "prog_uart_nofix.bin" : "prog_uart.bin");
    UartRun k = run_uart(mutating("P12-KPRE") ? "prog_uart.bin" : "prog_uart_k.bin");
    bool fixture = m.cap.loaded && m.cap.released && m.cap.fixture_ok && m.cap.done && m.have &&
                   k.cap.loaded && k.cap.released && k.cap.fixture_ok && k.cap.done && k.have;
    g.expect(fixture, "P12-FIXTURE: both runs loaded, booted, stored the $BEEF fixture, stored the four markers and reached DONE");
    g.expect(g_K > 0, "P12-UNIT: precondition: the CCK unit K was measured by the unit run before the 30 CCK bit time and the 0.5 ms windows were derived");
    if (!fixture || g_K == 0) { not_graded(checks, "P12 checks", "fixture or unit"); return; }
    auto txd_at = [&](const UartRun& u, uint64_t cyc) { return u.cap.txd.at(cyc - u.cap.base) != 0; };
    const uint64_t bit = 30 * g_K;                    // SERPER = $001D: one bit every 30 CCK (AA p.27)
    // precondition 1: TxD = 1 before t(M1) (the 0.5 ms before it) and from one bit time after t(M4) to the end
    auto high_over = [&](const UartRun& u, uint64_t a, uint64_t b) { for (uint64_t c = a; c < b; ++c) if (!txd_at(u, c)) return false; return true; };
    uint64_t pre_len = ms_to_cycles(0.5);
    bool pre1 = high_over(m, m.t[0] - pre_len, m.t[0]) &&
                high_over(m, m.t[3] + bit, std::min<uint64_t>(m.cap.base + m.cap.txd.size(), m.t[3] + bit + ms_to_cycles(0.5)));
    g.expect(pre1, "P12-PRE1: precondition: TxD is 1 during the 0.5 ms before marker M1 and from one bit time after marker M4 for 0.5 ms, so the 0 in the interval is the bit's doing");
    // precondition 2: in contrast run K a word is transmitted inside the interval: start 0, data 1 0 1 0 1 0 1 0 (LSB first), stop 1
    bool kpre = true; std::string pat;
    {
        uint64_t a = k.t[1], b = k.t[2];
        uint64_t start = 0; bool found = false;
        for (uint64_t c = a + 1; c < b; ++c) if (!txd_at(k, c) && txd_at(k, c - 1)) { start = c; found = true; break; }
        if (!found) kpre = false;
        else {
            const int expect_bits[10] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
            for (int i = 0; i < 10; ++i) {
                uint64_t c = start + (uint64_t)i * bit + bit / 2;
                bool v = c < b && txd_at(k, c);
                pat += v ? '1' : '0';
                if (v != (expect_bits[i] != 0)) kpre = false;
            }
        }
    }
    g.expect(kpre, "P12-KPRE: precondition: in contrast run K (ADKCON = $0800 at step 5) the interval carries the word $0155 as HRM p.258 draws it: a start bit 0, the data bits 1 0 1 0 1 0 1 0 LSB first and a stop bit 1, each 30 CCK long");
    checks.note("P12 run K bits at bit centres: " + pat);
    if (!(pre1 && kpre)) { not_graded(checks, "P12-MAIN", "TxD idle level or contrast word precondition"); return; }
    bool all0 = true; size_t n = 0, ones = 0;
    for (uint64_t c = m.t[1]; c <= m.t[2]; ++c) { ++n; if (txd_at(m, c)) { all0 = false; ++ones; } }
    checks.note("P12 main run: " + dec((long long)n) + " samples from M2 to M3, " + dec((long long)ones) + " of them 1");
    g.expect(all0, "P12-MAIN opus:AA-adkcon.c11: while ADKCON bit 11 (UARTBRK) is set TxD is 0 at every sample from marker M2 to marker M3, including while the word $0155 is shifted out (AA p.10 'UARTBRK Forces a UART break (clears TXD) if true')");
}

// ---- P13: disk DMA pointer ---------------------------------------------------

bool is_refresh_access(const Cap& c, const nmh::RamAccess& a) {
    if (a.cycle < c.base + 3) return false;
    uint64_t i = a.cycle - c.base;
    if (i >= c.refresh.size()) return false;
    for (int d = 0; d <= 3; ++d) if (c.refresh[i - d]) return true;
    return false;
}

struct DiskRun {
    Cap cap;
    std::vector<nmh::RamAccess> dma;   // reads that are neither CPU, register-list nor refresh accesses
    bool dskblk = false;
};

DiskRun run_disk(uint32_t a, uint16_t dmacon, uint16_t dskpth_override_hi = 0xFFFF, const char* bin = "prog_disk.bin") {
    DiskRun d;
    RunSpec s;
    s.bin = bin;
    s.cycles = 3'900'000;
    for (int i = 0; i < 8; ++i) s.pokes.push_back({a + 2 * i, uint16_t(0xD0D0 + i)});
    s1_step1(s.rl);
    s.rl.add(0x020, dskpth_override_hi != 0xFFFF ? dskpth_override_hi : uint16_t(a >> 16));
    s.rl.add(0x022, uint16_t(a & 0xFFFF));
    s.rl.add(0x09C, 0x0002);
    s.rl.add(0x096, dmacon);
    s.rl.add(0x024, 0x4000);
    s.rl.add(0x024, 0xC008);
    s.rl.add(0x024, 0xC008);
    s.rl.finish();
    d.cap = run_spec(s);
    for (auto& x : d.cap.ram) {
        if (x.is_write) continue;
        uint32_t byte = x.word_addr * 2;
        if (byte >= nmh::kProgramOrg && byte < nmh::kProgramOrg + 0x100) continue;    // the program's own fetches
        if (byte >= kRList && byte < kRList + 0x100) continue;                      // the register list
        if (x.word_addr >= nmh::kKickShadowWordBase) continue;                      // reset vectors
        if (is_refresh_access(d.cap, x)) continue;
        d.dma.push_back(x);
    }
    d.dskblk = (d.cap.results2 & 0x0002) != 0;
    return d;
}

void prop_p13(Checks& checks, Grader& g) {
    const uint32_t ax = 0x0A5432, ay = 0x154320;
    uint16_t dmacon_x = mutating("P13-RUN") ? 0x8200 : 0x8210;
    DiskRun x = run_disk(ax, dmacon_x, 0xFFFF, mutating("P13-FIXTURE") ? "prog_disk_nofix.bin" : "prog_disk.bin"), y = run_disk(ay, 0x8210);
    if (mutating("P13-MAIN")) y = run_disk(ay, 0x8210, uint16_t(((ay >> 16) & 7) ));   // DSKPTH held as only 3 bits
    bool fixture = x.cap.loaded && x.cap.released && x.cap.fixture_ok && y.cap.loaded && y.cap.released && y.cap.fixture_ok;
    g.expect(fixture, "P13-FIXTURE: both runs loaded, booted and stored the $BEEF fixture before any observation");
    bool occurs = fixture && !x.dma.empty() && !y.dma.empty() && x.dskblk && y.dskblk;
    checks.note("P13 run X (A=$" + hexw(ax, 6) + "): " + dec((long long)x.dma.size()) + " disk DMA reads, DSKBLK " + (x.dskblk ? "set" : "not set") +
                "; run Y (A=$" + hexw(ay, 6) + "): " + dec((long long)y.dma.size()) + " disk DMA reads, DSKBLK " + (y.dskblk ? "set" : "not set"));
    dep_expect(checks, fixture, occurs, "P13-RUN: precondition: in both runs at least one disk DMA access occurs after the second $C008 write to DSKLEN and DSKBLK (INTREQR bit 1) is set", "fixture");
    if (!occurs) { not_graded(checks, "P13-MAIN", "disk DMA does not run in this harness or the fixture failed"); return; }
    std::string seq;
    bool ok = true;
    const DiskRun* rs[2] = {&x, &y};
    const uint32_t as[2] = {ax, ay};
    for (int r = 0; r < 2; ++r) {
        const DiskRun& d = *rs[r];
        ok = ok && d.dma.size() <= 8 && d.dma[0].word_addr * 2 == as[r];
        for (size_t i = 0; i < d.dma.size(); ++i) ok = ok && d.dma[i].word_addr * 2 == as[r] + 2 * i;
        seq += std::string(r ? "; Y:" : "X:");
        for (size_t i = 0; i < d.dma.size() && i < 3; ++i) seq += " $" + hexw(d.dma[i].word_addr * 2, 6);
    }
    checks.note("P13 first disk DMA addresses " + seq);
    g.expect(ok, "P13-MAIN opus:AA-dskpth.c9 sol:AA-disk-pointer.c1: with DSKPTH/DSKPTL written $0A5432 (run X) and $154320 (run Y), the first disk DMA access of each run is a read at exactly that address and all its disk DMA accesses are reads at A, A+2, ... in increasing order, so the pair holds a 20-bit word address covering A20-A16 both ways (AA p.23 'This pair of registers contains the 20 bit address of Disk DMA data')");
}

// ---- P14: DUAL, the secondary beam counter -----------------------------------

constexpr uint32_t kDualBuf = 0x0C8000;

struct DualRun {
    Cap cap;
    R2 r2;
    std::vector<uint64_t> t;           // port time of the store of h_i
    std::vector<uint8_t> h, m;         // HHPOSR bits 7-0, VHPOSR bits 7-0
    std::vector<uint16_t> hw;          // the whole HHPOSR word
    std::vector<uint64_t> tm;          // port time of the store of m_i
    bool have = false;
};

DualRun run_dual(const std::string& bin, uint16_t beamcon0, uint16_t htotal, uint64_t cycles, bool varbeam_vtotal = false) {
    DualRun d;
    RunSpec s;
    s.bin = bin; s.cycles = cycles;
    s1_step1(s.rl, beamcon0);
    s.rl.add(0x1C0, htotal);
    if (varbeam_vtotal) s.rl.add(0x1C8, 0x0138);
    s.rl.finish();
    d.cap = run_spec(s);
    d.r2 = analyse_r2(d.cap);
    std::vector<uint8_t> have(1024, 0);
    d.t.assign(1024, 0); d.tm.assign(1024, 0); d.h.assign(1024, 0); d.m.assign(1024, 0); d.hw.assign(1024, 0);
    for (auto& a : d.cap.ram) {
        if (!a.is_write) continue;
        uint32_t byte = a.word_addr * 2;
        if (byte < kDualBuf || byte >= kDualBuf + 4096) continue;
        size_t i = (byte - kDualBuf) / 4;
        if ((byte - kDualBuf) % 4 == 0) { d.t[i] = a.cycle; d.h[i] = uint8_t(a.data & 0xFF); d.hw[i] = a.data; have[i] |= 1; }
        else { d.tm[i] = a.cycle; d.m[i] = uint8_t(a.data & 0xFF); have[i] |= 2; }
    }
    d.have = true;
    for (int i = 0; i < 1024; ++i) if (have[i] != 3) d.have = false;
    return d;
}

void prop_p14(Checks& checks, Grader& g) {
    const uint16_t u1_htotal = mutating("P14-U1") ? 0x0070 : mutating("P14-REL") ? 0x0050 : 0x0040;
    const uint16_t u2_htotal = mutating("P14-U2") ? 0x0040 : 0x0070;
    // the contrasts: a loop period of two lines (PAIRS); VARBEAMEN
    // lines in U1 (R2) or in U2 (MAIN); U3 without VARBEAMEN (U3)
    const char* u1bin = mutating("P14-PAIRS") ? "prog_dual_pad121.bin" : mutating("P14-FIXTURE") ? "prog_dual_nofix.bin" : "prog_dual.bin";
    const uint64_t u1cyc = mutating("P14-PAIRS") ? 3'700'000 : 3'100'000;
    DualRun u1 = run_dual(u1bin, mutating("P14-R2") ? 0x08E0 : 0x0860, u1_htotal, u1cyc);
    DualRun u2 = run_dual("prog_dual.bin", mutating("P14-MAIN") ? 0x08E0 : 0x0860, u2_htotal, 3'100'000);
    DualRun u0 = run_dual("prog_dual.bin", 0x0820, 0x0040, 3'100'000);
    DualRun u3 = run_dual("prog_dual.bin", mutating("P14-U3") ? 0x0820 : 0x08A0, 0x0040, 3'100'000, !mutating("P14-U3"));
    DualRun* all[4] = {&u1, &u2, &u0, &u3};
    bool fixture = true;
    for (DualRun* d : all) fixture = fixture && d->cap.loaded && d->cap.released && d->cap.fixture_ok && d->cap.done && d->have;
    g.expect(fixture, "P14-FIXTURE: the four runs loaded, booted, stored the $BEEF fixture, stored all 1024 samples and reached DONE");
    g.expect(g_K > 0, "P14-UNIT: precondition: the CCK unit K was measured by the unit run before the sample times were converted to CCK");
    if (!fixture || g_K == 0) { not_graded(checks, "P14 checks", "fixture or unit"); return; }
    bool r2ok = true;
    for (DualRun* d : {&u1, &u2, &u0}) r2ok = r2ok && d->r2.ok;
    g.expect(r2ok, "P14-R2: precondition R2 anchor: in runs U1, U2 and U0 the refresh output comes in groups of exactly four equally spaced accesses, consecutive groups 226 to 229 CCK apart");
    checks.note("P14 U3 (VARBEAMEN, HTOTAL=$0040): " + std::string(u3.r2.ok ? "line spacing still 226 to 229 CCK" : "line spacing differs from 226 to 229 CCK") + " (" + num(u3.r2.gap_min) + " to " + num(u3.r2.gap_max) + ")");

    // preconditions from times alone
    auto phase_ok = [&](const DualRun& d, unsigned htotal) {
        std::set<unsigned> seen;
        for (int i = 0; i < 1024; ++i) seen.insert(unsigned((d.t[i] / g_K) % (htotal + 1)));
        return seen.size() == htotal + 1;
    };
    auto period = [&](const DualRun& d) { std::vector<uint64_t> dt; for (int i = 1; i < 1024; ++i) dt.push_back(d.t[i] - d.t[i - 1]); std::sort(dt.begin(), dt.end()); return double(dt[dt.size() / 2]) / double(g_K); };
    checks.note("P14 loop period (median distance of consecutive HHPOSR stores, CCK): U1 " + num(period(u1)) + ", U2 " + num(period(u2)) + ", U0 " + num(period(u0)));
    auto period_hist = [&](const DualRun& d) { std::map<long long, int> h; for (int i = 1; i < 1024; ++i) ++h[(long long)((d.t[i] - d.t[i - 1]) / g_K)]; std::string r; for (auto& kv : h) r += " " + dec(kv.first) + "x" + dec(kv.second); return r; };
    checks.note("P14 distances between consecutive samples (CCK x count): U1" + period_hist(u1) + "; U2" + period_hist(u2));
    bool ph = phase_ok(u1, 0x40) && phase_ok(u2, 0x70);
    g.expect(ph, "P14-PHASE: precondition: in U1 and U2 the sample times in CCK taken modulo HTOTAL+1 take every value from 0 to HTOTAL");
    std::string pos_note;
    auto pos_ok = [&](const DualRun& d, const char* name) {
        std::set<unsigned> seen;
        for (int i = 0; i < 1024; ++i) {
            int64_t ln = line_of(d.r2, d.t[i]);
            if (ln < 0) continue;
            seen.insert(unsigned((d.t[i] - d.r2.start[ln]) / g_K));
        }
        std::string missing;
        for (unsigned p = 0; p < 227; ++p) if (!seen.count(p)) missing += " " + dec(p);
        pos_note += std::string(" ") + name + ": " + dec((long long)seen.size()) + " of 227 positions, missing:" + (missing.empty() ? " none" : missing) + ";";
        return seen.size() >= 227;
    };
    bool pos = pos_ok(u1, "U1") & pos_ok(u2, "U2") & pos_ok(u0, "U0");
    checks.note("P14 sample positions within the R2 line:" + pos_note);
    g.expect(pos, "P14-POS: precondition: in U1, U2 and U0 the sample times take every CCK position of the 227 within their R2 line");
    // qualifying pairs
    std::vector<uint64_t> rt[3];
    const DualRun* ds[3] = {&u1, &u2, &u0};
    for (int di = 0; di < 3; ++di)
        for (uint64_t st : ds[di]->r2.start) for (int k = 0; k < 4; ++k) rt[di].push_back(st + 2 * k * g_K);
    auto refresh_between = [&](const DualRun& d, uint64_t a, uint64_t b) {
        const std::vector<uint64_t>& v = (&d == &u1) ? rt[0] : (&d == &u2) ? rt[1] : rt[2];
        auto it = std::upper_bound(v.begin(), v.end(), a);
        return it != v.end() && *it < b;
    };
    struct Rel { size_t pairs = 0, bad = 0, notwhole = 0; };
    auto rel = [&](const DualRun& d, unsigned htotal) {
        Rel r;
        for (int i = 1; i + 1 < 1024; ++i) {
            if (refresh_between(d, d.tm[i - 1], d.t[i + 1])) continue;
            ++r.pairs;
            uint64_t dt = d.t[i + 1] - d.t[i];
            if (dt % g_K != 0) { ++r.notwhole; continue; }
            int lhs = ((int)d.h[i + 1] - (int)d.h[i]) % (int)(htotal + 1); if (lhs < 0) lhs += htotal + 1;
            int rhs = (int)((dt / g_K) % (htotal + 1));
            if (lhs != rhs) ++r.bad;
        }
        return r;
    };
    Rel r1 = rel(u1, 0x40), r2 = rel(u2, 0x70);
    checks.note("P14 qualifying pairs: U1 " + dec((long long)r1.pairs) + " (" + dec((long long)r1.notwhole) + " not a whole number of CCK, " + dec((long long)r1.bad) + " differ), U2 " +
                dec((long long)r2.pairs) + " (" + dec((long long)r2.notwhole) + ", " + dec((long long)r2.bad) + ")");
    bool pairs = r1.pairs > 0 && r2.pairs > 0 && r1.notwhole == 0 && r2.notwhole == 0;
    g.expect(pairs, "P14-PAIRS: precondition: in U1 and U2 qualifying pairs exist (no refresh access between the last store of the previous sample and the store of h of the second sample) and for each of them the difference of the two store times is a whole number of CCK");
    unsigned mx1 = 0, mx2 = 0, mxm0 = 0, mxm1 = 0, mxm2 = 0, mxm3 = 0, mw1 = 0, mw2 = 0; bool mid = false;
    for (int i = 0; i < 1024; ++i) {
        mx1 = std::max<unsigned>(mx1, u1.h[i]); mx2 = std::max<unsigned>(mx2, u2.h[i]);
        mw1 = std::max<unsigned>(mw1, u1.hw[i]); mw2 = std::max<unsigned>(mw2, u2.hw[i]);
        mxm0 = std::max<unsigned>(mxm0, u0.m[i]); mxm1 = std::max<unsigned>(mxm1, u1.m[i]); mxm2 = std::max<unsigned>(mxm2, u2.m[i]); mxm3 = std::max<unsigned>(mxm3, u3.m[i]);
        if (u2.h[i] > 0x40 && u2.h[i] <= 0x70) mid = true;
    }
    checks.note("P14 measured: largest whole HHPOSR word U1=$" + hexw(mw1) + " U2=$" + hexw(mw2));
    checks.note("P14 measured: max h U1=$" + hexw(mx1, 2) + " U2=$" + hexw(mx2, 2) + "; max m U0=$" + hexw(mxm0, 2) + " U1=$" + hexw(mxm1, 2) + " U2=$" + hexw(mxm2, 2) + " U3=$" + hexw(mxm3, 2));
    // the contrast of the main-counter comparison: U3's max m differs from U0's
    g.expect(mxm3 != mxm0, "P14-U3: precondition: with VARBEAMEN and HTOTAL = $0040 (run U3) the main counter's maximum m differs from run U0's, so the main-counter comparison can go red");
    if (!(r2ok && ph && pos && pairs)) { not_graded(checks, "P14 assertions", "R2 anchor, phase, position or qualifying-pair precondition"); return; }
    bool u1ok = true, u2ok = true;
    for (int i = 0; i < 1024; ++i) { u1ok = u1ok && u1.h[i] <= 0x40; u2ok = u2ok && u2.h[i] <= 0x70; }
    u1ok = u1ok && mx1 == 0x40;
    u2ok = u2ok && mx2 == 0x70 && mid;
    g.expect(u1ok, "P14-U1: with DUAL set and HTOTAL = $0040 every h read from HHPOSR bits 7-0 is at most $40 and the maximum is $40");
    g.expect(u2ok, "P14-U2: with DUAL set and HTOTAL = $0070 every h is at most $70, the maximum is $70 and values between $40 and $70 occur");
    bool relok = r1.bad == 0 && r2.bad == 0;
    g.expect(relok, "P14-REL: in U1 and U2, for every qualifying pair of consecutive samples, the difference of the h values modulo HTOTAL+1 equals the elapsed whole CCK modulo HTOTAL+1, exactly");
    bool mainok = mxm0 == mxm1 && mxm1 == mxm2;
    g.expect(mainok, "P14-MAIN: the maximum of the main counter's m (VHPOSR bits 7-0) is the same in U0, U1 and U2: DUAL leaves the line length of the main counter unchanged");
    g.expect(u1ok && u2ok && relok && mainok, "P14-AGG sol:AA-hhpos.c1 sol:AA-hhpos.c5: with DUAL set HHPOSR reads a second horizontal counter that counts colour clocks from 0 to HTOTAL and restarts, while the main counter keeps its line (AA p.24 'secondary beam counter', 'Highest color clock count in horiz line')");
}

// ---- P15: DUAL and the UHRES pointer -----------------------------------------

constexpr uint32_t kUhres = 0x160000;

struct UhRun {
    FetchRun fr;
    CopList cl{kCopList};
    std::optional<int64_t> idx_a, idx_b;
    std::vector<Fetch> acc;
};

UhRun run_uhres(uint16_t beamcon0, uint16_t htotal, uint16_t bplcon0, const char* bin = "prog_uhres.bin",
                int marker_b = 0x98, uint64_t cycles = 4'700'000) {
    UhRun u;
    RunSpec s;
    CopList& cl = u.cl;
    cl.marker(0x68);
    cl.marker(marker_b);
    cl.end();
    cl.poke_into(s.pokes);
    s.bin = bin;
    s1_step1(s.rl, beamcon0);
    if (htotal) s.rl.add(0x1C0, htotal);
    if (beamcon0 & 0x0080) s.rl.add(0x1C8, 0x0138);
    s.rl.add(0x100, bplcon0);
    s.rl.add(0x1E6, 0x0006);
    s.rl.add(0x1D4, 0x0060); s.rl.add(0x1D6, 0x00A0);
    s.rl.add(0x1E8, 0x0016); s.rl.add(0x1EA, 0x8000);
    s.rl.add(0x1D0, 0x0000); s.rl.add(0x1D2, 0x0000);
    s.rl.add(0x104, 0x0024); s.rl.add(0x106, 0x0C00); s.rl.add(0x10C, 0x0011);
    s.rl.add(0x08E, 0x2C81); s.rl.add(0x090, 0xF0C1);
    s.rl.add(0x080, uint16_t(kCopList >> 16)); s.rl.add(0x082, uint16_t(kCopList & 0xFFFF)); s.rl.add(0x088, 0);
    s.rl.add(0x096, 0x8380);
    s.rl.finish();
    s.cycles = cycles;
    u.fr = run_fetch(s, kCopList, kCopList + 0x100);   // the frame holding the first Copper fetch is the one the DMA enable falls in
    if (u.fr.f3_ok && u.fr.r2.ok) {
        u.idx_a = marker_line_idx(u.fr, u.cl.marker_move_word.at(0x68));
        u.idx_b = marker_line_idx(u.fr, u.cl.marker_move_word.at(marker_b));
        if (u.idx_a && u.idx_b) {
            u.acc = fetches_in(u.fr.cap, u.fr.r2, kUhres, kUhres + 0x8000, u.fr.f3);
            std::vector<Fetch> kept;
            for (auto& f : u.acc) if (f.line >= *u.idx_a && f.line <= *u.idx_b) kept.push_back(f);
            u.acc = kept;
        }
    }
    return u;
}

void prop_p15(Checks& checks, Grader& g) {
    const uint16_t v1_beam = 0x0860;
    // the contrasts: VARBEAMEN lines in V0 (R2); live inputs through the same comparators, a CPU loop reading the region in place
    // of the UHRES pointer, one read per line in V0 (V0) and one per 57 CCK in V0 and V1 (V1)
    UhRun v0 = run_uhres(mutating("P15-R2") ? 0x00A0 : 0x0820, mutating("P15-R2") ? 0x00E8 : 0, 0x0080,
                         mutating("P15-FIXTURE") ? "prog_uhres_nofix.bin" : (mutating("P15-V0") || mutating("P15-V1")) ? "prog_cpuuh_line.bin" : "prog_uhres.bin", mutating("P15-R3") ? 0x99 : 0x98,
                         mutating("P15-F3") ? 3'300'000 : 4'700'000);
    UhRun v1 = run_uhres(v1_beam, 0x0038, 0x0080, mutating("P15-V1") ? "prog_cpuuh_57.bin" : "prog_uhres.bin");
    UhRun* all[2] = {&v0, &v1};
    bool fixture = true, r2ok = true, f3ok = true, r3ok = true;
    for (UhRun* u : all) {
        fixture = fixture && u->fr.cap.loaded && u->fr.cap.released && u->fr.cap.fixture_ok;
        r2ok = r2ok && u->fr.r2.ok; f3ok = f3ok && u->fr.f3_ok;
        r3ok = r3ok && u->idx_a && u->idx_b && (*u->idx_b - *u->idx_a) == (0x98 - 0x68);
    }
    g.expect(fixture, "P15-FIXTURE: both runs loaded, booted and stored the $BEEF fixture before any observation");
    g.expect(r2ok, "P15-R2: precondition R2 anchor: in V0 the refresh output comes in groups of exactly four equally spaced accesses, consecutive groups 226 to 229 CCK apart; V1 runs with a 57 CCK secondary period and keeps the main line");
    g.expect(f3ok, "P15-F3: precondition frame F3: the frame holding the first Copper-list fetch is found and its third successor F3 is complete in both runs");
    dep_expect(checks, r2ok && f3ok, r3ok, "P15-R3: precondition R3 anchor: the markers at lines $68 and $98 fall 48 line boundaries apart in both runs", "R2 anchor or F3");
    if (!(fixture && r2ok && f3ok && r3ok)) { not_graded(checks, "P15 checks", "fixture, R2, F3 or R3 anchor"); return; }
    auto per_line = [](const UhRun& u) { std::map<int64_t, std::vector<Fetch>> m; for (auto& f : u.acc) m[f.line].push_back(f); return m; };
    auto m0 = per_line(v0), m1 = per_line(v1);
    bool v0pat = v0.idx_a && v0.idx_b && m0.size() == size_t(*v0.idx_b - *v0.idx_a + 1) && m0.size() >= 40;
    for (auto& kv : m0) v0pat = v0pat && kv.second.size() == 1;
    for (size_t i = 1; i < v0.acc.size(); ++i) v0pat = v0pat && v0.acc[i].addr - v0.acc[i - 1].addr == 8;
    g.expect(v0pat, "P15-V0: precondition: with DUAL clear (V0) exactly one UHRES access occurs on each of at least 40 lines between markers $68 and $98, consecutive accesses 8 bytes apart (AA p.15: BPLHMOD plus 2)");
    checks.note("P15 V0: " + dec((long long)v0.acc.size()) + " accesses on " + dec((long long)m0.size()) + " lines; V1: " + dec((long long)v1.acc.size()) + " accesses on " + dec((long long)m1.size()) + " lines");
    if (!v0pat) { not_graded(checks, "P15-V1", "V0 does not show the one-per-line UHRES pattern"); return; }
    bool v1ok = v1.idx_a && v1.idx_b && m1.size() == size_t(*v1.idx_b - *v1.idx_a + 1) && m1.size() >= 40;
    for (auto& kv : m1) v1ok = v1ok && kv.second.size() >= 3;
    for (size_t i = 1; i < v1.acc.size(); ++i) {
        v1ok = v1ok && v1.acc[i].addr - v1.acc[i - 1].addr == 8;
        double gap = double(v1.acc[i].cycle - v1.acc[i - 1].cycle) / double(v1.fr.r2.K);
        v1ok = v1ok && gap >= 55.0 && gap <= 59.0;
    }
    g.expect(v1ok, "P15-V1 sol:AA-hhpos.c2: with DUAL set and HTOTAL = $0038 the UHRES pointer comes out at least 3 times on every line, every pair of consecutive accesses 8 bytes apart and 57 CCK, give or take 2 CCK, apart (AA p.12 'starts the UHRES pointer chain with the reset of this counter rather than the normal one')");
}
} // namespace

// ===========================================================================

int main(int argc, char** argv) {
    g_dir = argc > 1 ? argv[1] : ".";
    g_mutate = std::getenv("NMH_MUTATE");
    std::string core = NMH_XSTR(NANOMIG_CORE_NAME);
    std::printf("CORE: %s\n", core.c_str());
    if (g_mutate) std::printf("NMH_MUTATE=%s\n", g_mutate);

    Checks checks;
    Grader& g = checks;
    const std::string selp = sel_prop();

    const bool all_runs = !g_mutate || std::string(g_mutate) == "ALL";
    auto want = [&](const char* p) { return all_runs || sel_prop() == p; };
    if (want("R1") || want("P1") || want("P2") || want("P3") || want("P4") || want("P5")) {
    // ---- R1 Scale: measure r_L (LORES) from a dedicated calibration run ----
    BarsConfig scale_cfg;
    scale_cfg.bplcon0 = 0x1000; // LORES, BPU=1
    scale_cfg.write_bplcon1 = false;
    scale_cfg.buf1 = scale_buf();
    if (run_mutation("R1")) scale_cfg.buf1 = zero_buf();   // no plane data: no COLOR01 pixel to measure
    RunOut scale_run = run_bars(scale_cfg);
    g.expect(scale_run.loaded, "R1 Scale fixture: prog_bars.bin loaded for the LORES all-$AAAA calibration run");
    g.expect(scale_run.released, "R1 Scale fixture: cpu_nrst_out released within the boot budget");
    g.expect(scale_run.fixture_ok, "R1 Scale fixture: $BEEF constant read back before any measurement is trusted");
    auto scale_f1 = find_f1(scale_run.video);
    g.expect(scale_f1.has_value(), "R1 Scale: F1 (first steady frame after DMA enable) was located from the video trace");
    checks.note("R1 Scale: F1 index = " + (scale_f1 ? std::to_string(*scale_f1) : std::string("n/a")) +
                 " (of " + std::to_string(nmh::find_frames(scale_run.video).size()) + " frames captured)");
    auto r_l_opt = scale_f1 ? measure_r_l(scale_run.video, *scale_f1 + 2 /* F3 */) : std::nullopt;
    g.expect(r_l_opt.has_value(), "R1 Scale: a stable LORES-pixel run length was found on F3's window lines (AA p.17, one LORES pixel = 140 ns)");
    int r_l = r_l_opt.value_or(0);
    checks.note("R1 Scale: r_L = " + std::to_string(r_l) + " video samples per LORES pixel, measured on F3");
    g.expect(r_l > 0 && r_l % 2 == 0, "R1 Scale anchor: r_L is even");
    double op = (r_l > 0) ? r_l / 2.0 : 1.0; // one op = 70 ns = half a LORES pixel (R1 "Scale")

    auto delta_ops = [&](std::optional<size_t> e_cfg, std::optional<size_t> e_ref) -> std::optional<double> {
        if (!e_cfg || !e_ref) return std::nullopt;
        return (double(*e_cfg) - double(*e_ref)) / op;
    };

    // =======================================================================
    // P1 -- BPLCON1 is the register at 102, written by a write (AA p.16)
    // =======================================================================
    if (!g_mutate || selp == "P1") {
        BarsConfig ref, test_run;
        ref.bplcon0 = 0x2000; ref.bplcon1 = 0x0000;
        test_run.bplcon0 = 0x2000; test_run.bplcon1 = 0x0033;
        // the contrasting case: $0000 to $102 and $0033 to $10C (BPLCON4) instead
        if (run_mutation("P1")) { test_run.bplcon1 = 0x0000; test_run.bplcon4 = 0x0033; }
        for (auto* c : {&ref, &test_run}) { c->buf1 = bars_buf(8); c->buf2 = bars_buf(24); }

        RunOut r_ref = run_bars(ref);
        RunOut r_tst = run_bars(test_run);

        bool loaded_ok = r_ref.loaded && r_tst.loaded && r_ref.released && r_tst.released
                          && r_ref.fixture_ok && r_tst.fixture_ok;
        g.expect(loaded_ok, "P1 fixture: both runs loaded, booted and stored the $BEEF fixture before any observation");

        auto f1_ref = find_f1(r_ref.video), f1_tst = find_f1(r_tst.video);
        g.expect(f1_ref.has_value() && f1_tst.has_value(), "P1: F1 (first steady frame after DMA enable) located in both runs");
        size_t f3_ref = f1_ref.value_or(0) + 2, f3_tst = f1_tst.value_or(0) + 2;

        HsyncShape sr = hsync_shape(r_ref.video, f3_ref), st = hsync_shape(r_tst.video, f3_tst);
        bool anchor = sr.line_count > 0 && sr.line_count == st.line_count
                      && near_int(sr.avg_period, st.avg_period, 1.0) && near_int(sr.avg_width, st.avg_width, 1.0);
        g.expect(anchor, "P1 precondition R1 anchor: hsync period, hsync width and output-line count per frame equal between the reference and test runs");
        checks.note("P1 R1 anchor: reference lines=" + std::to_string(sr.line_count) + " period=" + num(sr.avg_period) +
                     " width=" + num(sr.avg_width) + "; test lines=" + std::to_string(st.line_count) +
                     " period=" + num(st.avg_period) + " width=" + num(st.avg_width));

        EdgeMeasure e1_ref = measure_edge_all_lines(r_ref.video, f3_ref, kC01r,kC01g,kC01b);
        EdgeMeasure e2_ref = measure_edge_all_lines(r_ref.video, f3_ref, kC02r,kC02g,kC02b);
        g.expect(e1_ref.ok && e2_ref.ok, "P1 precondition S3b (reference run): E1 and E2 exist on every window line of F3 and are equal on all of them");
        checks.note("P1 S3b reference: window lines checked E1=" + std::to_string(e1_ref.lines_checked) +
                     " E2=" + std::to_string(e2_ref.lines_checked));

        EdgeMeasure e1_tst = measure_edge_all_lines(r_tst.video, f3_tst, kC01r,kC01g,kC01b);
        EdgeMeasure e2_tst = measure_edge_all_lines(r_tst.video, f3_tst, kC02r,kC02g,kC02b);
        g.expect(e1_tst.ok, "P1 precondition S3a/S3b (test run): E1 exists on every window line of F3 and is equal on all of them");
        g.expect(e2_tst.ok, "P1 precondition S3a/S3b (test run): E2 exists on every window line of F3 and is equal on all of them");

        auto d1 = delta_ops(e1_tst.offset, e1_ref.offset);
        auto d2 = delta_ops(e2_tst.offset, e2_ref.offset);
        g.expect(d1.has_value() && near_int(*d1, 6.0),
            "P1 opus:AA-bplcon1.h1 sol:AA-bplcon1.c6 sol:AA-bplcon1.c7: CPU write of $0033 to $DFF102 (BPLCON1, AA p.16 'BPLCON1 p 102 W D') "
            "delays playfield 1 (PF1H2/PF1H3 set) by Delta1=+6 op on every window line of F3");
        g.expect(d2.has_value() && near_int(*d2, 6.0),
            "P1 opus:AA-bplcon1.h1 sol:AA-bplcon1.c6 sol:AA-bplcon1.c7: CPU write of $0033 to $DFF102 (BPLCON1, AA p.16) "
            "delays playfield 2 (PF2H2/PF2H3 set) by Delta2=+6 op on every window line of F3");
        checks.note("P1 measured: Delta1=" + opt_num(d1) + " op, Delta2=" + opt_num(d2) + " op (r_L=" + std::to_string(r_l) + ")");

    }

    // =======================================================================
    // P2 -- weight and playfield of every BPLCON1 bit (AA p.16, p.17, p.5, p.3)
    // =======================================================================
    std::array<bool,16> p2_row_ok{}; // shared with P5 (rows 8-15)
    if (!g_mutate || selp == "P2" || selp == "P5") {
        struct Row { int bit; uint16_t value; const char* name; bool pf1; int op; bool a1, a2, b; std::string claims; };
        std::vector<Row> rows = {
            {0,0x0001,"PF1H2",true, 2, true, true, false, "opus:AA-bplcon1.b00 sol:AA-bplcon1.c30 opus:AA-pf1h.c1 sol:AA-bplcon1.c10 sol:AA-bplcon1.c12 opus:AA-bplcon1.c6 sol:AA-bplcon1.c5"},
            {1,0x0002,"PF1H3",true, 4, true, true, false, "opus:AA-bplcon1.b01 sol:AA-bplcon1.c29 opus:AA-pf1h.c1 sol:AA-bplcon1.c10"},
            {2,0x0004,"PF1H4",true, 8, true, true, false, "opus:AA-bplcon1.b02 sol:AA-bplcon1.c28 opus:AA-pf1h.c1 sol:AA-bplcon1.c10"},
            {3,0x0008,"PF1H5",true,16, true, true, false, "opus:AA-bplcon1.b03 sol:AA-bplcon1.c27 opus:AA-pf1h.c1 sol:AA-bplcon1.c10"},
            {4,0x0010,"PF2H2",false,2, true, true, false, "opus:AA-bplcon1.b04 sol:AA-bplcon1.c26 opus:AA-pf2h.c1 sol:AA-bplcon1.c9"},
            {5,0x0020,"PF2H3",false,4, true, true, false, "opus:AA-bplcon1.b05 sol:AA-bplcon1.c25 opus:AA-pf2h.c1 sol:AA-bplcon1.c9"},
            {6,0x0040,"PF2H4",false,8, true, true, false, "opus:AA-bplcon1.b06 sol:AA-bplcon1.c24 opus:AA-pf2h.c1 sol:AA-bplcon1.c9"},
            {7,0x0080,"PF2H5",false,16,true, true, false, "opus:AA-bplcon1.b07 sol:AA-bplcon1.c23 opus:AA-pf2h.c1 sol:AA-bplcon1.c9"},
            {8,0x0100,"PF1H0",true, 0, false,false,true,  "opus:AA-bplcon1.b08 opus:AA-pf1h.c1 sol:AA-bplcon1.c10"},
            {9,0x0200,"PF1H1",true, 1, true, true, false, "opus:AA-bplcon1.b09 opus:AA-pf1h.c1 sol:AA-bplcon1.c10"},
            {10,0x0400,"PF1H6",true,32,false,true, false, "opus:AA-bplcon1.b10 opus:AA-pf1h.c1 sol:AA-bplcon1.c10"},
            {11,0x0800,"PF1H7",true,64,false,true, false, "opus:AA-bplcon1.b11 opus:AA-pf1h.c1 sol:AA-bplcon1.c10"},
            {12,0x1000,"PF2H0",false,0, false,false,true,  "opus:AA-bplcon1.b12 opus:AA-pf2h.c1 sol:AA-bplcon1.c9"},
            {13,0x2000,"PF2H1",false,1, true, true, false, "opus:AA-bplcon1.b13 opus:AA-pf2h.c1 sol:AA-bplcon1.c9"},
            {14,0x4000,"PF2H6",false,32,false,true, false, "opus:AA-bplcon1.b14 opus:AA-pf2h.c1 sol:AA-bplcon1.c9"},
            {15,0x8000,"PF2H7",false,64,false,true, false, "opus:AA-bplcon1.b15 opus:AA-pf2h.c1 sol:AA-bplcon1.c9"},
        };
        // A mutation selects one row and one branch; the reference runs that row needs are the only ones run.
        const std::string msel = g_mutate ? g_mutate : "";
        const bool p2_full = !g_mutate || selp == "P5" || msel == "P2";
        auto msel_has = [&](const char* prefix) { return msel.compare(0, std::strlen(prefix), prefix) == 0; };
        const bool need_a1 = p2_full || msel_has("P2A1-") || msel_has("P2S-");
        const bool need_a2 = p2_full || msel_has("P2A2-") || msel_has("P2S-");
        const bool need_b  = p2_full || msel_has("P2B-");

        // reference runs, one per stimulus mode
        BarsConfig a1_ref; a1_ref.bplcon0 = 0x2000; a1_ref.fmode = 0x0000; a1_ref.buf1 = bars_buf(8); a1_ref.buf2 = bars_buf(24);
        BarsConfig a2_ref; a2_ref.bplcon0 = 0x2000; a2_ref.fmode = 0x0003; a2_ref.buf1 = bars_buf(8); a2_ref.buf2 = bars_buf(24);
        BarsConfig b_ref;  b_ref.bplcon0 = 0x2040; b_ref.fmode = 0x0000; b_ref.ddfstrt = kDdfstrtShres; b_ref.ddfstop = kDdfstopShres;
        b_ref.buf1 = stripe_buf(8,10,16); b_ref.buf2 = stripe_buf(56,58,16);
        if (run_mutation("P2")) { a1_ref.buf1 = zero_buf(); a2_ref.buf1 = zero_buf(); b_ref.buf1 = zero_buf(); }   // plane 1 shows no data

        RunOut ra1r, ra2r, rbr;
        if (need_a1) ra1r = run_bars(a1_ref);
        if (need_a2) ra2r = run_bars(a2_ref);
        if (need_b)  rbr = run_bars(b_ref);
        auto f1_a1r = find_f1(ra1r.video), f1_a2r = find_f1(ra2r.video), f1_br = find_f1(rbr.video);
        size_t f3_a1r = f1_a1r.value_or(0)+2, f3_a2r = f1_a2r.value_or(0)+2, f3_br = f1_br.value_or(0)+2;
        EdgeMeasure e1_a1r = measure_edge_all_lines(ra1r.video, f3_a1r, kC01r,kC01g,kC01b);
        EdgeMeasure e2_a1r = measure_edge_all_lines(ra1r.video, f3_a1r, kC02r,kC02g,kC02b);
        EdgeMeasure e1_a2r = measure_edge_all_lines(ra2r.video, f3_a2r, kC01r,kC01g,kC01b);
        EdgeMeasure e2_a2r = measure_edge_all_lines(ra2r.video, f3_a2r, kC02r,kC02g,kC02b);
        EdgeMeasure e1_br  = measure_edge_all_lines(rbr.video,  f3_br,  kC01r,kC01g,kC01b);
        EdgeMeasure e2_br  = measure_edge_all_lines(rbr.video,  f3_br,  kC02r,kC02g,kC02b);
        if (p2_full) {
            g.expect(f1_a1r.has_value() && f1_a2r.has_value() && f1_br.has_value(), "P2 fixture: F1 located in all three reference runs (a1/a2/b)");
            g.expect(e1_a1r.ok && e2_a1r.ok && e1_a2r.ok && e2_a2r.ok && e1_br.ok && e2_br.ok,
                     "P2 precondition S3b: E1/E2 exist and are equal on every window line of F3 in all three reference runs");
        }

        std::vector<double> offs = {12, 18, 24, 30, 36};

        for (auto& row : rows) {
            if (g_mutate && selp == "P5" && row.bit < 8) continue;   // P5 needs only the rows 8-15
            if (g_mutate && selp == "P2" && msel.find(row.name) == std::string::npos) continue;   // a mutation names one row
            double d1a1 = 0, d2a1 = 0, d1a2 = 0, d2a2 = 0;
            bool a1_ok = true, a2_ok = true;
            const std::string tok_a1 = "P2A1-" + std::string(row.name);
            const std::string tok_a2 = "P2A2-" + std::string(row.name);
            const std::string tok_b  = "P2B-" + std::string(row.name);
            const std::string tok_s  = "P2S-" + std::string(row.name);
            // The contrast of a row: the neighbouring bit written in place of the row's own (rows 0-7), or the ECS
            // chip set selected (rows 8-15, whose bits the AA chip set alone provides).
            const uint16_t neighbour = row.bit >= 1 ? (uint16_t)(1u << (row.bit - 1)) : (uint16_t)(1u << (row.bit + 1));
            auto contrast = [&](BarsConfig& c, bool on) {
                if (!on) return;
                if (row.bit >= 8) c.chipset = 0x08; else c.bplcon1 = neighbour;
            };
            if (row.a1 && need_a1) {
                BarsConfig c = a1_ref; c.bplcon1 = row.value;
                contrast(c, run_mutation(tok_a1));
                RunOut r = run_bars(c);
                auto f1 = find_f1(r.video);
                size_t f3 = f1.value_or(0) + 2;
                EdgeMeasure e1 = measure_edge_all_lines(r.video, f3, kC01r,kC01g,kC01b);
                EdgeMeasure e2 = measure_edge_all_lines(r.video, f3, kC02r,kC02g,kC02b);
                auto d1 = delta_ops(e1.offset, e1_a1r.offset), d2 = delta_ops(e2.offset, e2_a1r.offset);
                a1_ok = f1.has_value() && e1.ok && e2.ok && d1 && d2;
                double exp1 = row.pf1 ? row.op : 0, exp2 = row.pf1 ? 0 : row.op;
                a1_ok = a1_ok && near_int(*d1, exp1) && near_int(*d2, exp2);
                if (d1) d1a1 = *d1;
                if (d2) d2a1 = *d2;
                g.expect(a1_ok, tok_a1 + " " + row.claims + ": BPLCON1=$" + hex4(row.value) + " (" + row.name +
                          " alone) gives Delta1=" + num(exp1) + " op, Delta2=" + num(exp2) + " op on every window line of F3, LORES FMODE=0");
            }
            if (row.a2 && need_a2) {
                BarsConfig c = a2_ref; c.bplcon1 = row.value;
                contrast(c, run_mutation(tok_a2) || run_mutation(tok_s));
                RunOut r = run_bars(c);
                auto f1 = find_f1(r.video);
                size_t f3 = f1.value_or(0) + 2;
                EdgeMeasure e1 = measure_edge_all_lines(r.video, f3, kC01r,kC01g,kC01b);
                EdgeMeasure e2 = measure_edge_all_lines(r.video, f3, kC02r,kC02g,kC02b);
                auto d1 = delta_ops(e1.offset, e1_a2r.offset), d2 = delta_ops(e2.offset, e2_a2r.offset);
                a2_ok = f1.has_value() && e1.ok && e2.ok && d1 && d2;
                double exp1 = row.pf1 ? row.op : 0, exp2 = row.pf1 ? 0 : row.op;
                a2_ok = a2_ok && near_int(*d1, exp1) && near_int(*d2, exp2);
                if (d1) d1a2 = *d1;
                if (d2) d2a2 = *d2;
                g.expect(a2_ok, tok_a2 + " " + row.claims + ": BPLCON1=$" + hex4(row.value) + " (" + row.name +
                          " alone) gives Delta1=" + num(exp1) + " op, Delta2=" + num(exp2) + " op on every window line of F3, LORES FMODE=$0003");
                if (row.a1 && need_a1) g.expect(near_int(d1a1, d1a2, 0.6) && near_int(d2a1, d2a2, 0.6),
                          tok_s + " " + row.claims + ": (a1) and (a2) give the same Delta for " + row.name);
            }
            bool row_ok = (!row.a1 || !need_a1 || a1_ok) && (!row.a2 || !need_a2 || a2_ok);
            if (row.b && need_b) {
                BarsConfig c = b_ref; c.bplcon1 = row.value;
                contrast(c, run_mutation(tok_b));
                RunOut r = run_bars(c);
                auto f1 = find_f1(r.video);
                size_t f3 = f1.value_or(0) + 2;
                uint8_t pr = row.pf1 ? kC01r : kC02r, pg = row.pf1 ? kC01g : kC02g, pb = row.pf1 ? kC01b : kC02b;
                uint8_t or_ = row.pf1 ? kC02r : kC01r, og = row.pf1 ? kC02g : kC01g, ob = row.pf1 ? kC02b : kC01b;
                double own_ref_edge = row.pf1 ? *e1_br.offset : *e2_br.offset;
                double other_ref_edge = row.pf1 ? *e2_br.offset : *e1_br.offset;
                auto sig_own_ref = stripe_signature(rbr.video, f3_br, pr,pg,pb, own_ref_edge, op, offs);
                auto sig_own_new = stripe_signature(r.video, f3, pr,pg,pb, own_ref_edge, op, offs);
                auto sig_other_ref = stripe_signature(rbr.video, f3_br, or_,og,ob, other_ref_edge, op, offs);
                auto sig_other_new = stripe_signature(r.video, f3, or_,og,ob, other_ref_edge, op, offs);
                bool own_changed = sig_own_ref && sig_own_new && *sig_own_ref != *sig_own_new;
                bool other_same = sig_other_ref && sig_other_new && *sig_other_ref == *sig_other_new;
                bool b_ok = f1.has_value() && own_changed && other_same;
                row_ok = row_ok && b_ok;
                g.expect(b_ok, tok_b + " " + row.claims + ": BPLCON1=$" + hex4(row.value) + " (" + row.name +
                          " alone, the SHRES 0.5-op bit) flips this playfield's stripe pattern and leaves the other playfield's unchanged, SHRES FMODE=0");
            }
            p2_row_ok[row.bit] = row_ok;
        }
    }

    // =======================================================================
    // P3 -- the scroll range is quadrupled (AA p.17, p.31, p.3)
    // =======================================================================
    if (!g_mutate || selp == "P3") {
        BarsConfig ref; ref.bplcon0 = 0x2000; ref.fmode = 0x0003; ref.buf1 = bars_buf(8); ref.buf2 = bars_buf(24);
        if (run_mutation("P3-PRE")) ref.buf1 = zero_buf();   // plane 1 shows no data in the reference run
        RunOut rref = run_bars(ref);
        auto f1r = find_f1(rref.video);
        size_t f3r = f1r.value_or(0) + 2;
        EdgeMeasure e1r = measure_edge_all_lines(rref.video, f3r, kC01r,kC01g,kC01b);
        EdgeMeasure e2r = measure_edge_all_lines(rref.video, f3r, kC02r,kC02g,kC02b);
        g.expect(f1r.has_value() && e1r.ok && e2r.ok, "P3 precondition (as P2(a2)): E1/E2 found and equal on every window line of the reference run's F3");

        struct P3Row { uint16_t value; double exp1lo, exp1hi, exp2lo, exp2hi; const char* label; };
        std::vector<P3Row> p3rows = {
            {0x0E0F, 127,127, 0,0, "$0E0F (bits 0-3,9,10,11 = PF1H2..H7 minus H0): Delta1=127 op, Delta2=0 op"},
            {0x0F0F, 127,128, 0,0, "$0F0F (as above plus bit 8 = PF1H0): Delta1=127 or 128 op, Delta2=0 op"},
            {0xE0F0, 0,0, 127,127, "$E0F0 (bits 4-7,13,14,15 = PF2H2..H7 minus H0): Delta1=0 op, Delta2=127 op"},
            {0xF0F0, 0,0, 127,128, "$F0F0 (as above plus bit 12 = PF2H0): Delta1=0 op, Delta2=127 or 128 op"},
        };
        bool p3_all = true;
        for (auto& pr : p3rows) {
            BarsConfig c = ref; c.bplcon1 = pr.value;
            if (run_mutation("P3")) c.chipset = 0x08;
            RunOut r = run_bars(c);
            auto f1 = find_f1(r.video);
            size_t f3 = f1.value_or(0) + 2;
            EdgeMeasure e1 = measure_edge_all_lines(r.video, f3, kC01r,kC01g,kC01b);
            EdgeMeasure e2 = measure_edge_all_lines(r.video, f3, kC02r,kC02g,kC02b);
            auto d1 = delta_ops(e1.offset, e1r.offset), d2 = delta_ops(e2.offset, e2r.offset);
            bool ok = f1.has_value() && e1.ok && e2.ok && d1 && d2 &&
                      *d1 >= pr.exp1lo - 0.3 && *d1 <= pr.exp1hi + 0.3 && *d2 >= pr.exp2lo - 0.3 && *d2 <= pr.exp2hi + 0.3;
            p3_all = p3_all && ok;
            g.expect(ok, "P3-" + hex4(pr.value) + ": BPLCON1=" + std::string(pr.label) +
                      ", LORES FMODE=$0003, on every window line of F3");
            checks.note("P3 " + std::string(pr.label) + " measured Delta1=" + opt_num(d1) + " Delta2=" + opt_num(d2));
        }
        g.expect(p3_all, "P3-AGG opus:AA-pf1h.c3 sol:AA-bplcon1.c13: with 64-bit bit-plane fetch in LORES each playfield's delay reaches 254 and 255 SHRES pixels (BPLCON1 = $0E0F, $0F0F, $E0F0 and $F0F0), the earlier maximum being 15 LORES pixels (AA p.17 'the scroll range has been quadrupled')");
    }

    // =======================================================================
    // P4 -- RST_ values of BPLCON1 bits 15-8 (AA p.16)
    // =======================================================================
    std::optional<double> p4a_delta1, p4a_delta2; // shared with P5
    bool p4a_ok = false, p4b_ok = false;
    if (!g_mutate || selp == "P4" || selp == "P5") {
        // The reference runs carry the reset state in F3 and BPLCON1 = $0000 in F5.
        // A mutated run changes only what BPLCON1 holds in F3: the CPU writes the
        // value at the first VERTB edge. Its F5 side keeps coming from the
        // reference run, so the comparator is the one the reference runs use.
        // P4a-D1 / P4a-D2: $0200 / $2000 (PF1H1 / PF2H1) in F3, which the page
        // predicts to delay one playfield by 1 op; P4b-C1 / P4b-C2 / P5: $0000
        // written in F3, a live input through the same comparator.
        const bool ctrl_a = run_mutation("P5");
        const bool ctrl_b = run_mutation("P4b-C1") || run_mutation("P4b-C2") || run_mutation("P5");
        // ---- Run (a): LORES, FMODE=$0003, bars ----
        MidrunConfig ca;
        ca.bplcon0 = 0x2000; ca.fmode = 0x0003; ca.mid_val = 0x0000; ca.mid_edge = 5; ca.stop_edge = 6;
        ca.buf1 = bars_buf(8); ca.buf2 = bars_buf(24);
        if (run_mutation("P4-PRE")) ca.buf1 = zero_buf();   // plane 1 shows no data
        RunOut ra_ref = run_midrun(ca, kRunBudget);
        MidrunConfig ca_run = ca;
        bool mut_a = false;
        if (run_mutation("P4a-D1")) { ca_run.mid_val = 0x0200; mut_a = true; }
        if (run_mutation("P4a-D2")) { ca_run.mid_val = 0x2000; mut_a = true; }
        if (ctrl_a) { ca_run.mid_val = 0x0000; mut_a = true; }
        if (mut_a) { ca_run.mid_edge = 1; ca_run.stop_edge = 3; }
        RunOut ra_mut;
        if (mut_a) ra_mut = run_midrun(ca_run, kRunBudget);
        const RunOut& ra = mut_a ? ra_mut : ra_ref;

        g.expect(ra.loaded && ra.released && ra.fixture_ok, "P4 run(a) fixture: loaded, booted and stored $BEEF before any observation");

        auto f1a = find_f1(ra.video);
        auto f1a_ref = find_f1(ra_ref.video);
        g.expect(f1a.has_value(), "P4 run(a): F1 (first steady frame after DMA enable) located in the video trace");
        size_t f3a = f1a.value_or(0) + 2, f5a = f1a_ref.value_or(0) + 4;

        EdgeMeasure e1_reset = measure_edge_all_lines(ra.video, f3a /* F3: reset state */, kC01r,kC01g,kC01b);
        EdgeMeasure e2_reset = measure_edge_all_lines(ra.video, f3a, kC02r,kC02g,kC02b);
        g.expect(e1_reset.ok, "P4 run(a) precondition: bars visible with E1 equal on every window line of F3 (reset-state BPLCON1)");
        g.expect(e2_reset.ok, "P4 run(a) precondition: bars visible with E2 equal on every window line of F3 (reset-state BPLCON1)");

        EdgeMeasure e1_f5 = measure_edge_all_lines(ra_ref.video, f5a /* F5: BPLCON1 written to $0000 */, kC01r,kC01g,kC01b);
        EdgeMeasure e2_f5 = measure_edge_all_lines(ra_ref.video, f5a, kC02r,kC02g,kC02b);
        g.expect(e1_f5.ok && e2_f5.ok, "P4 run(a) precondition S3a/S3b: E1/E2 equal on every window line of F5 (BPLCON1=$0000 reference)");

        auto d1 = delta_ops(e1_reset.offset, e1_f5.offset);
        auto d2 = delta_ops(e2_reset.offset, e2_f5.offset);
        p4a_delta1 = d1; p4a_delta2 = d2;
        auto in_set = [](std::optional<double> d) {
            if (!d) return false;
            double r = std::round(*d);
            if (!near_int(*d, r, 0.26)) return false;
            int i = (int)r;
            return i >= 0 && i <= 30 && (i % 2 == 0);
        };
        bool d1_ok = in_set(d1), d2_ok = in_set(d2);
        p4a_ok = d1_ok && d2_ok;
        g.expect(d1_ok,
            "P4a-D1 opus:AA-bplcon1.b09.rst opus:AA-bplcon1.b10.rst opus:AA-bplcon1.b11.rst: after power-on reset and before any write to "
            "BPLCON1 (AA p.16 RST_ column), playfield-1's delay in F3 is one of 0,2,...,30 op (PF1H1/H6/H7 read 0)");
        g.expect(d2_ok,
            "P4a-D2 opus:AA-bplcon1.b13.rst opus:AA-bplcon1.b14.rst opus:AA-bplcon1.b15.rst: after power-on reset and before any write to "
            "BPLCON1 (AA p.16 RST_ column), playfield-2's delay in F3 is one of 0,2,...,30 op (PF2H1/H6/H7 read 0)");
        checks.note("P4 run(a) measured: F3-vs-F5 Delta1=" + opt_num(d1) + " op, Delta2=" + opt_num(d2) + " op");

        // ---- Run (b): SHRES, FMODE=$0003, stripes ----
        MidrunConfig cb;
        cb.bplcon0 = 0x2040; cb.fmode = 0x0003; cb.ddfstrt = kDdfstrtShres; cb.ddfstop = kDdfstopShres;
        cb.mid_val = 0x0000; cb.mid_edge = 5; cb.stop_edge = 6;
        cb.buf1 = stripe_buf(8, 10, 16);
        cb.buf2 = stripe_buf(56, 58, 16);
        if (run_mutation("P4-PRE")) cb.buf1 = zero_buf();
        RunOut rb_ref = run_midrun(cb, kRunBudget);
        MidrunConfig cb_run = cb;
        if (ctrl_b) { cb_run.mid_val = 0x0000; cb_run.mid_edge = 1; cb_run.stop_edge = 3; }
        RunOut rb_mut;
        if (ctrl_b) rb_mut = run_midrun(cb_run, kRunBudget);
        const RunOut& rb = ctrl_b ? rb_mut : rb_ref;
        g.expect(rb.loaded && rb.released && rb.fixture_ok, "P4 run(b) fixture: loaded, booted and stored $BEEF before any observation");

        auto f1b = find_f1(rb.video);
        auto f1b_ref = find_f1(rb_ref.video);
        g.expect(f1b.has_value(), "P4 run(b): F1 (first steady frame after DMA enable) located in the video trace");
        size_t f3b = f1b.value_or(0) + 2, f5b = f1b_ref.value_or(0) + 4;

        EdgeMeasure e1b_f5 = measure_edge_all_lines(rb_ref.video, f5b, kC01r,kC01g,kC01b);
        EdgeMeasure e2b_f5 = measure_edge_all_lines(rb_ref.video, f5b, kC02r,kC02g,kC02b);
        g.expect(e1b_f5.ok && e2b_f5.ok, "P4 run(b) precondition: stripe solid-part edges E1/E2 equal on every window line of F5 (BPLCON1=$0000 reference)");

        std::vector<double> offs = {12, 18, 24, 30, 36};
        auto sig1_f5 = e1b_f5.offset ? stripe_signature(rb_ref.video, f5b, kC01r,kC01g,kC01b, *e1b_f5.offset, op, offs) : std::nullopt;
        auto sig1_f3 = e1b_f5.offset ? stripe_signature(rb.video, f3b, kC01r,kC01g,kC01b, *e1b_f5.offset, op, offs) : std::nullopt;
        auto sig2_f5 = e2b_f5.offset ? stripe_signature(rb_ref.video, f5b, kC02r,kC02g,kC02b, *e2b_f5.offset, op, offs) : std::nullopt;
        auto sig2_f3 = e2b_f5.offset ? stripe_signature(rb.video, f3b, kC02r,kC02g,kC02b, *e2b_f5.offset, op, offs) : std::nullopt;

        bool c1_eq = sig1_f5 && sig1_f3 && *sig1_f5 == *sig1_f3;
        bool c2_eq = sig2_f5 && sig2_f3 && *sig2_f5 == *sig2_f3;
        p4b_ok = c1_eq && c2_eq;
        g.expect(c1_eq,
            "P4b-C1 opus:AA-bplcon1.b08.rst: after power-on reset, PF1H0 (BPLCON1 bit 8) reads 0 -- the plane-1 stripe pattern (period 2 "
            "SHRES pixels) samples identically in F3 (reset state) and F5 (BPLCON1=$0000)");
        g.expect(c2_eq,
            "P4b-C2 opus:AA-bplcon1.b12.rst: after power-on reset, PF2H0 (BPLCON1 bit 12) reads 0 -- the plane-2 stripe pattern samples "
            "identically in F3 (reset state) and F5 (BPLCON1=$0000)");
    }

    // =======================================================================
    // P5 -- BPLCON1 bits 15-8: tabled name and reset value together
    // (the conjunction of P2's rows 8-15 and P4)
    // =======================================================================
    if (!g_mutate || selp == "P5") {
        bool p2_rows_8_15_ok = true;
        for (int b = 8; b <= 15; ++b) p2_rows_8_15_ok = p2_rows_8_15_ok && p2_row_ok[b];
        bool p5_ok = p2_rows_8_15_ok && p4a_ok && p4b_ok;
        g.expect(p5_ok,
            "P5 sol:AA-bplcon1.c15 sol:AA-bplcon1.c16 sol:AA-bplcon1.c17 sol:AA-bplcon1.c18 sol:AA-bplcon1.c19 sol:AA-bplcon1.c20 "
            "sol:AA-bplcon1.c21 sol:AA-bplcon1.c22: each of BPLCON1 bits 15-8 is the field AA p.16 names for it (P2's rows 8-15) "
            "and RST_ initialises it to 0 (P4)");
        checks.note("P5 conjuncts: P2 rows 8-15 all-ok=" + std::to_string(p2_rows_8_15_ok) +
                     "; P4 run(a) even/0-30=" + std::to_string(p4a_ok) + "; P4 run(b) stripe equality=" + std::to_string(p4b_ok));
    }

    }
    if (want("UNIT") || want("P11") || want("P12") || want("P14")) prop_unit(checks, g);
    if (want("P6")) prop_p6(checks, g);
    if (want("P7")) prop_p7(checks, g);
    if (want("P8")) prop_p8(checks, g);
    if (want("P9")) prop_p9(checks, g);
    if (want("P10")) prop_p10(checks, g);
    if (want("P11")) prop_p11(checks, g);
    if (want("P12")) prop_p12(checks, g);
    if (want("P13")) prop_p13(checks, g);
    if (want("P14")) prop_p14(checks, g);
    if (want("P15")) prop_p15(checks, g);
    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
