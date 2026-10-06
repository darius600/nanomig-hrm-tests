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
// ordinary line of the same id then prints its verdict through the unchanged
// comparator. None is exercised by a plain `make test` and none is counted in
// its summary. "Page model" below is the private copy of the audio RTL built by
// `make pm-test-fx68k` / `make pm-test-tg68k` (page_model.patch), which
// implements the attach bits as pages 149-151 and 164-165 describe them; the
// repository RTL does not implement them, so the lines that grade attach
// behaviour are red on it before and after a stimulus change, and a record that
// can only move on the page model says so. Per-core results of every record are
// in SPECIFICATION.md, "Measured values".
//
// MUTATION FIXTURE-STORE stimulus red the program stores $DEAD in place of $BEEF (prog_fixture_mut) (NMH_MUTATE=FIXTURE-STORE); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION FIXTURE-ACTIVITY stimulus red AUD0EN is left out of the enable, so channel 0 never plays (prog_fixture_noplay) (NMH_MUTATE=FIXTURE-ACTIVITY); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION PC1 stimulus red the step-4 ADKCON write is omitted in S-AV (prog_s_av_noattach), graded against the session's own $01 mask (NMH_MUTATE=PC1); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION PC2 stimulus red R-MOD with the enable $8001: AUD0EN without DMAEN (prog_rmod_nodma) (NMH_MUTATE=PC2); RTL/fx68k n/a -> FAIL; RTL/TG68K n/a -> FAIL; PM/fx68k n/a -> FAIL; PM/TG68K n/a -> FAIL
// MUTATION PC3 stimulus red R-MOD with the enable $8200: DMAEN without AUD0EN (prog_rmod_noaud) (NMH_MUTATE=PC3); RTL/fx68k n/a -> FAIL; RTL/TG68K n/a -> FAIL; PM/fx68k n/a -> FAIL; PM/TG68K n/a -> FAIL
// MUTATION PC4-VOL stimulus red R-VOL-1 writes 40 in the step labelled 60 (prog_rvol1_mut); R-VOL-2 and R-VOL-3 are graded by the same call and have no contrast of their own (NMH_MUTATE=PC4-VOL); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION PC4-DUR stimulus red R-PER-1 writes 500 in the step labelled 700 (prog_rper1_mut); R-PER-2, R-PER-3 and R-RAMP are graded by the same call and have no contrast of their own (NMH_MUTATE=PC4-DUR); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION PC5-VOL stimulus red R-VOL-1's capture is graded as R-VOL-3's, so "channel 3's" output is channel 1's (NMH_MUTATE=PC5-VOL); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION PC5-PER stimulus red R-VOL-3's capture is graded as R-PER-1's, so "channel 1's" output is channel 3's (NMH_MUTATE=PC5-PER); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION PC6 stimulus red AUD0PER = 5000 is written halfway through R-MOD (prog_rmod_pc6) (NMH_MUTATE=PC6); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION PC7-CAL stimulus red R-MOD with AUD0VOL = 0 (prog_rmod_vol0): the output never changes and no instant is found (NMH_MUTATE=PC7-CAL); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION PC7-FRAME stimulus red M_on is written late, after a delay following the DMACON enable (prog_rmod_pc7frame), so a fetch occurs before it (NMH_MUTATE=PC7-FRAME); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION PC7-LOOP stimulus red AUD0PER = 3000 is written halfway through R-MOD (prog_rmod_pc7) (NMH_MUTATE=PC7-LOOP); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION PC7-N3 stimulus red S-AP's capture is graded through the same comparator: with napnav = 0 the figure prints no request on 101->010 (NMH_MUTATE=PC7-N3); RTL/fx68k PASS -> PASS; RTL/TG68K PASS -> PASS; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P1-WINDOW implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k PASS; RTL/TG68K PASS; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P1-A implementation green the ordinary run of the page model, where the attach bits are implemented (the ordinary run); RTL/fx68k FAIL; RTL/TG68K FAIL; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P1-A stimulus red the ADKCON $8001 write is omitted (prog_s_av_noattach); PC1 expects $00 (NMH_MUTATE=P1-NOATTACH); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P1-B implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k NOT EVALUATED; RTL/TG68K NOT EVALUATED; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P1-B stimulus red the last modulator word is 33, a volume with no reference level pair (prog_s_av_mv33) (NMH_MUTATE=P1-MV33); RTL/fx68k NOT EVALUATED -> NOT EVALUATED; RTL/TG68K NOT EVALUATED -> NOT EVALUATED; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P1-C implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k NOT EVALUATED; RTL/TG68K NOT EVALUATED; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P1-C stimulus red channel 1's own AUDxPER is rewritten from 200 to 300 halfway through (prog_s_av_per1write) (NMH_MUTATE=P1-PER1WRITE); RTL/fx68k NOT EVALUATED -> NOT EVALUATED; RTL/TG68K NOT EVALUATED -> NOT EVALUATED; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P1-JOINT implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k FAIL; RTL/TG68K FAIL; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P1-JOINT stimulus red the ADKCON $8001 write is omitted (prog_s_av_noattach) (NMH_MUTATE=P1-NOATTACH); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P2-A implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k FAIL; RTL/TG68K FAIL; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P2-A stimulus red the ADKCON $8010 write is omitted (prog_s_p2_noattach) (NMH_MUTATE=P2-NOATTACH); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P2-B implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k NOT EVALUATED; RTL/TG68K NOT EVALUATED; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P2-C implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k NOT EVALUATED; RTL/TG68K NOT EVALUATED; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P3-PRE stimulus red R-MOD with AUD0VOL = 0 (prog_rmod_vol0) plays no byte (NMH_MUTATE=P3-PRE); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P3-SILENT implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k NOT EVALUATED; RTL/TG68K NOT EVALUATED; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P3-SILENT stimulus red R-MOD's capture in place of S-AV's, graded on channel 1's output, which is silent there (NMH_MUTATE=P3-SILENT); RTL/fx68k NOT EVALUATED -> FAIL; RTL/TG68K NOT EVALUATED -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P3-WHOLE implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k NOT EVALUATED; RTL/TG68K NOT EVALUATED; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P3-WHOLE stimulus red R-MOD's capture in place of S-AV's, graded on channel 0's own output: every word is played as two bytes (NMH_MUTATE=P3-WHOLE); RTL/fx68k NOT EVALUATED -> FAIL; RTL/TG68K NOT EVALUATED -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P3-SP2 implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k NOT EVALUATED; RTL/TG68K NOT EVALUATED; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P4-A implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k FAIL; RTL/TG68K FAIL; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P4-A stimulus red S-BOTH-volonly's capture (ADKCON $8001) in place of S-BOTH's (NMH_MUTATE=P4-A); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P4-B implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k NOT EVALUATED; RTL/TG68K NOT EVALUATED; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P4-B stimulus red S-BOTH-volonly's capture in place of S-BOTH's: the even words (periods) reach the volume (NMH_MUTATE=P4-B); RTL/fx68k NOT EVALUATED -> NOT EVALUATED; RTL/TG68K NOT EVALUATED -> NOT EVALUATED; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P4-C implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k FAIL; RTL/TG68K FAIL; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P4-C stimulus red S-BOTH-volonly's capture in place of S-BOTH's: the period never changes (NMH_MUTATE=P4-C); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P4-VOLONLY stimulus red channel 1's own AUDxPER is rewritten from 200 to 300 halfway through S-BOTH-volonly (prog_s_both_volonly_per) (NMH_MUTATE=P4-VOLONLY); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P4-JOINT implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k FAIL; RTL/TG68K FAIL; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P4-JOINT stimulus red S-BOTH-volonly's capture in place of S-BOTH's (NMH_MUTATE=P4-A); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P5-AV implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k NOT EVALUATED; RTL/TG68K NOT EVALUATED; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P5-AV stimulus red AUD0PER = 6000 (prog_s_av_per6000), graded with R-MOD's D_mod (NMH_MUTATE=P5-AV); RTL/fx68k NOT EVALUATED -> NOT EVALUATED; RTL/TG68K NOT EVALUATED -> NOT EVALUATED; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P5-AP implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k NOT EVALUATED; RTL/TG68K NOT EVALUATED; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P5-AP stimulus red AUD0PER = 6000 (prog_s_ap_per6000), graded with R-MOD's D_mod (NMH_MUTATE=P5-AP); RTL/fx68k NOT EVALUATED -> NOT EVALUATED; RTL/TG68K NOT EVALUATED -> NOT EVALUATED; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P5-BOTH implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k NOT EVALUATED; RTL/TG68K NOT EVALUATED; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P5-BOTH stimulus red AUD0PER = 6000 (prog_s_both_per6000), graded with R-MOD's D_mod (NMH_MUTATE=P5-BOTH); RTL/fx68k NOT EVALUATED -> NOT EVALUATED; RTL/TG68K NOT EVALUATED -> NOT EVALUATED; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-A implementation green bit 0, the ordinary run of the page model (the ordinary run); RTL/fx68k FAIL; RTL/TG68K FAIL; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P6-A implementation green bit 1, the ordinary run of the page model (the ordinary run); RTL/fx68k FAIL; RTL/TG68K FAIL; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P6-A implementation green bit 2, the ordinary run of the page model (the ordinary run); RTL/fx68k FAIL; RTL/TG68K FAIL; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P6-A implementation green bit 4, the ordinary run of the page model (the ordinary run); RTL/fx68k FAIL; RTL/TG68K FAIL; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P6-A implementation green bit 5, the ordinary run of the page model (the ordinary run); RTL/fx68k FAIL; RTL/TG68K FAIL; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P6-A implementation green bit 6, the ordinary run of the page model (the ordinary run); RTL/fx68k FAIL; RTL/TG68K FAIL; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P6-A stimulus red bit 0, the attach write is omitted (prog_s_pair0_noattach) (NMH_MUTATE=P6-0); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-A stimulus red bit 1, the attach write is omitted (prog_s_pair1_noattach) (NMH_MUTATE=P6-1); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-A stimulus red bit 2, the attach write is omitted (prog_s_pair2_noattach) (NMH_MUTATE=P6-2); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-A stimulus red bit 4, the attach write is omitted (prog_s_pair4_noattach) (NMH_MUTATE=P6-4); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-A stimulus red bit 5, the attach write is omitted (prog_s_pair5_noattach) (NMH_MUTATE=P6-5); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-A stimulus red bit 6, the attach write is omitted (prog_s_pair6_noattach) (NMH_MUTATE=P6-6); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-B stimulus red bit 0: the target's own AUDxPER is rewritten from 200 to 300 halfway through (prog_s_pair0_tgtper) (NMH_MUTATE=P6-TGT-0); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-B stimulus red bit 4: the target's own AUDxVOL is rewritten from 40 to 20 halfway through (prog_s_pair4_tgtvol) (NMH_MUTATE=P6-TGT-4); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-C stimulus red bit 0: a tripwire (ch3) is given a non-zero starting AUDxVOL (prog_s_pair0_extra) (NMH_MUTATE=P6-0-EXTRA); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-C stimulus red bit 2: a tripwire (ch1) is given a non-zero starting AUDxVOL (prog_s_pair2_extra) (NMH_MUTATE=P6-2-EXTRA); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-D stimulus red bit 4: a tripwire's (ch3) AUDxPER is rewritten halfway through (prog_s_pair4_extra) (NMH_MUTATE=P6-4-EXTRA); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-D stimulus red bit 5: a tripwire's (ch3) AUDxPER is rewritten halfway through (prog_s_pair5_extra) (NMH_MUTATE=P6-5-EXTRA); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P6-D stimulus red bit 6: a tripwire's (ch1) AUDxPER is rewritten halfway through (prog_s_pair6_extra) (NMH_MUTATE=P6-6-EXTRA); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P7-CONST stimulus red a tripwire (ch0) is given a non-zero starting AUDxVOL (prog_s_ch3v_extra) (NMH_MUTATE=P7V); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P7-SPACING stimulus red a tripwire's (ch0) AUDxPER is rewritten halfway through (prog_s_ch3p_extra) (NMH_MUTATE=P7P); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P8-ATT implementation green the ordinary run of the page model (the first of the five lines, S-AV) (the ordinary run); RTL/fx68k FAIL; RTL/TG68K FAIL; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P8-ATT stimulus red the attach write is omitted: the unattached reference session takes the attached one's place (first line, S-AV) (NMH_MUTATE=P8-OMIT); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P8-PRE stimulus red R-MOD with AUD0VOL = 0 (prog_rmod_vol0) (NMH_MUTATE=P8-PRE); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P8-JOINT-AP implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k FAIL; RTL/TG68K FAIL; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P8-JOINT-AP stimulus red the attach write is omitted in the source half: R-MOD takes S-AP's place (NMH_MUTATE=P8-OMIT); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P9-A stimulus red AUD0LEN = 4 in S-LEN-A (prog_s_lena_len4), graded with the page's L_m = 5 (NMH_MUTATE=P9-A); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P9-B stimulus red AUD1LEN = 2 in S-LEN-A (prog_s_lena_ch1len2), graded with the page's L_c = 3 (NMH_MUTATE=P9-B); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P9-C implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k NOT EVALUATED; RTL/TG68K NOT EVALUATED; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P9-C stimulus red AUD0LEN = 4 in S-LEN-A (prog_s_lena_len4), graded with the page's modulator cycle of 5 words (NMH_MUTATE=P9-C); RTL/fx68k NOT EVALUATED -> NOT EVALUATED; RTL/TG68K NOT EVALUATED -> NOT EVALUATED; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P10-AV stimulus red AUD0PER = 6000 (prog_s_av_per6000), graded with R-MOD's D_mod (NMH_MUTATE=P10-AV); RTL/fx68k PASS -> FAIL; RTL/TG68K PASS -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P10-AP implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k FAIL; RTL/TG68K FAIL; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P10-AP stimulus red S-AV's capture in place of S-AP's (NMH_MUTATE=P10-AP); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P10-BOTH implementation green the ordinary run of the page model (the ordinary run); RTL/fx68k FAIL; RTL/TG68K FAIL; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P10-BOTH stimulus red S-AV's capture in place of S-BOTH's (NMH_MUTATE=P10-BOTH); RTL/fx68k FAIL -> FAIL; RTL/TG68K FAIL -> FAIL; PM/fx68k PASS -> FAIL; PM/TG68K PASS -> FAIL
// MUTATION P5-POP-AV implementation green the ordinary run of the page model; no stimulus makes the population smaller by a page-derived amount (Mutation scope) (the ordinary run); RTL/fx68k NOT EVALUATED; RTL/TG68K NOT EVALUATED; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P5-POP-AP implementation green the ordinary run of the page model; no stimulus makes the population smaller by a page-derived amount (Mutation scope) (the ordinary run); RTL/fx68k NOT EVALUATED; RTL/TG68K NOT EVALUATED; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P5-POP-BOTH implementation green the ordinary run of the page model; no stimulus makes the population smaller by a page-derived amount (Mutation scope) (the ordinary run); RTL/fx68k NOT EVALUATED; RTL/TG68K NOT EVALUATED; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P9-D none impossible not exercised, by a scope decision: no stimulus here makes the durations of channel 1 leave D_1(200) without changing what the other P9 lines grade (Mutation scope); the ordinary run (the ordinary run); RTL/fx68k PASS; RTL/TG68K PASS; PM/fx68k PASS; PM/TG68K PASS
// MUTATION P9-WINDOW none impossible not exercised, by a scope decision: a window that does not open would leave every P9 line NOT GRADED (Mutation scope); the ordinary run (the ordinary run); RTL/fx68k PASS; RTL/TG68K PASS; PM/fx68k PASS; PM/TG68K PASS
//
// Not built, and why: see SPECIFICATION.md, "Mutation scope".

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
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

// A program image and run budget determine the run, so each is simulated once
// per process; every session below may be graded by several lines.
Capture run_capture(const std::string& bin, uint64_t run_budget) {
    static std::map<std::string, Capture> memo;
    const std::string key = bin + "#" + std::to_string(run_budget);
    auto it = memo.find(key);
    if (it == memo.end()) it = memo.emplace(key, simulate(bin, run_budget)).first;
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
// graded in hrm/ch5-audio-dma-first-pass (NM-HRM-084), not here.
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

void pc1_line(Checks& checks, const std::string& tag, const Capture& c, uint16_t expect_mask) {
    bool pc1 = c.adkr && (*c.adkr & 0xFF) == expect_mask;
    checks.expect(c.loaded && c.done.has_value() && pc1,
        "PC1 [p.265] " + tag + ": stored ADKCONR bits 7-0 equal the session's attach mask");
}
void pc2_line(Checks& checks, const std::string& tag, const Capture& c, uint16_t expect_aud_mask) {
    bool pc2 = c.dmacr && (*c.dmacr & 0x0200) == 0x0200 && (*c.dmacr & expect_aud_mask) == expect_aud_mask;
    checks.expect(c.loaded && c.done.has_value() && pc2,
        "PC2 [p.278] " + tag + ": stored DMACONR has DMAEN and the session's AUDxEN bits set");
}
// min_cycles: the specification's population for PC3's modulator clause --
// two complete cycles after the first restart, or one in S-P2 (the only
// caller that passes 1). Short of that population, PC3 prints "not
// evaluated" (RECORDED), never PASS or FAIL; a broken step (stepping_cycles
// == -1) is always FAIL, whatever the population.
void pc3_lines(Checks& checks, const std::string& tag, const Capture& c, const std::vector<ChSpan>& enabled,
               bool have_bounds, uint64_t lo, uint64_t hi, uint32_t min_cycles = 2) {
    for (auto& e : enabled) {
        std::string label = "PC3 [p.147] " + tag + " ch" + std::to_string(e.ch) + ": fetches step 0.." +
            std::to_string(e.len - 1) + ",0,... from the channel's first restart on (the first "
            "pass is graded in hrm/ch5-audio-dma-first-pass), with >=" + std::to_string(min_cycles) +
            " complete cycle(s) in its own 4 KB region";
        if (!have_bounds || !c.loaded || !c.done.has_value()) { checks.expect(false, label); continue; }
        auto offs = fetch_offsets(c.log, ch_base(e.ch), kChannelRegionWords, lo, hi);
        int cycles = stepping_cycles(offs, e.len);
        if (cycles < 0) {
            checks.expect(false, label);
        } else if ((uint32_t)cycles < min_cycles) {
            checks.record(label + " [not evaluated: only " + std::to_string(cycles) +
                " complete cycle(s) after the first restart in this window]");
        } else {
            checks.expect(true, label);
        }
    }
}
void assert_pc123(Checks& checks, const std::string& tag, const Capture& c,
                  uint16_t expect_mask, uint16_t expect_aud_mask,
                  const std::vector<ChSpan>& enabled, bool have_bounds, uint64_t lo, uint64_t hi,
                  uint32_t min_cycles = 2) {
    pc1_line(checks, tag, c, expect_mask);
    pc2_line(checks, tag, c, expect_aud_mask);
    pc3_lines(checks, tag, c, enabled, have_bounds, lo, hi, min_cycles);
}

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
    double lambda = ticks(300);

    // PC6: the spread of channel 0's complete samples about D_mod.
    auto pc6_spread_ok = [&](const Capture& c, const std::vector<uint64_t>& b) -> bool {
        if (!(d_mod > 0 && side0.ok && b.size() >= 2)) return false;
        uint64_t lo = b.front(), hi = play_end(c.audio, side0.side, b.back());
        auto runs = runs_in_window(c.audio, lo, hi, side0.side);
        auto comp = complete_runs(runs, lo, hi);
        bool ok = !comp.empty();
        for (auto& r : comp)
            if (std::fabs((double)(r.end - r.start) - d_mod) > d_mod / 4000.0 + kClkRes) { ok = false; break; }
        return ok;
    };
    {
        Capture pc6c = mutating("PC6") ? run_capture(dir + "/prog_rmod_pc6.bin", budget_for(200000)) : rmod;
        mut_note("PC6", "prog_rmod_pc6.bin", pc6c);
        auto b = sweep_bounds(pc6c, mutating("PC6") ? std::vector<uint16_t>{3} : std::vector<uint16_t>{});
        checks.expect(pc6c.loaded && pc6c.done.has_value() && pc6_spread_ok(pc6c, b),
            "PC6 [p.141,p.164]: in R-MOD, channel 0's sample durations differ from D_mod by at "
            "most 1 part in 4000");
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
        // One line is 227.5 ticks (p.141, as in "Units"); requests go out once
        // each line (p.164), so the fetch nearest to the instant is unambiguous
        // within half a line. The page's "first data word ... thrown away" is one
        // fetch before F_2; "reset the pointer to the beginning of the table of
        // data" makes F_2 the data set's first word.
        cal.ok = bd <= ticks(227.5) / 2.0 && cal.r <= 1 && cal.offset == 0;
        return cal;
    };
    const Cal cal = calibrate(rmod);
    const bool cal_ok = rmod.loaded && rmod.done.has_value() && cal.ok;
    checks.note("PC7-CAL measured: the lower byte of the first word is output at M_on+" +
        std::to_string(cal.t010 + d_mod - (rmod_bounds ? (double)rmod_b[0] : 0.0)) +
        " clk, so state 010 was entered at M_on+" + std::to_string(cal.t010 - (rmod_bounds ? (double)rmod_b[0] : 0.0)) +
        " clk; the nearest fetch is record entry " + std::to_string(cal.r) + " of " + std::to_string(cal.n) +
        " (offset " + std::to_string(cal.offset) + ") " + std::to_string(cal.dist) + " clk away");
    {
        Capture cc = mutating("PC7-CAL") ? run_capture(dir + "/prog_rmod_vol0.bin", budget_for(200000)) : rmod;
        mut_note("PC7-CAL", "prog_rmod_vol0.bin", cc);
        Cal k = mutating("PC7-CAL") ? calibrate(cc) : cal;
        checks.expect(cc.loaded && cc.done.has_value() && k.ok,
            "PC7-CAL [p.164]: in R-MOD, the fetch whose arrival enters state 010 is calibrated from channel "
            "0's own output; it is the fetch nearest to that instant, within half a line of it, it is the "
            "data set's first word, and at most one fetch (the one thrown away) precedes it -- it is the "
            "page's F_2");
    }
    // The frame -- no read of channel 0's region before M_on.
    {
        const Capture& fc = mutating("PC7-FRAME") ? run_capture(dir + "/prog_rmod_pc7frame.bin", budget_for(200000)) : rmod;
        mut_note("PC7-FRAME", "prog_rmod_pc7frame.bin", fc);
        auto b = sweep_bounds(fc, {});
        bool no_early = b.size() == 2 && fetch_cycles(fc.log, ch_base(0), kChannelRegionWords, 0, b[0] > 0 ? b[0] - 1 : 0).empty();
        checks.expect(fc.loaded && fc.done.has_value() && b.size() == 2 && no_early,
            "PC7-FRAME [p.164]: no read of channel 0's data region occurs before M_on");
    }
    // PC7-LOOP: every consecutive pair of fetches from the page's F_3 on is 2*D_mod apart, within
    // lambda ("Transitions such as 010->011->010 occur whenever the period counter counts down to
    // one"; one request per 010->011->010 loop).
    auto pc7_loop_ok = [&](const std::vector<FetchRec>& rec, size_t r) -> bool {
        if (d_mod <= 0 || rec.size() < r + 4) return false;
        for (size_t i = r + 2; i < rec.size(); ++i) {
            double gap = (double)rec[i].cycle - (double)rec[i - 1].cycle;
            if (std::fabs(gap - 2.0 * d_mod) > lambda) return false;
        }
        return true;
    };
    {
        Capture lc = mutating("PC7-LOOP") ? run_capture(dir + "/prog_rmod_pc7.bin", budget_for(200000)) : rmod;
        mut_note("PC7-LOOP", "prog_rmod_pc7.bin", lc);
        dep_expect(checks, cal_ok, lc.loaded && lc.done.has_value() && side0.ok && pc7_loop_ok(rec_of(lc), cal.r),
            "PC7-LOOP [p.164,Fig.5-8 011->010]: loop anchor -- in R-MOD, every consecutive pair of fetches "
            "from the page's F_3 on is D_mod*2 apart, within lambda",
            "PC7-CAL");
    }
    // PC7-N3: Figure 5-8's 101->010 bracket, "AUDxDR if napnav" (napnav = 1 with no attach bit): the
    // request made when F_2 arrives is fetched one line later, so F_3 - F_2 lies within lambda of 0. No
    // claim id in either claims file names this transition.
    auto pc7_n3_ok = [&](const std::vector<FetchRec>& rec, size_t r) -> bool {
        return d_mod > 0 && rec.size() > r + 1 &&
               std::fabs((double)rec[r + 1].cycle - (double)rec[r].cycle) <= lambda;
    };
    {
        Capture nc = mutating("PC7-N3") ? run_capture(dir + "/prog_s_ap.bin", budget_for(110000)) : rmod;
        mut_note("PC7-N3", "prog_s_ap.bin", nc);
        dep_expect(checks, cal_ok, nc.loaded && nc.done.has_value() && pc7_n3_ok(rec_of(nc), cal.r),
            "PC7-N3 [Fig.5-8 101->010 'AUDxDR if napnav']: F_3 - F_2 lies within lambda of 0 (the request "
            "made on 101->010 is one line behind F_2)",
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
    // (below); their per-session call sites are the same functions.
    {
        Capture c2 = mutating("PC2") ? run_capture(dir + "/prog_rmod_nodma.bin", budget_for(200000)) : rmod;
        mut_note("PC2", "prog_rmod_nodma.bin", c2);
        if (mutating("PC2")) pc2_line(checks, "R-MOD (DMAEN omitted)", c2, 0x0001);
        Capture c3 = mutating("PC3") ? run_capture(dir + "/prog_rmod_noaud.bin", budget_for(200000)) : rmod;
        mut_note("PC3", "prog_rmod_noaud.bin", c3);
        if (mutating("PC3")) {
            auto b3 = sweep_bounds(c3, {});
            pc3_lines(checks, "R-MOD (AUD0EN omitted)", c3, {{0, 8}}, b3.size() == 2, b3.size() ? b3[0] : 0, b3.size() ? b3[1] : 0);
        }
    }
    assert_pc123(checks, "R-MOD", rmod, 0x00, 0x0001, {{0, 8}}, rmod_bounds, rmod_b.size() ? rmod_b[0] : 0, rmod_b.size() ? rmod_b[1] : 0);
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

    // The S-AP and S-AV sessions, loaded once for every section below.
    Capture sap_cap = run_capture(dir + "/prog_s_ap.bin", budget_for(110000));
    // The destination half of the AUDxAP joint line (filled in P5), the source half in P8.
    bool sap_dest_ok = false;
    size_t sap_dest_pop = 0;

    // ================= P1: ATVOL0 =========================================
    {
        const bool noattach = mutating("P1-NOATTACH") || mutating("PC1");
        std::string bin = dir + (noattach ? "/prog_s_av_noattach.bin"
                                 : mutating("P1-MV33") ? "/prog_s_av_mv33.bin"
                                 : mutating("P1-PER1WRITE") ? "/prog_s_av_per1write.bin" : "/prog_s_av.bin");
        Capture c = run_capture(bin, budget_for(200000));
        mut_note("P1-NOATTACH", bin, c);
        mut_note("P1-MV33", bin, c);
        mut_note("P1-PER1WRITE", bin, c);
        mut_note("PC1", bin, c);
        int side = side_of[1];
        // The contrast that omits the attach write expects mask $00 from PC1; the PC1 contrast keeps $01.
        uint16_t expect_mask = mutating("P1-NOATTACH") ? 0x00 : 0x01;
        auto mk = markers(c);
        auto on = m_on(mk), off = m_off(mk);
        bool have_bounds = on && off;
        assert_pc123(checks, "S-AV", c, expect_mask, 0x0003, {{0,8},{1,4}}, have_bounds, have_bounds ? *on : 0, have_bounds ? *off : 0);

        // The window runs from the third fetch of channel 0's own data set (the modulator is then in its
        // main loop: p.164) to the end of play of channel 1's output (see Measurements). It does not
        // depend on what channel 1 plays, so a channel 1 that is never modulated has a window and its
        // verdict is the line's own.
        uint64_t win_start = 0, win_end = 0;
        bool win_ok = false;
        if (have_bounds && side >= 0) {
            auto fc0w = fetch_cycles(c.log, ch_base(0), 8, *on, *off);
            if (fc0w.size() >= 3) {
                win_start = fc0w[2];
                win_end = play_end(c.audio, side, *off);
                win_ok = win_end > win_start;
                checks.note("S-AV: the window of channel 1's output ends " + std::to_string((long long)*off - (long long)win_end) +
                    " clk before M_off, inside the sample the DMA stop cuts short");
            }
        }
        // Applied volumes are the CHANGES of channel 1's level pair away from its own written starting value
        // (VOL 40); a segment that merely shows the value in force is not a change. A pair that is no
        // reference pair counts as a change to -1 and makes the window's levels fail P1-B.
        std::vector<int> applied;
        bool level_ok = win_ok, dur_ok = false;
        size_t dur_pop = 0;
        if (win_ok) {
            auto runs = runs_in_window(c.audio, win_start, win_end, side);
            auto segs = segment_by_pair(runs);
            int last_val = 40;
            for (auto& sg : segs) {
                int found = -1;
                for (auto& kv : L1) if (kv.second.first == sg.a && kv.second.second == sg.b) { found = kv.first; break; }
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
        // P1-WINDOW is a precondition of the lines that grade levels and durations inside the window; when
        // it is not met they print NOT GRADED. P1-A and P1-JOINT carry their own n >= 17.
        if (win_ok) {
            checks.expect(c.loaded && win_ok,
                "P1-WINDOW [p.149]: the window exists -- channel 0 has a third fetch of its own data set before "
                "the end of play of channel 1's output");
        } else {
            checks.record("P1-WINDOW [p.149]: precondition not met -- no window from channel 0's third fetch to "
                "the end of play; P1-B and P1-C print NOT GRADED");
        }
        bool a_ok = c.loaded && c.done.has_value() && applied.size() >= 17 && level_ok && cyclic_consecutive(applied, m_v_order);
        checks.note("P1: window has " + std::to_string(applied.size()) + " applied-volume changes "
            "(need >=17); cyclic-consecutive-in-M_V=" +
            std::string(cyclic_consecutive(applied, m_v_order) ? "yes" : "no") +
            "; every level pair matched a reference=" + std::string(level_ok ? "yes" : "no"));

        checks.expect(a_ok,
            "P1-A [p.149,p.150,p.265] 5-hw.c3,5-p136.c3,5-p149.c5,5-mod.c3,5-p149.c7,5-mod.c20,5-p150.c23,"
            "5-mod.c15,5-p150.c2,5-p150.c8,5-p150.c12,5-p150.c16,5-p150.c20,5-t54.w1.c4,5-t54.w2.c4,"
            "5-t54.w3.c4,5-t54.w4.c4,5-p151.c8,5-t55.b0,5-appA-p265.c29,A-adkcon.b00: the applied volumes in "
            "the window are cyclically consecutive in M_V, n>=17, every value from M_V");
        // P1-B and P1-C grade what the words that reach channel 1 do, so their population is the applied
        // changes; with none, no word has reached the volume and nothing can be said about its levels or
        // about the period "not being reached".
        if (!win_ok) {
            not_graded(checks, "P1-B", "P1-WINDOW");
        } else if (applied.empty()) {
            checks.record("P1-B [p.149,p.165]: not evaluated -- the window's applied-volume "
                "population is empty (measured population: " + std::to_string(applied.size()) + ")");
        } else {
            checks.expect(c.loaded && level_ok,
                "P1-B [p.149,p.165]: every level pair in the window is channel 1's own reference "
                "pair L_1(v) of the current applied volume");
        }
        if (!win_ok) {
            not_graded(checks, "P1-C", "P1-WINDOW");
        } else if (applied.empty() || dur_pop == 0) {
            checks.record("P1-C [Table 5-4 Modulating Volume Only]: not evaluated -- the "
                "window's applied-volume population is empty or it has no complete sample (measured populations: " +
                std::to_string(applied.size()) + " applied volumes, " + std::to_string(dur_pop) + " complete samples)");
        } else {
            checks.expect(c.loaded && dur_ok,
                "P1-C [Table 5-4 Modulating Volume Only]: every channel 1 duration (graded at "
                "SQ's sign alternations) in the window equals D_1(200) -- no word reaches the "
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
            "data modulating the next channel for every bit), so none is asserted; the volume half for "
            "bit 0 is P1-A and the AUDxAV joint line below.");

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
        checks.expect(a_ok && ch0_const,
            "P1-JOINT [p.165] 5-p165.c19,5-sig.audxav: in S-AV, channel 1 follows channel 0's "
            "data (destination half) AND channel 0's own side holds one value once its data is "
            "flowing (source half) -- both halves of the AUDxAV claim, one session");
    }

    // ================= P2: ATPER0 =========================================
    {
        std::string bin = dir + (mutating("P2-NOATTACH") ? "/prog_s_p2_noattach.bin" : "/prog_s_p2.bin");
        Capture c = run_capture(bin, budget_for(1400000));
        mut_note("P2-NOATTACH", bin, c);
        int side = side_of[1];
        uint16_t expect_mask = mutating("P2-NOATTACH") ? 0x00 : 0x10;
        auto mk = markers(c);
        auto on = m_on(mk), off = m_off(mk);
        bool have_bounds = on && off;
        assert_pc123(checks, "S-P2", c, expect_mask, 0x0003, {{0,17},{1,8}}, have_bounds, have_bounds ? *on : 0, have_bounds ? *off : 0, 1);

        // Window opens at the first run whose applied period differs from
        // D_1(700) (from R-RAMP) -- guarded against an idle lead-in run --
        // and then only when that run's duration matches a RECOGNIZED
        // reference duration -- some DR value != 700 -- not any
        // deviation from 700. A duration outside every reference (the
        // sample the DMA stop cuts short, for example) must not open the
        // window either.
        double ref700 = DR.count(700) ? DR[700] : 0;
        std::vector<int> applied_periods;
        uint64_t win_start = 0, win_end = 0;
        bool win_ok = false;
        if (have_bounds && side >= 0 && ref700 > 0) {
            auto runs = runs_in_window(c.audio, *on, *off, side);
            auto comp = complete_runs(runs, *on, *off);
            bool seen700 = false;
            for (auto& r : comp) {
                double dd = (double)(r.end - r.start);
                if (!seen700) { if (std::fabs(dd - ref700) <= 0.5 * ticks(1)) seen700 = true; continue; }
                bool matches_ref = false;
                for (auto& kv : DR) { if (kv.first == 700) continue; if (std::fabs(dd - kv.second) <= 0.5 * ticks(1)) { matches_ref = true; break; } }
                if (matches_ref) { win_start = r.start; win_ok = true; break; }
            }
            win_end = play_end(c.audio, side, *off);
        }
        std::vector<double> run_means; // for (b)'s own 0.5-tick check
        if (win_ok) {
            auto runs = runs_in_window(c.audio, win_start, win_end, side);
            auto comp = complete_runs(runs, win_start, win_end);
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
                for (auto& kv : DR) if (std::fabs(kv.second - mean) <= 0.5 * ticks(1)) { found = kv.first; break; }
                applied_periods.push_back(found);
                run_means.push_back(mean);
                i = j;
            }
        }
        // (a): the applied periods are cyclically consecutive in the 17 words of M_P, from at least
        // 18 runs (one full cycle of M_P plus one).
        bool cyclic_ok = win_ok && applied_periods.size() >= 18 &&
            std::find(applied_periods.begin(), applied_periods.end(), -1) == applied_periods.end() &&
            cyclic_consecutive(applied_periods, m_p_order);
        // (b) is its own assertion, 0.5 tick only.
        bool dur_ok = win_ok && !applied_periods.empty() && applied_periods.size() == run_means.size();
        if (dur_ok) for (size_t i = 0; i < applied_periods.size(); ++i) {
            if (applied_periods[i] < 0 || !DR.count(applied_periods[i])) { dur_ok = false; break; }
            if (std::fabs(run_means[i] - DR[applied_periods[i]]) > 0.5 * ticks(1)) { dur_ok = false; break; }
        }
        checks.note("P2: window has " + std::to_string(applied_periods.size()) + " applied-period "
            "runs (need >=18); cyclic-consecutive-in-M_P (via DR)=" + std::string(cyclic_ok ? "yes" : "no"));

        checks.expect(c.loaded && c.done.has_value() && cyclic_ok,
            "P2-A [p.149 Table 5-4 Modulating Period Only] 5-mod.c4,5-p149.c8,5-p149.c4,"
            "5-mod.c13,5-p149.c19,5-mod.c16,5-p150.c3,5-p150.c7,5-p150.c11,5-p150.c15,5-p150.c19,"
            "5-t54.w1.c3,5-t54.w2.c3,5-t54.w3.c3,5-t54.w4.c3,5-p151.c4,5-t55.b4,"
            "5-appA-p265.c25,A-adkcon.b04: the applied periods of the runs are "
            "cyclically consecutive in the 17 words of M_P (matched against R-RAMP's own durations), "
            "from at least 18 runs");
        if (applied_periods.empty()) {
            checks.record("P2-B [p.149]: not evaluated -- the window's applied-period-run "
                "population is empty (measured population: " + std::to_string(applied_periods.size()) + ")");
        } else {
            checks.expect(c.loaded && win_ok && dur_ok,
                "P2-B [p.149]: the mean complete-sample duration of each run equals D_c(P) of "
                "that run's word (R-RAMP), within 0.5 tick");
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
        if (p2c_pop == 0) {
            checks.record("P2-C [p.149,Table 5-4] 5-p149.c9: not evaluated -- the window's "
                "complete-run population is empty (measured population: " + std::to_string(p2c_pop) + ")");
        } else {
            checks.expect(c.loaded && win_ok && ramp_order.size() == 16 && ramp_seq_ok,
                "P2-C [p.149,Table 5-4] 5-p149.c9: channel 1's output follows RAMP's sixteen "
                "bytes in order at volume 48, none skipped or repeated, although the pitch changes");
        }
    }

    // ================= P3: whole word, not two bytes ======================
    {
        // S P3.3: graded in the P1 window (normal run) or, for the
        // contrasting cases, R-MOD's own [M_on, end of play] -- channel 0
        // unattached has no "away from 40" concept, since it is not
        // channel 1. P3-WHOLE grades R-MOD's channel 0 side (bytes), P3-SILENT
        // channel 1's side (silent in R-MOD).
        const bool m_sub = mutating("P3-WHOLE") || mutating("P3-SILENT");
        Capture ctrl = m_sub ? rmod : run_capture(dir + "/prog_s_av.bin", budget_for(200000));
        mut_note("P3-WHOLE", "prog_rmod.bin", ctrl);
        mut_note("P3-SILENT", "prog_rmod.bin", ctrl);
        auto ctrl_mk = markers(ctrl);
        auto con = m_on(ctrl_mk), coff = m_off(ctrl_mk);
        bool ctrl_ok = con && coff;
        int side_a = mutating("P3-WHOLE") ? side_of[0] : side_of[1];

        uint64_t p3_ws = 0, p3_we = 0;
        bool p3_win_ok = false;
        if (m_sub) {
            if (ctrl_ok && side_a >= 0) { p3_ws = *con; p3_we = play_end(ctrl.audio, side_a, *coff); p3_win_ok = p3_we > p3_ws; }
        } else if (ctrl_ok && side_a >= 0) {
            // P1's own window: from channel 0's third fetch to the end of play of channel 1's output.
            auto fc0w = fetch_cycles(ctrl.log, ch_base(0), 8, *con, *coff);
            if (fc0w.size() >= 3) { p3_ws = fc0w[2]; p3_we = play_end(ctrl.audio, side_a, *coff); p3_win_ok = p3_we > p3_ws; }
        }

        bool no_silent = false, one_pair_ok = false;
        size_t p3_pop = 0;
        if (p3_win_ok && side_a >= 0) {
            auto runs = runs_in_window(ctrl.audio, p3_ws, p3_we, side_a);
            auto segs = segment_by_pair(runs);
            // Word intervals are the intervals of applied words: the changes of channel 1's level pair
            // away from its written starting value (S-AV), or every segment where R-MOD's own output is
            // graded in place of it.
            if (m_sub) {
                p3_pop = segs.size();
            } else {
                int last_val = 40;
                for (auto& sg : segs) {
                    int found = -1;
                    for (auto& kv : L1) if (kv.second.first == sg.a && kv.second.second == sg.b) { found = kv.first; break; }
                    if (found != last_val) { ++p3_pop; last_val = found; }
                }
            }
            no_silent = !segs.empty();
            for (auto& s : segs) if (s.a == 0 && s.b == 0) { no_silent = false; break; }
            // "Exactly one level pair, the whole word's L_1(v)" -- every
            // SEGMENT (word interval), not every raw run, must match some
            // L1 entry exactly; a byte-split channel's segments (one member
            // always 0, the other a raw data byte) never match L1's
            // symmetric non-zero pairs.
            one_pair_ok = !segs.empty();
            for (auto& s : segs) {
                bool found = false;
                for (auto& kv : L1) if (kv.second.first == s.a && kv.second.second == s.b) { found = true; break; }
                if (!found) { one_pair_ok = false; break; }
            }
        }
        if (p3_pop == 0) {
            checks.record("P3-SILENT [p.149]: not evaluated -- the P1 window's word-interval population "
                "is empty (measured population: " + std::to_string(p3_pop) + ")");
            checks.record("P3-WHOLE [p.149] 5-mod.c8,5-mod.c9,5-p149.c14,5-p149.c15: not evaluated -- "
                "the P1 window's word-interval population is empty (measured population: " +
                std::to_string(p3_pop) + ")");
        } else {
            checks.expect(ctrl.loaded && p3_win_ok && no_silent,
                "P3-SILENT [p.149]: in the P1 window, no word interval is silent (volume 0 never occurs)");
            checks.expect(ctrl.loaded && p3_win_ok && one_pair_ok,
                "P3-WHOLE [p.149] 5-mod.c8,5-mod.c9,5-p149.c14,5-p149.c15: every word interval in the P1 "
                "window has exactly one level pair, the whole word's reference pair L_1(v) -- the "
                "modulator's whole word reaches the modulated channel and no intermediate byte-split value "
                "ever appears");
        }

        // The S-P2 half ("never a byte's D_1(P)") -- reuse P2's own
        // window (the same DMA-stop exclusion and reference-value opening
        // applied identically); verify no run duration matches any
        // byte-split period.
        {
            std::string p2bin = dir + "/prog_s_p2.bin";
            Capture p2c = run_capture(p2bin, budget_for(1400000));
            auto mk2 = markers(p2c); auto on2 = m_on(mk2), off2 = m_off(mk2);
            bool have2 = on2 && off2;
            double ref700 = DR.count(700) ? DR[700] : 0;
            uint64_t ws = 0, we = 0; bool wok = false;
            if (have2 && side_of[1] >= 0 && ref700 > 0) {
                auto runs = runs_in_window(p2c.audio, *on2, *off2, side_of[1]);
                auto comp = complete_runs(runs, *on2, *off2);
                bool seen700 = false;
                for (auto& r : comp) {
                    double dd = (double)(r.end - r.start);
                    if (!seen700) { if (std::fabs(dd - ref700) <= 0.5 * ticks(1)) seen700 = true; continue; }
                    bool matches_ref = false;
                    for (auto& kv : DR) { if (kv.first == 700) continue; if (std::fabs(dd - kv.second) <= 0.5 * ticks(1)) { matches_ref = true; break; } }
                    if (matches_ref) { ws = r.start; wok = true; break; }
                }
                we = play_end(p2c.audio, side_of[1], *off2);
            }
            bool no_byte_split = wok;
            size_t p3sp2_pop = 0;
            if (wok) {
                auto runs = runs_in_window(p2c.audio, ws, we, side_of[1]);
                auto comp = complete_runs(runs, ws, we);
                p3sp2_pop = comp.size();
                static const double bad[7] = {65, 129, 3, 33, 5, 17, 9};
                for (auto& r : comp) {
                    double dd = (double)(r.end - r.start);
                    for (double bp : bad) if (std::fabs(dd - ticks(bp)) <= ticks(1)) { no_byte_split = false; break; }
                    if (!no_byte_split) break;
                }
            }
            if (p3sp2_pop == 0) {
                checks.record("P3-SP2 [p.149,Table 5-4]: not evaluated -- the S-P2 window's run "
                    "population is empty (measured population: " + std::to_string(p3sp2_pop) + ")");
            } else {
                checks.expect(p2c.loaded && wok && no_byte_split,
                    "P3-SP2 [p.149,Table 5-4]: in S-P2, no run's duration matches a byte-split period "
                    "(65,129,3,33,5,17,9 ticks) -- a whole word is used, never its high byte alone");
            }
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
    {
        std::string bin = dir + (mutating("P4-C") || mutating("P4-B") || mutating("P4-A") ? "/prog_s_both_volonly.bin" : "/prog_s_both.bin");
        Capture c = run_capture(bin, budget_for(110000));
        mut_note("P4-A", bin, c);
        mut_note("P4-B", bin, c);
        mut_note("P4-C", bin, c);
        int side = side_of[1];
        uint16_t expect_mask = (bin.find("volonly") != std::string::npos) ? 0x01 : 0x11;
        auto mk = markers(c);
        auto on = m_on(mk), off = m_off(mk);
        bool have_bounds = on && off;
        assert_pc123(checks, "S-BOTH", c, expect_mask, 0x0003, {{0,8},{1,4}}, have_bounds, have_bounds ? *on : 0, have_bounds ? *off : 0);

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
                for (auto& kv : D1) if (std::fabs(kv.second - mean) <= 0.5*ticks(1)) {
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

        checks.expect(p4a_ok,
            "P4-A [p.149,p.150,p.265] 5-mod.c2,5-p149.c6,5-mod.c15,5-mod.c16,"
            "5-p150.c2,5-p150.c3,5-mod.c17,5-p150.c4,5-p150.c6,5-p150.c10,5-p150.c14,5-p150.c18,"
            "5-t54.w1.c2,5-t54.w2.c2,5-t54.w3.c2,5-t54.w4.c2,5-appA-p265.c30,5-appA-p265.c32,"
            "A-adkcon.note1: the merged sequence (events start only after the first genuine change) is "
            "cyclically consecutive in M_VP, alternating volume then period");
        // P4-B grades what the changes carry: it has a population only where a change of either kind
        // occurred.
        if (vol_events.empty() && per_events.empty()) {
            checks.record("P4-B [p.150]: not evaluated -- no applied volume and no applied period change in "
                "the window (measured populations: volume changes " + std::to_string(vol_events.size()) +
                ", period changes " + std::to_string(per_events.size()) + ")");
        } else {
            checks.expect(c.loaded && have_bounds && set_b,
                "P4-B [p.150]: after the first change of each kind, no applied volume lies outside "
                "{24,56,8,40} and no applied period lies outside {300,700,500,900}");
        }
        checks.expect(c.loaded && have_bounds && vol_events.size() >= 8 && per_events.size() >= 8,
            "P4-C [p.149]: both parameters change in the window, each at least 8 times");
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
        pc1_line(checks, "S-BOTH-volonly", s_both_volonly, 0x01);
        pc2_line(checks, "S-BOTH-volonly", s_both_volonly, 0x0003);
        pc3_lines(checks, "S-BOTH-volonly", s_both_volonly, {{0,8},{1,4}}, have_bounds, have_bounds ? *on : 0, have_bounds ? *off : 0);
        if (period_pop == 0) {
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
        checks.expect(p4a_ok && s_both_volonly.loaded && have_bounds && period_pop > 0 && period_const,
            "P4-JOINT [p.150] 5-mod.c14,5-p150.c1: modulating both period and volume requires both attach "
            "bits -- with both bits set, channel 1's volume and period follow M_VP (P4-A), and with only bit 0 "
            "set its period stays at D_1(200) (P4-VOLONLY)");
    }

    // ================= P5: time-out phase (S-AV/S-AP/S-BOTH) ==============
    {
        Capture av = run_capture(dir + (mutating("P5-AV") ? "/prog_s_av_per6000.bin" : "/prog_s_av.bin"), budget_for(200000));
        Capture ap = mutating("P5-AP") ? run_capture(dir + "/prog_s_ap_per6000.bin", budget_for(110000)) : sap_cap;
        Capture both = run_capture(dir + (mutating("P5-BOTH") ? "/prog_s_both_per6000.bin" : "/prog_s_both.bin"), budget_for(110000));
        mut_note("P5-AV", "prog_s_av_per6000.bin", av);
        mut_note("P5-AP", "prog_s_ap_per6000.bin", ap);
        mut_note("P5-BOTH", "prog_s_both_per6000.bin", both);
        int side = side_of[1];
        // Delta = C - F_k, graded per spec, against the page's relation (an
        // integer multiple of D_mod, within lambda plus a *measured*
        // granularity bound -- the largest reference duration in play,
        // never an invented additive constant).
        double eps5 = D1.count(900) ? D1[900] : (D1.count(700) ? D1[700] : 0);
        auto grid_ok = [&](std::vector<std::tuple<uint64_t,int,int>>& changes, // (time, pos, value) -- volume-only or period-only
                           const std::vector<FetchRec>& fr, double dmod) {
            if (changes.empty() || dmod <= 0) return false;
            for (auto& [ctime, pos, val] : changes) {
                if (pos < 0) return false;
                auto fk = latest_fetch_before(fr, (uint32_t)pos, ctime);
                if (!fk) return false;
                double delta = (double)ctime - (double)*fk;
                long m = (long)std::llround(delta / dmod);
                if (m < 1) m = 1;
                double lo = m * dmod - lambda, hi = m * dmod + lambda + eps5;
                if (!(delta >= lo && delta <= hi)) return false;
            }
            return true;
        };
        // Consecutive-change spacing: S P5.4's own ordering -- successive
        // changes are 2*D_mod apart with one attach bit, D_mod apart with
        // both, within lambda + eps (a changed period shows only in the first sample that has it).
        auto consec_ok = [&](std::vector<std::tuple<uint64_t,int,int>>& changes, double step) {
            if (changes.size() < 2 || step <= 0) return false;
            for (size_t i = 1; i < changes.size(); ++i) {
                double gap = (double)std::get<0>(changes[i]) - (double)std::get<0>(changes[i - 1]);
                if (std::fabs(gap - step) > lambda + eps5) return false;
            }
            return true;
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
        std::vector<std::tuple<uint64_t,int,int>> av_changes, ap_changes, both_changes;
        if (av_win && side >= 0) {
            auto runs = runs_in_window(av.audio, av_ws, av_we, side);
            auto segs = segment_by_pair(runs);
            // The first segment starts at the window's edge and shows the value in force there; it is not a change.
            int last_val = 40;
            bool in_force = true;
            for (auto& s : segs) for (auto& kv : L1) if (kv.second.first == s.a && kv.second.second == s.b) {
                if (in_force) { last_val = kv.first; in_force = false; break; }
                if (kv.first != last_val) { av_changes.push_back({s.start, m_v_pos(kv.first), kv.first}); last_val = kv.first; }
                break;
            }
        }
        if (ap_win && side >= 0) {
            auto runs = runs_in_window(ap.audio, ap_ws, ap_we, side);
            auto samples = samples_by_sign(runs);
            auto comp = complete_samples(samples, ap_ws, ap_we);
            int last_per = 200;
            bool in_force = true;
            size_t i = 0;
            while (i < comp.size()) {
                size_t j = i; uint64_t dur0 = comp[i].end - comp[i].start;
                while (j < comp.size() && (uint64_t)std::llabs((long long)(comp[j].end-comp[j].start)-(long long)dur0) <= (uint64_t)kClkRes) ++j;
                double mean = 0; for (size_t k=i;k<j;++k) mean += (double)(comp[k].end-comp[k].start); mean /= (double)(j-i);
                for (auto& kv : D1) if (std::fabs(kv.second - mean) <= 0.5*ticks(1)) {
                    if (in_force) { last_per = kv.first; in_force = false; break; }
                    if (kv.first != last_per) { ap_changes.push_back({comp[i].start, m_p4_pos(kv.first), kv.first}); last_per = kv.first; }
                    break;
                }
                i = j;
            }
        }
        if (both_win && side >= 0) {
            auto runs = runs_in_window(both.audio, both_ws, both_we, side);
            auto segs = segment_by_pair(runs);
            int last_val = 63;
            bool in_force = true;
            for (auto& s : segs) for (auto& kv : L1) if (kv.second.first == s.a && kv.second.second == s.b) {
                if (in_force) { last_val = kv.first; in_force = false; break; }
                if (kv.first != last_val) { both_changes.push_back({s.start, m_vp_pos(false, kv.first), kv.first}); last_val = kv.first; }
                break;
            }
            auto samples = samples_by_sign(runs);
            auto comp = complete_samples(samples, both_ws, both_we);
            int last_per = 200;
            bool in_force_p = true;
            size_t i = 0;
            while (i < comp.size()) {
                size_t j = i; uint64_t dur0 = comp[i].end - comp[i].start;
                while (j < comp.size() && (uint64_t)std::llabs((long long)(comp[j].end-comp[j].start)-(long long)dur0) <= (uint64_t)kClkRes) ++j;
                double mean = 0; for (size_t k=i;k<j;++k) mean += (double)(comp[k].end-comp[k].start); mean /= (double)(j-i);
                for (auto& kv : D1) if (std::fabs(kv.second - mean) <= 0.5*ticks(1)) {
                    if (in_force_p) { last_per = kv.first; in_force_p = false; break; }
                    if (kv.first != last_per) { both_changes.push_back({comp[i].start, m_vp_pos(true, kv.first), kv.first}); last_per = kv.first; }
                    break;
                }
                i = j;
            }
            std::sort(both_changes.begin(), both_changes.end());
        }
        // F_k is looked up in the whole record of channel 0's own data set (from M_on): the word a change
        // carries may have been fetched before the window opens.
        auto fr_av = (av_win && on_a && off_a) ? fetch_records(av.log, ch_base(0), 8, *on_a, *off_a) : std::vector<FetchRec>{};
        auto fr_ap = (ap_win && on_p && off_p) ? fetch_records(ap.log, ch_base(0), 4, *on_p, *off_p) : std::vector<FetchRec>{};
        auto fr_both = (both_win && on_b && off_b) ? fetch_records(both.log, ch_base(0), 8, *on_b, *off_b) : std::vector<FetchRec>{};

        // The destination half of the AUDxAP joint line (P8): in S-AP, channel 1's applied periods
        // follow M_P4 cyclically, from at least 2L = 8 changes.
        if (!mutating("P5-AP")) {
            sap_dest_pop = ap_changes.size();
            std::vector<int> seqp;
            for (auto& ch : ap_changes) seqp.push_back(std::get<2>(ch));
            sap_dest_ok = ap.loaded && ap.done.has_value() && ap_win && seqp.size() >= 8 && cyclic_consecutive(seqp, m_p4_order);
        }

        // Population precondition (S P5.5): at least 2L changes of
        // channel 1 (L=4, SQ's own length) in the window. An empty
        // population (0 genuine changes) is "not evaluated", never FAIL.
        if (av_changes.empty()) {
            checks.record("P5-POP-AV [p.149]: not evaluated -- S-AV's applied-change "
                "population is empty (measured population: 0)");
        } else {
            checks.expect(av.loaded && av_win && av_changes.size() >= 8,
                "P5-POP-AV [p.149]: S-AV -- the window (from channel 0's third fetch) has "
                ">=2L=8 changes of channel 1");
        }
        if (ap_changes.empty()) {
            checks.record("P5-POP-AP [p.149]: not evaluated -- S-AP's applied-change "
                "population is empty (measured population: 0)");
        } else {
            checks.expect(ap.loaded && ap_win && ap_changes.size() >= 8,
                "P5-POP-AP [p.149]: S-AP -- the window has >=2L=8 changes of channel 1");
        }
        if (both_changes.empty()) {
            checks.record("P5-POP-BOTH [p.149]: not evaluated -- S-BOTH's applied-change "
                "population is empty (measured population: 0)");
        } else {
            checks.expect(both.loaded && both_win && both_changes.size() >= 8,
                "P5-POP-BOTH [p.149]: S-BOTH -- the window has >=2L=8 changes of channel 1");
        }

        bool av_ok = grid_ok(av_changes, fr_av, d_mod) && consec_ok(av_changes, 2.0 * d_mod);
        bool ap_ok = grid_ok(ap_changes, fr_ap, d_mod) && consec_ok(ap_changes, 2.0 * d_mod);
        bool both_ok = grid_ok(both_changes, fr_both, d_mod) && consec_ok(both_changes, d_mod);
        checks.note("P5: S-AV " + std::to_string(av_changes.size()) + " changes, S-AP " +
            std::to_string(ap_changes.size()) + ", S-BOTH " + std::to_string(both_changes.size()) +
            "; av_ok=" + std::to_string(av_ok) + " ap_ok=" + std::to_string(ap_ok) +
            " both_ok=" + std::to_string(both_ok));
        if (av_changes.empty()) {
            checks.record("P5-AV [p.149,p.164] 5-mod.c10,5-p149.c16: not evaluated -- S-AV's "
                "applied-change population is empty (measured population: 0)");
        } else {
            checks.expect(av.loaded && av_win && av_ok,
                "P5-AV [p.149,p.164] 5-mod.c10,5-p149.c16: S-AV -- for every change, Delta=C-F_k lies "
                "within [m*D_mod-lambda, m*D_mod+lambda+eps] for some integer m>=1, and consecutive "
                "changes are 2*D_mod apart (as normal, 011->010)");
        }
        if (ap_changes.empty()) {
            checks.record("P5-AP [p.164,p.165] 5-p165.c18,5-sig.pbufld2: not evaluated -- S-AP's "
                "applied-change population is empty (measured population: 0)");
        } else {
            checks.expect(ap.loaded && ap_win && ap_ok,
                "P5-AP [p.164,p.165] 5-p165.c18,5-sig.pbufld2: S-AP -- Delta=C-F_k on the same grid, "
                "consecutive changes 2*D_mod apart (010->011)");
        }
        if (both_changes.empty()) {
            checks.record("P5-BOTH [p.164]: not evaluated -- S-BOTH's applied-change population is "
                "empty (measured population: 0)");
        } else {
            checks.expect(both.loaded && both_win && both_ok,
                "P5-BOTH [p.164]: S-BOTH -- Delta=C-F_k on the same grid, consecutive changes D_mod "
                "apart, alternating volume and period (both transitions)");
        }
    }

    // ================= P6: each attach bit pairs x with x+1 ================
    {
        struct Pair6 { int b; int mod_ch, tgt_ch, trip_a, trip_b; bool is_vol; std::string bin, noattach, extra, tgt; std::string extra_clause; };
        std::vector<Pair6> cases = {
            {0,0,1,2,3,true,"/prog_s_pair0.bin","/prog_s_pair0_noattach.bin","/prog_s_pair0_extra.bin","/prog_s_pair0_tgtper.bin","(c)"},
            {1,1,2,0,3,true,"/prog_s_pair1.bin","/prog_s_pair1_noattach.bin","","",""},
            {2,2,3,0,1,true,"/prog_s_pair2.bin","/prog_s_pair2_noattach.bin","/prog_s_pair2_extra.bin","","(c)"},
            {4,0,1,2,3,false,"/prog_s_pair4.bin","/prog_s_pair4_noattach.bin","/prog_s_pair4_extra.bin","/prog_s_pair4_tgtvol.bin","(d)"},
            {5,1,2,0,3,false,"/prog_s_pair5.bin","/prog_s_pair5_noattach.bin","/prog_s_pair5_extra.bin","","(d)"},
            {6,2,3,0,1,false,"/prog_s_pair6.bin","/prog_s_pair6_noattach.bin","/prog_s_pair6_extra.bin","","(d)"},
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
            std::string bin = mutating(tag) ? pc.noattach
                            : (mutating(tag + "-EXTRA") && !pc.extra.empty()) ? pc.extra
                            : (mutating("P6-TGT-" + std::to_string(pc.b)) && !pc.tgt.empty()) ? pc.tgt : pc.bin;
            Capture c = run_capture(dir + bin, budget_for(110000));
            mut_note(tag, bin, c);
            mut_note(tag + "-EXTRA", bin, c);
            mut_note("P6-TGT-" + std::to_string(pc.b), bin, c);
            int tside = side_of[pc.tgt_ch], oside = other_side(pc.tgt_ch);
            uint16_t bitval = (uint16_t)(1u << pc.b);
            uint16_t expect_mask = mutating(tag) ? 0x00 : bitval;
            auto mk = markers(c); auto on = m_on(mk), off = m_off(mk);
            bool have_bounds = on && off;
            // Length per channel: the modulator (M6V/M_P4, 4 words) and the
            // target (SQ, 4 words) both step through 4; the two tripwires
            // (W_V/W_P) step through 2.
            uint32_t len_by_ch[4] = {2, 2, 2, 2};
            len_by_ch[pc.mod_ch] = 4;
            len_by_ch[pc.tgt_ch] = 4;
            assert_pc123(checks, tag, c, expect_mask, 0x000F,
                         {{0,len_by_ch[0]},{1,len_by_ch[1]},{2,len_by_ch[2]},{3,len_by_ch[3]}},
                         have_bounds, have_bounds ? *on : 0, have_bounds ? *off : 0);

            auto mod_fetches = have_bounds ? fetch_cycles(c.log, ch_base(pc.mod_ch), 4, *on, *off) : std::vector<uint64_t>{};
            uint64_t win_start = 0; bool win_ok = have_bounds && mod_fetches.size() >= 3;
            if (win_ok) win_start = mod_fetches[2];
            // Samples, runs and levels of the target's output end before the sample the DMA stop
            // cuts short; the fetch records and the constant outputs end at M_off.
            uint64_t win_end = have_bounds ? play_end(c.audio, tside, *off) : 0;
            uint64_t fetch_end = have_bounds ? *off : 0;

            std::vector<int> applied; bool level_ok = true;
            auto& table = (pc.tgt_ch == 1) ? L1 : (pc.tgt_ch == 2) ? L2 : L3;
            auto& dtable = (pc.tgt_ch == 1) ? D1 : (pc.tgt_ch == 2) ? D2t : D3t;
            if (win_ok && tside >= 0) {
                auto runs = runs_in_window(c.audio, win_start, win_end, tside);
                if (pc.is_vol) {
                    auto segs = segment_by_pair(runs);
                    for (auto& s : segs) {
                        int found = -1;
                        for (auto& kv : table) if (kv.second.first == s.a && kv.second.second == s.b) { found = kv.first; break; }
                        if (found < 0) level_ok = false;
                        applied.push_back(found);
                    }
                } else {
                    auto comp = complete_runs(runs, win_start, win_end);
                    size_t i = 0;
                    while (i < comp.size()) {
                        size_t j = i; uint64_t dur0 = comp[i].end - comp[i].start;
                        while (j < comp.size() && (uint64_t)std::llabs((long long)(comp[j].end-comp[j].start)-(long long)dur0) <= (uint64_t)kClkRes) ++j;
                        double mean = 0; for (size_t k=i;k<j;++k) mean += (double)(comp[k].end-comp[k].start); mean /= (double)(j-i);
                        int found = -1;
                        for (auto& kv : dtable) if (std::fabs(kv.second - mean) <= 0.5*ticks(1)+kClkRes) { found = kv.first; break; }
                        if (found < 0) level_ok = false;
                        applied.push_back(found);
                        i = j;
                    }
                }
            }
            std::vector<int> order6 = pc.is_vol ? std::vector<int>{8,48,20,60} : m_p4_order;
            bool cyclic_ok = win_ok && applied.size() >= 9 && level_ok && cyclic_consecutive(applied, order6);
            std::string ids = bit_ids.count(pc.b) ? bit_ids[pc.b] : "";
            checks.expect(c.loaded && win_ok && cyclic_ok,
                pc.is_vol
                ? ("P6-A [p.149,p.150,p.151,p.265]" + (ids.empty() ? "" : (" " + ids)) + " bit " + std::to_string(pc.b) +
                   ": the target's applied volumes are cyclically consecutive in M6V, every level "
                   "pair a reference pair of the target")
                : ("P6-A [p.149,p.150,p.151,p.265]" + (ids.empty() ? "" : (" " + ids)) + " bit " + std::to_string(pc.b) +
                   ": the target's applied periods are cyclically consecutive in M_P4"));

            // (b): durations (volume sessions) / level pair (period
            // sessions) stay at their own reference, and per-session
            // preconditions (already added above via assert_pc123).
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
            checks.expect(c.loaded && win_ok && b_ok,
                pc.is_vol
                ? ("P6-B [p.149,p.150] bit " + std::to_string(pc.b) + ": the target's durations stay at D_c(200)")
                : ("P6-B [p.149,p.150] bit " + std::to_string(pc.b) + ": the target's level pair stays at the reference for VOL 40"));

            // (c) uses win_start (the modulator's third fetch), not M_on.
            bool other_const = win_ok && oside >= 0 && fetch_end > win_start && is_constant(c.audio, win_start, fetch_end, oside);
            checks.expect(c.loaded && win_ok && other_const,
                "P6-C [p.150] bit " + std::to_string(pc.b) + ": the other side's output is constant "
                "for the whole window");

            // (d): every channel but the target keeps fetching at 2*AUDxPER as written: the modulator
            // (4 words, PER 4000) and both tripwires (2 words, PER 1400..2000).
            bool spacing_ok = true;
            if (win_ok) {
                auto check_trip = [&](int ch, uint32_t span, double per) {
                    auto fc = fetch_cycles(c.log, ch_base(ch), span, win_start, fetch_end);
                    if (fc.size() < 2) return false;
                    for (size_t i = 1; i < fc.size(); ++i)
                        if (std::fabs((double)(fc[i]-fc[i-1]) - 2*ticks(per)) > lambda) return false;
                    return true;
                };
                double pers[4] = {1400,1600,1800,2000};
                if (!check_trip(pc.mod_ch, 4, 4000)) spacing_ok = false;
                if (!check_trip(pc.trip_a, 2, pers[pc.trip_a])) spacing_ok = false;
                if (!check_trip(pc.trip_b, 2, pers[pc.trip_b])) spacing_ok = false;
            }
            checks.expect(c.loaded && win_ok && spacing_ok,
                "P6-D [p.164] bit " + std::to_string(pc.b) + ": every non-target channel's fetch spacing (the "
                "modulator's and both tripwires', from the modulator's third fetch) equals 2*AUDxPER within lambda");
        }
    }

    // ================= P7: channel 3 attaches to nothing ===================
    {
        // Per-bit ids: 5-appA-p265.c22/A-adkcon.b03 name ATVOL3
        // (bit 3) specifically; c26/b07 name ATPER3 (bit 7).
        struct P7Case { std::string bin, mutbin; uint16_t mask; std::string tag, ids; };
        std::vector<P7Case> cases = {
            {"/prog_s_ch3v.bin", "/prog_s_ch3v_extra.bin", 0x08, "P7V", "5-appA-p265.c22,A-adkcon.b03"},
            {"/prog_s_ch3p.bin", "/prog_s_ch3p_extra.bin", 0x80, "P7P", "5-appA-p265.c26,A-adkcon.b07"},
        };
        checks.record("5-mod.c23, 5-p150.c26: the sentence says an attached channel always modulates the next "
            "higher channel, so channel 3 cannot be attached, and p.150 adds that its modulate bits only disable "
            "its audio output; the lines below grade that nothing is modulated, and the output half is P8's, so "
            "neither id is asserted.");
        for (auto& pc : cases) {
            std::string bin = mutating(pc.tag) ? pc.mutbin : pc.bin;
            Capture c = run_capture(dir + bin, budget_for(60000));
            mut_note(pc.tag, bin, c);
            uint16_t expect = pc.mask;
            auto mk = markers(c); auto on = m_on(mk), off = m_off(mk);
            bool have_bounds = on && off;
            assert_pc123(checks, pc.tag, c, expect, 0x000F, {{0,2},{1,2},{2,2},{3,2}}, have_bounds, have_bounds ? *on : 0, have_bounds ? *off : 0);
            auto ch3f = have_bounds ? fetch_cycles(c.log, ch_base(3), 2, *on, *off) : std::vector<uint64_t>{};
            uint64_t win_start = have_bounds && ch3f.size() >= 3 ? ch3f[2] : 0;
            uint64_t win_end = have_bounds ? *off : 0;
            bool win_ok = have_bounds && ch3f.size() >= 3 && win_end > win_start;
            bool both_const = win_ok && is_constant(c.audio, win_start, win_end, 0) && is_constant(c.audio, win_start, win_end, 1);
            checks.expect(c.loaded && win_ok && both_const,
                "P7-CONST [p.150,p.151,p.265] " + pc.ids + ": both "
                "outputs are constant for the whole window -- channel 3's attach bit modulates "
                "nothing");
            bool spacing_ok = true;
            double pers[4] = {1400,1600,1800,2000};
            if (win_ok) {
                for (int ch = 0; ch < 4; ++ch) {
                    auto fc = fetch_cycles(c.log, ch_base(ch), 2, win_start, win_end);
                    if (fc.size() < 2) { spacing_ok = false; continue; }
                    for (size_t i = 1; i < fc.size(); ++i)
                        if (std::fabs((double)(fc[i]-fc[i-1]) - 2*ticks(pers[ch])) > lambda) spacing_ok = false;
                }
            }
            checks.expect(c.loaded && win_ok && spacing_ok,
                "P7-SPACING [p.164]: every channel's consecutive fetch spacing equals 2*AUDxPER within "
                "lambda (2800,3200,3600,4000 ticks)");
        }
    }

    // ================= P8: an attached channel's own DAC is bypassed ======
    {
        struct P8Case { int ch; std::string bin; std::string ids; };
        std::vector<P8Case> cases = {
            {0, "/prog_s_av.bin",   ""},
            {0, "/prog_s_ap.bin",   ""},
            {0, "/prog_s_both.bin", ""},
            {3, "/prog_s_dac3v.bin","A-adkcon.b03"},
            {3, "/prog_s_dac3p.bin","A-adkcon.b07"},
        };
        bool sap_src_const = false;
        for (auto& pc : cases) {
            const bool omit = mutating("P8-OMIT");
            // The contrast omits the attach write: the matching unattached reference session runs in
            // the attached one's place (its session lines are not printed for it).
            std::string bin = pc.bin;
            Capture c;
            if (omit) {
                c = pc.ch == 0 ? rmod : (pc.bin == "/prog_s_dac3v.bin" ? rdac3v : rdac3p);
            } else {
                c = (pc.bin == "/prog_s_ap.bin") ? sap_cap : run_capture(dir + pc.bin, budget_for(200000));
            }
            int side = side_of[pc.ch];
            auto mk = markers(c); auto on = m_on(mk), off = m_off(mk);
            bool have_bounds = on && off;
            // S-AP and S-DAC3-V/S-DAC3-P get no PC1-3 assertions anywhere
            // else in this file (S-AV, S-BOTH already do, in P1 and P4).
            if (!omit) {
                if (pc.bin == "/prog_s_ap.bin")
                    assert_pc123(checks, "S-AP", c, 0x10, 0x0003, {{0,4},{1,4}}, have_bounds, have_bounds ? *on : 0, have_bounds ? *off : 0);
                else if (pc.bin == "/prog_s_dac3v.bin")
                    assert_pc123(checks, "S-DAC3-V", c, 0x08, 0x0008, {{3,2}}, have_bounds, have_bounds ? *on : 0, have_bounds ? *off : 0);
                else if (pc.bin == "/prog_s_dac3p.bin")
                    assert_pc123(checks, "S-DAC3-P", c, 0x80, 0x0008, {{3,2}}, have_bounds, have_bounds ? *on : 0, have_bounds ? *off : 0);
            }
            auto fc = have_bounds ? fetch_cycles(c.log, ch_base(pc.ch), pc.ch == 0 ? 17 : 2, *on, *off) : std::vector<uint64_t>{};
            uint64_t win_start = have_bounds && fc.size() >= 3 ? plus_La(fc[2]) : 0;
            uint64_t win_end = have_bounds ? play_end(c.audio, side, *off) : 0;
            bool win_ok = have_bounds && fc.size() >= 3 && side >= 0 && win_end > win_start;
            bool constant = win_ok && is_constant(c.audio, win_start, win_end, side);
            if (pc.bin == "/prog_s_ap.bin") sap_src_const = constant;
            mut_note("P8-OMIT", pc.bin + " replaced by its unattached reference session", c);
            checks.expect(c.loaded && win_ok && constant,
                "P8-ATT [p.165,p.151,p.265]" + (pc.ids.empty() ? "" : (" " + pc.ids)) + " channel " + std::to_string(pc.ch) +
                "'s own side has one value for the whole window while it is attached (" + pc.bin + ")");
        }
        // The unattached control -- the same channel, same data, period
        // and volume, changes its own side >=2x per data word.
        struct CtrlCase { int ch; const Capture* c; uint32_t len; std::string name; };
        Capture rmod_ctrl = mutating("P8-PRE") ? run_capture(dir + "/prog_rmod_vol0.bin", budget_for(200000)) : rmod;
        mut_note("P8-PRE", "prog_rmod_vol0.bin", rmod_ctrl);
        std::vector<CtrlCase> ctrls = {
            {0, &rmod_ctrl, 8, "R-MOD"}, {0, &rdacp4, 4, "R-DAC-P4"}, {0, &rdacvp, 8, "R-DAC-VP"},
            {3, &rdac3v, 2, "R-DAC3-V"}, {3, &rdac3p, 2, "R-DAC3-P"},
        };
        for (auto& cc : ctrls) {
            auto mk = markers(*cc.c); auto on = m_on(mk), off = m_off(mk);
            bool have_bounds = on && off;
            int side = side_of[cc.ch];
            bool changes_enough = false;
            if (have_bounds && side >= 0) {
                auto runs = runs_in_window(cc.c->audio, *on, *off, side);
                auto comp = complete_runs(runs, *on, *off);
                changes_enough = comp.size() >= 2 * cc.len;
            }
            checks.expect(cc.c->loaded && cc.c->done.has_value() && have_bounds && changes_enough,
                "P8-PRE [reference frame]: " + cc.name + " -- the same channel, unattached, "
                "changes its own side at least twice per data word");
        }
        // The AUDxAP joint line (5-p165.c20, 5-sig.audxap): both halves of "send data to the period
        // latch of the next channel instead of to the D->A converter", in S-AP -- channel 1's period
        // follows channel 0's words (destination, P5's S-AP window) and channel 0's own side holds one
        // value (source).
        checks.expect(sap_dest_ok && sap_src_const,
            "P8-JOINT-AP [p.165] 5-p165.c20,5-sig.audxap: in S-AP, channel 1's applied periods follow "
            "channel 0's words cyclically in M_P4 (destination half, measured applied-period changes: " +
            std::to_string(sap_dest_pop) + ") AND channel 0's own side holds one value once its data is "
            "flowing (source half) -- both halves of the AUDxAP claim, one session");
    }

    // ================= P9: data-set lengths are independent ===============
    {
        struct P9Case { std::string bin, mutbin; std::vector<int> mod_order; int lm, lc; };
        std::vector<P9Case> cases = {
            {"/prog_s_lena.bin", "/prog_s_lena_len4.bin", {8,48,20,60,32}, 5, 3},
            {"/prog_s_lenb.bin", "", {8,48}, 2, 7},
        };
        for (auto& pc : cases) {
            bool is_a = !pc.mutbin.empty();
            // The contrasts change the stimulus of S-LEN-A only; every line below grades with the page's
            // own lengths (L_m = 5, L_c = 3).
            std::string bin = pc.bin;
            if (is_a && (mutating("P9-A") || mutating("P9-C"))) bin = pc.mutbin;
            if (is_a && mutating("P9-B")) bin = "/prog_s_lena_ch1len2.bin";
            Capture c = run_capture(dir + bin, budget_for(150000));
            mut_note("P9-A", bin, c);
            mut_note("P9-B", bin, c);
            mut_note("P9-C", bin, c);
            int side = side_of[1];
            auto mk = markers(c); auto on = m_on(mk), off = m_off(mk);
            bool have_bounds = on && off;
            assert_pc123(checks, is_a ? "S-LEN-A" : "S-LEN-B", c, 0x01, 0x0003,
                         {{0, (uint32_t)pc.lm}, {1, (uint32_t)pc.lc}},
                         have_bounds, have_bounds ? *on : 0, have_bounds ? *off : 0);
            // The window opens after EACH channel's own first restart (PC3's
            // own anchor, SPECIFICATION.md) -- independent of any value
            // either channel plays. A scan for "departure from 40" is fooled
            // by a first-pass overrun on channel 1's own data region
            // (graded in hrm/ch5-audio-dma-first-pass, not here), which can
            // spuriously open the window inside that overrun.
            auto fr0_anchor = have_bounds ? fetch_records(c.log, ch_base(0), kChannelRegionWords, *on, *off) : std::vector<FetchRec>{};
            auto fr1_anchor = have_bounds ? fetch_records(c.log, ch_base(1), kChannelRegionWords, *on, *off) : std::vector<FetchRec>{};
            auto restart0 = first_restart_cycle(fr0_anchor);
            auto restart1 = first_restart_cycle(fr1_anchor);
            uint64_t win_start = (restart0 && restart1) ? std::max(*restart0, *restart1) : 0;
            uint64_t win_end = have_bounds ? play_end(c.audio, side, *off) : 0;
            bool win_ok = have_bounds && restart0.has_value() && restart1.has_value() && win_end > win_start;

            auto off0 = win_ok ? fetch_offsets(c.log, ch_base(0), pc.lm, win_start, *off) : std::vector<uint32_t>{};
            auto off1 = win_ok ? fetch_offsets(c.log, ch_base(1), pc.lc, win_start, *off) : std::vector<uint32_t>{};
            // Grade the stepping itself (previous+1 mod L), from the
            // channel's own first restart on.
            bool off0_ok = stepping_cycles(off0, (uint32_t)pc.lm) >= 0;
            bool off1_ok = stepping_cycles(off1, (uint32_t)pc.lc) >= 0;
            const std::string which = is_a ? "S-LEN-A" : "S-LEN-B";
            bool window_met = win_ok && !off0.empty() && !off1.empty();
            dep_expect(checks, true, window_met,
                "P9-WINDOW [p.147] " + which + ": the window opens after each channel's own first "
                "restart, with fetches recorded for both channels", "");

            // (c): applied-volume CHANGES only -- an unchanging segment
            // still at channel 1's own written starting VOL 40 is not a
            // change and must not be counted as one.
            std::vector<int> applied; bool level_ok = true;
            if (win_ok && side >= 0) {
                auto runs = runs_in_window(c.audio, win_start, win_end, side);
                auto segs = segment_by_pair(runs);
                int last_val = 40;
                for (auto& s : segs) {
                    int found = -1;
                    for (auto& kv : L1) if (kv.second.first == s.a && kv.second.second == s.b) { found = kv.first; break; }
                    if (found < 0) { level_ok = false; continue; }
                    if (found != last_val) { applied.push_back(found); last_val = found; }
                }
            }
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
            if (!window_met) {
                not_graded(checks, "P9-A", "P9-WINDOW (" + which + ")");
            } else {
                checks.expect(c.loaded && off0_ok,
                    "P9-A [p.147,p.150]: channel 0's offsets step 0.." + std::to_string(pc.lm - 1) +
                    ",0,... (" + bin + ")");
            }
            if (!window_met) {
                not_graded(checks, "P9-B", "P9-WINDOW (" + which + ")");
            } else {
                checks.expect(c.loaded && off1_ok,
                    "P9-B [p.147,p.150]: channel 1's offsets step 0.." + std::to_string(pc.lc-1) +
                    ",0,...(" + bin + ")");
            }
            if (!window_met) {
                not_graded(checks, "P9-C", "P9-WINDOW (" + which + ")");
            } else if (applied.empty()) {
                checks.record("P9-C [p.150] 5-mod.c18,5-p150.c21: not evaluated -- the window's "
                    "applied-volume-change population is empty (measured population: " +
                    std::to_string(applied.size()) + ") (" + bin + ")");
            } else {
                checks.expect(c.loaded && cyclic_ok,
                    "P9-C [p.150] 5-mod.c18,5-p150.c21: channel 1's applied volumes are cyclically "
                    "consecutive across the modulator's own restarts, none skipped or repeated (" + bin + ")");
            }
            if (!window_met) {
                not_graded(checks, "P9-D", "P9-WINDOW (" + which + ")");
            } else if (dur_pop == 0) {
                checks.record("P9-D: not evaluated -- the window's complete-sample population is "
                    "empty (measured population: " + std::to_string(dur_pop) + ") (" + bin + ")");
            } else {
                checks.expect(c.loaded && dur_ok,
                    "P9-D: channel 1's durations (graded at SQ's sign alternations) stay at D_1(200) "
                    "(" + bin + ")");
            }
        }
    }

    // ================= P10: DMA-request phase ==============================
    // p.164: F_1 is thrown away, F_2 arrives and enters state 010 (the record entry PC7-CAL calibrates), and
    // the main loop continues; the n-th fetch of the page is record entry r + n - 2 in every session, because
    // the start-up is the same in every session. Every line below grades only fetches from the page's F_3 on,
    // in that frame, and prints NOT GRADED where the frame is not established.
    {
        auto frame_met = [&](const std::vector<FetchRec>& rec) -> bool {
            return cal_ok && d_mod > 0 && rec.size() > cal.r + 3 && rec[cal.r].offset == 0;
        };
        // F_n - F_2 = k(n) * D_mod, within lambda, from n = 3 on; k(n) = 2(n-3) (as in normal operation).
        auto phase_normal_ok = [&](const std::vector<FetchRec>& rec) -> bool {
            for (size_t i = cal.r + 1; i < rec.size(); ++i) {
                double n = (double)(i - cal.r) + 2.0;
                double o = (double)rec[i].cycle - (double)rec[cal.r].cycle - 2.0 * (n - 3.0) * d_mod;
                if (std::fabs(o) > lambda) return false;
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
        // Attach period: the phase is R-MOD's plus D_mod (mod 2*D_mod), each fetch within lambda.
        auto phase_ap_ok = [&](const std::vector<FetchRec>& rec) -> bool {
            for (double ph : phases(rec)) {
                double d = std::fmod(ph - rmod_phase - d_mod, 2.0 * d_mod);
                if (d > d_mod) d -= 2.0 * d_mod;
                if (d <= -d_mod) d += 2.0 * d_mod;
                if (std::fabs(d) > lambda) return false;
            }
            return true;
        };
        // Both bits: consecutive fetches from the page's F_3 on are D_mod apart, within lambda.
        auto spacing_both_ok = [&](const std::vector<FetchRec>& rec) -> bool {
            for (size_t i = cal.r + 2; i < rec.size(); ++i) {
                double gap = (double)rec[i].cycle - (double)rec[i - 1].cycle;
                if (std::fabs(gap - d_mod) > lambda) return false;
            }
            return true;
        };
        Capture av = run_capture(dir + (mutating("P10-AV") ? "/prog_s_av_per6000.bin" : "/prog_s_av.bin"), budget_for(200000));
        Capture ap = mutating("P10-AP") ? run_capture(dir + "/prog_s_av.bin", budget_for(200000)) : sap_cap;
        Capture both = mutating("P10-BOTH") ? run_capture(dir + "/prog_s_av.bin", budget_for(200000))
                                            : run_capture(dir + "/prog_s_both.bin", budget_for(110000));
        mut_note("P10-AV", "prog_s_av_per6000.bin", av);
        mut_note("P10-AP", "prog_s_av.bin in place of prog_s_ap.bin", ap);
        mut_note("P10-BOTH", "prog_s_av.bin in place of prog_s_both.bin", both);
        auto av_rec = rec_of(av), ap_rec = rec_of(ap), both_rec = rec_of(both);
        dep_expect(checks, frame_met(av_rec), av.loaded && av.done.has_value() && phase_normal_ok(av_rec),
            "P10-AV [p.164,p.165] 5-p164.c33,5-sm.c33: S-AV -- in the calibrated frame F_n - F_2 lies within lambda of "
            "2(n-3)*D_mod for every n >= 3, 'as they do in normal operation' (011->010)",
            "PC7-CAL and a start-up frame in S-AV");
        // F58.loop.attach kept only here (the attach-period bracket S-AP
        // alone exercises); napnav's DMA-half is recorded, not
        // re-asserted (it is the same formula PC7 already grades).
        dep_expect(checks, frame_met(ap_rec), ap.loaded && ap.done.has_value() && phase_ap_ok(ap_rec),
            "P10-AP [p.164] 5-p164.c34,5-sm.c34,F58.loop.attach: S-AP -- the phase of the fetches (F_n - F_2 mod "
            "2*D_mod, n >= 3) is R-MOD's plus D_mod, within lambda (010->011)",
            "PC7-CAL and a start-up frame in S-AP");
        checks.record("5-p165.c22, 5-sig.napnav: the DMA-request half is graded only through the phases above "
            "(P10-AV, P10-AP, P10-BOTH); no assertion isolates napnav's own contribution beyond them, and the "
            "interrupt half is not graded at all (see spec's partial-claims table).");
        checks.record("5-p165.c4, 5-sig.intreq1: intreq1 is declared to combine with intreq2 to "
            "form AUDxIR, but the manual does not say how or when; no port access, audio sample "
            "or register value in this package distinguishes one way of combining from another, "
            "so this is not turned into an assertion (SPECIFICATION.md, claims not turned into a "
            "proposition).");
        dep_expect(checks, frame_met(both_rec), both.loaded && both.done.has_value() && spacing_both_ok(both_rec),
            "P10-BOTH [p.164] 5-p164.c35,5-sm.c35: S-BOTH -- the fetches from the page's F_3 on are D_mod apart, "
            "within lambda (both transitions)",
            "PC7-CAL and a start-up frame in S-BOTH");
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
