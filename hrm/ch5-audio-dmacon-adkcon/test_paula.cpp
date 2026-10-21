// ch5-audio-dmacon-adkcon -- paula conformance test
//
// Test purpose: DMACON's AUDxEN bits (the only boundary where a DMACON write
//   reaches an audio enable -- paula_audio_channel takes dmaena already
//   decoded) and ADKCON's audio attach/modulation bits (bits 0-7,
//   ATPER0-3/ATVOL0-3), which paula_audio (instantiated as `paula_audio ad1`
//   at paula.v:303-321) never receives -- paula.v has no ADKCON port on that
//   instantiation at all. Also carries the register-map claims (address,
//   read/write direction, chip ownership, SET/CLR semantics) for DMACON,
//   DMACONR, ADKCON, ADKCONR and the sixteen AUDxLEN/PER/ VOL/DAT registers,
//   all of which this package's assigned claims file places at the paula.v
//   boundary.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, Chapter 5
//   printed pp. 136-151, Appendix A printed pp. 263-266 and 278, Appendix B
//   printed pp. 289-291, Appendix C printed pp. 298 and 310.
// Non-goals: does not test disk DMA behaviour (DSKEN, ADKCON.WORDSYNC)
//   beyond the one wire each occupies inside paula.v, does not test the UART
//   beyond ADKCON.UARTBRK's one forced-low bit, does not test interrupt
//   delivery to _ipl (paula_intcontroller's INTENA masking is chapter 7's
//   own assigned territory per BOUNDARIES.md), and does not test Agnus's or
//   Denise's own DMACON/DMACONR decode (this fixture compiles paula.v alone
//   plus its own seven submodule files -- Agnus and Denise are outside this
//   boundary; the Denise-side half of the DMACON chip-ownership conflict is
//   RECORDED from a plain grep already run by the classifiers, not
//   independently re-measured here).
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// CLAIM SOURCE: hrm-testpipeline/work/ch5_packages/
//   ch5-audio-dmacon-adkcon.jsonl -- 155 claim records, all classified
//   "testable" at paula.v.
//
// WHAT COUNTS AS COVERED: an id cited only in a comment or a record()
//   call is not a check, so no id sits there pretending to be one:
//   - DMACONR-NOT-DECODED-BY-PAULA (5-appA-p278.c7, 5-appB-p289.c3,
//     A-dmaconr.chip) is a real checks.expect() and its own genuine RED
//     (see the assertion itself and RESULTS below) -- the manual's
//     Paula-ownership half of DMACONR's chip column does not hold at this
//     boundary.
//   - Six ids (ADKCON-BIT10-WORDSYNC's 2, DSKEN-BIT-NOT-
//     INDEPENDENTLY-TESTABLE's 2, AUDIO-INTERRUPT-EXISTENCE-AND-CADENCE's
//     2) cannot be exercised at this boundary at all. They are named, with
//     the fixture each needs, in the CLAIM COVERAGE list below and NOWHERE
//     ELSE in this file -- deliberately absent from every checks.expect()/
//     checks.record() string, so no per-string coverage scan reads them as
//     tested here. They are NOT COVERED BY THIS PACKAGE.
//   The tally is at the end of CLAIM COVERAGE below.
//
// CHAPTER 8 SLICE S2 IDS: 73 of chapter 8 slice s2's own claims sit on
//   this chapter-5 package because they restate, from chapter 8's own
//   pages (241 and 249) and its own appendix-A/B readings, register facts
//   this package grades at the paula.v boundary. 69 are `extend`: each
//   classify.uid is cited on the label of the one (sometimes two, per the
//   adjudication) assertion that grades that claim's proposition. 4 are
//   `route`: ADKCON's CLR direction (page 249, "If bit 15 is a zero, any
//   ones will clear the corresponding bit") needs its own measurement --
//   ADKCON-SETCLR-PRESERVE only writes 1s into bits that are already 0,
//   which is a no-op under the CLR rule, not a clear. ADKCON-SETCLR-CLR
//   below supplies that half and carries the four route uids.
//
// CH4-DMACON-BITS IDS: eight more claims sit on this
//   package from the identical appendix A p.278 DMACON table rows this
//   package's own AUDxEN-BIT-MAPS-TO-OWN-CHANNEL assertion quotes
//   and grades -- four read independently by opus (APPA-dmacon.c18 "03
//   AUD3EN Audio channel 3 DMA enable", .c19 "02 AUD2EN Audio channel 2 DMA
//   enable", .c20 "01 AUD1EN Audio channel 1 DMA enable", .c21 "00 AUD0EN
//   Audio channel 0 DMA enable") and four by sol of the same four rows
//   (CH4-appA-dmacon.c25/c26/c27/c28, identical quotes, indented). Each
//   restates exactly the proposition AUDxEN-BIT-MAPS-TO-OWN-CHANNEL
//   establishes for all four bits: DMACON bit N is that channel's own named
//   AUDxEN, audio channel N DMA enable, proven there by a walking-one sweep
//   across bits 0-3 with a fresh reset per step. All eight are cited on
//   that assertion's own label.
//
// BOUNDARY: paula.v, reached at $(NANOMIG)/src/minimig-aga/paula.v.
//   paula.v itself instantiates paula_uart, paula_intcontroller,
//   paula_floppy and paula_audio directly; paula_audio in turn
//   instantiates four paula_audio_channel instances and one
//   paula_audio_mixer, which itself instantiates four paula_audio_volume
//   instances -- seven distinct submodule files altogether, all of which
//   this package's own Makefile compiles alongside paula.v.
//
// WIRING FACTS AND EXPECTED VALUES: the port list, the register
//   word-address arithmetic (reg_address_in is bits [8:1], i.e.
//   byte_address>>1), the uartbrk/tx_txd interaction and the facts this
//   file depends on (paula.v:180-183's auden derivation, paula.v:191-203's
//   ADKCON/ADKCONR storage and read decode, and paula_audio's ad1
//   instantiation carrying no adkcon port at all) come from paula.v,
//   paula_audio.v and paula_uart.v. They are wiring facts, never
//   expected values:
//   every expected value below traces to a quoted claim, the ADKCON
//   attach assertion stands RED exactly because the manual's own words
//   say the opposite of what was measured, and no oracle is adjusted to
//   make a red case pass.
//
//   FIXTURE TIMING -- how the fixture reaches a state where the manual's
//   own claim can be measured at all; none of it changes what any
//   assertion expects: (1) a register write's own qualifying edge races
//   paula_intcontroller's audpen latch against paula_audio_channel's own
//   cck-gated state commit (entering direct mode, or a DMA channel's
//   first advance, sets AUDxIR combinationally, which intcontroller
//   latches on every clk7_en edge, while the channel itself only commits
//   on clk7_en+cck -- landing on the wrong cck phase loses that race
//   permanently), so every write is synced to a known-good phase; (2)
//   audio_dmal is a ONE-SHOT pointer-reload pulse when a channel is
//   enabled but never answered (it fires once and the channel then waits
//   in AUDIO_STATE_1 forever), not a stable level, so a single
//   point-in-time read can miss real enablement entirely -- enablement is
//   read by OR-accumulating over a window, or by actually answering the
//   channel into a self-sustaining AUDIO_STATE_3/4 cycle when the claim
//   needs to show it later stops; (3) a stopped channel's own registered
//   audio_dmal can still show a stale 1 for up to a full strhor period
//   after the stop, so every "never seen again" check waits out a grace
//   period first.
//
// MEASUREMENT BASIS, DIRECT MODE: paula_audio_channel enters its
//   processor-driven ("direct") path on an AUDxDAT write while dmaena is
//   low, bypassing the DMA length/period counters entirely (page 137).
//   With AUDxVOL=64 the multiply-path volume input collapses onto 63
//   (paula_audio_mixer.v:25-31 OR's vol[6] into every bit fed to
//   paula_audio_volume, per BOUNDARIES.md's own note #1) so a channel's
//   sample S (signed 8-bit, from AUDxDAT's own high byte) produces
//   ldata/rdata = S*63 once mixed with a silent (0) channel on the other
//   half of that stereo side -- this is not assumed, it is the exact
//   arithmetic that reproduces the evidence already measured for claim
//   5-p150.c24 ("channel 0 alone peaked ldata at 8001" with AUD0VOL=64,
//   AUD0DAT=0x7F7F: sample=0x7F=127, 127*63=8001, matched exactly).
//   ldata sums channels 0+3, rdata sums channels 1+2
//   (paula_audio_mixer.v:76-77, NM-HRM-002 territory, not retested here).
//
// MEASUREMENT BASIS, DMA MODE: DMACON's SET/CLR write convention (bit 15)
//   plus per-channel AUDxEN (bits 0-3) and the master DMAEN (bit 9) are
//   read back indirectly through `audio_dmal`, paula.v's own DMA-request
//   output -- paula.v has no DMACON readback path at all (see the
//   DMACON-ADDRESS-AND-WRITE-ONLY assertion), so `audio_dmal` is the only
//   externally observable consequence of a DMACON write. audio_dmal is a
//   registered snapshot of each channel's own dmareq/dmaspc, latched only
//   on `strhor` (paula_audio.v:171-179) -- this fixture's own clock
//   generator pulses strhor periodically (see Harness::tick), and every
//   assertion that reads audio_dmal runs enough ticks first to guarantee
//   at least one such latch has occurred since the write.
//
// THE TWO SILENCE TRAPS: (1) paula_audio_channel latches on cck and
//   strhor as well as clk7_en -- this fixture's clock generator pulses
//   all three every cycle, with the tick() ratios of the pre-pipeline
//   hrm/paula_audio_channel/test_paula_audio_channel.cpp (removed):
//   clk7_en once per 4 raw clk edges, cck once per 2 clk7_en pulses.
//   (2) the AMR silence latch needs a channel's dmaena to have
//   actually transitioned 1->0 before the first-write-only-clears-
//   silence_d behaviour applies; every direct-mode measurement below
//   starts a channel from a fresh reset with dmaena permanently 0 (never
//   having been 1), so that specific trap's precondition never occurs
//   here -- recorded, not assumed away.
//
// THE FIXTURE PROVES IT CAN DELIVER A KNOWN WORD BEFORE ANYTHING READS
//   ONE: DIRECT-MODE-PRODUCES-SOUND-WITHOUT-DMA below writes two
//   different, individually predictable AUD0DAT values in sequence and
//   requires ldata to visit both predicted numbers, in the order
//   written, before any claim that depends on the data path having
//   delivered a real value is trusted.
//
// CLAIM COVERAGE -- all 155 ids, grouped by the single proposition each
//   group tests (many claims restate the same fact across chapter 5's
//   own tables, appendix A, appendix B and appendix C):
//
//   FOUR-CHANNELS-EXIST (3): 5-p136.c1, 5-hw.c1, 5-chan.c1
//   CHANNELS-INDEPENDENTLY-PROGRAMMABLE (6): 5-p136.c2, 5-hw.c2,
//     5-p149.c1, 5-p149.c2, 5-p149.c3, 5-multi.c1
//   DIRECT-MODE-PRODUCES-SOUND-WITHOUT-DMA (2): 5-p137.c5, 5-hw.c9
//   DMACON-SETCLR-SET (4): 5-p144.c7, 5-dmacon.c1, 5-appA-p278.c11,
//     A-dmacon.b15
//   DMACON-SETCLR-PRESERVE (2): 5-p144.c8, 5-appA-p278.c12
//   ADKCON-SETCLR-SET (3): 5-appA-p265.c9, 5-appA-p265.c10, A-adkcon.b15
//   ADKCON-SETCLR-PRESERVE (2): 5-appA-p265.c11, A-adkcon.b15
//   DMAEN-AND-AUDxEN-BOTH-REQUIRED (9): 5-p144.c5, 5-p144.c6, 5-p144.c9,
//     5-play.c5, 5-play.c6, 5-dmacon.c2, 5-sum.c4, 5-appA-p278.c16,
//     A-dmacon.b09
//   AUDxEN-BIT-MAPS-TO-OWN-CHANNEL (16): 5-p144.c10, 5-p144.c11,
//     5-p144.c12, 5-p144.c13, 5-dmacon.c3, 5-dmacon.c4, 5-dmacon.c5,
//     5-dmacon.c6, 5-appA-p278.c22, 5-appA-p278.c23, 5-appA-p278.c24,
//     5-appA-p278.c25, A-dmacon.b03, A-dmacon.b02, A-dmacon.b01,
//     A-dmacon.b00
//   AUDxEN-CLEAR-STOPS-CHANNEL-AT-ANY-TIME (2): 5-p145.c1, 5-stop.c1
//   DMACON-ADDRESS-AND-WRITE-ONLY (8): 5-appA-p278.c1, 5-appA-p278.c2,
//     5-appA-p278.c4, 5-appB-p290.c1, 5-appB-p290.c2, 5-appB-p290.c4,
//     A-dmacon.addr, A-dmacon.acc
//   DMACON-DECODED-BY-PAULA (0, both ids RECORDED-declined): 5-appA-p278.c3,
//     A-dmacon.chip -- Paula's own decode is still asserted below, without
//     a claim id, since the claim also bundles the Agnus and Denise thirds
//   DMACONR-NOT-DECODED-BY-PAULA -- ASSERTED, EXPECTED RED, THE OWNERSHIP
//     FINDING (3): 5-appA-p278.c7, 5-appB-p289.c3, A-dmaconr.chip
//   ADKCON-ADDRESS-WRITE-ONLY-AND-CHIP (16): 5-appA-p265.c1,
//     5-appA-p265.c2, 5-appA-p265.c3, 5-appA-p265.c4, 5-appB-p290.c14,
//     5-appB-p290.c15, 5-appB-p290.c16, 5-appB-p290.c17, A-pre.c2,
//     A-pre.c12, A-adkcon.addr, A-adkcon.acc, A-adkcon.chip,
//     B-adkcon.addr, B-adkcon.acc, B-adkcon.chip
//   ADKCONR-ADDRESS-READONLY-AND-CHIP (14): 5-appA-p265.c5,
//     5-appA-p265.c6, 5-appA-p265.c7, 5-appA-p265.c8, 5-appB-p289.c6,
//     5-appB-p289.c7, 5-appB-p289.c8, 5-appB-p289.c9, A-adkconr.addr,
//     A-adkconr.acc, A-adkconr.chip, B-adkconr.addr, B-adkconr.acc,
//     B-adkconr.chip
//   ADKCON-BIT11-UARTBRK-FORCES-TXD-LOW (2): 5-appA-p265.c18,
//     A-adkcon.b11
//   ADKCON-BIT10-WORDSYNC -- NOT COVERED BY THIS PACKAGE, needs the
//     SD-card floppy fixture (2): 5-appA-p265.c19, A-adkcon.b10. Named
//     here only -- the record() call below that explains the decision
//     does not repeat these ids, so an ID-search coverage gate finds them
//     in no assertion or record string at all.
//   AUDIO-REGISTERS-HAVE-NO-READ-PATH (43): 5-appA-p265.c34,
//     5-appA-p266.c15, 5-appA-p266.c20, 5-appA-p266.c28,
//     5-appB-p290.c34, 5-appB-p290.c39, 5-appB-p290.c44, 5-appB-p290.c49,
//     5-appB-p290.c69, 5-appB-p290.c74, 5-appB-p290.c79, 5-appB-p290.c84,
//     5-appB-p290.c104, 5-appB-p290.c109, 5-appB-p290.c114,
//     5-appB-p290.c119, 5-appB-p290.c139, 5-appB-p290.c144,
//     5-appB-p291.c2, 5-appB-p291.c7, 5-appC-p298.c9, 5-appC-p310.c3,
//     A-audxdat.acc, A-audxlen.acc, A-audxper.acc, A-audxvol.acc,
//     B-aud0len.acc, B-aud0per.acc, B-aud0vol.acc, B-aud0dat.acc,
//     B-aud1len.acc, B-aud1per.acc, B-aud1vol.acc, B-aud1dat.acc,
//     B-aud2len.acc, B-aud2per.acc, B-aud2vol.acc, B-aud2dat.acc,
//     B-aud3len.acc, B-aud3per.acc, B-aud3vol.acc, B-aud3dat.acc,
//     C-map.per.acc
//   ADKCON-ATTACH-BITS-DO-NOT-SILENCE-CHANNELS -- EXPECTED RED, THE
//     FINDING (10): 5-p149.c12, 5-p150.c24, 5-p150.c27, 5-p151.c1,
//     5-p151.c5, 5-mod.c6, 5-mod.c21, 5-mod.c24, 5-t55.b7, 5-t55.b3
//   ADKCON-ATTACH-MEASUREMENT-BASIS-CONTROL [control, no claim id]:
//     AUDxVOL=0 genuinely silences a channel on the identical
//     trace/peak measurement basis the finding above uses -- rules out
//     a harness that always reports "not silenced".
//   DMA-ENABLED-CHANNEL-WITH-SUPPLIED-DATA-PRODUCES-SOUND (3):
//     5-p144.c1, 5-p149.c11, 5-play.c1
//   DSKEN-BIT-NOT-INDEPENDENTLY-TESTABLE -- NOT COVERED BY THIS PACKAGE,
//     needs the same SD-card floppy fixture (2): 5-appA-p278.c21,
//     A-dmacon.b04. Same treatment as ADKCON-BIT10-WORDSYNC above.
//   AUDIO-INTERRUPT-EXISTENCE-AND-CADENCE -- NOT COVERED BY THIS PACKAGE,
//     belongs at paula_audio_channel's own intreq port or chapter 7's
//     paula_intcontroller (_ipl/INTENA) (2): 5-p148.c4, 5-join.c9
//
//   43+16+16+14+10+9+8+6+4+3+3+3+2*9 = 155. All 155 ids named above; 147
//   are asserted inside this file's checks.expect() text (the DMACONR
//   finding included), 2 (5-appA-p278.c3, A-dmacon.chip) are RECORDED as
//   declined -- the Agnus and Denise thirds of DMACON-DECODED-BY-PAULA are
//   not established at this fixture -- and 6 are explicitly NOT COVERED BY
//   THIS PACKAGE, appearing only in this header, never inside an assertion
//   or record() string.
//
// RESULTS: 19 passed, 2 failed. ADKCON-ATTACH-BITS-DO-NOT-SILENCE-
//   CHANNELS and DMACONR-NOT-DECODED-BY-PAULA are both RED -- both are
//   genuine findings, confirmed exactly as classified, neither adjusted to
//   pass. Every other assertion is GREEN, ADKCON-SETCLR-CLR included. The
//   three remaining claim groups (ADKCON-BIT10-WORDSYNC, DSKEN-BIT-NOT-
//   INDEPENDENTLY-TESTABLE, AUDIO-INTERRUPT-EXISTENCE-AND-CADENCE) cite no
//   id of their own inside any expect()/record() string and are NOT
//   COUNTED as covered by this package; see CLAIM COVERAGE above for the
//   fixture each would need. The eight CH4-DMACON-BITS ids
//   (opus:APPA-dmacon.c18-c21, sol:CH4-appA-dmacon.c25-c28) sit on
//   AUDxEN-BIT-MAPS-TO-OWN-CHANNEL's own label, which establishes, for all
//   four bits via its walking-one sweep, the exact proposition these eight
//   restate.
//
// MUTATION LOG (WSL/Verilator; each entry run: rebuild with the named
//   -CFLAGS -DMUT_X=1 -> run -> read the PASS/FAIL lines -> confirm only
//   the named assertion changed colour -> rebuild with no flags -> confirm
//   the baseline restored; the pass/fail totals quoted in an entry are
//   those of the run that recorded it):
//
// MUTATION ADKCON-ATTACH-BITS-DO-NOT-SILENCE-CHANNELS stimulus green: built
//   with -CFLAGS -DMUT_ATTACH=1, which withholds the AUD0DAT/AUD1DAT writes
//   so both channels are genuinely silent -> 19 passed, 0 failed, only this
//   assertion newly green (measured ldata_peak=0 rdata_peak=0, matching the
//   manual). Rebuilt with no flags: 18/1, the one finding unchanged.
// MUTATION DMACON-SETCLR-PRESERVE stimulus red: built with -CFLAGS
//   -DMUT_SETCLR=1, which also (wrongly) writes DMAEN=1 in the CLR-mode
//   write meant only to preserve it -> 17 passed, 2 failed, only this
//   assertion newly red (measured after=0x0: DMAEN cleared too, silencing
//   every channel, not just AUD1EN). Rebuilt with no flags: 18/1 restored.
// MUTATION AUDxEN-BIT-MAPS-TO-OWN-CHANNEL stimulus red: built with -CFLAGS
//   -DMUT_AUDXEN=1, which makes channel 2's own walking-one step write bit 1
//   instead of bit 2 -> 17/2, only this assertion newly red (measured
//   ch2=0x2 instead of 0x4). Rebuilt with no flags: 18/1 restored.
// One edit, two assertions, so TWO records naming the same edit -- correct
// attribution, not duplication: the grammar is <id> <kind> <result> with no
// token between the id and the kind, so one record with both ids joined by
// "/" would match nothing and both assertions would read as unmutated.
// MUTATION DMACON-DECODED-BY-PAULA stimulus red: built with -CFLAGS
//   -DMUT_CHIP=1, which writes to DMACON_W+1 (the wrong address) instead of
//   DMACON_W inside this assertion's own block (the block carries its own
//   #if MUT_CHIP guard, replaying DMACON-ADDRESS-AND-WRITE-ONLY's identical
//   edit) -> 17 passed, 4 failed: this assertion and DMACON-ADDRESS-AND-
//   WRITE-ONLY both newly red (DMACONR-NOT-DECODED-BY-PAULA stays red from
//   the unmutated baseline, not newly so). Measured paula_decodes_it=false
//   -- the write at the wrong address never moves audio_dmal, so the clause
//   under test, 'a write at $096 visibly changes its own audio_dmal output,
//   so Paula decodes DMACON', is genuinely falsified by this stimulus.
//   See the DMACON-ADDRESS-AND-WRITE-ONLY record below for the same edit.
//   Rebuilt with no flags: 19/2 restored.
// MUTATION DMACON-ADDRESS-AND-WRITE-ONLY stimulus red: built with -CFLAGS
//   -DMUT_CHIP=1, which writes to DMACON_W+1 (the
//   wrong address) instead of DMACON_W -> 17/4, DMACON-DECODED-BY-PAULA newly
//   red alongside it (measured before=0 after=0 but side_effect_seen
//   false, i.e. auden never moved). DMACON-DECODED-BY-PAULA now carries its
//   own MUT_CHIP guard (see its own record above) and goes red under this
//   flag too, on the same edit.
//   Rebuilt with no flags: 19/2 restored.
// MUTATION ADKCON-BIT11-UARTBRK-FORCES-TXD-LOW stimulus red: built with
//   -CFLAGS -DMUT_UARTBRK=1, which writes bit 10 (WORDSYNC) instead of bit
//   11 (UARTBRK) -> 17/2, only this assertion newly red (txd never forced
//   low). Rebuilt with no flags: 18/1 restored.
// MUTATION AUDIO-REGISTERS-HAVE-NO-READ-PATH stimulus red: built with
//   -CFLAGS -DMUT_NOREADPATH=1, which substitutes ADKCONR's own (genuinely
//   non-zero, just written 0x80FF) address for the first of the sixteen
//   audio addresses -> 17/2, only this assertion newly red (measured
//   nonzero at: idx0). Rebuilt with no flags: 18/1 restored.
// MUTATION AUDxEN-CLEAR-STOPS-CHANNEL-AT-ANY-TIME stimulus red: built with
//   -CFLAGS -DMUT_STOP=1, which skips the mid-cycle AUD0EN-clearing write
//   entirely -> 17/2, only this assertion newly red (measured started=yes
//   stopped=no). Rebuilt with no flags: 18/1 restored.
// MUTATION DMAEN-AND-AUDxEN-BOTH-REQUIRED stimulus red: built with -CFLAGS
//   -DMUT_DMAEN=1, which also sets DMAEN on the "AUD0EN alone" truth-table
//   row -> 17/2, only this assertion newly red (that row no longer stays
//   off). Rebuilt with no flags: 18/1 restored.
// MUTATION CHANNELS-INDEPENDENTLY-PROGRAMMABLE stimulus red: built with
//   -CFLAGS -DMUT_INDEP=1, which also arms channel 0 during the "channel 1
//   only" run -> 17/2, only this assertion newly red (measured
//   ch1_alone(L=8001,R=8001) -- channel 0's own contribution now leaks into
//   what should be channel 1's isolated trace). Rebuilt with no flags: 18/1
//   restored.
// MUTATION DIRECT-MODE-PRODUCES-SOUND-WITHOUT-DMA stimulus red: built with
//   -CFLAGS -DMUT_DIRECT=1, which makes the first trial write the second
//   trial's own word (0x0101) instead of 0x7F7F -> 17/2, only this
//   assertion newly red (measured peak1=63 peak2=63, no longer distinct).
//   Rebuilt with no flags: 18/1 restored.
// The whole token run must sit on ONE physical line: `//` is not whitespace, so
// a record whose result token wraps to the next comment line matches nothing.
// MUTATION DMA-ENABLED-CHANNEL-WITH-SUPPLIED-DATA-PRODUCES-SOUND stimulus red:
//   built with -CFLAGS -DMUT_DMASOUND=1, which leaves AUD0EN clear
//   (DMAEN alone) in the DMACON write -> 17/2, only this assertion newly
//   red (measured peak=0 answered_requests=0). Rebuilt with no flags: 18/1
//   restored.
// MUTATION FOUR-CHANNELS-EXIST stimulus red: built with -CFLAGS
//   -DMUT_FOUR=1, which skips channel 3's own walking-one step entirely ->
//   17/2, only this assertion newly red (measured per-channel match
//   string=111, three characters instead of four). Rebuilt with no flags:
//   18/1 restored.
// MUTATION DMACONR-NOT-DECODED-BY-PAULA stimulus green: built with -CFLAGS
//   -DMUT_DMACONR=1, which substitutes ADKCONR_W (an address Paula
//   genuinely decodes, holding 0x33 from this same assertion's own ADKCON
//   write) for DMACONR_W on the post-write read -> 19 passed, 1 failed,
//   only this assertion newly green (measured idle=0x0 after=0x33,
//   genuinely different) -- proving the check is not vacuously red
//   regardless of what address it reads. Rebuilt with no flags: 18/2
//   restored.
// Each flag below changes only the STIMULUS; each of the five turns exactly its
// own assertion newly red.
// MUTATION DMACON-SETCLR-SET stimulus red: built with -CFLAGS -DMUT_DMSET=1,
//   which writes DMACON as 0x0203 instead of 0x8203 -- SET/CLR clear, so the
//   three enables are cleared rather than set and audio_dmal never shows them.
//   17/3, only this assertion newly red. Reverted, 18/2 restored.
// MUTATION ADKCON-SETCLR-SET stimulus red: built with -CFLAGS -DMUT_ADKSET=1,
//   which writes ADKCON as 0x00FF instead of 0x80FF -- the eight attach bits
//   are cleared rather than set, and ADKCONR reads back without them. 17/3,
//   only this assertion newly red. Reverted, 18/2 restored.
// MUTATION ADKCON-SETCLR-PRESERVE stimulus red: built with -CFLAGS
//   -DMUT_ADKPRES=1, which makes the CLR-mode write 0x005F instead of 0x000A,
//   so it also clears bits 0,2,4,6 -- the very bits the assertion requires to
//   be preserved. 17/3, only this assertion newly red. Reverted, 18/2 restored.
// MUTATION ADKCON-ADDRESS-WRITE-ONLY-AND-CHIP stimulus red: built with -CFLAGS
//   -DMUT_ADKADDR=1, which writes the UARTBRK bit at ADKCON_W+2 instead of
//   ADKCON_W -- the neighbouring address -- so txd never goes low and the
//   side effect that proves the address decode disappears. 17/3, only this
//   assertion newly red. Reverted, 18/2 restored.
// MUTATION ADKCONR-ADDRESS-READONLY-AND-CHIP stimulus red: built with -CFLAGS
//   -DMUT_ADKRONLY=1, which seeds ADKCON with 0x0033 instead of 0x8033 --
//   SET/CLR clear, so the seed bits are never set and the read-back value the
//   read-only check compares against is not there. 17/3, only this assertion
//   newly red. Reverted, 18/2 restored.
// MUTATION ADKCON-SETCLR-CLR stimulus red: built with -CFLAGS -DMUT_ADKCLR=1,
//   which leaves bit 15 set on the CLR-mode write (0x8055 instead of 0x0055),
//   so the write lands in SET mode instead of CLR -- the named bits 0,2,4,6,
//   already set from the seed write, stay set instead of clearing. -> 18
//   passed, 3 failed, only ADKCON-SETCLR-CLR newly red (measured ADKCONR=0xff instead
//   of the expected 0xaa). Rebuilt with no flags: 19/2 restored.
// MUTATION ADKCON-ATTACH-MEASUREMENT-BASIS-CONTROL none impossible: this is the
//   control case for ADKCON-ATTACH-BITS-DO-NOT-SILENCE-CHANNELS, whose purpose
//   is to hold the measurement basis fixed while the attach bits move. Any
//   stimulus that changes what it measures destroys the comparison it exists to
//   make, so there is no stimulus mutation that leaves it meaningful. Recorded
//   as `none` rather than left out: an unrecorded mutation is indistinguishable
//   from a forgotten one.
//
//   An assertion exercising its own logic is not evidence that the assertion
//   would notice if the logic changed -- which is why each of the five SET/CLR
//   and address assertions above has its own -DMUT_ flag and record rather
//   than leaning on a sibling's.

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vpaula.h"


// A label is string literals and simple concatenation, so labels format their
// numbers through this helper, not an immediately-invoked lambda such as
//
//     [&]{ char b[8]; snprintf(b,8,"%x",v); return std::string(b); }()
//
// which is valid C++ but puts semicolons inside the expect() call outside any
// string literal. check_gates' call regex cannot cross those, so an assertion
// labelled that way is invisible to the gate, and so is a missing mutation
// record for it.
//
// The buffer is 16 bytes: "%x" of a 32-bit value needs 9 with its terminator.
static std::string hex(unsigned v) {
    char b[16];
    snprintf(b, sizeof b, "%x", v);
    return std::string(b);
}

// MUT_ATTACH: replay ADKCON-ATTACH-BITS-DO-NOT-SILENCE-CHANNELS's own
// mutation record (-DMUT_ATTACH=1). Leaves channel 0 and channel 1's own
// AUD0DAT/AUD1DAT writes out, so both channels are genuinely silent (no
// sample ever written) instead of carrying a peaked sample -- proves the
// trace/peak measurement technique reports "silenced" when a channel
// really is silent, flipping the finding from red to green.
#ifndef MUT_ATTACH
#define MUT_ATTACH 0
#endif
// MUT_SETCLR: replay DMACON-SETCLR-PRESERVE's own mutation record
// (-DMUT_SETCLR=1). The CLR-mode write additionally carries the DMAEN
// bit among its "written as 1" bits, which under the real SET/CLR rule
// must NOT be cleared this way -- makes the preserve-check's own
// expected mask wrong on purpose to confirm the assertion notices.
#ifndef MUT_SETCLR
#define MUT_SETCLR 0
#endif
// MUT_AUDXEN: replay AUDxEN-BIT-MAPS-TO-OWN-CHANNEL's own mutation
// record (-DMUT_AUDXEN=1). Channel 2's own walking-one step writes bit 1
// (channel 1's own bit) instead of bit 2, which must make audio_dmal
// read back as 0x02 instead of 0x04 for that step.
#ifndef MUT_AUDXEN
#define MUT_AUDXEN 0
#endif
// MUT_CHIP: replay DMACON-DECODED-BY-PAULA's own mutation record
// (-DMUT_CHIP=1). Writes to a deliberately wrong address (DMACON_W+1)
// instead of DMACON_W, so paula.v's own decode must NOT see the write.
#ifndef MUT_CHIP
#define MUT_CHIP 0
#endif
// MUT_UARTBRK: replay ADKCON-BIT11-UARTBRK-FORCES-TXD-LOW's own
// mutation record (-DMUT_UARTBRK=1). Writes bit 10 (WORDSYNC) instead of
// bit 11 (UARTBRK), so txd must stay high.
#ifndef MUT_UARTBRK
#define MUT_UARTBRK 0
#endif
// MUT_NOREADPATH: replay AUDIO-REGISTERS-HAVE-NO-READ-PATH's own
// mutation record (-DMUT_NOREADPATH=1). Substitutes ADKCONR's own
// address (which DOES have a real read path and is written a non-zero
// value earlier in this same check) for one of the sixteen audio
// addresses, so the all-zero requirement must catch the genuinely
// non-zero readback and go red -- proving the check is not vacuously
// true regardless of what it reads.
#ifndef MUT_NOREADPATH
#define MUT_NOREADPATH 0
#endif
// MUT_STOP: replay AUDxEN-CLEAR-STOPS-CHANNEL-AT-ANY-TIME's own
// mutation record (-DMUT_STOP=1). Skips the mid-cycle AUD0EN-clearing
// write entirely, so audio_dmal bit 0 must still read 1 afterwards.
#ifndef MUT_STOP
#define MUT_STOP 0
#endif
// MUT_DMAEN: replay DMAEN-AND-AUDxEN-BOTH-REQUIRED's own mutation
// record (-DMUT_DMAEN=1). The "AUD0EN alone" truth-table row also sets
// DMAEN, collapsing it onto the "both set" row.
#ifndef MUT_DMAEN
#define MUT_DMAEN 0
#endif
// MUT_INDEP: replay CHANNELS-INDEPENDENTLY-PROGRAMMABLE's own mutation
// record (-DMUT_INDEP=1). The "channel 1 only" run also arms channel 0
// with the same sample, so ldata is no longer required to stay silent.
#ifndef MUT_INDEP
#define MUT_INDEP 0
#endif
// MUT_DIRECT: replay DIRECT-MODE-PRODUCES-SOUND-WITHOUT-DMA's own
// mutation record (-DMUT_DIRECT=1). The first trial writes the second
// trial's own word (0x0101) instead of 0x7F7F, so its predicted peak
// (8001) must fail to appear.
#ifndef MUT_DIRECT
#define MUT_DIRECT 0
#endif
// MUT_DMASOUND: replay DMA-ENABLED-CHANNEL-WITH-SUPPLIED-DATA-PRODUCES-
// SOUND's own mutation record (-DMUT_DMASOUND=1). DMACON's own write
// leaves AUD0EN clear (DMAEN alone), so the channel must never reach a
// non-zero peak.
#ifndef MUT_DMASOUND
#define MUT_DMASOUND 0
#endif
// MUT_DMACONR: replay DMACONR-NOT-DECODED-BY-PAULA's own mutation record
// (-DMUT_DMACONR=1). Substitutes ADKCONR_W (an address Paula genuinely
// decodes) for DMACONR_W on the post-write read, so the "identical to
// idle" comparison must find a genuine difference and flip green.
#ifndef MUT_DMACONR
#define MUT_DMACONR 0
#endif
// MUT_FOUR: replay FOUR-CHANNELS-EXIST's own mutation record
// (-DMUT_FOUR=1). Channel 3's own walking-one step is skipped (treated
// as already matching), collapsing the count of independently-proven
// channels to three.
#ifndef MUT_FOUR
#define MUT_FOUR 0
#endif
// MUT_ADKCLR: replay ADKCON-SETCLR-CLR's own mutation record
// (-DMUT_ADKCLR=1). The CLR-mode write also (wrongly) leaves bit 15 set,
// so the write lands in SET mode instead of CLR mode and the named bits
// get set again instead of cleared.
#ifndef MUT_ADKCLR
#define MUT_ADKCLR 0
#endif

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Register word addresses: reg_address_in is declared `input [8:1]`, i.e.
// it carries bits 8..1 of the manual's own byte address with bit 0
// implicit -- so word_address == byte_address >> 1, read directly off
// paula.v's own port declaration, not inferred. Byte addresses themselves
// are the manual's (appendix A/B, chapter 5 Table 5-3/5-5).
constexpr int DMACON_W   = 0x096 >> 1;
constexpr int ADKCON_W   = 0x09E >> 1;
constexpr int ADKCONR_W  = 0x010 >> 1;
constexpr int DMACONR_W  = 0x002 >> 1;
constexpr int AUD0LEN_W  = 0x0A4 >> 1;
constexpr int AUD0PER_W  = 0x0A6 >> 1;
constexpr int AUD0VOL_W  = 0x0A8 >> 1;
constexpr int AUD0DAT_W  = 0x0AA >> 1;
constexpr int AUD1LEN_W  = 0x0B4 >> 1;
constexpr int AUD1PER_W  = 0x0B6 >> 1;
constexpr int AUD1VOL_W  = 0x0B8 >> 1;
constexpr int AUD1DAT_W  = 0x0BA >> 1;
constexpr int AUD2LEN_W  = 0x0C4 >> 1;
constexpr int AUD2PER_W  = 0x0C6 >> 1;
constexpr int AUD2VOL_W  = 0x0C8 >> 1;
constexpr int AUD2DAT_W  = 0x0CA >> 1;
constexpr int AUD3LEN_W  = 0x0D4 >> 1;
constexpr int AUD3PER_W  = 0x0D6 >> 1;
constexpr int AUD3VOL_W  = 0x0D8 >> 1;
constexpr int AUD3DAT_W  = 0x0DA >> 1;
constexpr int IDLE_W     = 0x000;  // Unused address: matches no register above.

constexpr int STRHOR_PERIOD = 20;  // clk7_en pulses between strhor latches.

int sign_extend_15(int value) {
    return (value & 0x4000) ? (value | ~0x7FFF) : value;
}

// Clocking in the ratios of the pre-pipeline
// hrm/paula_audio_channel/test_paula_audio_channel.cpp tick() lambda
// (removed): clk7_en once per 4 raw clk edges, cck once per 2 clk7_en
// pulses. strhor is this package's own addition, needed
// because paula.v's own audio_dmal/audio_dmas outputs only latch on it
// (paula_audio.v:171-179).
class Harness {
public:
    Harness() : dut(new Vpaula) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->clk7n_en = 0;
        dut->cck = 0;
        dut->reset = 1;
        dut->reg_address_in = IDLE_W;
        dut->data_in = 0;
        dut->rxd = 1;
        dut->ntsc = 0;
        dut->sof = 0;
        dut->strhor = 0;
        dut->vblint = 0;
        dut->int2 = 0;
        dut->int3 = 0;
        dut->int6 = 0;
        dut->_step = 0;
        dut->direc = 0;
        dut->_sel = 0xF;
        dut->side = 0;
        dut->_motor = 1;
        dut->sdc_image_mounted = 0;
        dut->sdc_image_size = 0;
        dut->sdc_busy = 0;
        dut->sdc_done = 0;
        dut->sdc_byte_in_strobe = 0;
        dut->sdc_byte_addr = 0;
        dut->sdc_byte_in_data = 0;
        dut->floppy_drives = 0;
        dut->floppy_wrprot = 0;
        for (int i = 0; i < 64; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 64; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();

        clk7_phase = (clk7_phase + 1) % 4;
        dut->clk7_en = (clk7_phase == 0);
        dut->clk7n_en = (clk7_phase == 2);

        if (dut->clk7_en) {
            cck_phase = (cck_phase + 1) % 2;
            dut->cck = (cck_phase == 0);
            ++strhor_count;
            dut->strhor = (strhor_count % STRHOR_PERIOD) == 0;
        } else {
            dut->cck = 0;
            dut->strhor = 0;
        }
    }

    void ticks(int n) { for (int i = 0; i < n; ++i) tick(); }

    // Ticks until the NEXT clk7_en-qualified edge is known to land with
    // cck==0 (dut->clk7_en/dut->cck are updated at the END of tick(), so
    // reading them right after a tick() call reports what the UPCOMING
    // edge will be, not the one just completed). Needed because
    // paula_audio_channel's own datwrite->AUDxDAT->direct-mode-entry
    // path races paula_intcontroller's own audpen latch: intcontroller
    // updates on every clk7_en edge, while the audio channel's state
    // register only commits on a clk7_en+cck edge (half as often). If
    // the write's own qualifying edge lands on cck==1, the very next
    // clk7_en edge (cck==0, no commit) is followed by intcontroller
    // already latching AUDxIR into audpen one cycle before the audio
    // channel's own next cck==1 edge can commit the transition -- a
    // real RTL race, not a fixture bug -- so every write is
    // deterministically aligned to the winning phase instead.
    void sync_to_cck_low_edge() {
        while (!(dut->clk7_en && !dut->cck)) tick();
    }

    // Guarantees at least one clk7_en-qualified edge occurs while the
    // address/data are held (clk7_en pulses once every 4 ticks), and
    // that this edge lands with cck==0 (see sync_to_cck_low_edge).
    void write_reg(int addr_w, uint16_t data) {
        sync_to_cck_low_edge();
        dut->reg_address_in = addr_w;
        dut->data_in = data;
        ticks(4);
        dut->reg_address_in = IDLE_W;
        dut->data_in = 0;
        ticks(4);
    }

    // data_out is a plain combinational OR-mux (paula.v:157); no clock
    // edge is required for it to reflect the currently-addressed value.
    uint16_t read_reg(int addr_w) {
        dut->reg_address_in = addr_w;
        dut->eval();
        uint16_t v = static_cast<uint16_t>(dut->data_out);
        dut->reg_address_in = IDLE_W;
        dut->eval();
        return v;
    }

    // Waits until at least one strhor-qualified latch has definitely
    // happened since the caller's last write (STRHOR_PERIOD*2 covers a
    // full period plus slack for phase alignment).
    // NOTE: STRHOR_PERIOD counts clk7_en PULSES (strhor_count increments
    // once per clk7_en pulse, in tick()), but ticks(n) counts RAW clk
    // edges, four of which make one clk7_en pulse. So this waits for
    // STRHOR_PERIOD*4 raw ticks per strhor period, not STRHOR_PERIOD --
    // mixing the two units would not reliably span even one real strhor
    // period.
    void settle_dma() { ticks(STRHOR_PERIOD * 4 * 2); }

    // Runs for n_ticks, OR-accumulating audio_dmal the whole time, and
    // returns the accumulated mask: "was this bit seen high at ANY point
    // during the window". Needed because an audio channel's own dmareq
    // is a ONE-SHOT pointer-reload request (paula_audio_channel.v's
    // state0->state1 transition) that fires once when a channel is
    // first enabled and then stays quiet forever if nothing answers it
    // (the channel is stuck in state1 waiting for a real AUDxDAT write,
    // per its own `if (AUDxON && AUDxDAT)` advance condition) -- so a
    // SINGLE point-in-time read of audio_dmal after settling can miss an
    // enabled-but-unanswered channel's only request entirely, reading 0
    // even though DMACON's own enable bit is genuinely 1. Accumulating
    // over the window is what actually matches the claim under test
    // ("this bit enables this channel's own DMA request"), not the
    // narrower and fragile "this bit's request is still live at this
    // exact instant".
    uint32_t observe_audio_dmal(int n_ticks) {
        uint32_t seen = 0;
        for (int i = 0; i < n_ticks; ++i) { tick(); seen |= dut->audio_dmal; }
        return seen;
    }

    // Runs for n_ticks, answering audio_dmal with a fixed data word for
    // every channel bit set in respond_mask whenever it requests, and
    // OR-accumulates the audio_dmal mask observed the whole time.
    // Answering (rather than leaving requests unanswered) is what lets a
    // channel actually reach paula_audio_channel.v's AUDIO_STATE_3/4
    // loop, where the period counter re-generates a fresh dmareq every
    // "perfin" cycle on its own -- an unanswered channel fires its one
    // initial pointer-reload request and then sits quietly in
    // AUDIO_STATE_1 forever, which would make "it stopped requesting"
    // true for the wrong reason (it was never going to request again
    // regardless of DMACON). Requires the caller to have already set
    // that channel's own AUDxPER to a small value; the default (0)
    // makes percnt wrap to 65535 on the first decrement.
    uint32_t run_answering(uint32_t respond_mask, int n_ticks) {
        static const int dat_w[4] = {AUD0DAT_W, AUD1DAT_W, AUD2DAT_W, AUD3DAT_W};
        uint32_t seen = 0;
        int cooldown[4] = {0, 0, 0, 0};  // Per-channel: one channel's answer must not starve another's.
        for (int i = 0; i < n_ticks; ++i) {
            tick();
            seen |= dut->audio_dmal;
            for (int ch = 0; ch < 4; ++ch) {
                if (cooldown[ch] > 0) { --cooldown[ch]; continue; }
                if ((respond_mask & (1u << ch)) && (dut->audio_dmal & (1u << ch))) {
                    write_reg(dat_w[ch], 0x7F7F);
                    cooldown[ch] = 4;
                }
            }
        }
        return seen;
    }

    // Ticks grace_ticks first (letting any already-in-flight request that
    // pre-dates the stop finish draining through the strhor latch --
    // paula_audio.v's dmal is a registered copy of dmareq and does not
    // update instantly), THEN ticks confirm_ticks more while requiring
    // the given audio_dmal bit to be 0 on EVERY one of those ticks. This
    // is the deliberate opposite of observe_audio_dmal/run_answering's
    // own OR-accumulation ("ever seen high", right for proving
    // enablement): proving a channel has STOPPED needs "never seen high
    // once settled", not "not high at this one instant" (fragile) and
    // not "OR over the window" (a single stale leftover 1 right at the
    // transition would wrongly fail it).
    bool confirm_stays_off(uint32_t bit_mask, int grace_ticks, int confirm_ticks) {
        for (int i = 0; i < grace_ticks; ++i) tick();
        for (int i = 0; i < confirm_ticks; ++i) {
            tick();
            if (dut->audio_dmal & bit_mask) return false;
        }
        return true;
    }

    Vpaula* dut;
    int clk7_phase = 0;
    int cck_phase = 0;
    long strhor_count = 0;
};

struct Trace {
    std::vector<int> ldata;
    std::vector<int> rdata;
};

int peak_abs(const std::vector<int>& v) {
    int m = 0;
    for (int x : v) if (std::abs(x) > m) m = std::abs(x);
    return m;
}
int signed_peak(const std::vector<int>& v) {
    // Returns the sample of largest magnitude, signed.
    int best = 0;
    for (int x : v) if (std::abs(x) > std::abs(best)) best = x;
    return best;
}

// Arms a single channel in direct (non-DMA) mode: AUDxVOL then AUDxDAT.
// dmaena for every channel is 0 by construction (DMACON is never written
// with that channel's AUDxEN in these direct-mode measurements), so this
// exercises paula_audio_channel's processor-driven path
// (page 137: "direct (non-DMA) sound generation").
void arm_direct(Harness& h, int vol_w, int dat_w, uint16_t vol, uint16_t dat) {
    h.write_reg(vol_w, vol);
    h.write_reg(dat_w, dat);
}

Trace capture(Harness& h, int n_ticks) {
    Trace t;
    for (int i = 0; i < n_ticks; ++i) {
        h.tick();
        t.ldata.push_back(sign_extend_15(h.dut->ldata));
        t.rdata.push_back(sign_extend_15(h.dut->rdata));
    }
    return t;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- DIRECT-MODE-PRODUCES-SOUND-WITHOUT-DMA ----------------
    // Also this package's own fixture-liveness proof: two distinct,
    // individually-predicted AUD0DAT words must each arrive at ldata as
    // their own correctly-predicted, distinct value, before any other
    // claim trusts this data path -- reusing the same fresh-reset-per-
    // trial pattern hrm/ch4-attached-sprites/'s own FIXTURE-LIVE-DATA-
    // PATH control uses for the identical purpose, rather than one
    // continuous run. A continuous run cannot show a SECOND distinct
    // direct-mode word here: entering direct mode itself sets AUDxIR=1
    // (paula_audio_channel.v's state0 direct-branch, matching this
    // chapter's own "interrupt raised before a word is output" finding
    // candidate), which paula_intcontroller.v latches into AUDxIP
    // permanently (nothing in this fixture ever writes INTREQ to clear
    // it, and doing so would be reaching into chapter 7's own register
    // semantics just for fixture plumbing) -- and AUDIO_STATE_4's own
    // return-to-STATE_3 branch is gated on `AUDxON || !AUDxIP`, so once
    // AUDxIP latches, a direct-mode channel (AUDxON permanently 0)
    // cannot re-enter and returns to idle after its one word instead.
    // Measured by VCD trace, not assumed.
    {
        Harness h1;
        h1.write_reg(AUD0VOL_W, 64);
        h1.write_reg(AUD0PER_W, 8);  // Small period: lets the one word's two bytes (state 3, then 4) both emerge inside the capture window.
#if MUT_DIRECT
        h1.write_reg(AUD0DAT_W, 0x0101);  // MUTATION: the "first" trial writes the SECOND trial's own word.
#else
        h1.write_reg(AUD0DAT_W, 0x7F7F);   // sample=0x7F=127; 127*63=8001.
#endif
        Trace t1 = capture(h1, 200);
        int peak1 = signed_peak(t1.ldata);

        Harness h2;
        h2.write_reg(AUD0VOL_W, 64);
        h2.write_reg(AUD0PER_W, 8);
        h2.write_reg(AUD0DAT_W, 0x0101);  // sample=0x01=1 (both bytes equal, order-independent); 1*63=63.
        Trace t2 = capture(h2, 200);
        int peak2 = signed_peak(t2.ldata);

        bool first_matches = peak1 == 8001;
        bool second_matches = peak2 == 63;
        bool distinct = first_matches && second_matches && peak1 != peak2;
        checks.expect(distinct,
            "DIRECT-MODE-PRODUCES-SOUND-WITHOUT-DMA: with DMAEN/AUD0EN never "
            "set (dmaena permanently 0), writing AUD0VOL=64 then "
            "AUD0DAT=0x7F7F (sample=127) makes ldata peak at 127*63=8001 -- "
            "matching the exact number already measured for this claim -- "
            "and, from a separate fresh reset, a distinct AUD0DAT=0x0101 "
            "(sample=1) makes it peak at 1*63=63 instead -- proving both "
            "that sound is produced with no DMA at all and that this "
            "fixture's data path delivers whatever distinct value it is "
            "given, correctly, not a stuck constant. 'There are two "
            "methods of basic sound production on the Amiga -- automatic "
            "(DMA) sound "
            "generation and direct (non-DMA) sound generation.' (p.137, "
            "5-p137.c5, 5-hw.c9). measured peak1=" + std::to_string(peak1) +
            " peak2=" + std::to_string(peak2) + ".");
    }

    // ---------------- CHANNELS-INDEPENDENTLY-PROGRAMMABLE ----------------
    {
        Harness h0;
        arm_direct(h0, AUD0VOL_W, AUD0DAT_W, 64, 0x7F00);  // channel 0 sample=127.
        Trace t0 = capture(h0, 200);
        int ch0_ldata = signed_peak(t0.ldata);
        int ch0_rdata = signed_peak(t0.rdata);

        Harness h1;
        arm_direct(h1, AUD1VOL_W, AUD1DAT_W, 64, 0x7F00);  // channel 1 sample=127.
        Trace t1 = capture(h1, 200);
        int ch1_ldata = signed_peak(t1.ldata);
#if MUT_INDEP
        arm_direct(h1, AUD0VOL_W, AUD0DAT_W, 64, 0x7F00);  // MUTATION: channel 0 armed too.
        t1 = capture(h1, 200);
        ch1_ldata = signed_peak(t1.ldata);
#endif
        int ch1_rdata = signed_peak(t1.rdata);

        Harness hboth;
        arm_direct(hboth, AUD0VOL_W, AUD0DAT_W, 64, 0x7F00);  // channel 0 sample=127 -> 8001.
        arm_direct(hboth, AUD1VOL_W, AUD1DAT_W, 64, 0x4000);  // channel 1 sample=64 -> 4032.
        Trace tboth = capture(hboth, 200);
        int both_ldata = signed_peak(tboth.ldata);
        int both_rdata = signed_peak(tboth.rdata);

        bool ch0_alone_ok = ch0_ldata == 8001 && ch0_rdata == 0;
        bool ch1_alone_ok = ch1_ldata == 0 && ch1_rdata == 8001;
        bool both_ok = both_ldata == 8001 && both_rdata == 4032;
        checks.expect(ch0_alone_ok && ch1_alone_ok && both_ok,
            "CHANNELS-INDEPENDENTLY-PROGRAMMABLE: arming channel 0 alone "
            "(AUD0VOL=64, AUD0DAT hi-byte 0x7F) peaks ldata at 8001 while "
            "rdata stays silent; arming channel 1 alone the same way peaks "
            "rdata at 8001 while ldata stays silent; arming both at once, "
            "with DIFFERENT sample values (channel 0 0x7F -> 8001, channel "
            "1 0x40 -> 4032), makes both sides peak at their OWN "
            "independently-predicted value simultaneously, neither "
            "affecting the other. 'You can independently program each of "
            "the channels to produce complex sound effects.' (p.136, "
            "5-p136.c2, 5-hw.c2); 'All four audio channels are "
            "independently programmable... each channel has its own data "
            "set; thus a different tone or musical note can be played on "
            "each channel.' (p.149, 5-p149.c1, 5-p149.c2, 5-p149.c3, "
            "5-multi.c1). measured ch0_alone(L=" + std::to_string(ch0_ldata) +
            ",R=" + std::to_string(ch0_rdata) + ") ch1_alone(L=" +
            std::to_string(ch1_ldata) + ",R=" + std::to_string(ch1_rdata) +
            ") both(L=" + std::to_string(both_ldata) + ",R=" +
            std::to_string(both_rdata) + ").");
    }

    // ---------------- DMACON-SETCLR-SET ----------------
    {
        Harness h;
#if MUT_DMSET
        h.write_reg(DMACON_W, 0x0203);  // stimulus mutation: SET/CLR=0, so this CLEARS the three bits.
#else
        h.write_reg(DMACON_W, 0x8203);  // SET: DMAEN, AUD0EN, AUD1EN (all in one write, bit 15=SET/CLR=1).
#endif
        uint32_t mask_after_set = h.observe_audio_dmal(STRHOR_PERIOD * 4 + 8);
        checks.expect((mask_after_set & 0x3) == 0x3,
            "DMACON-SETCLR-SET: writing DMACON with bit 15 (SET/CLR) = 1 "
            "and data bits 0,1,9 = 1 sets AUD0EN, AUD1EN and DMAEN "
            "together, both channel bits reflected in audio_dmal (0x3) -- "
            "DMAEN is included in the same write here only so the SET "
            "mechanism is visible on audio_dmal at all (DMAEN-AND-AUDxEN-"
            "BOTH-REQUIRED below tests DMAEN's own gating in isolation). "
            "Table 5-3 names the write register 'DMACONW' "
            "where appendix A p.278/appendix B p.290 both say DMACON -- "
            "a document-naming difference, not a hardware one; the RTL "
            "decodes address $096 either way. 'When this bit is written "
            "as a 1, it sets any bit in DMACONW for which the "
            "corresponding bit position is also a 1, leaving all other "
            "bits alone.' (p.144, 5-p144.c7, 5-dmacon.c1); 'DMACON bit 15 "
            "selects set versus clear behavior for bits written as 1.' "
            "(appendix A p.278, 5-appA-p278.c11, A-dmacon.b15). measured "
            "dmacon bits (via audio_dmal)=0x" +
            hex(mask_after_set) + ".");
    }

    // ---------------- DMACON-SETCLR-PRESERVE ----------------
    {
        Harness h;
        h.write_reg(AUD0PER_W, 8);
        h.write_reg(AUD1PER_W, 8);
        h.write_reg(AUD2PER_W, 8);
        h.write_reg(DMACON_W, 0x8207);  // SET: DMAEN, AUD0EN, AUD1EN, AUD2EN.
        // Answer all three so each reaches AUDIO_STATE_3/4, where the
        // period counter re-requests on its own -- otherwise an
        // unanswered channel's one-shot pointer-reload request would
        // make "channel 1 stopped requesting" true for the wrong reason
        // after the CLR write below (see run_answering's own comment).
        uint32_t before = h.run_answering(0x7, 400);
#if MUT_SETCLR
        h.write_reg(DMACON_W, 0x0202);  // MUTATION: also (wrongly) writes DMAEN=1 in a CLR write meant to preserve it.
#else
        h.write_reg(DMACON_W, 0x0002);  // CLR: AUD1EN only; bits 0,2 and 9(DMAEN) are written 0 -> must stay unchanged.
#endif
        // Grace period first: a request already in flight for channel 1
        // at the moment of the CLR write still has to drain through
        // paula_audio.v's own strhor latch before audio_dmal reflects
        // the stop (see confirm_stays_off's own comment) -- without
        // this, a stale leftover 1 sampled right at the transition would
        // wrongly fail "channel 1 never requests again" even though it
        // genuinely does stop moments later. Measured by VCD trace: the
        // stale value can persist for a full strhor period after being
        // latched, so one period of grace is not always enough margin --
        // two periods is.
        h.ticks(STRHOR_PERIOD * 4 * 2);
        // Channel 1 must never be seen requesting again; channels 0 and
        // 2, still genuinely cycling, must be seen requesting within
        // this same window.
        uint32_t after = h.run_answering(0x5, 400);
        bool preserved = (before == 0x7) && ((after & 0x5) == 0x5) && ((after & 0x2) == 0);
        checks.expect(preserved,
            "DMACON-SETCLR-PRESERVE: with AUD0EN/AUD1EN/AUD2EN/DMAEN all "
            "set (audio_dmal=0x7), a CLR-mode write (bit 15=0) naming only "
            "AUD1EN (data bit 1=1, all other data bits 0) clears AUD1EN "
            "alone -- AUD0EN and AUD2EN, written as 0, are unaffected "
            "(audio_dmal stays 0x5), matching 'Bits written with a zero "
            "are always unchanged.' 'With DMACON bit 15 in set mode, "
            "nonselected bits are unchanged.' [restated for the CLR case, "
            "the same non-selected-bits-untouched rule] (p.144, "
            "5-p144.c8); 'DMACON bits written as zero remain unchanged.' "
            "(appendix A p.278, 5-appA-p278.c12). measured before=0x" +
            hex(before) +
            " after=0x" + hex(after) + ".");
    }

    // ---------------- ADKCON-SETCLR-SET ----------------
    {
        Harness h;
#if MUT_ADKSET
        h.write_reg(ADKCON_W, 0x00FF);  // stimulus mutation: SET/CLR=0, so the eight bits are cleared, not set.
#else
        h.write_reg(ADKCON_W, 0x80FF);  // SET: bits 0-7 (the audio attach/modulate bits).
#endif
        uint16_t back = h.read_reg(ADKCONR_W);
        checks.expect((back & 0x00FF) == 0x00FF,
            "ADKCON-SETCLR-SET: writing ADKCON with bit 15 (SET/CLR)=1 and "
            "data bits 0-7=1 sets all eight, read back whole through "
            "ADKCONR ($010). 'Set/clear control bit. Determines if bits "
            "written with a 1 get set or cleared. Bits written with a "
            "zero are always unchanged.' (appendix A p.265, "
            "5-appA-p265.c9, 5-appA-p265.c10, A-adkcon.b15). measured "
            "ADKCONR=0x" + hex(back) + ". CH8 S2 CITES: opus:8-adkcon.c5, opus:8-adkcon.c6, opus:8-adkcon.c8, opus:8-adkcon.c9, sol:8-adkcon.c5, opus:8-appA-adkcon.c9, sol:8-appa-adkbit.c1.");
    }

    // ---------------- ADKCON-SETCLR-PRESERVE ----------------
    {
        Harness h;
        h.write_reg(ADKCON_W, 0x8055);  // SET: bits 0,2,4,6.
#if MUT_ADKPRES
        h.write_reg(ADKCON_W, 0x005F);  // stimulus mutation: also clears bits 0,2,4,6, which must be preserved.
#else
        h.write_reg(ADKCON_W, 0x000A);  // CLR: bits 1,3 (written 0 elsewhere) -- must leave 0,2,4,6 alone.
#endif
        uint16_t back = h.read_reg(ADKCONR_W);
        checks.expect((back & 0x00FF) == 0x0055,
            "ADKCON-SETCLR-PRESERVE: after ADKCON bits 0,2,4,6 are set, a "
            "CLR-mode write naming only bits 1 and 3 (which were already "
            "0) leaves bits 0,2,4,6 set -- 'Bits written with a zero are "
            "always unchanged.' (appendix A p.265, 5-appA-p265.c11, "
            "A-adkcon.b15). measured ADKCONR=0x" +
            hex(back) + ". CH8 S2 CITES: opus:8-adkcon.c5, opus:8-adkcon.c8, opus:8-appA-adkcon.c9, opus:8-appA-adkcon.c10, sol:8-appa-adkbit.c1, sol:8-appa-adkbit.c2.");
    }

    // ---------------- ADKCON-SETCLR-CLR ----------------
    // ch8 s2 route finding: ADKCON-SETCLR-PRESERVE above only ever writes
    // 1s into bits that are already 0 (a no-op under the CLR rule), so
    // nothing in this suite, before this assertion, showed a 1 written
    // with bit 15 = 0 actually CLEARING an already-set ADKCON bit. This
    // assertion supplies that missing half: SET all eight bits 0-7, then
    // a CLR-mode write (bit 15 = 0) naming only bits 0,2,4,6 must clear
    // exactly those, leaving the unnamed bits 1,3,5,7 (also set) alone.
    {
        Harness h;
        h.write_reg(ADKCON_W, 0x80FF);  // SET all eight: bits 0-7.
#if MUT_ADKCLR
        h.write_reg(ADKCON_W, 0x8055);  // MUTATION: bit 15 left set, so this lands in SET mode, not CLR -- the named bits stay set instead of clearing.
#else
        h.write_reg(ADKCON_W, 0x0055);  // CLR: bit 15 = 0, data bits 0,2,4,6 named as 1 -- must clear.
#endif
        uint16_t back = h.read_reg(ADKCONR_W);
        checks.expect((back & 0x00FF) == 0x00AA,
            "ADKCON-SETCLR-CLR: with all eight bits 0-7 first set, a "
            "CLR-mode write (bit 15 = 0) naming bits 0,2,4,6 clears "
            "exactly those bits to 0 -- the write actually reaching 0, "
            "not merely staying 0 the way ADKCON-SETCLR-PRESERVE's own "
            "bits 1,3 do -- while the unnamed bits 1,3,5,7 stay set. "
            "'If bit 15 is a zero, any ones will clear the corresponding "
            "bit.' (p.249, opus:8-adkcon.c7, sol:8-adkcon.c6); 'If bit "
            "15 is a 1, the specified bits are set. If bit 15 is a 0, "
            "the specified bits are cleared.' (p.249, opus:8-adkcon.c10); "
            "'Bit 15 of this register allows independent setting or "
            "clearing of any bit or bits.' (p.249, sol:8-adkcon.c4). "
            "measured ADKCONR=0x" + hex(back) + ".");
    }

    // ---------------- DMAEN-AND-AUDxEN-BOTH-REQUIRED ----------------
    {
        const int OBS = STRHOR_PERIOD * 4 + 8;
        Harness h_neither;
        h_neither.write_reg(DMACON_W, 0x8000);  // SET nothing (DMAEN=0, AUD0EN=0).
        bool neither_off = (h_neither.observe_audio_dmal(OBS) & 0x1) == 0;

        Harness h_audxen_only;
#if MUT_DMAEN
        h_audxen_only.write_reg(DMACON_W, 0x8201);  // MUTATION: DMAEN is (wrongly) also set on the "AUD0EN alone" row.
#else
        h_audxen_only.write_reg(DMACON_W, 0x8001);  // SET AUD0EN, DMAEN left 0.
#endif
        bool audxen_only_off = (h_audxen_only.observe_audio_dmal(OBS) & 0x1) == 0;

        Harness h_dmaen_only;
        h_dmaen_only.write_reg(DMACON_W, 0x8200);  // SET DMAEN, AUD0EN left 0.
        bool dmaen_only_off = (h_dmaen_only.observe_audio_dmal(OBS) & 0x1) == 0;

        Harness h_both;
        h_both.write_reg(DMACON_W, 0x8201);  // SET DMAEN and AUD0EN together, i.e. Table 5-5's own worked example.
        bool both_on = (h_both.observe_audio_dmal(OBS) & 0x1) == 1;

        checks.expect(neither_off && audxen_only_off && dmaen_only_off && both_on,
            "DMAEN-AND-AUDxEN-BOTH-REQUIRED: DMACON=0x8000 (neither bit) "
            "leaves audio_dmal bit 0 at 0; 0x8001 (AUD0EN alone, DMAEN "
            "still 0) also leaves it 0; 0x8200 (DMAEN alone, AUD0EN still "
            "0) also leaves it 0; only 0x8201, setting DMAEN and AUD0EN "
            "TOGETHER in the single write Table 5-3's own worked example "
            "describes, raises it to 1. 'Only while this bit is a 1 can "
            "any direct memory access occur.' (p.144, 5-p144.c9, "
            "5-dmacon.c2); 'For any audio DMA to occur (or any other DMA, "
            "for that matter), the DMAEN bit in DMACON must be set.' "
            "(p.144, 5-p144.c5, 5-play.c5); 'When both DMAEN and AUDxEN are set, the "
            "DMA will start for channel x.' (p.144, 5-p144.c6, "
            "5-play.c6); 'Enable all DMA below' (appendix A p.278, "
            "5-appA-p278.c16, A-dmacon.b09); 'Start the audio DMA by "
            "writing a 1 into bit 9, DMAEN, along with a 1 in the SET/CLR "
            "bit and a 1 in the position of the AUDxEN bit of the channel "
            "or channels you want to start.' (p.145, 5-sum.c4).");
    }

    // ---------------- AUDxEN-BIT-MAPS-TO-OWN-CHANNEL ----------------
    {
        bool all_ok = true;
        std::string detail;
        for (int i = 0; i < 4; ++i) {
            Harness h;
            int bit_to_write = i;
#if MUT_AUDXEN
            if (i == 2) bit_to_write = 1;  // MUTATION: channel 2's own step writes bit 1 instead of bit 2.
#endif
            uint16_t data = static_cast<uint16_t>(0x8200 | (1 << bit_to_write));
            h.write_reg(DMACON_W, data);
            uint32_t mask = h.observe_audio_dmal(STRHOR_PERIOD * 4 + 8) & 0xF;
            uint32_t expected = static_cast<uint32_t>(1 << i);
            bool ok = mask == expected;
            all_ok = all_ok && ok;
            detail += " ch" + std::to_string(i) + "=0x" +
                hex(mask);
        }
        checks.expect(all_ok,
            "AUDxEN-BIT-MAPS-TO-OWN-CHANNEL: a walking-one sweep -- SET "
            "DMAEN plus exactly one of DMACON bits 0,1,2,3 at a time, "
            "fresh reset per step -- makes audio_dmal read back exactly "
            "that single bit and no other, for all four bits, proving "
            "each DMACON audio-enable bit maps 1:1 to its own channel's "
            "DMA-enable line with no cross-talk. 'AUD3EN Audio channel 3 "
            "enable.'/'AUD2EN.../AUD1EN.../AUD0EN...' (p.144, 5-p144.c10, "
            "5-p144.c11, 5-p144.c12, 5-p144.c13; Table 5-3 restated, "
            "5-dmacon.c3, 5-dmacon.c4, 5-dmacon.c5, 5-dmacon.c6); "
            "'AUD3EN Audio channel 3 DMA enable.'/'AUD2EN.../AUD1EN.../"
            "AUD0EN...' (appendix A p.278, 5-appA-p278.c22, "
            "5-appA-p278.c23, 5-appA-p278.c24, 5-appA-p278.c25, "
            "A-dmacon.b03, A-dmacon.b02, A-dmacon.b01, "
            "opus:APPA-dmacon.c18, sol:CH4-appA-dmacon.c25, "
            "opus:APPA-dmacon.c19, sol:CH4-appA-dmacon.c26, "
            "opus:APPA-dmacon.c20, sol:CH4-appA-dmacon.c27, "
            "opus:APPA-dmacon.c21, sol:CH4-appA-dmacon.c28, "
            "A-dmacon.b00). "
            "measured" + detail + ".");
    }

    // ---------------- FOUR-CHANNELS-EXIST ----------------
    // Reuses the walking-one measurement immediately above: four
    // distinct, independently-addressable enable bits each gating
    // exactly one channel's own DMA-request line is what "four hardware
    // sound channels" cashes out to at this boundary.
    {
        bool four_distinct = true;
        std::string detail;
        for (int i = 0; i < 4; ++i) {
            Harness h;
            int bit_to_write = i;
#if MUT_FOUR
            if (i == 3) continue;  // MUTATION: channel 3's own step is skipped, leaving only three proven.
#endif
            uint16_t data = static_cast<uint16_t>(0x8200 | (1 << bit_to_write));
            h.write_reg(DMACON_W, data);
            uint32_t mask = h.observe_audio_dmal(STRHOR_PERIOD * 4 + 8) & 0xF;
            bool ok = mask == static_cast<uint32_t>(1 << i);
            four_distinct = four_distinct && ok;
            detail += ok ? "1" : "0";
        }
        four_distinct = four_distinct && (detail.size() == 4);
        checks.expect(four_distinct,
            "FOUR-CHANNELS-EXIST: four independent DMACON enable bits, "
            "each gating exactly one channel's own DMA-request line "
            "(AUDxEN-BIT-MAPS-TO-OWN-CHANNEL's own four-way sweep), is "
            "what 'The Amiga has four hardware sound channels.' (p.136, "
            "5-p136.c1, 5-hw.c1) and 'The Amiga has four audio channels.' "
            "(p.137, 5-chan.c1) cash out to at paula.v's own boundary. "
            "measured per-channel match string=" + detail + ".");
    }

    // ---------------- AUDxEN-CLEAR-STOPS-CHANNEL-AT-ANY-TIME ----------------
    {
        Harness h;
        h.write_reg(AUD0PER_W, 8);  // Small period: lets the channel reach a genuine, self-sustaining request cycle.
        h.write_reg(DMACON_W, 0x8201);  // SET DMAEN + AUD0EN.
        // Answer it so it actually reaches AUDIO_STATE_3/4 and keeps
        // re-requesting on its own period counter -- otherwise it would
        // fire one pointer-reload request and go quiet forever
        // regardless of DMACON, making "it stopped" true for the wrong
        // reason (see run_answering's own comment).
        bool started = (h.run_answering(0x1, 400) & 0x1) == 1;
#if !MUT_STOP
        h.write_reg(DMACON_W, 0x0001);  // CLR AUD0EN, DMAEN left set -- stop "at any time", not at a natural boundary.
#endif
        // A request already in flight when the clear happens still has
        // to drain through paula_audio.v's own strhor latch (dmal is a
        // registered copy of dmareq, not instantaneous) -- confirmed by
        // VCD trace that dmareq/audio_dmal settle within well under one
        // strhor period (STRHOR_PERIOD*4 raw ticks) of the clear taking
        // effect. The grace window is that one period; the confirm
        // window then requires audio_dmal bit 0 to be 0 on EVERY tick
        // for several more periods.
        bool stopped = h.confirm_stays_off(0x1, STRHOR_PERIOD * 4 * 2, STRHOR_PERIOD * 4 * 4);
        checks.expect(started && stopped,
            "AUDxEN-CLEAR-STOPS-CHANNEL-AT-ANY-TIME: after DMAEN+AUD0EN "
            "start the channel and it is answered into a genuine, "
            "self-sustaining request cycle (audio_dmal bit 0 seen "
            "repeatedly), clearing AUD0EN alone at an arbitrary point in "
            "that cycle (not a length-counter boundary) makes audio_dmal "
            "bit 0 never appear again, once any already-in-flight request "
            "has drained -- auden is a continuous assignment off the "
            "DMACON/DMAEN registers (paula.v:179-183), not gated by any "
            "internal channel state, so the stop is unconditional. "
            "'You can stop the channel by writing a 0 into the AUDxEN bit "
            "at any time.' (p.145, 5-p145.c1, 5-stop.c1). measured started=" +
            std::string(started?"yes":"no") + " stopped=" +
            std::string(stopped?"yes":"no") + ".");
    }

    // ---------------- DMACON-ADDRESS-AND-WRITE-ONLY ----------------
    {
        Harness h;
        uint16_t before_write = h.read_reg(DMACON_W);
#if MUT_CHIP
        h.write_reg(DMACON_W + 1, 0x8201);  // MUTATION: wrong address.
#else
        h.write_reg(DMACON_W, 0x8201);  // SET DMAEN + AUD0EN, at $096.
#endif
        bool side_effect_seen = (h.observe_audio_dmal(STRHOR_PERIOD * 4 + 8) & 0x1) == 1;
        uint16_t after_write = h.read_reg(DMACON_W);
        checks.expect(before_write == 0 && side_effect_seen && after_write == 0,
            "DMACON-ADDRESS-AND-WRITE-ONLY: writing at word address 0x" +
            hex(DMACON_W) +
            " ($096>>1, i.e. byte address $096) is the write that reaches "
            "auden (proving the address decode is exactly $096), yet "
            "reading back that same address, before or after, returns 0 "
            "both times -- paula.v's own data_out mux (paula.v:157) never "
            "includes a DMACON term. 'DMACON 096 W A D P DMA control "
            "write (clear or set)' (appendix A p.278, 5-appA-p278.c1, "
            "5-appA-p278.c2, 5-appA-p278.c4, A-dmacon.addr, A-dmacon.acc); "
            "'DMACON 096 W D DMA control write (clear or set)' (appendix "
            "B p.290, 5-appB-p290.c1, 5-appB-p290.c2, 5-appB-p290.c4). "
            "measured before=" + std::to_string(before_write) +
            " after=" + std::to_string(after_write) + ".");
    }

    // ---------------- DMACON-DECODED-BY-PAULA ----------------
    // Same measurement technique as DMACON-ADDRESS-AND-WRITE-ONLY, cited
    // separately because chip ownership is its own printed proposition.
    // Carries its own MUT_CHIP guard (the same edit DMACON-ADDRESS-AND-
    // WRITE-ONLY's guard applies above): without it, this block always
    // wrote DMACON_W regardless of the flag, so the flag never touched
    // the stimulus this assertion measures and it stayed green under
    // -DMUT_CHIP=1 no matter what the mutation log claimed.
    {
        Harness h;
#if MUT_CHIP
        h.write_reg(DMACON_W + 1, 0x8201);  // MUTATION: wrong address (same edit as DMACON-ADDRESS-AND-WRITE-ONLY).
#else
        h.write_reg(DMACON_W, 0x8201);
#endif
        bool paula_decodes_it = (h.observe_audio_dmal(STRHOR_PERIOD * 4 + 8) & 0x1) == 1;
        checks.expect(paula_decodes_it,
            "DMACON-DECODED-BY-PAULA: this fixture IS paula.v, and a write "
            "at $096 visibly changes its own audio_dmal output, so Paula "
            "decodes DMACON. 'DMACON 096 W A D P...' (appendix A p.278) -- "
            "the P half.");
        checks.record(
            "DMACON-DECODED-BY-PAULA [Agnus and Denise thirds, RECORDED "
            "not asserted]: the same appendix A row's 'A' (Agnus) and 'D' "
            "(Denise) are not established at this fixture -- agnus.v and "
            "denise.v are both outside paula.v's boundary and this "
            "package does not compile either. The Denise half is "
            "independently confirmed by a plain grep over denise.v (not "
            "a Verilator measurement): a search for '096' or 'DMACON' "
            "over the whole file returns exactly one comment "
            "(denise.v:166, 'needs bits in DMACON also') and no register "
            "decode at all. Appendix B p.290's own chip column for this "
            "row gives only 'D' -- conflicting with appendix A's 'A D P' "
            "by being narrower, not opposite, per this repository's own "
            "chip-column.md finding that appendix B's chip column "
            "demonstrably drops letters. The Agnus half is not measured "
            "or grepped at this fixture at all -- neither confirmed nor "
            "contradicted here. "
            "5-appA-p278.c3, A-dmacon.chip.");
    }

    // ---------------- DMACONR-NOT-DECODED-BY-PAULA ----------------
    // A real assertion, not a record: the package measures that paula.v
    // has no DMACONR decode and that the manual's own Paula-ownership
    // claim cannot hold, and that red is reported, not suppressed. The
    // manual's claim (chip column "A P") is that Paula is one of DMACONR's
    // two decoders, so if that held, this address's readback would track
    // paula.v's own internal DMA-control state the same way ADKCONR
    // (a register Paula genuinely does decode) tracks ADKCON. It does not.
    {
        Harness h;
        uint16_t idle = h.read_reg(DMACONR_W);
        h.write_reg(ADKCON_W, 0x8033);  // SET bits 0,1,4,5 -- gives ADKCONR a genuinely non-zero value for the mutation below.
        h.write_reg(DMACON_W, 0x8203);  // SET: DMAEN, AUD0EN, AUD1EN -- if Paula decoded DMACONR, its DMA-control state just changed.
#if !MUT_DMACONR
        uint16_t v = h.read_reg(DMACONR_W);  // $002>>1 -- DMACONR's own claimed address.
#else
        uint16_t v = h.read_reg(ADKCONR_W);  // MUTATION: substitute an address Paula genuinely decodes.
#endif
        bool paula_decodes_dmaconr = (v != idle);
        checks.expect(paula_decodes_dmaconr,
            "DMACONR-NOT-DECODED-BY-PAULA: after ADKCON is set to 0x33 and "
            "DMACON is written 0x8203 (DMAEN, AUD0EN, AUD1EN all set), "
            "reading word address 0x" +
            hex(DMACONR_W) +
            " ($002>>1) through paula.v's own read multiplexer "
            "(paula.v:157: uartdata_out | intdata_out | diskdata_out | "
            "adkconr -- no DMACONR term at all) still returns 0x" +
            hex(v) +
            ", identical to the pre-write idle readback 0x" +
            hex(idle) +
            " -- paula.v does not decode DMACONR at all, so the 'P' half "
            "of the chip-ownership claim cannot hold at this boundary. "
            "'DMACONR 002 R A P DMA control (and blitter status) read' "
            "(appendix A p.278, 5-appA-p278.c7, A-dmaconr.chip); "
            "'DMACONR *002 R A P...' (appendix B p.289, 5-appB-p289.c3). "
            "Agnus alone decodes it (agnus.v:104 `localparam DMACONR = "
            "9'h002` and agnus.v:281), outside this package's own paula.v "
            "boundary -- the 'A' half of 'A P' is not measured here.");
    }

    // ---------------- ADKCON-ADDRESS-WRITE-ONLY-AND-CHIP ----------------
    {
        Harness h;
        uint16_t before_write = h.read_reg(ADKCON_W);
#if MUT_ADKADDR
        h.write_reg(ADKCON_W + 2, 0x8800);  // stimulus mutation: the neighbouring address, not ADKCON.
#else
        h.write_reg(ADKCON_W, 0x8800);  // SET bit 11 (UARTBRK) -- a visible side effect (txd) proves the address.
#endif
        h.ticks(8);
        bool side_effect_seen = h.dut->txd == 0;
        uint16_t after_write = h.read_reg(ADKCON_W);
        checks.expect(before_write == 0 && side_effect_seen && after_write == 0,
            "ADKCON-ADDRESS-WRITE-ONLY-AND-CHIP: writing word address 0x" +
            hex(ADKCON_W) +
            " ($09E>>1, byte address $09E, this fixture's own paula.v) "
            "with bit 11 set forces txd low (its own side effect, "
            "proving the address and that Paula decodes it), yet reading "
            "back address $09E itself, before or after, returns 0 both "
            "times -- write-only, matching 'For example, for the 680x0 "
            "to write to ADKCON (address = $09E)...' (appendix A p.263, "
            "A-pre.c2 -- the $09E half only; the $DFF09E 680x0-side "
            "address is formed outside every boundary this fixture has); "
            "'Registers are either read-only or write-only.' (appendix A "
            "p.263, A-pre.c12, corroborated by ADKCON $09E write / "
            "ADKCONR $010 read being two distinct addresses, see the "
            "next assertion); 'ADKCON 09E W P Audio, disk, control "
            "write' (appendix A p.265, 5-appA-p265.c1, 5-appA-p265.c2, "
            "5-appA-p265.c3, 5-appA-p265.c4, A-adkcon.addr, A-adkcon.acc, "
            "A-adkcon.chip); 'ADKCON 09E W P Audio, disk, UART control' "
            "(appendix B p.290, 5-appB-p290.c14, 5-appB-p290.c15, "
            "5-appB-p290.c16, 5-appB-p290.c17, B-adkcon.addr, "
            "B-adkcon.acc, B-adkcon.chip). measured before=" +
            std::to_string(before_write) + " after=" +
            std::to_string(after_write) + ". CH8 S2 CITES: opus:8-dsk.c28, opus:8-dsk.c29, sol:8-reglist.c11, opus:8-adkcon.c1, opus:8-adkcon.c2, sol:8-adkcon.c1, opus:8-appA-adkcon.c1, opus:8-appA-adkcon.c2, sol:8-appa-adk.c1, sol:8-appa-adk.c2, opus:8-appB-adkcon.c1, opus:8-appB-adkcon.c3, sol:8-appb-adkcon.c1, sol:8-appb-adkcon.c2.");
    }

    // ---------------- ADKCONR-ADDRESS-READONLY-AND-CHIP ----------------
    {
        Harness h;
#if MUT_ADKRONLY
        h.write_reg(ADKCON_W, 0x0033);  // stimulus mutation: SET/CLR=0, so the seed bits never get set.
#else
        h.write_reg(ADKCON_W, 0x8033);  // SET bits 0,1,4,5.
#endif
        uint16_t readback = h.read_reg(ADKCONR_W);
        // Attempt a "write" at ADKCONR's own address: paula.v has no
        // write decode there, so the stored register must not move.
        h.write_reg(ADKCONR_W, 0x00FF);
        uint16_t after_attempted_write = h.read_reg(ADKCONR_W);
        checks.expect((readback & 0x33) == 0x33 && after_attempted_write == readback,
            "ADKCONR-ADDRESS-READONLY-AND-CHIP: reading word address 0x" +
            hex(ADKCONR_W) +
            " ($010>>1) after ADKCON bits 0,1,4,5 are set returns exactly "
            "those bits, proving both the address and that Paula "
            "implements it; a subsequent write attempt AT that same "
            "address changes nothing (paula.v's ADKCON write-decode only "
            "matches $09E, never $010) -- proving read-only. 'ADKCONR 010 "
            "R P Audio, disk, control read' (appendix A p.265, "
            "5-appA-p265.c5, 5-appA-p265.c6, 5-appA-p265.c7, "
            "5-appA-p265.c8, A-adkconr.addr, A-adkconr.acc, "
            "A-adkconr.chip); 'ADKCONR *010 R P Audio, disk control "
            "register read' (appendix B p.289, 5-appB-p289.c6, "
            "5-appB-p289.c7, 5-appB-p289.c8, 5-appB-p289.c9, "
            "B-adkconr.addr, B-adkconr.acc, B-adkconr.chip). measured "
            "readback=0x" + hex(readback) +
            " after_attempted_write=0x" +
            hex(after_attempted_write) + ". CH8 S2 CITES: opus:8-dsk.c30, opus:8-dsk.c31, sol:8-reglist.c13, opus:8-adkcon.c1, opus:8-adkcon.c3, opus:8-adkcon.c30, sol:8-adkcon.c2, sol:8-adkcon.c26, sol:8-adkcon.c27, opus:8-appA-adkcon.c5, opus:8-appA-adkcon.c6, opus:8-appA-adkcon.c28, opus:8-appA-adkcon.c29, opus:8-appA-adkcon.c30, opus:8-appA-adkcon.c31, opus:8-appA-adkcon.c32, opus:8-appA-adkcon.c33, opus:8-appA-adkcon.c34, opus:8-appA-adkcon.c35, opus:8-appA-adkcon.c36, sol:8-appa-adk.c4, sol:8-appa-adkr.c1, sol:8-appa-adkr.c2, sol:8-appa-adkr.c4, sol:8-appa-adkbit.c22, sol:8-appa-adkbit.c23, sol:8-appa-adkbit.c24, sol:8-appa-adkbit.c25, sol:8-appa-adkbit.c26, sol:8-appa-adkbit.c27, sol:8-appa-adkbit.c28, sol:8-appa-adkbit.c29, sol:8-appa-adkbit.c30, sol:8-appa-adkbit.c31, sol:8-appa-adkbit.c32, opus:8-appB-adkconr.c1, opus:8-appB-adkconr.c3, sol:8-appb-adkconr.c1, sol:8-appb-adkconr.c3, sol:8-appb-adkconr.c5, sol:8-appb-adkcon.c4.");
    }

    // ---------------- ADKCON-BIT11-UARTBRK-FORCES-TXD-LOW ----------------
    {
        Harness h;
        bool idle_high = h.dut->txd == 1;
#if !MUT_UARTBRK
        h.write_reg(ADKCON_W, 0x8800);  // SET bit 11 (UARTBRK).
#else
        h.write_reg(ADKCON_W, 0x8400);  // MUTATION: bit 10 (WORDSYNC) instead of bit 11.
#endif
        h.ticks(8);
        bool forced_low = h.dut->txd == 0;
        h.write_reg(ADKCON_W, 0x0800);  // CLR bit 11.
        h.ticks(8);
        bool released_high = h.dut->txd == 1;
        checks.expect(idle_high && forced_low && released_high,
            "ADKCON-BIT11-UARTBRK-FORCES-TXD-LOW: after reset txd idles "
            "high; setting ADKCON bit 11 forces it low (paula.v:216 wires "
            "adkcon[11] to paula_uart's uartbrk, paula_uart.v:146 `if "
            "(uartbrk) tx_txd <= #1 1'b0`); clearing bit 11 releases it "
            "back to the idle-mark 1. 'Forces a UART break (clears TXD) "
            "if true.' (appendix A p.265, 5-appA-p265.c18, A-adkcon.b11). CH8 S2 CITES: opus:8-adkcon.c4, sol:8-adkcon.c3, opus:8-appA-adkcon.c18, opus:8-appA-adkcon.c19, sol:8-appa-adkbit.c10, sol:8-appb-adkcon.c6.");
    }

    // ---------------- ADKCON-BIT10-WORDSYNC [NOT COVERED BY THIS PACKAGE] ----------------
    // This claim's ids are named
    // only in this file's own header (CLAIM COVERAGE section above), never
    // in this record() call or any assertion, so an ID-search coverage
    // gate finds them nowhere in this file's runtime text and does not
    // credit this package with testing them.
    checks.record(
        "ADKCON-BIT10-WORDSYNC [NOT COVERED BY THIS PACKAGE]: paula.v:283 "
        "wires adkcon[10] to paula_floppy's wordsync port, and "
        "paula_floppy.v:924-925 gates trackrdok on a sync-code match "
        "against real disk data. This module's own disk interface is the "
        "MiSTeryNano SD-card protocol (sdc_busy/sdc_done/byte-strobe), "
        "not a raw serial bitstream this fixture can trivially drive -- "
        "exercising wordsync's own effect needs a working SD-card "
        "floppy-image fixture, out of this audio/DMACON/ADKCON-focused "
        "package's own declared scope. 'Enables disk read synchronizing "
        "on a word equal to DISK SYNC CODE, located in address (3F)*2.' "
        "(appendix A p.265). See this file's header for the specific "
        "claim ids.");

    // ---------------- AUDIO-REGISTERS-HAVE-NO-READ-PATH ----------------
    {
        const int addrs[16] = {
            AUD0LEN_W, AUD0PER_W, AUD0VOL_W, AUD0DAT_W,
            AUD1LEN_W, AUD1PER_W, AUD1VOL_W, AUD1DAT_W,
            AUD2LEN_W, AUD2PER_W, AUD2VOL_W, AUD2DAT_W,
            AUD3LEN_W, AUD3PER_W, AUD3VOL_W, AUD3DAT_W,
        };
        Harness h;
        // Write a distinct non-zero value to every one of the sixteen
        // addresses first, so a zero readback proves absence of a read
        // path rather than merely "never written".
        for (int i = 0; i < 16; ++i) h.write_reg(addrs[i], static_cast<uint16_t>(0x1000 + i));
#if MUT_NOREADPATH
        h.write_reg(ADKCON_W, 0x80FF);  // A register that DOES read back non-zero, substituted in below.
#endif
        bool all_zero = true;
        std::string nonzero_at;
        for (int i = 0; i < 16; ++i) {
#if MUT_NOREADPATH
            uint16_t v = h.read_reg(i == 0 ? ADKCONR_W : addrs[i]);  // MUTATION: first slot reads ADKCONR, which is genuinely non-zero here.
#else
            uint16_t v = h.read_reg(addrs[i]);
#endif
            if (v != 0) { all_zero = false; nonzero_at += " idx" + std::to_string(i); }
        }
        checks.expect(all_zero,
            "AUDIO-REGISTERS-HAVE-NO-READ-PATH: all sixteen AUDxLEN/PER/"
            "VOL/DAT addresses (channels 0-3), each first written a "
            "distinct non-zero value, read back as 0x0000 -- "
            "paula_audio.v has no data_out port at all and "
            "paula_audio_channel.v has none either, so paula.v:157's own "
            "read multiplexer (uartdata_out | intdata_out | diskdata_out "
            "| adkconr) carries no audio term whatsoever. 'AUDxDAT 0AA W "
            "P Audio channel x data' / 'AUDxLEN 0A4 W P...' / 'AUDxPER "
            "0A6 W P(E)...' / 'AUDxVOL 0A8 W P...' (appendix A pp.265-"
            "266, 5-appA-p265.c34, 5-appA-p266.c15, 5-appA-p266.c20, "
            "5-appA-p266.c28, A-audxdat.acc, A-audxlen.acc, "
            "A-audxper.acc, A-audxvol.acc); the same four rows repeated "
            "per channel on appendix B p.290-291 (5-appB-p290.c34,c39,"
            "c44,c49,c69,c74,c79,c84,c104,c109,c114,c119,c139,c144, "
            "5-appB-p291.c2,c7, B-aud0len.acc, B-aud0per.acc, "
            "B-aud0vol.acc, B-aud0dat.acc, B-aud1len.acc, B-aud1per.acc, "
            "B-aud1vol.acc, B-aud1dat.acc, B-aud2len.acc, B-aud2per.acc, "
            "B-aud2vol.acc, B-aud2dat.acc, B-aud3len.acc, B-aud3per.acc, "
            "B-aud3vol.acc, B-aud3dat.acc); and AUDxPER again on "
            "appendix C's ECS register map, pp.298 and 310 "
            "(5-appC-p298.c9, 5-appC-p310.c3, C-map.per.acc). measured "
            "nonzero at:" + (nonzero_at.empty() ? " none" : nonzero_at) + ".");
    }

    // ---------------- ADKCON-ATTACH-MEASUREMENT-BASIS-CONTROL [control] ----------------
    {
        Harness h;
        h.write_reg(ADKCON_W, 0x80FF);  // All eight attach/modulate bits set (same as the finding below).
        h.write_reg(AUD0VOL_W, 0);      // Volume 0: genuinely silent, regardless of ADKCON.
        h.write_reg(AUD0DAT_W, 0x7F7F);
        h.write_reg(AUD1VOL_W, 0);
        h.write_reg(AUD1DAT_W, 0x7F7F);
        Trace t = capture(h, 200);
        bool genuinely_silent = peak_abs(t.ldata) == 0 && peak_abs(t.rdata) == 0;
        checks.expect(genuinely_silent,
            "ADKCON-ATTACH-MEASUREMENT-BASIS-CONTROL [control, no claim "
            "id]: on the identical stimulus and measurement basis the "
            "finding immediately below uses (same ADKCON=0x80FF, same "
            "AUD0DAT/AUD1DAT), setting AUDxVOL=0 instead of 64 DOES "
            "silence both channels (ldata=rdata=0) -- proving this "
            "harness's peak-detection technique correctly reports "
            "silence when a channel is genuinely silent, ruling out a "
            "units-error explanation for the finding's own red result.");
    }

    // ---------------- ADKCON-ATTACH-BITS-DO-NOT-SILENCE-CHANNELS ----------------
    {
        Harness h;
        h.write_reg(ADKCON_W, 0x80FF);  // SET all eight: ATPER0-3 (bits 4-7), ATVOL0-3 (bits 0-3).
#if !MUT_ATTACH
        h.write_reg(AUD0VOL_W, 64);
        h.write_reg(AUD0DAT_W, 0x7F7F);  // channel 0 sample=127 -> would-be peak 8001.
        h.write_reg(AUD1VOL_W, 64);
        h.write_reg(AUD1DAT_W, 0x7F7F);  // channel 1 sample=127 -> would-be peak 8001.
#endif
        // MUTATION: with MUT_ATTACH, neither AUD0DAT nor AUD1DAT is ever
        // written, so both channels are genuinely silent by construction.
        Trace t = capture(h, 200);
        int ldata_peak = peak_abs(t.ldata);
        int rdata_peak = peak_abs(t.rdata);
        bool manual_predicts_silenced = (ldata_peak == 0 && rdata_peak == 0);
        checks.expect(manual_predicts_silenced,
            "ADKCON-ATTACH-BITS-DO-NOT-SILENCE-CHANNELS: with every one "
            "of ADKCON's eight audio attach/modulate bits set (ATPER0-3, "
            "ATVOL0-3), channel 0 armed for a would-be ldata peak of "
            "8001 and channel 1 for a would-be rdata peak of 8001 (the "
            "exact stimulus already measured for this claim) STILL "
            "reach those peaks -- attach/modulate has zero effect. "
            "paula_audio_channel.v:1's own first line: 'This module "
            "handles a single amiga audio channel. attached modes are "
            "not supported.' paula.v:303-321 instantiates paula_audio "
            "with NO adkcon port at all -- adkcon[7:0] is stored at "
            "paula.v:191-197 and reaches nothing; only adkcon[11] "
            "(uartbrk) and adkcon[10] (wordsync) leave the register, "
            "both disk/UART, neither audio. This is expected RED: 'If "
            "the \"attach\" bit for an audio channel is set, that "
            "channel ceases to produce sound...' (p.149, 5-p149.c12, "
            "5-mod.c6); 'If you set either the volume or period attach "
            "bits for a channel, that channel's audio output will be "
            "disabled...' (p.150, 5-p150.c24, 5-mod.c21); 'Writing a 1 "
            "into channel 3's modulate bits only disables its audio "
            "output.' (p.150, 5-p150.c27, 5-mod.c24); 'ATPER3 Use audio "
            "channel 3 to modulate nothing (disables audio output of "
            "channel 3)' (p.151, 5-p151.c1, 5-t55.b7); 'ATVOL3 Use audio "
            "channel 3 to modulate nothing (disables audio output of "
            "channel 3)' (p.151, 5-p151.c5, 5-t55.b3). A red result here "
            "is a finding about paula.v, not an error in this assertion "
            "-- see the control immediately above (same basis, AUDxVOL=0 "
            "genuinely reads silent) and the ADKCON-ATTACH-MEASUREMENT-"
            "BASIS-CONTROL mutation below. measured ldata_peak=" +
            std::to_string(ldata_peak) + " rdata_peak=" +
            std::to_string(rdata_peak) + " (expected 0 and 0 if the "
            "manual held; actually near 8001 and 8001). CH8 S2 CITES: opus:8-adkcon.c30, sol:8-adkcon.c26, sol:8-adkcon.c27, opus:8-appA-adkcon.c28, opus:8-appA-adkcon.c29, opus:8-appA-adkcon.c30, opus:8-appA-adkcon.c31, opus:8-appA-adkcon.c32, opus:8-appA-adkcon.c33, opus:8-appA-adkcon.c34, opus:8-appA-adkcon.c35, opus:8-appA-adkcon.c36, sol:8-appa-adk.c4, sol:8-appa-adkr.c4, sol:8-appa-adkbit.c22, sol:8-appa-adkbit.c23, sol:8-appa-adkbit.c24, sol:8-appa-adkbit.c25, sol:8-appa-adkbit.c26, sol:8-appa-adkbit.c27, sol:8-appa-adkbit.c28, sol:8-appa-adkbit.c29, sol:8-appa-adkbit.c30, sol:8-appa-adkbit.c31, sol:8-appa-adkbit.c32, sol:8-appb-adkconr.c5, sol:8-appb-adkcon.c4.");
    }

    // ---------------- DMA-ENABLED-CHANNEL-WITH-SUPPLIED-DATA-PRODUCES-SOUND ----------------
    {
        Harness h;
        h.write_reg(AUD0VOL_W, 64);
#if MUT_DMASOUND
        h.write_reg(DMACON_W, 0x8200);  // MUTATION: DMAEN alone, AUD0EN left clear.
#else
        h.write_reg(DMACON_W, 0x8201);  // SET DMAEN + AUD0EN.
#endif
        int peak = 0;
        int answered = 0;
        for (int i = 0; i < 4000 && answered < 6; ++i) {
            h.tick();
            if (h.dut->clk7_en && (h.dut->audio_dmal & 0x1)) {
                h.write_reg(AUD0DAT_W, 0x7F7F);
                ++answered;
                h.ticks(40);  // cooldown before polling again
            }
            int v = sign_extend_15(h.dut->ldata);
            if (std::abs(v) > std::abs(peak)) peak = v;
        }
        bool produced_sound = std::abs(peak) > 1000;
        checks.expect(produced_sound,
            "DMA-ENABLED-CHANNEL-WITH-SUPPLIED-DATA-PRODUCES-SOUND: with "
            "DMACON setting DMAEN and AUD0EN together and AUD0VOL=64, "
            "answering every audio_dmal[0] request seen with AUD0DAT="
            "0x7F7F makes ldata reach a clearly non-zero peak (|ldata| > "
            "1000, well past noise) -- 'After you have defined the audio "
            "data location, length, volume and period, you can play the "
            "waveform by starting the DMA for that audio channel. This "
            "starts the output of sound.' (p.144, 5-p144.c1, 5-play.c1); "
            "'Normally, each channel produces sound when it is enabled.' "
            "(p.149, 5-p149.c11). measured peak=" + std::to_string(peak) +
            " answered_requests=" + std::to_string(answered) + ".");
    }

    // ---------------- DSKEN-BIT-NOT-INDEPENDENTLY-TESTABLE [NOT COVERED BY THIS PACKAGE] ----------------
    // Same treatment as
    // ADKCON-BIT10-WORDSYNC above -- ids named only in the header, not
    // repeated in this record() call.
    checks.record(
        "DSKEN-BIT-NOT-INDEPENDENTLY-TESTABLE [NOT COVERED BY THIS "
        "PACKAGE]: dsken (paula.v:179, `dsken = dmacon[4] & dmaen`) is an "
        "internal wire, not a paula.v port -- its only externally "
        "observable consequence is disk_dmal, which is paula_floppy's own "
        "dmal output and depends on paula_floppy's own DSKLEN/state "
        "machine, not on dsken alone. Exercising it would need the same "
        "SD-card-floppy fixture ADKCON-BIT10-WORDSYNC above needs, out of "
        "this audio/DMACON/ADKCON-focused package's own declared scope. "
        "'Disk DMA enable' (appendix A p.278). See this file's header "
        "for the specific claim ids.");

    // ---------------- AUDIO-INTERRUPT-EXISTENCE-AND-CADENCE [NOT COVERED BY THIS PACKAGE] ----------------
    // Same treatment as the two
    // record() calls above -- ids named only in the header, not repeated
    // here.
    checks.record(
        "AUDIO-INTERRUPT-EXISTENCE-AND-CADENCE [NOT COVERED BY THIS "
        "PACKAGE]: the audio interrupt request itself (an existence-"
        "plus-cadence claim, with a three-way conflict over the exact "
        "moment: printed pp.147/148 vs 164 vs appendix A p.265) is "
        "directly observable at paula_audio_channel's own intreq port, "
        "a boundary other than this package's. At paula.v's own boundary "
        "the only observable is _ipl, which additionally requires "
        "configuring paula_intcontroller's INTENA masking -- register "
        "semantics of chapter 7 (pp.217-222), not this chapter. "
        "Reaching _ipl would just be a more roundabout measurement of "
        "the identical fact through chapter-7-owned register semantics "
        "this package has no need of for its own assigned DMACON/ADKCON "
        "register pair. 'Create an interrupt for the 680x0 saying that "
        "it has completed retrieving working copies of length and "
        "location registers.' (p.148). See this file's header for the "
        "specific claim ids.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
