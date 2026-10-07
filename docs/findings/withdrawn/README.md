# Withdrawn findings

A withdrawn finding is the record of a claim that was raised, measured and then
**disproved**. It asks nothing of a reader, so it is kept here, apart from
[the register](../../findings.md) and [`rtl-review.md`](../../rtl-review.md),
which list the findings that do. It is kept at all because deleting it would
mean the next reader raises the claim again, and it keeps its number: a key is
issued once and never reused.

Fourteen entries are withdrawn. They answer two questions:

- **Would someone re-file this?** [NM-HRM-001](NM-HRM-001.md) reads as a finding
  until Table 7-5 is read, and Table 7-5 sanctions the measured value. Without
  the entry, the next reader of that page files it again.
- **What kind of mistake was it?** Thirteen of the fourteen pages name it in an
  `Error class` row; [NM-HRM-001](NM-HRM-001.md) says `none`. Six are one error in different forms, something the manual
  does not require read as a requirement: a silence
  ([NM-HRM-009](NM-HRM-009.md), [NM-HRM-034](NM-HRM-034.md),
  [NM-HRM-056](NM-HRM-056.md), [NM-HRM-079](NM-HRM-079.md)), a scope note
  ([NM-HRM-013](NM-HRM-013.md)) or a name ([NM-HRM-018](NM-HRM-018.md)). That
  is the most common way this suite produces a false finding. Three more graded
  a sentence or a legend without the figure, paragraph or appendix that
  completes it ([NM-HRM-027](NM-HRM-027.md), [NM-HRM-036](NM-HRM-036.md),
  [NM-HRM-053](NM-HRM-053.md)). One graded a proxy as the page's quantity: the
  disk DMA's reads of Chip memory timed as bit cells
  ([NM-HRM-070](NM-HRM-070.md)).

**NM-HRM-029 is the one worth the minute.** Its assertion tested *"dmasen is
true only when LENFIN=1"* by looking for a case where both held and failing
when none existed — but absence satisfies "only when". A one-way implication
had been tested as a biconditional, which is a trap somebody will walk into
again.

Each page has the header table [the findings README](../README.md) describes,
with `Lifecycle` reading `withdrawn`, and states the reason for the withdrawal
where a live page would suggest a change.

| Key | Subject | Status | Package | Verdict of the four |
|---|---|---|---|---|
| [NM-HRM-001](NM-HRM-001.md) | PAL field terminates at 313 lines, not 312 | **withdrawn** | `hrm/ch3-field-geometry` | **all four** run a 313-line PAL field with the long-frame bit set (WinUAE clears it at hard reset); Table 7-5 sanctions 313 |
| [NM-HRM-009](NM-HRM-009.md) | CLXDAT bit 15 reads as 1, not 0 | **withdrawn** | `hrm/ch3-collision-registers` | **all four** set bit 15 on every CLXDAT read |
| [NM-HRM-012](NM-HRM-012.md) | SPRxCTL bit 0 (SH0) honoured at every resolution | **withdrawn** | `hrm/ch3-sprite-registers` | all four take SH0 at every resolution, as NanoMig does |
| [NM-HRM-013](NM-HRM-013.md) | SPRxCTL bits 3 and 4 are read nowhere | **withdrawn** | `hrm/ch3-sprite-registers` | bit 4 is real from ECS on, bit 3 only on AGA -- the split this was withdrawn into |
| [NM-HRM-015](NM-HRM-015.md) | BRDNTRAN is declared and never connected | **withdrawn** | `hrm/ch3-horizontal-display-window` | **B3** for the first two; **none of the four** changes the border's colour: WinUAE and Copperline implement BRDNTRAN as genlock keying only, Copperline with a test; pp. 296, 297 and 307 place it among the genlock features |
| [NM-HRM-018](NM-HRM-018.md) | ECS BPLCON0 bit 4 (SPRHWRM) has no effect | **withdrawn** | `hrm/ch3-bplcon0-display-modes` | neither Denise nor Copperline gives bit 4 an effect on ECS |
| [NM-HRM-027](NM-HRM-027.md) | in direct mode the interrupt is raised before a word is output | **withdrawn** | `hrm/ch5-audio-direct-output` | **all four** raise it at the write, as Figure 5-8 draws, and again at every word transition |
| [NM-HRM-029](NM-HRM-029.md) | Figure 5-8's `dmasen` note does not hold outside its own exception | **withdrawn** | `hrm/ch5-audio-channel-state` | not comparable — `dmasen` appears at no pin |
| [NM-HRM-034](NM-HRM-034.md) | VPOSR reports the video standard in bit 12 of an OCS configuration | **withdrawn** | `hrm/ch7-beam-position-word-layout` | **2-2**: WinUAE and Denise report $10 for an OCS NTSC Agnus, as NanoMig does; p.299 itself gives 10 for the 8361 and 8370 |
| [NM-HRM-036](NM-HRM-036.md) | chip memory banks 1-3 stay reachable while the overlay is active | **withdrawn** | `hrm/ch7-reset-overlay` | **3-1**: vAmiga, Denise and Copperline replace only the ROM's 512K, as NanoMig does; p.223 itself allows part of the RAM to respond |
| [NM-HRM-053](NM-HRM-053.md) | Appendix B's `*` mark has no ECS qualifier, and nothing honours it under ECS | **withdrawn** | `hrm/ch8-appb-copper-access-marks` | **all four** keep the Copper out of `$000`–`$03E` on OCS and let it reach every register with ECS and CDANG set, as NanoMig does; p. 310 itself gives the ECS Copper all of the chip registers once CDANG is set |
| [NM-HRM-056](NM-HRM-056.md) | SERDATR's TBE bit cannot be observed clear from outside | **withdrawn** | `hrm/ch8-serdatr-read-word` | **all four keep TBE at 0 while a second word waits**, as NanoMig does; after a write from idle vAmiga, Denise and Copperline set it again at once, and WinUAE holds it clear for one `CYCLE_UNIT` in its `ser_accurate` mode; p. 258 moves the word into the shift register at the start of the operation |
| [NM-HRM-070](NM-HRM-070.md) | disk write DMA takes a word in every disk slot whatever FAST says | **withdrawn** | `integration/ch5-audio-output-scanrate`, `integration/ch8-disk-serial` | **all four** shift a disk write out at a fixed cell of about 2 µs and take a word from memory only when their FIFO has room; none lets ADKCON bit 8 reach the write path |
| [NM-HRM-079](NM-HRM-079.md) | a write to the TOD counter's middle byte leaves the clock running; p. 344 stops it on a write to "the register" and does not say which | **withdrawn** | `hrm/appF-cia-tod-clock-and-alarm` | **all four** stop the TOD clock on a write to $A alone, restart it on a write to $8 and leave it running across a write to $9, as NanoMig does and as the 6502.org dissection of the 8520R4 wires its count enable; p. 344 names no register for the stop, and its next sentence restarts the clock after the $8 write the withdrawn claim requires to leave it stopped |
