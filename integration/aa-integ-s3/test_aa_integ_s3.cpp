// test_aa_integ_s3.cpp
//
// aa-integ-s3: see SPECIFICATION.md. Bit-plane and sprite pointers and data,
// sprite position and control, the UHRES sprite window, collision control.
//
// Every run selects the AA chip set with chipset_config = 0x10 (the ECS runs of
// S16 select 0x08); the chip set is an input of nanomig.v and not a property of
// the build, so every proposition, precondition and anchor is graded
// (PASS:/FAIL:) on both cores.
//
// NMH_MUTATE selects one mutation in place of the normal run: it names the
// leading id of one assertion or precondition, and only the proposition that
// owns it runs, with a changed stimulus and the unchanged comparator. The
// normal run (NMH_MUTATE unset) never runs a mutant. NMH_ONLY=<id> runs one
// proposition alone without a mutation.
//
// MUTATION_RECORDS_BEGIN
// MUTATION FIXTURE stimulus red NMH_MUTATE=FIXTURE: the program variant without the setup-marker store runs; tg68k: FAIL FIXTURE; fx68k: FAIL FIXTURE
// MUTATION B1 stimulus red NMH_MUTATE=B1: the Copper list writes l_3 + 0x0100 as the BPL3PTL data word; tg68k: FAIL B1; fx68k: FAIL B1
// MUTATION B1 stimulus red NMH_MUTATE=B1-b: the BPU = 3 run carries BPU = 4 in BPLCON0; tg68k: FAIL B1; fx68k: FAIL B1
// MUTATION B1-PC-SETUP stimulus red NMH_MUTATE=B1-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL B1-PC-SETUP B1-PC-FRAME B1-PC-MOVE B1-PC-BPL; fx68k: FAIL B1-PC-SETUP B1-PC-FRAME B1-PC-MOVE B1-PC-BPL
// MUTATION B1-PC-FRAME stimulus red NMH_MUTATE=B1-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL B1-PC-FRAME B1-PC-MOVE B1-PC-BPL; fx68k: FAIL B1-PC-FRAME B1-PC-MOVE B1-PC-BPL
// MUTATION B1-PC-MOVE stimulus red NMH_MUTATE=B1-PC-MOVE: the BPL1PTL data word in the list differs from the intended value; tg68k: FAIL B1-PC-MOVE; fx68k: FAIL B1-PC-MOVE
// MUTATION B1-PC-BPL stimulus red NMH_MUTATE=B1-PC-BPL: BPLEN is clear; tg68k: FAIL B1-PC-BPL; fx68k: FAIL B1-PC-BPL
// MUTATION B1-PC-BPLN stimulus red NMH_MUTATE=B1-PC-BPLN: BPLEN is clear in the BPU = 3 run; tg68k: FAIL B1-PC-BPLN; fx68k: FAIL B1-PC-BPLN
// MUTATION B1-PC-STATIC stimulus red NMH_MUTATE=B1-PC-STATIC: the program writes BPL1PTH; tg68k: FAIL B1-PC-STATIC; fx68k: FAIL B1-PC-STATIC
// MUTATION B2 stimulus red NMH_MUTATE=B2: A_b is replaced by A_a in both variants; tg68k: FAIL B2; fx68k: FAIL B2
// MUTATION B2-PC-SETUP stimulus red NMH_MUTATE=B2-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL B2-PC-SETUP B2-PC-FRAME; fx68k: FAIL B2-PC-SETUP B2-PC-FRAME
// MUTATION B2-PC-FRAME stimulus red NMH_MUTATE=B2-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL B2-PC-FRAME; fx68k: FAIL B2-PC-FRAME
// MUTATION B2-PC-BPL stimulus red NMH_MUTATE=B2-PC-BPL: BPLEN is clear; tg68k: FAIL B2-PC-BPL; fx68k: FAIL B2-PC-BPL
// MUTATION B2-PC-MOVE stimulus red NMH_MUTATE=B2-PC-MOVE: the Copper variant replaces the two data words after the vertical-blank block of F3 has been fetched; tg68k: FAIL B2-PC-MOVE; fx68k: FAIL B2-PC-MOVE
// MUTATION B2-PC-MARKER stimulus red NMH_MUTATE=B2-PC-MARKER: the CPU variant writes the pointers at line 0x80; tg68k: FAIL B2-PC-MARKER; fx68k: FAIL B2-PC-MARKER
// MUTATION B3 stimulus red NMH_MUTATE=B3: plane 1 points to a region filled with zeros; tg68k: FAIL B3; fx68k: FAIL B3
// MUTATION B3-PC-SETUP stimulus red NMH_MUTATE=B3-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL B3-PC-SETUP B3-PC-FRAME B3-PC-BPL; fx68k: FAIL B3-PC-SETUP B3-PC-FRAME B3-PC-BPL
// MUTATION B3-PC-FRAME stimulus red NMH_MUTATE=B3-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL B3-PC-FRAME B3-PC-TAG B3-PC-PIXEL B3-PC-BPL; fx68k: FAIL B3-PC-FRAME B3-PC-TAG B3-PC-PIXEL B3-PC-BPL
// MUTATION B3-PC-TAG stimulus red NMH_MUTATE=B3-PC-TAG: the tag block carries no MOVE COLOR00; tg68k: FAIL B3-PC-TAG B3-PC-PIXEL; fx68k: FAIL B3-PC-TAG B3-PC-PIXEL
// MUTATION B3-PC-PIXEL stimulus red NMH_MUTATE=B3-PC-PIXEL: HTOTAL = 0x00E3 with BEAMCON0 = 0x08A0 (228 colour clocks per line); tg68k: FAIL B3-PC-PIXEL; fx68k: FAIL B3-PC-PIXEL
// MUTATION B3-PC-BPL stimulus red NMH_MUTATE=B3-PC-BPL: BPLEN is clear; tg68k: FAIL B3-PC-BPL B3-PC-WORDS; fx68k: FAIL B3-PC-BPL B3-PC-WORDS
// MUTATION B3-PC-WORDS stimulus red NMH_MUTATE=B3-PC-WORDS: DDFSTOP = 0x0040 (two words per line); tg68k: FAIL B3-PC-WORDS; fx68k: FAIL B3-PC-WORDS
// MUTATION B4 stimulus red NMH_MUTATE=B4: the BPL1DAT write of E_3 is omitted; tg68k: FAIL B4; fx68k: FAIL B4
// MUTATION B4 stimulus red NMH_MUTATE=B4-d: the BPL1DAT write of every D_k is made at line g_k; tg68k: FAIL B4; fx68k: FAIL B4
// MUTATION B4-PC-SETUP stimulus red NMH_MUTATE=B4-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL B4-PC-SETUP B4-PC-FRAME B4-PC-BEAM B4-PC-MOVE; fx68k: FAIL B4-PC-SETUP B4-PC-FRAME B4-PC-BEAM B4-PC-MOVE
// MUTATION B4-PC-FRAME stimulus red NMH_MUTATE=B4-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL B4-PC-FRAME B4-PC-TAG B4-PC-PIXEL B4-PC-BEAM B4-PC-MOVE; fx68k: FAIL B4-PC-FRAME B4-PC-TAG B4-PC-PIXEL B4-PC-BEAM B4-PC-MOVE
// MUTATION B4-PC-TAG stimulus red NMH_MUTATE=B4-PC-TAG: the tag block carries no MOVE COLOR00; tg68k: FAIL B4-PC-TAG B4-PC-PIXEL; fx68k: FAIL B4-PC-TAG B4-PC-PIXEL
// MUTATION B4-PC-PIXEL stimulus red NMH_MUTATE=B4-PC-PIXEL: HTOTAL = 0x00E3 with BEAMCON0 = 0x08A0 (228 colour clocks per line); tg68k: FAIL B4-PC-PIXEL; fx68k: FAIL B4-PC-PIXEL
// MUTATION B4-PC-NODMA stimulus red NMH_MUTATE=B4-PC-NODMA: BPLEN is set and the plane pointers point to Z; tg68k: FAIL B4-PC-TAG B4-PC-PIXEL B4-PC-NODMA B4-PC-BEAM; fx68k: FAIL B4-PC-TAG B4-PC-PIXEL B4-PC-NODMA B4-PC-BEAM
// MUTATION B4-PC-BEAM stimulus red NMH_MUTATE=B4-PC-BEAM: the CPU events target H = 0xE0; tg68k: FAIL B4-PC-BEAM; fx68k: FAIL B4-PC-BEAM
// MUTATION B4-PC-MOVE stimulus red NMH_MUTATE=B4-PC-MOVE: a Copper event MOVE carries a data word that differs from the intended one; tg68k: FAIL B4-PC-MOVE; fx68k: FAIL B4-PC-MOVE
// MUTATION S1 stimulus red NMH_MUTATE=S1: sprite 4 points to l_5 + 0x0100 and its list lies there; tg68k: FAIL S1; fx68k: FAIL S1
// MUTATION S1-PC-SETUP stimulus red NMH_MUTATE=S1-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL S1-PC-SETUP S1-PC-FRAME S1-PC-MOVE S1-PC-SPR; fx68k: FAIL S1-PC-SETUP S1-PC-FRAME S1-PC-MOVE S1-PC-SPR
// MUTATION S1-PC-FRAME stimulus red NMH_MUTATE=S1-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL S1-PC-FRAME S1-PC-MOVE S1-PC-SPR; fx68k: FAIL S1-PC-FRAME S1-PC-MOVE S1-PC-SPR
// MUTATION S1-PC-MOVE stimulus red NMH_MUTATE=S1-PC-MOVE: the SPR0PTL data word differs from the intended value; tg68k: FAIL S1-PC-MOVE S1-PC-SPR; fx68k: FAIL S1-PC-MOVE S1-PC-SPR
// MUTATION S1-PC-SPR stimulus red NMH_MUTATE=S1-PC-SPR: SPREN is clear; tg68k: FAIL S1-PC-SPR; fx68k: FAIL S1-PC-SPR
// MUTATION S2 stimulus red NMH_MUTATE=S2: list B is replaced by list A in both variants; tg68k: FAIL S2; fx68k: FAIL S2
// MUTATION S2-PC-SETUP stimulus red NMH_MUTATE=S2-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL S2-PC-SETUP S2-PC-FRAME; fx68k: FAIL S2-PC-SETUP S2-PC-FRAME
// MUTATION S2-PC-FRAME stimulus red NMH_MUTATE=S2-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL S2-PC-FRAME; fx68k: FAIL S2-PC-FRAME
// MUTATION S2-PC-SPR stimulus red NMH_MUTATE=S2-PC-SPR: SPREN is clear; tg68k: FAIL S2-PC-SPR; fx68k: FAIL S2-PC-SPR
// MUTATION S2-PC-MOVE stimulus red NMH_MUTATE=S2-PC-MOVE: the Copper variant replaces the two data words after the vertical-blank block of F3 has been fetched; tg68k: FAIL S2-PC-MOVE; fx68k: FAIL S2-PC-MOVE
// MUTATION S2-PC-MARKER stimulus red NMH_MUTATE=S2-PC-MARKER: the CPU variant writes the pointer at line 0x80; tg68k: FAIL S2-PC-MARKER; fx68k: FAIL S2-PC-MARKER
// MUTATION S3 stimulus red NMH_MUTATE=S3: the SPR3CTL write at d_3 is omitted; tg68k: FAIL S3; fx68k: FAIL S3
// MUTATION S3 stimulus red NMH_MUTATE=S3-b: the SPR3DATA write at d_3 + 8 is omitted; tg68k: FAIL S3; fx68k: FAIL S3
// MUTATION S3 stimulus red NMH_MUTATE=S3-c: SPR3DATA is written in place of SPR3DATB at d_3 + 4; tg68k: FAIL S3; fx68k: FAIL S3
// MUTATION S3-PC-SETUP stimulus red NMH_MUTATE=S3-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL S3-PC-SETUP S3-PC-FRAME S3-PC-F1 S3-PC-BEAM; fx68k: FAIL S3-PC-SETUP S3-PC-FRAME S3-PC-F1 S3-PC-BEAM
// MUTATION S3-PC-FRAME stimulus red NMH_MUTATE=S3-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL S3-PC-FRAME S3-PC-TAG S3-PC-PIXEL S3-PC-F1 S3-PC-BEAM; fx68k: FAIL S3-PC-FRAME S3-PC-TAG S3-PC-PIXEL S3-PC-F1 S3-PC-BEAM
// MUTATION S3-PC-TAG stimulus red NMH_MUTATE=S3-PC-TAG: the tag block carries no MOVE COLOR00; tg68k: FAIL S3-PC-TAG S3-PC-PIXEL S3-PC-F1; fx68k: FAIL S3-PC-TAG S3-PC-PIXEL S3-PC-F1
// MUTATION S3-PC-PIXEL stimulus red NMH_MUTATE=S3-PC-PIXEL: HTOTAL = 0x00E3 with BEAMCON0 = 0x08A0 (228 colour clocks per line); tg68k: FAIL S3-PC-PIXEL; fx68k: FAIL S3-PC-PIXEL
// MUTATION S3-PC-F1 stimulus red NMH_MUTATE=S3-PC-F1: SPRxDATA = SPRxDATB = 0 at setup; tg68k: FAIL S3-PC-F1; fx68k: FAIL S3-PC-F1
// MUTATION S3-PC-BEAM stimulus red NMH_MUTATE=S3-PC-BEAM: the CPU events target H = 0xE0; tg68k: FAIL S3-PC-BEAM; fx68k: FAIL S3-PC-BEAM
// MUTATION S3-PC-STATIC stimulus red NMH_MUTATE=S3-PC-STATIC: SPREN is set by the program; tg68k: FAIL S3-PC-F1 S3-PC-STATIC; fx68k: FAIL S3-PC-F1 S3-PC-STATIC
// MUTATION S4 stimulus red NMH_MUTATE=S4: DATA = DATB = 0x8007; tg68k: FAIL S4; fx68k: FAIL S4
// MUTATION S4-PC-SETUP stimulus red NMH_MUTATE=S4-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL S4-PC-SETUP S4-PC-FRAME; fx68k: FAIL S4-PC-SETUP S4-PC-FRAME
// MUTATION S4-PC-FRAME stimulus red NMH_MUTATE=S4-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL S4-PC-FRAME S4-PC-TAG S4-PC-PIXEL; fx68k: FAIL S4-PC-FRAME S4-PC-TAG S4-PC-PIXEL
// MUTATION S4-PC-TAG stimulus red NMH_MUTATE=S4-PC-TAG: the tag block carries no MOVE COLOR00; tg68k: FAIL S4-PC-TAG S4-PC-PIXEL; fx68k: FAIL S4-PC-TAG S4-PC-PIXEL
// MUTATION S4-PC-PIXEL stimulus red NMH_MUTATE=S4-PC-PIXEL: HTOTAL = 0x00E3 with BEAMCON0 = 0x08A0 (228 colour clocks per line); tg68k: FAIL S4-PC-PIXEL; fx68k: FAIL S4-PC-PIXEL
// MUTATION S5 stimulus red NMH_MUTATE=S5: the k = 3 shifted run sets bit k + 1 (SH4) in place of bit k (SH3); S5 and S5-SH10-SH3 grade the k = 3 pair alone in this run; tg68k: FAIL S5 S5-SH10-SH3; fx68k: FAIL S5 S5-SH10-SH3
// MUTATION S5-SH10-SH3 stimulus red NMH_MUTATE=S5: the k = 3 shifted run sets bit k + 1 (SH4) in place of bit k (SH3); S5 and S5-SH10-SH3 grade the k = 3 pair alone in this run; tg68k: FAIL S5 S5-SH10-SH3; fx68k: FAIL S5 S5-SH10-SH3
// MUTATION S5-SH1 stimulus green NMH_MUTATE=S5-SH1: live input through the same comparator: the k = 1 run sets bit 2 (SH2) in place of bit 1 (SH1) and the comparator is applied with k = 2; S5-SH1 grades this pair alone in this run; tg68k: PASS S5-SH1; fx68k: PASS S5-SH1
// MUTATION S5-PC-SETUP stimulus red NMH_MUTATE=S5-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL S5-PC-SETUP S5-PC-FRAME S5-PC-SPR S5-PC-BAND; fx68k: FAIL S5-PC-SETUP S5-PC-FRAME S5-PC-SPR S5-PC-BAND
// MUTATION S5-PC-FRAME stimulus red NMH_MUTATE=S5-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL S5-PC-FRAME S5-PC-TAG S5-PC-PIXEL S5-PC-SPR S5-PC-BAND; fx68k: FAIL S5-PC-FRAME S5-PC-TAG S5-PC-PIXEL S5-PC-SPR S5-PC-BAND
// MUTATION S5-PC-TAG stimulus red NMH_MUTATE=S5-PC-TAG: the tag block carries no MOVE COLOR00; tg68k: FAIL S5-PC-TAG S5-PC-PIXEL S5-PC-BAND; fx68k: FAIL S5-PC-TAG S5-PC-PIXEL S5-PC-BAND
// MUTATION S5-PC-PIXEL stimulus red NMH_MUTATE=S5-PC-PIXEL: HTOTAL = 0x00E3 with BEAMCON0 = 0x08A0 (228 colour clocks per line); tg68k: FAIL S5-PC-PIXEL; fx68k: FAIL S5-PC-PIXEL
// MUTATION S5-PC-SPR stimulus red NMH_MUTATE=S5-PC-SPR: SPREN is clear in the reference run; tg68k: FAIL S5-PC-SPR S5-PC-BAND; fx68k: FAIL S5-PC-SPR S5-PC-BAND
// MUTATION S5-PC-BAND stimulus red NMH_MUTATE=S5-PC-BAND: the k = 3 shifted run uses DATA = 0xC000 (a 4-output-pixel cS run); tg68k: FAIL S5-PC-BAND; fx68k: FAIL S5-PC-BAND
// MUTATION S6 stimulus red NMH_MUTATE=S6: the SH0 = 1 runs use H = 0x440 (SH0 = 0); tg68k: FAIL S6 (the line prints FAIL in the normal run as well); fx68k: FAIL S6 (the line prints FAIL in the normal run as well)
// MUTATION S6-PC-SETUP stimulus red NMH_MUTATE=S6-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL S6-PC-SETUP S6-PC-FRAME S6-PC-SPR S6-PC-WIDTH; fx68k: FAIL S6-PC-SETUP S6-PC-FRAME S6-PC-SPR S6-PC-WIDTH
// MUTATION S6-PC-FRAME stimulus red NMH_MUTATE=S6-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL S6-PC-FRAME S6-PC-TAG S6-PC-PIXEL S6-PC-SPR S6-PC-WIDTH; fx68k: FAIL S6-PC-FRAME S6-PC-TAG S6-PC-PIXEL S6-PC-SPR S6-PC-WIDTH
// MUTATION S6-PC-TAG stimulus red NMH_MUTATE=S6-PC-TAG: the tag block carries no MOVE COLOR00; tg68k: FAIL S6-PC-TAG S6-PC-PIXEL S6-PC-WIDTH; fx68k: FAIL S6-PC-TAG S6-PC-PIXEL S6-PC-WIDTH
// MUTATION S6-PC-PIXEL stimulus red NMH_MUTATE=S6-PC-PIXEL: HTOTAL = 0x00E3 with BEAMCON0 = 0x08A0 (228 colour clocks per line); tg68k: FAIL S6-PC-PIXEL; fx68k: FAIL S6-PC-PIXEL
// MUTATION S6-PC-SPR stimulus red NMH_MUTATE=S6-PC-SPR: SPREN is clear; tg68k: FAIL S6-PC-SPR S6-PC-WIDTH; fx68k: FAIL S6-PC-SPR S6-PC-WIDTH
// MUTATION S6-PC-WIDTH stimulus red NMH_MUTATE=S6-PC-WIDTH: SPRES = 01 (140-ns sprites); tg68k: FAIL S6-PC-WIDTH S6-PC-HALF; fx68k: FAIL S6-PC-WIDTH S6-PC-HALF
// MUTATION S6-PC-HALF stimulus red NMH_MUTATE=S6-PC-HALF: the 0x5555 runs use DATA = DATB = 0xFFFF; tg68k: FAIL S6-PC-HALF S6-CONTROL; fx68k: FAIL S6-PC-HALF S6-CONTROL
// MUTATION S6-CONTROL stimulus red NMH_MUTATE=S6-CONTROL: the 0x5555 runs use DATA = DATB = 0xAAAA; tg68k: FAIL S6-PC-HALF S6-CONTROL; fx68k: FAIL S6-PC-HALF S6-CONTROL
// MUTATION S7 stimulus impossible NMH_MUTATE=S7: R0 writes SPRxCTL = 0x0010 (SH1 = 1); the stimulus runs, but the line prints NOT GRADED on both cores in every run, because S7-PC-SH1 and S7-PC-SH0 print FAIL (SH1 and SH0 do not act, see S5-SH1 and S6); the comparators of S7 are live on S7-CONTROL, S7-CONTROL-SH1 and S7-CONTROL-V; tg68k: FAIL S7-PC-SH1 S7-PC-SH0 (target NOT GRADED); fx68k: FAIL S7-PC-SH1 S7-PC-SH0 (target NOT GRADED)
// MUTATION S7 stimulus impossible NMH_MUTATE=S7-b: R0s writes SPRxCTL = 0x0008 (SH0 = 1); the stimulus runs, but the line prints NOT GRADED on both cores in every run, because S7-PC-SH0 and S7-PC-SH1 print FAIL (SH0 and SH1 do not act, see S6 and S5-SH1); the comparators of S7 are live on S7-CONTROL, S7-CONTROL-SH1 and S7-CONTROL-V; tg68k: FAIL S7-PC-SH1 S7-PC-SH0 (target NOT GRADED); fx68k: FAIL S7-PC-SH1 S7-PC-SH0 (target NOT GRADED)
// MUTATION S7-PC-SETUP stimulus red NMH_MUTATE=S7-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL S7-PC-SETUP S7-PC-FRAME S7-PC-SH1; fx68k: FAIL S7-PC-SETUP S7-PC-FRAME S7-PC-SH1
// MUTATION S7-PC-FRAME stimulus red NMH_MUTATE=S7-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL S7-PC-FRAME S7-PC-TAG S7-PC-PIXEL S7-PC-SH1; fx68k: FAIL S7-PC-FRAME S7-PC-TAG S7-PC-PIXEL S7-PC-SH1
// MUTATION S7-PC-TAG stimulus red NMH_MUTATE=S7-PC-TAG: the tag block carries no MOVE COLOR00; tg68k: FAIL S7-PC-TAG S7-PC-PIXEL S7-PC-SH1; fx68k: FAIL S7-PC-TAG S7-PC-PIXEL S7-PC-SH1
// MUTATION S7-PC-PIXEL stimulus red NMH_MUTATE=S7-PC-PIXEL: HTOTAL = 0x00E3 with BEAMCON0 = 0x08A0 (228 colour clocks per line); tg68k: FAIL S7-PC-PIXEL S7-PC-SH1 S7-PC-SH0; fx68k: FAIL S7-PC-PIXEL S7-PC-SH1 S7-PC-SH0
// MUTATION S7-PC-STATIC stimulus red NMH_MUTATE=S7-PC-STATIC: R0 contains a SPR0CTL write; tg68k: FAIL S7-PC-STATIC S7-PC-SH1; fx68k: FAIL S7-PC-STATIC S7-PC-SH1
// MUTATION S7-PC-SH1 stimulus red NMH_MUTATE=S7-PC-SH1: R2 writes SPRxCTL = 0x0000; tg68k: FAIL S7-PC-SH1 S7-PC-SH0 (the line prints FAIL in the normal run as well); fx68k: FAIL S7-PC-SH1 S7-PC-SH0 (the line prints FAIL in the normal run as well)
// MUTATION S7-PC-SH0 stimulus red NMH_MUTATE=S7-PC-SH0: R2s writes SPRxCTL = 0x0000; tg68k: FAIL S7-PC-SH1 S7-PC-SH0 (the line prints FAIL in the normal run as well); fx68k: FAIL S7-PC-SH1 S7-PC-SH0 (the line prints FAIL in the normal run as well)
// MUTATION S7-PC-SHRES stimulus red NMH_MUTATE=S7-PC-SHRES: SPRES = 01 in the SHRES runs (140-ns sprites); tg68k: FAIL S7-PC-SH1 S7-PC-SH0 S7-PC-SHRES S7-CONTROL-SH0; fx68k: FAIL S7-PC-SH1 S7-PC-SH0 S7-PC-SHRES S7-CONTROL-SH0
// MUTATION S7-CONTROL stimulus red NMH_MUTATE=S7-CONTROL: R0c writes SPRxPOS + 1 (SH3 = 1, 4 output pixels); tg68k: FAIL S7-PC-SH1 S7-PC-SH0 S7-CONTROL S7-CONTROL-SH1; fx68k: FAIL S7-PC-SH1 S7-PC-SH0 S7-CONTROL S7-CONTROL-SH1
// MUTATION S7-CONTROL-V stimulus red NMH_MUTATE=S7-CONTROL-V: R0sc uses DATA = DATB = 0x5555; tg68k: FAIL S7-PC-SH1 S7-PC-SH0 S7-CONTROL-V; fx68k: FAIL S7-PC-SH1 S7-PC-SH0 S7-CONTROL-V
// MUTATION S7-CONTROL-SH0 stimulus red NMH_MUTATE=S7-CONTROL-SH0: R2sc uses DATA = DATB = 0xAAAA, the data of R1s; tg68k: FAIL S7-PC-SH1 S7-PC-SH0 S7-CONTROL-SH0; fx68k: FAIL S7-PC-SH1 S7-PC-SH0 S7-CONTROL-SH0
// MUTATION S7-CONTROL-SH1 stimulus red NMH_MUTATE=S7-CONTROL: R0c writes SPRxPOS + 1 (SH3 = 1, 4 output pixels); tg68k: FAIL S7-PC-SH1 S7-PC-SH0 S7-CONTROL S7-CONTROL-SH1; fx68k: FAIL S7-PC-SH1 S7-PC-SH0 S7-CONTROL S7-CONTROL-SH1
// MUTATION S8 stimulus red NMH_MUTATE=S8: the k = 2 pair is shifted by 2^(k+1) = 8 lines; S8 grades the k = 2 pair alone in this run; tg68k: FAIL S8; fx68k: FAIL S8
// MUTATION S8-PC-SETUP stimulus red NMH_MUTATE=S8-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL S8-PC-SETUP S8-PC-FRAME S8-PC-SPR S8-PC-LOM; fx68k: FAIL S8-PC-SETUP S8-PC-FRAME S8-PC-SPR S8-PC-LOM
// MUTATION S8-PC-FRAME stimulus red NMH_MUTATE=S8-PC-FRAME: VARBEAMEN is omitted from BEAMCON0 (0x0820) in the k = 9 long-frame pair, so the field is a normal PAL field; tg68k: FAIL S8-PC-FRAME S8-PC-SPR S8-PC-LOM; fx68k: FAIL S8-PC-FRAME S8-PC-SPR S8-PC-LOM
// MUTATION S8-PC-TAG stimulus red NMH_MUTATE=S8-PC-TAG: the tag block carries no MOVE COLOR00; tg68k: FAIL S8-PC-TAG; fx68k: FAIL S8-PC-TAG
// MUTATION S8-PC-SPR stimulus red NMH_MUTATE=S8-PC-SPR: SPREN is clear in the reference run; tg68k: FAIL S8-PC-SPR S8-PC-LOM; fx68k: FAIL S8-PC-SPR S8-PC-LOM
// MUTATION S8-PC-STATIC stimulus red NMH_MUTATE=S8-PC-STATIC: the reference run has an odd start and an even stop; tg68k: FAIL S8-PC-STATIC; fx68k: FAIL S8-PC-STATIC
// MUTATION S8-PC-LOM stimulus red NMH_MUTATE=S8-PC-LOM: the shifted k = 2 run uses HTOTAL = 0x0105 (262 colour clocks per line) with BEAMCON0 = 0x08A0; tg68k: FAIL S8-PC-TAG S8-PC-SPR S8-PC-LOM; fx68k: FAIL S8-PC-TAG S8-PC-SPR S8-PC-LOM
// MUTATION S9 stimulus red NMH_MUTATE=S9: the k = 2 pair is shifted by 2^(k+1) = 8 lines; S9 grades the k = 2 pair alone in this run; tg68k: FAIL S9; fx68k: FAIL S9
// MUTATION S9-PC-SETUP stimulus red NMH_MUTATE=S9-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL S9-PC-SETUP S9-PC-FRAME S9-PC-SPR S9-PC-LOM; fx68k: FAIL S9-PC-SETUP S9-PC-FRAME S9-PC-SPR S9-PC-LOM
// MUTATION S9-PC-FRAME stimulus red NMH_MUTATE=S9-PC-FRAME: VARBEAMEN is omitted from BEAMCON0 (0x0820) in the k = 9 long-frame pair, so the field is a normal PAL field; tg68k: FAIL S9-PC-FRAME S9-PC-SPR S9-PC-LOM; fx68k: FAIL S9-PC-FRAME S9-PC-SPR S9-PC-LOM
// MUTATION S9-PC-TAG stimulus red NMH_MUTATE=S9-PC-TAG: the tag block carries no MOVE COLOR00; tg68k: FAIL S9-PC-TAG; fx68k: FAIL S9-PC-TAG
// MUTATION S9-PC-SPR stimulus red NMH_MUTATE=S9-PC-SPR: SPREN is clear in the reference run; tg68k: FAIL S9-PC-SPR S9-PC-LOM; fx68k: FAIL S9-PC-SPR S9-PC-LOM
// MUTATION S9-PC-STATIC stimulus red NMH_MUTATE=S9-PC-STATIC: the reference run has an odd start and an even stop; tg68k: FAIL S9-PC-STATIC; fx68k: FAIL S9-PC-STATIC
// MUTATION S9-PC-LOM stimulus red NMH_MUTATE=S9-PC-LOM: the shifted k = 2 run uses HTOTAL = 0x0105 (262 colour clocks per line) with BEAMCON0 = 0x08A0; tg68k: FAIL S9-PC-TAG S9-PC-SPR S9-PC-LOM; fx68k: FAIL S9-PC-TAG S9-PC-SPR S9-PC-LOM
// MUTATION S10 stimulus red NMH_MUTATE=S10: run b leaves out the SPR1CTL and SPR5CTL attach bit; tg68k: FAIL S10; fx68k: FAIL S10
// MUTATION S10-PC-SETUP stimulus red NMH_MUTATE=S10-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL S10-PC-SETUP S10-PC-FRAME S10-PC-VISIBLE; fx68k: FAIL S10-PC-SETUP S10-PC-FRAME S10-PC-VISIBLE
// MUTATION S10-PC-FRAME stimulus red NMH_MUTATE=S10-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL S10-PC-FRAME S10-PC-TAG S10-PC-PIXEL S10-PC-VISIBLE; fx68k: FAIL S10-PC-FRAME S10-PC-TAG S10-PC-PIXEL S10-PC-VISIBLE
// MUTATION S10-PC-TAG stimulus red NMH_MUTATE=S10-PC-TAG: the tag block carries no MOVE COLOR00; tg68k: FAIL S10-PC-TAG S10-PC-PIXEL S10-PC-VISIBLE; fx68k: FAIL S10-PC-TAG S10-PC-PIXEL S10-PC-VISIBLE
// MUTATION S10-PC-PIXEL stimulus red NMH_MUTATE=S10-PC-PIXEL: HTOTAL = 0x00E3 with BEAMCON0 = 0x08A0 (228 colour clocks per line); tg68k: FAIL S10-PC-PIXEL; fx68k: FAIL S10-PC-PIXEL
// MUTATION S10-PC-VISIBLE stimulus red NMH_MUTATE=S10-PC-VISIBLE: the even sprites have DATA = 0; tg68k: FAIL S10-PC-VISIBLE; fx68k: FAIL S10-PC-VISIBLE
// MUTATION S11 stimulus red NMH_MUTATE=S11: the SSCAN2 = 1 runs use FMODE = 0; tg68k: FAIL S11; fx68k: FAIL S11
// MUTATION S11-PC-SETUP stimulus red NMH_MUTATE=S11-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL S11-PC-SETUP S11-PC-FRAME S11-PC-BEAM S11-PC-SHOWN S11-PC-SHOWN1; fx68k: FAIL S11-PC-SETUP S11-PC-FRAME S11-PC-BEAM S11-PC-SHOWN S11-PC-SHOWN1
// MUTATION S11-PC-FRAME stimulus red NMH_MUTATE=S11-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL S11-PC-FRAME S11-PC-TAG S11-PC-PIXEL S11-PC-BEAM S11-PC-SHOWN S11-PC-SHOWN1; fx68k: FAIL S11-PC-FRAME S11-PC-TAG S11-PC-PIXEL S11-PC-BEAM S11-PC-SHOWN S11-PC-SHOWN1
// MUTATION S11-PC-TAG stimulus red NMH_MUTATE=S11-PC-TAG: the tag block carries no MOVE COLOR00; tg68k: FAIL S11-PC-TAG S11-PC-PIXEL S11-PC-SHOWN S11-PC-SHOWN1; fx68k: FAIL S11-PC-TAG S11-PC-PIXEL S11-PC-SHOWN S11-PC-SHOWN1
// MUTATION S11-PC-PIXEL stimulus red NMH_MUTATE=S11-PC-PIXEL: HTOTAL = 0x00E3 with BEAMCON0 = 0x08A0 (228 colour clocks per line); tg68k: FAIL S11-PC-PIXEL; fx68k: FAIL S11-PC-PIXEL
// MUTATION S11-PC-BEAM stimulus red NMH_MUTATE=S11-PC-BEAM: the CPU events target H = 0xE0; tg68k: FAIL S11-PC-BEAM; fx68k: FAIL S11-PC-BEAM
// MUTATION S11-PC-SHOWN stimulus red NMH_MUTATE=S11-PC-SHOWN: the SSCAN2 = 0, SH10 = 0 run uses data 0; tg68k: FAIL S11-PC-SHOWN; fx68k: FAIL S11-PC-SHOWN
// MUTATION S11-PC-SHOWN1 stimulus red NMH_MUTATE=S11-PC-SHOWN1: the SSCAN2 = 1 runs use data 0; tg68k: FAIL S11-PC-SHOWN1; fx68k: FAIL S11-PC-SHOWN1
// MUTATION S12 stimulus red NMH_MUTATE=S12: run A has FMODE = 0 and 16-row lists for the SH10 = 1 sprites; tg68k: FAIL S12; fx68k: FAIL S12
// MUTATION S12-PC-SETUP stimulus red NMH_MUTATE=S12-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL S12-PC-SETUP S12-PC-FRAME S12-PC-SPR; fx68k: FAIL S12-PC-SETUP S12-PC-FRAME S12-PC-SPR
// MUTATION S12-PC-FRAME stimulus red NMH_MUTATE=S12-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL S12-PC-FRAME S12-PC-TAG S12-PC-SPR; fx68k: FAIL S12-PC-FRAME S12-PC-TAG S12-PC-SPR
// MUTATION S12-PC-TAG stimulus red NMH_MUTATE=S12-PC-TAG: the tag block carries no MOVE COLOR00; tg68k: FAIL S12-PC-TAG S12-PC-VISIBLE; fx68k: FAIL S12-PC-TAG S12-PC-VISIBLE
// MUTATION S12-PC-SPR stimulus red NMH_MUTATE=S12-PC-SPR: SPREN is clear; tg68k: FAIL S12-PC-SPR; fx68k: FAIL S12-PC-SPR
// MUTATION S12-PC-VISIBLE stimulus red NMH_MUTATE=S12-PC-VISIBLE: every list row has DATA = DATB = 0; tg68k: FAIL S12-PC-VISIBLE; fx68k: FAIL S12-PC-VISIBLE
// MUTATION S12-PC-LOM stimulus red NMH_MUTATE=S12-PC-LOM: the shifted run of A uses HTOTAL = 0x00E3 with BEAMCON0 = 0x08A0 (228 colour clocks per line); tg68k: FAIL S12-PC-LOM; fx68k: FAIL S12-PC-LOM
// MUTATION S13 stimulus red NMH_MUTATE=S13: row 5 of the list holds other words; tg68k: FAIL S13; fx68k: FAIL S13
// MUTATION S13-PC-SETUP stimulus red NMH_MUTATE=S13-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL S13-PC-SETUP S13-PC-FRAME S13-PC-SPR; fx68k: FAIL S13-PC-SETUP S13-PC-FRAME S13-PC-SPR
// MUTATION S13-PC-FRAME stimulus red NMH_MUTATE=S13-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL S13-PC-FRAME S13-PC-TAG S13-PC-PIXEL S13-PC-SPR; fx68k: FAIL S13-PC-FRAME S13-PC-TAG S13-PC-PIXEL S13-PC-SPR
// MUTATION S13-PC-TAG stimulus red NMH_MUTATE=S13-PC-TAG: the tag block carries no MOVE COLOR00; tg68k: FAIL S13-PC-TAG S13-PC-PIXEL; fx68k: FAIL S13-PC-TAG S13-PC-PIXEL
// MUTATION S13-PC-PIXEL stimulus red NMH_MUTATE=S13-PC-PIXEL: HTOTAL = 0x00E3 with BEAMCON0 = 0x08A0 (228 colour clocks per line); tg68k: FAIL S13-PC-PIXEL; fx68k: FAIL S13-PC-PIXEL
// MUTATION S13-PC-SPR stimulus red NMH_MUTATE=S13-PC-SPR: SPREN is clear; tg68k: FAIL S13-PC-SPR; fx68k: FAIL S13-PC-SPR
// MUTATION S14 stimulus red NMH_MUTATE=S14: the Copper event e3 is omitted; part (a) only is run in this mutation; tg68k: FAIL S14; fx68k: FAIL S14
// MUTATION S14 stimulus red NMH_MUTATE=S14-e4: the 68000 writes SPR0CTL = 0x0000 at e4, where the comparator expects the 0x0001 shift; part (a) only is run in this mutation; tg68k: FAIL S14; fx68k: FAIL S14
// MUTATION S14 stimulus red NMH_MUTATE=S14-b: the line-0x80 SPR3POS write is omitted; parts (a) and (b) are run; tg68k: FAIL S14 S14-B; fx68k: FAIL S14 S14-B
// MUTATION S14-B stimulus red NMH_MUTATE=S14-b: the line-0x80 SPR3POS write is omitted; tg68k: FAIL S14 S14-B; fx68k: FAIL S14 S14-B
// MUTATION S14-PC-SETUP stimulus red NMH_MUTATE=S14-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL S14-PC-SETUP S14-PC-FRAME S14-PC-BEAM S14-PC-MOVE S14-PC-SPR; fx68k: FAIL S14-PC-SETUP S14-PC-FRAME S14-PC-BEAM S14-PC-MOVE S14-PC-SPR
// MUTATION S14-PC-FRAME stimulus red NMH_MUTATE=S14-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL S14-PC-FRAME S14-PC-TAG S14-PC-PIXEL S14-PC-BEAM S14-PC-MOVE S14-PC-SPR; fx68k: FAIL S14-PC-FRAME S14-PC-TAG S14-PC-PIXEL S14-PC-BEAM S14-PC-MOVE S14-PC-SPR
// MUTATION S14-PC-TAG stimulus red NMH_MUTATE=S14-PC-TAG: the tag block carries no MOVE COLOR00; tg68k: FAIL S14-PC-TAG S14-PC-PIXEL; fx68k: FAIL S14-PC-TAG S14-PC-PIXEL S14-PC-BEAM
// MUTATION S14-PC-PIXEL stimulus red NMH_MUTATE=S14-PC-PIXEL: HTOTAL = 0x00E3 with BEAMCON0 = 0x08A0 (228 colour clocks per line); tg68k: FAIL S14-PC-PIXEL; fx68k: FAIL S14-PC-PIXEL
// MUTATION S14-PC-BEAM stimulus red NMH_MUTATE=S14-PC-BEAM: the CPU events target H = 0xE0; tg68k: FAIL S14-PC-BEAM; fx68k: FAIL S14-PC-BEAM
// MUTATION S14-PC-MOVE stimulus red NMH_MUTATE=S14-PC-MOVE: a Copper event MOVE carries a data word that differs from the intended one; tg68k: FAIL S14-PC-MOVE; fx68k: FAIL S14-PC-MOVE
// MUTATION S14-PC-SPR stimulus red NMH_MUTATE=S14-PC-SPR: SPREN is clear in (b); tg68k: FAIL S14-PC-SPR; fx68k: FAIL S14-PC-SPR
// MUTATION S15 stimulus impossible NMH_MUTATE=S15: the k = 2 pair is shifted by 2^(k+1) = 8 lines; the stimulus runs, but the line prints NOT GRADED on both cores in every run, because S15-PC-GROUPS and S15-PC-LOM print FAIL (the port shows no access in region U, with AA and with ECS selected); the comparators of those two preconditions are live on S15-CONTROL-GROUPS and S15-CONTROL-LOM; tg68k: FAIL S15-PC-GROUPS S15-PC-LOM (target NOT GRADED); fx68k: FAIL S15-PC-GROUPS S15-PC-LOM (target NOT GRADED)
// MUTATION S15-PC-SETUP stimulus red NMH_MUTATE=S15-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL S15-PC-SETUP S15-PC-FRAME S15-PC-MOVE S15-PC-GROUPS S15-PC-LOM; fx68k: FAIL S15-PC-SETUP S15-PC-FRAME S15-PC-MOVE S15-PC-GROUPS S15-PC-LOM
// MUTATION S15-PC-FRAME stimulus red NMH_MUTATE=S15-PC-FRAME: VARBEAMEN is omitted from BEAMCON0 (0x0820) in the k = 9 long-frame pair, so the field is a normal PAL field; tg68k: FAIL S15-PC-FRAME S15-PC-GROUPS S15-PC-LOM; fx68k: FAIL S15-PC-FRAME S15-PC-GROUPS S15-PC-LOM
// MUTATION S15-PC-TAG stimulus red NMH_MUTATE=S15-PC-TAG: the tag block carries no MOVE COLOR00; tg68k: FAIL S15-PC-TAG S15-PC-GROUPS S15-PC-LOM; fx68k: FAIL S15-PC-TAG S15-PC-GROUPS S15-PC-LOM
// MUTATION S15-PC-MOVE stimulus red NMH_MUTATE=S15-PC-MOVE: the SPRHPTL data word differs from the intended value; tg68k: FAIL S15-PC-MOVE S15-PC-GROUPS S15-PC-LOM; fx68k: FAIL S15-PC-MOVE S15-PC-GROUPS S15-PC-LOM
// MUTATION S15-PC-GROUPS stimulus red NMH_MUTATE=S15-PC-GROUPS: the UHRES bit of BPLCON0 is clear in the reference run; tg68k: FAIL S15-PC-GROUPS S15-PC-LOM (the line prints FAIL in the normal run as well); fx68k: FAIL S15-PC-GROUPS S15-PC-LOM (the line prints FAIL in the normal run as well)
// MUTATION S15-CONTROL-GROUPS stimulus red NMH_MUTATE=S15-CONTROL-GROUPS: BPLEN is clear in the reference run of the pair that runs the bit-plane DMA of P_1; tg68k: FAIL S15-PC-GROUPS S15-PC-LOM S15-CONTROL-GROUPS S15-CONTROL-LOM; fx68k: FAIL S15-PC-GROUPS S15-PC-LOM S15-CONTROL-GROUPS S15-CONTROL-LOM
// MUTATION S15-CONTROL-LOM stimulus red NMH_MUTATE=S15-CONTROL-LOM: the second run of the pair uses HTOTAL = 0x00E3 with BEAMCON0 = 0x08A0 (228 colour clocks per line); tg68k: FAIL S15-PC-GROUPS S15-PC-LOM S15-CONTROL-LOM; fx68k: FAIL S15-PC-GROUPS S15-PC-LOM S15-CONTROL-LOM
// MUTATION S16 stimulus impossible NMH_MUTATE=S16: both runs write SPRHSTOP = 0x0048; the stimulus runs, but the line prints NOT GRADED on both cores in every run, because S16-PC-ACCESS prints FAIL (the port shows no access in region U or U2); its comparator is live on S16-CONTROL-ACCESS; tg68k: FAIL S16-PC-ACCESS (target NOT GRADED); fx68k: FAIL S16-PC-ACCESS (target NOT GRADED)
// MUTATION S16-PC-SETUP stimulus red NMH_MUTATE=S16-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL S16-PC-SETUP S16-PC-FRAME S16-PC-MOVE S16-PC-ACCESS; fx68k: FAIL S16-PC-SETUP S16-PC-FRAME S16-PC-MOVE S16-PC-ACCESS
// MUTATION S16-PC-FRAME stimulus red NMH_MUTATE=S16-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL S16-PC-FRAME S16-PC-TAG S16-PC-MOVE S16-PC-ACCESS; fx68k: FAIL S16-PC-FRAME S16-PC-TAG S16-PC-MOVE S16-PC-ACCESS
// MUTATION S16-PC-TAG stimulus red NMH_MUTATE=S16-PC-TAG: the tag block carries no MOVE COLOR00; tg68k: FAIL S16-PC-TAG S16-PC-ACCESS; fx68k: FAIL S16-PC-TAG S16-PC-ACCESS
// MUTATION S16-PC-MOVE stimulus red NMH_MUTATE=S16-PC-MOVE: the SPRHPTL data word differs from the intended value; tg68k: FAIL S16-PC-MOVE S16-PC-ACCESS; fx68k: FAIL S16-PC-MOVE S16-PC-ACCESS
// MUTATION S16-PC-ACCESS stimulus red NMH_MUTATE=S16-PC-U: the UHRES bit of BPLCON0 is clear; tg68k: FAIL S16-PC-ACCESS (the line prints FAIL in the normal run as well); fx68k: FAIL S16-PC-ACCESS (the line prints FAIL in the normal run as well)
// MUTATION S16-PC-ACCESS stimulus red NMH_MUTATE=S16-PC-U2: BPLHSTRT = BPLHSTOP = 0; tg68k: FAIL S16-PC-ACCESS (the line prints FAIL in the normal run as well); fx68k: FAIL S16-PC-ACCESS (the line prints FAIL in the normal run as well)
// MUTATION S16-CONTROL-ACCESS stimulus red NMH_MUTATE=S16-CONTROL-ACCESS: BPLEN is clear in the run that runs the bit-plane DMA of P_1 and P_5; tg68k: FAIL S16-PC-ACCESS S16-CONTROL-ACCESS; fx68k: FAIL S16-PC-ACCESS S16-CONTROL-ACCESS
// MUTATION C1 stimulus red NMH_MUTATE=C1: the clear run of ENSP1 sets the ENSP1 bit as well; tg68k: FAIL C1; fx68k: FAIL C1
// MUTATION C1-PC-SETUP stimulus red NMH_MUTATE=C1-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL C1-PC-SETUP C1-PC-FRAME C1-PC-SPR C1-PC-BEAM C1-PC-WINDOW C1-PC-UNION; fx68k: FAIL C1-PC-SETUP C1-PC-FRAME C1-PC-SPR C1-PC-BEAM C1-PC-WINDOW C1-PC-UNION
// MUTATION C1-PC-FRAME stimulus red NMH_MUTATE=C1-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL C1-PC-FRAME C1-PC-TAG C1-PC-PIXEL C1-PC-SPR C1-PC-BEAM C1-PC-WINDOW C1-PC-UNION; fx68k: FAIL C1-PC-FRAME C1-PC-TAG C1-PC-PIXEL C1-PC-SPR C1-PC-BEAM C1-PC-WINDOW C1-PC-UNION
// MUTATION C1-PC-TAG stimulus red NMH_MUTATE=C1-PC-TAG: the tag block carries no MOVE COLOR00; tg68k: FAIL C1-PC-TAG C1-PC-PIXEL C1-PC-UNION; fx68k: FAIL C1-PC-TAG C1-PC-PIXEL C1-PC-UNION
// MUTATION C1-PC-PIXEL stimulus red NMH_MUTATE=C1-PC-PIXEL: HTOTAL = 0x00E3 with BEAMCON0 = 0x08A0 (228 colour clocks per line); tg68k: FAIL C1-PC-PIXEL; fx68k: FAIL C1-PC-PIXEL
// MUTATION C1-PC-SPR stimulus red NMH_MUTATE=C1-PC-SPR: SPREN is clear; tg68k: FAIL C1-PC-SPR C1-PC-WINDOW C1-PC-CTRL C1-PC-UNION; fx68k: FAIL C1-PC-SPR C1-PC-WINDOW C1-PC-CTRL C1-PC-UNION
// MUTATION C1-PC-BEAM stimulus red NMH_MUTATE=C1-PC-BEAM: the CF events target H = 0xE0; tg68k: FAIL C1-PC-BEAM; fx68k: FAIL C1-PC-BEAM
// MUTATION C1-PC-WINDOW stimulus red NMH_MUTATE=C1-PC-WINDOW: the result read is at line 0x68; tg68k: FAIL C1-PC-WINDOW C1-PC-CTRL; fx68k: FAIL C1-PC-WINDOW C1-PC-CTRL
// MUTATION C1-PC-CTRL stimulus red NMH_MUTATE=C1-PC-CTRL: sprite 6 is at X = 0xD0; tg68k: FAIL C1-PC-CTRL; fx68k: FAIL C1-PC-CTRL
// MUTATION C1-PC-UNION stimulus red NMH_MUTATE=C1-PC-UNION: the scenario sprite has DATA = 0; tg68k: FAIL C1-PC-UNION; fx68k: FAIL C1-PC-UNION
// MUTATION C2 stimulus red NMH_MUTATE=C2: F3 of the data-1 run of plane 3 writes MVBP3 = 1; C2 grades plane 3 alone in this run; tg68k: FAIL C2; fx68k: FAIL C2
// MUTATION C2-PC-SETUP stimulus red NMH_MUTATE=C2-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL C2-PC-SETUP C2-PC-FRAME C2-PC-SPR C2-PC-WINDOW; fx68k: FAIL C2-PC-SETUP C2-PC-FRAME C2-PC-SPR C2-PC-WINDOW
// MUTATION C2-PC-FRAME stimulus red NMH_MUTATE=C2-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL C2-PC-FRAME C2-PC-SPR C2-PC-WINDOW; fx68k: FAIL C2-PC-FRAME C2-PC-SPR C2-PC-WINDOW
// MUTATION C2-PC-SPR stimulus red NMH_MUTATE=C2-PC-SPR: SPREN is clear; tg68k: FAIL C2-PC-SPR C2-PC-WINDOW C2-PC-OTHER; fx68k: FAIL C2-PC-SPR C2-PC-WINDOW C2-PC-OTHER
// MUTATION C2-PC-WINDOW stimulus red NMH_MUTATE=C2-PC-WINDOW: the result read is at line 0x68; tg68k: FAIL C2-PC-WINDOW; fx68k: FAIL C2-PC-WINDOW
// MUTATION C2-PC-OTHER stimulus red NMH_MUTATE=C2-PC-OTHER: sprite 0 has DATA = 0; tg68k: FAIL C2-PC-OTHER; fx68k: FAIL C2-PC-OTHER
// MUTATION C3 stimulus red NMH_MUTATE=C3: the CLXCON2 write is left out at both addresses; C3 grades plane 7, data 1, at both addresses in this run; tg68k: FAIL C3; fx68k: FAIL C3
// MUTATION C3-PC-SETUP stimulus red NMH_MUTATE=C3-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL C3-PC-SETUP C3-PC-FRAME C3-PC-SPR C3-PC-WINDOW; fx68k: FAIL C3-PC-SETUP C3-PC-FRAME C3-PC-SPR C3-PC-WINDOW
// MUTATION C3-PC-FRAME stimulus red NMH_MUTATE=C3-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL C3-PC-FRAME C3-PC-SPR C3-PC-WINDOW; fx68k: FAIL C3-PC-FRAME C3-PC-SPR C3-PC-WINDOW
// MUTATION C3-PC-SPR stimulus red NMH_MUTATE=C3-PC-SPR: SPREN is clear; tg68k: FAIL C3-PC-SPR C3-PC-WINDOW C3-PC-OTHER; fx68k: FAIL C3-PC-SPR C3-PC-WINDOW C3-PC-OTHER
// MUTATION C3-PC-WINDOW stimulus red NMH_MUTATE=C3-PC-WINDOW: the result read is at line 0x68; tg68k: FAIL C3-PC-WINDOW; fx68k: FAIL C3-PC-WINDOW
// MUTATION C3-PC-OTHER stimulus red NMH_MUTATE=C3-PC-OTHER: sprite 0 has DATA = 0; tg68k: FAIL C3-PC-OTHER; fx68k: FAIL C3-PC-OTHER
// MUTATION C4 stimulus red NMH_MUTATE=C4: the CLXCON writes of F3 and F5 are replaced by a COLOR00 write; tg68k: FAIL C4; fx68k: FAIL C4
// MUTATION C4-PC-SETUP stimulus red NMH_MUTATE=C4-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL C4-PC-SETUP C4-PC-FRAME C4-PC-SPR C4-PC-BEAM C4-PC-WINDOW C4-PC-EFFECT; fx68k: FAIL C4-PC-SETUP C4-PC-FRAME C4-PC-SPR C4-PC-BEAM C4-PC-WINDOW C4-PC-EFFECT
// MUTATION C4-PC-FRAME stimulus red NMH_MUTATE=C4-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL C4-PC-FRAME C4-PC-SPR C4-PC-BEAM C4-PC-WINDOW C4-PC-EFFECT; fx68k: FAIL C4-PC-FRAME C4-PC-SPR C4-PC-BEAM C4-PC-WINDOW C4-PC-EFFECT
// MUTATION C4-PC-EFFECT stimulus red NMH_MUTATE=C4-PC-EFFECT: the CLXCON2 write of F2 is left out; tg68k: FAIL C4-PC-EFFECT; fx68k: FAIL C4-PC-BEAM C4-PC-EFFECT
// MUTATION C4-PC-SPR stimulus red NMH_MUTATE=C4-PC-SPR: SPREN is clear; tg68k: FAIL C4-PC-SPR C4-PC-WINDOW; fx68k: FAIL C4-PC-SPR C4-PC-BEAM C4-PC-WINDOW
// MUTATION C4-PC-BEAM stimulus red NMH_MUTATE=C4-PC-BEAM: the CF events target H = 0xE0; tg68k: FAIL C4-PC-BEAM; fx68k: FAIL C4-PC-BEAM
// MUTATION C4-PC-WINDOW stimulus red NMH_MUTATE=C4-PC-WINDOW: the result read is at line 0x68; tg68k: FAIL C4-PC-WINDOW; fx68k: FAIL C4-PC-WINDOW
// MUTATION C5 none impossible NMH_MUTATE=C5: unfalsifiable at this boundary: a reset value can only be changed by a write, and the write is what C5-PC-STATIC excludes; with run A and run B writing CLXCON2 (0x00C0, 0x00C3) at an address where it is in effect, C5-PC-STATIC prints FAIL and the line prints NOT GRADED on both cores; the comparator of C5 is live on C5-CONTROL, which reads the same CLXDAT bits with CLXCON2 written; tg68k: FAIL C5-PC-STATIC (target NOT GRADED); fx68k: FAIL C5-PC-STATIC (target NOT GRADED)
// MUTATION C5-CONTROL stimulus red NMH_MUTATE=C5-CONTROL: the contrast runs write CLXCON2 = 0x0000; tg68k: FAIL C5-CONTROL; fx68k: FAIL C5-CONTROL
// MUTATION C5-PC-SETUP stimulus red NMH_MUTATE=C5-PC-SETUP: the program variant without the setup-marker store runs; tg68k: FAIL C5-PC-SETUP C5-PC-FRAME C5-PC-SPR C5-PC-WINDOW; fx68k: FAIL C5-PC-SETUP C5-PC-FRAME C5-PC-SPR C5-PC-WINDOW
// MUTATION C5-PC-FRAME stimulus red NMH_MUTATE=C5-PC-FRAME: COPEN is not set (no fetch at COP1LC); tg68k: FAIL C5-PC-FRAME C5-PC-SPR C5-PC-WINDOW; fx68k: FAIL C5-PC-FRAME C5-PC-SPR C5-PC-WINDOW
// MUTATION C5-PC-STATIC stimulus red NMH_MUTATE=C5-PC-STATIC: the program writes CLXCON; tg68k: FAIL C5-PC-STATIC; fx68k: FAIL C5-PC-STATIC
// MUTATION C5-PC-SPR stimulus red NMH_MUTATE=C5-PC-SPR: SPREN is clear; tg68k: FAIL C5-PC-SPR C5-PC-WINDOW; fx68k: FAIL C5-PC-SPR C5-PC-WINDOW
// MUTATION C5-PC-WINDOW stimulus red NMH_MUTATE=C5-PC-WINDOW: the result read is at line 0x68; tg68k: FAIL C5-PC-WINDOW; fx68k: FAIL C5-PC-WINDOW
// MUTATION S15-PC-LOM none impossible NMH_MUTATE=-: already red at baseline, no stimulus can reach it: the line prints FAIL in every run because the port shows no access in region U (S15-PC-GROUPS prints FAIL), so no stimulus change of S15 reaches a group whose line offset could be measured; its comparator is live on S15-CONTROL-LOM, which runs it on the region of P_1 that bit-plane DMA reaches
// MUTATION J1 none impossible NMH_MUTATE=-: not exercised, by a scope decision: J1 adds no stimulus (SPECIFICATION.md section 3) and is the conjunction of the verdicts of S5-SH10-SH3 and S8; the conjunction is shown live in both directions by J2 and J3 (FAIL through S5-SH1 and S6) and J-CONTROL (PASS), and the contrasting cases of the parts are the records of S5-SH10-SH3 and S8.
// MUTATION J2 none impossible NMH_MUTATE=-: not exercised, by a scope decision: J2 adds no stimulus and is the conjunction of the verdicts of S5, S5-SH1 and S6; S5-SH1 and S6 print FAIL in the normal run on both cores; see J1 for the comparator.
// MUTATION J3 none impossible NMH_MUTATE=-: not exercised, by a scope decision: J3 adds no stimulus and is the conjunction of the verdicts of S5, S5-SH1, S6, S8, S9, S10 and S12; S5-SH1 and S6 print FAIL in the normal run on both cores; see J1 for the comparator.
// MUTATION J4 none impossible NMH_MUTATE=-: not exercised, by a scope decision: J4 adds no stimulus and is the conjunction of the verdicts of S1 and S14-B, both PASS in the normal run on both cores; see J1 for the comparator.
// MUTATION J-CONTROL none impossible NMH_MUTATE=-: not exercised, by a scope decision: J-CONTROL is the conjunction of the verdicts of S1-S4, all PASS on both cores; the contrasting cases of its parts are the records of S1, S2, S3 and S4, and J1-J4 show the same conjunction printing FAIL.
// MUTATION_RECORDS_END

#include <cstdio>
#include <cstring>
#include <iostream>

#include "s3_props_j.h"
#include "s3_props_b.h"
#include "s3_props_b34.h"
#include "s3_props_s12.h"
#include "s3_props_s34.h"
#include "s3_props_s567.h"
#include "s3_props_s89.h"
#include "s3_props_s1011.h"
#include "s3_props_s121314.h"
#include "s3_props_s1516.h"
#include "s3_props_c.h"

using namespace s3;

int main(int argc, char** argv) {
    g_dir = argc > 1 ? argv[1] : ".";
    g_mutate = std::getenv("NMH_MUTATE");
    std::string core = NMH_XSTR(NANOMIG_CORE_NAME);
    std::printf("CORE: %s\n", core.c_str());
    if (g_mutate) std::printf("NMH_MUTATE=%s\n", g_mutate);

    fixture();
    prop_B1();
    prop_B2();
    prop_B3();
    prop_B4();
    prop_S1();
    prop_S2();
    prop_S3();
    prop_S4();
    prop_S5();
    prop_S6();
    prop_S7();
    prop_S8();
    prop_S9();
    prop_S10();
    prop_S11();
    prop_S12();
    prop_S13();
    prop_S14();
    prop_S15();
    prop_S16();
    prop_C1();
    prop_C2();
    prop_C3();
    prop_C4();
    prop_C5();
    joint_assertions();

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
