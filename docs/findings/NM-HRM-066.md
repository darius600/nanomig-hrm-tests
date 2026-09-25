# NM-HRM-066 — DDFSTRT = $0030 disables sprite 7 only when enough bitplanes are fetched [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red, contested — the implementations split |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `agnus.v`, `agnus_bitplanedma.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L125) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus.v#L123) |
| **Report to** | **NanoMig and MiSTer-devel/Minimig-AGA_MiSTer.** TC64 already blocks sprite DMA for the whole bitplane fetch, so a fix exists to port — see *Where a fix already exists*. There is no automatic merge in either direction |
| **Test** | [`hrm/ch7-dmacon-register-bits`](../../hrm/ch7-dmacon-register-bits) — three assertions at one lores bitplane: `DDFSTRT-EARLIER-THAN-0038-COSTS-SOME-SPRITES-FETCH-CYCLES`, claims `3-max-window.c18`, `3-max.c12` (p. 80); `DDFSTRT-0030-SPRITE-7-AT-ONE-BITPLANE`, claims `3-hscr.c10`, `3-hscroll.c12` (p. 82); `DDFSTRT-0030-SOME-SPRITE-AFFECTED-AT-ONE-BITPLANE`, claims `3-hscr.c17`, `3-hscroll.c17` (p. 83) |
| **Chapter** | 3 — Playfield, printed pp. 80, 82–83; 6 — Blitter, printed pp. 195–197 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 3 red, all with one lores bitplane (`BPLCON0 = $1200`), where no sprite loses a fetch at DDFSTRT = $0030. Their siblings at 2, 4 and 6 lores bitplanes, `DDFSTRT-0030-SPRITE-7-AT-MULTIPLE-BITPLANES` and `DDFSTRT-0030-SOME-SPRITE-AFFECTED-AT-MULTIPLE-BITPLANES`, are green in the same run |
| **Verdict of the four** | **3–1 for p. 82's sentence.** WinUAE, vAmiga and Denise stop sprite DMA once the bitplane fetch has started, at any plane count; Copperline keeps sprite 7 at $0030 at any plane count and asserts it in its own test. NanoMig is the one whose answer depends on the plane count — and Figure 6-9, read by plane with its own footnote, predicts NanoMig's counts slot for slot |

---

## What the HRM says

**Printed p. 80**, on the horizontal hardware limits:

> In addition, it should be noted that using a data-fetch start earlier than
> ($38) will disable some sprites.

**Printed p. 82**, *Specifying Data Fetch in Horizontal Scrolling*:

> The normal data-fetch start for non-scrolled displays is ($38). If
> horizontal scrolling is desired, then the data fetch must start one word
> sooner (DDFSTRT = $0030). Incidentally, this will disable sprite 7.

**Printed p. 83**, the note under Figure 3-24:

> NOTE: Fetching an extra word for scrolling will disable some sprites.

**Printed p. 197**, chapter 6 on DMA time slots:

> bitplane DMA takes precedence over sprite DMA. As shown in Figure 6-9
> larger displays may block out one or more of the highest-numbered sprites,
> especially with scrolling.

**None of the four sentences names a number of bitplanes.** p. 82 names the
value, $0030, and the sprite, 7.

**Figure 6-9 on p. 195 does, slot by slot.** *Read from the drawing, not
quoted:* one 320-mode fetch unit of eight colour clocks carries planes 4, 6
and 2 in its second to fourth slots and planes 3, 5 and 1 in its sixth to
eighth, the first and fifth being free. The unit that starts one word early, at
$30, is drawn over sprite 7's two slots, which fall on its planes 4 and 2; the
unit before it, from $28, falls on sprite 5's two slots (planes 4 and 2) and
sprite 6's (planes 3 and 1). The figure's note says that in this case the
bit-plane DMA steals the cycles normally allocated to the sprites. Its legend
marks the bitplane slots *"320 mode Bit-Plane DMA, by plane"* with a footnote
sign, and the footnote, printed in the figure and read from the image, reads:

> These operations only take slots if the associated operation is being
> performed.

**Printed p. 196** counts bitplane slots the same way, plane by plane:

> If the display contains four or fewer low resolution bitplanes, the 68000
> can be granted alternate memory cycles (if it is ready to ask for the cycle
> and is the highest priority item at the time). However, if there are more
> than four bitplanes, bitplane DMA will begin to steal cycles from the 68000
> during the display.

**So the manual gives two answers for one bitplane.** Read by plane, the
figure has a single lores plane at DDFSTRT = $0030 take only the unit's eighth
slot, which is no sprite's, so no sprite is disabled; at $0028 it takes sprite
6's second slot, at $0020 sprite 4's and 6's, at $0018 sprite 2's, 4's and 6's.
A second plane adds plane 2's slot, which in the unit at $30 is sprite 7's
second; a fourth adds plane 4's, sprite 7's first. pp. 80, 82 and 83 disable
sprites at an early fetch start with no plane count at all. With two or more
lores planes, or any hires plane, the figure costs sprite 7 at $0030 too — one
of its two slots with two or three lores planes or one hires plane, both with
four or more lores planes or two or more hires — and the package's siblings
grade that green.

---

## What NanoMig does

**It takes sprite 7's fetches at DDFSTRT = $0030 only where a fetched plane
needs the same cycle.** With one lores plane that never happens, and sprite 7
keeps every fetch; with four or more lores planes it loses almost all of them.

The sprite request is gated on SPREN and nothing else —
[`src/minimig-aga/agnus.v#L125`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L125):

```verilog
wire dma_spr = req_spr & spren; //sprite dma is using its slot
```

and the priority chain puts bitplane ahead of sprite for a cycle both request —
[`#L207`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L207)
and [`#L219`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L219):

```verilog
	else if (dma_bpl) begin
	else if (dma_spr) begin
```

`dma_bpl` is the bitplane engine's own request,
[`src/minimig-aga/agnus_bitplanedma.v#L472`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v#L472):

```verilog
assign dma = (ddfrun) && dmaena_delayed[1] && hpos[0] && (plane[4:0] < {1'b0,bpu[3:0]}) ? 1'b1 : 1'b0;
```

**`ddfrun`, the fetch sequencer running, is one term of it and leaves the
module by no other path.** `agnus_bitplanedma` outputs `hde`, `dma`,
`reg_address_out` and `address_out`, and nothing else. The request also needs
`plane < bpu`, so a cycle whose plane is not fetched is not requested and the
sprite engine gets it. The sprite engine's own window,
[`src/minimig-aga/agnus_spritedma.v#L263-L272`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_spritedma.v#L263-L272),
opens at `8'h18` and closes at `8'h38` on `hpos` alone:

```verilog
    if (hpos[8:1]==8'h18 && hpos[0])
      enable <= 1;
    else if (hpos[8:1]==8'h38 && hpos[0])
      enable <= 0;
```

So what a sprite loses depends on which planes are fetched, not on whether the
fetch is running. One lores plane is fetched in one cycle of eight, the last
([`agnus_bitplanedma.v#L463`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v#L463),
`plane = {2'b00,~ddfseq[0],~ddfseq[1],~ddfseq[2]};`, which is 0 only for
`ddfseq[2:0] == 7`); each further plane takes another cycle of the eight.

### Measured

The fixture is the Agnus top level with every DMA engine compiled in, in the OCS
configuration (`ecs = 0`, `aga = 0`). All eight sprites are armed identically
(VSTART 40, VSTOP 220), SPREN, BPLEN and DMAEN are set, DIWSTRT is `$2C81`,
DIWSTOP `$2CC1`, DDFSTOP `$00D0`, and each sprite's SPRxDATA and SPRxDATB
fetches are counted over the same 90 lines. At DDFSTRT = $0038 every sprite
counts **172** in every row below.

| BPLCON0 | planes | sprite 7 at DDFSTRT = $0030 | sprites 0–6 at $0030 | graded |
|---|---|---:|---|---|
| `$1200` | 1 lores | **172** | 172 each | **red**, `DDFSTRT-0030-SPRITE-7-AT-ONE-BITPLANE` and `DDFSTRT-0030-SOME-SPRITE-AFFECTED-AT-ONE-BITPLANE` |
| `$2200`, `$3200` | 2 or 3 lores | **90** | 172 each | `$2200` green, in both `-AT-MULTIPLE-BITPLANES` siblings |
| `$4200`, `$5200`, `$6200` | 4, 5 or 6 lores | **8** | 172 each | `$4200` and `$6200` green, in the same two |
| `$9200` | 1 hires | **90** | 172 each | measured only |
| `$A200`, `$C200` | 2 or 4 hires | **8** | 172 each | measured only |

On every row only sprite 7 moves. The p. 80 assertion sweeps the fetch start
with one lores plane, and it is red because its first point loses nothing:

| DDFSTRT, one lores plane | channels that lose a fetch | each keeps | Figure 6-9, read by plane |
|---|---|---:|---|
| `$0030` | none | 172 of 172 | none |
| `$0028` | 6 | 90 of 172 | sprite 6's second slot |
| `$0020` | 4, 6 | 90 of 172 | sprites 4's and 6's second slots |
| `$0018` | 2, 4, 6 | 90 of 172 | sprites 2's, 4's and 6's second slots |

The package reports the number of channels reduced and the worst one at each
point; which channels, and the rows it does not grade, come from the same
harness printing all eight counts for every BPLCON0 above and every DDFSTRT in
the sweep and at $0038 — a measurement for this page, not a graded assertion.
**Every one of those forty-five rows of eight counts is what Figure 6-9
predicts when its bitplane slots are read by plane**, with a sprite keeping 172
when no fetched plane lands on its slots, 90 when one does and 8 when both do:
with three lores planes at $0028, for instance, sprite 5
keeps 90, sprite 6 keeps 8 and sprite 7 keeps 90, because plane 2 lands on
sprite 5's second slot, planes 3 and 1 on both of sprite 6's, and plane 2 of the
next unit on sprite 7's second. The three reds are the three one-plane points at
which pp. 80, 82 and 83 and that reading of the figure part company.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:9612`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9612), [`:9504`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9504), [`:9241-9243`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9241-L9243) | <pre>if (bprun != 1 \|\| bplconflict) {</pre> a sprite slot is generated only while the bitplane sequencer is not running. On OCS `bprun` starts at the DDFSTRT match from the DMA enable and the display window alone: <pre>if (!ddf_limit_out && ddfstrt_match && !bprun && dma && diw) {</pre> Neither line consults BPLCON0's plane count, so **1, 2, 4 and 6 planes all block sprite 7**. On OCS a sprite slot already decided is also cleared once `bprun` is set, under the comment *"OCS off by one bitplane sprite cycle stealing bug"* |
| **vAmiga** | [`Core/Components/Agnus/Agnus.cpp:824-830`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L824-L830), [`Sequencer/SequencerBpl.cpp:248`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Sequencer/SequencerBpl.cpp#L248), [`Sequencer/SequencerDas.cpp:57-58`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Sequencer/SequencerDas.cpp#L57-L58) | <pre>if (isOCS()) {<br>&nbsp;&nbsp;&nbsp;&nbsp;return sequencer.bprunUp &lt;= pos.h + 1;<br>} else {<br>&nbsp;&nbsp;&nbsp;&nbsp;return sequencer.bprunUp &lt;= pos.h;</pre> `spriteCycleIsBlocked()` blocks every sprite slot from the first cycle the sequencer runs, `if (state.bprun) bprunUp = std::min(bprunUp, j);`, and the run state does not depend on the plane count, so **1, 2, 4 and 6 planes all block sprite 7**. Its slots are `p[0x31] = DAS_S7_1;` and `p[0x33] = DAS_S7_2;` |
| **Denise** | [`emulation/libami/agnus/graphics.cpp:342-345`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/graphics.cpp#L342-L345), [`:407`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/graphics.cpp#L407), [`agnus.h:324`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.h#L324) | <pre>if (start && !bplState && !hardStop && useBitplaneDMA() && diwFlipFlop) {<br>&nbsp;&nbsp;&nbsp;&nbsp;bplState = 1;<br>&nbsp;&nbsp;&nbsp;&nbsp;sprInhibited = true;</pre> starting the bitplane fetch inhibits sprites, and a sprite fetch is queued only `if (!sprInhibited)`. `useBitplaneDMA()` is `(dmaConImm & 0x300) == 0x300`, BPLEN and DMAEN, so **1, 2, 4 and 6 planes all block sprite 7**, whose two slots are decided at `case 0x33` and `case 0x35` ([`agnus.cpp:600-601`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L600-L601)). The ECS path sets the same flag at `:306` |
| **Copperline** | [`src/chipset/agnus.rs:529-560`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L529-L560), test [`:1810-1829`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L1810-L1829), [`src/bus/dma_slots.rs:849-854`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L849-L854) | <pre>let sprite7_block_start = 0x0030;<br>ddfstart &lt; sprite7_block_start<br>&nbsp;&nbsp;&nbsp;&nbsp;&& last_block_start &gt;= sprite7_block_start</pre> **keeps sprite 7 at $0030 with 1, 2, 4 or 6 planes**: the fetch has to start *before* $30 to disable it, the plane count only has to be non-zero, and no other sprite is ever disabled this way (`if sprite != 7 \|\| ...`). Its own test `early_bitplane_ddfstart_disables_only_sprite_seven_dma` asserts that $0030 leaves sprite 7 on and $0028 turns it off, with one lores plane. On the bus a sprite slot is resolved before a bitplane slot |
| **MiSTer** | [`rtl/agnus.v:123`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus.v#L123), [`rtl/agnus_bitplanedma.v:472`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_bitplanedma.v#L472) | descendant — identical to NanoMig: `wire dma_spr = req_spr & spren; //sprite dma is using its slot`, the same `dma_bpl` before `dma_spr` priority at `:170`/`:181`, and the same per-plane bitplane request |
| **TC64** | [`rtl/minimig/agnus.v:186`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus.v#L186), [`rtl/minimig/agnus_bitplanedma.v:390`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_bitplanedma.v#L390) | descendant — **has already changed this line**: `assign dma_spr = req_spr & spren & !(ddf & bplen); // AMR - Disallow sprite fetches during bitplane data fetch.`, with `assign ddf = ecs ? ddfrun : ddfena; // AMR - Export ddfrun/ddfena for sprite DMA logic. (Imitates OCS bug, inhibiting too soon.)`. The per-plane bitplane request at `agnus_bitplanedma.v:448` is unchanged; what changed is that the sprite side now sees the whole fetch, at any plane count |

**Three of the four stop sprite DMA for as long as the bitplane fetch runs, and
none of the three asks how many planes are fetched.** At DDFSTRT = $0030 that
disables sprite 7 with one plane or six, which is p. 82's sentence as printed.
WinUAE and vAmiga each block one cycle earlier on OCS than on ECS, and TC64's
comment names the same OCS behaviour.

**Copperline is the dissent, and it is deliberate.** It keeps sprite 7 at $0030
at every plane count and disables it at $0028, and it has a test that says so.
That is not NanoMig's rule either: Copperline disables all of sprite 7 or none
of it by where the fetch starts, and NanoMig takes individual fetches by which
planes are fetched. They agree only at one lores plane, where the package's
three reds are; with two or more planes NanoMig sides with the other three and
against Copperline.

---

## Where a fix already exists

**minimig-dev's TC64 line has made exactly this change.** Its commit `fe2db71`,
*"Sprite fetch now completely blocked once DDF starts, instead of just when
bitplanes actually want data"*, touches `rtl/minimig/agnus.v` and
`rtl/minimig/agnus_bitplanedma.v` and nothing else, and both lines it adds are
present at the pinned revision (`agnus.v:186`, `agnus_bitplanedma.v:390`).
MiSTer has not taken it, and NanoMig has the MiSTer form.

This is the case the findings README describes: a finding with a patch to
point at in one maintained line and the original code in the other.

---

## Suggested change

**Port TC64's two lines.** In `agnus_bitplanedma.v`, export the fetch state:

```verilog
output wire ddf,
...
assign ddf = ecs ? ddfrun : ddfena;
```

and in `agnus.v` gate the sprite request on it:

```verilog
wire dma_spr = req_spr & spren & !(ddf & bplen);
```

NanoMig already has `bplen` (`agnus.v:299`, `dmacon[8] & dmacon[9]`) and both
`ddfrun` and `ddfena` in `agnus_bitplanedma.v`; the declaration of `bplen` has to
come before its first use. That makes sprite 7's fate at $0030 independent of the
plane count. Whether the three reds turn green with this change has not been
run here. It would also move NanoMig away from Figure 6-9's by-plane slots at
every plane count, which is the other half of the decision.

---

## What holds the finding up

- **The fixture can see a sprite lose fetches.** The same count on the same
  fixture drops to 90 or 8 when more planes are fetched, where the two
  multiple-bitplane siblings are green, and to 90 on three channels at
  DDFSTRT = $0018 with one plane. The reds at $0030 are not counts that could
  never move.
- **The comparators move when the stimulus does.** The package's mutation
  record substitutes $0018 and channel 2 into the sprite-7 comparison, and
  $0018 for $0030 in the sweep and in the some-sprite comparison; each of the
  three goes green and nothing else moves.
- **p. 82 is specific.** It names the value and the sprite, and pp. 80, 83 and
  197 say the same in general terms. The fixture uses p. 82's own value.
- **Three independent implementations and one maintained descendant** block
  sprite DMA for the whole fetch, which is what p. 82 needs to hold at every
  plane count.
- **What weakens it is the manual's own figure.** Read by plane, with the
  footnote printed in it, Figure 6-9 predicts every count NanoMig produces,
  the three reds' zeros included. The finding rests on p. 82's sentence and on
  three of four implementations, not on every page of the manual.

---

## For the specialists

- **Does p. 82 cover a one-plane display?** The manual answers both ways. The
  sentence names no plane count; Figure 6-9, whose bitplane slots are drawn
  *by plane* and take a slot *only if the associated operation is being
  performed*, leaves every sprite whole with one lores plane at $0030. The
  package grades the sentence with one lores plane, the one case in which the
  two readings differ at $0030. Which of them describes the silicon is a
  reading question, not a measurement.
- **Does real Agnus block a sprite slot that no fetched plane uses?** WinUAE,
  vAmiga, Denise and TC64 say yes: once the fetch sequencer runs, sprite DMA
  stops. NanoMig says no, and leaves sprite 7 90 of its 172 fetches with two or
  three lores planes and 8 with four or more.
- **$0030 or $0028.** Copperline disables sprite 7 only when the fetch starts
  before $30, which contradicts p. 82's worked value directly.
- **OCS and ECS.** WinUAE, vAmiga and TC64 all block sprite DMA one cycle earlier
  on OCS. The manual does not distinguish the two here, and the fixture runs as
  OCS.
