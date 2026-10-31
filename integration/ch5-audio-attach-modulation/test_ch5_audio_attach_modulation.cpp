// test_ch5_audio_attach_modulation.cpp -- ch5-audio-attach-modulation. See
// SPECIFICATION.md for every assertion's proposition, program, observed
// value and expected value. This file only implements what
// SPECIFICATION.md already committed to.
//
// Each channel's output side is not assumed from either page: it is
// measured, once per channel, from that channel's own reference session
// (the one output that changes while the other stays constant -- see
// measure_channel_side() and side_of[]). PC5 grades only the pairing the
// two pages agree on (0 with 3 on one output, 1 with 2 on the other, the
// two outputs different); every other assertion below uses side_of[] and
// depends on neither page's naming of which physical jack that is.
//
// A label starts with its assertion id, and the id is the token the
// // MUTATION records below name. NMH_MUTATE selects a mutated program (or a
// substituted capture of another session) in place of the ordinary one: the
// line of that id then prints its verdict through the unchanged comparator. A selector
// changes the program (or the captured session) and nothing else: no expected value,
// window, observed channel, population or gate depends on it. The contrast of a
// precondition replaces the capture of the line it grades and the lines gated on it
// print NOT GRADED. None is exercised by a plain `make test` and none is counted in its
// summary. "PM" below is the private copy of the audio RTL built by
// `make pm-test-fx68k` / `make pm-test-tg68k` (page_model.patch), which implements the
// attach bits as pages 149-151 and 164-165 describe them; a run against it is a positive
// control and is not a mutation record. NMH_CAPCACHE=<dir> stores each session's capture
// on disk and reads it back (one directory per build); it changes no verdict.
// Each record names the line (or lines, one record each) that the selector moves and
// gives "before -> after" per core on the repository RTL and on PM; "#n" is the n-th line
// printed with that id.
//
// MUTATION FIXTURE-STORE stimulus red NMH_MUTATE=FIXTURE-STORE: the program stores $DEAD in place of $BEEF (prog_fixture_mut); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION FIXTURE-ACTIVITY stimulus red NMH_MUTATE=FIXTURE-ACTIVITY: AUD0EN is left out of the enable, so channel 0 never plays (prog_fixture_noplay); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION PC1 stimulus red NMH_MUTATE=PC1: the step-4 ADKCON write is omitted in S-AV (prog_s_av_noattach); the line keeps expecting the session's $01 mask; RTL/fx68k PASS -> FAIL #13; RTL/TG68K PASS -> FAIL #13; PM/fx68k PASS -> FAIL #13; PM/TG68K PASS -> FAIL #13
// MUTATION PC2 stimulus red NMH_MUTATE=PC2: R-MOD with the enable $8001: AUD0EN without DMAEN (prog_rmod_nodma) replaces the capture of the R-MOD line; RTL/fx68k PASS -> FAIL #1; RTL/TG68K PASS -> FAIL #1; PM/fx68k PASS -> FAIL #1; PM/TG68K PASS -> FAIL #1
// MUTATION PC3 stimulus red NMH_MUTATE=PC3: R-MOD with AUD0LEN = 7 (prog_rmod_len7) replaces the capture of the R-MOD line, graded with LEN 8; RTL/fx68k PASS -> FAIL #1; RTL/TG68K PASS -> FAIL #1; PM/fx68k PASS -> FAIL #1; PM/TG68K PASS -> FAIL #1
// MUTATION PC4-VOL stimulus red NMH_MUTATE=PC4-VOL: R-VOL-1 writes 40 in the step labelled 60 (prog_rvol1_mut); RTL/fx68k PASS -> FAIL #1; RTL/TG68K PASS -> FAIL #1; PM/fx68k PASS -> FAIL #1; PM/TG68K PASS -> FAIL #1
// MUTATION PC4-DUR stimulus red NMH_MUTATE=PC4-DUR: R-PER-1 writes 500 in the step labelled 700 (prog_rper1_mut); RTL/fx68k PASS -> FAIL #1; RTL/TG68K PASS -> FAIL #1; PM/fx68k PASS -> FAIL #1; PM/TG68K PASS -> FAIL #1
// MUTATION PC5-VOL stimulus red NMH_MUTATE=PC5-VOL: R-VOL-1's capture is graded as R-VOL-3's, so channel 3's output is channel 1's; RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION PC5-PER stimulus red NMH_MUTATE=PC5-PER: R-VOL-3's capture is graded as R-PER-1's, so channel 1's output is channel 3's; RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION PC6 stimulus red NMH_MUTATE=PC6: AUD0PER = 5000 is written halfway through R-MOD (prog_rmod_pc6); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION PC7-CAL stimulus red NMH_MUTATE=PC7-CAL: R-MOD with AUD0PER = 6000 from the start (prog_rmod_per6000), graded with R-MOD's D_mod of 4000 ticks; RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION PC7-FRAME stimulus red NMH_MUTATE=PC7-FRAME: M_on is written late, after a delay following the DMACON enable (prog_rmod_pc7frame), so a fetch occurs before it; RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION PC7-LOOP stimulus red NMH_MUTATE=PC7-LOOP: AUD0PER = 3000 is written halfway through R-MOD (prog_rmod_pc7); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION PC7-N3 stimulus red NMH_MUTATE=PC7-N3: S-AP's capture is graded through the same comparator; RTL/fx68k PASS -> PASS; RTL/TG68K PASS -> PASS; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P1-WINDOW stimulus red NMH_MUTATE=P1-WINDOW: channel 1's DMA is stopped after one eighth of the session (prog_s_av_stop): channel 1's output ends before channel 0 restarts; RTL/fx68k PASS -> PRECONDITION NOT MET; RTL/TG68K PASS -> PRECONDITION NOT MET; PM/fx68k PASS -> PRECONDITION NOT MET; PM/TG68K PASS -> PRECONDITION NOT MET
// MUTATION P1-A stimulus red NMH_MUTATE=P1-WINDOW: channel 1's DMA is stopped after one eighth of the session (prog_s_av_stop): channel 1's output ends before channel 0 restarts; RTL/fx68k FAIL -> NOT GRADED; RTL/TG68K FAIL -> NOT GRADED; PM/fx68k PASS -> NOT GRADED; PM/TG68K PASS -> NOT GRADED
// MUTATION P1-A stimulus red NMH_MUTATE=P1-A: the second and third modulator words are exchanged (prog_s_av_swap); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P1-A stimulus red NMH_MUTATE=P1-VOL1WRITE: channel 1's own AUDxVOL is rewritten from 40 to 24 halfway through (prog_s_av_vol1write): 24 has a reference level pair and is no word of M_V; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P1-JOINT stimulus red NMH_MUTATE=P1-A: the second and third modulator words are exchanged (prog_s_av_swap); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-JOINT stimulus red NMH_MUTATE=P1-A: the second and third modulator words are exchanged (prog_s_av_swap); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P1-C stimulus red NMH_MUTATE=P1-PER1WRITE: channel 1's own AUDxPER is rewritten from 200 to 300 halfway through (prog_s_av_per1write); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P1-JOINT stimulus red NMH_MUTATE=P1-JOINT: ch3 also plays, audibly, on the output channel 0 is observed on (prog_s_av_ch3): the source half; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P1-JOINT stimulus red NMH_MUTATE=P1-VOL1WRITE: channel 1's own AUDxVOL is rewritten from 40 to 24 halfway through (prog_s_av_vol1write): 24 has a reference level pair and is no word of M_V; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P2-A stimulus red NMH_MUTATE=P2-A: the eighth and ninth modulator words are exchanged (prog_s_p2_swap); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P2-A stimulus red NMH_MUTATE=P2-B: channel 1's own AUD1PER is rewritten to 700 halfway through (prog_s_p2_per1write): 700 is R-RAMP's settle period and is no word of M_P; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P2-JOINT stimulus red NMH_MUTATE=P2-A: the eighth and ninth modulator words are exchanged (prog_s_p2_swap); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-JOINT stimulus red NMH_MUTATE=P2-A: the eighth and ninth modulator words are exchanged (prog_s_p2_swap); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P2-C stimulus red NMH_MUTATE=P2-C: the second and third words of channel 1's own data (RAMP) are exchanged (prog_s_p2_ramp); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P2-JOINT stimulus red NMH_MUTATE=P2-C: the second and third words of channel 1's own data (RAMP) are exchanged (prog_s_p2_ramp); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P2-JOINT stimulus red NMH_MUTATE=P2-B: channel 1's own AUD1PER is rewritten to 700 halfway through (prog_s_p2_per1write): 700 is R-RAMP's settle period and is no word of M_P; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P2-B stimulus red NMH_MUTATE=P2-B: channel 1's own AUD1PER is rewritten to 700 halfway through (prog_s_p2_per1write): 700 is R-RAMP's settle period and is no word of M_P; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P2-B stimulus red NMH_MUTATE=PC7-CAL: R-MOD with AUD0PER = 6000 from the start (prog_rmod_per6000), graded with R-MOD's D_mod of 4000 ticks: the lines gated on PC7-CAL; RTL/fx68k FAIL -> NOT GRADED; RTL/TG68K FAIL -> NOT GRADED; PM/fx68k PASS -> NOT GRADED; PM/TG68K PASS -> NOT GRADED
// MUTATION P3-PRE stimulus red NMH_MUTATE=P3-PRE: R-MOD with AUD0VOL = 0 (prog_rmod_vol0) plays no byte; RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P3-SILENT stimulus red NMH_MUTATE=P3-SILENT: every modulator word is 0 (prog_s_av_zero); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P3-WHOLE stimulus red NMH_MUTATE=P3-WHOLE: channel 1 plays the first four words of M_V as its own data (prog_s_av_ch1bytes); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P3-JOINT stimulus red NMH_MUTATE=P3-WHOLE: channel 1 plays the first four words of M_V as its own data (prog_s_av_ch1bytes); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P3-SP2 stimulus red NMH_MUTATE=P3-SP2: the modulator word 144 is 129 (prog_s_p2_w129); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P3-JOINT stimulus red NMH_MUTATE=P3-SP2: the modulator word 144 is 129 (prog_s_p2_w129); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P4-VOLONLY stimulus red NMH_MUTATE=P4-VOLONLY: channel 1's own AUDxPER is rewritten from 200 to 300 halfway through S-BOTH-volonly (prog_s_both_volonly_per); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P4-JOINT stimulus red NMH_MUTATE=P4-VOLONLY: channel 1's own AUDxPER is rewritten from 200 to 300 halfway through S-BOTH-volonly (prog_s_both_volonly_per); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P4-A stimulus red NMH_MUTATE=P4-A: the volume words 56 and 8 are exchanged (prog_s_both_swap); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P4-JOINT stimulus red NMH_MUTATE=P4-A: the volume words 56 and 8 are exchanged (prog_s_both_swap); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P4-B stimulus red NMH_MUTATE=P4-B: the last volume word is 32 instead of 40 (prog_s_both_v32); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P4-A stimulus red NMH_MUTATE=P4-B: the last volume word is 32 instead of 40 (prog_s_both_v32); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P4-C stimulus red NMH_MUTATE=P4-C: every period word is 300 (prog_s_both_perconst); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P4-A stimulus red NMH_MUTATE=P4-C: every period word is 300 (prog_s_both_perconst); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P5-AV stimulus red NMH_MUTATE=P5-AV: AUD0PER = 6000 (prog_s_av_per6000), graded with R-MOD's D_mod; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P5-AV stimulus red NMH_MUTATE=PC7-CAL: R-MOD with AUD0PER = 6000 from the start (prog_rmod_per6000), graded with R-MOD's D_mod of 4000 ticks: the lines gated on PC7-CAL; RTL/fx68k FAIL -> NOT GRADED; RTL/TG68K FAIL -> NOT GRADED; PM/fx68k PASS -> NOT GRADED; PM/TG68K PASS -> NOT GRADED
// MUTATION P5-AP stimulus red NMH_MUTATE=P5-AP: AUD0PER = 6000 (prog_s_ap_per6000), graded with R-MOD's D_mod; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P5-AP stimulus red NMH_MUTATE=P5-AP-GRID: AUD0PER = 5000 at the start, rewritten to 4000 about 1000 ticks after M_on (prog_s_ap_shift): every time-out lies 1000 ticks off the grid of R-MOD, the spacing is unchanged; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P5-AP stimulus red NMH_MUTATE=PC7-CAL: R-MOD with AUD0PER = 6000 from the start (prog_rmod_per6000), graded with R-MOD's D_mod of 4000 ticks: the lines gated on PC7-CAL; RTL/fx68k FAIL -> NOT GRADED; RTL/TG68K FAIL -> NOT GRADED; PM/fx68k PASS -> NOT GRADED; PM/TG68K PASS -> NOT GRADED
// MUTATION P5-BOTH stimulus red NMH_MUTATE=P5-BOTH: AUD0PER = 6000 (prog_s_both_per6000), graded with R-MOD's D_mod; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P5-BOTH stimulus red NMH_MUTATE=P5-BOTH-GRID: AUD0PER = 5000 at the start, rewritten to 4000 about 1000 ticks after M_on (prog_s_both_shift): every time-out lies 1000 ticks off the grid of R-MOD, the spacing is unchanged; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P5-BOTH stimulus red NMH_MUTATE=P5-BOTH-ALT: every period-position word of M_VP is 200, the period channel 1 already holds (prog_s_both_volwords): only the volume words change anything, the window holds no period change, and the volume changes lie 2*D_mod apart, so the D_mod spacing clause fails with the population and alternation clause (not isolated); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P5-BOTH stimulus red NMH_MUTATE=PC7-CAL: R-MOD with AUD0PER = 6000 from the start (prog_rmod_per6000), graded with R-MOD's D_mod of 4000 ticks: the lines gated on PC7-CAL; RTL/fx68k FAIL -> NOT GRADED; RTL/TG68K FAIL -> NOT GRADED; PM/fx68k PASS -> NOT GRADED; PM/TG68K PASS -> NOT GRADED
// MUTATION P6-A stimulus red NMH_MUTATE=P6-0: bit 0, the second and third modulator words are exchanged (prog_s_pair0_swap); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL #1; PM/TG68K PASS -> FAIL #1
// MUTATION P6-A stimulus red NMH_MUTATE=P6-1: bit 1, the second and third modulator words are exchanged (prog_s_pair1_swap); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL #2; PM/TG68K PASS -> FAIL #2
// MUTATION P6-JOINT stimulus red NMH_MUTATE=P6-1: bit 1, the second and third modulator words are exchanged (prog_s_pair1_swap); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-A stimulus red NMH_MUTATE=P6-2: bit 2, the second and third modulator words are exchanged (prog_s_pair2_swap); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL #3; PM/TG68K PASS -> FAIL #3
// MUTATION P6-JOINT stimulus red NMH_MUTATE=P6-2: bit 2, the second and third modulator words are exchanged (prog_s_pair2_swap); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-A stimulus red NMH_MUTATE=P6-4: bit 4, the second and third modulator words are exchanged (prog_s_pair4_swap); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL #4; PM/TG68K PASS -> FAIL #4
// MUTATION P6-A stimulus red NMH_MUTATE=P6-5: bit 5, the second and third modulator words are exchanged (prog_s_pair5_swap); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL #5; PM/TG68K PASS -> FAIL #5
// MUTATION P6-JOINT stimulus red NMH_MUTATE=P6-5: bit 5, the second and third modulator words are exchanged (prog_s_pair5_swap); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-A stimulus red NMH_MUTATE=P6-6: bit 6, the second and third modulator words are exchanged (prog_s_pair6_swap); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL #6; PM/TG68K PASS -> FAIL #6
// MUTATION P6-JOINT stimulus red NMH_MUTATE=P6-6: bit 6, the second and third modulator words are exchanged (prog_s_pair6_swap); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-JOINT stimulus red NMH_MUTATE=P1-VOL1WRITE: channel 1's own AUDxVOL is rewritten from 40 to 24 halfway through (prog_s_av_vol1write): 24 has a reference level pair and is no word of M_V; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-JOINT stimulus red NMH_MUTATE=P2-B: channel 1's own AUD1PER is rewritten to 700 halfway through (prog_s_p2_per1write): 700 is R-RAMP's settle period and is no word of M_P; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-B stimulus red NMH_MUTATE=P6-TGT-0: bit 0: the target's own AUDxPER is rewritten from 200 to 300 halfway through (prog_s_pair0_tgtper); RTL/fx68k PASS -> FAIL #1; RTL/TG68K PASS -> FAIL #1; PM/fx68k PASS -> FAIL #1; PM/TG68K PASS -> FAIL #1
// MUTATION P6-B stimulus red NMH_MUTATE=P6-TGT-4: bit 4: the target's own AUDxVOL is rewritten from 40 to 20 halfway through (prog_s_pair4_tgtvol); RTL/fx68k PASS -> FAIL #4; RTL/TG68K PASS -> FAIL #4; PM/fx68k PASS -> FAIL #4; PM/TG68K PASS -> FAIL #4
// MUTATION P6-C stimulus red NMH_MUTATE=P6-0-ATTC: bit 0 with bit 2 added: ch2 routes its words to the volume of ch3, a tripwire ($8005, prog_s_pair0_attc); RTL/fx68k PASS -> PASS; RTL/TG68K PASS -> PASS; PM/fx68k PASS -> FAIL #1; PM/TG68K PASS -> FAIL #1
// MUTATION P6-C stimulus red NMH_MUTATE=P6-1-ATTC: bit 1 with bit 2 added: ch2, the target, routes its words to the volume of ch3, a tripwire ($8006, prog_s_pair1_attc); RTL/fx68k PASS -> PASS; RTL/TG68K PASS -> PASS; PM/fx68k PASS -> FAIL #2; PM/TG68K PASS -> FAIL #2
// MUTATION P6-C stimulus red NMH_MUTATE=P6-2-ATTC: bit 2 with bit 0 added: ch0 routes its words to the volume of ch1, a tripwire ($8005, prog_s_pair2_attc); RTL/fx68k PASS -> PASS; RTL/TG68K PASS -> PASS; PM/fx68k PASS -> FAIL #3; PM/TG68K PASS -> FAIL #3
// MUTATION P6-C stimulus red NMH_MUTATE=P6-4-ATTC: bit 4 with bit 2 added: ch2 routes its words to the volume of ch3, a tripwire ($8014, prog_s_pair4_attc); RTL/fx68k PASS -> PASS; RTL/TG68K PASS -> PASS; PM/fx68k PASS -> FAIL #4; PM/TG68K PASS -> FAIL #4
// MUTATION P6-C stimulus red NMH_MUTATE=P6-5-ATTC: bit 5 with bit 2 added: ch2, the target, routes its words to the volume of ch3, a tripwire ($8024, prog_s_pair5_attc); RTL/fx68k PASS -> PASS; RTL/TG68K PASS -> PASS; PM/fx68k PASS -> FAIL #5; PM/TG68K PASS -> FAIL #5
// MUTATION P6-C stimulus red NMH_MUTATE=P6-6-ATTC: bit 6 with bit 0 added: ch0 routes its words to the volume of ch1, a tripwire ($8041, prog_s_pair6_attc); RTL/fx68k PASS -> PASS; RTL/TG68K PASS -> PASS; PM/fx68k PASS -> FAIL #6; PM/TG68K PASS -> FAIL #6
// MUTATION P6-D stimulus red NMH_MUTATE=P6-4-ATTD: bit 4 with bit 6 added: ch2 routes its words to the period of ch3, a tripwire ($8050, prog_s_pair4_attd); RTL/fx68k PASS -> PASS; RTL/TG68K PASS -> PASS; PM/fx68k PASS -> FAIL #4; PM/TG68K PASS -> FAIL #4
// MUTATION P6-D stimulus red NMH_MUTATE=P6-5-ATTD: bit 5 with bit 4 added: ch0 routes its words to the period of ch1, the modulator ($8030, prog_s_pair5_attd); RTL/fx68k PASS -> PASS; RTL/TG68K PASS -> PASS; PM/fx68k PASS -> FAIL #5; PM/TG68K PASS -> FAIL #5
// MUTATION P6-D stimulus red NMH_MUTATE=P6-6-ATTD: bit 6 with bit 4 added: ch0 routes its words to the period of ch1, a tripwire ($8050, prog_s_pair6_attd); RTL/fx68k PASS -> PASS; RTL/TG68K PASS -> PASS; PM/fx68k PASS -> FAIL #6; PM/TG68K PASS -> FAIL #6
// MUTATION P7-CONST stimulus red NMH_MUTATE=P7V: S-CH3-V with bit 2 in place of bit 3: ch2 routes its words to the volume of ch3 ($8004, prog_s_ch3v_att); RTL/fx68k PASS -> PASS; RTL/TG68K PASS -> PASS; PM/fx68k PASS -> FAIL #1; PM/TG68K PASS -> FAIL #1
// MUTATION P7-JOINT-V stimulus red NMH_MUTATE=P7V: S-CH3-V with bit 2 in place of bit 3: ch2 routes its words to the volume of ch3 ($8004, prog_s_ch3v_att); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P7-SPACING stimulus red NMH_MUTATE=P7P: S-CH3-P with bit 6 in place of bit 7: ch2 routes its words to the period of ch3 ($8040, prog_s_ch3p_att); RTL/fx68k PASS -> PASS; RTL/TG68K PASS -> PASS; PM/fx68k PASS -> FAIL #2; PM/TG68K PASS -> FAIL #2
// MUTATION P7-JOINT-P stimulus red NMH_MUTATE=P7P: S-CH3-P with bit 6 in place of bit 7: ch2 routes its words to the period of ch3 ($8040, prog_s_ch3p_att); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P8-ATT stimulus red NMH_MUTATE=P8-OMIT: the attach write is omitted: the matching unattached session takes the attached one's place; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL #1; PASS -> FAIL #2; PASS -> FAIL #3; PASS -> FAIL #4; PASS -> FAIL #5; PM/TG68K PASS -> FAIL #1; PASS -> FAIL #2; PASS -> FAIL #3; PASS -> FAIL #4; PASS -> FAIL #5
// MUTATION P8-JOINT-AP stimulus red NMH_MUTATE=P8-OMIT: the attach write is omitted: the matching unattached session takes the attached one's place; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P7-JOINT-V stimulus red NMH_MUTATE=P8-OMIT: the attach write is omitted: the matching unattached session takes the attached one's place; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P7-JOINT-P stimulus red NMH_MUTATE=P8-OMIT: the attach write is omitted: the matching unattached session takes the attached one's place; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P8-PRE stimulus red NMH_MUTATE=P8-PRE: R-MOD with AUD0VOL = 0 (prog_rmod_vol0); RTL/fx68k PASS -> FAIL #1; RTL/TG68K PASS -> FAIL #1; PM/fx68k PASS -> FAIL #1; PM/TG68K PASS -> FAIL #1
// MUTATION P8-JOINT-AP stimulus red NMH_MUTATE=P8-JOINT-AP: the second and third words of M_P4 are exchanged in S-AP (prog_s_ap_swap): the destination half; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P9-WINDOW stimulus red NMH_MUTATE=P9-WINDOW: AUD1LEN = 1000 in S-LEN-A (prog_s_lena_ch1len1000): channel 1 does not restart; RTL/fx68k PASS -> PRECONDITION NOT MET #1; RTL/TG68K PASS -> PRECONDITION NOT MET #1; PM/fx68k PASS -> PRECONDITION NOT MET #1; PM/TG68K PASS -> PRECONDITION NOT MET #1
// MUTATION P9-A stimulus red NMH_MUTATE=P9-WINDOW: AUD1LEN = 1000 in S-LEN-A (prog_s_lena_ch1len1000): channel 1 does not restart; RTL/fx68k PASS -> NOT GRADED #1; RTL/TG68K PASS -> NOT GRADED #1; PM/fx68k PASS -> NOT GRADED #1; PM/TG68K PASS -> NOT GRADED #1
// MUTATION P9-C stimulus red NMH_MUTATE=P9-WINDOW: AUD1LEN = 1000 in S-LEN-A (prog_s_lena_ch1len1000): channel 1 does not restart; RTL/fx68k FAIL -> NOT GRADED #1; RTL/TG68K FAIL -> NOT GRADED #1; PM/fx68k PASS -> NOT GRADED #1; PM/TG68K PASS -> NOT GRADED #1
// MUTATION P9-A stimulus red NMH_MUTATE=P9-A: AUD0LEN = 10 over a table that holds M_LA twice, 260000 ticks (prog_s_lena_len10), graded with L_m = 5; RTL/fx68k PASS -> FAIL #1; RTL/TG68K PASS -> FAIL #1; PM/fx68k PASS -> FAIL #1; PM/TG68K PASS -> FAIL #1
// MUTATION P9-B stimulus red NMH_MUTATE=P9-B: AUD1LEN = 2 in S-LEN-A (prog_s_lena_ch1len2), graded with L_c = 3; RTL/fx68k PASS -> FAIL #1; RTL/TG68K PASS -> FAIL #1; PM/fx68k PASS -> FAIL #1; PM/TG68K PASS -> FAIL #1
// MUTATION P9-C stimulus red NMH_MUTATE=P9-C: the third and fourth words of M_LA are exchanged (prog_s_lena_swap); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL #1; PM/TG68K PASS -> FAIL #1
// MUTATION P9-D stimulus red NMH_MUTATE=P9-D: channel 1's own AUDxPER is rewritten from 200 to 300 halfway through S-LEN-A (prog_s_lena_per1write); RTL/fx68k PASS -> FAIL #1; RTL/TG68K PASS -> FAIL #1; PM/fx68k PASS -> FAIL #1; PM/TG68K PASS -> FAIL #1
// MUTATION P10-AV stimulus red NMH_MUTATE=P10-AV: AUD0PER = 6000 (prog_s_av_per6000), graded with R-MOD's D_mod; RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P10-AV stimulus red NMH_MUTATE=P10-AV-PHASE: S-AP's capture in place of S-AV's: the phase alone; RTL/fx68k PASS -> PASS; RTL/TG68K PASS -> PASS; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P10-AP stimulus red NMH_MUTATE=P10-AP: S-AV's capture in place of S-AP's; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P10-AP stimulus red NMH_MUTATE=P10-AP-GRID: AUD0PER = 5000 at the start, rewritten to 4000 about 1000 ticks after M_on (prog_s_ap_shift): the requests lie 1000 ticks off the grid of R-MOD, 2*D_mod apart; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P10-AP stimulus red NMH_MUTATE=P10-AP-RATE: AUD0PER rewritten to 12000 about 1000 ticks after M_on and to 4000 about 4500 ticks later (prog_s_ap_pulse): one half-loop lasts 12000 ticks, the later requests come 2*D_mod late; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P10-BOTH stimulus red NMH_MUTATE=P10-BOTH: S-AV's capture in place of S-BOTH's; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P10-BOTH stimulus red NMH_MUTATE=P10-BOTH-GRID: AUD0PER = 4050 (prog_s_both_per4050), graded with R-MOD's D_mod: the requests are 4050 ticks apart and drift off the grid of R-MOD; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P1-B stimulus red NMH_MUTATE=P1-VOL1WRITE: channel 1's own AUDxVOL is rewritten from 40 to 24 halfway through (prog_s_av_vol1write): 24 has a reference level pair and is no word of M_V; RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
//
// Dynamic instances without a contrast of their own, and why: SPECIFICATION.md, "Mutation scope".

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
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

// ---- harness-side constants (nanomig.i / ch5_regs.i, restated) --------

constexpr uint32_t kResults      = 0x0C0000;
constexpr uint32_t kDoneAddr     = 0x0C0FFE;
constexpr uint16_t kDoneValue    = 0x600D;
constexpr uint32_t kFixtureStore = kResults + 0;
constexpr uint32_t kAdkrStore    = kResults + 2;
constexpr uint32_t kDmacrStore   = kResults + 4;
constexpr uint32_t kMarker       = kResults + 0x10;

constexpr uint32_t kCh0Data = 0x088000;
constexpr uint32_t kCh1Data = 0x089000;
constexpr uint32_t kCh2Data = 0x08A000;
constexpr uint32_t kCh3Data = 0x08B000;

constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kCck = 8; // budget-sizing constant only; not a graded quantity.
uint64_t budget_for(uint64_t ticks) { return kBootBudget + ticks * kCck * 3 + 200000; }

// One clk_sys cycle: the resolution of the audio output and of the port log
// (SPECIFICATION.md, Units). A duration compared with a reference measured at
// another alignment may differ by this much; no page gives a tolerance.
constexpr double kClkRes = 1.0;

uint32_t word_addr(uint32_t byte_addr) { return byte_addr / 2; }
uint32_t ch_base(int ch) { return word_addr(ch == 0 ? kCh0Data : ch == 1 ? kCh1Data : ch == 2 ? kCh2Data : kCh3Data); }

struct Checks {
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

// A line whose precondition is not met prints NOT GRADED, never PASS or FAIL.
void not_graded(Checks& checks, const std::string& id, const std::string& unmet) {
    checks.record("NOT GRADED " + id + ": precondition(s) not met: " + unmet);
}
void dep_expect(Checks& checks, bool deps, bool cond, const std::string& label, const std::string& unmet) {
    if (!deps) { not_graded(checks, label.substr(0, label.find_first_of(" :")), unmet); return; }
    checks.expect(cond, label);
}

// ---- capture ------------------------------------------------------------

struct Capture {
    bool loaded = false;
    bool released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;
    std::vector<nmh::AudioSample> audio;
    std::optional<uint16_t> adkr, dmacr, fixture;
};

Capture simulate(const std::string& bin, uint64_t run_budget) {
    Capture c;
    nmh::Config cfg;
    nmh::NanomigHarness h(cfg);
    c.loaded = h.load_program(bin) > 0;
    if (!c.loaded) return c;
    c.released = h.boot(kBootBudget).cpu_released;
    c.done = h.run_until_write(kDoneAddr, kDoneValue, run_budget);
    c.log = h.ram_log();
    c.audio = h.audio();
    c.adkr = h.peek_logical(kAdkrStore);
    c.dmacr = h.peek_logical(kDmacrStore);
    c.fixture = h.peek_logical(kFixtureStore);
    return c;
}

// Optional on-disk store of captures: when NMH_CAPCACHE names a directory, a capture is
// read from it, or simulated and written to it, under a name made from the program image
// and the run budget. It changes no verdict (a stored capture is the capture the same
// image and budget produced on the same build); a directory must hold the captures of one
// build only. It is unset in the ordinary run.
uint64_t fnv_file(const std::string& path) {
    uint64_t h = 1469598103934665603ULL;
    FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) return 0;
    unsigned char buf[4096];
    size_t n;
    while ((n = std::fread(buf, 1, sizeof buf, f)) > 0)
        for (size_t i = 0; i < n; ++i) { h ^= buf[i]; h *= 1099511628211ULL; }
    std::fclose(f);
    return h;
}
template <class T> void put_pod(FILE* f, const T& v) { std::fwrite(&v, sizeof v, 1, f); }
template <class T> bool get_pod(FILE* f, T& v) { return std::fread(&v, sizeof v, 1, f) == 1; }
void put_opt16(FILE* f, const std::optional<uint16_t>& o) { put_pod<uint8_t>(f, o.has_value()); put_pod<uint16_t>(f, o.value_or(0)); }
bool get_opt16(FILE* f, std::optional<uint16_t>& o) {
    uint8_t has; uint16_t v;
    if (!get_pod(f, has) || !get_pod(f, v)) return false;
    o = has ? std::optional<uint16_t>(v) : std::nullopt;
    return true;
}
bool cache_read(const std::string& path, Capture& c) {
    FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) return false;
    bool ok = true;
    uint32_t magic = 0; uint8_t b8 = 0; uint64_t n = 0, d = 0;
    ok = ok && get_pod(f, magic) && magic == 0x4E4D4831u;
    ok = ok && get_pod(f, b8); c.loaded = b8;
    ok = ok && get_pod(f, b8); c.released = b8;
    ok = ok && get_pod(f, b8) && get_pod(f, d); c.done = b8 ? std::optional<uint64_t>(d) : std::nullopt;
    ok = ok && get_opt16(f, c.adkr) && get_opt16(f, c.dmacr) && get_opt16(f, c.fixture);
    ok = ok && get_pod(f, n);
    if (ok) { c.log.resize(n); ok = n == 0 || std::fread(c.log.data(), sizeof(nmh::RamAccess), n, f) == n; }
    ok = ok && get_pod(f, n);
    if (ok) { c.audio.resize(n); ok = n == 0 || std::fread(c.audio.data(), sizeof(nmh::AudioSample), n, f) == n; }
    std::fclose(f);
    return ok;
}
void cache_write(const std::string& path, const Capture& c) {
    const std::string tmp = path + ".part";
    FILE* f = std::fopen(tmp.c_str(), "wb");
    if (!f) return;
    put_pod<uint32_t>(f, 0x4E4D4831u);
    put_pod<uint8_t>(f, c.loaded); put_pod<uint8_t>(f, c.released);
    put_pod<uint8_t>(f, c.done.has_value()); put_pod<uint64_t>(f, c.done.value_or(0));
    put_opt16(f, c.adkr); put_opt16(f, c.dmacr); put_opt16(f, c.fixture);
    put_pod<uint64_t>(f, c.log.size());
    if (!c.log.empty()) std::fwrite(c.log.data(), sizeof(nmh::RamAccess), c.log.size(), f);
    put_pod<uint64_t>(f, c.audio.size());
    if (!c.audio.empty()) std::fwrite(c.audio.data(), sizeof(nmh::AudioSample), c.audio.size(), f);
    std::fclose(f);
    std::rename(tmp.c_str(), path.c_str());
}

// A program image and run budget determine the run, so each is simulated once
// per process; every session below may be graded by several lines.
Capture run_capture(const std::string& bin, uint64_t run_budget) {
    static std::map<std::string, Capture> memo;
    const std::string key = bin + "#" + std::to_string(run_budget);
    auto it = memo.find(key);
    if (it == memo.end()) {
        const char* cdir = std::getenv("NMH_CAPCACHE");
        Capture c;
        bool have = false;
        std::string cpath;
        if (cdir && *cdir) {
            const size_t slash = bin.find_last_of('/');
            cpath = std::string(cdir) + "/" + (slash == std::string::npos ? bin : bin.substr(slash + 1)) + "." +
                    std::to_string(run_budget) + "." + std::to_string(fnv_file(bin)) + ".cap";
            have = cache_read(cpath, c);
        }
        if (!have) {
            c = simulate(bin, run_budget);
            if (!cpath.empty() && c.loaded) cache_write(cpath, c);
        }
        it = memo.emplace(key, std::move(c)).first;
    }
    return it->second;
}

// ---- markers --------------------------------------------------------

std::vector<std::pair<uint64_t, uint16_t>> markers(const Capture& c) {
    std::vector<std::pair<uint64_t, uint16_t>> out;
    uint32_t w = word_addr(kMarker);
    for (const auto& a : c.log)
        if (a.is_write && a.word_addr == w) out.push_back({a.cycle, a.data});
    return out;
}
std::optional<uint64_t> m_on(const std::vector<std::pair<uint64_t, uint16_t>>& mk) {
    for (const auto& p : mk) if (p.second == 1) return p.first;
    return std::nullopt;
}
std::optional<uint64_t> m_off(const std::vector<std::pair<uint64_t, uint16_t>>& mk) {
    std::optional<uint64_t> last;
    for (const auto& p : mk) if (p.second == 2) last = p.first;
    return last;
}
std::optional<uint64_t> marker_code(const std::vector<std::pair<uint64_t, uint16_t>>& mk, uint16_t code) {
    for (const auto& p : mk) if (p.second == code) return p.first;
    return std::nullopt;
}

// Boundary cycles [on, step1, step2, ..., off] for a sweep whose steps
// (after the first, which is M_on itself) are marked by `codes`, in
// order. Empty on any missing marker (an incomplete/failed capture).
std::vector<uint64_t> sweep_bounds(const Capture& c, const std::vector<uint16_t>& codes) {
    auto mk = markers(c);
    std::vector<uint64_t> b;
    auto on = m_on(mk);
    if (!on) return b;
    b.push_back(*on);
    for (auto code : codes) {
        auto m = marker_code(mk, code);
        if (!m) return {};
        b.push_back(*m);
    }
    auto off = m_off(mk);
    if (!off) return {};
    b.push_back(*off);
    return b;
}

// SideResult: the output on which a channel is measured (see
// measure_channel_side() below, defined after is_constant()).
struct SideResult { int side = -1; bool ok = false; };

// ---- audio runs -------------------------------------------------------

struct Run { uint64_t start, end; uint16_t value; };

std::vector<Run> runs_in_window(const std::vector<nmh::AudioSample>& audio, uint64_t lo, uint64_t hi, int side) {
    std::vector<Run> out;
    hi = std::min<uint64_t>(hi, audio.size());
    if (lo >= hi) return out;
    uint64_t start = lo;
    uint16_t val = side ? audio[lo].right : audio[lo].left;
    for (uint64_t i = lo + 1; i < hi; ++i) {
        uint16_t v = side ? audio[i].right : audio[i].left;
        if (v != val) { out.push_back({start, i, val}); start = i; val = v; }
    }
    out.push_back({start, hi, val});
    return out;
}
std::vector<Run> complete_runs(const std::vector<Run>& runs, uint64_t lo, uint64_t hi) {
    std::vector<Run> out;
    for (size_t i = 0; i < runs.size(); ++i) {
        if (i == 0 && runs[i].start == lo) continue;
        if (i + 1 == runs.size() && runs[i].end == hi) continue;
        out.push_back(runs[i]);
    }
    return out;
}
bool is_constant(const std::vector<nmh::AudioSample>& audio, uint64_t lo, uint64_t hi, int side) {
    auto runs = runs_in_window(audio, lo, hi, side);
    return runs.size() <= 1;
}

// The end of every window that grades samples, runs or levels of a channel's
// output: the sample the DMA stop cuts short must not be in it. The stop is
// the DMACON clear that precedes M_off by one instruction; it is seen on the
// output as the final run (the output idle until the capture ends). The window
// ends one clk after the start of the run before it -- the sample in progress at
// the stop, or the last complete one where that sample and the idle level are the
// same value -- so that sample is in progress at the window's end and every
// complete-run and complete-sample population leaves it out. Nothing here
// depends on a measured latency. An output that never goes idle (constant to the
// end of the capture) has no cut sample; the window ends at M_off.
uint64_t play_end(const std::vector<nmh::AudioSample>& audio, int side, uint64_t m_off) {
    if (side < 0 || audio.size() < 3) return m_off;
    auto val = [&](size_t i) -> uint16_t { return side ? audio[i].right : audio[i].left; };
    const uint16_t idle = val(audio.size() - 1);
    size_t j = audio.size() - 1;
    while (j > 0 && val(j - 1) == idle) --j;     // start of the final run
    if (j == 0) return m_off;
    size_t k = j - 1;
    const uint16_t prev = val(j - 1);
    while (k > 0 && val(k - 1) == prev) --k;     // start of the run before it
    return (uint64_t)k + 1;
}

// ---- side measurement (measured, never assumed from a page) --
// Exactly one output must change while the other stays constant, in a
// session where only this channel plays. Returns -1 (ambiguous/unusable)
// otherwise -- callers gate on the .ok field.
SideResult measure_channel_side(const std::vector<nmh::AudioSample>& audio, uint64_t lo, uint64_t hi) {
    if (lo >= hi || hi > audio.size()) return {};
    bool left_const = is_constant(audio, lo, hi, 0);
    bool right_const = is_constant(audio, lo, hi, 1);
    if (!left_const && right_const) return {0, true};
    if (left_const && !right_const) return {1, true};
    return {-1, false};
}

uint64_t median_u64(std::vector<uint64_t> v) {
    if (v.empty()) return 0;
    std::sort(v.begin(), v.end());
    return v[v.size() / 2];
}
double median_d(std::vector<double> v) {
    if (v.empty()) return 0;
    std::sort(v.begin(), v.end());
    return v[v.size() / 2];
}
std::pair<uint16_t, uint16_t> level_pair(const std::vector<Run>& runs) {
    std::map<uint16_t, int> freq;
    for (const auto& r : runs) freq[r.value]++;
    std::vector<std::pair<int, uint16_t>> byfreq;
    for (const auto& kv : freq) byfreq.push_back({kv.second, kv.first});
    std::sort(byfreq.begin(), byfreq.end(), [](auto& a, auto& b) { return a.first > b.first; });
    uint16_t a = byfreq.size() > 0 ? byfreq[0].second : 0;
    uint16_t b = byfreq.size() > 1 ? byfreq[1].second : a;
    if (a > b) std::swap(a, b);
    return {a, b};
}
// Groups consecutive runs into maximal segments that alternate between
// exactly two values (an SQ channel's own alternation) -- a "word
// interval" under a constant applied volume/period.
struct Segment { uint64_t start, end; uint16_t a, b; int n; };
std::vector<Segment> segment_by_pair(const std::vector<Run>& runs) {
    std::vector<Segment> segs;
    size_t i = 0;
    while (i < runs.size()) {
        if (i + 1 >= runs.size()) { segs.push_back({runs[i].start, runs[i].end, runs[i].value, runs[i].value, 1}); break; }
        uint16_t lo = std::min(runs[i].value, runs[i + 1].value), hi = std::max(runs[i].value, runs[i + 1].value);
        size_t j = i;
        while (j < runs.size() && (runs[j].value == lo || runs[j].value == hi)) ++j;
        segs.push_back({runs[i].start, runs[j - 1].end, lo, hi, (int)(j - i)});
        i = j;
    }
    return segs;
}

// Sample boundaries are SQ's sign alternations (the +100/-100
// members of the *current* reference level pair), not every raw value
// change -- a volume word landing inside a sample must not split it.
// Consecutive runs of the same sign are merged into one sample, whose
// duration is the merged span and whose representative value is the
// *last* raw run's value (the settled one, after any mid-sample word).
struct Sample { uint64_t start, end; uint16_t value; };
// The output is a 15-bit two's-complement value (AudioSample): SQ's bytes are +100 and -100, so the sign
// (bit 14) alternates at every sample boundary whatever the volume, and 0 is the idle level.
int sample_value(uint16_t v) { v &= 0x7FFF; return (v & 0x4000) ? (int)v - 0x8000 : (int)v; }
int sign_of(uint16_t v) { int x = sample_value(v); return x > 0 ? 1 : x < 0 ? -1 : 0; }
std::vector<Sample> samples_by_sign(const std::vector<Run>& runs) {
    std::vector<Sample> out;
    size_t i = 0;
    while (i < runs.size()) {
        int s = sign_of(runs[i].value);
        size_t j = i + 1;
        while (j < runs.size() && sign_of(runs[j].value) == s) ++j;
        out.push_back({runs[i].start, runs[j - 1].end, runs[j - 1].value});
        i = j;
    }
    return out;
}
std::vector<Sample> complete_samples(const std::vector<Sample>& s, uint64_t lo, uint64_t hi) {
    std::vector<Sample> out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (i == 0 && s[i].start == lo) continue;
        if (i + 1 == s.size() && s[i].end == hi) continue;
        out.push_back(s[i]);
    }
    return out;
}

// ---- fetch records ------------------------------------------------------

std::vector<uint64_t> fetch_cycles(const std::vector<nmh::RamAccess>& log, uint32_t base_word, uint32_t span_words,
                                    uint64_t lo, uint64_t hi) {
    std::vector<nmh::RamAccess> region;
    for (const auto& a : log)
        if (!a.is_write && a.word_addr >= base_word && a.word_addr < base_word + span_words && a.cycle >= lo && a.cycle <= hi)
            region.push_back(a);
    auto co = nmh::coalesce(region);
    std::vector<uint64_t> out;
    for (auto& a : co) out.push_back(a.cycle);
    return out;
}
// Offsets (word index within the region) of each fetch, in order.
std::vector<uint32_t> fetch_offsets(const std::vector<nmh::RamAccess>& log, uint32_t base_word, uint32_t span_words,
                                     uint64_t lo, uint64_t hi) {
    std::vector<nmh::RamAccess> region;
    for (const auto& a : log)
        if (!a.is_write && a.word_addr >= base_word && a.word_addr < base_word + span_words && a.cycle >= lo && a.cycle <= hi)
            region.push_back(a);
    auto co = nmh::coalesce(region);
    std::vector<uint32_t> out;
    for (auto& a : co) out.push_back(a.word_addr - base_word);
    return out;
}
struct FetchRec { uint64_t cycle; uint32_t offset; };
std::vector<FetchRec> fetch_records(const std::vector<nmh::RamAccess>& log, uint32_t base_word, uint32_t span_words,
                                     uint64_t lo, uint64_t hi) {
    auto times = fetch_cycles(log, base_word, span_words, lo, hi);
    auto offs = fetch_offsets(log, base_word, span_words, lo, hi);
    std::vector<FetchRec> out;
    for (size_t i = 0; i < times.size() && i < offs.size(); ++i) out.push_back({times[i], offs[i]});
    return out;
}
std::optional<uint64_t> latest_fetch_before(const std::vector<FetchRec>& fr, uint32_t offset, uint64_t before) {
    std::optional<uint64_t> best;
    for (auto& f : fr) if (f.offset == offset && f.cycle <= before) { if (!best || f.cycle > *best) best = f.cycle; }
    return best;
}
// PC3 grades cyclic stepping from the channel's first restart (the first
// fetch at offset 0 after the first fetch of the record) on; the first pass is
// graded in hrm/ch5-audio-dma-first-pass, not here.
// Returns: -1 if there is no restart, or if a fetch after the restart
// breaks the cyclic step (a genuine PC3 violation); otherwise the number
// of complete post-restart cycles found (0 if the stepping itself holds
// but too little data follows the restart to complete even one cycle --
// gated by the caller on the population requirement, never PASS or FAIL
// by itself).
int stepping_cycles(const std::vector<uint32_t>& offs, uint32_t L) {
    if (L == 0 || offs.empty()) return -1;
    size_t restart = 1;
    while (restart < offs.size() && offs[restart] != 0) ++restart;
    if (restart >= offs.size()) return -1;
    for (size_t i = restart + 1; i < offs.size(); ++i)
        if (offs[i] != (offs[i - 1] + 1) % L) return -1;
    return (int)((offs.size() - restart) / L);
}
// The cycle of a channel's own first restart (the same anchor stepping_cycles
// uses): the first fetch at offset 0 after the first fetch of the record.
// std::nullopt if the record is too short to show one. It depends only on
// fetch offsets, never on a value the channel plays.
std::optional<uint64_t> first_restart_cycle(const std::vector<FetchRec>& fr) {
    if (fr.size() < 2) return std::nullopt;
    for (size_t i = 1; i < fr.size(); ++i) if (fr[i].offset == 0) return fr[i].cycle;
    return std::nullopt;
}

// Cyclic-consecutive check: does `seq` match `order` cycled from some
// starting index, repeated as many times as needed? Every element of seq
// must be present in order for this to succeed.
bool cyclic_consecutive(const std::vector<int>& seq, const std::vector<int>& order) {
    if (seq.empty() || order.empty()) return false;
    for (size_t r = 0; r < order.size(); ++r) {
        bool ok = true;
        for (size_t i = 0; i < seq.size(); ++i)
            if (seq[i] != order[(r + i) % order.size()]) { ok = false; break; }
        if (ok) return true;
    }
    return false;
}

// ---- common per-session preconditions (PC1/PC2/PC3) ----------------

// PC3's own region: the channel's own 4 KB (ch5_regs.i: CH0_DATA..CH3_DATA
// are 4 KB, $800 words, apart), not a LEN-wide span -- a LEN-wide span
// hides any fetch that overruns the data set (S-LEN-A ch1's first pass, for
// example), which then never reaches stepping_cycles at all.
constexpr uint32_t kChannelRegionWords = 0x800;

struct ChSpan { int ch; uint32_t len; };

// The cycle of channel ch's own first restart in [lo, hi] (the anchor PC3 grades from), if any.
std::optional<uint64_t> restart_in(const Capture& c, int ch, uint64_t lo, uint64_t hi) {
    return first_restart_cycle(fetch_records(c.log, ch_base(ch), kChannelRegionWords, lo, hi));
}

// The verdicts of one session's PC1, PC2 and PC3 lines. The lines of the propositions that grade the
// session are gated on them. PC3 passes when no enabled channel breaks its step; a channel with too
// few cycles prints "not evaluated" and does not fail it.
struct Pre {
    bool pc1 = false, pc2 = false, pc3 = false;
    bool ok() const { return pc1 && pc2 && pc3; }
    std::string unmet(const std::string& session) const {
        std::string u;
        if (!pc1) u += "PC1";
        if (!pc2) u += std::string(u.empty() ? "" : "/") + "PC2";
        if (!pc3) u += std::string(u.empty() ? "" : "/") + "PC3";
        return u + " (" + session + ")";
    }
};

// A session as SPECIFICATION.md defines it: its attach mask, the AUDxEN bits it sets, the channels it
// enables with their AUDxLEN, and the complete cycles PC3 needs.
struct SessSpec {
    std::string name;
    uint16_t mask, aud;
    std::vector<ChSpan> enabled;
    uint32_t min_cycles;
    bool pc3;
};

// Each evaluator prints its line when `out` is given and returns the verdict either way.
bool pc1_eval(Checks* out, const std::string& tag, const Capture& c, uint16_t expect_mask) {
    bool pc1 = c.adkr && (*c.adkr & 0xFF) == expect_mask;
    bool ok = c.loaded && c.done.has_value() && pc1;
    if (out) out->expect(ok, "PC1 [p.265] " + tag + ": stored ADKCONR bits 7-0 equal the session's attach mask");
    return ok;
}
bool pc2_eval(Checks* out, const std::string& tag, const Capture& c, uint16_t expect_aud_mask) {
    bool pc2 = c.dmacr && (*c.dmacr & 0x0200) == 0x0200 && (*c.dmacr & expect_aud_mask) == expect_aud_mask;
    bool ok = c.loaded && c.done.has_value() && pc2;
    if (out) out->expect(ok, "PC2 [p.278] " + tag + ": stored DMACONR has DMAEN and the session's AUDxEN bits set");
    return ok;
}
// min_cycles: the specification's population for PC3's modulator clause --
// two complete cycles after the first restart, or one in S-P2 (the only
// caller that passes 1). Short of that population, PC3 is unmet: it prints
// RECORDED (never PASS or FAIL), returns false, and the lines gated on it
// print NOT GRADED; a broken step (stepping_cycles == -1) is always FAIL,
// whatever the population.
bool pc3_eval(Checks* out, const std::string& tag, const Capture& c, const std::vector<ChSpan>& enabled,
              bool have_bounds, uint64_t lo, uint64_t hi, uint32_t min_cycles = 2) {
    bool all_ok = true;
    for (auto& e : enabled) {
        std::string label = "PC3 [p.147] " + tag + " ch" + std::to_string(e.ch) + ": fetches step 0.." +
            std::to_string(e.len - 1) + ",0,... from the channel's first restart on (the first "
            "pass is graded in hrm/ch5-audio-dma-first-pass), with >=" + std::to_string(min_cycles) +
            " complete cycle(s) in its own 4 KB region";
        if (!have_bounds || !c.loaded || !c.done.has_value()) {
            all_ok = false;
            if (out) out->expect(false, label);
            continue;
        }
        auto offs = fetch_offsets(c.log, ch_base(e.ch), kChannelRegionWords, lo, hi);
        int cycles = stepping_cycles(offs, e.len);
        if (cycles < 0) {
            all_ok = false;
            if (out) out->expect(false, label);
        } else if ((uint32_t)cycles < min_cycles) {
            // The stepping holds but the population the precondition needs is not there: the precondition
            // is unmet (neither PASS nor FAIL), and every line gated on it prints NOT GRADED.
            all_ok = false;
            if (out) out->record(label + " [precondition not met: only " + std::to_string(cycles) +
                " complete cycle(s) after the first restart in this window; the lines gated on PC3 print NOT GRADED]");
        } else if (out) {
            out->expect(true, label);
        }
    }
    return all_ok;
}
Pre run_pre(Checks* out, const std::string& tag, const Capture& c,
            uint16_t expect_mask, uint16_t expect_aud_mask,
            const std::vector<ChSpan>& enabled, bool have_bounds, uint64_t lo, uint64_t hi,
            uint32_t min_cycles = 2, bool with_pc3 = true) {
    Pre p;
    p.pc1 = pc1_eval(out, tag, c, expect_mask);
    p.pc2 = pc2_eval(out, tag, c, expect_aud_mask);
    p.pc3 = with_pc3 ? pc3_eval(out, tag, c, enabled, have_bounds, lo, hi, min_cycles) : true;
    return p;
}
void assert_pc123(Checks& checks, const std::string& tag, const Capture& c,
                  uint16_t expect_mask, uint16_t expect_aud_mask,
                  const std::vector<ChSpan>& enabled, bool have_bounds, uint64_t lo, uint64_t hi,
                  uint32_t min_cycles = 2) {
    run_pre(&checks, tag, c, expect_mask, expect_aud_mask, enabled, have_bounds, lo, hi, min_cycles);
}
// The PC lines of a named session, bounds from the capture's own markers.
Pre sess_pre(Checks* out, const SessSpec& sp, const Capture& c) {
    auto mk = markers(c);
    auto on = m_on(mk), off = m_off(mk);
    bool have_bounds = on && off;
    return run_pre(out, sp.name, c, sp.mask, sp.aud, sp.enabled, have_bounds,
                   have_bounds ? *on : 0, have_bounds ? *off : 0, sp.min_cycles, sp.pc3);
}

const SessSpec kSAV{"S-AV", 0x01, 0x0003, {{0, 8}, {1, 4}}, 2, true};
const SessSpec kSAP{"S-AP", 0x10, 0x0003, {{0, 4}, {1, 4}}, 2, true};
const SessSpec kSBOTH{"S-BOTH", 0x11, 0x0003, {{0, 8}, {1, 4}}, 2, true};
const SessSpec kSBV{"S-BOTH-volonly", 0x01, 0x0003, {{0, 8}, {1, 4}}, 2, true};
const SessSpec kSP2{"S-P2", 0x10, 0x0003, {{0, 17}, {1, 8}}, 1, true};
const SessSpec kSLENA{"S-LEN-A", 0x01, 0x0003, {{0, 5}, {1, 3}}, 2, true};
const SessSpec kSLENB{"S-LEN-B", 0x01, 0x0003, {{0, 2}, {1, 7}}, 2, true};
const SessSpec kDAC3V{"S-DAC3-V", 0x08, 0x0008, {{3, 2}}, 2, true};
const SessSpec kDAC3P{"S-DAC3-P", 0x80, 0x0008, {{3, 2}}, 2, true};
const SessSpec kRMOD{"R-MOD", 0x00, 0x0001, {{0, 8}}, 2, true};
const SessSpec kRDACP4{"R-DAC-P4", 0x00, 0x0001, {{0, 4}}, 2, true};
const SessSpec kRDACVP{"R-DAC-VP", 0x00, 0x0001, {{0, 8}}, 2, true};
const SessSpec kRDAC3V{"R-DAC3-V", 0x00, 0x0008, {{3, 2}}, 2, true};
const SessSpec kRDAC3P{"R-DAC3-P", 0x00, 0x0008, {{3, 2}}, 2, true};

}  // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";
    auto mutating = [&](const std::string& id) { return mv == id; };
    // A mutation run says which program it ran and whether that run finished.
    auto mut_note = [&](const std::string& sel, const std::string& what, const Capture& c) {
        if (mv == sel)
            checks.note("MUTATION " + sel + ": " + what + " -- loaded=" + std::to_string((int)c.loaded) +
                        " reached DONE=" + std::to_string((int)c.done.has_value()));
    };

    // ================= FIXTURE (no claim id) =============================
    {
        std::string bin = dir + (mutating("FIXTURE-STORE") ? "/prog_fixture_mut.bin"
                                 : mutating("FIXTURE-ACTIVITY") ? "/prog_fixture_noplay.bin" : "/prog_fixture.bin");
        Capture c = run_capture(bin, budget_for(2000));
        mut_note("FIXTURE-STORE", bin, c);
        mut_note("FIXTURE-ACTIVITY", bin, c);
        bool fix_ok = c.loaded && c.done.has_value() && c.fixture && *c.fixture == 0xBEEF;
        checks.expect(fix_ok, "FIXTURE-STORE: a known constant is stored before anything reads one "
                              "[apparatus check, no claim uid]");
        bool activity = c.loaded && c.done.has_value() &&
            (!is_constant(c.audio, 0, c.audio.size(), 0) || !is_constant(c.audio, 0, c.audio.size(), 1));
        checks.expect(activity,
            "FIXTURE-ACTIVITY: channel 0's own output takes more than one value on at least one side "
            "while it plays [apparatus check, no claim uid]");
    }

    // ================= R-MOD / D_mod / side_of[0] / PC6 ====================
    Capture rmod = run_capture(dir + "/prog_rmod.bin", budget_for(200000));
    auto rmod_b = sweep_bounds(rmod, {});
    bool rmod_bounds = rmod_b.size() == 2;

    // Channel 0's own side, measured from R-MOD itself (only channel 0
    // plays there), not assumed from any page or RTL constant.
    SideResult side0 = rmod_bounds ? measure_channel_side(rmod.audio, rmod_b[0], rmod_b[1]) : SideResult{};

    double d_mod = 0;
    if (rmod_bounds && side0.ok) {
        auto runs = runs_in_window(rmod.audio, rmod_b[0], rmod_b[1], side0.side);
        auto comp = complete_runs(runs, rmod_b[0], rmod_b[1]);
        std::vector<uint64_t> durs;
        for (auto& r : comp) durs.push_back(r.end - r.start);
        d_mod = (double)median_u64(durs);
    }
    checks.note("D_mod (R-MOD, AUD0PER=4000, channel 0's own measured side) = " +
        std::to_string(d_mod) + " clk_sys cycles");
    auto ticks = [&](double t) { return d_mod * t / 4000.0; };
    // PC6: the spread of channel 0's complete samples about D_mod.
    auto pc6_spread_ok = [&](const Capture& c, const std::vector<uint64_t>& b) -> bool {
        if (!(d_mod > 0 && side0.ok && b.size() >= 2)) return false;
        uint64_t lo = b.front(), hi = play_end(c.audio, side0.side, b.back());
        auto runs = runs_in_window(c.audio, lo, hi, side0.side);
        auto comp = complete_runs(runs, lo, hi);
        bool ok = !comp.empty();
        for (auto& r : comp)
            if (std::fabs((double)(r.end - r.start) - d_mod) > kClkRes) { ok = false; break; }
        return ok;
    };
    {
        Capture pc6c = mutating("PC6") ? run_capture(dir + "/prog_rmod_pc6.bin", budget_for(200000)) : rmod;
        mut_note("PC6", "prog_rmod_pc6.bin", pc6c);
        auto b = sweep_bounds(pc6c, {});
        checks.expect(pc6c.loaded && pc6c.done.has_value() && pc6_spread_ok(pc6c, b),
            "PC6 [p.140,p.141,p.164]: in R-MOD, every complete sample duration of channel 0 equals "
            "D_mod within the apparatus resolution (one clk_sys cycle)");
    }

    // ================= PC7: the fetch record of channel 0 and its frame ====
    // p.164: "Because of pipelining in Agnus, the first data word must be
    // thrown away. State 101 is entered as soon as this word arrives; a
    // request for the next data word has already gone out. When the data
    // arrives, state 010 is entered". The page gives no address for the word
    // thrown away, so the record (the reads of channel 0's own 4 KB region) may
    // or may not hold it. The page's F_2 -- the word whose arrival enters state
    // 010 -- is therefore found in the run, from channel 0's own output:
    // "In the 010 state the upper byte is output, and in the 011 state the lower
    // byte is output", and 010 lasts one period, so the first change of the
    // output to a lower-byte level begins D_mod after 010 was entered.
    struct Cal { bool ok = false; size_t r = 0; double t010 = 0, dist = 0; uint32_t offset = 0; size_t n = 0; };
    auto rec_of = [&](const Capture& c) -> std::vector<FetchRec> {
        auto mk = markers(c);
        auto on = m_on(mk), off = m_off(mk);
        if (!on || !off) return {};
        return fetch_records(c.log, ch_base(0), kChannelRegionWords, *on, *off);
    };
    auto calibrate = [&](const Capture& c) -> Cal {
        Cal cal;
        auto mk = markers(c);
        auto on = m_on(mk), off = m_off(mk);
        if (!(on && off && d_mod > 0 && side0.ok)) return cal;
        auto runs = runs_in_window(c.audio, *on, *off, side0.side);
        if (runs.size() < 3) return cal;
        std::map<uint16_t, int> freq;
        for (auto& r : runs) freq[r.value]++;
        uint16_t zero_level = runs[0].value;
        int best_count = 0;
        for (auto& kv : freq) if (kv.second > best_count) { zero_level = kv.first; best_count = kv.second; }
        std::optional<uint64_t> t011;
        for (auto& r : runs)
            if (r.value != runs[0].value && r.value != zero_level) { t011 = r.start; break; }
        if (!t011) return cal;
        cal.t010 = (double)*t011 - d_mod;
        auto rec = rec_of(c);
        cal.n = rec.size();
        if (rec.empty()) return cal;
        double bd = 1e18;
        for (size_t i = 0; i < rec.size(); ++i) {
            double d = std::fabs((double)rec[i].cycle - cal.t010);
            if (d < bd) { bd = d; cal.r = i; }
        }
        cal.dist = bd;
        cal.offset = rec[cal.r].offset;
        // The fetch nearest to the instant 010 was entered is the page's F_2 when the two are the same
        // event: the page puts the arrival of the word and the entry to 010 together ("When the data
        // arrives, state 010 is entered"). No address of the word thrown away is assumed: the entry is
        // found by time alone, and it is unambiguous when the instant lies nearer to that entry than the
        // entry lies to any other fetch of the record.
        double gap_min = 1e18;
        if (cal.r > 0) gap_min = std::min(gap_min, (double)rec[cal.r].cycle - (double)rec[cal.r - 1].cycle);
        if (cal.r + 1 < rec.size()) gap_min = std::min(gap_min, (double)rec[cal.r + 1].cycle - (double)rec[cal.r].cycle);
        cal.ok = rec.size() > cal.r + 1 && bd < gap_min;
        return cal;
    };
    // The calibration is made once, on R-MOD; a contrast of PC7-CAL replaces the capture it is
    // made on, and the verdict gates PC7-LOOP, PC7-N3 and every P10 line.
    Capture cal_cap = mutating("PC7-CAL") ? run_capture(dir + "/prog_rmod_per6000.bin", budget_for(200000)) : rmod;
    mut_note("PC7-CAL", "prog_rmod_per6000.bin", cal_cap);
    const Cal cal = calibrate(cal_cap);
    const bool cal_ok = cal_cap.loaded && cal_cap.done.has_value() && cal.ok;
    // One horizontal line: p.164 says requests are transferred to Agnus once each horizontal line, and the
    // request made when F_2 arrives (Figure 5-8, 101->010) is the next one, so F_3 - F_2 in R-MOD is one
    // line, measured here on the ordinary R-MOD whichever capture a contrast of PC7-CAL replaces. A
    // counter-driven interval between two fetches of one channel is realised to within less than one line
    // (each fetch follows its request by less than a line plus a constant delay): every comparison of two
    // fetch times of one channel, or of the same two fetches in two sessions, uses `< ell`.
    const Cal cal_ord = mutating("PC7-CAL") ? calibrate(rmod) : cal;
    const auto rec_rmod = rec_of(rmod);
    const double ell = (cal_ord.ok && rec_rmod.size() > cal_ord.r + 1)
        ? (double)rec_rmod[cal_ord.r + 1].cycle - (double)rec_rmod[cal_ord.r].cycle : 0.0;
    checks.note("one horizontal line (F_3 - F_2 of the ordinary R-MOD) = " + std::to_string(ell) + " clk_sys cycles");
    auto cal_bounds = sweep_bounds(cal_cap, {});
    checks.note("PC7-CAL measured: the lower byte of the first word is output at M_on+" +
        std::to_string(cal.t010 + d_mod - (cal_bounds.size() == 2 ? (double)cal_bounds[0] : 0.0)) +
        " clk, so state 010 was entered at M_on+" + std::to_string(cal.t010 - (cal_bounds.size() == 2 ? (double)cal_bounds[0] : 0.0)) +
        " clk; the nearest fetch is record entry " + std::to_string(cal.r) + " of " + std::to_string(cal.n) +
        " (offset " + std::to_string(cal.offset) + ") " + std::to_string(cal.dist) + " clk away");
    checks.expect(cal_ok,
        "PC7-CAL [p.164]: in R-MOD, the fetch whose arrival enters state 010 is calibrated from channel "
        "0's own output: the instant 010 was entered lies nearer to one fetch than that fetch lies to any "
        "other fetch of the record -- it is the page's F_2");
    // The frame -- no read of channel 0's region before M_on.
    {
        const Capture& fc = mutating("PC7-FRAME") ? run_capture(dir + "/prog_rmod_pc7frame.bin", budget_for(200000)) : rmod;
        mut_note("PC7-FRAME", "prog_rmod_pc7frame.bin", fc);
        auto b = sweep_bounds(fc, {});
        bool no_early = b.size() == 2 && fetch_cycles(fc.log, ch_base(0), kChannelRegionWords, 0, b[0] > 0 ? b[0] - 1 : 0).empty();
        checks.expect(fc.loaded && fc.done.has_value() && b.size() == 2 && no_early,
            "PC7-FRAME [p.164]: no read of channel 0's data region occurs before M_on");
    }
    // PC7-LOOP: every consecutive pair of fetches from the page's F_3 on is 2*D_mod apart, to within
    // less than one line ("Transitions such as 010->011->010 occur whenever the period counter counts
    // down to one"; one request per 010->011->010 loop; requests are transferred once each line).
    auto pc7_loop_ok = [&](const std::vector<FetchRec>& rec, size_t r) -> bool {
        if (d_mod <= 0 || rec.size() < r + 4) return false;
        for (size_t i = r + 2; i < rec.size(); ++i) {
            double gap = (double)rec[i].cycle - (double)rec[i - 1].cycle;
            if (!(std::fabs(gap - 2.0 * d_mod) < ell)) return false;
        }
        return true;
    };
    {
        Capture lc = mutating("PC7-LOOP") ? run_capture(dir + "/prog_rmod_pc7.bin", budget_for(200000)) : rmod;
        mut_note("PC7-LOOP", "prog_rmod_pc7.bin", lc);
        dep_expect(checks, cal_ok, lc.loaded && lc.done.has_value() && side0.ok && pc7_loop_ok(rec_of(lc), cal.r),
            "PC7-LOOP [p.164,Fig.5-8 011->010]: loop anchor -- in R-MOD, every consecutive pair of fetches "
            "from the page's F_3 on is D_mod*2 apart, to within less than one line",
            "PC7-CAL");
    }
    // PC7-N3: Figure 5-8's 101->010 bracket, "AUDxDR if napnav" (napnav = 1 with no attach bit): the
    // request made when F_2 arrives is fetched before the first time-out of the period counter, which
    // enters 011 one period after 010, so 0 < F_3 - F_2 < D_mod. No claim id in either claims file names
    // this transition.
    auto pc7_n3_ok = [&](const std::vector<FetchRec>& rec, size_t r) -> bool {
        if (!(d_mod > 0 && rec.size() > r + 1)) return false;
        const double gap = (double)rec[r + 1].cycle - (double)rec[r].cycle;
        return gap > 0 && gap < d_mod;
    };
    {
        Capture nc = mutating("PC7-N3") ? run_capture(dir + "/prog_s_ap.bin", budget_for(110000)) : rmod;
        mut_note("PC7-N3", "prog_s_ap.bin", nc);
        dep_expect(checks, cal_ok, nc.loaded && nc.done.has_value() && pc7_n3_ok(rec_of(nc), cal.r),
            "PC7-N3 [Fig.5-8 101->010 'AUDxDR if napnav']: 0 < F_3 - F_2 < D_mod (the request made on "
            "101->010 is fetched before the first time-out of the period counter)",
            "PC7-CAL");
    }

    // ================= reference sweeps ===================================

    struct SweepDef { std::vector<uint16_t> codes; std::vector<int> values; };
    auto level_table = [&](const Capture& c, const SweepDef& sd, int side) {
        std::map<int, std::pair<uint16_t, uint16_t>> table;
        auto b = sweep_bounds(c, sd.codes);
        if (b.size() != sd.values.size() + 1) return table;
        for (size_t i = 0; i < sd.values.size(); ++i) {
            auto runs = runs_in_window(c.audio, b[i], b[i + 1], side);
            if (runs.size() > 2) runs.erase(runs.begin(), runs.begin() + 2);
            table[sd.values[i]] = level_pair(runs);
        }
        return table;
    };
    auto duration_table = [&](const Capture& c, const SweepDef& sd, int side) {
        std::map<int, double> table;
        auto b = sweep_bounds(c, sd.codes);
        if (b.size() != sd.values.size() + 1) return table;
        for (size_t i = 0; i < sd.values.size(); ++i) {
            auto runs = runs_in_window(c.audio, b[i], b[i + 1], side);
            auto comp = complete_runs(runs, b[i], b[i + 1]);
            if (comp.size() > 2) comp.erase(comp.begin(), comp.begin() + 2);
            std::vector<uint64_t> durs;
            for (auto& r : comp) durs.push_back(r.end - r.start);
            table[sd.values[i]] = (double)median_u64(durs);
        }
        return table;
    };
    auto pairwise_distinct = [](auto& table) {
        std::vector<typename std::decay<decltype(table)>::type::mapped_type> vals;
        for (auto& kv : table) vals.push_back(kv.second);
        for (size_t i = 0; i < vals.size(); ++i)
            for (size_t j = i + 1; j < vals.size(); ++j)
                if (vals[i] == vals[j]) return false;
        return true;
    };
    auto duration_rises = [](std::map<int, double>& table) {
        double prev = -1; bool first = true;
        for (auto& kv : table) { if (!first && kv.second <= prev) return false; prev = kv.second; first = false; }
        return true;
    };

    SweepDef vol1_def{{10,11,12,13,14,15,16,17,18,19,20,21,22}, {40,1,2,4,8,16,20,21,24,32,48,56,60,63}};
    SweepDef vol2_def{{10,11,12,13}, {40,8,20,48,60}};
    SweepDef vol3_def{{10,11,12,13}, {40,8,20,48,60}};
    SweepDef per1_def{{10,11,12,13}, {200,300,500,700,900}};
    SweepDef per2_def{{10,11,12,13}, {200,300,500,700,900}};
    SweepDef per3_def{{10,11,12,13}, {200,300,500,700,900}};
    // R-RAMP: the 700 settle interval [M_on, marker10) is added at the front of the 17 M_P values, so
    // the boundary count (19: on + 17 codes + off) matches values.size()+1 (18 values + 1).
    // ramp_def.values[1..] are the 17 words of M_P in the order the data set holds them.
    SweepDef ramp_def{{10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26},
                      {700,400,401,402,404,408,384,432,464,272,16784,144,33168,912,8592,1424,4496,2448}};

    Capture rvol1 = run_capture(dir + "/prog_rvol1.bin", budget_for(56000));
    Capture rvol2 = run_capture(dir + "/prog_rvol2.bin", budget_for(20000));
    Capture rvol3 = run_capture(dir + "/prog_rvol3.bin", budget_for(20000));
    Capture rper1 = run_capture(dir + "/prog_rper1.bin", budget_for(100000));
    Capture rper2 = run_capture(dir + "/prog_rper2.bin", budget_for(100000));
    Capture rper3 = run_capture(dir + "/prog_rper3.bin", budget_for(100000));
    Capture rramp = run_capture(dir + "/prog_rramp.bin", budget_for(440000));

    // Each channel's own side, measured from its own reference session
    // (the full R-VOL-c extent: only channel c plays there).
    auto rvol1_bnd = sweep_bounds(rvol1, vol1_def.codes);
    auto rvol2_bnd = sweep_bounds(rvol2, vol2_def.codes);
    auto rvol3_bnd = sweep_bounds(rvol3, vol3_def.codes);
    auto rper1_bnd = sweep_bounds(rper1, per1_def.codes);
    SideResult side1 = rvol1_bnd.size() == vol1_def.values.size() + 1
        ? measure_channel_side(rvol1.audio, rvol1_bnd.front(), rvol1_bnd.back()) : SideResult{};
    SideResult side2 = rvol2_bnd.size() == vol2_def.values.size() + 1
        ? measure_channel_side(rvol2.audio, rvol2_bnd.front(), rvol2_bnd.back()) : SideResult{};
    SideResult side3 = rvol3_bnd.size() == vol3_def.values.size() + 1
        ? measure_channel_side(rvol3.audio, rvol3_bnd.front(), rvol3_bnd.back()) : SideResult{};
    SideResult side1p = rper1_bnd.size() == per1_def.values.size() + 1
        ? measure_channel_side(rper1.audio, rper1_bnd.front(), rper1_bnd.back()) : SideResult{};
    // side_of[]: the output on which each channel is graded everywhere in
    // this file. Falls back to -1 (nothing matches) if the reference
    // session itself did not measure cleanly.
    int side_of[4] = {
        side0.ok ? side0.side : -1,
        side1.ok ? side1.side : -1,
        side2.ok ? side2.side : -1,
        side3.ok ? side3.side : -1,
    };
    auto other_side = [&](int ch) { return side_of[ch] < 0 ? -1 : 1 - side_of[ch]; };

    auto L1 = side_of[1] >= 0 ? level_table(rvol1, vol1_def, side_of[1]) : std::map<int, std::pair<uint16_t,uint16_t>>{};
    auto L2 = side_of[2] >= 0 ? level_table(rvol2, vol2_def, side_of[2]) : std::map<int, std::pair<uint16_t,uint16_t>>{};
    auto L3 = side_of[3] >= 0 ? level_table(rvol3, vol3_def, side_of[3]) : std::map<int, std::pair<uint16_t,uint16_t>>{};
    auto D1 = side_of[1] >= 0 ? duration_table(rper1, per1_def, side_of[1]) : std::map<int, double>{};
    auto D2t = side_of[2] >= 0 ? duration_table(rper2, per2_def, side_of[2]) : std::map<int, double>{};
    auto D3t = side_of[3] >= 0 ? duration_table(rper3, per3_def, side_of[3]) : std::map<int, double>{};
    auto DR = side_of[1] >= 0 ? duration_table(rramp, ramp_def, side_of[1]) : std::map<int, double>{}; // R-RAMP's channel is ch1

    // PC4, per channel/sweep. The two ids share one call each: PC4-VOL for the level-pair sweeps, PC4-DUR
    // for the duration sweeps (R-RAMP included).
    auto pc4_vol = [&](const std::string& name, const Capture& c, const SweepDef& sd,
                       std::map<int, std::pair<uint16_t,uint16_t>>& table) {
        checks.expect(c.loaded && c.done.has_value() && table.size() == sd.values.size() && pairwise_distinct(table),
            "PC4-VOL [reference frame]: in " + name + " the reference level pairs of all VSET values are "
            "pairwise distinct");
    };
    auto pc4_dur = [&](const std::string& name, const Capture& c, const SweepDef& sd, std::map<int, double>& table) {
        checks.expect(c.loaded && c.done.has_value() && table.size() == sd.values.size() && pairwise_distinct(table) &&
                      duration_rises(table),
            "PC4-DUR [reference frame]: in " + name + " the reference durations are pairwise distinct and "
            "rise strictly with the written value");
    };
    {
        Capture rvm = mutating("PC4-VOL") ? run_capture(dir + "/prog_rvol1_mut.bin", budget_for(56000)) : rvol1;
        mut_note("PC4-VOL", "prog_rvol1_mut.bin", rvm);
        auto L1m = mutating("PC4-VOL") && side_of[1] >= 0 ? level_table(rvm, vol1_def, side_of[1]) : L1;
        pc4_vol("R-VOL-1 (VSET_1)", rvm, vol1_def, L1m);
        pc4_vol("R-VOL-2 (VSET_2)", rvol2, vol2_def, L2);
        pc4_vol("R-VOL-3 (VSET_3)", rvol3, vol3_def, L3);
        Capture rpm = mutating("PC4-DUR") ? run_capture(dir + "/prog_rper1_mut.bin", budget_for(100000)) : rper1;
        mut_note("PC4-DUR", "prog_rper1_mut.bin", rpm);
        auto D1m = mutating("PC4-DUR") && side_of[1] >= 0 ? duration_table(rpm, per1_def, side_of[1]) : D1;
        pc4_dur("R-PER-1", rpm, per1_def, D1m);
        pc4_dur("R-PER-2", rper2, per2_def, D2t);
        pc4_dur("R-PER-3", rper3, per3_def, D3t);
        pc4_dur("R-RAMP (including the 700 settle)", rramp, ramp_def, DR);
    }

    // PC5: grades only the pairing the two pages agree on --
    // 0 with 3 on one output, 1 with 2 on the other, the two outputs
    // different. Two independent instances: the volume-sweep family and
    // the period-sweep family (channel 0 via R-MOD in both).
    {
        SideResult s3 = side3;
        if (mutating("PC5-VOL")) {
            // R-VOL-1's capture in place of R-VOL-3's: "channel 3's" side is then channel 1's.
            auto b = sweep_bounds(rvol1, vol3_def.codes);
            s3 = b.size() == vol3_def.values.size() + 1 ? measure_channel_side(rvol1.audio, b.front(), b.back()) : SideResult{};
            checks.note("MUTATION PC5-VOL: prog_rvol1.bin as R-VOL-3 -- measured side " + std::to_string(s3.side));
        }
        bool pairing_ok = side0.ok && side1.ok && side2.ok && s3.ok &&
            side0.side == s3.side && side1.side == side2.side && side0.side != side1.side;
        checks.expect(pairing_ok,
            "PC5-VOL [p.137,p.326]: the pairing (volume-sweep family) -- p.137 'Channels 1 and 2 are "
            "connected to the left-side stereo output jack. Channels 0 and 3 are connected to the "
            "right-side output jack.'; p.326 (Appendix E, J6) AUDO 'left (channels 0, 3)', AUDI "
            "'right (channels 1, 2)' -- the two pages disagree on which jack is which but agree on "
            "the pairing; channels 0 and 3 measure on one output, 1 and 2 on the other, and the two "
            "outputs differ");
        checks.note(std::string("PC5 measured sides: ch0=") + (side0.ok ? std::to_string(side0.side) : "?") +
            " ch1=" + (side1.ok ? std::to_string(side1.side) : "?") +
            " ch2=" + (side2.ok ? std::to_string(side2.side) : "?") +
            " ch3=" + (side3.ok ? std::to_string(side3.side) : "?") + " (0=left,1=right)");
    }
    {
        SideResult s1p = side1p;
        if (mutating("PC5-PER")) {
            // R-VOL-3's capture in place of R-PER-1's: "channel 1's" side is then channel 3's.
            auto b = sweep_bounds(rvol3, per1_def.codes);
            s1p = b.size() == per1_def.values.size() + 1 ? measure_channel_side(rvol3.audio, b.front(), b.back()) : SideResult{};
            checks.note("MUTATION PC5-PER: prog_rvol3.bin as R-PER-1 -- measured side " + std::to_string(s1p.side));
        }
        bool pairing_ok = side0.ok && s1p.ok && side2.ok && side3.ok &&
            side0.side == side3.side && s1p.side == side2.side && side0.side != s1p.side;
        checks.expect(pairing_ok,
            "PC5-PER [p.137,p.326]: the pairing (period-sweep family) -- channel 1's own side measured "
            "from R-PER-1 agrees with channel 2's (from R-VOL-2, the same physical routing), and "
            "with channel 0's (R-MOD) and channel 3's (R-VOL-3) differing, as above");
    }

    // L_a: minimum over all R-VOL-1 transitions of (first sample at the new
    // level - marker time), on channel 1's measured own side. The register
    // write for step i's value precedes marker i in program order, so the
    // scan for step i starts at the previous marker (step 0 has none
    // earlier than "on"), not at marker i itself -- starting the scan at
    // marker i can only ever find the new value at or after marker i,
    // which forces a >=0 result even where the true first sample is
    // earlier. lat is kept signed here so an earlier-than-marker sample is
    // reported honestly instead of being clamped. It aligns audio time with
    // port time where a check compares the two (P8's window start).
    double L_a = 0;
    if (side1.ok && rvol1_bnd.size() == vol1_def.values.size() + 1) {
        bool have_best = false;
        double best = 0;
        int side = side_of[1];
        for (size_t i = 0; i < vol1_def.values.size(); ++i) {
            auto pr = L1[vol1_def.values[i]];
            uint64_t scan_from = i >= 1 ? rvol1_bnd[i - 1] : rvol1_bnd[i];
            for (uint64_t t = scan_from; t < rvol1_bnd[i + 1] && t < rvol1.audio.size(); ++t) {
                uint16_t v = side ? rvol1.audio[t].right : rvol1.audio[t].left;
                if (v == pr.first || v == pr.second) {
                    double lat = (double)t - (double)rvol1_bnd[i];
                    if (!have_best || lat < best) { best = lat; have_best = true; }
                    break;
                }
            }
        }
        L_a = have_best ? best : 0;
    }
    checks.note("L_a (audio latency, from R-VOL-1, channel 1's own measured side) = " +
        std::to_string(L_a) + " clk_sys cycles");
    int64_t L_a_i = (int64_t)std::llround(L_a);
    auto plus_La = [&](uint64_t t) -> uint64_t { return (uint64_t)((int64_t)t + L_a_i); };

    // PC1/PC2/PC3 for every reference session (every session asserts
    // these, not only the attach sessions). PC1, PC2 and PC3 each have their own mutation
    // (below), which replaces the capture of the R-MOD line it grades; their per-session
    // call sites are the same functions.
    {
        Capture c2 = mutating("PC2") ? run_capture(dir + "/prog_rmod_nodma.bin", budget_for(200000)) : rmod;
        mut_note("PC2", "prog_rmod_nodma.bin", c2);
        Capture c3 = mutating("PC3") ? run_capture(dir + "/prog_rmod_len7.bin", budget_for(200000)) : rmod;
        mut_note("PC3", "prog_rmod_len7.bin", c3);
        auto b3 = sweep_bounds(c3, {});
        pc1_eval(&checks, "R-MOD", rmod, 0x00);
        pc2_eval(&checks, "R-MOD", c2, 0x0001);
        pc3_eval(&checks, "R-MOD", c3, {{0, 8}}, b3.size() == 2, b3.size() ? b3[0] : 0, b3.size() ? b3[1] : 0);
    }
    assert_pc123(checks, "R-VOL-1", rvol1, 0x00, 0x0002, {{1, 4}}, rvol1_bnd.size() == vol1_def.values.size() + 1, rvol1_bnd.size() ? rvol1_bnd.front() : 0, rvol1_bnd.size() ? rvol1_bnd.back() : 0);
    assert_pc123(checks, "R-VOL-2", rvol2, 0x00, 0x0004, {{2, 4}}, rvol2_bnd.size() == vol2_def.values.size() + 1, rvol2_bnd.size() ? rvol2_bnd.front() : 0, rvol2_bnd.size() ? rvol2_bnd.back() : 0);
    assert_pc123(checks, "R-VOL-3", rvol3, 0x00, 0x0008, {{3, 4}}, rvol3_bnd.size() == vol3_def.values.size() + 1, rvol3_bnd.size() ? rvol3_bnd.front() : 0, rvol3_bnd.size() ? rvol3_bnd.back() : 0);
    assert_pc123(checks, "R-PER-1", rper1, 0x00, 0x0002, {{1, 4}}, rper1_bnd.size() == per1_def.values.size() + 1, rper1_bnd.size() ? rper1_bnd.front() : 0, rper1_bnd.size() ? rper1_bnd.back() : 0);
    {
        auto b = sweep_bounds(rper2, per2_def.codes);
        assert_pc123(checks, "R-PER-2", rper2, 0x00, 0x0004, {{2, 4}}, b.size() == per2_def.values.size() + 1, b.size() ? b.front() : 0, b.size() ? b.back() : 0);
    }
    {
        auto b = sweep_bounds(rper3, per3_def.codes);
        assert_pc123(checks, "R-PER-3", rper3, 0x00, 0x0008, {{3, 4}}, b.size() == per3_def.values.size() + 1, b.size() ? b.front() : 0, b.size() ? b.back() : 0);
    }
    {
        auto b = sweep_bounds(rramp, ramp_def.codes);
        assert_pc123(checks, "R-RAMP", rramp, 0x00, 0x0002, {{1, 8}}, b.size() == ramp_def.values.size() + 1, b.size() ? b.front() : 0, b.size() ? b.back() : 0);
    }

    // R- controls for P8: the unattached reference sessions. Loaded
    // here so P8 can assert the population precondition below.
    Capture rdacp4 = run_capture(dir + "/prog_rdacp4.bin", budget_for(60000));
    Capture rdacvp = run_capture(dir + "/prog_rdacvp.bin", budget_for(125000));
    Capture rdac3v = run_capture(dir + "/prog_rdac3v.bin", budget_for(30000));
    Capture rdac3p = run_capture(dir + "/prog_rdac3p.bin", budget_for(30000));
    {
        auto b = sweep_bounds(rdacp4, {});
        assert_pc123(checks, "R-DAC-P4", rdacp4, 0x00, 0x0001, {{0, 4}}, b.size() == 2, b.size() ? b[0] : 0, b.size() ? b[1] : 0);
    }
    {
        auto b = sweep_bounds(rdacvp, {});
        assert_pc123(checks, "R-DAC-VP", rdacvp, 0x00, 0x0001, {{0, 8}}, b.size() == 2, b.size() ? b[0] : 0, b.size() ? b[1] : 0);
    }
    {
        auto b = sweep_bounds(rdac3v, {});
        assert_pc123(checks, "R-DAC3-V", rdac3v, 0x00, 0x0008, {{3, 2}}, b.size() == 2, b.size() ? b[0] : 0, b.size() ? b[1] : 0);
    }
    {
        auto b = sweep_bounds(rdac3p, {});
        assert_pc123(checks, "R-DAC3-P", rdac3p, 0x00, 0x0008, {{3, 2}}, b.size() == 2, b.size() ? b[0] : 0, b.size() ? b[1] : 0);
    }

    // Data-set orders (the specified data, not RTL behaviour -- see
    // SPECIFICATION.md's "Data sets").
    const std::vector<int> m_v_order  = {32, 1, 2, 4, 8, 16, 63, 21};
    const std::vector<int> m_p_order(ramp_def.values.begin() + 1, ramp_def.values.end()); // the 17 words of M_P
    const std::vector<int> m_p4_order = {300, 700, 500, 900};
    auto m_v_pos = [&](int v) -> int {
        for (size_t i = 0; i < m_v_order.size(); ++i) if (m_v_order[i] == v) return (int)i;
        return -1;
    };
    auto m_p4_pos = [&](int v) -> int {
        for (size_t i = 0; i < m_p4_order.size(); ++i) if (m_p4_order[i] == v) return (int)i;
        return -1;
    };
    // M_VP position table: even positions volume, odd positions period.
    struct VpEntry { bool is_period; int value; };
    const std::vector<VpEntry> m_vp_order = {{false,24},{true,300},{false,56},{true,700},{false,8},{true,500},{false,40},{true,900}};
    auto m_vp_pos = [&](bool is_period, int v) -> int {
        for (size_t i = 0; i < m_vp_order.size(); ++i)
            if (m_vp_order[i].is_period == is_period && m_vp_order[i].value == v) return (int)i;
        return -1;
    };

    // The S-AP session, loaded once for every section below.
    Capture sap_cap = run_capture(dir + "/prog_s_ap.bin", budget_for(110000));
    // The destination half of the AUDxAP joint line (filled in P5), the source half in P8.
    bool sap_dest_ok = false;
    size_t sap_dest_pop = 0;
    // P7 hands the verdicts of channel 3's two bits to the joint lines printed after P8:
    // index 0 is bit 3 (ATVOL3), index 1 is bit 7 (ATPER3).
    bool p7_const_ok[2] = {false, false}, p7_spacing_ok[2] = {false, false};
    Pre p7_pre[2];
    // The verdicts of the lines that the joint lines printed after them contain: P1-A (bit 0), P2-A (bit 4),
    // P6-A (bits 1, 2, 5, 6; indexed by bit number).
    bool p1a_ok = false, p1a_gate = false, p2a_ok = false, p2a_gate = false;
    bool p6a_ok[8] = {false, false, false, false, false, false, false, false};
    bool p6a_gate[8] = {false, false, false, false, false, false, false, false};

    // S-P2's window on channel 1's output (P2, P3-SP2): from the modulator's first restart to the
    // end of play (see Measurements).
    struct P2Win { bool ok = false; uint64_t ws = 0, we = 0; };
    auto p2_window = [&](const Capture& c) -> P2Win {
        P2Win w;
        auto mk = markers(c);
        auto on = m_on(mk), off = m_off(mk);
        if (!(on && off) || side_of[1] < 0) return w;
        auto restart = restart_in(c, 0, *on, *off);
        if (!restart) return w;
        w.ws = *restart;
        w.we = play_end(c.audio, side_of[1], *off);
        w.ok = w.we > w.ws;
        return w;
    };

    // The applied periods of S-P2's channel 1 in its window: runs of equal durations, each matched to the
    // reference duration of a word (R-RAMP), -1 for a run no word explains. The first run shows the period in
    // force at the window's edge, applied before the window opened; the changes are the runs after it. A window
    // without one has an empty population of applied changes: P2-B and P3-SP2 print FAIL for it (an applied-event
    // population), and P2-C grades the waveform on the window's complete runs, which exist whatever channel 1 receives.
    struct P2Runs { P2Win w; std::vector<int> periods; std::vector<double> means; std::vector<uint64_t> starts; size_t changes() const { return periods.size() > 1 ? periods.size() - 1 : 0; }
        std::vector<int> applied() const { return periods.size() > 1 ? std::vector<int>(periods.begin() + 1, periods.end()) : std::vector<int>{}; }
        std::vector<double> applied_means() const { return means.size() > 1 ? std::vector<double>(means.begin() + 1, means.end()) : std::vector<double>{}; }
        std::vector<uint64_t> applied_starts() const { return starts.size() > 1 ? std::vector<uint64_t>(starts.begin() + 1, starts.end()) : std::vector<uint64_t>{}; } };
    auto p2_runs = [&](const Capture& c) -> P2Runs {
        P2Runs r;
        r.w = p2_window(c);
        if (!r.w.ok) return r;
        auto runs = runs_in_window(c.audio, r.w.ws, r.w.we, side_of[1]);
        auto comp = complete_runs(runs, r.w.ws, r.w.we);
        size_t i = 0;
        while (i < comp.size()) {
            size_t j = i;
            uint64_t dur0 = comp[i].end - comp[i].start;
            std::vector<uint64_t> durs;
            while (j < comp.size() && (uint64_t)std::llabs((long long)(comp[j].end - comp[j].start) - (long long)dur0) <= (uint64_t)kClkRes)
                { durs.push_back(comp[j].end - comp[j].start); ++j; }
            double mean = 0; for (auto d : durs) mean += (double)d; mean /= std::max<size_t>(1, durs.size());
            // Match against DR (R-RAMP), not D1 (R-PER-1).
            int found = -1;
            for (auto& kv : DR) if (std::fabs(kv.second - mean) <= kClkRes) { found = kv.first; break; }
            r.periods.push_back(found);
            r.means.push_back(mean);
            r.starts.push_back(comp[i].start);
            i = j;
        }
        return r;
    };

    // ================= P1: ATVOL0 =========================================
    {
        std::string bin = dir + (mutating("PC1") ? "/prog_s_av_noattach.bin"
                                 : mutating("P1-A") ? "/prog_s_av_swap.bin"
                                 : mutating("P1-JOINT") ? "/prog_s_av_ch3.bin"
                                 : mutating("P1-WINDOW") ? "/prog_s_av_stop.bin"
                                 : mutating("P1-VOL1WRITE") ? "/prog_s_av_vol1write.bin"
                                 : mutating("P1-PER1WRITE") ? "/prog_s_av_per1write.bin" : "/prog_s_av.bin");
        Capture c = run_capture(bin, budget_for(200000));
        mut_note("PC1", bin, c);
        mut_note("P1-A", bin, c);
        mut_note("P1-JOINT", bin, c);
        mut_note("P1-WINDOW", bin, c);
        mut_note("P1-VOL1WRITE", bin, c);
        mut_note("P1-PER1WRITE", bin, c);
        int side = side_of[1];
        auto mk = markers(c);
        auto on = m_on(mk), off = m_off(mk);
        bool have_bounds = on && off;
        const Pre pre = sess_pre(&checks, kSAV, c);

        // The window runs from the modulator's first restart (channel 0's first fetch at offset 0 after the
        // first entry of its record: the first pass is graded in hrm/ch5-audio-dma-first-pass, not here) to the
        // end of play of channel 1's output (see Measurements). It does not depend on what channel 1 plays,
        // so a channel 1 that is never modulated has a window and its verdict is the line's own.
        uint64_t win_start = 0, win_end = 0;
        bool win_ok = false;
        if (have_bounds && side >= 0) {
            auto restart0 = restart_in(c, 0, *on, *off);
            if (restart0) {
                win_start = *restart0;
                win_end = play_end(c.audio, side, *off);
                win_ok = win_end > win_start;
                checks.note("S-AV: the window of channel 1's output ends " + std::to_string((long long)*off - (long long)win_end) +
                    " clk before M_off, inside the sample the DMA stop cuts short");
            }
        }
        // Applied volumes are the CHANGES of channel 1's level pair: the first segment of the window shows the
        // value in force at its edge, which was applied before the window opened and is not a change, and
        // neither is a later segment that merely repeats the value in force. A pair that is no reference pair
        // counts as a change to -1 and makes the window's levels fail P1-B.
        std::vector<int> applied;
        bool level_ok = win_ok, dur_ok = false;
        size_t dur_pop = 0;
        if (win_ok) {
            auto runs = runs_in_window(c.audio, win_start, win_end, side);
            auto segs = segment_by_pair(runs);
            int last_val = 40;
            bool in_force = true;
            for (auto& sg : segs) {
                int found = -1;
                for (auto& kv : L1) if (kv.second.first == sg.a && kv.second.second == sg.b) { found = kv.first; break; }
                if (in_force) {
                    in_force = false;
                    if (found < 0) level_ok = false; else last_val = found;
                    continue;
                }
                if (found < 0) { level_ok = false; applied.push_back(-1); continue; }
                if (found != last_val) { applied.push_back(found); last_val = found; }
            }
            // Duration is graded on sign-alternation samples, not raw runs.
            auto samples = samples_by_sign(runs);
            auto comp = complete_samples(samples, win_start, win_end);
            dur_pop = comp.size();
            dur_ok = !comp.empty();
            for (auto& sm : comp)
                if (D1.count(200) && std::fabs((double)(sm.end - sm.start) - D1[200]) > kClkRes) { dur_ok = false; break; }
        }
        // P1-WINDOW is a precondition of every line below it; when it is not met they print NOT GRADED,
        // and so they do when PC1, PC2 or PC3 of the session fails.
        if (win_ok) {
            checks.expect(c.loaded && win_ok,
                "P1-WINDOW [p.149]: the window exists -- channel 0 has a first restart (its first fetch at "
                "offset 0 after the first entry of its record) before the end of play of channel 1's output");
        } else {
            checks.record("P1-WINDOW [p.149]: precondition not met -- no window from channel 0's first restart to "
                "the end of play; P1-A, P1-B, P1-C and P1-JOINT print NOT GRADED");
        }
        const bool gate = pre.ok() && win_ok;
        const std::string unmet = std::string(pre.ok() ? "" : pre.unmet("S-AV")) +
            ((!pre.ok() && !win_ok) ? " and " : "") + (win_ok ? "" : "P1-WINDOW");
        bool a_ok = c.loaded && c.done.has_value() && applied.size() >= 17 && level_ok && cyclic_consecutive(applied, m_v_order);
        checks.note("P1: window has " + std::to_string(applied.size()) + " applied-volume changes "
            "(need >=17); cyclic-consecutive-in-M_V=" +
            std::string(cyclic_consecutive(applied, m_v_order) ? "yes" : "no") +
            "; every level pair matched a reference=" + std::string(level_ok ? "yes" : "no"));

        p1a_ok = gate && a_ok;
        p1a_gate = gate;
        dep_expect(checks, gate, a_ok,
            "P1-A [p.149,p.150,p.265] 5-hw.c3,5-p136.c3,5-p149.c5,5-mod.c3,5-p149.c7,"
            "5-mod.c15,5-p150.c2,5-p150.c8,5-p150.c12,5-p150.c16,5-p150.c20,5-t54.w1.c4,5-t54.w2.c4,"
            "5-t54.w3.c4,5-t54.w4.c4,5-p151.c8,5-t55.b0,5-appA-p265.c29,A-adkcon.b00: the applied volumes in "
            "the window are cyclically consecutive in M_V, n>=17, every value from M_V", unmet);
        // P1-B and P1-C grade what the words that reach channel 1 do. The window and the complete samples
        // exist whatever channel 1 receives, so they are the populations that gate; a window without an
        // applied volume is a window in which no word reached the volume, and the two clauses fail with it.
        if (!gate) {
            not_graded(checks, "P1-B", unmet);
        } else {
            // Every level pair in the window is the reference pair L_1(v) of a word v the modulator's data
            // set holds: a pair that is no reference pair, and a reference pair of a volume no word of the
            // data set carries (the CPU's own write to AUD1VOL), both fail.
            bool applied_in_mv = true;
            for (int v : applied) if (m_v_pos(v) < 0) applied_in_mv = false;
            checks.expect(c.loaded && !applied.empty() && level_ok && applied_in_mv,
                "P1-B [p.149,p.165]: the window holds applied volumes and every level pair in it is "
                "channel 1's own reference pair L_1(v) of a word v of the modulator's data set M_V");
        }
        if (!gate) {
            not_graded(checks, "P1-C", unmet);
        } else {
            checks.expect(c.loaded && !applied.empty() && dur_pop > 0 && dur_ok,
                "P1-C [Table 5-4 Modulating Volume Only]: with applied volumes in the window, every channel 1 "
                "duration (graded at SQ's sign alternations) equals D_1(200) -- no word reaches the "
                "period");
        }
        checks.record("5-mod.c12, 5-p149.c18: V6 is withdrawn from P1-A; the pages disagree on "
            "whether a volume value is six or seven bits wide (spec Ambiguities #1), and every "
            "stimulus here has bit 6 clear, so only V5-V0 are graded.");
        checks.record("5-mod.c11, 5-p149.c17: the volume-modulator word's bits 15-7 are declared "
            "not used; every stimulus here has bits 15-7 clear, and no check depends on their "
            "value, so this is not turned into an assertion (SPECIFICATION.md, claims not turned "
            "into a proposition).");
        checks.record("5-mod.c1, 5-mod.c5, 5-p149.c10, 5-mod.c7, 5-p149.c13: each bundles a clause no single "
            "line here establishes (both kinds of modulation; every attach bit; the output ceasing and the "
            "data modulating the next channel for every bit), so none is asserted.");

        // Joint AUDxAV assertion: the destination effect (channel
        // 1 follows M_V, established above) and the source DAC bypass
        // (channel 0's own side holds one value) graded together, since
        // the claim bundles both halves.
        bool ch0_const = false;
        if (have_bounds && side_of[0] >= 0) {
            auto fc0 = fetch_cycles(c.log, ch_base(0), 8, *on, *off);
            if (fc0.size() >= 3) {
                uint64_t s0 = plus_La(fc0[2]), e0 = play_end(c.audio, side_of[0], *off);
                ch0_const = e0 > s0 && is_constant(c.audio, s0, e0, side_of[0]);
            }
        }
        dep_expect(checks, gate, a_ok && ch0_const,
            "P1-JOINT [p.165]: in S-AV, channel 1 follows channel 0's "
            "data (destination half) AND channel 0's own side holds one value once its data is "
            "flowing (source half) -- both halves of the AUDxAV sentence for channel 0, one session", unmet);
        checks.record("5-p165.c19, 5-sig.audxav: the sentence is generic in the channel x. Its two halves are "
            "held in one session for channel 0 only (P1-JOINT). Channel 1's converter half cannot be observed "
            "in a session that also shows its destination: channels 1 and 2 share one output, so channel 1's "
            "own output and channel 2's modulated output cannot be told apart. For channel 2 the destination "
            "half is P6-A (bit 2), whose modulator is silent at volume 0, and no session holds its converter "
            "half. The claim quantifies over every x, so neither id is asserted.");
    }

    // ================= P2: ATPER0 =========================================
    {
        // P2-A reads the data-set order (prog_s_p2_swap); P2-B the duration of the run at each position
        // (prog_s_p2_per1write: the CPU writes channel 1's own period); P2-C the waveform order
        // (prog_s_p2_ramp); P2-JOINT contains P2-A and P2-C. Each selector replaces the one session all of them read.
        std::string bin = dir + (mutating("P2-A") ? "/prog_s_p2_swap.bin"
                                 : mutating("P2-B") ? "/prog_s_p2_per1write.bin"
                                 : mutating("P2-C") ? "/prog_s_p2_ramp.bin" : "/prog_s_p2.bin");
        Capture c = run_capture(bin, budget_for(2700000));
        mut_note("P2-A", bin, c);
        mut_note("P2-B", bin, c);
        mut_note("P2-C", bin, c);
        int side = side_of[1];
        const Pre pre = sess_pre(&checks, kSP2, c);

        // The window runs from the modulator's first restart (the first pass is graded in
        // hrm/ch5-audio-dma-first-pass, not here) to the end of play of channel 1's output, which leaves out
        // the sample the DMA stop cuts short.
        const P2Runs pr = p2_runs(c);
        const bool win_ok = pr.w.ok;
        const uint64_t win_start = pr.w.ws, win_end = pr.w.we;
        const std::vector<int> applied_periods = pr.applied();      // the runs after the one in force at the edge
        const std::vector<double> run_means = pr.applied_means();   // for (b)'s own check
        // (a): the applied periods are cyclically consecutive in the 17 words of M_P, from at least
        // 18 runs (one full cycle of M_P plus one).
        bool cyclic_ok = win_ok && applied_periods.size() >= 18 &&
            std::find(applied_periods.begin(), applied_periods.end(), -1) == applied_periods.end() &&
            cyclic_consecutive(applied_periods, m_p_order);
        // (b) is its own assertion: the mean complete-sample duration of every applied run equals R-RAMP's
        // reference duration of the word the data set holds at that position. The position of a run is
        // channel 0's own record and the page's time-out grid, not channel 1's output (p.149: the word is
        // written "each time the period register of the modulator channel times out"; p.164: the time-outs
        // lie at t010 + m*D_mod, t010 being the session's own F_2 shifted by the offset PC7-CAL measured):
        // the word a run carries is the one the data register holds at the time-out that precedes the run's
        // first complete sample, i.e. the data-set word of the latest fetch before that time-out (the next
        // fetch is requested at that time-out and arrives later). The expected duration is DR[M_P[offset]]
        // and no expected rotation is derived from the periods observed.
        const std::vector<uint64_t> run_starts = pr.applied_starts();
        std::vector<FetchRec> fr_p2;
        {
            auto mk_p2 = markers(c);
            auto on_p2 = m_on(mk_p2), off_p2 = m_off(mk_p2);
            if (win_ok && on_p2 && off_p2) fr_p2 = fetch_records(c.log, ch_base(0), kChannelRegionWords, *on_p2, *off_p2);
        }
        const auto cal_rec_p2 = rec_of(cal_cap);
        const bool p2_frame = cal_ok && d_mod > 0 && cal_rec_p2.size() > cal.r && fr_p2.size() > cal.r + 3;
        const double p2_t010 = p2_frame ? (double)fr_p2[cal.r].cycle + (cal.t010 - (double)cal_rec_p2[cal.r].cycle) : 0.0;
        const double p2_dmod = ticks(35000);   // S-P2's modulator period, AUD0PER = 35000 ticks
        size_t b_anchored = 0, b_matched = 0;
        const bool b_pop = win_ok && !applied_periods.empty() && applied_periods.size() == run_means.size() &&
                           run_starts.size() == run_means.size();
        if (b_pop && p2_frame) {
            for (size_t i = 0; i < run_means.size(); ++i) {
                const double tout = p2_t010 + std::floor(((double)run_starts[i] - p2_t010) / p2_dmod) * p2_dmod;
                const FetchRec* fk = nullptr;
                for (auto& f : fr_p2) if ((double)f.cycle < tout && (!fk || f.cycle > fk->cycle)) fk = &f;
                if (!fk || fk->offset >= m_p_order.size()) continue;
                ++b_anchored;
                const int word = m_p_order[fk->offset];
                if (DR.count(word) && std::fabs(run_means[i] - DR[word]) <= kClkRes) ++b_matched;
            }
        }
        const bool dur_ok = b_pop && p2_frame && b_anchored == run_means.size() && b_matched == b_anchored;
        checks.note("P2: window has " + std::to_string(applied_periods.size()) + " applied-period "
            "runs (need >=18); cyclic-consecutive-in-M_P (via DR)=" + std::string(cyclic_ok ? "yes" : "no"));
        checks.note("P2-B: applied-period runs " + std::to_string(applied_periods.size()) + ", anchored to a "
            "channel-0 fetch of the data set " + std::to_string(b_anchored) + ", duration equal to DR[M_P[offset of "
            "the latest fetch before the run's time-out]] " + std::to_string(b_matched) + "; clause not met: " +
            std::string(!b_pop ? "population (no applied-period run in the window)"
                        : !p2_frame ? "frame (PC7-CAL or a start-up frame in S-P2)"
                        : b_anchored != run_means.size() ? "anchor (a run without a fetch before its time-out)"
                        : b_matched != b_anchored ? "duration (a run differs from DR[M_P[offset]])" : "none"));

        const std::string unmet = pre.unmet("S-P2");
        p2a_ok = pre.ok() && c.loaded && c.done.has_value() && cyclic_ok;
        p2a_gate = pre.ok();
        dep_expect(checks, pre.ok(), c.loaded && c.done.has_value() && cyclic_ok,
            "P2-A [p.149 Table 5-4 Modulating Period Only] 5-mod.c4,5-p149.c8,5-p149.c4,"
            "5-mod.c13,5-p149.c19,5-mod.c16,5-p150.c3,5-p150.c7,5-p150.c11,5-p150.c15,5-p150.c19,"
            "5-t54.w1.c3,5-t54.w2.c3,5-t54.w3.c3,5-t54.w4.c3,5-p151.c4,5-t55.b4,"
            "5-appA-p265.c25,A-adkcon.b04: the applied periods of the runs are "
            "cyclically consecutive in the 17 words of M_P (matched against R-RAMP's own durations), "
            "from at least 18 runs", unmet);
        // The window exists whatever channel 1 receives; with no applied-period run in it no word has reached
        // the period, and (b) fails.
        if (!pre.ok() || !p2_frame) {
            not_graded(checks, "P2-B", std::string(pre.ok() ? "" : unmet) + ((!pre.ok() && !p2_frame) ? " and " : "") +
                (p2_frame ? "" : "PC7-CAL and a start-up frame in S-P2"));
        } else {
            checks.expect(c.loaded && win_ok && dur_ok,
                "P2-B [p.149]: the window holds applied-period runs and the mean complete-sample "
                "duration of each run equals D_c(P) of the word the data set holds at that position "
                "(R-RAMP), within the apparatus resolution");
        }
        // (c): RAMP's sixteen bytes in the order the data set holds them
        // (SPECIFICATION.md, Data sets), against channel 1's raw output
        // sequence in the same window. The levels are R-RAMP's own sixteen
        // distinct output values at volume 48 (the 144 interval); a byte's level
        // is taken from the rank of the byte among the sixteen, because volume
        // scales every byte by one positive factor.
        const int ramp_bytes[16] = {8, 24, 40, 56, 72, 88, 104, 120, -8, -24, -40, -56, -72, -88, -104, -120};
        std::vector<int> ramp_order;
        {
            auto b = sweep_bounds(rramp, ramp_def.codes);
            int idx = -1;
            for (size_t i = 0; i < ramp_def.values.size(); ++i) if (ramp_def.values[i] == 144) { idx = (int)i; break; }
            if (idx >= 0 && (size_t)(idx + 1) < b.size() && side_of[1] >= 0) {
                auto runs = runs_in_window(rramp.audio, b[idx], b[idx + 1], side_of[1]);
                auto comp = complete_runs(runs, b[idx], b[idx + 1]);
                std::vector<int> levels;
                for (auto& r : comp) {
                    int v = sample_value(r.value);
                    if (std::find(levels.begin(), levels.end(), v) == levels.end()) levels.push_back(v);
                }
                if (levels.size() == 16) {
                    std::vector<int> sorted_levels = levels, sorted_bytes(ramp_bytes, ramp_bytes + 16);
                    std::sort(sorted_levels.begin(), sorted_levels.end());
                    std::sort(sorted_bytes.begin(), sorted_bytes.end());
                    for (int bv : ramp_bytes) {
                        size_t rank = std::find(sorted_bytes.begin(), sorted_bytes.end(), bv) - sorted_bytes.begin();
                        ramp_order.push_back(sorted_levels[rank]);
                    }
                }
            }
        }
        bool ramp_seq_ok = false;
        size_t p2c_pop = 0;
        if (win_ok && ramp_order.size() == 16) {
            auto runs = runs_in_window(c.audio, win_start, win_end, side);
            auto comp = complete_runs(runs, win_start, win_end);
            std::vector<int> seq;
            for (auto& r : comp) seq.push_back(sample_value(r.value));
            p2c_pop = seq.size();
            ramp_seq_ok = seq.size() >= 32 && cyclic_consecutive(seq, ramp_order);
        }
        // The complete runs exist whatever channel 1 receives, so this line grades the waveform on its own.
        if (!pre.ok()) {
            not_graded(checks, "P2-C", unmet);
        } else {
            checks.expect(c.loaded && win_ok && ramp_order.size() == 16 && p2c_pop > 0 && ramp_seq_ok,
                "P2-C [p.149,Table 5-4]: channel 1's output follows RAMP's sixteen "
                "bytes in order at volume 48, none skipped or repeated");
        }
        // 5-p149.c9 says both that the pitch changes and that the waveform stays: one line holds both.
        dep_expect(checks, pre.ok(),
            c.loaded && c.done.has_value() && cyclic_ok && win_ok && ramp_order.size() == 16 && p2c_pop > 0 && ramp_seq_ok,
            "P2-JOINT [p.149,Table 5-4] 5-p149.c9: in S-P2 the pitch of channel 1 follows channel 0's period "
            "words (P2-A) AND its output follows RAMP's sixteen bytes in order (P2-C) -- the pitch is changed "
            "by frequency modulation and the basic waveform remains the same", unmet);
    }

    // ================= P3: whole word, not two bytes ======================
    {
        // S P3.3: graded in the P1 window (S-AV: from channel 0's first restart to the end of play of
        // channel 1's output). Each of P3-SILENT and P3-WHOLE reads its own capture, so the contrast of one
        // never reaches the other: a session in which channel 1 is silent (P3-SILENT), and one in which
        // channel 1 plays modulator words as its own data (P3-WHOLE).
        Capture sav = run_capture(dir + "/prog_s_av.bin", budget_for(200000));
        Capture sil_c = mutating("P3-SILENT") ? run_capture(dir + "/prog_s_av_zero.bin", budget_for(200000)) : sav;
        Capture who_c = mutating("P3-WHOLE") ? run_capture(dir + "/prog_s_av_ch1bytes.bin", budget_for(200000)) : sav;
        mut_note("P3-SILENT", "prog_s_av_zero.bin", sil_c);
        mut_note("P3-WHOLE", "prog_s_av_ch1bytes.bin", who_c);

        struct P3Eval { bool win_ok = false; size_t pop = 0; bool no_silent = false, one_pair_ok = false; };
        auto p3_eval = [&](const Capture& ctrl) -> P3Eval {
            P3Eval r;
            auto ctrl_mk = markers(ctrl);
            auto con = m_on(ctrl_mk), coff = m_off(ctrl_mk);
            const int side_a = side_of[1];
            uint64_t ws = 0, we = 0;
            if (con && coff && side_a >= 0) {
                auto restart = restart_in(ctrl, 0, *con, *coff);
                if (restart) { ws = *restart; we = play_end(ctrl.audio, side_a, *coff); r.win_ok = we > ws; }
            }
            if (!r.win_ok) return r;
            auto runs = runs_in_window(ctrl.audio, ws, we, side_a);
            auto segs = segment_by_pair(runs);
            // Word intervals are the intervals of applied words: the changes of channel 1's level pair
            // away from its written starting value.
            int last_val = 40;
            for (auto& sg : segs) {
                int found = -1;
                for (auto& kv : L1) if (kv.second.first == sg.a && kv.second.second == sg.b) { found = kv.first; break; }
                if (found != last_val) { ++r.pop; last_val = found; }
            }
            r.no_silent = !segs.empty();
            for (auto& s : segs) if (s.a == 0 && s.b == 0) { r.no_silent = false; break; }
            // "Exactly one level pair, the whole word's L_1(v)" -- every
            // SEGMENT (word interval), not every raw run, must match some
            // L1 entry exactly; a byte-split channel's segments (one member
            // always 0, the other a raw data byte) never match L1's
            // symmetric non-zero pairs.
            r.one_pair_ok = !segs.empty();
            for (auto& s : segs) {
                bool found = false;
                for (auto& kv : L1) if (kv.second.first == s.a && kv.second.second == s.b) { found = true; break; }
                if (!found) { r.one_pair_ok = false; break; }
            }
            return r;
        };
        const P3Eval e_sil = p3_eval(sil_c), e_who = p3_eval(who_c);
        const Pre pre_sil = sess_pre(nullptr, kSAV, sil_c), pre_who = sess_pre(nullptr, kSAV, who_c);

        const bool p3_whole_line_ok = who_c.loaded && e_who.win_ok && e_who.pop > 0 && e_who.one_pair_ok;
        if (!pre_sil.ok()) {
            not_graded(checks, "P3-SILENT", pre_sil.unmet("S-AV"));
        } else {
            checks.expect(sil_c.loaded && e_sil.win_ok && e_sil.pop > 0 && e_sil.no_silent,
                "P3-SILENT [p.149]: the P1 window holds word intervals and none is silent (volume 0 never "
                "occurs)");
        }
        if (!pre_who.ok()) {
            not_graded(checks, "P3-WHOLE", pre_who.unmet("S-AV"));
        } else {
            checks.expect(p3_whole_line_ok,
                "P3-WHOLE [p.149]: the P1 window holds word intervals and every one has exactly one level "
                "pair, the whole word's reference pair L_1(v) -- the modulator's whole word reaches the "
                "modulated channel and no intermediate byte-split value ever appears");
        }

        // The S-P2 half ("never a byte's D_1(P)") -- reuse P2's own
        // window (from the modulator's first restart to the end of play of channel 1's output);
        // verify the window holds applied-period runs and no run duration matches any byte-split period.
        {
            std::string p2bin = dir + (mutating("P3-SP2") ? "/prog_s_p2_w129.bin" : "/prog_s_p2.bin");
            Capture p2c = run_capture(p2bin, budget_for(2700000));
            mut_note("P3-SP2", p2bin, p2c);
            const Pre pre_p2 = sess_pre(nullptr, kSP2, p2c);
            const P2Runs pr3 = p2_runs(p2c);
            const P2Win& w3 = pr3.w;
            bool no_byte_split = w3.ok;
            size_t p3sp2_pop = 0;
            if (w3.ok) {
                auto runs = runs_in_window(p2c.audio, w3.ws, w3.we, side_of[1]);
                auto comp = complete_runs(runs, w3.ws, w3.we);
                p3sp2_pop = comp.size();
                static const double bad[7] = {65, 129, 3, 33, 5, 17, 9};
                for (auto& r : comp) {
                    double dd = (double)(r.end - r.start);
                    for (double bp : bad) if (std::fabs(dd - ticks(bp)) <= ticks(1)) { no_byte_split = false; break; }
                    if (!no_byte_split) break;
                }
            }
            const bool p3_sp2_line_ok = p2c.loaded && w3.ok && pr3.changes() > 0 && p3sp2_pop > 0 && no_byte_split;
            if (!pre_p2.ok()) {
                not_graded(checks, "P3-SP2", pre_p2.unmet("S-P2"));
            } else {
                checks.expect(p3_sp2_line_ok,
                    "P3-SP2 [p.149,Table 5-4]: in S-P2, the window holds applied-period runs and no run's "
                    "duration matches a byte-split period (65,129,3,33,5,17,9 ticks) -- a whole word is used, "
                    "never its high byte alone");
            }
            // 5-mod.c8 .. 5-p149.c15 say the words are used whole in a modulator, whatever they carry: one
            // line holds the volume mode (P3-WHOLE) and the period mode (P3-SP2).
            dep_expect(checks, pre_who.ok() && pre_p2.ok(), p3_whole_line_ok && p3_sp2_line_ok,
                "P3-JOINT [p.149] 5-mod.c8,5-mod.c9,5-p149.c14,5-p149.c15: a modulator's words are used whole, "
                "not as two bytes, in volume mode (P3-WHOLE, S-AV) AND in period mode (P3-SP2, S-P2)",
                std::string(pre_who.ok() ? "" : pre_who.unmet("S-AV")) + ((!pre_who.ok() && !pre_p2.ok()) ? " and " : "") +
                (pre_p2.ok() ? "" : pre_p2.unmet("S-P2")));
        }
        // R-MOD precondition (p.138) -- the unattached control DOES show
        // byte-level playback, proving the apparatus could detect it.
        {
            Capture pc = mutating("P3-PRE") ? run_capture(dir + "/prog_rmod_vol0.bin", budget_for(200000)) : rmod;
            mut_note("P3-PRE", "prog_rmod_vol0.bin", pc);
            auto pb = sweep_bounds(pc, {});
            bool ctrl_bytes = false;
            if (pb.size() == 2 && side0.ok) {
                auto runs = runs_in_window(pc.audio, pb[0], pb[1], side0.side);
                auto segs = segment_by_pair(runs);
                for (auto& s : segs) if (s.a == 0 && s.b > 0) { ctrl_bytes = true; break; }
            }
            checks.expect(pc.loaded && pc.done.has_value() && pb.size() == 2 && ctrl_bytes,
                "P3-PRE [p.138]: R-MOD's own unattached channel 0 shows two-bytes-per-word "
                "playback (a segment with high byte 0 alongside a nonzero low byte), so this "
                "apparatus can detect byte-splitting where it occurs");
        }
    }

    // ================= P4: both attach bits ===============================
    // The S-BOTH-volonly session, graded below on its own and as P4's contrast.
    Capture s_both_volonly = run_capture(dir + (mutating("P4-VOLONLY") ? "/prog_s_both_volonly_per.bin" : "/prog_s_both_volonly.bin"), budget_for(110000));
    mut_note("P4-VOLONLY", "prog_s_both_volonly_per.bin", s_both_volonly);
    bool p4a_ok = false;
    bool p4_pre_ok = false;
    {
        std::string bin = dir + (mutating("P4-A") ? "/prog_s_both_swap.bin"
                                 : mutating("P4-B") ? "/prog_s_both_v32.bin"
                                 : mutating("P4-C") ? "/prog_s_both_perconst.bin" : "/prog_s_both.bin");
        Capture c = run_capture(bin, budget_for(110000));
        mut_note("P4-A", bin, c);
        mut_note("P4-B", bin, c);
        mut_note("P4-C", bin, c);
        int side = side_of[1];
        auto mk = markers(c);
        auto on = m_on(mk), off = m_off(mk);
        bool have_bounds = on && off;
        const Pre pre = sess_pre(&checks, kSBOTH, c);
        p4_pre_ok = pre.ok();
        const std::string unmet = pre.unmet("S-BOTH");

        // Events start only after the first genuine change -- seed with
        // the pre-modulation initial values (63 for volume, 200 for
        // period), which never recur in M_VP.
        std::vector<std::pair<uint64_t,int>> vol_events, per_events;
        if (have_bounds && side >= 0) {
            uint64_t end = play_end(c.audio, side, *off);
            auto runs = runs_in_window(c.audio, *on, end, side);
            auto segs = segment_by_pair(runs);
            int last_val = 63;
            for (auto& s : segs) {
                for (auto& kv : L1) if (kv.second.first == s.a && kv.second.second == s.b) {
                    if (kv.first != last_val) { vol_events.push_back({s.start, kv.first}); last_val = kv.first; }
                    break;
                }
            }
            auto samples = samples_by_sign(runs);
            auto comp = complete_samples(samples, *on, end);
            int last_per = 200;
            size_t i = 0;
            while (i < comp.size()) {
                size_t j = i; uint64_t dur0 = comp[i].end - comp[i].start;
                while (j < comp.size() && (uint64_t)std::llabs((long long)(comp[j].end-comp[j].start)-(long long)dur0) <= (uint64_t)kClkRes) ++j;
                double mean = 0; for (size_t k=i;k<j;++k) mean += (double)(comp[k].end-comp[k].start); mean /= (double)(j-i);
                for (auto& kv : D1) if (std::fabs(kv.second - mean) <= kClkRes) {
                    if (kv.first != last_per) { per_events.push_back({comp[i].start, kv.first}); last_per = kv.first; }
                    break;
                }
                i = j;
            }
        }
        std::vector<std::pair<uint64_t,int>> merged;
        for (auto& e : vol_events) merged.push_back({e.first, e.second*2+0});
        for (auto& e : per_events) merged.push_back({e.first, e.second*2+1});
        std::sort(merged.begin(), merged.end());
        std::vector<int> seq; for (auto& e : merged) seq.push_back(e.second);
        std::vector<int> m_vp_seq_order = {24*2+0,300*2+1,56*2+0,700*2+1,8*2+0,500*2+1,40*2+0,900*2+1};
        bool cyclic_ok = have_bounds && seq.size() >= 16 && cyclic_consecutive(seq, m_vp_seq_order);
        p4a_ok = c.loaded && c.done.has_value() && cyclic_ok;
        bool set_b = true;
        for (auto& e : vol_events) if (e.second != 24 && e.second != 56 && e.second != 8 && e.second != 40) set_b = false;
        for (auto& e : per_events) if (e.second != 300 && e.second != 700 && e.second != 500 && e.second != 900) set_b = false;
        checks.note("P4: merged sequence has " + std::to_string(seq.size()) + " events (need >=16); "
            "cyclic-consecutive-in-M_VP=" + std::string(cyclic_ok ? "yes" : "no"));

        dep_expect(checks, pre.ok(), p4a_ok,
            "P4-A [p.149,p.150,p.265] 5-mod.c2,5-p149.c6,5-mod.c15,5-mod.c16,"
            "5-p150.c2,5-p150.c3,5-mod.c17,5-p150.c4,5-p150.c6,5-p150.c10,5-p150.c14,5-p150.c18,"
            "5-t54.w1.c2,5-t54.w2.c2,5-t54.w3.c2,5-t54.w4.c2,5-appA-p265.c30,5-appA-p265.c32,"
            "A-adkcon.note1: the merged sequence (events start only after the first genuine change) is "
            "cyclically consecutive in M_VP, alternating volume then period", unmet);
        // P4-B grades what the changes carry. The session and its window exist whatever channel 1 receives;
        // a window with no applied change of either kind is one in which no word has reached channel 1.
        if (!pre.ok()) {
            not_graded(checks, "P4-B", unmet);
        } else {
            checks.expect(c.loaded && have_bounds && (!vol_events.empty() || !per_events.empty()) && set_b,
                "P4-B [p.150]: the window holds applied changes and, after the first change of each kind, no "
                "applied volume lies outside {24,56,8,40} and no applied period lies outside {300,700,500,900}");
        }
        dep_expect(checks, pre.ok(), c.loaded && have_bounds && vol_events.size() >= 8 && per_events.size() >= 8,
            "P4-C [p.149]: both parameters change in the window, each at least 8 times", unmet);
        checks.record("5-appA-p265.c31 (V5-V0), A-adkcon.note2: V6 is withdrawn here as in P1 -- "
            "every stimulus has bit 6 clear.");
    }
    // S-BOTH-volonly graded as its own session (the "requires both bits" half of the claim).
    {
        auto mk = markers(s_both_volonly);
        auto on = m_on(mk), off = m_off(mk);
        bool have_bounds = on && off;
        // The window closes before the sample the DMA stop cuts short.
        uint64_t sbv_end = have_bounds ? play_end(s_both_volonly.audio, side_of[1], *off) : 0;
        // The period side ("only bit 0 set does not modulate the period"), graded on complete samples
        // only. Channel 1's own volume-change count is measured for context, not gated: it shares
        // P1-A's population, and requiring it here would fold that line into this one.
        bool period_const = false; size_t period_pop = 0; int vol_changes = 0;
        if (have_bounds && side_of[1] >= 0) {
            auto runs = runs_in_window(s_both_volonly.audio, *on, sbv_end, side_of[1]);
            auto samples = samples_by_sign(runs);
            auto comp = complete_samples(samples, *on, sbv_end);
            period_pop = comp.size();
            period_const = !comp.empty();
            for (auto& s : comp)
                if (D1.count(200) && std::fabs((double)(s.end - s.start) - D1[200]) > kClkRes) { period_const = false; break; }
            auto segs = segment_by_pair(runs);
            int last = 63;
            for (auto& s : segs) for (auto& kv : L1) if (kv.second.first == s.a && kv.second.second == s.b) {
                if (kv.first != last) { ++vol_changes; last = kv.first; } break;
            }
        }
        checks.note("S-BOTH-volonly: channel 1 volume-change count (context only, not graded) = " +
            std::to_string(vol_changes));
        const Pre pre_v = sess_pre(&checks, kSBV, s_both_volonly);
        const std::string unmet_v = pre_v.unmet("S-BOTH-volonly");
        if (!pre_v.ok()) {
            not_graded(checks, "P4-VOLONLY", unmet_v);
        } else if (period_pop == 0) {
            checks.record("P4-VOLONLY [p.149,p.150]: not evaluated -- S-BOTH-volonly's "
                "complete-sample population is empty (measured population: " +
                std::to_string(period_pop) + ")");
        } else {
            checks.expect(s_both_volonly.loaded && have_bounds && period_const,
                "P4-VOLONLY [p.149,p.150]: S-BOTH-volonly -- with only bit 0 set, "
                "channel 1's period stays at D_1(200) throughout every complete sample, even "
                "though M_VP's even words are valid periods");
        }
        // 5-mod.c14 / 5-p150.c1: both halves, one line -- both bits modulate both parameters (P4-A) AND
        // bit 0 alone leaves the period alone (P4-VOLONLY).
        dep_expect(checks, p4_pre_ok && pre_v.ok(),
            p4a_ok && s_both_volonly.loaded && have_bounds && period_pop > 0 && period_const,
            "P4-JOINT [p.150] 5-mod.c14,5-p150.c1: modulating both period and volume requires both attach "
            "bits -- with both bits set, channel 1's volume and period follow M_VP (P4-A), and with only bit 0 "
            "set its period stays at D_1(200) (P4-VOLONLY)",
            std::string(p4_pre_ok ? "" : "PC1/PC2/PC3 (S-BOTH)") + ((!p4_pre_ok && !pre_v.ok()) ? " and " : "") +
            (pre_v.ok() ? "" : unmet_v));
    }

    // ================= P5: time-out phase (S-AV/S-AP/S-BOTH) ==============
    {
        // S-AP is graded by P5, P8 and P10; its PC lines are printed here, once.
        sess_pre(&checks, kSAP, sap_cap);

        Capture av = run_capture(dir + (mutating("P5-AV") ? "/prog_s_av_per6000.bin" : "/prog_s_av.bin"), budget_for(200000));
        Capture ap = mutating("P5-AP") ? run_capture(dir + "/prog_s_ap_per6000.bin", budget_for(110000))
                   : mutating("P5-AP-GRID") ? run_capture(dir + "/prog_s_ap_shift.bin", budget_for(110000)) : sap_cap;
        Capture both = mutating("P5-BOTH") ? run_capture(dir + "/prog_s_both_per6000.bin", budget_for(250000))
                     : mutating("P5-BOTH-GRID") ? run_capture(dir + "/prog_s_both_shift.bin", budget_for(110000))
                     : mutating("P5-BOTH-ALT") ? run_capture(dir + "/prog_s_both_volwords.bin", budget_for(110000))
                     : run_capture(dir + "/prog_s_both.bin", budget_for(110000));
        mut_note("P5-AV", "prog_s_av_per6000.bin", av);
        mut_note("P5-AP", "prog_s_ap_per6000.bin", ap);
        mut_note("P5-AP-GRID", "prog_s_ap_shift.bin", ap);
        mut_note("P5-BOTH", "prog_s_both_per6000.bin", both);
        mut_note("P5-BOTH-GRID", "prog_s_both_shift.bin", both);
        mut_note("P5-BOTH-ALT", "prog_s_both_volwords.bin", both);
        const Pre pre_av = sess_pre(nullptr, kSAV, av), pre_ap = sess_pre(nullptr, kSAP, ap),
                  pre_both = sess_pre(nullptr, kSBOTH, both);
        int side = side_of[1];
        // eps5 is the granularity at which a period change can first be observed (p.164: "The period counter
        // is reloaded at these transitions"): the largest reference duration in play, measured in R-PER-1.
        double eps5 = D1.count(900) ? D1[900] : (D1.count(700) ? D1[700] : 0);
        using Changes = std::vector<std::tuple<uint64_t,int,int>>;   // (time, data-set position, value)
        // Ordering (p.149: a word takes effect only at a time-out after it was fetched): for every change the
        // latest fetch of the offset of the word it carries, before the change, exists and strictly precedes it.
        auto ordered_ok = [&](Changes& changes, const std::vector<FetchRec>& fr) {
            if (changes.empty()) return false;
            for (auto& [ctime, pos, val] : changes) {
                if (pos < 0) return false;
                auto fk = latest_fetch_before(fr, (uint32_t)pos, ctime);
                if (!fk || !(*fk < ctime)) return false;
            }
            return true;
        };
        // Spacing: consecutive changes lie `step` apart (2*D_mod with one attach bit, D_mod with both: p.164),
        // to within `tol`. The time-outs lie on the period counter's own grid, which a line-quantised request
        // does not move, so a volume change (visible at once) is graded to the apparatus resolution and a
        // period change (visible at the first sample that has the new period) to eps5.
        double max_gap_dev = 0;
        auto consec_ok = [&](Changes& changes, double step, double tol) {
            if (changes.size() < 2 || step <= 0) return false;
            bool ok = true;
            max_gap_dev = 0;
            for (size_t i = 1; i < changes.size(); ++i) {
                double gap = (double)std::get<0>(changes[i]) - (double)std::get<0>(changes[i - 1]);
                max_gap_dev = std::max(max_gap_dev, std::fabs(gap - step));
                if (std::fabs(gap - step) > tol) ok = false;
            }
            return ok;
        };
        // The time-out grid (p.164): state 010 is entered when F_2 arrives, and "transitions such as
        // 010->011->010 occur whenever the period counter counts down to one", so the time-outs lie at
        // t010 + m*D_mod for m = 1, 2, ...; odd m are the 010->011 transitions and even m the 011->010
        // transitions. t010 of a session is its own F_2 (record entry r) shifted by the offset between F_2 and
        // the instant 010 was entered that PC7-CAL measured in R-MOD.
        const auto cal_rec = rec_of(cal_cap);
        const double t010_off = (cal_rec.size() > cal.r) ? cal.t010 - (double)cal_rec[cal.r].cycle : 0.0;
        // The distance from t to the nearest grid point t010 + m*D_mod with m >= 1 of the given parity
        // (0 even, 1 odd, -1 every m).
        auto grid_dist = [&](double t, double t010, int parity) -> double {
            const double step = parity < 0 ? d_mod : 2.0 * d_mod;
            const double first = t010 + (parity == 0 ? 2.0 : 1.0) * d_mod;
            double k = (double)std::llround((t - first) / step);
            if (k < 0) k = 0;
            return std::fabs(t - (first + k * step));
        };
        // Every change lies within `tol` of the grid (volume changes to the apparatus resolution, period
        // changes to eps5); `grid_max` returns the largest distance. An empty list is not on the grid.
        double grid_max = 0;
        auto grid_ok = [&](const Changes& ch, const std::vector<FetchRec>& rec, int parity, double tol) -> bool {
            grid_max = 0;
            if (ch.empty() || d_mod <= 0 || rec.size() <= cal.r || cal_rec.size() <= cal.r) return false;
            const double t010 = (double)rec[cal.r].cycle + t010_off;
            bool ok = true;
            for (auto& [ctime, pos, val] : ch) {
                const double d = grid_dist((double)ctime, t010, parity);
                grid_max = std::max(grid_max, d);
                if (d > tol) ok = false;
            }
            return ok;
        };
        auto mk_a = markers(av); auto on_a = m_on(mk_a), off_a = m_off(mk_a);
        auto mk_p = markers(ap); auto on_p = m_on(mk_p), off_p = m_off(mk_p);
        auto mk_b = markers(both); auto on_b = m_on(mk_b), off_b = m_off(mk_b);

        // The window runs from the third fetch of channel 0's own data set (by then the modulator is in
        // its main loop: p.164) to the sample the DMA stop cuts short, which is left out.
        auto win3rd = [&](Capture& c, std::optional<uint64_t> on, std::optional<uint64_t> off, uint32_t span) -> std::pair<uint64_t,uint64_t> {
            if (!on || !off) return {0, 0};
            auto fc = fetch_cycles(c.log, ch_base(0), span, *on, *off);
            if (fc.size() < 3) return {0, 0};
            return {fc[2], play_end(c.audio, side, *off)};
        };
        auto [av_ws, av_we] = win3rd(av, on_a, off_a, 8);
        auto [ap_ws, ap_we] = win3rd(ap, on_p, off_p, 4);
        auto [both_ws, both_we] = win3rd(both, on_b, off_b, 8);
        bool av_win = av_we > av_ws, ap_win = ap_we > ap_ws, both_win = both_we > both_ws;

        // "Changes" are departures from the value in force: the first segment or period run of the window
        // shows the value in force at the window's edge and is not a change, and neither is a later one that
        // merely repeats it; an unchanging channel 1 has no changes (an empty population of real events).
        auto vol_changes = [&](const Capture& cap, uint64_t ws, uint64_t we, int init, const std::function<int(int)>& posf) {
            Changes out;
            auto runs = runs_in_window(cap.audio, ws, we, side);
            auto segs = segment_by_pair(runs);
            int last_val = init;
            bool in_force = true;
            // The first segment starts at the window's edge and shows the value in force there; it is not a change.
            for (auto& s : segs) for (auto& kv : L1) if (kv.second.first == s.a && kv.second.second == s.b) {
                if (in_force) { last_val = kv.first; in_force = false; break; }
                if (kv.first != last_val) { out.push_back({s.start, posf(kv.first), kv.first}); last_val = kv.first; }
                break;
            }
            return out;
        };
        auto per_changes = [&](const Capture& cap, uint64_t ws, uint64_t we, int init, const std::function<int(int)>& posf) {
            Changes out;
            auto runs = runs_in_window(cap.audio, ws, we, side);
            auto samples = samples_by_sign(runs);
            auto comp = complete_samples(samples, ws, we);
            int last_per = init;
            bool in_force = true;
            size_t i = 0;
            while (i < comp.size()) {
                size_t j = i; uint64_t dur0 = comp[i].end - comp[i].start;
                while (j < comp.size() && (uint64_t)std::llabs((long long)(comp[j].end-comp[j].start)-(long long)dur0) <= (uint64_t)kClkRes) ++j;
                double mean = 0; for (size_t k=i;k<j;++k) mean += (double)(comp[k].end-comp[k].start); mean /= (double)(j-i);
                for (auto& kv : D1) if (std::fabs(kv.second - mean) <= kClkRes) {
                    if (in_force) { last_per = kv.first; in_force = false; break; }
                    if (kv.first != last_per) { out.push_back({comp[i].start, posf(kv.first), kv.first}); last_per = kv.first; }
                    break;
                }
                i = j;
            }
            return out;
        };
        Changes av_changes, ap_changes, both_changes, both_vol_changes, both_per_changes;
        if (av_win && side >= 0)
            av_changes = vol_changes(av, av_ws, av_we, 40, [&](int v) { return m_v_pos(v); });
        if (ap_win && side >= 0)
            ap_changes = per_changes(ap, ap_ws, ap_we, 200, [&](int v) { return m_p4_pos(v); });
        if (both_win && side >= 0) {
            both_vol_changes = vol_changes(both, both_ws, both_we, 63, [&](int v) { return m_vp_pos(false, v); });
            both_per_changes = per_changes(both, both_ws, both_we, 200, [&](int v) { return m_vp_pos(true, v); });
            both_changes = both_vol_changes;
            both_changes.insert(both_changes.end(), both_per_changes.begin(), both_per_changes.end());
            std::sort(both_changes.begin(), both_changes.end());
        }
        // F_k is looked up in the whole record of channel 0's own data set (from M_on): the word a change
        // carries may have been fetched before the window opens.
        auto fr_av = (av_win && on_a && off_a) ? fetch_records(av.log, ch_base(0), 8, *on_a, *off_a) : std::vector<FetchRec>{};
        auto fr_ap = (ap_win && on_p && off_p) ? fetch_records(ap.log, ch_base(0), 4, *on_p, *off_p) : std::vector<FetchRec>{};
        auto fr_both = (both_win && on_b && off_b) ? fetch_records(both.log, ch_base(0), 8, *on_b, *off_b) : std::vector<FetchRec>{};

        // The destination half of the AUDxAP joint line (P8): in S-AP, channel 1's applied periods
        // follow M_P4 cyclically from at least 2L = 8 changes, counted from the modulator's first restart on
        // (the first pass is graded in hrm/ch5-audio-dma-first-pass, not here). It reads the ordinary S-AP
        // capture (or its own contrast), never the capture a contrast of P5-AP replaces.
        {
            Capture dcap = mutating("P8-JOINT-AP") ? run_capture(dir + "/prog_s_ap_swap.bin", budget_for(110000)) : sap_cap;
            mut_note("P8-JOINT-AP", "prog_s_ap_swap.bin", dcap);
            auto mk = markers(dcap);
            auto on = m_on(mk), off = m_off(mk);
            Changes dc;
            bool dwin = false;
            if (on && off && side >= 0) {
                auto restart = restart_in(dcap, 0, *on, *off);
                if (restart) {
                    uint64_t we = play_end(dcap.audio, side, *off);
                    dwin = we > *restart;
                    if (dwin) dc = per_changes(dcap, *restart, we, 200, [&](int v) { return m_p4_pos(v); });
                }
            }
            sap_dest_pop = dc.size();
            std::vector<int> seqp;
            for (auto& ch : dc) seqp.push_back(std::get<2>(ch));
            sap_dest_ok = dcap.loaded && dcap.done.has_value() && dwin && seqp.size() >= 8 && cyclic_consecutive(seqp, m_p4_order);
        }

        // The start-up frame of each session (record entry r of channel 0 is F_2), for the time-out grid.
        const auto rec_av = rec_of(av), rec_ap = rec_of(ap), rec_both = rec_of(both);
        const bool av_frame = cal_ok && d_mod > 0 && rec_av.size() > cal.r + 3;
        const bool ap_frame = cal_ok && d_mod > 0 && rec_ap.size() > cal.r + 3;
        const bool both_frame = cal_ok && d_mod > 0 && rec_both.size() > cal.r + 3;
        max_gap_dev = 0;
        grid_max = 0;
        // The volume-output grid is measured, not graded: the target volume latch is not observable at nanomig.v,
        // and no cited page fixes the latency from the target volume write to the external output.
        const bool av_grid = av_frame && grid_ok(av_changes, rec_av, 0, kClkRes);
        const double av_grid_max = grid_max;
        const bool av_ok = av_changes.size() >= 8 && ordered_ok(av_changes, fr_av) && consec_ok(av_changes, 2.0 * d_mod, kClkRes);
        const double av_dev = max_gap_dev;
        max_gap_dev = 0;
        grid_max = 0;
        const bool ap_grid = ap_frame && grid_ok(ap_changes, rec_ap, 1, eps5);
        const double ap_grid_max = grid_max;
        const bool ap_ok = ap_changes.size() >= 8 && ordered_ok(ap_changes, fr_ap) && consec_ok(ap_changes, 2.0 * d_mod, eps5) &&
                           ap_grid;
        const double ap_dev = max_gap_dev;
        max_gap_dev = 0;
        grid_max = 0;
        const bool both_vol_grid = both_frame && (both_vol_changes.empty() || grid_ok(both_vol_changes, rec_both, -1, kClkRes));
        const double both_vol_max = grid_max;
        (void)both_vol_grid;
        grid_max = 0;
        const bool both_per_grid = both_frame && (both_per_changes.empty() || grid_ok(both_per_changes, rec_both, -1, eps5));
        const double both_per_max = grid_max;
        // p.150: with both attach bits the words alternate volume and period. The population of each kind is
        // required (the data set holds four volume and four period words: one full cycle of each), and the
        // kinds of the changes, in time, alternate strictly, each change one D_mod after the one before.
        std::vector<std::pair<uint64_t, int>> both_kinds;   // (time, 0 volume / 1 period)
        for (auto& ch : both_vol_changes) both_kinds.push_back({std::get<0>(ch), 0});
        for (auto& ch : both_per_changes) both_kinds.push_back({std::get<0>(ch), 1});
        std::sort(both_kinds.begin(), both_kinds.end());
        size_t both_alt_breaks = 0;
        for (size_t i = 1; i < both_kinds.size(); ++i)
            if (both_kinds[i].second == both_kinds[i - 1].second) ++both_alt_breaks;
        const bool both_pop = both_vol_changes.size() >= 4 && both_per_changes.size() >= 4;
        const bool both_alt = both_pop && both_kinds.size() >= 8 && both_alt_breaks == 0;
        const bool both_ok = both_changes.size() >= 8 && ordered_ok(both_changes, fr_both) && consec_ok(both_changes, d_mod, eps5) &&
                             both_per_grid && both_alt;
        const double both_dev = max_gap_dev;
        checks.note("P5: S-AV " + std::to_string(av_changes.size()) + " changes (largest deviation of a gap from 2*D_mod: " +
            std::to_string(av_dev) + " clk; largest distance of a change from the even time-out grid: " +
            std::to_string(av_grid_max) + " clk), S-AP " + std::to_string(ap_changes.size()) + " (" + std::to_string(ap_dev) +
            " clk; largest distance from the odd time-out grid: " + std::to_string(ap_grid_max) +
            " clk), S-BOTH " + std::to_string(both_changes.size()) + " (largest deviation of a gap from D_mod: " +
            std::to_string(both_dev) + " clk; largest distance from the time-out grid, volume changes " +
            std::to_string(both_vol_max) + " clk, period changes " + std::to_string(both_per_max) +
            " clk); eps = " + std::to_string(eps5) + " clk");
        checks.note("P5-BOTH: " + std::to_string(both_vol_changes.size()) + " volume changes and " +
            std::to_string(both_per_changes.size()) + " period changes (need >=4 of each), " +
            std::to_string(both_alt_breaks) + " places where two consecutive changes are of one kind (need 0)");
        // The delay between a request and its fetch is given by the page as one line plus about 14 clock
        // cycles (p.164) and not as a bound, so the offset of a change from the fetch of its word is
        // measured, not graded.
        {
            double dmin = 1e18, dmax = -1e18;
            auto span = [&](Changes& ch, const std::vector<FetchRec>& fr) {
                for (auto& [ctime, pos, val] : ch) {
                    if (pos < 0) continue;
                    auto fk = latest_fetch_before(fr, (uint32_t)pos, ctime);
                    if (!fk) continue;
                    double dl = (double)ctime - (double)*fk;
                    dmin = std::min(dmin, dl); dmax = std::max(dmax, dl);
                }
            };
            span(av_changes, fr_av); span(ap_changes, fr_ap); span(both_changes, fr_both);
            if (dmin <= dmax)
                checks.note("P5: C - F_k, the delay from the fetch of a word to its change, over the three sessions: " +
                    std::to_string(dmin) + " .. " + std::to_string(dmax) + " clk (not graded: the page gives the request-to-data "
                    "delay only as one line plus about 14 clock cycles)");
        }
        if (!pre_av.ok() || !av_frame) {
            not_graded(checks, "P5-AV", std::string(pre_av.ok() ? "" : pre_av.unmet("S-AV")) + ((!pre_av.ok() && !av_frame) ? " and " : "") +
                (av_frame ? "" : "PC7-CAL and a start-up frame in S-AV"));
        } else {
            checks.expect(av_ok,
                "P5-AV [p.149,p.164]: S-AV -- the window holds at least 2L=8 applied changes of channel 1, "
                "each after the fetch of its word, and consecutive changes are 2*D_mod apart");
            checks.note("P5-AV volume-output grid, not graded: the largest distance of a volume change on the "
                "output from the 011->010 time-out grid of the calibrated state 010 (even time-outs) is " +
                std::to_string(av_grid_max) + " clk over " + std::to_string(av_changes.size()) + " changes; the target "
                "volume latch is not observable at nanomig.v and no cited page fixes the latency from the "
                "target volume write to the external output");
        }
        if (!pre_ap.ok() || !ap_frame) {
            not_graded(checks, "P5-AP", std::string(pre_ap.ok() ? "" : pre_ap.unmet("S-AP")) + ((!pre_ap.ok() && !ap_frame) ? " and " : "") +
                (ap_frame ? "" : "PC7-CAL and a start-up frame in S-AP"));
        } else {
            checks.expect(ap_ok,
                "P5-AP [p.164,p.165] 5-p165.c18,5-sig.pbufld2: S-AP -- the window holds at least 2L=8 applied "
                "changes of channel 1, each after the fetch of its word, consecutive changes are 2*D_mod apart, "
                "and every change lies within eps of the 010->011 time-out grid of the calibrated state 010 "
                "(odd time-outs)");
        }
        if (!pre_both.ok() || !both_frame) {
            not_graded(checks, "P5-BOTH", std::string(pre_both.ok() ? "" : pre_both.unmet("S-BOTH")) + ((!pre_both.ok() && !both_frame) ? " and " : "") +
                (both_frame ? "" : "PC7-CAL and a start-up frame in S-BOTH"));
        } else {
            checks.expect(both_ok,
                "P5-BOTH [p.149,p.150,p.164] 5-mod.c10,5-p149.c16: S-BOTH -- the window holds at least 2L=8 applied "
                "changes of channel 1, at least four of volume and four of period, each after the fetch of its "
                "word, consecutive changes are D_mod apart and strictly alternate volume, period, volume, "
                "period, and every period change (to eps) lies on the time-out grid of the calibrated state 010 "
                "(both transitions)");
            checks.note("P5-BOTH volume-output grid, not graded: the largest distance of a volume change on the "
                "output from the time-out grid of the calibrated state 010 is " + std::to_string(both_vol_max) +
                " clk over " + std::to_string(both_vol_changes.size()) + " changes; the target volume latch is not "
                "observable at nanomig.v and no cited page fixes the latency from the target volume write to the "
                "external output");
        }
    }

    // ================= P6: each attach bit pairs x with x+1 ================
    {
        // swap: P6-A's contrast. tgt: P6-B's contrast (bits 0 and 4). attc / attd: P6-C's and P6-D's contrasts,
        // each a second attach bit that Table 5-5 makes route a tripwire's data to the next channel (attc to
        // its volume, attd to its period); PC1 then expects the combined mask (attc_mask / attd_mask).
        struct Pair6 { int b; int mod_ch, tgt_ch, trip_a, trip_b; bool is_vol; std::string bin, swap, tgt, attc, attd;
                       uint16_t attc_mask, attd_mask; };
        std::vector<Pair6> cases = {
            {0,0,1,2,3,true,"/prog_s_pair0.bin","/prog_s_pair0_swap.bin","/prog_s_pair0_tgtper.bin","/prog_s_pair0_attc.bin","",0x05,0},
            {1,1,2,0,3,true,"/prog_s_pair1.bin","/prog_s_pair1_swap.bin","","/prog_s_pair1_attc.bin","",0x06,0},
            {2,2,3,0,1,true,"/prog_s_pair2.bin","/prog_s_pair2_swap.bin","","/prog_s_pair2_attc.bin","",0x05,0},
            {4,0,1,2,3,false,"/prog_s_pair4.bin","/prog_s_pair4_swap.bin","/prog_s_pair4_tgtvol.bin","/prog_s_pair4_attc.bin","/prog_s_pair4_attd.bin",0x14,0x50},
            {5,1,2,0,3,false,"/prog_s_pair5.bin","/prog_s_pair5_swap.bin","","/prog_s_pair5_attc.bin","/prog_s_pair5_attd.bin",0x24,0x30},
            {6,2,3,0,1,false,"/prog_s_pair6.bin","/prog_s_pair6_swap.bin","","/prog_s_pair6_attc.bin","/prog_s_pair6_attd.bin",0x41,0x50},
        };
        // Claim ids per bit (Table 5-5 / p.265), matching the bit numbers the ids' own quotes name.
        // Bits 0 and 4 carry theirs on P1-A and P2-A.
        std::map<int, std::string> bit_ids = {
            {1, "5-p151.c7,5-t55.b1,5-appA-p265.c28,A-adkcon.b01"},
            {2, "5-p151.c6,5-t55.b2,5-appA-p265.c27,A-adkcon.b02"},
            {5, "5-p151.c3,5-t55.b5,5-appA-p265.c24,A-adkcon.b05"},
            {6, "5-p151.c2,5-t55.b6,5-appA-p265.c23,A-adkcon.b06"},
        };
        checks.record("5-mod.c19, 5-p150.c22: 'a predetermined order' is used only in part (the "
            "six fixed pairs are checked; that no other pairing can be selected is not, see "
            "spec's partial-claims table).");
        checks.record("5-mod.c22, 5-p150.c25: the sentence has two halves -- the channel's output is disabled "
            "(P8) and it is attached to the next higher channel (P6-A) -- and no single line here "
            "establishes both, so neither id is asserted.");
        for (auto& pc : cases) {
            std::string tag = "P6-" + std::to_string(pc.b);
            const bool m_attc = mutating(tag + "-ATTC") && !pc.attc.empty(), m_attd = mutating(tag + "-ATTD") && !pc.attd.empty();
            std::string bin = mutating(tag) ? pc.swap
                            : m_attc ? pc.attc : m_attd ? pc.attd
                            : (mutating("P6-TGT-" + std::to_string(pc.b)) && !pc.tgt.empty()) ? pc.tgt : pc.bin;
            Capture c = run_capture(dir + bin, budget_for(110000));
            mut_note(tag, bin, c);
            mut_note(tag + "-ATTC", bin, c);
            mut_note(tag + "-ATTD", bin, c);
            mut_note("P6-TGT-" + std::to_string(pc.b), bin, c);
            int tside = side_of[pc.tgt_ch], oside = other_side(pc.tgt_ch);
            uint16_t bitval = m_attc ? pc.attc_mask : m_attd ? pc.attd_mask : (uint16_t)(1u << pc.b);
            auto mk = markers(c); auto on = m_on(mk), off = m_off(mk);
            bool have_bounds = on && off;
            // Length per channel: the modulator (M6V/M_P4, 4 words) and the
            // target (SQ, 4 words) both step through 4; the two tripwires
            // (W_V/W_P) step through 2.
            uint32_t len_by_ch[4] = {2, 2, 2, 2};
            len_by_ch[pc.mod_ch] = 4;
            len_by_ch[pc.tgt_ch] = 4;
            const Pre pre = run_pre(&checks, tag, c, bitval, 0x000F,
                         {{0,len_by_ch[0]},{1,len_by_ch[1]},{2,len_by_ch[2]},{3,len_by_ch[3]}},
                         have_bounds, have_bounds ? *on : 0, have_bounds ? *off : 0);
            const std::string unmet = pre.unmet("S-PAIR-" + std::to_string(pc.b)) + ", bit " + std::to_string(pc.b);

            // The window runs from the modulator's first restart (the first pass is graded in
            // hrm/ch5-audio-dma-first-pass, not here) to the end of play of the target's output.
            auto mod_restart = have_bounds ? restart_in(c, pc.mod_ch, *on, *off) : std::optional<uint64_t>{};
            uint64_t win_start = 0; bool win_ok = have_bounds && mod_restart.has_value();
            if (win_ok) win_start = *mod_restart;
            // Samples, runs and levels of the target's output end before the sample the DMA stop
            // cuts short; the fetch records and the constant outputs end at M_off.
            uint64_t win_end = have_bounds ? play_end(c.audio, tside, *off) : 0;
            uint64_t fetch_end = have_bounds ? *off : 0;
            if (win_ok && win_end <= win_start) win_ok = false;

            std::vector<int> applied; bool level_ok = true;
            auto& table = (pc.tgt_ch == 1) ? L1 : (pc.tgt_ch == 2) ? L2 : L3;
            auto& dtable = (pc.tgt_ch == 1) ? D1 : (pc.tgt_ch == 2) ? D2t : D3t;
            if (win_ok && tside >= 0) {
                auto runs = runs_in_window(c.audio, win_start, win_end, tside);
                if (pc.is_vol) {
                    // The first segment shows the value in force at the window's edge, applied before the
                    // window opened: it is not an applied change.
                    auto segs = segment_by_pair(runs);
                    bool in_force = true;
                    for (auto& s : segs) {
                        int found = -1;
                        for (auto& kv : table) if (kv.second.first == s.a && kv.second.second == s.b) { found = kv.first; break; }
                        if (found < 0) level_ok = false;
                        if (in_force) { in_force = false; continue; }
                        applied.push_back(found);
                    }
                } else {
                    // The durations of a period session are graded on sign-alternation samples (see
                    // Measurements), so a volume written to the target halfway through a sample does not
                    // split it.
                    auto samples = samples_by_sign(runs);
                    auto comp = complete_samples(samples, win_start, win_end);
                    size_t i = 0;
                    bool in_force = true;
                    while (i < comp.size()) {
                        size_t j = i; uint64_t dur0 = comp[i].end - comp[i].start;
                        while (j < comp.size() && (uint64_t)std::llabs((long long)(comp[j].end-comp[j].start)-(long long)dur0) <= (uint64_t)kClkRes) ++j;
                        double mean = 0; for (size_t k=i;k<j;++k) mean += (double)(comp[k].end-comp[k].start); mean /= (double)(j-i);
                        int found = -1;
                        for (auto& kv : dtable) if (std::fabs(kv.second - mean) <= kClkRes) { found = kv.first; break; }
                        if (found < 0) level_ok = false;
                        if (in_force) in_force = false; else applied.push_back(found);
                        i = j;
                    }
                }
            }
            std::vector<int> order6 = pc.is_vol ? std::vector<int>{8,48,20,60} : m_p4_order;
            bool cyclic_ok = win_ok && applied.size() >= 9 && level_ok && cyclic_consecutive(applied, order6);
            checks.note("P6 bit " + std::to_string(pc.b) + ": the window has " + std::to_string(applied.size()) +
                (pc.is_vol ? " applied volumes" : " applied periods") + " (need >=9)");
            std::string ids = bit_ids.count(pc.b) ? bit_ids[pc.b] : "";
            p6a_gate[pc.b] = pre.ok();
            p6a_ok[pc.b] = pre.ok() && c.loaded && win_ok && cyclic_ok;
            dep_expect(checks, pre.ok(), c.loaded && win_ok && cyclic_ok,
                pc.is_vol
                ? ("P6-A [p.149,p.150,p.151,p.265]" + (ids.empty() ? "" : (" " + ids)) + " bit " + std::to_string(pc.b) +
                   ": the target's applied volumes are cyclically consecutive in M6V, every level "
                   "pair a reference pair of the target")
                : ("P6-A [p.149,p.150,p.151,p.265]" + (ids.empty() ? "" : (" " + ids)) + " bit " + std::to_string(pc.b) +
                   ": the target's applied periods are cyclically consecutive in M_P4"), unmet);

            // (b): durations (volume sessions) / level pair (period
            // sessions) stay at their own reference, and per-session
            // preconditions (already added above via run_pre).
            bool b_ok = false;
            if (win_ok && tside >= 0) {
                auto runs = runs_in_window(c.audio, win_start, win_end, tside);
                if (pc.is_vol) {
                    auto samples = samples_by_sign(runs);
                    auto comp = complete_samples(samples, win_start, win_end);
                    b_ok = !comp.empty();
                    for (auto& s : comp)
                        if (dtable.count(200) && std::fabs((double)(s.end - s.start) - dtable[200]) > kClkRes) { b_ok = false; break; }
                } else {
                    auto segs = segment_by_pair(runs);
                    auto pr40 = table.count(40) ? table[40] : std::make_pair<uint16_t,uint16_t>(0,0);
                    b_ok = !segs.empty();
                    for (auto& s : segs) if (s.a != pr40.first || s.b != pr40.second) { b_ok = false; break; }
                }
            }
            dep_expect(checks, pre.ok(), c.loaded && win_ok && b_ok,
                pc.is_vol
                ? ("P6-B [p.149,p.150] bit " + std::to_string(pc.b) + ": the target's durations stay at D_c(200)")
                : ("P6-B [p.149,p.150] bit " + std::to_string(pc.b) + ": the target's level pair stays at the reference for VOL 40"), unmet);

            // (c) uses win_start (the modulator's first restart), not M_on.
            bool other_const = win_ok && oside >= 0 && fetch_end > win_start && is_constant(c.audio, win_start, fetch_end, oside);
            dep_expect(checks, pre.ok(), c.loaded && win_ok && other_const,
                "P6-C [p.150] bit " + std::to_string(pc.b) + ": the other side's output is constant "
                "for the whole window", unmet);

            // (d): every channel but the target keeps fetching at 2*AUDxPER as written: the modulator
            // (4 words, PER 4000) and both tripwires (2 words, PER 1400..2000).
            bool spacing_ok = true;
            if (win_ok) {
                auto check_trip = [&](int ch, uint32_t span, double per) {
                    auto fc = fetch_cycles(c.log, ch_base(ch), span, win_start, fetch_end);
                    if (fc.size() < 2) return false;
                    for (size_t i = 1; i < fc.size(); ++i)
                        if (!(std::fabs((double)(fc[i]-fc[i-1]) - 2*ticks(per)) < ell)) return false;
                    return true;
                };
                double pers[4] = {1400,1600,1800,2000};
                if (!check_trip(pc.mod_ch, 4, 4000)) spacing_ok = false;
                if (!check_trip(pc.trip_a, 2, pers[pc.trip_a])) spacing_ok = false;
                if (!check_trip(pc.trip_b, 2, pers[pc.trip_b])) spacing_ok = false;
            }
            dep_expect(checks, pre.ok(), c.loaded && win_ok && spacing_ok,
                "P6-D [p.164] bit " + std::to_string(pc.b) + ": every non-target channel's fetch spacing (the "
                "modulator's and both tripwires', from the modulator's first restart) equals 2*AUDxPER to within less "
                "than one line",
                unmet);
        }
        // 5-mod.c20 and 5-p150.c23 say that setting a channel's attach bit to 1 attaches it as a modulator,
        // for every attach bit: one line holds all eight cases the package can show. Bit 0 (volume, P1-A) and
        // bit 4 (period, P2-A) come from their sessions, bits 1, 2, 5 and 6 from P6-A. Bits 3 and 7 attach
        // nothing (p.151) and are P7's.
        {
            bool all_gate = p1a_gate && p2a_gate, all_ok = p1a_ok && p2a_ok;
            for (int b : {1, 2, 5, 6}) { all_gate = all_gate && p6a_gate[b]; all_ok = all_ok && p6a_ok[b]; }
            dep_expect(checks, all_gate, all_ok,
                "P6-JOINT [p.150,p.151] 5-mod.c20,5-p150.c23: setting an attach bit to 1 attaches the channel as a "
                "modulator, for each of the six bits 0, 1, 2 (volume) and 4, 5, 6 (period): the next channel's "
                "volume (P1-A for bit 0, P6-A for bits 1 and 2) or period (P2-A for bit 4, P6-A for bits 5 and 6) "
                "follows the channel's data words in every session",
                "PC1-PC3 of S-AV, S-P2 or an S-PAIR session");
        }
    }

    // ================= P7: channel 3 attaches to nothing ===================
    {
        // Per-bit ids: 5-appA-p265.c26/A-adkcon.b03 name ATVOL3 (bit 3), c22/b07 name ATPER3 (bit 7).
        // Bit 3 modulates a volume, which P7-CONST sees (every channel is at VOL 0, so a modulated
        // volume makes an output move); bit 7 modulates a period, which P7-SPACING sees (a modulated
        // period changes a fetch spacing) and P7-CONST cannot. The A-adkcon ids bundle "modulates nothing"
        // with "disables channel 3's audio output": they sit on the joint lines printed after P8.
        // The contrast writes the attach bit that Table 5-5 makes route channel 2's data to channel 3 (bit 2,
        // volume; bit 6, period) in place of channel 3's own; PC1 then expects that mask (mutmask).
        struct P7Case { std::string bin, mutbin; uint16_t mask, mutmask; std::string tag, const_ids, spacing_ids; };
        std::vector<P7Case> cases = {
            {"/prog_s_ch3v.bin", "/prog_s_ch3v_att.bin", 0x08, 0x04, "P7V", "5-appA-p265.c26", ""},
            {"/prog_s_ch3p.bin", "/prog_s_ch3p_att.bin", 0x80, 0x40, "P7P", "", "5-appA-p265.c22"},
        };
        checks.record("5-mod.c23, 5-p150.c26: the sentence says an attached channel always modulates the next "
            "higher channel, so channel 3 cannot be attached, and p.150 adds that its modulate bits only disable "
            "its audio output; the lines below grade that nothing is modulated, and the output half is P8's, so "
            "neither id is asserted.");
        size_t bit_idx = 0;
        for (auto& pc : cases) {
            std::string bin = mutating(pc.tag) ? pc.mutbin : pc.bin;
            Capture c = run_capture(dir + bin, budget_for(60000));
            mut_note(pc.tag, bin, c);
            uint16_t expect = mutating(pc.tag) ? pc.mutmask : pc.mask;
            auto mk = markers(c); auto on = m_on(mk), off = m_off(mk);
            bool have_bounds = on && off;
            const Pre pre = run_pre(&checks, pc.tag, c, expect, 0x000F, {{0,2},{1,2},{2,2},{3,2}}, have_bounds,
                                    have_bounds ? *on : 0, have_bounds ? *off : 0);
            const std::string unmet = pre.unmet(pc.tag);
            auto ch3f = have_bounds ? fetch_cycles(c.log, ch_base(3), 2, *on, *off) : std::vector<uint64_t>{};
            uint64_t win_start = have_bounds && ch3f.size() >= 3 ? ch3f[2] : 0;
            uint64_t win_end = have_bounds ? *off : 0;
            bool win_ok = have_bounds && ch3f.size() >= 3 && win_end > win_start;
            bool both_const = win_ok && is_constant(c.audio, win_start, win_end, 0) && is_constant(c.audio, win_start, win_end, 1);
            dep_expect(checks, pre.ok(), c.loaded && win_ok && both_const,
                "P7-CONST [p.150,p.151,p.265]" + (pc.const_ids.empty() ? "" : (" " + pc.const_ids)) + ": both "
                "outputs are constant for the whole window -- channel 3's attach bit modulates "
                "nothing", unmet);
            bool spacing_ok = true;
            double pers[4] = {1400,1600,1800,2000};
            if (win_ok) {
                for (int ch = 0; ch < 4; ++ch) {
                    auto fc = fetch_cycles(c.log, ch_base(ch), 2, win_start, win_end);
                    if (fc.size() < 2) { spacing_ok = false; continue; }
                    for (size_t i = 1; i < fc.size(); ++i)
                        if (!(std::fabs((double)(fc[i]-fc[i-1]) - 2*ticks(pers[ch])) < ell)) spacing_ok = false;
                }
            }
            dep_expect(checks, pre.ok(), c.loaded && win_ok && spacing_ok,
                "P7-SPACING [p.164]" + (pc.spacing_ids.empty() ? "" : (" " + pc.spacing_ids)) + ": every channel's "
                "consecutive fetch spacing equals 2*AUDxPER to within less than one line (2800,3200,3600,4000 ticks)", unmet);
            p7_pre[bit_idx] = pre;
            p7_const_ok[bit_idx] = c.loaded && win_ok && both_const;
            p7_spacing_ok[bit_idx] = c.loaded && win_ok && spacing_ok;
            ++bit_idx;
        }
    }

    // ================= P8: an attached channel's own DAC is bypassed ======
    {
        // The unattached controls -- the same channel, same data, period
        // and volume, changes its own side >=2x per data word (P8-PRE). Their verdicts gate the P8-ATT
        // line of the attached session each one belongs to.
        struct CtrlCase { int ch; const Capture* c; uint32_t len; std::string name; const SessSpec* spec; };
        Capture rmod_ctrl = mutating("P8-PRE") ? run_capture(dir + "/prog_rmod_vol0.bin", budget_for(200000)) : rmod;
        mut_note("P8-PRE", "prog_rmod_vol0.bin", rmod_ctrl);
        std::vector<CtrlCase> ctrls = {
            {0, &rmod_ctrl, 8, "R-MOD", &kRMOD}, {0, &rdacp4, 4, "R-DAC-P4", &kRDACP4}, {0, &rdacvp, 8, "R-DAC-VP", &kRDACVP},
            {3, &rdac3v, 2, "R-DAC3-V", &kRDAC3V}, {3, &rdac3p, 2, "R-DAC3-P", &kRDAC3P},
        };
        bool ctrl_pass[5] = {false, false, false, false, false};
        for (size_t k = 0; k < ctrls.size(); ++k) {
            auto& cc = ctrls[k];
            auto mk = markers(*cc.c); auto on = m_on(mk), off = m_off(mk);
            bool have_bounds = on && off;
            int side = side_of[cc.ch];
            bool changes_enough = false;
            if (have_bounds && side >= 0) {
                auto runs = runs_in_window(cc.c->audio, *on, *off, side);
                auto comp = complete_runs(runs, *on, *off);
                changes_enough = comp.size() >= 2 * cc.len;
            }
            const Pre pre_c = sess_pre(nullptr, *cc.spec, *cc.c);
            ctrl_pass[k] = pre_c.ok() && cc.c->loaded && cc.c->done.has_value() && have_bounds && changes_enough;
            dep_expect(checks, pre_c.ok(), cc.c->loaded && cc.c->done.has_value() && have_bounds && changes_enough,
                "P8-PRE [reference frame]: " + cc.name + " -- the same channel, unattached, "
                "changes its own side at least twice per data word", pre_c.unmet(cc.name));
        }

        struct P8Case { int ch; std::string bin; const SessSpec* spec; size_t ctrl; };
        std::vector<P8Case> cases = {
            {0, "/prog_s_av.bin",   &kSAV,   0},
            {0, "/prog_s_ap.bin",   &kSAP,   1},
            {0, "/prog_s_both.bin", &kSBOTH, 2},
            {3, "/prog_s_dac3v.bin", &kDAC3V, 3},
            {3, "/prog_s_dac3p.bin", &kDAC3P, 4},
        };
        bool sap_src_const = false;
        bool att3_ok[2] = {false, false};   // S-DAC3-V, S-DAC3-P: the P8-ATT comparator holds
        bool att3_gate[2] = {false, false}; // PC1-PC3 of the session and its P8-PRE control hold
        for (auto& pc : cases) {
            const bool omit = mutating("P8-OMIT");
            // The session itself, as every other section grades it; S-AV's, S-AP's and S-BOTH's PC lines are
            // printed where those sessions are graded, S-DAC3-V's and S-DAC3-P's here.
            Capture c_att = (pc.bin == "/prog_s_ap.bin") ? sap_cap : run_capture(dir + pc.bin, budget_for(200000));
            const bool own_pc = (pc.spec == &kDAC3V || pc.spec == &kDAC3P);
            const Pre pre_att = sess_pre(own_pc ? &checks : nullptr, *pc.spec, c_att);
            // The contrast omits the attach write: the matching unattached reference session is graded in
            // the attached one's place.
            const Capture& c = omit ? *ctrls[pc.ctrl].c : c_att;
            int side = side_of[pc.ch];
            auto mk = markers(c); auto on = m_on(mk), off = m_off(mk);
            bool have_bounds = on && off;
            auto fc = have_bounds ? fetch_cycles(c.log, ch_base(pc.ch), pc.ch == 0 ? 17 : 2, *on, *off) : std::vector<uint64_t>{};
            uint64_t win_start = have_bounds && fc.size() >= 3 ? plus_La(fc[2]) : 0;
            uint64_t win_end = have_bounds ? play_end(c.audio, side, *off) : 0;
            bool win_ok = have_bounds && fc.size() >= 3 && side >= 0 && win_end > win_start;
            bool constant = win_ok && is_constant(c.audio, win_start, win_end, side);
            if (pc.bin == "/prog_s_ap.bin") sap_src_const = constant;
            if (pc.spec == &kDAC3V) { att3_ok[0] = c.loaded && win_ok && constant; att3_gate[0] = pre_att.ok() && ctrl_pass[pc.ctrl]; }
            if (pc.spec == &kDAC3P) { att3_ok[1] = c.loaded && win_ok && constant; att3_gate[1] = pre_att.ok() && ctrl_pass[pc.ctrl]; }
            mut_note("P8-OMIT", pc.bin + " replaced by its unattached reference session", c);
            const bool gate = pre_att.ok() && ctrl_pass[pc.ctrl];
            dep_expect(checks, gate, c.loaded && win_ok && constant,
                "P8-ATT [p.165,p.151,p.265] channel " + std::to_string(pc.ch) +
                "'s own side has one value for the whole window while it is attached (" + pc.bin + ")",
                std::string(pre_att.ok() ? "" : pre_att.unmet(pc.spec->name)) + ((!pre_att.ok() && !ctrl_pass[pc.ctrl]) ? " and " : "") +
                (ctrl_pass[pc.ctrl] ? "" : "P8-PRE (" + ctrls[pc.ctrl].name + ")"));
        }
        // The AUDxAP joint line (5-p165.c20, 5-sig.audxap): both halves of "send data to the period
        // latch of the next channel instead of to the D->A converter", in S-AP -- channel 1's period
        // follows channel 0's words (destination, from the modulator's first restart) and channel 0's own
        // side holds one value (source).
        {
            const Pre pre_sap = sess_pre(nullptr, kSAP, sap_cap);
            dep_expect(checks, pre_sap.ok() && ctrl_pass[1], sap_dest_ok && sap_src_const,
                "P8-JOINT-AP [p.165]: in S-AP, channel 1's applied periods follow "
                "channel 0's words cyclically in M_P4 (destination half, measured applied-period changes: " +
                std::to_string(sap_dest_pop) + ") AND channel 0's own side holds one value once its data is "
                "flowing (source half) -- both halves of the AUDxAP claim, one session",
                std::string(pre_sap.ok() ? "" : pre_sap.unmet("S-AP")) + ((!pre_sap.ok() && !ctrl_pass[1]) ? " and " : "") +
                (ctrl_pass[1] ? "" : "P8-PRE (R-DAC-P4)"));
            checks.record("5-p165.c20, 5-sig.audxap: the sentence is generic in the channel x. Its two halves are held "
                "in one session for channel 0 only (P8-JOINT-AP). Channel 1's converter half cannot be observed in a "
                "session that also shows its destination: channels 1 and 2 share one output. For channel 2 the "
                "destination half is P6-A (bit 6), whose modulator is silent at volume 0, and no session holds its "
                "converter half. The claim quantifies over every x, so neither id is asserted.");
        }
        // Channel 3's two bits, each bundling "modulates nothing" (P7) with "disables channel 3's audio
        // output" (P8-ATT): one joint line per bit.
        {
            auto joint_unmet = [&](size_t k, const std::string& p7name, const std::string& p8name) {
                return std::string(p7_pre[k].ok() ? "" : p7_pre[k].unmet(p7name)) +
                       ((!p7_pre[k].ok() && !att3_gate[k]) ? " and " : "") + (att3_gate[k] ? "" : "PC1-PC3 and P8-PRE of " + p8name);
            };
            dep_expect(checks, p7_pre[0].ok() && att3_gate[0], p7_const_ok[0] && att3_ok[0],
                "P7-JOINT-V [p.150,p.151,p.265] A-adkcon.b03: channel 3's attach-volume bit modulates nothing "
                "(P7-CONST, S-CH3-V) AND disables channel 3's audio output (P8-ATT, S-DAC3-V) -- both halves "
                "of the claim", joint_unmet(0, "P7V", "S-DAC3-V"));
            dep_expect(checks, p7_pre[1].ok() && att3_gate[1], p7_spacing_ok[1] && att3_ok[1],
                "P7-JOINT-P [p.150,p.151,p.265] A-adkcon.b07: channel 3's attach-period bit modulates nothing "
                "(P7-SPACING, S-CH3-P) AND disables channel 3's audio output (P8-ATT, S-DAC3-P) -- both halves "
                "of the claim", joint_unmet(1, "P7P", "S-DAC3-P"));
        }
    }

    // ================= P9: data-set lengths are independent ===============
    {
        struct P9Case { std::string bin; std::vector<int> mod_order; int lm, lc; const SessSpec* spec; };
        std::vector<P9Case> cases = {
            {"/prog_s_lena.bin", {8,48,20,60,32}, 5, 3, &kSLENA},
            {"/prog_s_lenb.bin", {8,48}, 2, 7, &kSLENB},
        };
        for (auto& pc : cases) {
            bool is_a = pc.spec == &kSLENA;
            // The contrasts change the stimulus of S-LEN-A only; every line below grades with the page's
            // own lengths (L_m = 5, L_c = 3) and the page's data set.
            std::string bin = pc.bin;
            if (is_a) {
                if (mutating("P9-A")) bin = "/prog_s_lena_len10.bin";
                else if (mutating("P9-B")) bin = "/prog_s_lena_ch1len2.bin";
                else if (mutating("P9-C")) bin = "/prog_s_lena_swap.bin";
                else if (mutating("P9-D")) bin = "/prog_s_lena_per1write.bin";
                else if (mutating("P9-WINDOW")) bin = "/prog_s_lena_ch1len1000.bin";
            }
            Capture c = run_capture(dir + bin, budget_for(150000));
            if (is_a) mut_note("P9-A", bin, c);
            if (is_a) mut_note("P9-B", bin, c);
            if (is_a) mut_note("P9-C", bin, c);
            if (is_a) mut_note("P9-D", bin, c);
            if (is_a) mut_note("P9-WINDOW", bin, c);
            int side = side_of[1];
            auto mk = markers(c); auto on = m_on(mk), off = m_off(mk);
            bool have_bounds = on && off;
            // PC1, PC2 and PC3 are printed for the session as its ordinary program builds it. PC3's stepping
            // is the check P9-A (channel 0) and P9-B (channel 1) make from the later of the two restarts, so
            // a contrast of those lines replaces the session for P9-A to P9-D only and PC3 stays that of the
            // ordinary program.
            Capture c_ord = run_capture(dir + pc.bin, budget_for(150000));
            const Pre pre = sess_pre(&checks, *pc.spec, c_ord);
            const std::string which = pc.spec->name;
            // The window opens after EACH channel's own first restart (PC3's
            // own anchor, SPECIFICATION.md) -- independent of any value
            // either channel plays. A scan for "departure from 40" is fooled
            // by a first-pass overrun on channel 1's own data region
            // (graded in hrm/ch5-audio-dma-first-pass, not here), which can
            // spuriously open the window inside that overrun.
            auto restart0 = have_bounds ? restart_in(c, 0, *on, *off) : std::optional<uint64_t>{};
            auto restart1 = have_bounds ? restart_in(c, 1, *on, *off) : std::optional<uint64_t>{};
            uint64_t win_start = (restart0 && restart1) ? std::max(*restart0, *restart1) : 0;
            uint64_t win_end = have_bounds ? play_end(c.audio, side, *off) : 0;
            bool win_ok = have_bounds && restart0.has_value() && restart1.has_value() && win_end > win_start;

            // The offsets of each channel are read in its own 4 KB region, so that a fetch beyond the
            // channel's own length is seen.
            auto off0 = win_ok ? fetch_offsets(c.log, ch_base(0), kChannelRegionWords, win_start, *off) : std::vector<uint32_t>{};
            auto off1 = win_ok ? fetch_offsets(c.log, ch_base(1), kChannelRegionWords, win_start, *off) : std::vector<uint32_t>{};
            // Grade the stepping itself (previous+1 mod L), from the
            // channel's own first restart on.
            bool off0_ok = stepping_cycles(off0, (uint32_t)pc.lm) >= 0;
            bool off1_ok = stepping_cycles(off1, (uint32_t)pc.lc) >= 0;
            bool window_met = win_ok && !off0.empty() && !off1.empty();
            if (window_met) {
                checks.expect(true, "P9-WINDOW [p.147] " + which + ": the window opens after each channel's own first "
                    "restart, with fetches recorded for both channels");
            } else {
                checks.record("P9-WINDOW [p.147] " + which + ": precondition not met -- no window after each "
                    "channel's own first restart with fetches recorded for both channels; P9-A to P9-D print NOT GRADED");
            }
            const bool gate = window_met && pre.ok();
            const std::string unmet = std::string(pre.ok() ? "" : pre.unmet(which)) +
                ((!pre.ok() && !window_met) ? " and " : "") + (window_met ? "" : "P9-WINDOW (" + which + ")");

            // (c): applied-volume CHANGES only -- the value in force at the window's edge is not a
            // change (it was applied before the window opened), and neither is a later segment that
            // repeats it.
            std::vector<int> applied; bool level_ok = true;
            if (win_ok && side >= 0) {
                auto runs = runs_in_window(c.audio, win_start, win_end, side);
                auto segs = segment_by_pair(runs);
                int last_val = 40;
                bool in_force = true;
                for (auto& s : segs) {
                    int found = -1;
                    for (auto& kv : L1) if (kv.second.first == s.a && kv.second.second == s.b) { found = kv.first; break; }
                    if (found < 0) { level_ok = false; continue; }
                    if (in_force) { last_val = found; in_force = false; continue; }
                    if (found != last_val) { applied.push_back(found); last_val = found; }
                }
            }
            checks.note("P9 " + which + ": the window has " + std::to_string(applied.size()) +
                " applied-volume changes (need >=" + std::to_string(2 * pc.mod_order.size()) + ")");
            bool cyclic_ok = level_ok && applied.size() >= 2*pc.mod_order.size() &&
                cyclic_consecutive(applied, pc.mod_order);
            // (d): channel 1's durations stay at D_1(200).
            bool dur_ok = false; size_t dur_pop = 0;
            if (win_ok && side >= 0) {
                auto runs = runs_in_window(c.audio, win_start, win_end, side);
                auto samples = samples_by_sign(runs);
                auto comp = complete_samples(samples, win_start, win_end);
                dur_pop = comp.size();
                dur_ok = !comp.empty();
                for (auto& s : comp)
                    if (D1.count(200) && std::fabs((double)(s.end - s.start) - D1[200]) > kClkRes) { dur_ok = false; break; }
            }
            if (!gate) {
                not_graded(checks, "P9-A", unmet);
            } else {
                checks.expect(c.loaded && off0_ok,
                    "P9-A [p.147,p.150]: channel 0's offsets step 0.." + std::to_string(pc.lm - 1) +
                    ",0,... (" + bin + ")");
            }
            if (!gate) {
                not_graded(checks, "P9-B", unmet);
            } else {
                checks.expect(c.loaded && off1_ok,
                    "P9-B [p.147,p.150]: channel 1's offsets step 0.." + std::to_string(pc.lc-1) +
                    ",0,...(" + bin + ")");
            }
            if (!gate) {
                not_graded(checks, "P9-C", unmet);
            } else {
                checks.expect(c.loaded && !applied.empty() && cyclic_ok,
                    "P9-C [p.150]: the window holds applied volumes of channel 1 and they are cyclically "
                    "consecutive across the modulator's own restarts, none skipped or repeated (" + bin + ")");
            }
            if (!gate) {
                not_graded(checks, "P9-D", unmet);
            } else if (dur_pop == 0) {
                checks.record("P9-D: not evaluated -- the window's complete-sample population is "
                    "empty (measured population: " + std::to_string(dur_pop) + ") (" + bin + ")");
            } else {
                checks.expect(c.loaded && dur_ok,
                    "P9-D: channel 1's durations (graded at SQ's sign alternations) stay at D_1(200) "
                    "(" + bin + ")");
            }
        }
        checks.record("5-mod.c18, 5-p150.c21: the lengths of the data sets are 'completely independent'. Two "
            "length pairs are observed, 5 and 3 (S-LEN-A) and 2 and 7 (S-LEN-B), the modulator once longer and "
            "once shorter than the modulated channel's; two ratios do not establish 'completely', so neither id "
            "is asserted.");
    }

    // ================= P10: DMA-request phase ==============================
    // p.164: F_1 is thrown away, F_2 arrives and enters state 010 (the record entry PC7-CAL calibrates), and
    // the main loop continues; the n-th fetch of the page is record entry r + n - 2 in every session, because
    // the start-up is the same in every session. Every line below grades only fetches from the page's F_3 on,
    // in that frame, and prints NOT GRADED where the frame is not established or PC1-PC3 of its session fail.
    {
        auto frame_met = [&](const std::vector<FetchRec>& rec) -> bool {
            return cal_ok && d_mod > 0 && ell > 0 && rec.size() > cal.r + 3;
        };
        // Rate (as in normal operation, 011->010): F_n - F_3 = 2(n-3)*D_mod for every n >= 4, to within less than
        // one line. The requests from F_3 on lie on the period counter's own grid, 2*D_mod apart, and each
        // fetch follows its request by less than a line plus a constant delay.
        auto rate_normal_ok = [&](const std::vector<FetchRec>& rec) -> bool {
            if (rec.size() < cal.r + 3) return false;
            for (size_t i = cal.r + 2; i < rec.size(); ++i) {
                double n = (double)(i - cal.r) + 2.0;
                double o = (double)rec[i].cycle - (double)rec[cal.r + 1].cycle - 2.0 * (n - 3.0) * d_mod;
                if (!(std::fabs(o) < ell)) return false;
            }
            return true;
        };
        // The phase of a session: (F_n - F_2) mod 2*D_mod, for n >= 3.
        auto phases = [&](const std::vector<FetchRec>& rec) -> std::vector<double> {
            std::vector<double> out;
            for (size_t i = cal.r + 1; i < rec.size(); ++i)
                out.push_back(std::fmod((double)rec[i].cycle - (double)rec[cal.r].cycle, 2.0 * d_mod));
            return out;
        };
        const double rmod_phase = median_d(phases(rec_of(rmod)));
        // The page names two phases: the requests of normal operation (011->010) and those of attach period
        // (010->011), one period later. The phase grid of a session is R-MOD's phase (normal operation), R-MOD's
        // phase plus D_mod (attach period), or both (both bits: every D_mod). A fetch lies on a grid when its
        // circular distance from the grid's phase (modulo 2*D_mod, or modulo D_mod for both bits) is less than
        // one line.
        auto circ = [&](double x, double m) { x = std::fmod(x, m); if (x < 0) x += m; return std::min(x, m - x); };
        double phase_max_dist = 0;
        auto phase_on_grid = [&](const std::vector<FetchRec>& rec, double shift, double modulus) -> bool {
            auto ph = phases(rec);
            phase_max_dist = 0;
            if (ph.empty()) return false;
            bool ok = true;
            for (double p : ph) {
                const double d = circ(p - rmod_phase - shift, modulus);
                phase_max_dist = std::max(phase_max_dist, d);
                if (!(d < ell)) ok = false;
            }
            return ok;
        };
        // Both bits: consecutive fetches from the page's F_3 on are D_mod apart, to within less than one line.
        auto spacing_both_ok = [&](const std::vector<FetchRec>& rec) -> bool {
            for (size_t i = cal.r + 2; i < rec.size(); ++i) {
                double gap = (double)rec[i].cycle - (double)rec[i - 1].cycle;
                if (!(std::fabs(gap - d_mod) < ell)) return false;
            }
            return true;
        };
        Capture av = mutating("P10-AV") ? run_capture(dir + "/prog_s_av_per6000.bin", budget_for(200000))
                   : mutating("P10-AV-PHASE") ? sap_cap
                   : run_capture(dir + "/prog_s_av.bin", budget_for(200000));
        Capture ap = mutating("P10-AP") ? run_capture(dir + "/prog_s_av.bin", budget_for(200000))
                   : mutating("P10-AP-GRID") ? run_capture(dir + "/prog_s_ap_shift.bin", budget_for(110000))
                   : mutating("P10-AP-RATE") ? run_capture(dir + "/prog_s_ap_pulse.bin", budget_for(110000)) : sap_cap;
        Capture both = mutating("P10-BOTH") ? run_capture(dir + "/prog_s_av.bin", budget_for(200000))
                     : mutating("P10-BOTH-GRID") ? run_capture(dir + "/prog_s_both_per4050.bin", budget_for(110000))
                     : run_capture(dir + "/prog_s_both.bin", budget_for(110000));
        mut_note("P10-AV", "prog_s_av_per6000.bin", av);
        mut_note("P10-AV-PHASE", "prog_s_ap.bin in place of prog_s_av.bin", av);
        mut_note("P10-AP", "prog_s_av.bin in place of prog_s_ap.bin", ap);
        mut_note("P10-AP-GRID", "prog_s_ap_shift.bin", ap);
        mut_note("P10-AP-RATE", "prog_s_ap_pulse.bin", ap);
        mut_note("P10-BOTH", "prog_s_av.bin in place of prog_s_both.bin", both);
        mut_note("P10-BOTH-GRID", "prog_s_both_per4050.bin", both);
        // Each session's own PC lines gate its P10 line; a contrast that replaces a capture with another
        // session's is graded against the session it stands in for.
        const Pre pre_av = sess_pre(nullptr, kSAV, mutating("P10-AV-PHASE") ? run_capture(dir + "/prog_s_av.bin", budget_for(200000)) : av);
        const Pre pre_ap = sess_pre(nullptr, kSAP, mutating("P10-AP") ? sap_cap : ap);
        const Pre pre_both = sess_pre(nullptr, kSBOTH, mutating("P10-BOTH") ? run_capture(dir + "/prog_s_both.bin", budget_for(110000)) : both);
        auto av_rec = rec_of(av), ap_rec = rec_of(ap), both_rec = rec_of(both);
        auto p10_unmet = [&](const Pre& pre, const std::string& sess, bool frame) {
            return std::string(pre.ok() ? "" : pre.unmet(sess)) + ((!pre.ok() && !frame) ? " and " : "") +
                   (frame ? "" : "PC7-CAL and a start-up frame in " + sess);
        };
        const bool av_rate = rate_normal_ok(av_rec), ap_rate = rate_normal_ok(ap_rec);
        const bool av_phase = phase_on_grid(av_rec, 0.0, 2.0 * d_mod);
        const double av_phase_dist = phase_max_dist;
        const bool ap_phase = phase_on_grid(ap_rec, d_mod, 2.0 * d_mod);
        const double ap_phase_dist = phase_max_dist;
        const bool both_phase = phase_on_grid(both_rec, 0.0, d_mod);
        const double both_phase_dist = phase_max_dist;
        const bool both_spacing = spacing_both_ok(both_rec);
        checks.note("P10: largest circular distance of a fetch from the phase grid: S-AV " + std::to_string(av_phase_dist) +
            " clk, S-AP " + std::to_string(ap_phase_dist) + " clk, S-BOTH " + std::to_string(both_phase_dist) +
            " clk (one line = " + std::to_string(ell) + " clk); rate: S-AV " + std::to_string((int)av_rate) + ", S-AP " +
            std::to_string((int)ap_rate) + "; spacing: S-BOTH " + std::to_string((int)both_spacing));
        dep_expect(checks, pre_av.ok() && frame_met(av_rec),
            av.loaded && av.done.has_value() && av_rate && av_phase,
            "P10-AV [p.164,p.165] 5-p164.c33,5-sm.c33: S-AV -- in the calibrated frame F_n - F_3 equals "
            "2(n-3)*D_mod to within less than one line for every n >= 4, and every fetch lies less than one "
            "line from R-MOD's phase (F_n - F_2 mod 2*D_mod, n >= 3), 'as they do in normal operation' (011->010)",
            p10_unmet(pre_av, "S-AV", frame_met(av_rec)));
        // F58.loop.attach kept only here (the attach-period bracket S-AP
        // alone exercises); napnav's DMA-half is recorded, not
        // re-asserted (it is the same formula PC7 already grades).
        dep_expect(checks, pre_ap.ok() && frame_met(ap_rec), ap.loaded && ap.done.has_value() && ap_rate && ap_phase,
            "P10-AP [p.164] 5-p164.c34,5-sm.c34,F58.loop.attach: S-AP -- one request per 010->011->010 loop (F_n - "
            "F_3 equals 2(n-3)*D_mod to within less than one line for every n >= 4) and every fetch lies less than "
            "one line from the attach-period phase, R-MOD's plus D_mod (F_n - F_2 mod 2*D_mod, n >= 3; 010->011)",
            p10_unmet(pre_ap, "S-AP", frame_met(ap_rec)));
        checks.record("5-p165.c22, 5-sig.napnav: the DMA-request half is graded only through the phases above "
            "(P10-AV, P10-AP, P10-BOTH); no assertion isolates napnav's own contribution beyond them, and the "
            "interrupt half is not graded at all (see spec's partial-claims table).");
        checks.record("5-p165.c4, 5-sig.intreq1: intreq1 is declared to combine with intreq2 to "
            "form AUDxIR, but the manual does not say how or when; no port access, audio sample "
            "or register value in this package distinguishes one way of combining from another, "
            "so this is not turned into an assertion (SPECIFICATION.md, claims not turned into a "
            "proposition).");
        dep_expect(checks, pre_both.ok() && frame_met(both_rec),
            both.loaded && both.done.has_value() && both_spacing && both_phase,
            "P10-BOTH [p.164] 5-p164.c35,5-sm.c35: S-BOTH -- the fetches from the page's F_3 on are D_mod apart, "
            "to within less than one line, and every fetch lies less than one line from R-MOD's phase grid "
            "(F_n - F_2 modulo D_mod, n >= 3; both transitions)",
            p10_unmet(pre_both, "S-BOTH", frame_met(both_rec)));
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
