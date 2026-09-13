# Upstream: what NanoMig inherited from Minimig-AGA, and what MiSTer has since changed

NanoMig descends from MiSTer's Minimig-AGA. For every finding there is a question
the four emulators structurally cannot answer and this comparison can:

> **Is this NanoMig's own, or did it come down from upstream — and has upstream
> fixed it since?**

That decides where a fix belongs, and whether fixing it in NanoMig means
diverging from Minimig-AGA.

| | revision |
|---|---|
| NanoMig (the submodule this suite builds) | [`d6f7df9`](https://github.com/harbaum/NanoMig/tree/d6f7df957fad655bc07f6a381e5094a9edff0be0) · v0.14-beta-33 |
| Minimig-AGA | [`da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/tree/da7632e44498840850e78966947f8770166e6e77) · 2026-09-11 |

Compared with `diff --strip-trailing-cr -b`: NanoMig's files are CRLF and
upstream's are LF, so a naive diff reports every line of every file as changed
and says nothing.

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

## What this does not say

It says nothing about whether a finding is right. Upstream carrying the same code
is not evidence that the manual permits it — Minimig-AGA and NanoMig are one
implementation for this purpose, and the four emulators remain four independent
readings. What it establishes is **where the decision was made**, which is a
different question and the one a developer asks first.
