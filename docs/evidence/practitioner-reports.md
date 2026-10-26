# Practitioner reports

What people who work with the real chips and with emulators of them have written down about the behaviour this suite tests. They are **witnesses, like the emulators, never an oracle**: the printed Commodore documents stay the oracle, a post never changes a verdict, a lifecycle, a *Filed against* or an *Evidence* row, and it is never counted among the four implementations. One post is one person's report, usually of one chip revision, and most say how they know only in passing. A finding page takes a post in only where it contradicts something the page asserts, supports or explains what the page asks; everything else stays here.

## The thread

| | |
|---|---|
| Title | "Undocumented Amiga hardware stuff" |
| Where | English Amiga Board, [showthread.php?t=19676](https://eab.abime.net/showthread.php?t=19676); a post's permalink is `https://eab.abime.net/showpost.php?p=<id>` |
| Span | 2005 to 2026, 376 posts |
| Retrieved | 2026-10-25 |
| Read by | two independent readers, each producing a list of the hardware statements in the thread and where each touches a finding or a test package; the two lists were then merged and compared, and every quotation on a page was re-checked against its post |
| Text | private; the posts are other people's writing and are not in either repository. Pages quote at most two lines, with the permalink |

## How a statement relates to a page

| category | meaning | where it appears |
|---|---|---|
| CONTRADICTS | the post contradicts something the page itself asserts (its verdict, its reading of the silicon, its decision). A post that contradicts only the printed text the page quotes is not this | on the page, with one sentence under *Risks and unknowns* |
| SUPPORTS | the post says real hardware behaves as the page's requirement or decision says | on the page |
| EXPLAINS | the post gives the mechanism behind what an implementation does, or behind what the page records | on the page |
| EXTENDS | the post is about the same subject but speaks to something the page does not ask (an edge, another chip, another register bit) | this document only |
| TESTED-ELSEWHERE | a package assertion, not a finding page, already grades the statement | this document only |

The hardware basis in an entry comes from the post alone; where the post names none, the entry says so.

## Page entries

24 entries on 19 pages: CONTRADICTS 1, SUPPORTS 16, EXPLAINS 7. Each page holds them in a subsection *What practitioners report* at the end of *Independent evidence*.

| page | category | post |
|---|---|---|
| [NM-HRM-023](../findings/NM-HRM-023.md) | CONTRADICTS | [208156](https://eab.abime.net/showpost.php?p=208156) |
| [NM-HRM-066](../findings/NM-HRM-066.md) | SUPPORTS, EXPLAINS | [935951](https://eab.abime.net/showpost.php?p=935951), [935172](https://eab.abime.net/showpost.php?p=935172) |
| [NM-HRM-019](../findings/NM-HRM-019.md) | SUPPORTS, EXPLAINS | [1503541](https://eab.abime.net/showpost.php?p=1503541) |
| [NM-HRM-008](../findings/NM-HRM-008.md) | SUPPORTS | [208517](https://eab.abime.net/showpost.php?p=208517) |
| [NM-HRM-010](../findings/NM-HRM-010.md) | SUPPORTS, SUPPORTS | [1718467](https://eab.abime.net/showpost.php?p=1718467), [601811](https://eab.abime.net/showpost.php?p=601811) |
| [NM-HRM-068](../findings/NM-HRM-068.md) | SUPPORTS, SUPPORTS | [600609](https://eab.abime.net/showpost.php?p=600609), [1718467](https://eab.abime.net/showpost.php?p=1718467) |
| [NM-HRM-011](../findings/NM-HRM-011.md) | SUPPORTS, SUPPORTS | [1677675](https://eab.abime.net/showpost.php?p=1677675), [628336](https://eab.abime.net/showpost.php?p=628336) |
| [NM-HRM-005](../findings/NM-HRM-005.md) | EXPLAINS | [1787593](https://eab.abime.net/showpost.php?p=1787593) |
| [NM-HRM-089](../findings/NM-HRM-089.md) | EXPLAINS | [1787593](https://eab.abime.net/showpost.php?p=1787593) |
| [NM-HRM-073](../findings/NM-HRM-073.md) | EXPLAINS | [1787593](https://eab.abime.net/showpost.php?p=1787593) |
| [NM-HRM-075](../findings/NM-HRM-075.md) | SUPPORTS | [1085395](https://eab.abime.net/showpost.php?p=1085395) |
| [NM-HRM-069](../findings/NM-HRM-069.md) | SUPPORTS | [1795712](https://eab.abime.net/showpost.php?p=1795712) |
| [NM-HRM-091](../findings/NM-HRM-091.md) | EXPLAINS | [579567](https://eab.abime.net/showpost.php?p=579567) |
| [NM-HRM-032](../findings/NM-HRM-032.md) | SUPPORTS, in part | [1521516](https://eab.abime.net/showpost.php?p=1521516) |
| [NM-HRM-048](../findings/NM-HRM-048.md) | EXPLAINS | [1657241](https://eab.abime.net/showpost.php?p=1657241) |
| [NM-AA-007](../findings/NM-AA-007.md) | SUPPORTS | [1070412](https://eab.abime.net/showpost.php?p=1070412) |
| [NM-AA-013](../findings/NM-AA-013.md) | SUPPORTS | [1537859](https://eab.abime.net/showpost.php?p=1537859) |
| [NM-HRM-009](../findings/withdrawn/NM-HRM-009.md) (withdrawn) | SUPPORTS | [208517](https://eab.abime.net/showpost.php?p=208517) |
| [NM-HRM-079](../findings/withdrawn/NM-HRM-079.md) (withdrawn) | SUPPORTS | [1468737](https://eab.abime.net/showpost.php?p=1468737) |

Three entries are inferred in part, and say which step: NM-HRM-011 (the end of the blank at the next STRHOR), NM-HRM-089 and NM-HRM-073 (what the post's state machine implies for a case it does not mention). NM-HRM-091 is borderline and kept only as EXPLAINS: no post names BLTPRI.

## Merged statement list

217 statements, P-001 to P-217, in thread order: the union of the two readers' lists, each statement once, a later correction replacing the earlier wording and named in the subject. Flags: `hw` a statement about chip-set or system behaviour (185), `hyp` the author calls it a theory (14), `emu` about an emulator or core and not about hardware (7), `oos?` real hardware outside the chip-set suite, scope disputed between the readers: Gayle PCMCIA, CD32 and Akiko, pixel clock and genlock, monitor (11). Quotations are not repeated here; the post is the source.

| id | subject | posts | flag |
|---|---|---|---|
| P-001 | Copper / COPCON, dangerous-register stop | 206242 | hw |
| P-002 | Copper / SKIP | 206242 | hw |
| P-003 | Copper / skipped dangerous MOVE | 206242 | hw |
| P-004 | Copper / DMA disable and both COPJMP strobes | 206242 | hw |
| P-005 | Bitplane DMA / high DDFSTOP | 206264 | hw |
| P-006 | Bitplane DMA / illegal DDFSTOP | 206264 | hw |
| P-007 | Bitplane scroll / non-aligned DDFSTRT | 206264 | hw |
| P-008 | Playfield priority / five- or six-plane non-AGA mode (later completed) | 206345, 1213908 | hw |
| P-009 | Blitter line mode / disabled D channel | 206412 | hw |
| P-010 | Blitter line mode / required C channel | 206412, 591526 | hw |
| P-011 | Blitter line mode / disabled A channel | 206412 | hw |
| P-012 | Blitter line mode / modulo source | 206412 | hw |
| P-013 | Blitter line mode / first destination pixel | 206412, 223373 | hw |
| P-014 | Attached sprites / attach-bit side | 208156 | hw |
| P-015 | Collision data / CLXDAT bit 15 | 208517 | hw |
| P-016 | Collision logic / single versus dual playfield | 208517 | hw |
| P-017 | Blitter line mode / terminal destination pointer | 223373 | hw |
| P-018 | Bitplane DMA re-enable inside fetch window, OCS | 223376 | hw |
| P-019 | Bitplane DMA re-enable, ECS/AGA after disable outside window | 223376 | hw |
| P-020 | Bitplane DMA re-enable, ECS/AGA after disable inside window | 223376 | hw |
| P-021 | Blitter line mode / BLTALWM | 375341 | hw |
| P-022 | Blitter line mode / height semantics (hypothesis) | 375341 | hyp |
| P-023 | AGA sprites / FMODE.SSCAN2 | 401996 | hw |
| P-024 | Denise display activation / first bitplane-data write | 414480, 412931, 469500, 641095, 641187, 668846 | hw |
| P-025 | OCS bitplane DMA / DDFSTRT < $18 | 461682 | hw |
| P-026 | OCS bitplane DMA / refresh-slot conflict | 461682 | hw |
| P-027 | OCS bitplane state machine (hypothesis) | 461682 | hyp |
| P-028 | OCS/ECS seven-plane setting / Agnus sequencer | 482689, 488062 | hw |
| P-029 | OCS/ECS seven-plane setting / Denise output | 482689 | hw |
| P-030 | OCS/ECS seven-plane setting / static data registers | 482689 | hw |
| P-031 | A1000 custom-register mirror / system address decode | 491545 | hw |
| P-032 | Blitter scheduling / every cycle arbitrates | 579567 | hw |
| P-033 | Blitter fill-mode extra cycle | 579567, 591219 | hw |
| P-034 | Blitter / changing enables or fill during an active blit | 579567, 591526 | hw |
| P-035 | Blitter startup / idle cycles | 579567, 591219 | hw |
| P-036 | Copper versus CPU custom-register writes / delayed effects (later qualified) | 579567, 895424, 895928 | hw |
| P-037 | Blitter data paths / source latency (hypothesis) | 591066, 591219 | hyp |
| P-038 | Blitter A slot / disabled source (hypothesis) | 591066 | hyp |
| P-039 | Blitter cycle-cost explanation (hypothesis) | 591066 | hyp |
| P-040 | Minimig blitter / missing fill idle cycle | 591811 | emu |
| P-041 | PAL Copper / unusable pre-refresh cycle | 598432, 600609, 600878 | hw |
| P-042 | PAL Copper blocked cycle / bus signals | 600609 | hw |
| P-043 | Copper / ordinary COPJMPx cycle sequence (later correction) | 600609, 832397 | hw |
| P-044 | Copper / ordinary COPJMPx special second cycle | 600609, 832397, 832458 | hw |
| P-045 | PAL Copper / COPJMPx exception to the blocked cycle | 600609 | hw |
| P-046 | Copper blocked cycle / software and blitter consequence | 600878 | hw |
| P-047 | NTSC long lines / no unusable Copper cycle | 601811 | hw |
| P-048 | Right-border colour-write cutoff | 604321, 604361, 604658 | hw |
| P-049 | Denise / normal horizontal-counter range | 627136, 627177 | hw |
| P-050 | Agnus beam readout / refresh-slot holes | 627136 | hw |
| P-051 | Agnus beam counter / line rollover and Copper waits | 627136 | hw |
| P-052 | Denise / vertical display-window strobes | 627136, 627154 | hw |
| P-053 | Denise versus Agnus horizontal counters (partly hypothesis) | 627136 | hyp |
| P-054 | LISAID high byte / board wiring and model values (later corrected) | 627136, 627154, 627177 | hw |
| P-055 | LISAID high bits / OS AGA enable | 627136 | hw |
| P-056 | Denise horizontal display-window comparator limits (later correction) | 627835, 628336 | hw |
| P-057 | OCS Denise / PAL early-frame display-window anomaly | 627835, 628336, 628349 | hw |
| P-058 | OCS Denise / PAL early-frame terminal count | 627835, 628336 | hw |
| P-059 | PAL: STREQU lines 0-8, STRVBL lines 9-25 | 628336 | hw |
| P-060 | OCS Denise / NTSC early-frame terminal count | 628465, 628474 | hw |
| P-061 | A1000 DIP Agnus / vertical-blank interrupt delay | 641096 | hw |
| P-062 | Agnus-to-Paula interrupt signalling | 641096, 641187 | hw |
| P-063 | AGA colour selection / BPLCON4 delay | 669147 | hw |
| P-064 | Chip RAM / mirroring | 692852 | hw |
| P-065 | ECS Agnus / split 512 KiB + 512 KiB mapping | 692852, 695619, 695652 | hw |
| P-066 | WinUAE / split-bank ECS aliasing | 695630, 695656, 695661, 695666 | emu |
| P-067 | Blitter line-mode bus sequence | 698703 | hw |
| P-068 | Blitter line mode / A enable without A DMA | 698703 | hw |
| P-069 | WinUAE/Minimig line-mode emulation | 698703 | emu |
| P-070 | Blitter line-mode internal pipeline (hypothesis) | 698887 | hyp |
| P-071 | Blitter line mode / first empty cycle and shared ALU | 702496, 702591 | hw |
| P-072 | Blitter line mode / empty-cycle arbitration | 702569, 702591 | hw |
| P-073 | ECS Denise / BPLCON2.KILLEHB | 736599 | hw |
| P-074 | CIA timer / start latency | 741695 | hw |
| P-075 | CIA timer / terminal interrupt delay | 741695 | hw |
| P-076 | Paula serial transmit / full-width SERDAT | 751277 | hw |
| P-077 | Paula disk write DMA / WORDSYNC | 779822 | hw |
| P-078 | Gayle PCMCIA card reset / model-dependent methods | 782840 | oos? |
| P-079 | A600 Gayle PCMCIA reset / independent state | 782840 | oos? |
| P-080 | Denise NTSC sprite trigger / half-cycle sawtooth (hypothesis) | 830253 | hyp |
| P-081 | Agnus DMA address machinery / shared bus and adder | 832453 | hw |
| P-082 | Copper / waiting COPJMP2 cycle sequence | 832494 | hw |
| P-083 | Copper waiting-jump / blitter-pointer corruption and exact trigger (later completed) | 832494, 1356856 | hw |
| P-084 | Superhires / disabled display-fetch hard stops | 867717 | hw |
| P-085 | Programmable scan modes / disabled DMA limits | 867717 | hw |
| P-086 | ECS superhires / sprite interaction (documentation, partly unverified) | 867717, 867841 | hw |
| P-087 | Custom-register read holes / bus noise | 873546 | hw |
| P-088 | Custom-register bus noise / accumulating ones (hypothesis on cause) | 873546 | hyp |
| P-089 | OCS Agnus / BLTDDAT read exception | 873546 | hw |
| P-090 | WinUAE / custom-register bus noise | 873593 | emu |
| P-091 | Sprite exposed by first bitplane write / chipset-dependent lead (later completed) | 885306, 1524277 | hw |
| P-092 | CIA TOD input / debounce latency | 901357 | hw |
| P-093 | AGA FMODE / CPU writability (correction of a false report) | 915562, 915729, 915779 | hw |
| P-094 | AGA wide-fetch misalignment transformation (later solved) | 915729, 916139, 1154386, 1806431 | hw |
| P-095 | AGA wide bitplane fetch / scroll-start sprite cost | 917417 | hw |
| P-096 | Copper / post-refresh COPJMP shortcut | 928507 | hw |
| P-097 | AGA DMACONR.BBUSY under saturated display DMA (unconfirmed hypothesis) | 932588, 932796, 933126, 932827, 934698 | hyp |
| P-098 | Copper adjacent waits under heavy DMA (unconfirmed hypothesis) | 932588, 932796, 932827 | hyp |
| P-099 | OCS Agnus / sprite-DMA slot immediately before DDFSTRT | 934699, 935172, 935320 | hw |
| P-100 | OCS Agnus / stale second sprite word after mid-frame fetch-window change | 935646, 935944, 993760 | hw |
| P-101 | OCS sprite loss / disabled bitplanes still reserve slots | 935951 | hw |
| P-102 | Denise bitplane shifter / BPL1DAT latch | 939569 | hw |
| P-103 | Denise bitplane shifter / continuous delay comparison | 939569 | hw |
| P-104 | Denise bitplane shifting / display-mode independence | 967507 | hw |
| P-105 | OCS/ECS dual playfield / undefined PF1P or PF2P values (later completed) | 971685, 1213908 | hw |
| P-106 | AGA wide sprites / CPU/Copper SPRxDATx fill rules (final correction) | 986848, 1275034, 1805833 | hw |
| P-107 | Vertical blank / Paula strobe versus Agnus output signal (later correction) | 986921, 987075 | hw |
| P-108 | Vertical blank / Copper wins first Chip-bus access | 986921, 987312 | hw |
| P-109 | A500 video output / vertical-blank “no-show” interval | 986921 | oos? |
| P-110 | Beam counter/Copper during vertical blank | 986921 | hw |
| P-111 | AGA wide sprites / apparent CPU-versus-Copper difference explained later | 988528, 993760, 1275034 | hw |
| P-112 | AGA wide sprites / retained DMA-loaded data | 988528 | hw |
| P-113 | A1000 Agnus / final-field-line DMA | 993760 | hw |
| P-114 | A1000 Agnus / vertical display start zero | 993760 | hw |
| P-115 | sprite VSTOP comparison always active (Toni 2015; contested by P-204) | 1048304 | hw |
| P-116 | Sprite horizontal positions 1–4 / right-border visibility | 1048304 | hw |
| P-117 | Sprite DMA / top-of-frame inhibit and forced control fetch | 1048395 | hw |
| P-118 | Denise / disabled BPLxDAT registers | 1070311, 1070328 | hw |
| P-119 | AGA BPLCON0.BYPASS / HAM and EHB control bits | 1070412 | hw |
| P-120 | AGA BPLCON0.BYPASS / grayscale expansion | 1070412 | hw |
| P-121 | AGA BPLCON0.BYPASS / sprites | 1070412 | hw |
| P-122 | CD32 address bus / unmapped reads | 1085395 | oos? |
| P-123 | CIA select decode / Gary, Gayle, and Akiko | 1085395 | hw |
| P-124 | CIA address windows / Gary, Gayle, and Akiko | 1085395 | hw |
| P-125 | CD32 / custom-register mirror | 1085395 | oos? |
| P-126 | Akiko / register-space mapping and ID | 1085395 | oos? |
| P-127 | Akiko / write-only readback and writable masks | 1085395 | oos? |
| P-128 | Akiko / boot subchannel state | 1085395 | oos? |
| P-129 | Akiko internal CIAs / generated TOD clocks | 1085395 | oos? |
| P-130 | AGA 64-bit lores / aligned cross-line fetch overrun | 1108182 | hw |
| P-131 | Denise bitplane output / register-to-pixel latency | 1116055 | hw |
| P-132 | Agnus DMA pointer write / collision with same channel | 1116055 | hw |
| P-133 | Bitplane modulo / active-plane scope | 1116055 | hw |
| P-134 | Denise scroll-delay increase / inserted pixels | 1116091 | hw |
| P-135 | Sprite x-trigger before DMA data / stale and mixed line | 1134700 | hw |
| P-136 | Blitter B shifter / hidden old word retention | 1154372, 1215101 | hw |
| P-137 | Blitter A shifter / old word reset | 1154372, 1215101 | hw |
| P-138 | AGA sprites / width latched per data write and mid-sprite width change | 1275034 | hw |
| P-139 | AGA manual bitplane-data writes / wide-mode fill rules (final correction) | 1280214, 1805833 | hw |
| P-140 | AGA Lisa shifters / live FMODE reduction | 1280214, 1805833 | hw |
| P-141 | AGA manual data writes / source of “last bus data.” | 1280214 | hw |
| P-142 | AGA wide fetch / unaligned modulo carry loss (internal cause completed) | 1282840, 1806431 | hw |
| P-143 | Custom-register byte writes / 68000 and AGA bus semantics | 1369745 | hw |
| P-144 | TG68K / missing 68000 byte-write mirroring | 1369756 | emu |
| P-145 | OCS/ECS combined HAM+DPF / direct-colour path | 1425928, 1425939 | hw |
| P-146 | OCS/ECS combined HAM+DPF / component-modify path | 1425928, 1425939 | hw |
| P-147 | Blitter line mode / high word of BLTAPT | 1433709 | hw |
| P-148 | CIA TOD / normal read latch and write behavior | 1468737, 1469734 | hw |
| P-149 | CIA TOD read latch / ALARM-mode interaction (correcting post, wording ambiguous) | 1469734, 1468737 | hw |
| P-150 | CIA TOD / reset clears and stops the counters | 1469253, 1469734 | hw |
| P-151 | Minimig CIA reset/TOD discrepancy / system clock | 1470020 | emu |
| P-152 | Blitter line mode / B-channel pattern DMA | 1484460 | hw |
| P-153 | Blitter completion / final D-write race | 1485797 | hw |
| P-154 | ECS/AGA beam control / VPOSW.LOL | 1500097 | hw |
| P-155 | ECS Denise sprite hires-position bit outside superhires | 1503541 | hw |
| P-156 | ECS Agnus / undocumented DIWHIGH.V11 | 1504785 | hw |
| P-157 | OCS Denise / blanking source | 1521516 | hw |
| P-158 | ECS programmed horizontal blanking / normal signal path | 1521516 | hw |
| P-159 | ECS programmed horizontal blanking / alternate control combinations | 1521516 | hw |
| P-160 | AGA programmed blanking / Lisa-local implementation | 1521516 | hw |
| P-161 | Pixel clock versus programmable line geometry | 1521615, 1522891 | oos? |
| P-162 | External clock/genlock and whole-system timing | 1522198, 1522223 | oos? |
| P-163 | Simultaneous bitplane/sprite DMA conflict / register and pointer combination | 1531774 | hw |
| P-164 | Simultaneous bitplane/sprite DMA conflict / transfer and pointer writeback | 1531774 | hw |
| P-165 | Simultaneous bitplane/sprite DMA conflict / observable consequences | 1531774 | hw |
| P-166 | RGA custom-register select bus / active-low encoding | 1531932 | hw |
| P-167 | Agnus/Alice refresh / hidden full DMA pointer | 1537859 | hw |
| P-168 | REFPTR / internal refresh-pointer write mapping | 1537859 | hw |
| P-169 | Refresh slots / external DRAM cycle | 1537859 | hw |
| P-170 | Early OCS Denise / right display-window sprite mask | 1538877, 1540249 | hw |
| P-171 | Bitplane/refresh conflict / pointer arithmetic | 1542797 | hw |
| P-172 | Bitplane/refresh conflict / destination-register corruption | 1542797 | hw |
| P-173 | Bitplane/refresh conflict / missing strobe at Denise and Paula | 1542797 | hw |
| P-174 | Bitplane/refresh conflict / free-running Denise effects | 1542797 | hw |
| P-175 | CIA TOD / split-counter carry bug | 1557407, 1557422 | hw |
| P-176 | CIA TOD / four-E-clock update quantization | 1557422 | hw |
| P-177 | Sprite DMA enable / three-cycle allocation race | 1585760 | hw |
| P-178 | ECS/AGA horizontal window end / wraparound workaround | 1633893 | hw |
| P-179 | Display-window comparators / exact match and coincident start/stop | 1633902, 1633972 | hw |
| P-180 | VHPOSW / Denise horizontal-counter write | 1634542 | hw |
| P-181 | Paula disk DMA / premature terminal condition and lost last word | 1655087, 1657241 | hw |
| P-182 | Paula disk DMA / horizontal dependence of the last-word bug | 1655087, 1657241 | hw |
| P-183 | VHPOSW rewinding / sprite DMA and counter side effects | 1670680, 1670742 | hw |
| P-184 | Armed sprite / repeated horizontal comparator match | 1670735 | hw |
| P-185 | WinUAE / Denise-strobe emulation gap and fix | 1677384 | emu |
| P-186 | OCS Denise strobes / corrected master-blank state machine | 1677465, 1677814 | hw |
| P-187 | hardwired vblank usually lines 0-20 NTSC / 0-25 PAL | 1677675 | hw |
| P-188 | hardwired H-blank end: first pixel at Denise lores 92 ($5c), values confirmed | 1677675, 1680068 | hw |
| P-189 | OCS Denise strobes / line-zero and blank-edge side effects | 1677814 | hw |
| P-190 | Paula audio / pending INTREQ2 retrigger | 1679952, 1680088, 1773514 | hw |
| P-191 | ECS Denise strobe blank timing | 1682826, 1688233 | hw |
| P-192 | Paula vertical-blank interrupt / strobe-edge detector | 1682826 | hw |
| P-193 | Copper-wait/blitter conflict / pointer result correction | 1689265 | hw |
| P-194 | Keyboard MCU / reset-warning capability | 1709377 | hw |
| P-195 | A500/A600 keyboard reset / external immediate override | 1709377 | hw |
| P-196 | Custom registers / read-versus-write access type | 1709381 | hw |
| P-197 | A2000 keyboard reset / proposed CIA control (questioned hypothesis) | 1709388, 1709395 | hyp |
| P-198 | OCS Denise / colour-burst-time BPL1DAT inhibit | 1717220, 1717312, 1717330 | hw |
| P-199 | Denise burst pin / timing and BPLCON0.COLOR | 1717220 | hw |
| P-200 | NTSC STRLONG / alternating-line realignment | 1718467 | hw |
| P-201 | Denise/Lisa STRLONG output mux / switch artifacts | 1718467 | hw |
| P-202 | STRLONG latch / chipset-specific handoff position | 1718467 | hw |
| P-203 | STRLONG in programmed blanking and manual PAL use | 1718467 | hw |
| P-204 | Sprite DMA / vertical-stop loading (later contradictory correction) | 1741152 | hw |
| P-205 | AGA simultaneous HIRES+SHRES / apparent ultra-resolution (hypothesis) | 1764994 | hyp |
| P-206 | Copper / three-stage instruction pipeline | 1787593, 1787687 | hw |
| P-207 | Copper / COPJMP state reset and pending request | 1787593 | hw |
| P-208 | Copper / WAIT and SKIP stop/wakeup logic | 1787593 | hw |
| P-209 | Copper / back-to-back-even-cycle phantom DMA bug | 1787593, 1787687 | hw |
| P-210 | Copper / odd/even phase, enable, and allocation conditions | 1787593 | hw |
| P-211 | Blitter non-nasty mode / CPU grant after three CCKs | 1795712 | hw |
| P-212 | AGA seven-plane lores / non-nasty blitter starvation | 1795712 | hw |
| P-213 | Blitter busy / pending final D write and next-blit corruption | 1795712 | hw |
| P-214 | A1200/A4000/CD32 motherboard workaround / BLS | 1795712 | hw |
| P-215 | AGA Copper blitter-wait fix | 1795712 | hw |
| P-216 | OCS/ECS Copper-load analogue of blitter starvation (hypothesis) | 1799627 | hyp |
| P-217 | AGA Lisa / two chained 32-bit bitplane and sprite shifters | 1805833 | hw |

## Statements that extend a page

No assertion grades these, and no page asks for them. Each stays here.

| target | statements |
|---|---|
| NM-HRM-004 | P-001, P-003 |
| NM-HRM-016 | P-084, P-085, P-086 |
| NM-HRM-026, NM-HRM-084 | P-190 |
| NM-HRM-033 | P-154 |
| NM-HRM-055 | P-076 |
| NM-HRM-062 | P-123, P-124 (bit order ambiguous) |
| NM-HRM-066 (stale SPRxDATB) | P-100 |
| NM-HRM-010, NM-AA-005 (Denise delay) | P-201, P-202, P-203 |
| NM-HRM-032 (other roles) | P-157, P-158, P-160 |
| NM-HRM-073 | P-004, P-043, P-108 |
| NM-HRM-089 (vpos late, WAIT xx01 = xx03) | P-051 |
| NM-AA-013 (CBR) | P-169 |
| NM-AA-007 (HAM/EHB, sprites) | P-119, P-121 |
| NM-HRM-048 (A12 condition) | P-182 |
| NM-HRM-067 | P-187, P-188 |
| NM-HRM-001 (withdrawn) | P-189 |
| NM-HRM-013 (withdrawn) | folded into P-155 |

Package-level statements that no assertion grades: reader A lists 40, reader B 11 in groups; they agree on "not graded".

## Statements a package already grades

The verdicts are those of the two readers' run logs; nothing was run for this document.

| package · assertion | statements | verdict | relation |
|---|---|---|---|
| hrm/ch4-attached-sprites · `ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE` | P-014 | red | the post contradicts the expectation for ECS |
| hrm/ch7-dmacon-register-bits · three one-plane reds | P-101 | red | supports the expectation |
| hrm/ch3-sprite-registers · `SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE`; integration/ch4-sprite-registers · `P1` | P-155 | red | supports (SuperHires only) |
| integration/aa-integ-s1 · `P6-II`, `P6-AGG` | P-120 | red | supports |
| integration/aa-integ-s7-rest · `P1`, `PR-2a` | P-167 | red | supports; the AGA bus output is unsaid |
| hrm/ch3-collision-registers; hrm/ch7-collision-playfield-mode | P-016 | red | direction only, no gate |
| hrm/ch3-beam-counters-and-blanking NTSC pair; integration/ch2-interlace-fields, ch2-copper-beam, aa-integ-s2 | P-200, P-047 | red | supports the alternation |
| hrm/ch3-beam-counters-and-blanking PAL pair | P-041 | red | the post sides with NanoMig's 227; the reds stay against p. 304 |
| hrm/ch7-vertb-vertical-blank-timing · `PAL-VBLANK-ENDS-BY-LINE-26`, `NTSC-VBLANK-ENDS-BY-LINE-21`, `VBL-INT-AT-LINE-0` | P-059, P-107 | green | consistent |
| hrm/appF-cia-tod-clock-and-alarm · `TOD-WRITE-*` | P-148, P-149 | green | consistent; the ALARM read is ungraded |
| hrm/appF-cia-address-decode · `CIA-AB-TWOBIT-RULE` green, `CIA-DECODE-DONTCARE` red | P-123, P-124 | | consistent |
| hrm/ch8-serdat-transmit · `FIG12-UPPER-ZERO` | P-076 | green | consistent |
| aa/aa-lisa-colour-registers · `LISAID-*` | P-054 | green | the values equal the post's |
| hrm/ch6-blitter-cycles · `LINE-MODE-EIGHT-TICKS-PER-PIXEL` | P-067 | green | consistent (reader A's row, unchecked) |
| hrm/ch2-copper-rejected-move · R2, R4, R5, R6 | P-001 | green | regression class, not the manual |
| hrm/ch6-blitter-line · `LINE-C-D-SEPARATE-REGISTERS-SAME-ROLE`; hrm/appDE-memory-map-decode · `HIGHIOREG-PANEL-SWEEP`; hrm/ch2-copper-rejected-move · R3 | P-012, P-013, P-031, P-206 | green | questioned: see below |
| hrm/ch1-chip-memory-banking · `A1000-512K-MIRROR`, `ECS-FATAGNUS-1MB-TWO-BANKS` | P-064 | green | supports, extends (reader A's row, unchecked) |

Three green assertions the readers questioned. The question is whether the printed page requires what the assertion checks; the posts do not decide it.

- `LINE-C-D-SEPARATE-REGISTERS-SAME-ROLE`, part (b): the assertion has D's own step follow BLTDMOD, programmed to a value different from BLTCMOD. Post [206412](https://eab.abime.net/showpost.php?p=206412) says BLTDMOD is not used in line mode and BLTCMOD serves both C and D; part (a) agrees with it. The test's cited pages ask only that both modulo registers be set to the width.
- `HIGHIOREG-PANEL-SWEEP`: the message gives the $D8 to $DD range as `sel_reg`. Post [491545](https://eab.abime.net/showpost.php?p=491545) says on an A1000 $D80000-$DDFFFF is a custom-chip mirror and on other models there are no custom chips. The cited claims are an A3000 map; the printed panel's text is what decides it.
- `R3` of `ch2-copper-rejected-move`: the test is labelled a regression and cites no page. Post [1787593](https://eab.abime.net/showpost.php?p=1787593) describes, from the Alice schematics, a second word requested before a dangerous MOVE is rejected; it contradicts only the behaviour the regression protects.

The readers also called `PAL-VBLANK-DURATION-25-LINES` and `NTSC-VBLANK-DURATION-20-LINES` contradicted, and the BLTPRI reds `P7-ON`, `P7` and `A4`. Neither holds: "usually" in post [1677675](https://eab.abime.net/showpost.php?p=1677675) and the strobe lines of [628336](https://eab.abime.net/showpost.php?p=628336) read as 26 and 21 lines when counted inclusively, and no post names BLTPRI.

## Not covered by any finding or assertion

Both readers' top-ten lists merged by rank points (11 minus rank, summed). "Not covered" means no finding or assertion grades the exact edge, although NanoMig may implement it.

| # | item | statements | points A + B |
|---|---|---|---|
| 1 | Hidden BPLCON1 offset from a non-aligned DDFSTRT | P-007, P-102, P-103 | 10 + 10 |
| 2 | Audio `intreq2` kept past state 000 | P-190 | 9 + 9 |
| 3 | First BPL1DAT write enables sprites | P-024 | 2 + 8 |
| 4 | BPLCON2 priority codes 5 to 7 | P-008, P-105 | 8 + 0 |
| 5 | Copper SKIP corner cases | P-002, P-003 | 7 + 0 |
| 6 | BPLCON4 one-pixel delay | P-063 | 0 + 7 |
| 7 | Blocked Copper cycle before the refresh slots | P-041 to P-047 | 6 + 0 |
| 8 | Line mode with USEB | P-152 | 1 + 5 |
| 9 | Phantom Copper DMA at cycle 226 to 0 | P-209 | 0 + 6 |
| 10 | BPLEN on mid-line | P-018 to P-020 | 5 + 0 |
| 11 | OCS Denise counter free-running | P-049 to P-052 | 4 + 0 |
| 12 | Disk write DMA with WORDSYNC | P-077 | 0 + 4 |
| 13 | BPU = 7 | P-028 to P-030, P-118 | 3 + 0 |
| 14 | Line pointer BLTDPT/BLTCPT | P-013, P-017 | 0 + 3 |
| 15 | SSCAN2 | P-023 | 0 + 2 |
| 16 | BPL1DAT inhibit at burst | P-198 | 0 + 1 |

## Out of scope

Statements about the 68020/030, Gayle PCMCIA, CD32 and Akiko, monitors and genlock, the keyboard controller, floppy mechanics, software technique and emulator configuration are not about the chip set this suite tests.

- Reader A counts 28: 68020/030 flags 1, PAUSE pin 1, Gayle PCMCIA 4, CD32/Akiko 9, monitor/TV 3, genlock/pixel clock 4, keyboard controller 3, floppy mechanism 1, emulator configuration 2.
- Reader B counts 26: teletext/genlock 7, monitor/TV 5, software/OS only 5, programming technique 4, CPU only 2, floppy 2, instrumentation 1.
- Neither gives a per-statement list, so the union cannot be counted; taking the larger count per subject gives about 44 as an upper bound.
- The Gayle PCMCIA and CD32/Akiko statements are out of scope for reader A and in scope for reader B; they are the 11 `oos?` rows above.
