# NM-AA-006 — BPLCON1 resets to $3300, where the AA bit table marks bits 15-8 `=0` [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should `denise_bitplanes.v` reset BPLCON1 to $0000 instead of $3300 when the AA chip set is selected, so that PF1H0 (bit 8) and PF2H0 (bit 12) read 0 after RST_, as AA PDF p. 16 marks them `=0`? Closes when the reset value is changed and `P4b-C1`, `P4b-C2` and `P5` are green on both cores, or when an Amiga specialist rejects the change; if the rejection means that p. 16's marks on BPLCON1 are wrong, the page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | AA chip set (`chipset_config` = $10), the state after the harness's power-on reset and before the first write to BPLCON1, on fx68k and TG68K; graded for PF1H0 (bit 8) and PF2H0 (bit 12) only, the other six marked bits (15, 14, 13, 11, 10, 9) being recorded, not graded; chip revision unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF p. 16, the BPLCON1 column of the bit table and the line under it; PDF p. 17, PF1Hx and PF2Hx; PDF pp. 2 and 4, what RST_ resets; PDF pp. 3 and 5. The document prints no page numbers: p. N is the PDF page · <details><summary>claim ids by assertion (16)</summary> `P4b-C1`: `opus:AA-bplcon1.b08.rst`, p. 16; `P4b-C2`: `opus:AA-bplcon1.b12.rst`, p. 16; `P5`: `sol:AA-bplcon1.c18`, `sol:AA-bplcon1.c22`, p. 16. Recorded, not graded: `P4a-D1`: `opus:AA-bplcon1.b09.rst`, `opus:AA-bplcon1.b10.rst`, `opus:AA-bplcon1.b11.rst`, p. 16; `P4a-D2`: `opus:AA-bplcon1.b13.rst`, `opus:AA-bplcon1.b14.rst`, `opus:AA-bplcon1.b15.rst`, p. 16; `P5-PF2H7`: `sol:AA-bplcon1.c15`; `P5-PF2H6`: `sol:AA-bplcon1.c16`; `P5-PF2H1`: `sol:AA-bplcon1.c17`; `P5-PF1H7`: `sol:AA-bplcon1.c19`; `P5-PF1H6`: `sol:AA-bplcon1.c20`; `P5-PF1H1`: `sol:AA-bplcon1.c21`, p. 16. `P4-FIXTURE`, `P4-F1`, `P4-R1`, `P4-S3A`, `P4-S3B-A`, `P4-S3B-B` and `P4-SIG` carry no claim id</details> |
| **Module** | `src/minimig-aga/denise_bitplanes.v:168-175` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_bitplanes.v#L168-L175) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_bitplanes.v#L138-L145); inherited (TC64's block at `rtl/minimig/denise_bitplanes.v:150-157` is identical) |
| **Test** | [`integration/aa-integ-s1`](../../integration/aa-integ-s1): `make -k -C integration/aa-integ-s1 test`, or `test-fx68k` and `test-tg68k` of the same package; an expected red exits nonzero. The AA chip set, graded on both CPU cores |
| **Assertions** | 3 red on fx68k and 3 on TG68K, in a package run of 156 passed, 10 failed, 11 NOT GRADED on each core: `P4b-C1` expects PF1H0 (BPLCON1 bit 8) to read 0 after reset, the plane-1 stripe colour being the same in F3 (reset state) and F5 (BPLCON1 = $0000), and observes colour 0 in F3 and 1 in F5; `P4b-C2` the same for PF2H0 (bit 12) and plane 2; `P5` expects bit 12 and bit 8 to be PF2H0 and PF1H0 and 0 after reset, and observes the stripe colours unequal. `P4a-D1`, `P4a-D2` and the six `P5-PF*` lines, which grade the other six marked bits through the delay, print NOT GRADED because each rests on `P4b-C1` or `P4b-C2` showing PFnH0 = 0. The run's seven other reds are not this finding's: `P6-II` and `P6-AGG` grade BPLCON0's BYPASS bit ([NM-AA-007](NM-AA-007.md)), `P10-LATCH` the light-pen latch ([NM-HRM-087](NM-HRM-087.md)), and `P14-U1`, `P14-U2`, `P14-REL` and `P14-AGG` HHPOSR with DUAL set ([NM-AA-008](NM-AA-008.md)) |
| **Verdict of the four** | **three of three with the page, and none with NanoMig.** WinUAE, vAmiga and Copperline all start an AA machine with BPLCON1 = $0000: vAmiga and Copperline clear it on every reset, WinUAE never resets it and starts from zero. Denise models no AA machine and casts no vote. MiSTer and TC64, descendants, reset it to $3300 as NanoMig does. Hardware: not measured |
| **Effort** | S — `denise_bitplanes.v:172` and its copy at `:458`, the reset value of `bplcon1`, which becomes $0000 when `aga` is set |

---

## Source requirement

PDF p. 16 prints the five bit-plane control registers in one bit table. Bits 15 to 8 of BPLCON1 carry an `=0` mark and bits 7 to 0 carry none; the line under the table says what the marks mean:

```
      BIT#      BPLCON0     BPLCON1     BPLCON2     BPLCON3     BPLCON4
      ----      --------    --------    --------    --------    --------
      15        HIRES       PF2H7=0        x        BANK2=0     BPLAM7=0
      14        BPU2        PF2H6=0     ZDBPSEL2    BANK1=0     BPLAM6=0
      13        BPU1        PF2H1=0     ZDBPSEL1    BANK0=0     BPLAM5=0
      12        BPU0        PF2H0=0     ZDBPSEL0    PF2OF2=0    BPLAM4=0
      11        HAM         PF1H7=0     ZDBPEN      PF2OF1=1    BPLAM3=0
      10        DPF         PF1H6=0     ZDCTEN      PF2OF0=1    BPLAM2=0
      09        COLOR       PF1H1=0     KILLEHB     LOCT=0      BPLAM1=0
      08        GAUD        PF1H0=0     RDRAM=0        x        BPLAM0=0
      07        UHRES       PF2H5       SOGEN=0     SPRES1=0    ESPRM7=0
      06        SHRES       PF2H4       PF2PRI      SPRES0=0    ESPRM6=0
      05        BYPASS=0    PF2H3       PF2P2       BRDRBLNK=0  ESPRM5=0
      04        BPU3=0      PF2H2       PF2P1       BRDNTRAN=0  ESPRM4=1
      03        LPEN        PF1H5       PF2P0          x        OSPRM7=0
      02        LACE        PF1H4       PF1P2       ZDCLKEN=0   OSPRM6=0
      01        ERSY        PF1H3       PF1P1       BRDSPRT=0   OSPRM5=0
      00        ECSENA=0    PF1H2       PF1P0       EXTBLKEN=0  OSPRM4=1
                    x= don't care; but drive to 0 for upward compatibility !
                    =0/=1 bit values initialized by RST_ pin going low.
```

PDF p. 17 says what the eight-bit fields are:

```
PF2Hx= Playfield 2 horizontal scroll code,x=0-7
PF1Hx= Playfield 1 horizontal scroll code,x=0-7
        where PFyH0=LSB=35nS=1 SHRES pixel(bits have been
        renamed,old PFyH0 now PFyH2,etc.). Note that the
        scroll range has been quadrupled to allow for wider
        (32 or 64 bit) bitplanes.
```

After RST_ each playfield's delay is therefore the four unmarked bits only, PF1H5-PF1H2 in bits 3-0 and PF2H5-PF2H2 in bits 7-4, in steps of 4 SHRES pixels. The eight marked bits are the ones AA added. PDF p. 3:

```
BPLCON1 now contains an 8 bit scroll value
```

and PDF p. 5:

```
bit 00 in BPLCON1 used to be named PF1H0 and now it's called PF1H2.
```

PDF pp. 2 and 4 list the registers RST_ resets. p. 2:

```
  - A RST_ input pin has been added, which resets all the bits
    contained in registers that were new for ECS or LISA:
    BPLCON3,BPLCON4,CLXCON2,DIWHIGH,FMODE
```

p. 4:

```
        RST_ pin resets all bits in all registers new to AA. These
        registers include: BPLCON3,BPLCON4,CLXCON2,DIWHIGH,FMODE.
```

## RTL and measurement

**BPLCON1 resets to $3300.** [`src/minimig-aga/denise_bitplanes.v#L168-L175`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_bitplanes.v#L168-L175):

```verilog
    if (reset)
      bplcon1 <= #1 16'h3300;
    if ((reg_address_in[8:1] == BPLCON1[8:1]))
      bplcon1 <= #1 aga ? data_in[15:0] : {2'b00,2'b11,2'b00,2'b11,data_in[7:0]};
```

$3300 sets bits 13, 12, 9 and 8: PF2H1, PF2H0, PF1H1 and PF1H0, four of the eight bits p. 16 marks `=0`. Each delay takes them as its two lowest bits ([`#L148`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_bitplanes.v#L148), [`#L160`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_bitplanes.v#L160)), so after reset both delays are 3 SHRES pixels (105 ns), where the table gives 0. $3300 is the pattern an ECS-mode write forces into the same bits (`aga` low); with `aga` high a write stores all sixteen bits, so an AA machine keeps the 3-pixel delay until software first writes BPLCON1. The block lies in the `ifdef DENISE_EBR` branch, which the TG68K build defines; the fx68k build compiles the other branch, whose block at [`#L454-L461`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_bitplanes.v#L454-L461) is the same, `bplcon1 <= #1 16'h3300;` on reset.

[`integration/aa-integ-s1`](../../integration/aa-integ-s1) reads the reset state off the screen, because p. 16 lists BPLCON1 as written (`W`) and gives no value for a read. Each run comes out of the harness's power-on reset, writes everything its display needs except BPLCON1 (`prog_bars_midrun.s` writes none before F3 and reads no custom register but INTREQR), shows a frame, F3, with BPLCON1 as reset left it, then writes $0000 at the VERTB that starts F5 and shows F5 as the reference.

- **Run (b)**, SuperHires with 64-bit fetch, shows a band of alternating SHRES pixels in each playfield. The video output keeps one sample per 70 ns, so the band shows one of its two colours and a delay of one SHRES pixel shows the other. Read at every op from 12 to 68 past each frame's own solid edge, the band shows its playfield's colour in F5 (`1`) and does not in F3 (`0`), in both playfields: each playfield sits an odd number of SHRES pixels from where $0000 puts it, so PF1H0 and PF2H0 are not 0. `P4b-C1` and `P4b-C2` are red on both cores, and `P5` with them.
- **Run (a)**, low resolution with 64-bit fetch, measures the delay: F3 lies 2 op right of F5 on both cores, for both playfields. 3 SHRES pixels is 1.5 op, between two samples. `P4a-D1` and `P4a-D2` accept only even whole-op delays from 0 to 30 and are graded only where run (b) shows PFnH0 = 0, so both print NOT GRADED, as do the six `P5-PF*` lines that rest on them.
- **The bits themselves work.** `P2` writes each of bits 15-8 alone and its rows are green on both cores: PF1H0 alone ($0100) and PF2H0 alone ($1000) turn their own playfield's band from colour 1 to 0 and leave the other's.

```
MEASURED: P4 run(b) measured: plane-1 stripe colour F3 0 F5 1, plane-2 stripe colour F3 0 F5 1
MEASURED: P4 run(a) measured: F3-vs-F5 Delta1=2 op, Delta2=2 op
MEASURED: P5 conjuncts: P2 rows 8-15 all-ok=1; P4 run(b) stripe colours equal=0; P4 run(a) graded=0
```

(The same on fx68k and TG68K.) `P4-FIXTURE`, `P4-F1`, `P4-R1`, `P4-S3A`, `P4-S3B-A`, `P4-S3B-B` and `P4-SIG`, on which the reds rest, are green on both cores.

`P4b-C1` is graded at `test_aa_integ_s1.cpp:2927-2929`, `P4b-C2` at `:2930-2932` and `P5` at `:2969-2971`; `P4a-D1` and `P4a-D2` are recorded at `:2951-2956`.

## Risks and unknowns

- **PDF pp. 2 and 4 list the registers RST_ resets, and BPLCON1 is not among them** (quoted under *Source requirement*). These lists name whole registers; p. 16 also marks bits in BPLCON0, BPLCON1 and BPLCON2, which neither names. The readings differ only on whether p. 16's marks on old registers hold at all; neither gives BPLCON1 a reset value of 1 in any bit. The package's `SPECIFICATION.md` records the disagreement (section 6, *RST_ scope*).
- **The reading of the page.** The rows of the p. 16 table are tagged `[2/3] ocr+model` in the corpus, and the 1993 `AGA.guide` retyping, the third reading, prints the same marks in its own BPLCON1 table (`| 08   | PF1H0=0 |`). The line under the table is tagged `[1/3]`; the Tesseract reading of the page (`ocr/p16_300dpi_psm6.txt`, line 41) has it word for word. The document is marked PRELIMINARY.
- **Six of the eight marked bits are not graded.** Bits 15, 14, 13, 11, 10 and 9 are recorded, not graded; of them $3300 sets bits 13 and 9 (PF2H1, PF1H1) and clears the rest. Only PF1H0 and PF2H0 are shown non-zero.
- **Real hardware is not measured.** A display shown before any write to BPLCON1, with a band of alternating SHRES pixels as in run (b), settles what Lisa holds after RST_: the band's colour changes when $0000 is then written only if PF1H0 or PF2H0 was set.
- **Does anything rely on an AA display starting where ECS mode puts it?** This core's AA reset reproduces the ECS-mode write pattern, so an AA machine and an ECS one show the same position until BPLCON1 is written.
- **Witness limits.** WinUAE gets $0000 by never resetting the register, so it is not evidence for RST_ either way. vAmiga stores the two lowest bits of each field and never reads them, so its value for the four bits $3300 sets is a stored one, not one a display could show.

## Independent evidence

The question put to each: on an AA machine, what does BPLCON1 hold after reset?

| | where | result |
|---|---|---|
| **WinUAE** | [`custom.cpp:676`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L676), [`:3575-3582`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L3575-L3582), [`:6676`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L6676), [`:6754-6756`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L6754-L6756), [`drawing.cpp:428`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L428), [`:3525-3533`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L3525-L3533) | **$0000 at start-up, because nothing resets it**: the reset sets `bplcon0`, `bplcon3` and `bplcon4`, and through `denise_reset(true)` Denise's copies of the same three, and never `bplcon1` or `bplcon1_denise`, file-scope statics that start at zero; a later reset keeps the last value written |
| **vAmiga** | [`Serializable.h:671-676`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Infrastructure/Serializable.h#L671-L676), [`CoreComponent.cpp:232-244`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Infrastructure/CoreComponent.cpp#L232-L244), [`Denise.h:632`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.h#L632), [`:684`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.h#L684), [`Denise.cpp:29-47`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L29-L47) | **$0000 on every reset**: every reset walks each component with a `SerResetter`, which sets each member it visits to zero; `bplcon1` is visited before the soft-reset return, and `_didReset` then sets `bplcon3` and `bplcon4` and leaves `bplcon1` at zero. Its AGA Denise, marked [`(partially supported)`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/DeniseTypes.h#L24), [stores all sixteen bits](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/DeniseRegs.cpp#L212) and [scrolls by bits 3-0 and 11-10 only](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/DeniseRegs.cpp#L702-L703) |
| **Denise** | [`system.cpp:444-459`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/system.cpp#L444-L459), [`denise.h:22`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.h#L22) | **no vote**: `setModel` offers OCS and ECS chip sets only, and the display chip's models end at `ECS = 8` |
| **Copperline** | [`denise.rs:398-405`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/denise.rs#L398-L405), [`bus.rs:3661`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L3661), [`:5145`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L5145) | **$0000 at power-on and at every reset**: `Denise::new()` sets `bplcon1: 0,` beside `bplcon3` and `bplcon4`, and both the bus's constructor and `reset_for_keyboard_reset` build a new one |
| **MiSTer** | [`rtl/denise_bitplanes.v:138-145`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_bitplanes.v#L138-L145) | descendant: the identical block, `bplcon1 <= #1 16'h3300;` on reset |
| **TC64** | [`rtl/minimig/denise_bitplanes.v:150-157`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise_bitplanes.v#L150-L157) | descendant: the identical block |

## Decision

**One option: reset BPLCON1 to $0000 when the AA chip set is selected.**

```verilog
-      bplcon1 <= #1 16'h3300;
+      bplcon1 <= #1 aga ? 16'h0000 : 16'h3300;
```

With `aga` low it keeps today's reset, which equals what every ECS-mode write forces into the same bits, so the ECS display does not move. With `aga` high, which is every run of the package, a build against a private copy whose BPLCON1 resets to `16'h0000` (`NMH_MUTATE=P5-NONE`; mutation build, not run by `make test`) prints PASS for `P4b-C1`, `P4b-C2`, `P4a-D1`, `P4a-D2`, `P5` and the six `P5-PF*` lines on both cores, by the package's own record. On an AA machine both playfields would then start 3 SHRES pixels (105 ns) further left than today, until software first writes BPLCON1; a display that writes BPLCON1 before it is shown sees no difference.
