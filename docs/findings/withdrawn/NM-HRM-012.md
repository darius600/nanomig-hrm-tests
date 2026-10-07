# NM-HRM-012 — SPRxCTL bit 0 (SH0) is honoured at every resolution, not only SuperHires

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | p. 303's SH0 row reads "Low bit 140 ns increment" with no resolution condition, and `SPRXCTL-BIT0-SH0-IS-140NS-LOW-BIT` measures SH0 moving the sprite start by one raw unit at all four `spres` encodings |
| **What is asked of a reader** | Nothing. SPRxCTL bit 0 is the ordinary 140 ns low bit of the start position at every resolution; the SuperHires positions are bits 4 and 3 |
| **Withdrawal reason** | The assertion required SH0 to work only in SuperHires, a restriction p. 303's row does not state; the row gives SH0 as the ordinary 140 ns low bit |
| **Error class** | **reading the page backwards** |
| **Replacement** | Green: `SPRXCTL-BIT0-SH0-IS-140NS-LOW-BIT` in [`hrm/ch3-sprite-registers`](../../../hrm/ch3-sprite-registers) (toggling SPR0CTL bit 0 moved the start by 1 raw unit at each of `spres` 0 to 3), with no mode gate |
| **Module** | `denise_sprites_shifter.v` — [NanoMig, SH0 capture](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites_shifter.v#L137) · [NanoMig, start comparison](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites_shifter.v#L115); inherited (the same two statements stand in MiSTer, lines 95 and 74, and TC64, lines 91 and 70) |
| **Test** | `make -C hrm/ch3-sprite-registers test`; the package reads `10 passed, 1 failed` and exits nonzero, its one red being another finding's assertion |
| **Chapter** | Appendix C — ECS Hardware and the Graphics Library, printed p. 303 (SPRxCTL bit table) · *Amiga Hardware Reference Manual*, 3rd ed., 1991. `SPRXCTL-BIT0-SH0-IS-140NS-LOW-BIT`: `sol:C-shres-sprite.c12`, `sol:C-shres-sprite.c9`, `opus:APPC-pos.c8`, `opus:APPC-pos.c12`, `sol:CH4-appC-position.c14`, `sol:CH4-appC-position.c20`, p. 303. `SH0-IS-HALF-OF-POS-BYTE-STEP`: `sol:C-shres-sprite.c3`, `opus:C-spr.c02`, p. 303 |
| **Assertions** | `SPRXCTL-BIT0-SH0-IS-140NS-LOW-BIT` green (graded at `spres` 0: delta 1; its MEASURED line prints deltas 1, 1, 1, 1 at `spres` 0 to 3); `SH0-IS-HALF-OF-POS-BYTE-STEP` green (delta 1 at SuperHires). The package's one red, `SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE`, is another finding: [NM-HRM-019](../NM-HRM-019.md) |
| **Verdict of the four** | all four take SH0 at every resolution, as NanoMig does. Hardware: not measured |

---

## Why it was withdrawn

**Printed p. 303** reads:

> 00      SH0       Start horiz. value Low bit 140 ns increment

SH0 is the ordinary 140 ns low bit. The SuperHires positions are bits 4 (70 ns)
and 3 (35 ns), printed four and three rows above it. The assertion demanded
that SH0 work only in SuperHires, which is not merely unstated but the opposite
of what the row says. The replacement requires only what the row states:
toggling `SPRxCTL` bit 0 moves the start by exactly one raw unit, with no
mode gate. The implementation was right; the oracle was not.

**Condition for filing again.** A printed page that ties SH0 to a resolution.
The SH0 row of p. 303 does not.

---

## RTL and measurement

`denise_sprites_shifter.v` stores SH0 on every SPRxCTL write
([`:137`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites_shifter.v#L137))
and compares it with `hpos` without reading `spres`
([`:115`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites_shifter.v#L115)).
The package measures the shift as 1 raw unit at each of the four `spres`
encodings.

---

## Risks and unknowns

- The measurement is of the Verilated sprite module, not of a chip: hardware is
  not measured.

---

## Independent evidence

| | |
|---|---|
| **WinUAE** | as NanoMig. [`drawing.cpp:2822`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L2822) adds `s->ctl & 1` to the start with no mode condition |
| **vAmiga** | as NanoMig. [`Denise.cpp:170-172`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L170-L172) puts SH0 into `sprhpos` unconditionally |
| **Denise** | as NanoMig. [`denise.cpp:294-298`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L294-L298) `setSprCtl` ors `value & 1` into `spr.x` |
| **Copperline** | as NanoMig. [`bus.rs:10579-10580`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L10579-L10580) builds `hstart` from `ctl & 0x0001` |
| **MiSTer** | descendant — the same two statements, `rtl/denise_sprites_shifter.v:95` and `:74` |
| **TC64** | descendant — the same two statements, `rtl/minimig/denise_sprites_shifter.v:91` and `:70` |

### What other Commodore documents say

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 10 — agrees with the manual:

  > "00 SH0 Start horiz. value low bit."
  <!-- corpus: denise-spec-252126/p010.txt -->

  The ECS Denise specification, which also defines SHRES, gives SH0 as the ordinary start-horizontal low bit with no SuperHires condition (SPRxPOS: 'Low bit (SH0) is in SPRxCTL reg below.'). Manual and NanoMig already agree here; this is one more voice for the same reading.
