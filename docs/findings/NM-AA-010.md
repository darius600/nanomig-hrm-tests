# NM-AA-010 — the UHRES pointers never come out: with BPLCON0 bit 7 and DMAEN set, no BPLHPT or SPRHPT access reaches Chip memory, where AA pp. 15 and 27 give them the first three free cycles of each line [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Should `agnus.v` give the UHRES chain of AA pp. 15 and 27 a source on the chip bus — with BPLCON0 bit 7 and DMACON DMAEN set, the sprite pointer, the bit-plane pointer and the sprite pointer again in the first three free cycles after each line start, on the lines the start and stop registers name — where NanoMig makes no access at either pointer on any graded line? Closes when the chain is added and `P12-UHRES` of `aa-integ-s2` and `PR-U6`, `PR-U7`, `P3-E1`, `P3-E6`, `P3-AGG`, `P4`, `P5`, `P6-E1`, `P6-CTRL`, `P6-E6`, `P6-AGG`, `P7`, `PR-9-U5` and `P9-D0` of `aa-integ-s7-rest` are green on both cores, or when an Amiga specialist rejects the chain: if no chip took these cycles, the page is refiled against the printed page; if "(hires chips only)" excludes Alice, the finding moves to NanoMig's ECS configuration (*Risks and unknowns*). |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | the UHRES chain with BPLCON0 = $0080 (UHRES alone) and DMACON = $8320 (DMAEN, BPLEN, SPREN). In `aa-integ-s2`, DUAL clear: the bit-plane pointer at $180000 and the sprite pointer at $190000, BPLHSTRT and SPRHSTRT 100, BPLHSTOP and SPRHSTOP 110, BPLHMOD 0, the eight ordinary sprites on a null sprite and the CPU stopped, accesses counted on lines 102 to 108. In `aa-integ-s7-rest`, DUAL clear in runs U1 to U4, UW and D0 and set in D1: BPLHSTRT/BPLHSTOP $050/$070 and SPRHSTRT/SPRHSTOP $058/$068 in U1 to U3 (U4 $0F8/$108 and $100/$110, across line $100; UW $050/$07A for both), BPLHMOD $0010 in U1 and $FFF0 in U2, whose pointer values are U1's complemented, and in UW 40 walking-one and walking-zero cases of each pointer, one per line; every ordinary pointer at a zero block, and every port access classified as refresh, CPU, zero-block or UHRES access. AA chip set selected (`chipset_config` $10); on fx68k and TG68K; chip revision unknown; hardware not measured. Not graded: more than one access per line under DUAL (`P9-E1a` and `P9-E1b` print RECORDED), the WRM bits and the RGA identifiers |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF pp. 15, 16, 22, 27 and 28 (the BPLHPTH/BPLHPTL, BPLHSTRT, BPLHSTOP, BPLHMOD, BPLCON0, DMACON, SPRHDAT, SPRHPTH/SPRHPTL, SPRHSTRT and SPRHSTOP entries), pp. 6, 7 and 10 (the address list, both legends and p. 6's `+` pair), p. 12 (DUAL), p. 29 (the reading of "(hires chips only)"). The document prints no page numbers: p. N is the PDF page. *Amiga Hardware Reference Manual*, 3rd ed., printed p. 298 (the ECS register list) and p. 293 (Appendix B) · <details><summary>claim ids by assertion (61)</summary> `aa-integ-s7-rest`: `P3-AGG`: `opus:AA-bplhpth.c1`, `opus:AA-bplhpth.c2`, `opus:AA-bplhpth.c4`, `opus:AA-bplhpth.c6`, `opus:AA-bplhptl.c1`, `opus:AA-bplhptl.c2`, `opus:AA-bplhptl.c4`, `opus:AA-bplhptl.c6`, `sol:AA-bplhpth.c1`, `sol:AA-bplhpth.c2`, `sol:AA-bplhpth.c4`, `sol:AA-bplhpth.c6`, `sol:AA-bplhptl.c1`, `sol:AA-bplhptl.c2`, `sol:AA-bplhptl.c4`, `sol:AA-bplhptl.c6`, p. 10; `P4`: `opus:AA-bplhmod.c1`, `opus:AA-bplhmod.c2`, `opus:AA-bplhmod.c5`, `sol:AA-bplhmod.c1`, `sol:AA-bplhmod.c2`, p. 10; `P5`: `opus:AA-bplhstrt.c1`, `opus:AA-bplhstrt.c2`, `opus:AA-bplhstrt.c5`, `opus:AA-bplhstop.c1`, `opus:AA-bplhstop.c2`, `opus:AA-bplhstop.c5`, `sol:AA-bplhstrt.c1`, `sol:AA-bplhstrt.c2`, `sol:AA-bplhstop.c1`, `sol:AA-bplhstop.c2`, p. 10; `P6-AGG`: `opus:AA-sprhpth.c1`, `opus:AA-sprhpth.c2`, `opus:AA-sprhpth.c4`, `opus:AA-sprhpth.c6`, `opus:AA-sprhptl.c1`, `opus:AA-sprhptl.c2`, `opus:AA-sprhptl.c4`, `opus:AA-sprhptl.c6`, `sol:AA-sprhpth.c1`, `sol:AA-sprhpth.c2`, `sol:AA-sprhpth.c4`, `sol:AA-sprhpth.c6`, `sol:AA-sprhptl.c1`, `sol:AA-sprhptl.c2`, `sol:AA-sprhptl.c4`, `sol:AA-sprhptl.c6`, p. 10; `P7`: `opus:AA-sprhstrt.c1`, `opus:AA-sprhstrt.c2`, `opus:AA-sprhstrt.c5`, `opus:AA-sprhstop.c1`, `opus:AA-sprhstop.c2`, `opus:AA-sprhstop.c5`, `sol:AA-sprhstrt.c1`, `sol:AA-sprhstrt.c2`, `sol:AA-sprhstop.c1`, `sol:AA-sprhstop.c2`, p. 10. `aa-integ-s2`: `P12` (not graded): `opus:AA-beamcon0.c31`, `opus:AA-beamcon0.c32`, `sol:AA-beamcon0.c48`, `sol:AA-beamcon0.c49`, p. 12. `PR-U6`, `PR-U7`, `P3-E1`, `P3-E6`, `P6-E1`, `P6-CTRL`, `P6-E6`, `PR-9-U5` and `P9-D0` of `aa-integ-s7-rest`, and `P12-UHRES`, `P12-UHRESEN`, `P12-F3`, `P12-BANDWIDTH`, `P12-0`, `P12-1-BPL`, `P12-1-SPR`, `P12-PHASE` and `P12-K` of `aa-integ-s2`, carry no claim id</details> |
| **Module** | `agnus.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L169-L267) (the chip-bus multiplexer, which has no UHRES source) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus.v#L137-L215); inherited (MiSTer has the same multiplexer and keeps Denise's `uhres` as a comment; TC64 has the same multiplexer, assigns `uhres` from BPLCON0 bit 7 where nothing reads it, as NanoMig does, and decodes $1D4 as a line-compare register for its RTG display, which a fix ported there would meet) |
| **Test** | [`integration/aa-integ-s2`](../../integration/aa-integ-s2): `make -k -C integration/aa-integ-s2 test`, or `test-fx68k` and `test-tg68k` of the same package; [`integration/aa-integ-s7-rest`](../../integration/aa-integ-s7-rest): `make -k -C integration/aa-integ-s7-rest test`, or `test-fx68k` and `test-tg68k`; an expected red exits nonzero. The whole design at `nanomig.v`'s ports, once with each CPU core |
| **Assertions** | In `aa-integ-s7-rest`, 14 red on fx68k and 14 on TG68K, in a package run of 36 passed, 21 failed, 11 NOT GRADED on each core, every one because no UHRES access occurs: `P3-E1`, expected exactly one BPLH access on every line $51 to $6F of U1, U2 and U3 → 0 of 31 lines; `P3-E6`, an access at the address of each of the 40 walking cases of the bit-plane pointer in UW → 40 of 40 missing; `P4`, one BPLH access per line in U1 and U2 and a step of BPLHMOD + 2 between consecutive lines, +18 and −14 → 0 BPLH accesses in either run; `P5`, the first BPLH access on line start or start+1 of U1 and U4 and one on every line up to stop−1 → none; `P6-E1`, two SPRH accesses on every line $59 to $67 of U1, U2 and U3 → none; `P6-CTRL`, one BPLH access and no SPRH access on lines $51 to $56 of U1 → no BPLH access; `P6-E6`, an access at the address of each of the sprite pointer's 40 walking cases → 40 of 40 missing; `P7`, two SPRH accesses on every line start+1 to stop−1 of U1 and U4 → none; `P9-D0`, one BPLH access on each of D0's 30 lines $52 to $6F → 0; `P3-AGG` and `P6-AGG`, the two pointers' aggregates, with them; and the preconditions `PR-U6` (a BPLH and an SPRH access in each of U1 to U3), `PR-U7` (two SPRH accesses and one BPLH access on every line $59 to $67) and `PR-9-U5` (two BPLH accesses in D1, DUAL set) → none. Green on the same run: `R2-XCHK-U`, `PR-U1` to `PR-U4`, `PR-9a` and the contrasts `P3-U0`, `P3-U0d` and `P6-U0`. `PR-U5`, `P3-E2`, `P3-E4`, `P3-E5`, `P6-E2` to `P6-E5` and `P9-E2` print NOT GRADED for the same absence, and `P9-E1a` and `P9-E1b`, more than one access per line with DUAL set, print RECORDED. That run's seven other reds are not this finding's: `P1` and `PR-2a` ([NM-AA-013](NM-AA-013.md)), and `P8-E1`, `P8-E2`, `P8-E3`, `P8-E4` and `P8-AGG` ([NM-AA-008](NM-AA-008.md)). In `aa-integ-s2`, 1 red on fx68k and 1 on TG68K, in a package run of 131 passed, 13 failed on each core: `P12-UHRES`, expected on every line 102 to 108 accesses in both pointer regions, the first three of the line in the order sprite pointer, bit-plane pointer, sprite pointer → 0/0 on every line. Green on the same run: `P12-UHRESEN` (BPLCON0 bit 7 and DMAEN are written set), `P12-F3` (the line anchor), `P12-BANDWIDTH` (nothing but refresh, UHRES and null-sprite accesses on those lines, 0 unexplained) and the fixtures. `P12-0` (one bit-plane and two sprite-pointer accesses per line with DUAL clear) prints NOT GRADED behind `P12-UHRES`; `P12-1-BPL`, `P12-1-SPR`, `P12-PHASE` and `P12`, the chain under DUAL, print NOT GRADED behind it and behind `P12-K`, the alternate counter of [NM-AA-008](NM-AA-008.md). The run's twelve other reds are not this finding's: `F5-SV-INV` ([NM-HRM-032](NM-HRM-032.md)), `P9-BV`, `P9-B` and `P9-ID` ([NM-AA-009](NM-AA-009.md)), `P11-HHRUN` and `P12-K` ([NM-AA-008](NM-AA-008.md)), `P13-0-LINES` and `P13-0-COUNT` ([NM-AA-005](NM-AA-005.md)), `P14-0-LINES` and `P14` ([NM-HRM-010](NM-HRM-010.md)), and the two light-pen lines ([NM-HRM-087](NM-HRM-087.md)) |
| **Verdict of the four** | **split, one against three**: WinUAE runs the chain — with BPLCON0 bit 7 and DMAEN set it gives the UHRES sprite pointer, the bit-plane pointer and the sprite pointer again three consecutive free slots after each line start, on the lines the start and stop registers name, with an ECS or an AGA Agnus and not with an OCS one. vAmiga, Denise and Copperline have no UHRES DMA and no UHRES pointer register; Denise lists "UHRES/DUAL stuff" among its todos and Copperline calls UHRES/DUAL "explicitly out of scope", so neither offers the absence as a reading of the page. Hardware: not measured |
| **Effort** | none; if the chain is accepted, L: a new source in the chip-bus multiplexer of `agnus.v` (lines 169-267), nine registers decoded in Agnus with BPLCON0 bit 7 kept there, two pointers with a modulo, and the choice of the three free cycles |
| **See also** | [NM-AA-008](NM-AA-008.md): with DUAL set, p. 12 restarts this chain at each reset of the alternate counter, and `P12` needs both; [NM-AA-002](NM-AA-002.md): SPRHDAT, the identifier the UHRES sprite cycle puts on the register bus, is the one register in the Coprocessor-restricted range the AA list leaves unmarked |

---

## Source requirement

The AA register list has nine registers for two DMA pointers whose data go to external logic rather than to the display: the UHRES bit-plane pointer and the UHRES sprite pointer. p. 7 lists the sprite pointer's identifier:

```
SPRHDAT  &h  078  W   A      ext logic UHRES sprite pointer & data identifier
```

and p. 27's SPRHDAT entry says what it marks:

```
                    This identifies the cycle when this pointer address is on the
                    bus accessing the memory.
```

**Page 15**, the bit-plane pointer, says when:

```
BPLHPTH  H 1EC  W   A        UHRES (VRAM) bit plane pntr (High 5 bits)
BPLHPTL  H 1EE  W   A        UHRES (VRAM) bit plane pntr (Low 15 bits)
                    When UHRES is enabled, this pointer comes out on the 2nd
                    'free' cycle after the start of each horiz. line. Its modulo
                    is added every time it comes out. 'Free' means priority
                    above the copper and below the fixed stuff (audio,sprites...)
                    BPLHDAT comes out as an identifier on the RGA lines when the
                    pointer address is valid so that external detectors can use
                    this to do the special cycle for the VRAMs. The SHRHDAT gets
                    the first and third free cycles.
```

and on which lines, BPLHSTRT and BPLHSTOP:

```
                    This controls the line when the data fetch starts for the
                    BPLHPTH,L pointers. V10-V0 on DB10-0.
```

```
                    This controls the line when the data fetch stops for the
                    BPLHPTH,L pointers.
```

and by how much it moves, BPLHMOD:

```
                    This is the number (sign extended) that is added to the
                    UHRES bit plane pointer (BPLHPTL,H) every line, and then
                    another 2 is added, just like the other modulos.
```

**Page 27**, the sprite pointer, takes the other two free cycles:

```
SPRHPTH  H 1E8  W   A        UHRES sprite pointer (High 5 bits)
SPRHPTL  H 1EA  W   A        UHRES sprite pointer (Low 15 bits)
                    This pointer is activated in the 1st & 3rd 'free' cycles
                    (see BPLHPTH,L) after horiz line start. It increments
                    for the next line.
```

and **page 28** bounds it in lines, `SPRHSTRT H 1D0  W   A        UHRES sprite vertical display start` and `SPRHSTOP H 1D2  W   A        UHRES sprite vertical display stop`, each with `v10` to `v0` in bits 10-0.

**Page 16** turns it on, BPLCON0 bit 7:

```
                    UHRES= ultrahi res enables the UHRES pointers (for 1k*1k)
                             (also needs bits in DMACON) (hires chips only)
```

and **page 22**, the DMACON table, names the bit:

```
        09    DMAEN       Enable all DMA below (also UHRES dma)
```

**Page 6**'s legend gives the two pointer pairs, marked `+` in the address list, their bits:

```
+=Address register pair.Low word uses DB1-DB15,High word DB0-DB4.
```

So with BPLCON0 bit 7 and DMAEN set, every line between BPLHSTRT and BPLHSTOP has one access at the bit-plane pointer and every line between SPRHSTRT and SPRHSTOP two at the sprite pointer, in the free cycles after the line starts: sprite, bit plane, sprite. Each address is DB0-DB4 of the pointer's high word above DB1-DB15 of its low word, and the bit-plane pointer moves by BPLHMOD plus 2 from line to line. "SHRHDAT" on p. 15 is not a register name anywhere; p. 27 gives the same two cycles to the sprite pointer independently.

### "(hires chips only)": how this page reads it

**This page reads p. 16's "(hires chips only)" as "added in the hires chips":** absent from the original chips, present in the hires chips — the 8372 "agnushr" of p. 29's identifier list — and in the chip set this document specifies, which extends them.

- **The nine registers are rows of this document's own register list, in chip A.** Its legend, p. 6:

  ```
  h=new for HiRes chip set
  p=new for IAA chip set
  A=Agnus/Alice chip, D=Denise/Lisa chip, P=Paula chip
  ```

  and its rows, p. 10:

  ```
  SPRHSTRT   h  1D0  W    A       UHRES sprite vertical start
  SPRHSTOP   h  1D2  W    A       UHRES sprite vertical stop
  BPLHSTRT   h  1D4  W    A       UHRES bit plane vertical start
  BPLHSTOP   h  1D6  W    A       UHRES bit plane vertical stop
  ...
  BPLHMOD    h  1E6  W    A       UHRES bit plane modulo
  SPRHPTH   +h  1E8  W    A       UHRES sprite pointer (High 5 bits)
  SPRHPTL   +h  1EA  W    A       UHRES sprite pointer (Low 15 bits)
  BPLHPTH   +h  1EC  W    A       VRam (UHRES) bit plane pointer (High 5 bits)
  BPLHPTL   +h  1EE  W    A       VRam (UHRES) bit plane pointer (Low 15 bits)
  ```

  `h` marks the chip set that added a row; the same list marks `HTOTAL`, `BEAMCON0`, `HSSTRT`, `VSSTRT`, `HCENTER` and `DIWHIGH` the same way, each of which NanoMig decodes.
- **The alphabetical list's legend says the same**, p. 10: `H= new register in hires chips` and `h= stuff added or changed in hires chips`. Eight of the nine entries carry `H` (pp. 15, 27, 28); BPLHSTOP carries `p` (p. 15), which the same legend defines as `p= stuff added or changedin hires chips` and p. 6 as `p=new for IAA chip set`.
- **The document uses "hires chips only" as a mark of origin elsewhere.** p. 29's VPOSR entry, whose bit row gives the register bits v10 and v9, closes with the same words:

  ```
                      USE   LOF I6 I5 I4 I3 I2 I1 I0,LOL -- -- -- --v10 v9 V8
  ...
                        8372(fat-hr) (agnushr),thru rev. 4      = 20 PAL,30 NTSC
                        8372(fat-hr) (agnushr),rev. 5           = 21 PAL,31 NTSC
                        8374(alice)                             = 22 PAL,32 NTSC
  ...
                      v9,v10-- hires chips only (20,30 identifiers)
  ```

  The parenthesis names the identifiers of the first hires Agnus only. Read as a list of the chips that have the bits, it would take them from the 8372's own revision 5 as well as from Alice; read as "added in the hires chips", it says where V9 and V10 came from, and Alice keeps them.
- **For the two bits that turn a UHRES cycle into a write, the document says it outright:** SPRHWRM, p. 28, `(ECS  and later chips only)`; BPLHWRM, p. 15, `(ECS and later versions)`.

**The other reading is "in the hires chips and not in Alice".** The *Hardware Reference Manual*, 3rd ed., is then the oracle for the ECS chips. Its ECS register list, printed p. 298, names UHRES only in BEAMCON0's function and lists none of the nine registers:

> Beam counter control register (SHRES,UHRES,PAL)

and its full register list, Appendix B, printed p. 293, prints the rows between VBSTOP and BEAMCON0, $1D0 to $1DA, as `Reserved`.

## RTL and measurement

**No module decodes any of the nine registers.** Agnus's beam counter names the registers from $1C0 on, and its list steps from VBSTOP at $1CE to HSSTRT at $1DE and BEAMCON0 at $1DC ([`agnus_beamcounter.v#L65-L76`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L65-L76)). `agnus.v` adds DIWHIGH at $1E4 ([`#L103-L107`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L103-L107)), and FMODE at $1FC and SCRDAT at $1F0 are the only other registers named between $1C0 and $1FE in [NanoMig's minimig-aga sources](https://github.com/harbaum/NanoMig/tree/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga); none of them names $1D0–$1D6 or $1E6–$1EE.

**BPLCON0 bit 7 reaches nothing.** Denise stores the whole register and assigns the bit to a wire that no line reads ([`denise.v#L166`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise.v#L166), [`#L188`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise.v#L188)):

```verilog
wire          uhres;      // ultra-hires enables the UHRES pointers; needs bits in DMACON also
...
assign uhres    = bplcon0[7];
```

**The chip bus has no UHRES source.** Agnus's multiplexer gives the bus, in priority order, to disk, refresh, audio, bit-plane, sprite, Copper and blitter DMA, and otherwise to the CPU ([`agnus.v#L169-L267`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L169-L267)). DMAEN gates Agnus's bit-plane, Copper, blitter and sprite enables ([`#L298-L302`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L298-L302)) and Paula's copy of it the disk and audio enables ([`paula.v#L179-L183`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula.v#L179-L183)), and nothing else, so the "(also UHRES dma)" of p. 22 has nothing to enable. None of this depends on the chip-set inputs: the multiplexer has no `aga` or `ecs` term.

[`integration/aa-integ-s2`](../../integration/aa-integ-s2)'s P12 writes the two pointers, BPLHMOD and the four start and stop registers, then BEAMCON0 = $0020, BPLCON0 = $0080 (the UHRES bit alone, no bit plane) and DMACON = $8320 (`test_aa_integ_s2.cpp:1884-1888`). In each frame the program points the eight ordinary sprites at a null sprite, waits for line 90, writes a marker and stops the CPU until the vertical blank (`prog_uhres.s`), so that on lines 90 to 120 the port shows only refresh, UHRES and null-sprite traffic. The test counts the accesses in the two 64 KB regions, $180000 and $190000, on every port line 102 to 108, the lines inside 100 to 110 with one line of margin at each end:

```
MEASURED: P12 measured, DUAL clear: BPLHPT/SPRHPT accesses on lines 102-108: 0/0 0/0 0/0 0/0 0/0 0/0 0/0
FAIL: P12-UHRES: with UHRES enabled and DUAL clear the UHRES pointer regions are accessed on every graded line, and the first three accesses of the line in them are the sprite pointer, the bit-plane pointer and the sprite pointer, in this order
PASS: P12-UHRESEN: the runs that grade the UHRES chain write BPLCON0 with bit 7 (UHRES) set and DMACON with SET and DMAEN set, with DUAL clear and with DUAL set
MEASURED: P12 measured, DUAL clear: accesses on lines 102-108 that refresh, UHRES and the null sprite do not explain = 0
PASS: P12-BANDWIDTH: on lines 102-108 the port shows only refresh, UHRES and null-sprite accesses: no bit plane is fetched, no other DMA runs and the CPU is stopped
MEASURED: P12 measured, DUAL set: BPLHPT/SPRHPT accesses on lines 102-108: 0/0 0/0 0/0 0/0 0/0 0/0 0/0
```

The same lines on fx68k and on TG68K. The refresh anchor `P12-F3` is green on both, so the lines are located; `P12-BANDWIDTH` says the free cycles p. 15 speaks of are there and nothing takes them. `P12-UHRES` is graded at `test_aa_integ_s2.cpp:2090`, `P12-UHRESEN` at `:2092` and `P12-BANDWIDTH` at `:2097`.

[`integration/aa-integ-s7-rest`](../../integration/aa-integ-s7-rest) writes the nine registers in the vertical blank of each run, then BPLCON0 = $0080 and DMACON = $8320, with every ordinary bit-plane and sprite pointer at a zero block (`prog_u.s`). It sorts every Chip-memory access at the port from the end of the setup on: refresh (the `refresh` output), CPU (program, results and stack), zero block, and what is left, a UHRES access: a BPLH access near the expected bit-plane addresses, an SPRH access in the 4 KB from the sprite pointer, any other a stray. Markers the program stores from VPOSR and VHPOSR in the middle of each line assign each access to its line. Nothing is left over in any run:

```
MEASURED: PR-U5 population: 0 UHRES accesses in U1, U2, U3, U4 and UW (fx68k)
MEASURED: U1: BPLH 0 SPRH 0 stray 0; lines with one BPLH access 0 of 31, with two SPRH accesses 0 of 15; first BPLH access on line none (BPLHSTRT $050), first SPRH access on line none (SPRHSTRT $058) (fx68k)
MEASURED: E3.6 (BPLH walking cases in UW): line $051: expected $000002, seen none; line $052: expected $000004, seen none; line $053: expected $000008, seen none; cases without their address: 40 of 40 (fx68k)
MEASURED: P4: U1 and U2 do not have exactly one BPLH access on every line $51..$6F; BPLH accesses U1 0, U2 0 (fx68k)
MEASURED: P9: D1 has 0 BPLH accesses between t($51) and t($6F); D0 has 0 (fx68k)
FAIL: P3-E1: in U1, U2 and U3 every line V with $51 <= V <= $6F, the lines after BPLHSTRT up to BPLHSTOP-1, has exactly one BPLH access (AA p.15 BPLHPTH,L: 'this pointer comes out on the 2nd free cycle after the start of each horiz. line')
PASS: P3-U0d: U0d, run as U1 with DMACON lacking DMAEN, has no BPLH and no SPRH access (AA p.22 '09 DMAEN Enable all DMA below (also UHRES dma)')
```

The same lines on TG68K, with `(tg68k)`; U2 to U4 print U1's line with their own ranges, and E6.6 counts 40 of 40 sprite-pointer cases without their address. The refresh classification (`R2-XCHK-U`) and the markers (`PR-U1` to `PR-U3`) are green on both cores, and `PR-U4` finds nothing but refresh, CPU and zero-block accesses in U0, whose UHRES bit is clear. `PR-U6` is graded at `s7r_p3.h:177`, `PR-U7` at `:178`, `P3-E1` at `:189-190`, `P3-E6` at `:210-211`, `P3-AGG` at `:212-213`, `P4` at `:238-240`, `P5` at `:246-248`, `P6-E1` at `:261`, `P6-CTRL` at `:268`, `P6-E6` at `:271`, `P6-AGG` at `:272-274`, `P7` at `:280-282`, `PR-9-U5` at `:350` and `P9-D0` at `:379-380`.

## Risks and unknowns

- **The reading of "(hires chips only)"** is the page's largest limit. Under the other reading, "in the hires chips and not in Alice", the nine rows in chip A, "Agnus/Alice", and DMAEN's `(also UHRES dma)` would make this document list a channel its own chip set lacks. The RTL has no chain in its ECS configuration either (the multiplexer has no chip-set term), but no package grades the ECS configuration, so that is a reading of the code. The *Hardware Reference Manual* does not give the ECS chips the nine registers (the scan's text gives the first and the last address of the $1D0 to $1DA rows as `IDO` and `IDA`), so this document would be the only printed description of the chain, and the finding would move to NanoMig's ECS configuration rather than go away.
- **Does an 8374 (Alice) take UHRES cycles?** Set BPLCON0 bit 7 and DMAEN, write SPRHPTH/SPRHPTL, BPLHPTH/BPLHPTL and the four start and stop registers in the vertical blank, and watch the Chip-memory address bus, or the register bus for $078 and $07A, in the first free cycles after a line starts. Accesses at the two pointers settle the reading this page takes; none on an Alice moves the finding to the printed page.
- **Did any Agnus take them?** If an 8372 takes them and an 8374 does not, "(hires chips only)" means "not Alice", and the finding belongs to NanoMig's ECS configuration. If neither does, pp. 15, 16, 27 and 28 describe a mode no chip had, and the *Hardware Reference Manual*'s `Reserved` rows on p. 293 are the more accurate statement.
- **Which DMACON bits does "(also needs bits in DMACON)" mean?** p. 22 names DMAEN and leaves bits 12 and 11 `X`; both packages set DMAEN with BPLEN and SPREN, and WinUAE enables the chain on DMAEN alone. NanoMig has nothing for any DMACON bit to enable.
- **On this RTL only the existence of the chain is observed.** `P12-UHRES` asks for accesses in both regions and the order sprite, bit plane, sprite at the start of each line. `aa-integ-s7-rest` grades the addresses bit by bit (`P3-E6`, `P6-E6`), BPLHMOD (`P4`) and the line limits (`P5`, `P7`), but each of these is red because no access exists at all, which says nothing yet about its own clause. The free cycle each access takes, the WRM bits and the RGA identifiers $078 and $07A are graded by neither package. `P12-0`, the counts per line, prints NOT GRADED behind `P12-UHRES`.
- **With DUAL set the chain is graded for its presence only.** `PR-9-U5` asks D1, whose DUAL bit is set, for two BPLH accesses; p. 12 starts the chain there "with the reset of this counter rather than the normal one", the counter [NM-AA-008](NM-AA-008.md) finds absent, so on that reading `PR-9-U5` needs both changes. `P9-E1a` and `P9-E1b`, more than one access per line, print RECORDED: p. 12 qualifies it "assuming there is some memory bandwidth left", and no precondition establishes that bandwidth.
- **Green lines that say nothing here:** `P3-U0`, `P3-U0d` and `P6-U0`, the contrasts with the UHRES bit or DMAEN clear, ask for no access in either range, which a design without the chain cannot fail.
- **Three of the four have no UHRES DMA either**, though two of them call that unfinished rather than right. TC64's comment, "never implemented on real hardware", is the one statement here about real hardware and it is unsourced: it may mean that no Agnus performs the cycles, or that no product used them.
- **The reading of the page.** The register rows, the `When UHRES is enabled` and `BPLHPTH,L pointers. V10-V0 on DB10-0.` lines, p. 27's `(see BPLHPTH,L) after horiz line start. It increments`, p. 16's `(also needs bits in DMACON) (hires chips only)`, the DMAEN line and BPLHMOD's `UHRES bit plane pointer (BPLHPTL,H) every line, and then` are `[3/3]` in the corpus, and p. 6's `+` line is `[2/3]`. Continuation lines that carry no register name are `[1/3]`, among them p. 15's `'free' cycle after the start ...`, `the first and third free cycles.` and BPLHMOD's other two lines, and p. 27's `This pointer is activated in the 1st & 3rd 'free' cycles`.

## Independent evidence

The question put to each: with BPLCON0 bit 7 and DMAEN set, do the UHRES pointers take Chip-memory cycles, and on which chip set?

| | where | what it does |
|---|---|---|
| **WinUAE** | [`custom.cpp:9259-9285`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9259-L9285), [`:11921-11924`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L11921-L11924), [`:10052-10063`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10052-L10063), [`:11853-11878`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L11853-L11878), [`:7507-7516`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L7507-L7516), [`:2545-2615`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2545-L2615), [`:3504-3512`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L3504-L3512), [`:1187`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L1187) | **takes the cycles.** With BPLCON0 bit 7 set, `if (bplcon0 & 0x0080) {` / `generate_uhres();` runs after the bit-plane request and before the Copper's, and acts only in a free slot with DMAEN set, `if ((dmacon & DMA_MASTER) && check_rga_free_slot_in())`. The chain starts at the line start, `if (agnus_hpos == 1 && !beamcon0_dual) {` / `// !DUAL: trigger is START`, and takes three consecutive free slots after it for the sprite pointer, `write_rga(RGA_SLOT_IN, CYCLE_UHRESSPR, 0x078, NULL)`, the bit-plane pointer (`0x07a`) and the sprite pointer again. The cycle reads no data — the page sends it to external logic — but it takes the slot from the Copper, the blitter and the CPU, and advances the pointer, `pt += 2 + r->bplmod;` and `pt += 2;`. The lines are switched on and off at the start and stop registers, `if (vpos == bplhstrt) {` / `uhres_bpl = true;`. The nine registers are decoded, `case 0x1E8: SPRHPTH(value); break;` and its eight neighbours, and written only `if (ecs_agnus) {`, and an OCS Agnus never sees bit 7, `if (!ecs_agnus) {` / `v &= 0xff0f;`; `ecs_agnus` is set for an ECS and for an AGA chip set. Its pointer writes are crossed: SPRHPTH and SPRHPTL write `hhbpl`, which the bit-plane slot uses (`r->p = &hhbpl;`), BPLHPTH and BPLHPTL write `hhspr`, which the sprite slot uses, and each high-word write ORs into bits 15–0 (`hhbpl \|= v;`) while each low-word write ORs into bits 31–16. Its UHRES addresses therefore do not follow p. 6's `+` pair; that the cycles happen does not depend on them, and the addresses are WinUAE's question |
| **vAmiga** | [`Core/Components/Memory/Memory.cpp:3086-3112`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L3086-L3112), [`Core/Components/AmigaTypes.h:298-305`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/AmigaTypes.h#L298-L305) | **no UHRES DMA.** The register write switch goes from `case 0x1CE >> 1: // VBSTOP (ECS)` to `case 0x1D8 >> 1: // HHPOSW (AGA)` and from `case 0x1E4 >> 1: // DIWHIGH (ECS)` to `case 0x1FC >> 1: // FMODE (AGA)`; a write to any of the nine addresses falls out of it and is logged, `"pokeCustom16(%X [%s]): NON-OCS\n"`. Its register-name table calls $1D0–$1D6 and $1E6–$1EE `"unused"`. No DMA code reads BPLCON0 bit 7 |
| **Denise** | [`emulation/libami/agnus/agnus.h:15-21`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.h#L15-L21), [`emulation/libami/agnus/register.cpp:704-710`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L704-L710) | **no UHRES DMA, and it says so.** The Agnus header's todo list ends with ` * UHRES/DUAL stuff`. `writeCustom` has no case for the nine addresses; a write to one takes the `default:` arm, `readCustom(adr, true); // writes to read only addresses cause read access` |
| **Copperline** | [`src/chipset/agnus.rs:1305-1307`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L1305-L1307), [`src/bus/custom_regs.rs:946-957`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L946-L957), [`:1362`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L1362), [`src/regcheck.rs:321`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/regcheck.rs#L321) | **no UHRES DMA, deliberately.** Its HHPOSW comment says the counter `itself is outside the emulated model (UHRES/DUAL is explicitly out of` / `scope)`, and a BEAMCON0 write with DUAL logs `"BEAMCON0 DUAL set: A2024/Productivity (UHRES dual-monitor) display is not emulated"`. The custom-register write has no arm for the nine addresses and ends `_ => false,`. Its register checker counts the bit among BPLCON0's defined bits on the later chips, `// BPLCON0: HIRES..ECSENA (BPU3/SHRES/UHRES on ECS/AGA)` |
| **MiSTer** | [`rtl/agnus.v:137-215`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus.v#L137-L215), [`rtl/denise.v:128`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise.v#L128), [`:151`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise.v#L151) | descendant — the same seven-source multiplexer, and the bit as two comments, `//wire          uhres;      // ultra-hires enables the UHRES pointers; needs bits in DMACON also` and `//assign uhres    = bplcon0[7];`. No UHRES DMA |
| **TC64** | [`rtl/minimig/agnus.v:195-266`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus.v#L195-L266), [`rtl/minimig/agnus_beamcounter.v:92`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L92), [`:224-225`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L224-L225), [`:485`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L485), [`rtl/minimig/denise.v:158`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise.v#L158) | descendant — the same multiplexer, no UHRES DMA. Denise assigns `assign uhres    = bplcon0[7];` and nothing reads it. BPLHSTRT's address is decoded for another purpose, `parameter	BPLHSTRT = 9'h1D4; // UHRES vstart - never implemented on real hardware - use for RTG split`, with `// AMR - Register for never-implemented-on-real-hardware Dual mode - we use as line compare` above the register and `assign rtg_linecompare = vpos==bplhstrt_reg ? 1'b1 : 1'b0;` as its use |

WinUAE puts the chain where pp. 15 and 27 put it: in free slots after the line start, ahead of the Copper, with DMAEN, with an ECS or an AGA Agnus — the page's "hires chips" and Alice alike. Denise and Copperline leave it out and say so in their own words; vAmiga leaves the nine addresses unnamed. MiSTer and TC64, descendants, have NanoMig's multiplexer and no UHRES DMA.

## Decision

One option: give Agnus the UHRES chain pp. 15 and 27 describe.

- **The registers.** Decode SPRHSTRT, SPRHSTOP, BPLHSTRT, BPLHSTOP ($1D0–$1D6, bits 10–0), BPLHMOD ($1E6) and the two pointer pairs ($1E8/$1EA and $1EC/$1EE, five high bits and fifteen low bits, p. 6's `+` pair) in Agnus, and keep BPLCON0 bit 7 there.
- **The chain.** At each line start, with BPLCON0 bit 7 and DMAEN set, take the next three free chip-set memory cycles — those no disk, refresh, audio, bit-plane or sprite slot uses — for the sprite pointer, the bit-plane pointer and the sprite pointer again; a new source in the multiplexer of lines 169–267, below the sprite branch and above the Copper. The bit-plane cycle runs on the lines from BPLHSTRT up to BPLHSTOP and the sprite cycles from SPRHSTRT up to SPRHSTOP, and each puts its identifier on the register bus, BPLHDAT ($07A) and SPRHDAT ($078). With DUAL set the line start is the alternate counter's reset (p. 12), which [NM-AA-008](NM-AA-008.md)'s counter carries.
- **The pointers.** After each access the bit-plane pointer advances by 2 plus the sign-extended BPLHMOD, the sprite pointer by 2.

No display NanoMig drives reads these pointers: the page sends their data to external logic for a VRAM display. What a program that sets BPLCON0 bit 7 with DMAEN would notice is up to three free cycles a line, on the lines it names, that the chain takes from the Copper, the blitter and the CPU, and the addresses that appear on the bus. This option has not been run by `make test`.
