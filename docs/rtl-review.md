# RTL review

A finding page whose title ends in **[RTL review]** asks an Amiga specialist
to decide whether NanoMig's RTL should change. Its header says three things:
it is open, it is filed against the RTL, and a graded red holds it up -- a
test that measures the RTL doing something the manual does not describe.
This suite stops there. The decision is the specialist's, and each page's
*Suggested change* states one option, not a verdict.

**47 findings ask for a review, in 25 modules.** They are listed by the
module they are about, so a reader who knows one part of the chip set can
start with their own; a finding about two modules is under both. The
others ask for no RTL review:

| findings | why no RTL review |
|---|---|
| 12 | filed against the printed page or the classification: no RTL change follows |
| 5 | filed against the RTL, but a source observation no test measures yet: it needs a test before it asks anything |
| 7 | withdrawn |

The marks and this list are derived from each page's header table by
`hrm-testpipeline/tools/write_rtl_review.py`, which lives in the separate
`hrm-testpipeline` repository, and are rewritten whenever a header changes.
Module links point at the NanoMig revision the pages cite, `4495f86`.

## [`src/minimig-aga/agnus.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-066](findings/NM-HRM-066.md) | DDFSTRT = $0030 disables sprite 7 only when enough bitplanes are fetched | a graded red, contested — the implementations split |

## [`src/minimig-aga/agnus_audiodma.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_audiodma.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-024](findings/NM-HRM-024.md) | the audio DMA pointer takes five high bits in both chipsets | a graded red |

## [`src/minimig-aga/agnus_beamcounter.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v) — 6

| finding | subject | evidence |
|---|---|---|
| [NM-AA-004](findings/NM-AA-004.md) | VPOSR reports chip identifier $23, where the AA list gives an Alice $22 | a graded red |
| [NM-AA-005](findings/NM-AA-005.md) | the long-line toggle runs, but no line is a colour clock longer | a graded red |
| [NM-HRM-011](findings/NM-HRM-011.md) | PAL vertical blanking is 26 lines, the manual says 29 | a graded red |
| [NM-HRM-032](findings/NM-HRM-032.md) | BEAMCON0's three sync-polarity bits are decoded nowhere | a graded red |
| [NM-HRM-033](findings/NM-HRM-033.md) | LOLDIS does not disable the long-line toggle on its own | a graded red |
| [NM-HRM-034](findings/NM-HRM-034.md) | VPOSR reports the video standard in bit 12 of an OCS configuration | a graded red, contested — the implementations split |

## [`src/minimig-aga/agnus_bitplanedma.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v) — 2

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-007](findings/NM-HRM-007.md) | the hires fetch width cannot reach the manual's maximum of 49 words | a graded red |
| [NM-HRM-066](findings/NM-HRM-066.md) | DDFSTRT = $0030 disables sprite 7 only when enough bitplanes are fetched | a graded red, contested — the implementations split |

## [`src/minimig-aga/agnus_blitter.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_blitter.v) — 3

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-020](findings/NM-HRM-020.md) | BLTSIZV is not gated on ECS, while its two siblings are | a graded red |
| [NM-HRM-021](findings/NM-HRM-021.md) | DOFF (BLTCON1 bit 7) has no observable effect | a graded red |
| [NM-HRM-022](findings/NM-HRM-022.md) | BLTxPTH carries five high bits in both chipsets | a graded red |

## [`src/minimig-aga/agnus_copper.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v) — 3

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-003](findings/NM-HRM-003.md) | OCS Copper address uses five high bits instead of three | a graded red |
| [NM-HRM-004](findings/NM-HRM-004.md) | ECS danger bit admits too small a range | a graded red |
| [NM-HRM-005](findings/NM-HRM-005.md) | WAIT and SKIP take longer than the manual states | a graded red |

## [`src/minimig-aga/agnus_diskdma.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_diskdma.v) — 2

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-042](findings/NM-HRM-042.md) | the disk DMA pointer stores 20 word-address bits where two pages print 32 | a graded red |
| [NM-HRM-043](findings/NM-HRM-043.md) | DSKPTH carries five high bits in both chipsets | a graded red |

## [`src/misc/amiga_keymap.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/misc/amiga_keymap.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-050](findings/NM-HRM-050.md) | one of the two international cutout keys reaches no code | a graded red |

## [`src/minimig-aga/cia_int.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_int.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-063](findings/NM-HRM-063.md) | FLAG is negative edge-sensitive; the RTL ORs a level | a graded red |

## [`src/minimig-aga/ciaa.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v) — 2

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-040](findings/NM-HRM-040.md) | CIAADDRA's direction mask corresponds one-to-one for two bit positions of eight | a graded red, contested — the implementations split |
| [NM-HRM-054](findings/NM-HRM-054.md) | the parallel port has a direction register and no data register | a graded red |

## [`src/minimig-aga/cpu_cache_new.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_cache_new.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-060](findings/NM-HRM-060.md) | the instruction cache monitors writes, and the manual says it does not | a graded red |

## [`src/minimig-aga/cpu_wrapper.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-064](findings/NM-HRM-064.md) | the board advertises that it can be shut up, and register 4C has no handler | a graded red |

## [`src/minimig-aga/denise.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v) — 2

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-015](findings/NM-HRM-015.md) | BRDNTRAN is declared and never connected | a graded red |
| [NM-HRM-017](findings/NM-HRM-017.md) | SuperHires bitplane colour encoding is not implemented | a graded red |

## [`src/minimig-aga/denise_collision.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_collision.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-008](findings/NM-HRM-008.md) | playfield-1 collision depends on playfield 2, by design, undocumented | a graded red |

## [`src/minimig-aga/denise_colortable.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_colortable.v) — 2

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-016](findings/NM-HRM-016.md) | SuperHires sprite colour encoding is not implemented | a graded red |
| [NM-HRM-017](findings/NM-HRM-017.md) | SuperHires bitplane colour encoding is not implemented | a graded red |

## [`src/minimig-aga/denise_sprites.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites.v) — 3

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-016](findings/NM-HRM-016.md) | SuperHires sprite colour encoding is not implemented | a graded red |
| [NM-HRM-019](findings/NM-HRM-019.md) | SPRxCTL bit 4 (SHSH1) is not implemented | a graded red |
| [NM-HRM-023](findings/NM-HRM-023.md) | the ATTACH bit is honoured in the even sprite, not only the odd | a graded red |

## [`src/misc/hid.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/misc/hid.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-051](findings/NM-HRM-051.md) | Caps Lock is delivered as an ordinary key | a graded red |

## [`src/minimig-aga/minimig.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-035](findings/NM-HRM-035.md) | any write to CIA-A clears the kickstart overlay, not only a write to the OVL bit | a graded red |

## [`src/minimig-aga/paula.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v) — 2

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-028](findings/NM-HRM-028.md) | ADKCON's audio attach bits are stored and reach nothing | a graded red |
| [NM-HRM-030](findings/NM-HRM-030.md) | Paula does not decode DMACONR, though the appendix gives it part ownership | a graded red |

## [`src/minimig-aga/paula_audio_channel.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v) — 2

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-027](findings/NM-HRM-027.md) | in direct mode the interrupt is raised before a word is output | a graded red |
| [NM-HRM-028](findings/NM-HRM-028.md) | ADKCON's audio attach bits are stored and reach nothing | a graded red |

## [`src/minimig-aga/paula_audio_mixer.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_mixer.v) — 2

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-002](findings/NM-HRM-002.md) | audio channels on the wrong stereo side | a graded red |
| [NM-HRM-025](findings/NM-HRM-025.md) | volume 64 and volume 63 are the same gain on the multiply path | a graded red |

## [`src/minimig-aga/paula_floppy.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v) — 5

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-041](findings/NM-HRM-041.md) | DSKBYTR is a constant word in three of its five fields | a graded red |
| [NM-HRM-046](findings/NM-HRM-046.md) | the drive activity light follows DMA, not the motor | a graded red |
| [NM-HRM-047](findings/NM-HRM-047.md) | one step-spacing threshold where the page prints two | a graded red |
| [NM-HRM-048](findings/NM-HRM-048.md) | a documented hardware bug that does not occur here | a graded red, inverted — see its page |
| [NM-HRM-049](findings/NM-HRM-049.md) | the MFM encoder is context-free | a graded red |

## [`src/minimig-aga/paula_intcontroller.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_intcontroller.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-031](findings/NM-HRM-031.md) | INTREQ bit 14 is a settable request bit where the appendix says enable-only | a graded red |

## [`src/minimig-aga/paula_uart.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_uart.v) — 3

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-055](findings/NM-HRM-055.md) | SERDAT written with all zeros still transmits | a graded red |
| [NM-HRM-056](findings/NM-HRM-056.md) | SERDATR's TBE bit cannot be observed clear from outside | a graded red, contested — the implementations split |
| [NM-HRM-057](findings/NM-HRM-057.md) | SERDATR bit 11 is a registered copy of RXD, not a direct read | a graded red, contested — the implementations split |

## [`src/minimig-aga/userio.v`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v) — 2

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-037](findings/NM-HRM-037.md) | the right controller port's X potentiometer input does not exist | a graded red |
| [NM-HRM-038](findings/NM-HRM-038.md) | the pot counters are not held in reset after a POTGO write | a graded red |
