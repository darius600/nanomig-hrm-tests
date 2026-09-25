# NM-HRM-067 — sprites 6 and 7 fetch their control words after horizontal blanking ends, and no implementation keeps them inside it

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a graded red |
| **What is asked of a reader** | No RTL change follows from this. The manual puts all sixteen sprite control-word fetches inside horizontal blanking; NanoMig ends its blanking before the last three, and no independent implementation keeps all sixteen inside its own blanking either. The entry exists so the next reader does not take the red for an implementation fault. |
| **Module** | `agnus_beamcounter.v`, `agnus_spritedma.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L78-L81) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L78-L81) |
| **Report to** | **nobody, as an RTL defect.** Both constants and the sprite window are inherited unchanged from MiSTer, and TC64 drives its displayed blank from the same positions. The question is whether printed pp. 23 and 303 describe where real Agnus blanks |
| **Test** | [`hrm/ch4-sprite-dma-control`](../../hrm/ch4-sprite-dma-control) · `SPRXPOS-SPRXCTL-FETCH-DURING-HBLANK`, claims `APPC-pos.c5` and `CH4-appC-position.c8` (p. 303) |
| **Chapter** | Appendix C — ECS, printed p. 303; Appendix A, printed p. 284; 4 — Sprites, printed p. 126; 2 — Coprocessor, printed p. 23; 6 — Blitter, printed p. 195 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 1 red of 8 graded. `SPREN-GATES-EVERY-SPRITE-CHANNEL`, green in the same run, brings the same sixteen control-word fetches onto the bus |
| **Verdict of the four** | **two of the four end blanking before the last sprite slots, and none keeps all sixteen inside it.** vAmiga ends it at $23, before eight of the sixteen slots, and rejects the $0F and $35 bounds p. 23 prints; Denise ends it before sprite 6's second slot and both of sprite 7's, the three NanoMig loses. WinUAE blanks on Denise's own counter and Copperline has no hardwired end of blanking, so neither places one on the sprite-slot grid |

---

## What the HRM says

**Appendix C, printed p. 303**, on SPRxPOS and SPRxCTL:

> They are usually loaded by the sprite DMA channel, during horizontal blank,
> however they may be loaded by the processor.

**Appendix A, printed p. 284**, says the same of the same pair, and then of the
data registers:

> They are usually loaded by the sprite DMA channel during horizontal blank;
> however, they may be loaded by either processor at any time.

> They are usually loaded by the sprite DMA channel but may be loaded by
> either processor at any time.

The second sentence has *usually* and no blanking clause, so on this page
*usually* sets the sprite DMA channel against the processor. It does not
qualify when the channel's own loads happen.

**Chapter 4, printed p. 126**, on the data words the same slots carry:

> The fetch and store for each horizontal scan line occurs during a horizontal
> blanking interval, far to the left of the start of the screen display.

**Chapter 2, printed p. 23**, says where that interval is:

> Horizontal blanking falls in the range of $0F to $35.

Figure 6-9 on p. 195, *read from the drawing, not quoted*, puts the sixteen
sprite slots, two per channel, from about $15 to about $34, sprite 7's pair
just after $30, and ends the arrow marked *SPRITE DMA TIME* at about $34. A
footnote printed in the figure, read from the image, says the chart is
approximate:

> Actual system hardware demands certain specific values for data fetch start
> and display start. Therefore this timing chart has been "adjusted" to match
> those requirements.

**So the pages and the figure agree with each other**: every sprite slot, sprite 7's
last included, falls before $35, where p. 23 ends horizontal blanking. No page
of the manual places a sprite slot outside it.

---

## What NanoMig does

**It fetches the last three control words after its horizontal blanking has
ended.** The two windows are set in two modules, neither of which refers to
the other.

The sprite engine's window opens at colour clock $18 and closes at $38, on
`hpos` alone —
[`src/minimig-aga/agnus_spritedma.v#L263-L272`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_spritedma.v#L263-L272):

```verilog
//dma slots for sprites: even cycles from 18 to 38 (inclusive)
always @ (posedge clk) begin
  if (clk7_en) begin
    if (hpos[8:1]==8'h18 && hpos[0])
      enable <= 1;
    else if (hpos[8:1]==8'h38 && hpos[0])
      enable <= 0;
  end
end
```

and requests a slot every second colour clock inside it,
`if (enable && hpos[1:0]==2'b01)`
([`#L283-L285`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_spritedma.v#L283-L285)).

The blanking window is two constants in `hpos` units, half a colour clock
each —
[`src/minimig-aga/agnus_beamcounter.v#L78-L81`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L78-L81):

```verilog
parameter HBSTRT_VAL      = 17+4+4;          // horizontal blanking start
...
parameter HBSTOP_VAL      = 103-5+4;         // back porch = 4.7us (103) shorter blanking for overscan visibility
```

applied at
[`#L452-L456`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L452-L456),
`if (hpos==hbstrt)` sets `hblank` and `else if (hpos==hbstop)` clears it.
That is `hpos` 25 to 102, colour clocks $0C to $33, about as long as p. 23's
$0F to $35 but placed about seven colour clocks earlier against the sprite
slots. The `-5` in `HBSTOP_VAL` moves the end two and a half colour clocks
earlier for the reason its own comment gives; the measurement below shows that
this alone does not account for the red.

### Measured

[`hrm/ch4-sprite-dma-control`](../../hrm/ch4-sprite-dma-control) arms all
eight sprites, sets SPREN and samples agnus's own `hblank` output on the tick
of every SPRxPOS and SPRxCTL fetch:

```
FAIL: SPRXPOS-SPRXCTL-FETCH-DURING-HBLANK: ... of 16 SPRxPOS/SPRxCTL fetch
      events across 8 sprite channels swept, 13 occurred while hblank was
      high; 2 channels (6 and 7, the LAST two in the fetch window) had at
      least one fetch outside hblank
```

one of the package's `7 passed, 1 failed`. Where each fetch lands, read with
the package's own harness printing each fetch's offset from the start-of-line
pulse — a measurement for this page, not a graded assertion — in ticks of half
a colour clock:

| | ticks after the start of the line |
|---|---|
| `hblank` rises | 22 |
| sprite 0 POS … sprite 6 POS, 13 fetches four ticks apart | 48 … 96 |
| `hblank` falls | 99 |
| sprite 6 CTL | 100 |
| sprite 7 POS | 104 |
| sprite 7 CTL | 108 |

Sprite 6's CTL comes half a colour clock after blanking ends, sprite 7's two
words two and a half and four and a half. Without the `-5`, blanking would end
at tick 104, and sprite 7's CTL would still come after it.

---

## What the others say

The question put to each: where does horizontal blanking end, against the
sixteen sprite control-word slots?

| | code | |
|---|---|---|
| **WinUAE** | [`drawing.cpp:3063-3064`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L3063-L3064), [`:4026-4030`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L4026-L4030), [`:5207-5212`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L5207-L5212) | <pre>denise_hbstrt_lores = ecs_denise ? 0x10 : 0x0f;<br>denise_hbstop_lores = ecs_denise ? 0x5d : 0x5d;</pre> WinUAE blanks on **Denise's** horizontal counter, in lores pixels, not on the Agnus beam counter its sprite slots are scheduled on; the counter restarts on the horizontal strobe (*"OCS Denise hcounter only resets if STRHOR or STRVBL."*). No expression relates the window to a sprite slot, and this reading did not place its end on the slot grid: **no vote** |
| **vAmiga** | [`Core/Infrastructure/Constants.h:94-107`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Infrastructure/Constants.h#L94-L107), [`Core/Components/Agnus/Sequencer/SequencerDas.cpp:43-58`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Sequencer/SequencerDas.cpp#L43-L58), [`Core/Components/Denise/PixelEngine.cpp:551-555`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/PixelEngine.cpp#L551-L555) | <pre>static const isize HBLANK_MIN = 0x12;   // First HBLANK cycle<br>static const isize HBLANK_MAX = 0x23;   // Last HBLANK cycle</pre> The comment above them names $0F and $35 as the first and last HBLANK cycles and rejects them: *"However, these values seem to be wrong. According to the Agnus schematics, the first HBLANK cycle is $12 and the last cycle is $23."* The sprite slots run from `p[0x15] = DAS_S0_1;` to `p[0x33] = DAS_S7_2;`, so **sprites 4 to 7, eight of the sixteen slots, come after blanking ends** — further out than NanoMig. The pixel engine wipes exactly that range, `for (pixel = start; pixel <= stop; pixel++) dst[pixel] = u32(hbcol);` |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:498`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L498), [`:525-526`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L525-L526), [`:596-601`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L596-L601) | <pre>case 0x2f:<br>&nbsp;&nbsp;&nbsp;&nbsp;endHblank();<br>&nbsp;&nbsp;&nbsp;&nbsp;if (!vBlank) spriteControl&lt;6, true&gt;(); break;<br>case 0x31: if (!vBlank) spriteControl&lt;6, false&gt;(); break;<br>case 0x33: if (!vBlank) spriteControl&lt;7, true&gt;(); break;<br>case 0x35: if (!vBlank) spriteControl&lt;7, false&gt;(); break;</pre> Blanking starts at `case 0xc:` with `startHblank();` and ends in the cycle that decides sprite 6's first slot; **sprite 6's second slot and both of sprite 7's are decided after it — the three fetches NanoMig loses.** The case labels are beam positions, which its own comment sets four cycles from the manual's chart, *"de-adjust HRM DMA view by 4 cycles to Beam position."*; the order of the two windows does not depend on that |
| **Copperline** | [`src/video/bitplane.rs:134-138`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane.rs#L134-L138), [`src/chipset/agnus.rs:955-966`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L955-L966) | <pre>const DENISE_HBLANK_START_HPOS: u32 = 0x12;</pre> the one fixed blanking position in the tree, the colour clock at which *"Denise's texture/output line starts"*. Otherwise horizontal blanking exists only as ECS's programmable window under BLANKEN, `programmable_horizontal_blank()`. There is no hardwired end of blanking to put a sprite slot before or after: **no vote** |
| **MiSTer** | [`rtl/agnus_beamcounter.v:78-81`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L78-L81), [`rtl/agnus_spritedma.v:267-269`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_spritedma.v#L267-L269) | descendant — identical to NanoMig: `parameter HBSTOP_VAL      = 103-5+4;` and the same window, `if (hpos[8:1]==8'h18 && hpos[0])` to `else if (hpos[8:1]==8'h38 && hpos[0])` |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:94-97`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L94-L97), [`:496-503`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L496-L503), [`rtl/minimig/agnus_spritedma.v:267-269`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_spritedma.v#L267-L269) | descendant — the same sprite window; the blanking constants are NanoMig's less 12, `parameter	HBSTOP_VAL      = 103-5+4-12;`, and the displayed blank is delayed by 12 ticks, `blank<=displaydual \| hblank_delay[11];`, which puts it where NanoMig's `hblank` is. Its `hblank_out` ends 12 ticks earlier still |

**None of the four keeps all sixteen control-word fetches inside horizontal
blanking.** The two that put a hardwired blanking end on the same grid as the
sprite slots both put it earlier than p. 23 does: vAmiga before eight of the
sixteen slots, Denise before three, exactly the three NanoMig loses. vAmiga
names the bounds p. 23 prints and says what it took instead. The other two do not model a hardwired
blanking end that could be compared at all.

---

## Why this is filed against the page

The red is real and the measurement is not in doubt: the fixture's `hblank`
rises and falls on every line, and the package's mutation record puts sprites
0 to 5 alone through the same per-fetch comparator and gets 12 of 12 inside
blanking. What is in doubt is the printed range. p. 23's $0F to
$35 is the only place the manual says where blanking is, and every
implementation that models a hardwired blanking end on the slot grid ends it
earlier — one of them with a comment that names the bounds p. 23 prints, calls
them wrong, and cites the Agnus schematics for $12 and $23. NanoMig's own
constant is a display choice with its reason written beside it. Nobody read
here keeps sprite 7's control words inside blanking.

The finding key names the pair that disagrees, NanoMig and p. 303, and does not
prejudge which side is wrong.

---

## Suggested change

Not an RTL change. What would settle it is a measurement of a real Agnus's
blanking output against its sprite DMA slots, or a reading of the schematics
vAmiga cites. If p. 23's $35 holds, p. 303 and p. 126 are right and every
implementation read here ends blanking early; if vAmiga's $23 holds, real
hardware loads sprites 4 to 7 during the visible part of the line, and the two
pages describe the early channels only.

---

## For the specialists

1. **Does anything depend on it?** A sprite's control words are loaded in the
   slot they are loaded in; nothing a program does waits on the blanking edge.
   The question is only whether pp. 303 and 126 are accurate statements about
   the timing.
2. **Which end moved?** In NanoMig's own counter both windows are displaced
   from the printed numbers: blanking runs from $0C to $33 against p. 23's $0F
   to $35, and the sprite window opens at $18 where Figure 6-9's first slot is
   at about $15. Blanking is about as long as p. 23's and sits about seven
   colour clocks earlier against the slots. The fixture grades the relation,
   which is what the printed pages state, and cannot say which of the two
   positions a fix would move.
