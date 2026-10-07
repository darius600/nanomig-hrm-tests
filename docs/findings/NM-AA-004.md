# NM-AA-004 — VPOSR reports chip identifier $23, where the AA list gives an Alice $22 [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should VPOSR bit 8 (identifier bit I0) read 0 on an Alice, giving the AA list's `$22` PAL / `$32` NTSC instead of this core's `$23` / `$33`? Closes when `agnus_beamcounter.v:110` drives bit 8 from `1'b0` instead of the second `aga` and `VPOSR-REPORTS-THE-CHIP-IDENTIFIER` turns green, or when an Amiga specialist rejects the change on an 8374 capture reading `$23` / `$33` (the probe under *Risks and unknowns*), in which case this page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | Alice (AGA), VPOSR bits 14-8, PAL and NTSC; isolated to the duplicated `aga` term, since the NTSC `+$10` matches the list |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF p. 29, the VPOSR entry · claim ids by assertion: `VPOSR-REPORTS-THE-CHIP-IDENTIFIER`: `opus:AA-vhpos.c6`, `opus:AA-vhpos.c7`, `opus:AA-vhpos.c8`, `opus:AA-vhpos.c9`, `opus:AA-vhpos.c10`, `opus:AA-vhpos.c11`, `opus:AA-vhpos.c12`, `opus:AA-vhpos.c13`, `opus:AA-vhpos.c14`, `opus:AA-vhpos.c15`, `opus:AA-vhpos.c16`, `opus:AA-vhpos.c17`, `opus:AA-vhpos.c18`, `opus:AA-vhpos.c19`, `opus:AA-vhpos.c20`, `opus:AA-vhpos.c21`, `opus:AA-vpos.c6`, `opus:AA-vpos.c7`, `opus:AA-vpos.c8`, `opus:AA-vpos.c9`, `opus:AA-vpos.c10`, `opus:AA-vpos.c11`, `opus:AA-vpos.c12`, `opus:AA-vpos.c13`, `opus:AA-vpos.c14`, `opus:AA-vpos.c15`, `opus:AA-vpos.c16`, `opus:AA-vpos.c17`, `opus:AA-vpos.c18`, `opus:AA-vpos.c20`, `opus:AA-vpos.c25`, `sol:AA-vpos.c2`, `sol:AA-vpos.c3`, `sol:AA-vpos.c4`, `sol:AA-vpos.c5`, `sol:AA-vpos.c6`, `sol:AA-vpos.c7`, `sol:AA-vpos.c8`, `sol:AA-vpos.c19`, `sol:AA-vpos.c26`, p. 29 |
| **Module** | `src/minimig-aga/agnus_beamcounter.v:110` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L110) at the pinned revision; inherited (MiSTer `rtl/agnus_beamcounter.v:116` and TC64 `rtl/minimig/agnus_beamcounter.v:132` carry the same `{2{aga}}` expression) |
| **Test** | [`aa/aa-beam-position-words`](../../aa/aa-beam-position-words) · `make -C aa/aa-beam-position-words test`, or `hrm/run_all.sh --in ../aa` (that one runs the whole tree) |
| **Assertions** | `VPOSR-REPORTS-THE-CHIP-IDENTIFIER`, red: expected `0x22` PAL / `0x32` NTSC (the AA list's Alice), observed `0x23` / `0x33`; the package has 4 graded assertions, 3 green and this one red |
| **Verdict of the four** | **three of three against the document.** Every one of the four that models an identifier at all returns **$23** for AGA — the value this RTL returns — and none returns the document's $22. Denise casts no vote: its AGA term lands in the wrong bit position. Hardware: not measured on an 8374 |
| **Effort** | none; if an 8374 capture reads `$22` / `$32`, S: one expression in `agnus_beamcounter.v:110`, one bit of one read-only register |
| **See also** | [NM-HRM-034](withdrawn/NM-HRM-034.md), withdrawn: the Hardware Reference Manual's p. 299 prints the same field as an identifier, with 10 for an OCS NTSC Agnus |

## Source requirement

Page 29 prints VPOSR's sixteen bits in one row of a bit table. Bit 15 is LOF, bit 7 is LOL,
bits 2 to 0 are V10 to V8, and bits 14 to 8 are the seven-bit chip identifier:

```
                    I0-I6 chip identification:
                      8361(regular) or 8370(fat) (agnus-ntsc)  = 10
                      8367(pal) or 8371(fat-pal) (agnus-pal)    = 00
                      8372(fat-hr) (agnushr),thru rev. 4      = 20 PAL,30 NTSC
                      8372(fat-hr) (agnushr),rev. 5           = 21 PAL,31 NTSC
                      8374(alice)                             = 22 PAL,32 NTSC
```

`8374` is Alice, the Agnus of the chip set this document specifies. The list
is a progression, `20`, `21`, `22` for successive revisions, with `$10` added
for NTSC. The `8374(alice)` line is tagged `[3/3] ocr+guide+model` in the
corpus: all three independent readings produced it.

## RTL and measurement

`agnus_beamcounter.v:110` builds the whole word in one expression:

```verilog
data_out[15:0] = {long_frame,1'b0,ecs,ntsc,2'b00,{2{aga}},long_line,4'b0000,vpos[10:8]};
```

Bits 14 to 8 are `{1'b0, ecs, ntsc, 2'b00, aga, aga}`. With `aga` and `ecs` high
and `ntsc` low, an Alice on PAL, that is `0100011`, **$23**. The document's Alice
is `0100010`, **$22**: they differ in I0, VPOSR bit 8, which the RTL takes from
`aga` as it does I1.

[`aa/aa-beam-position-words`](../../aa/aa-beam-position-words) reads VPOSR bits 14-8:
expected `0x22` PAL and `0x32` NTSC, observed `0x23` and `0x33` —
`VPOSR-REPORTS-THE-CHIP-IDENTIFIER`, red, one of the package's `3 passed, 1 failed`.
The `+$10` for NTSC matches the list, so only the duplicated `aga` is in question. The
three green assertions (VHPOSW's line number read back in VHPOSR's high byte, the low
byte following the beam, the vertical counter's top three bits at the bottom of VPOSR)
show the fixture reads the register.

## Risks and unknowns

- **What an 8374 returns is not measured.** The AA specification is marked
  PRELIMINARY and dated 06/07/91, its list is a progression `20, 21, 22`, and the
  shipped Alice appears to have been one revision further on.
- **Copperline's citation is to a table this corpus does not carry.** Its comment cites
  an HRM row "8374 (Alice) rev 3 thru rev 4" = $23 PAL / $33 NTSC; the 3rd edition's own
  list, on `appC-playfield/p299`, stops at `8368 (hr) or 8372 (fat-hr) = 20 for PAL, 30
  for NTSC`, with no Alice row. The $23 the three share comes from something later than
  both documents this project reads.
- **Software dependence is unknown.** A program that probes VPOSR to tell an AGA machine
  from an ECS one may read either bit.
- **Probe that would settle it.** Hardware: an Amiga with an 8374 Alice,
  PAL, and an NTSC unit if one is to hand. Minimal read: a program that reads VPOSR
  (`$DFF004`) and masks bits 14-8. Expected capture: `$22` / `$32` or `$23` / `$33`; *Decision* says
  what each means.

## Independent evidence

| | where | result |
|---|---|---|
| **WinUAE** | [`custom.cpp:2399`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2399) | **$23** for AGA, this RTL's value |
| **vAmiga** | [`Agnus.cpp:272`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L272) | **$23** |
| **Denise** | [`agnus.cpp:745`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L745) | **no vote**: its AGA term lands at bits 1:0 instead of I1:I0, so bits 14-8 read the ECS value |
| **Copperline** | [`agnus.rs:98`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L98) | **$23** PAL, $33 NTSC |
| **MiSTer** | [`rtl/agnus_beamcounter.v:116`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L116) | descendant: the identical `{2{aga}}` expression |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:132`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L132) | descendant: the identical expression |

## Decision

If Alice is to report what this document lists, the two duplicated bits become one:

```verilog
-    data_out[15:0] = {long_frame,1'b0,ecs,ntsc,2'b00,{2{aga}},long_line,4'b0000,vpos[10:8]};
+    data_out[15:0] = {long_frame,1'b0,ecs,ntsc,2'b00,aga,1'b0,long_line,4'b0000,vpos[10:8]};
```

It is one edit and changes one bit of one read-only register. It is **not** proposed as
obviously correct: which value real silicon returns is the thing to establish first
(the probe under *Risks and unknowns*). The question closes when that capture is in:
`$22` / `$32` means apply the edit upstream first, `$23` / `$33` means the finding moves
to the printed page.
