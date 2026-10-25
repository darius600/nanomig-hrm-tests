# RTL review

A finding page whose title ends in **[RTL review]** asks an Amiga specialist
to decide whether NanoMig's RTL should change. Its header says three things:
it is open, it is filed against the RTL, and a failing test holds it up -- a
test that measures the RTL doing something the manual does not describe.
This suite stops there. The decision is the specialist's, and each page's
*Decision* states one option, not a verdict.

**Have one evening?** Start at the register's shortlist,
[If you have one evening](findings.md#if-you-have-one-evening): a few
findings, each with what it takes to settle -- a page to re-read, or one
short change and one package run. This list is sorted by module, not by
effort.

**62 findings ask for a review, in 30 modules.** They are listed by the
module they are about, so a reader who knows one part of the chip set can
start with their own; a finding about two modules is under both. The
others ask for no RTL review:

| findings | why no RTL review |
|---|---|
| 24 | filed against the printed page or the classification: no RTL change follows |
| 2 | filed against the RTL, but seen in the RTL source and not red at any pin: no test yet, or measured only inside the module |

Module links point at the NanoMig revision the pages cite, `4315acb`.

## [`src/minimig-aga/agnus.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v) — 5

| finding | subject | evidence |
|---|---|---|
| [NM-AA-010](findings/NM-AA-010.md) | the UHRES pointers never come out: with BPLCON0 bit 7 and DMAEN set, no BPLHPT or SPRHPT access reaches Chip memory, where AA pp. 15 and 27 give them the first three free cycles of each line | a test fails against the page; the implementations split |
| [NM-AA-013](findings/NM-AA-013.md) | a Copper write to REFPTR moves no refresh address: every refresh slot drives address 0, where AA p. 27 makes REFPTR the refresh address generator | a test fails against the page; the implementations split |
| [NM-HRM-066](findings/NM-HRM-066.md) | DDFSTRT = $0030 disables sprite 7 only when enough bitplanes are fetched | a test fails against the page; the implementations split |
| [NM-HRM-069](findings/NM-HRM-069.md) | the 68000 is given chip-bus slots of both parities, where p. 196 allows it only the even ones | a test fails against the page |
| [NM-HRM-091](findings/NM-HRM-091.md) | with BLTPRI set, the 68000 still gets the chip-bus cycles a blit does not use, where p. 278 prevents it from stealing any bus cycles while blitter DMA is running | a test fails against the page |

## [`src/minimig-aga/agnus_audiodma.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_audiodma.v) — 2

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-024](findings/NM-HRM-024.md) | the audio DMA pointer takes five high bits in both chipsets | a test fails against the page |
| [NM-HRM-092](findings/NM-HRM-092.md) | below the minimum period a pass of audio DMA does not always start again at AUDxLC: channel 0 reads past its table, fetches the first word twice or ends a pass one word short, where pp. 139 and 147 restart it each time | a test fails against the page; the implementations split |

## [`src/minimig-aga/agnus_beamcounter.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v) — 10

| finding | subject | evidence |
|---|---|---|
| [NM-AA-004](findings/NM-AA-004.md) | VPOSR reports chip identifier $23, where the AA list gives an Alice $22 | a test fails against the page |
| [NM-AA-005](findings/NM-AA-005.md) | the long-line toggle runs, but no line is a colour clock longer | a test fails against the page |
| [NM-AA-008](findings/NM-AA-008.md) | HHPOSR reads $0000 and HHPOSW reaches nothing with DUAL set: the secondary beam counter of AA p. 24 is not there | a test fails against the page; the implementations split |
| [NM-AA-009](findings/NM-AA-009.md) | with CSCBEN set, VSY* keeps the vertical sync and VBSTRT never reaches it, where AA pp. 12 and 29 put the variable composite blank on that pin | a test fails against the page |
| [NM-AA-012](findings/NM-AA-012.md) | with VARBEAMEN set and LACE clear, a field lasts VTOTAL + 2 lines while the long-frame bit is set, where AA p. 29 gives VTOTAL + 1 | a test fails against the page; the implementations split |
| [NM-HRM-010](findings/NM-HRM-010.md) | the NTSC line is 227 colour clocks every time, where the manual alternates 228 and 227 | a test fails against the page |
| [NM-HRM-032](findings/NM-HRM-032.md) | BEAMCON0's three sync-polarity bits are decoded nowhere | a test fails against the page |
| [NM-HRM-033](findings/NM-HRM-033.md) | LOLDIS does not disable the long-line toggle on its own | a test fails against the page |
| [NM-HRM-087](findings/NM-HRM-087.md) | with BPLCON0 bit 3 set, VPOSR and VHPOSR keep counting: neither a light-pen trigger nor the end of a field freezes them, where pp. 238 and 239 do both | a test fails against the page; the implementations split |
| [NM-HRM-090](findings/NM-HRM-090.md) | with VARVBEN set, VBSTRT starts no vertical blank: the line it names still shows playfield data, where pp. 304 and 305 make it the line at which blanking starts | a test fails against the page; the implementations split |

## [`src/minimig-aga/agnus_bitplanedma.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_bitplanedma.v) — 3

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-007](findings/NM-HRM-007.md) | the hires fetch width cannot reach the manual's maximum of 49 words | a test fails against the page |
| [NM-HRM-066](findings/NM-HRM-066.md) | DDFSTRT = $0030 disables sprite 7 only when enough bitplanes are fetched | a test fails against the page; the implementations split |
| [NM-HRM-088](findings/NM-HRM-088.md) | BPLxPTH carries five high bits on OCS, where pp. 272 and 291 give it three | a test fails against the page |

## [`src/minimig-aga/agnus_blitter.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_blitter.v) — 4

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-020](findings/NM-HRM-020.md) | BLTSIZV is not gated on ECS, while its two siblings are | a test fails against the page |
| [NM-HRM-021](findings/NM-HRM-021.md) | DOFF (BLTCON1 bit 7) has no observable effect | a test fails against the page |
| [NM-HRM-022](findings/NM-HRM-022.md) | BLTxPTH carries five high bits in both chipsets | a test fails against the page |
| [NM-HRM-091](findings/NM-HRM-091.md) | with BLTPRI set, the 68000 still gets the chip-bus cycles a blit does not use, where p. 278 prevents it from stealing any bus cycles while blitter DMA is running | a test fails against the page |

## [`src/minimig-aga/agnus_copper.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_copper.v) — 4

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-003](findings/NM-HRM-003.md) | OCS Copper address uses five high bits instead of three | a test fails against the page |
| [NM-HRM-005](findings/NM-HRM-005.md) | WAIT and SKIP take longer than the manual states | a test fails against the page |
| [NM-HRM-073](findings/NM-HRM-073.md) | a COPJMP2 written before Copper DMA is turned on starts the Copper at COP1LC, where pp. 25, 26, 30 and 276 start it at COP2LC | a test fails against the page |
| [NM-HRM-089](findings/NM-HRM-089.md) | a Copper WAIT for ($FF,$E2) never ends on line 255, where pp. 23, 32 and 33 make $E2 a position the comparison reaches | a test fails against the page |

## [`src/minimig-aga/agnus_diskdma.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_diskdma.v) — 2

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-042](findings/NM-HRM-042.md) | the disk DMA pointer stores 20 word-address bits where two pages print 32 | a test fails against the page |
| [NM-HRM-043](findings/NM-HRM-043.md) | DSKPTH carries five high bits in both chipsets | a test fails against the page |

## [`src/misc/amiga_keymap.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/misc/amiga_keymap.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-050](findings/NM-HRM-050.md) | one of the two international cutout keys reaches no code | a test fails against the page |

## [`src/minimig-aga/cia_int.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/cia_int.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-063](findings/NM-HRM-063.md) | FLAG is negative edge-sensitive; the RTL ORs a level | a test fails against the page |

## [`src/minimig-aga/cia_timera.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/cia_timera.v) — 2

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-074](findings/NM-HRM-074.md) | PBON and OUTMODE are stored and read back but reach no logic, so PB6 and PB7 never carry the timer output pp. 341, 342 and 348–350 put there | a test fails against the page; the implementations split |
| [NM-HRM-078](findings/NM-HRM-078.md) | CRA bit 5 and CRB bit 5 are stored and never read: a timer set to count CNT counts φ2 | a test fails against the page |

## [`src/minimig-aga/cia_timerb.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/cia_timerb.v) — 2

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-074](findings/NM-HRM-074.md) | PBON and OUTMODE are stored and read back but reach no logic, so PB6 and PB7 never carry the timer output pp. 341, 342 and 348–350 put there | a test fails against the page; the implementations split |
| [NM-HRM-078](findings/NM-HRM-078.md) | CRA bit 5 and CRB bit 5 are stored and never read: a timer set to count CNT counts φ2 | a test fails against the page |

## [`src/minimig-aga/ciaa.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/ciaa.v) — 3

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-040](findings/NM-HRM-040.md) | CIAADDRA's direction mask corresponds one-to-one for two bit positions of eight | a test fails against the page; the implementations split |
| [NM-HRM-054](findings/NM-HRM-054.md) | the parallel port has a direction register and no data register | a test fails against the page |
| [NM-HRM-074](findings/NM-HRM-074.md) | PBON and OUTMODE are stored and read back but reach no logic, so PB6 and PB7 never carry the timer output pp. 341, 342 and 348–350 put there | a test fails against the page; the implementations split |

## [`src/minimig-aga/ciab.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/ciab.v) — 2

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-074](findings/NM-HRM-074.md) | PBON and OUTMODE are stored and read back but reach no logic, so PB6 and PB7 never carry the timer output pp. 341, 342 and 348–350 put there | a test fails against the page; the implementations split |
| [NM-HRM-086](findings/NM-HRM-086.md) | CIA-B has no serial port: in output mode a byte written to its SDR never sets the ICR's SP bit, where pp. 339, 345 and 348 give both 8520s one | a test fails against the page |

## [`src/minimig-aga/cpu_cache_new.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/cpu_cache_new.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-060](findings/NM-HRM-060.md) | the instruction cache monitors writes, and the manual says it does not | a test fails against the page |

## [`src/minimig-aga/cpu_wrapper.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/cpu_wrapper.v) — 2

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-064](findings/NM-HRM-064.md) | the board advertises that it can be shut up, and register 4C has no handler | a test fails against the page |
| [NM-HRM-085](findings/NM-HRM-085.md) | a configured Zorro II memory board answers across the whole 8 MB Zorro II space, not only at the base software wrote to it | a test fails against the page |

## [`src/minimig-aga/denise.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise.v) — 2

| finding | subject | evidence |
|---|---|---|
| [NM-AA-007](findings/NM-AA-007.md) | BPLCON0's BYPASS bit has no effect: the red output still follows the colour table, where AA p. 17 puts the colour address on R(7:0) | a test fails against the page; the implementations split |
| [NM-HRM-017](findings/NM-HRM-017.md) | SuperHires bitplane colour encoding is not implemented | a test fails against the page; the implementations split |

## [`src/minimig-aga/denise_bitplanes.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_bitplanes.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-AA-006](findings/NM-AA-006.md) | BPLCON1 resets to $3300, where the AA bit table marks bits 15-8 `=0` | a test fails against the page |

## [`src/minimig-aga/denise_colortable.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_colortable.v) — 2

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-016](findings/NM-HRM-016.md) | SuperHires sprite colour encoding is not implemented | a test fails against the page; the implementations split |
| [NM-HRM-017](findings/NM-HRM-017.md) | SuperHires bitplane colour encoding is not implemented | a test fails against the page; the implementations split |

## [`src/minimig-aga/denise_sprites.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_sprites.v) — 3

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-016](findings/NM-HRM-016.md) | SuperHires sprite colour encoding is not implemented | a test fails against the page; the implementations split |
| [NM-HRM-019](findings/NM-HRM-019.md) | SPRxCTL bit 4 (SHSH1) is not implemented | a test fails against the page |
| [NM-HRM-023](findings/NM-HRM-023.md) | the ATTACH bit is honoured in the even sprite, not only the odd | a test fails against the page |

## [`src/minimig-aga/denise_sprites_shifter.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_sprites_shifter.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-AA-011](findings/NM-AA-011.md) | SPRxCTL bits 4 and 3 (SH1, SH0) do not move a sprite: its start keeps the 140 ns step, where AA p. 28 gives them 70 ns and 35 ns | a test fails against the page; the implementations split |

## [`src/minimig-aga/gary.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/gary.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-075](findings/NM-HRM-075.md) | the CIAs answer only at $BF0000–$BFFFFF, where p. 353 selects them on the upper three address bits | a test fails against the page; the implementations split |

## [`src/misc/hid.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/misc/hid.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-051](findings/NM-HRM-051.md) | Caps Lock is delivered as an ordinary key | a test fails against the page |

## [`src/minimig-aga/minimig.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/minimig.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-035](findings/NM-HRM-035.md) | any write to CIA-A clears the kickstart overlay, not only a write to the OVL bit | a test fails against the page |

## [`src/minimig-aga/minimig_m68k_bridge.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/minimig_m68k_bridge.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-069](findings/NM-HRM-069.md) | the 68000 is given chip-bus slots of both parities, where p. 196 allows it only the even ones | a test fails against the page |

## [`src/minimig-aga/paula.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-028](findings/NM-HRM-028.md) | ADKCON's audio attach bits are stored and reach nothing | a test fails against the page |

## [`src/minimig-aga/paula_audio_channel.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_audio_channel.v) — 3

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-028](findings/NM-HRM-028.md) | ADKCON's audio attach bits are stored and reach nothing | a test fails against the page |
| [NM-HRM-084](findings/NM-HRM-084.md) | the length counter also counts the first data word after audio DMA starts: the first pass is one word short of AUDxLEN, and at AUDxLEN = 3 runs two words past the data set | a test fails against the page |
| [NM-HRM-092](findings/NM-HRM-092.md) | below the minimum period a pass of audio DMA does not always start again at AUDxLC: channel 0 reads past its table, fetches the first word twice or ends a pass one word short, where pp. 139 and 147 restart it each time | a test fails against the page; the implementations split |

## [`src/minimig-aga/paula_audio_mixer.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_audio_mixer.v) — 1

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-025](findings/NM-HRM-025.md) | volume 64 and volume 63 are the same gain on the multiply path | a test fails against the page |

## [`src/minimig-aga/paula_floppy.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_floppy.v) — 6

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-041](findings/NM-HRM-041.md) | DSKBYTR is a constant word in three of its five fields | a test fails against the page |
| [NM-HRM-046](findings/NM-HRM-046.md) | the drive activity light follows DMA, not the motor | a test fails against the page |
| [NM-HRM-047](findings/NM-HRM-047.md) | one step-spacing threshold where the page prints two | a test fails against the page |
| [NM-HRM-048](findings/NM-HRM-048.md) | a documented hardware bug that does not occur here | a test fails because NanoMig lacks a documented bug |
| [NM-HRM-049](findings/NM-HRM-049.md) | the MFM encoder is context-free | a test fails against the page |
| [NM-HRM-080](findings/NM-HRM-080.md) | with the motor on, RDY* is asserted for a drive with no disk in it, where pp. 334 and 368 make it mean a disk installed and up to speed | a test fails against the page |

## [`src/minimig-aga/paula_uart.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_uart.v) — 3

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-055](findings/NM-HRM-055.md) | SERDAT written with all zeros still transmits | a test fails against the page |
| [NM-HRM-057](findings/NM-HRM-057.md) | SERDATR bit 11 is a registered copy of RXD, not a direct read | a test fails against the page; the implementations split |
| [NM-HRM-072](findings/NM-HRM-072.md) | a stop bit with a word waiting behind it lasts N+1½ colour clocks, where p. 255 gives every transmitted bit N+1 | a test fails against the page |

## [`src/minimig-aga/userio.v`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/userio.v) — 2

| finding | subject | evidence |
|---|---|---|
| [NM-HRM-037](findings/NM-HRM-037.md) | the right controller port's X potentiometer input does not exist | a test fails against the page |
| [NM-HRM-038](findings/NM-HRM-038.md) | after POTGO START, the pot counters are not held in reset and the input lines are not dumped | a test fails against the page; the implementations split |

The marks and this list are derived from each page's header table by
`hrm-testpipeline/tools/write_rtl_review.py`, which lives in the separate
`hrm-testpipeline` repository, and are rewritten whenever a header changes.
