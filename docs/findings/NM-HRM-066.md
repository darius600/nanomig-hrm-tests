# NM-HRM-066 — DDFSTRT = $0030 disables sprite 7 only when enough bitplanes are fetched [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Does p. 82's "this will disable sprite 7" at DDFSTRT = $0030 hold with one lores bitplane (`BPLCON0 = $1200`), where NanoMig keeps all 172 of sprite 7's fetches and loses none on any sprite? Closes when the change under *Decision* is made and `DDFSTRT-0030-SPRITE-7-AT-ONE-BITPLANE`, `DDFSTRT-0030-SOME-SPRITE-AFFECTED-AT-ONE-BITPLANE` and `DDFSTRT-EARLIER-THAN-0038-COSTS-SOME-SPRITES-FETCH-CYCLES` turn green, or when an Amiga specialist rejects it because Figure 6-9 read by plane governs, in which case this page is refiled against the printed page. |
| **Report to** | `NanoMig direct` — harbaum/NanoMig and MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | Agnus in the OCS configuration (`ecs = 0`, `aga = 0`), measured with eight identically armed sprites; DDFSTRT = $0030 at one lores plane isolated, swept to $0018 at one lores plane; chip revision unknown, hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 3 — Playfield, printed pp. 80, 82–83; 6 — Blitter, printed pp. 195–197 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims, by assertion: `DDFSTRT-EARLIER-THAN-0038-COSTS-SOME-SPRITES-FETCH-CYCLES` `opus:3-max.c12` / `sol:3-max-window.c18` (p. 80); `DDFSTRT-0030-SPRITE-7-AT-ONE-BITPLANE` and `DDFSTRT-0030-SPRITE-7-AT-MULTIPLE-BITPLANES` `opus:3-hscr.c10` / `sol:3-hscroll.c12` (p. 82); `DDFSTRT-0030-SOME-SPRITE-AFFECTED-AT-ONE-BITPLANE` and `DDFSTRT-0030-SOME-SPRITE-AFFECTED-AT-MULTIPLE-BITPLANES` `opus:3-hscr.c17` / `sol:3-hscroll.c17` (p. 83); the chapter 6 passages (pp. 195–197) carry no claim id |
| **Module** | `agnus.v`, `agnus_bitplanedma.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L125) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus.v#L123); MiSTer has the same code; fixed in TC64 |
| **Test** | [`hrm/ch7-dmacon-register-bits`](../../hrm/ch7-dmacon-register-bits): `make -C hrm/ch7-dmacon-register-bits test`; an expected red exits nonzero |
| **Assertions** | 3 red, all with one lores bitplane (`BPLCON0 = $1200`): `DDFSTRT-0030-SPRITE-7-AT-ONE-BITPLANE` (expected sprite 7 to lose fetches at $0030, observed 172 of 172), `DDFSTRT-0030-SOME-SPRITE-AFFECTED-AT-ONE-BITPLANE` (expected some of the eight channels reduced, observed none) and `DDFSTRT-EARLIER-THAN-0038-COSTS-SOME-SPRITES-FETCH-CYCLES` (expected at least one channel disabled, with no SPRxDATA or SPRxDATB fetch on any line where the bitplane data fetch runs, at each of $0030, $0028, $0020 and $0018; observed 0 of 8 disabled at every one of the four). Their siblings at 2, 4 and 6 lores bitplanes, `DDFSTRT-0030-SPRITE-7-AT-MULTIPLE-BITPLANES` and `DDFSTRT-0030-SOME-SPRITE-AFFECTED-AT-MULTIPLE-BITPLANES`, are green in the same run; the package reports `30 passed, 3 failed` |
| **Verdict of the four** | **3–1 for p. 82's sentence.** WinUAE, vAmiga and Denise stop sprite DMA once the bitplane fetch has started, at any plane count; Copperline keeps sprite 7 at $0030 at any plane count and asserts it in its own test. NanoMig is the one whose answer depends on the plane count — and Figure 6-9, read by plane with its own footnote, predicts NanoMig's counts slot for slot. Hardware: not measured |
| **Effort** | S — two lines ported from TC64, one in `agnus_bitplanedma.v` and one in `agnus.v`; not run here |

---

## Source requirement

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

None of the four sentences names a number of bitplanes; p. 82 names the value,
$0030, and the sprite, 7.

**Figure 6-9 on p. 195 does, slot by slot.** *Read from the drawing, not
quoted:* one 320-mode fetch unit of eight colour clocks carries planes 4, 6 and
2 in its second to fourth slots and planes 3, 5 and 1 in its sixth to eighth,
the first and fifth being free. The unit that starts one word early, at $30, is
drawn over sprite 7's two slots, which fall on its planes 4 and 2; the unit
before it, from $28, falls on sprite 5's two slots (planes 4 and 2) and sprite
6's (planes 3 and 1). The figure's note says that in this case the bit-plane
DMA steals the cycles normally allocated to the sprites. The figure's legend marks the bitplane
slots *"320 mode Bit-Plane DMA, by plane"* with a footnote sign, and the
footnote, read from the image, reads:

> These operations only take slots if the associated operation is being
> performed.

**Printed p. 196** counts bitplane slots the same way, plane by plane:

> If the display contains four or fewer low resolution bitplanes, the 68000
> can be granted alternate memory cycles (if it is ready to ask for the cycle
> and is the highest priority item at the time). However, if there are more
> than four bitplanes, bitplane DMA will begin to steal cycles from the 68000
> during the display.

**So the manual gives two answers for one bitplane.** Read by plane, the figure
has a single lores plane at DDFSTRT = $0030 take only the unit's eighth slot,
which is no sprite's, so no sprite is disabled; at $0028 it takes sprite 6's
second slot, at $0020 sprite 4's and 6's, at $0018 sprite 2's, 4's and 6's. A
second plane adds plane 2's slot, which in the unit at $30 is sprite 7's
second; a fourth adds plane 4's, sprite 7's first. pp. 80, 82 and 83 disable
sprites at an early fetch start with no plane count at all.

## RTL and measurement

**NanoMig takes sprite 7's fetches at DDFSTRT = $0030 only where a fetched plane
needs the same cycle.** The sprite request is gated on SPREN and nothing else
([`agnus.v#L125`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L125)),
the priority chain puts bitplane ahead of sprite for a cycle both request
([`#L207`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L207),
[`#L219`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L219)),
and the bitplane request needs `ddfrun` and `plane < bpu`
([`agnus_bitplanedma.v#L472`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_bitplanedma.v#L472)),
so a cycle whose plane is not fetched goes to the sprite engine, whose window
opens at `8'h18` and closes at `8'h38` on `hpos` alone
([`agnus_spritedma.v#L263-L272`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_spritedma.v#L263-L272)).
`ddfrun` leaves `agnus_bitplanedma` by no other path. One lores plane is
fetched in one cycle of eight, the last
([`agnus_bitplanedma.v#L463`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_bitplanedma.v#L463));
each further plane takes another.

The fixture is the Agnus top level in the OCS configuration. All eight sprites
are armed identically (VSTART 40, VSTOP 220), SPREN, BPLEN and DMAEN are set,
DIWSTRT is `$2C81`, DIWSTOP `$2CC1`, DDFSTOP `$00D0`, and each sprite's SPRxDATA
and SPRxDATB fetches are counted over 90 lines. At DDFSTRT = $0038 every sprite
counts **172** in every row below.

| BPLCON0 | planes | sprite 7 at DDFSTRT = $0030 | sprites 0–6 at $0030 | graded |
|---|---|---:|---|---|
| `$1200` | 1 lores | **172** | 172 each | **red**, `DDFSTRT-0030-SPRITE-7-AT-ONE-BITPLANE` and `DDFSTRT-0030-SOME-SPRITE-AFFECTED-AT-ONE-BITPLANE` |
| `$2200`, `$3200` | 2 or 3 lores | **90** | 172 each | `$2200` green, in both `-AT-MULTIPLE-BITPLANES` siblings |
| `$4200`, `$5200`, `$6200` | 4, 5 or 6 lores | **8** | 172 each | `$4200` and `$6200` green, in the same two |
| `$9200` | 1 hires | **90** | 172 each | measured only |
| `$A200`, `$C200` | 2 or 4 hires | **8** | 172 each | measured only |

The package prints, for the p. 80 assertion:

```text
FAIL: DDFSTRT-EARLIER-THAN-0038-COSTS-SOME-SPRITES-FETCH-CYCLES: ... (172 events/channel over 90 lines, 164 of them on the 82 lines where the bitplane data fetch runs) at the DDFSTRT=$38 baseline. A sprite is disabled when its channel makes no such fetch on those bitplane lines. Swept across $0030/$0028/$0020/$0018: $0030: 0 of 8 disabled (none), fetches on the 82 bitplane lines per channel 164/164/164/164/164/164/164/164; $0028: 0 of 8 disabled (none), fetches on the 82 bitplane lines per channel 164/164/164/164/164/164/82/164; $0020: 0 of 8 disabled (none), fetches on the 82 bitplane lines per channel 164/164/164/164/82/164/82/164; $0018: 0 of 8 disabled (none), fetches on the 82 bitplane lines per channel 164/164/82/164/82/164/82/164; ...
```

The p. 80 assertion `DDFSTRT-EARLIER-THAN-0038-COSTS-SOME-SPRITES-FETCH-CYCLES`
sweeps the fetch start with one lores plane and grades a sprite as disabled
when its channel makes no SPRxDATA or SPRxDATB fetch on any line where the
bitplane data fetch runs; it asks for at least one disabled channel at each of
$0030, $0028, $0020 and $0018. It is red at all four points, with 0 of 8
channels disabled at each. At the $0038 baseline every channel makes 172
fetches over the 90 lines, 164 of them on the 82 lines where the bitplane data
fetch runs. On those 82 lines the channels keep 164 each at $0030; at $0028
channel 6 keeps 82; at $0020 channels 4 and 6 keep 82; at $0018 channels 2, 4
and 6 keep 82, and none keeps 0. A channel that loses half its fetches on those
lines is reduced and not disabled by that criterion. The package prints the
disabled channels and the fetches per channel on the bitplane lines at each
point; the rows it does not grade come from the same harness
printing all eight counts for every BPLCON0 above and every DDFSTRT in the sweep
and at $0038, a measurement for this page, not a graded assertion. **Every one
of those forty-five rows of eight counts is what Figure 6-9 predicts when its
bitplane slots are read by plane**, a sprite keeping 172 when no fetched plane
lands on its slots, 90 when one does and 8 when both do.

## Risks and unknowns

- **Does p. 82 cover a one-plane display?** The manual answers both ways. The
  sentence names no plane count; Figure 6-9, whose slots are drawn by plane and
  taken only if the operation is performed, leaves every sprite whole with one
  lores plane at $0030, and predicts every count NanoMig produces, the three
  reds' zeros included. The package grades the sentence with one lores plane, the
  one case in which the two readings differ at $0030; which describes the
  silicon is a reading question, not a measurement. The finding rests on p. 82's
  sentence and three of four implementations.
- **$0030 or $0028.** Copperline disables sprite 7 only when the fetch starts
  before $30, which contradicts p. 82's worked value directly.
- **OCS and ECS.** WinUAE, vAmiga and TC64 all block sprite DMA one cycle earlier
  on OCS. The manual does not distinguish the two here, and the fixture runs as
  OCS.
- Whether the three reds turn green with the change under *Decision* has not been
  run. Whether real Agnus blocks a sprite slot that no fetched plane uses is
  not measured; WinUAE, vAmiga, Denise and TC64 say it does.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:9612`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9612), [`:9504`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9504), [`:9241-9243`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9241-L9243) | a sprite slot is generated only while the bitplane sequencer is not running, `if (bprun != 1 \|\| bplconflict) {`; on OCS `bprun` starts at the DDFSTRT match from the DMA enable and the display window alone, `if (!ddf_limit_out && ddfstrt_match && !bprun && dma && diw) {`. Neither consults BPLCON0's plane count, so **1, 2, 4 and 6 planes all block sprite 7**. On OCS a decided sprite slot is also cleared once `bprun` is set, under the comment *"OCS off by one bitplane sprite cycle stealing bug"* |
| **vAmiga** | [`Core/Components/Agnus/Agnus.cpp:824-830`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L824-L830), [`Sequencer/SequencerBpl.cpp:248`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Sequencer/SequencerBpl.cpp#L248), [`Sequencer/SequencerDas.cpp:57-58`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Sequencer/SequencerDas.cpp#L57-L58) | `spriteCycleIsBlocked()` blocks every sprite slot from the first cycle the sequencer runs, `if (state.bprun) bprunUp = std::min(bprunUp, j);`, one cycle earlier on OCS (`bprunUp <= pos.h + 1`); the run state does not depend on the plane count, so **1, 2, 4 and 6 planes all block sprite 7**, whose slots are `p[0x31] = DAS_S7_1;` and `p[0x33] = DAS_S7_2;` |
| **Denise** | [`emulation/libami/agnus/graphics.cpp:342-345`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/graphics.cpp#L342-L345), [`:407`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/graphics.cpp#L407), [`agnus.h:324`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.h#L324) | starting the bitplane fetch sets `sprInhibited = true`, and a sprite fetch is queued only `if (!sprInhibited)`; `useBitplaneDMA()` is `(dmaConImm & 0x300) == 0x300`, BPLEN and DMAEN, so **1, 2, 4 and 6 planes all block sprite 7**, whose two slots are decided at `case 0x33` and `case 0x35` ([`agnus.cpp:600-601`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L600-L601)). The ECS path sets the same flag at `:306` |
| **Copperline** | [`src/chipset/agnus.rs:529-560`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L529-L560), test [`:1810-1829`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L1810-L1829), [`src/bus/dma_slots.rs:849-854`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L849-L854) | **keeps sprite 7 at $0030 with 1, 2, 4 or 6 planes**: `let sprite7_block_start = 0x0030;` and the fetch has to start *before* $30 to disable it; the plane count only has to be non-zero, and no other sprite is ever disabled this way (`if sprite != 7 \|\| ...`). Its own test `early_bitplane_ddfstart_disables_only_sprite_seven_dma` asserts that $0030 leaves sprite 7 on and $0028 turns it off, with one lores plane. On the bus a sprite slot is resolved before a bitplane slot |
| **MiSTer** | [`rtl/agnus.v:123`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus.v#L123), [`rtl/agnus_bitplanedma.v:472`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_bitplanedma.v#L472) | descendant — identical to NanoMig: `wire dma_spr = req_spr & spren;`, the same `dma_bpl` before `dma_spr` priority at `:170`/`:181`, and the same per-plane bitplane request |
| **TC64** | [`rtl/minimig/agnus.v:186`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus.v#L186), [`rtl/minimig/agnus_bitplanedma.v:390`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_bitplanedma.v#L390) | descendant — **has already changed this line**: `assign dma_spr = req_spr & spren & !(ddf & bplen); // AMR - Disallow sprite fetches during bitplane data fetch.`, with `assign ddf = ecs ? ddfrun : ddfena; // AMR - Export ddfrun/ddfena for sprite DMA logic. (Imitates OCS bug, inhibiting too soon.)`. The per-plane bitplane request at [`agnus_bitplanedma.v:448`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_bitplanedma.v#L448) is unchanged; the sprite side now sees the whole fetch, at any plane count |

WinUAE, vAmiga and Denise stop sprite DMA for as long as the bitplane fetch
runs, and none of the three asks how many planes are fetched; that disables
sprite 7 at DDFSTRT = $0030 with one plane or six, which is p. 82's sentence as
printed. Copperline's dissent is deliberate and is not NanoMig's rule either:
it disables all of sprite 7 or none of it by where the fetch starts, NanoMig
takes individual fetches by which planes are fetched. They agree only at one
lores plane, where the three reds are; with two or more planes NanoMig sides
with the other three and against Copperline.

### What practitioners report

Reports from the forum thread [EAB 19676](https://eab.abime.net/showthread.php?t=19676): witnesses, never an oracle ([practitioner reports](../evidence/practitioner-reports.md)).

**SUPPORTS.** [EAB post 935951](https://eab.abime.net/showpost.php?p=935951), Toni Wilen, 2014-02-02. Chip set: not stated; the discussion is about OCS and ECS Agnus. Known from: "I tested it"; the set-up is not stated.

> "Unfortunately "empty" bitplane slots still count, I tested it, number of planes makes no difference."

This answers a poster who had one bitplane on, the one-plane case of p. 82's sentence that the page calls a reading question: the lost sprite slots do not depend on the number of planes.

**EXPLAINS.** [EAB post 935172](https://eab.abime.net/showpost.php?p=935172), Toni Wilen, 2014-01-28. Chip set: OCS against ECS Agnus. Known from: "I tested all DDFSTRT positions from $18 to $38 with OCS and ECS Agnus"; the machines are not stated.

> "OCS Agnus is always "off by one", DMA slot at DDFSTRT-1 is never used for sprite DMA if OCS"

This is the OCS-versus-ECS difference of one slot behind the OCS blocking one cycle earlier that the Risks give for WinUAE, vAmiga and TC64.

## Decision

**Question:** does p. 82's "this will disable sprite 7" at DDFSTRT = $0030 hold
with one lores plane? **Suggested change: port TC64's two lines.** minimig-dev's
TC64 line commit `fe2db71`, *"Sprite fetch now completely blocked once DDF
starts, instead of just when bitplanes actually want data"*, touches
`rtl/minimig/agnus.v` and `rtl/minimig/agnus_bitplanedma.v` and nothing else;
MiSTer has not taken it and NanoMig has the MiSTer form. In `agnus_bitplanedma.v`,
export the fetch state:

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
`ddfrun` and `ddfena` in `agnus_bitplanedma.v`; the declaration of `bplen` has
to come before its first use. That would make sprite 7's fate at $0030
independent of the plane count, and move it away from Figure 6-9's by-plane
counts with one to three lores planes or one hires plane, which is the other
half of the decision.

**NanoMig pull request.** [#181](https://github.com/MiSTle-Dev/NanoMig/pull/181), commit [`5e6d8de`](https://github.com/MiSTle-Dev/NanoMig/commit/5e6d8deef7b5299f9267d168ab6c9846da16d24b), blocks sprite DMA while the bitplane data fetch runs: the sprite request is dropped when `ddf` and the bitplane DMA enable are both set, with `ddf` taken from `ddfrun` on ECS and from `ddfena` on OCS, as in TC64's `fe2db71`. Merged on 2026-10-23 as [`113bcda`](https://github.com/MiSTle-Dev/NanoMig/commit/113bcda15e916739deebac429e075aea92d8c7ec). This suite pins NanoMig at `4315acb`, which is before the merge, so every NanoMig line this page cites is as it was before the change: the RTL section, the Effort row and the Decision describe NanoMig at `4315acb`. At the merge (113bcda) the sprite grant is gated at line 127 of agnus.v by the term & !(ddf & bplen), and ddf leaves agnus_bitplanedma.v at line 34.
