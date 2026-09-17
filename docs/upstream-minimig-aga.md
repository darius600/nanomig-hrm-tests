# Upstream: what NanoMig inherited from Minimig-AGA, and what the two maintained lines have since changed

NanoMig descends from MiSTer's Minimig-AGA. For every finding there is a question
the four emulators structurally cannot answer and this comparison can:

> **Is this NanoMig's own, or did it come down from upstream — and has upstream
> fixed it since?**

That decides where a fix belongs, and whether fixing it in NanoMig means
diverging from Minimig-AGA.

| | revision |
|---|---|
| NanoMig (the submodule this suite builds) | [`d6f7df9`](https://github.com/harbaum/NanoMig/tree/d6f7df957fad655bc07f6a381e5094a9edff0be0) · v0.14-beta-33 |
| Minimig-AGA on MiSTer | [`da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/tree/da7632e44498840850e78966947f8770166e6e77) · 2026-09-11 |
| Minimig-AGA on MiST/TC64 | [`9e8e308`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/tree/9e8e30844fe134dca54cea65f1bd38539dfc15ad) · 2025-05-03 |

**Two groups maintain this ancestry independently**, and until 17.09.2026 this
document knew only one of them. Neither is a witness to what the manual requires
— all three trees descend from the same code, so agreement between them says
nothing about the HRM. What a second line adds is a second answer to *where a
fix belongs*, and on two findings it turns out to hold the fix already.

Compared with `diff --strip-trailing-cr -b`: NanoMig's files are **LF** and both maintained lines are **CRLF**, so a naive
byte comparison reports every line of every file as changed and says nothing.

**That direction was stated backwards here until 17.09.2026.** Measured byte
by byte: `src/minimig-aga/cia_int.v` carries **zero** CR bytes and 70 LF;
MiSTer's copy carries 94 CRLF and no bare LF; TC64's carries 70 CRLF and no
bare LF. Across the trees, 56 of NanoMig's 58 files are LF and 2 mixed, 63 of
MiSTer's are CRLF and 2 mixed, and all 66 of TC64's are CRLF. No number in
this document moves -- the comparison normalises both sides either way -- but
the sentence was wrong and had been copied into two other documents.

## The shape of it

Of 58 Verilog files in NanoMig's `src/minimig-aga`, **20 are content-identical to
current upstream**, 32 differ, and 6 exist only in NanoMig (`bitplane_ram.v`,
`dpram.v`, `dpram_be_1024x16.v`, `paula_floppy_dma_buf.v`, `paula_floppy_ebr.v`,
`sprite_ram.v` — the memory primitives, which is what a port to a different FPGA
family replaces).

**Identical to upstream:**

```
agnus_audiodma.v              denise_collision.v         paula_audio.v
agnus_bitplanedma.v           denise_playfields.v        paula_audio_channel.v
agnus_blitter_adrgen.v        denise_spritepriority.v    paula_audio_mixer.v
agnus_blitter_barrelshifter.v amiga_clk.v                paula_audio_volume.v
agnus_blitter_fill.v          cart.v                     paula_intcontroller.v
agnus_blitter_minterm.v       agnus_diskdma.v            paula_uart.v
agnus_refresh.v               agnus_spritedma.v
```

**Every module chapter 5's findings touch is in that list.** The whole audio
path, the interrupt controller and the audio DMA engine are byte-for-byte what
upstream has today.

## Where NanoMig diverged, it is mostly style

Sampled on `agnus_copper.v`, whose 18 differing lines are all of one shape:

```diff
-  		copjmp1 <= 1;      NanoMig
+  		copjmp1 = 1;       upstream
```

Blocking assignments changed to non-blocking. That is a port-side lint or
synthesis decision, not a behavioural change, and it does not touch any finding.

## Where upstream has moved and NanoMig has not

`agnus_blitter.v` is the clearest case. Upstream has gained a blitter freeze
state that NanoMig does not have:

```verilog
    else if (blt_state == BLT_FROZEN) begin
      // A new BLTSIZE starts the next blit and releases the freeze.
      if (reg_address_in[8:1] == BLTSIZE[8:1] ||
          (reg_address_in[8:1] == BLTSIZH[8:1] && ecs))
        blt_state <= BLT_INIT;
```

and a fix for a phantom read, with the reason in the comment:

```verilog
  // AMR - Contraz Domination uses clr.w bltcon1, which does a phantom read from
  // a w/o register, momentarily enabling line mode and wiping out bltbhold
```

**Neither is a finding this suite could ever have, and the reason is worth
stating once.** *Contraz Domination* is an AGA demo, and **the oracle has no
AGA**. Counted over the whole corpus: `ECS` appears 490 times in the extracted
text and `AGA` appears zero times — the single match is inside the PDF's own
bytes, not on any page. The third edition documents OCS and the Enhanced Chip
Set, and its preamble says so directly:

> "registers have been changed the Enhanced Chip Set (ECS). The ECS is …"

So this is not upstream work we happen not to have tested. It is upstream work
**outside what this suite can adjudicate at all** — as is every other
AGA-specific behaviour in the RTL, because the RTL implements AGA and the manual
does not describe it. See the scope note in the top-level README.

## Per-finding answers established so far

| finding | module | inherited? | fixed upstream? |
|---|---|---|---|
| `NM-HRM-020` BLTSIZV not ECS-gated | `agnus_blitter.v` | **yes** — the line is identical in both (`:548` here, `:568` there), and `BLTSIZH` carries `&& ecs` in both | **no** |
| `NM-HRM-024` audio pointer takes five high bits | `agnus_audiodma.v` | **yes** — file identical | **no** |
| `NM-HRM-025` volume 64 collapses onto 63 | `paula_audio_mixer.v` | **yes** — file identical, OR-collapse included | **no** |
| `NM-HRM-026` state codes differ from Figure 5-8 | `paula_audio_channel.v` | **yes** — file identical | **no** |
| `NM-HRM-027` direct-mode interrupt is early | `paula_audio_channel.v` | **yes** — file identical | **no** |
| `NM-HRM-028` ADKCON attach bits reach nothing | `paula_audio_channel.v`, `paula.v` | **yes** for the channel; `paula.v` differs but not at the ADKCON wiring | **no** |
| `NM-HRM-030` Paula does not decode DMACONR | `paula.v` | **yes** — `DMACONR` appears in none of the 50 differing lines, so both decode it the same way, which is to say not at all | **no** |
| `NM-HRM-031` INTREQ bit 14 is settable | `paula_intcontroller.v` | **yes** — file identical | **no** |
| `NM-HRM-015` BRDNTRAN declared and never connected | `denise.v` | **yes** — `BRDNTRAN` appears in none of the 91 differing lines | **no** |
| `NM-HRM-032` BEAMCON0 sync-polarity bits decoded nowhere | `agnus_beamcounter.v` | **yes** — the three commented-out `//wire` declarations are byte-identical (`:140-142` here, `:146-148` there) | **no** |
| `NM-HRM-033` LOLDIS needs VARBEAMEN too | `agnus_beamcounter.v` | **yes** — `(loldis && varbeamen)` is byte-identical (`:295-297` here, `:282-284` there) | **no** |
| `NM-HRM-034` VPOSR bit 12 carries `ntsc` in OCS | `agnus_beamcounter.v` | **yes** — the read mux differs only in a signal name (`vpos` here, `vpos_rb` there); the bit assignment is identical | **no** |
| `NM-HRM-035` any CIA-A write clears the overlay | `minimig.v` | **yes** — the two lines are byte-identical (`:861-862` here, `:933-934` there), in a file that otherwise differs in 789 lines | **no** |
| `NM-HRM-036` chip banks 1-3 stay mapped under the overlay | `gary.v` | **yes** — the four `sel_chip` lines are byte-identical (`:159-162` here, `:172-175` there), in a file that otherwise differs in 71 lines | **no** |

Chapter 7's five were each checked line by line rather than file by file,
because `minimig.v` and `gary.v` differ substantially between the two trees
and a whole-file verdict would have said nothing about the lines in question.

The remaining findings sit in files that differ, and each needs its own line
checked the same way before this table can claim anything about it.

## The second line, measured

Same method, over the 28 modules the findings cite.

**Six modules are identical in all three trees**, line numbers included:
`agnus_audiodma.v`, `agnus_blitter_adrgen.v`, `denise_collision.v`,
`paula_audio_mixer.v`, `paula_intcontroller.v`, `paula_uart.v`.

**Three are NanoMig's own** and exist in neither line: `src/misc/amiga_keymap.v`,
`hid.v`, `sysctrl.v`.

**The two lines have drifted apart, in both directions.** TC64 is closer to
NanoMig on `cia_int.v` (identical, against MiSTer's 113 differing lines),
`ciab.v` (6 against 218) and `agnus_blitter.v` (14 against 61, and all fourteen
are whitespace). It is much further on `agnus.v` (741 against 115), `minimig.v`
(1563 against 811) and `userio.v` (736 against 165).

**A module is absent only when the whole repository has no copy.** TC64 spreads
Verilog across `rtl/minimig`, `rtl/sdram`, `rtl/mist`, `rtl/audio`, `rtl/io`,
`rtl/clock` and `rtl/jtag`. A first pass scoped to `rtl/minimig` reported
`cpu_cache_new.v` as missing; it is at `rtl/sdram/cpu_cache_new.v`, live in the
board target, and the statement NM-HRM-060 is about is identical in all three.

### Where both lines agree and NanoMig differs, the divergence is NanoMig's own

Two files make this exact:

- **`agnus_copper.v`** — MiSTer and TC64 each differ from NanoMig by 18 lines,
  and it is the **same 18**, byte for byte: nine `<=` assignments written `=` on
  `copjmp1`, `copjmp2` and the strobes. Both lines kept the original; NanoMig
  changed it.
- **`denise_colortable.v`** — likewise 11 lines in each, and the same 11: both
  lines carry the 32-bit byte-enabled RAM primitive where NanoMig has the
  12/24-bit interface.

Neither touches any finding. What the pattern gives is a cheap test: when both
maintained lines hold the same code and NanoMig does not, the edit was made
downstream.

### Two findings where the second line already does what the manual says

**NM-HRM-035 — any CIA-A write clears the overlay.** NanoMig's `minimig.v:861-862`
makes `ovl` a register set on reset and cleared by *any* write to CIA-A; MiSTer's
`:933-934` is byte-identical. **TC64 has no such lines.** There `ovl` is a plain
`wire` (`rtl/minimig/minimig.v:343`) taken straight from CIA-A's port-A output
(`:922`), which `ciaa.v` computes from `regporta` gated by `ddrporta` — the
effective-pin formula all four independent implementations use. TC64 moved the
decision out of `minimig.v` entirely, to a place where it tracks the OVL bit by
construction.

**NM-HRM-046 — the drive light follows DMA, not the motor.** Both alternatives
are present in all three trees and the lines differ only in which one is live:

```
NanoMig  paula_floppy.v:972   assign fdd_led  = (dskstate!=DISKDMA_IDLE);
MiSTer   rtl/paula_floppy.v:544  assign fdd_led  = (dskstate!=DISKDMA_IDLE);
                            :545  //assign disk_led = |motor_on;
TC64     rtl/minimig/paula_floppy.v:795  //assign disk_led = dskstate!=DISKDMA_IDLE ? 1'b1 : 1'b0;
                                    :796  assign disk_led = |motor_on;
```

**TC64 has flipped which line is live.** A fix to NanoMig has a working sibling
to match rather than a commented-out suggestion. It does not resolve the whole
finding: TC64's lamp is still the single aggregate, not one per drive.

### Where the second line has moved and NanoMig has not

- **NM-HRM-032** — the BEAMCON0 sync-polarity bits. NanoMig and MiSTer keep
  `CSYTRUE`, `VSYTRUE` and `HSYTRUE` commented out. TC64 does not: `HSYTRUE` and
  `VSYTRUE` feed two module outputs, `hsyncpol` and `vsyncpol`, that `agnus.v`
  exposes upward. `CSYTRUE` stays declared and unused there too, so the three
  bits are not alike and the finding's row says so.
- **NM-HRM-054** — CIAA port B. TC64's `regportb` is live rather than commented
  out and drives `portb_out`, but the top-level CIA-A instantiation never
  connects `.portb_out` and the read mux still ignores it. The register exists
  and reaches nothing.
- **NM-HRM-015** — `BRDNTRAN`. Live in TC64 at `denise.v:205,224` and still
  consumed nowhere: a dangling net rather than a connected one. Same observable
  behaviour, different shape.
- **NM-HRM-037** — the right port's pot X. TC64 has an `_mthird1 &` term that
  reads like a fix, but its default build defines `MINIMIG_PS2_MOUSE`, which
  hardwires `_mthird1` to `1'b1`. The built behaviour matches NanoMig's.

### Where the second line has less than NanoMig

**NM-HRM-025's supporting citation.** `paula_audio_mixer.v` is identical in all
three, but the PWM `volgate` block in `paula_audio_channel.v` that the finding
cites alongside it **does not exist in TC64 at all** — TC64 carries only the
multiply path. MiSTer carries both, as NanoMig does.

**NM-HRM-048 and NM-HRM-049.** "NanoMig's own" extends to TC64: it has no
in-core bit-serial write stage either, using an SPI host link, and no MFM
encoder.

## What this does not say

It says nothing about whether a finding is right. Upstream carrying the same code
is not evidence that the manual permits it — Minimig-AGA and NanoMig are one
implementation for this purpose, and the four emulators remain four independent
readings. What it establishes is **where the decision was made**, which is a
different question and the one a developer asks first.
