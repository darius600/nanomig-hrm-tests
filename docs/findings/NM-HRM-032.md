# NM-HRM-032 — BEAMCON0's three sync-polarity bits are decoded nowhere [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Do BEAMCON0 bits 2, 1 and 0 (CSYTRUE, VSYTRUE, HSYTRUE), which Appendix C p. 305 prints as polarity controls for the C, V and H sync pins, change `_csync`, `_vsync` and `_hsync` on a real ECS Agnus? Closes when the RTL decodes them and `P11-BEAMCON0-CSYTRUE-BIT2-POLARITY-CONTROL-FOR-C-SYNC-PIN`, `P12-BEAMCON0-VSYTRUE-BIT1-POLARITY-CONTROL-FOR-V-SYNC-PIN`, `P13-BEAMCON0-HSYTRUE-BIT0-POLARITY-CONTROL-FOR-H-SYNC-PIN` and `SYNC-POLARITY-BITS-CHANGE-THE-PINS` turn green, or when an Amiga specialist records them as inert on a real ECS Agnus, in which case this page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | `agnus_beamcounter` with ECS enabled (the BEAMCON0 write is gated on it); in the HRM package each bit written alone and with every other implemented BEAMCON0 bit set, in the AA package the three set together; chip revision unknown; real-chip behaviour not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Appendix C, printed p. 305, `BEAMCON0` bit table (test chapter 7) · *Amiga Hardware Reference Manual*, 3rd ed., 1991. <details><summary>claim ids by assertion (16)</summary> Claim ids: `P11-BEAMCON0-CSYTRUE-BIT2-POLARITY-CONTROL-FOR-C-SYNC-PIN`: `sol:7-appC-beamcon0.c15`, `opus:7-appC-beamcon0.c15`; `P12-BEAMCON0-VSYTRUE-BIT1-POLARITY-CONTROL-FOR-V-SYNC-PIN`: `sol:7-appC-beamcon0.c16`, `opus:7-appC-beamcon0.c16`; `P13-BEAMCON0-HSYTRUE-BIT0-POLARITY-CONTROL-FOR-H-SYNC-PIN`: `sol:7-appC-beamcon0.c17`, `opus:7-appC-beamcon0.c17`. Second document: Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, preliminary, pp. 12-13 as the PDF numbers them (the document prints none); `SYNC-POLARITY-BITS-CHANGE-THE-PINS`: `sol:AA-beamcon0.c19`, `sol:AA-beamcon0.c20`, `sol:AA-beamcon0.c21`, `opus:AA-beamcon0.b2`, `opus:AA-beamcon0.b1`, `opus:AA-beamcon0.b0` (p. 12), `sol:AA-beamcon0.c62`, `sol:AA-beamcon0.c63`, `sol:AA-beamcon0.c64`, `opus:AA-beamcon0.c43` (p. 13). All four assertions carry claim ids. </details> |
| **Module** | `agnus_beamcounter.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L140-L142) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L146-L148); MiSTer has the same code; TC64 differs (it decodes HSYTRUE and VSYTRUE into outputs; CSYTRUE stays unused): inherited |
| **Test** | [`hrm/ch7-beamcon0-ecs-control-bits`](../../hrm/ch7-beamcon0-ecs-control-bits): `make -C hrm/ch7-beamcon0-ecs-control-bits test`; corroborated by [`aa/aa-beamcon0-control-bits`](../../aa/aa-beamcon0-control-bits): `make -C aa/aa-beamcon0-control-bits test`; an expected red exits nonzero |
| **Assertions** | In the HRM package, 3 red (package `8 passed, 4 failed`; the fourth red, `P6-BEAMCON0-LOLDIS-BIT11-DISABLES-LONG-LINE-TOGGLE`, is a different claim, on [NM-HRM-033](NM-HRM-033.md)): `P11-BEAMCON0-CSYTRUE-BIT2-POLARITY-CONTROL-FOR-C-SYNC-PIN`, expected the `_csync` trace to change with bit 2 set, observed byte-for-byte identical (883 edges); `P12-BEAMCON0-VSYTRUE-BIT1-POLARITY-CONTROL-FOR-V-SYNC-PIN`, `_vsync` identical (3 edges); `P13-BEAMCON0-HSYTRUE-BIT0-POLARITY-CONTROL-FOR-H-SYNC-PIN`, `_hsync` identical (881 edges). In the AA package (`4 passed, 1 failed`), `SYNC-POLARITY-BITS-CHANGE-THE-PINS`, expected the three pins to differ somewhere, observed 0 of 260 samples |
| **Verdict of the four** | **split**: only WinUAE implements all three; Denise implements one; two do not model them at all. Hardware: not measured |
| **Effort** | S — uncomment the three wire declarations in `agnus_beamcounter.v` and apply them to the three sync outputs |

---

## Source requirement

**Appendix C, printed p. 305**, in the `BEAMCON0` bit table:

> ```
> 2     CSYTRUE    Polarity control for C sync pin
> 1     VSYTRUE    Polarity control for V sync pin
> 0     HSYTRUE    Polarity control for H sync pin
> ```

Three bits, each naming a pin whose polarity it controls.

## RTL and measurement

The three bits are declared and **commented out**, and nothing reads `beamcon0_reg[2:0]`:
[`src/minimig-aga/agnus_beamcounter.v#L138-L142`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L138-L142)

```verilog
//wire csynctrue    = beamcon0_reg[ 2];
//wire vsynctrue    = beamcon0_reg[ 1];
//wire hsynctrue    = beamcon0_reg[ 0];
```

The composite sync is formed without them:
[`src/minimig-aga/agnus_beamcounter.v#L421`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L421)

```verilog
assign _csync = _hsync & _vsync | vser; //composite sync with serration pulses
```

`hrm/ch7-beamcon0-ecs-control-bits` writes each bit alone, and again with every other implemented BEAMCON0 bit set, over a full field, and compares the trace byte for byte against the same run with the bit clear:

| bit | trace | edges observed |
|---|---|---|
| 2 — CSYTRUE | `_csync` **identical** | 883 |
| 1 — VSYTRUE | `_vsync` **identical** | 3 |
| 0 — HSYTRUE | `_hsync` **identical** | 881 |

The edge counts make the comparison non-vacuous. The fixture does reach BEAMCON0: PAL (bit 5), written the same way, moves the frame from 262 to 312 lines (`P10-BEAMCON0-PAL-BIT5-SELECTS-VTOTAL-312-OVER-262`, green).

`hrm/ch3-beam-counters-and-blanking` discarded these bits on the reasoning that nothing at this boundary could be observed. That does not hold: `_csync`, `_vsync` and `_hsync` are real outputs of this module (port list, l.42-46) and the manual says each bit transforms one of them, so a bit the RTL never decodes, driving a pin that exists, is a deviation. Chapter 7 overrules chapter 3 on this point.

## Risks and unknowns

- Whether the bits change anything on a real ECS Agnus is not measured. Two of the four emulators model none of them, which could mean the bits are unimportant in practice, or only that no software exercises them.
- This finding stands on the printed page, not on a consensus among implementations, and must not be reported as though everyone else did this and NanoMig did not. WinUAE implementing all three while Denise implements one is the kind of split that usually means somebody measured one of them and nobody measured the others.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:9809-9817`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9809-L9817) | <pre>if (beamcon0 & BEAMCON0_CSYTRUE) flags ^= DENISE_RGA_FLAG_CSYNC;<br>if (beamcon0 & BEAMCON0_VSYTRUE) flags ^= DENISE_RGA_FLAG_VSYNC;<br>if (beamcon0 & BEAMCON0_HSYTRUE) flags ^= DENISE_RGA_FLAG_HSYNC;</pre> all three |
| **vAmiga** | [`Core/Components/Agnus/AgnusRegs.cpp:550-570`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L550-L570) | `pokeBEAMCON0` names all three in its bit-map comment and acts on **PAL and LOLDIS only** |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:1051-1053`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L1051-L1053) | <pre>inline auto Agnus::csyncPolTrue(bool state) -> bool {<br>&nbsp;&nbsp;&nbsp;&nbsp;return (beamCon & CSYTRUE) ? !state : state;<br>}</pre> **CSYTRUE only** — no VSYTRUE or HSYTRUE exists anywhere in the tree |
| **Copperline** | [`src/chipset/agnus.rs:21-42`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L21-L42) | defines `BEAMCON0_` constants for bits 3 and 5-14 — **no constant exists for bits 0, 1 or 2**; its own comment gives the reason: *"the xSYTRUE pin polarities are latched without physical host output"* |
| **MiSTer** | [`rtl/agnus_beamcounter.v:146-148`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L146-L148) | descendant — identical to NanoMig: `csynctrue`/`vsynctrue`/`hsynctrue` are commented-out declarations, nothing reads `beamcon0_reg[2:0]` |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:171-173`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L171-L173) | descendant — the three wires are **not** commented out: `wire hsynctrue = displaydual ? beamcon0_sh[0] : beamcon0_reg[0];` and likewise `vsynctrue`, feeding two new outputs, `hsyncpol`/`vsyncpol` (`:175-176`), that `agnus.v` forwards outward. `csynctrue` is declared the same way but used nowhere else; `_csync` is still formed at `:458` as `(_hsync & _vsync) \| vser` |

The 1991 AA functional specification prints the same requirement, in its own bit table and in the paragraph

```
HSYTRUE,VSYTRUE,CSYTRUE= These change the polarity of the
 HSY*, VSY*, & CSY* pins to HSY, VSY,& CSY respectively
 for input & output.
```

`aa/aa-beamcon0-control-bits` runs one video line twice at the `agnus_beamcounter` boundary, once with the three bits clear and once with them set, sampling `_hsync`, `_vsync` and `_csync` once per colour clock:

```
MEASURED: over a line the three sync pins differ in 0 of 260 samples between
          bits clear and bits set; _hsync takes 2 level(s) either way
```

The pins pulse (two levels), so the fixture is driving a beam, and the bits change nothing about them. This is corroboration of the same requirement, not a second finding; no new key was issued for it.

## Decision

Suggested change: uncomment the three declarations and invert the outputs they name:

```verilog
wire csynctrue    = beamcon0_reg[ 2];
wire vsynctrue    = beamcon0_reg[ 1];
wire hsynctrue    = beamcon0_reg[ 0];
...
assign _csync = csynctrue ^ (_hsync & _vsync | vser);
```

The polarity convention needs care: these outputs are active low by name (`_csync`), while the manual's "TRUE" bits are written from the active-high perspective. WinUAE's XOR against its own sync flags is the model to follow.

Where it goes: MiSTer first, since it has the same code. TC64 already decodes HSYTRUE and VSYTRUE into outputs, so a fix decoding those two diverges from MiSTer but meets TC64; a fix to CSYTRUE diverges from all three.

Re-run `make -C hrm/ch7-beamcon0-ecs-control-bits test` and `make -C aa/aa-beamcon0-control-bits test`. Today `P11`, `P12`, `P13` and `SYNC-POLARITY-BITS-CHANGE-THE-PINS` are red; they are the assertions a change must turn green. Closes as stated in *What is asked of a reader*.
