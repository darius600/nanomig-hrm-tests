# NM-HRM-008 — playfield-1 collision depends on playfield 2 in single-playfield mode, where pp. 214 and 273 name no mode

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this; the red is against pp. 214 and 273, not against the RTL. Question: do pp. 214 and 273 need a single-playfield condition, so that CLXDAT bits 1 to 4 (playfield 1 to sprite pairs) stay clear while BPLCON0's DPF bit is clear and an enabled even bitplane misses its `CLXCON` match value, as NanoMig registers them? Closes when an Amiga specialist records that condition on p. 214's sentence and p. 273's playfield-1 rows, or confirms that the pages hold as printed, in which case this page is refiled against the RTL. |
| **Report to** | `no RTL action` — HRM documentation: whoever keeps errata for Chapter 7 and Appendix A of the *Amiga Hardware Reference Manual*, 3rd ed. |
| **Scope** | CLXDAT bits 1 to 4 in single-playfield mode (BPLCON0 DPF clear), measured on `denise_collision.v` at the pinned revision with `CLXCON` enabling bitplane 1 (matched) and bitplane 2 (unmatched); the pages name no chip revision; the silicon reading is the ECS Denise 8373R4 only. The gate does not show where playfield 2 matches too (`BITMAP-PAIR PF1-PF2`) or where no even plane is enabled (`PLAYFIELD-ODD-EVEN`) |
| **Software impact** | named only by the sources, not measured here: the RTL comment names Rotor, Spaceport (helicopter crash at takeoff) and Archon-1 ('sticky effect' of the player sprite); WinUAE's changelog entry on p. 214's sentence `Fixes Inferior and Sub Rally`; the Rotor attribution does not match WinUAE's changelog (*Risks and unknowns*) |
| **Chapter** | 7 — System Control, printed pp. 210, 213 and 214 (Table 7-3); Appendix A, printed pp. 272 and 273 (`CLXCON`, `CLXDAT`) · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims by assertion: `A1-PLAYFIELD-MODE-INVARIANCE` (p. 214) `sol:7-collision.c27`, `opus:7-cd.c26`; `A3-PLAYFIELD-1-IS-ODD-BITPLANES` (p. 273) `sol:7-appA-clxdat.c6`, `opus:7-appA-clxdat.c3`; `BITMAP-PAIR PF1-SPR01` (pp. 214, 273) `opus:A-clxdat.b01`, `sol:A-clxdat.c20`, `sol:7-collision.c24`, `sol:7-appA-clxdat.c22`, `opus:7-cd.c22`, `opus:7-appA-clxdat.c19`; `PF1-SPR23` (pp. 214, 273) `opus:A-clxdat.b02`, `sol:A-clxdat.c19`, `sol:7-collision.c23`, `sol:7-appA-clxdat.c21`, `opus:7-cd.c21`, `opus:7-appA-clxdat.c18`; `PF1-SPR45` (pp. 214, 273) `opus:A-clxdat.b03`, `sol:A-clxdat.c18`, `sol:7-collision.c22`, `sol:7-appA-clxdat.c20`, `opus:7-cd.c20`, `opus:7-appA-clxdat.c17`; `PF1-SPR67` (pp. 214, 273) `opus:A-clxdat.b04`, `sol:A-clxdat.c17`, `sol:7-collision.c21`, `sol:7-appA-clxdat.c19`, `opus:7-cd.c19`, `opus:7-appA-clxdat.c16`; all nine reds carry claim ids. Green assertions the page names: `BITMAP-PAIR PF1-PF2` (pp. 214, 273) `opus:A-clxdat.b00`, `sol:A-clxdat.c21`, `sol:7-collision.c25`, `sol:7-appA-clxdat.c23`, `opus:7-cd.c23`, `opus:7-appA-clxdat.c20`; `PLAYFIELD-ODD-EVEN` (p. 273) `opus:A-clxdat.note1`, `opus:A-clxdat.note2`, `sol:A-clxdat.c4`, `sol:A-clxdat.c5`; `A2-BITPLANE-CONTENT-DETERMINES-COLLISION` (p. 214) `sol:7-collision.c28`, `opus:7-cd.c27`; `A4-PLAYFIELD-2-IS-EVEN-BITPLANES` (p. 273) `sol:7-appA-clxdat.c7`, `opus:7-appA-clxdat.c4` |
| **Module** | `denise_collision.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_collision.v#L62-L63) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_collision.v#L62-L63); inherited (identical in MiSTer and TC64) |
| **Test** | [`hrm/ch3-collision-registers`](../../hrm/ch3-collision-registers): `make -C hrm/ch3-collision-registers test` · [`hrm/ch7-collision-playfield-mode`](../../hrm/ch7-collision-playfield-mode): `make -C hrm/ch7-collision-playfield-mode test`; an expected red exits nonzero |
| **Assertions** | 9 red. `hrm/ch3-collision-registers` (`32 passed, 4 failed`): `BITMAP-PAIR PF1-SPR01`, `PF1-SPR23`, `PF1-SPR45`, `PF1-SPR67` — expected exactly CLXDAT bit 1, 2, 3 or 4 set (`$0002`, `$0004`, `$0008`, `$0010`); the FAIL lines print no observed value, and the same stimulus reads `$0000` at `dblpf=0` in `A1-PLAYFIELD-MODE-INVARIANCE`. `hrm/ch7-collision-playfield-mode` (`16 passed, 5 failed`): `A1-PLAYFIELD-MODE-INVARIANCE PF1-SPR01`, `PF1-SPR23`, `PF1-SPR45`, `PF1-SPR67` — expected `CLXDAT & 0x7FFF` at `dblpf=0` equal to that at `dblpf=1`, observed `$0000` against `$0002`, `$0004`, `$0008`, `$0010`; `A3-PLAYFIELD-1-IS-ODD-BITPLANES dblpf=0` — expected only-BP1-matched to set bit 1, observed `$0000` |
| **Verdict of the four** | **unanimous with NanoMig, against pp. 214 and 273**: WinUAE, vAmiga, Denise and Copperline all register a playfield-1 collision in single-playfield mode only where playfield 2's enabled planes match as well. Copperline's expression is NanoMig's term for term; WinUAE's changelog calls p. 214's sentence wrong and vAmiga's comment calls the gate a hardware oddity; vAmiga also applies it in dual-playfield mode where playfield 1 is opaque and shown ([`Denise.h:494-499`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.h#L494-L499)); outside the emulators, the 8373R4 dissection gates the playfield-1 match on the even planes when BPLCON0's DPF bit is clear, as NanoMig does — see *What other Commodore documents say*. Hardware: not measured |
| **Effort** | none — no RTL change; the correction is a single-playfield condition on p. 214's sentence and p. 273's playfield-1 rows; if the pages are confirmed as printed, S: drop `(dblpf | evenmatch)` from `denise_collision.v:62` |

---

## Source requirement

**The manual states the rule three times and conditions it on no mode.**

**Chapter 7, printed p. 214**, below Table 7-3, whose bits 1 to 4 read "Odd
bitplanes to sprite 0 (or 1)" to "Odd bitplanes to sprite 6 (or 7)" and bits
5 to 8 the same for the even bitplanes:

> Notice that in this table, collision detection does not change when you
> select either single- or dual-playfield mode. Collision detection depends
> only on the actual bits present in the odd-numbered or even-numbered
> bitplanes.

**Appendix A, printed p. 273**, `CLXDAT`, the note above the bit table:

> NOTE: Playfield 1 is all odd-numbered enabled bitplanes. Playfield 2 is all
> even-numbered enabled bitplanes

The table under it gives each pair its own bit: 01 to 04 "Playfield 1 to
sprite 0 (or 1)" to "Playfield 1 to sprite 6 (or 7)", 05 to 08 the same for
playfield 2, 00 "Playfield 1 to playfield 2".

**Appendix A, printed p. 272**, `CLXCON`, says which planes take part:

> NOTE: Disabled bitplanes cannot prevent collisions. Therefore if all
> bitplanes are disabled, collisions will be continuous, regardless of the
> match values.

The pages around these do not qualify them. p. 213 introduces collisions
between "any sprite group and either of the playfields"; pp. 209-212 set
display priority, and the one sentence there that names single-playfield mode
is about priority, not collision (p. 210):

> PF2P2 - PF2P0, bits 5-3, are the priority bits for normal (non-dual)
> playfields.

No figure in chapter 7 draws the collision logic, and Appendix C does not
mention collision. No printed page says what NanoMig does.

## RTL and measurement

[`src/minimig-aga/denise_collision.v#L62-L63`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_collision.v#L62-L63)

```verilog
assign oddmatch = bm[6] & bm[4] & bm[2] & bm[0] & (dblpf | evenmatch);
assign evenmatch = bm[7] & bm[5] & bm[3] & bm[1];
```

`dblpf` is BPLCON0 bit 10, the dual-playfield bit
([`src/minimig-aga/denise.v#L183`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L183)).
With `dblpf = 0` — single playfield — `oddmatch` additionally requires
`evenmatch`: playfield 1 registers a collision only where playfield 2's
enabled planes match as well. An even plane that `CLXCON` does not enable
counts as matched, as p. 272's note has it
([`denise_collision.v#L55`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_collision.v#L55)
ORs in the inverted enable bit), so the gate shows only when at least one
even plane is enabled and differs from its match value.

The RTL says why, in the comment directly above it:

> this is the implementation of an undocumented function in the real Denise chip,
> developed by Yaqube. trigger was the game Rotor. mentioned in WinUAE sources to
> be the only known game needing this feature. it also fixes the Spaceport
> instandly helicopter crash at takeoff and Archon-1 'sticky effect' of player
> sprite at the battlefield.

### Measured

[`hrm/ch3-collision-registers`](../../hrm/ch3-collision-registers) presents
playfield 1 with each sprite pair, `dblpf` at 0, bitplane 1 enabled and matched
for playfield 1 and bitplane 2 enabled and unmatched for playfield 2: p. 273's
table has each pair set its own bit, and NanoMig sets none —
`BITMAP-PAIR PF1-SPR01`, `PF1-SPR23`, `PF1-SPR45` and `PF1-SPR67` are red,
four of the package's `32 passed, 4 failed`. `BITMAP-PAIR PF1-PF2`, where
playfield 2 matches too, and `PLAYFIELD-ODD-EVEN`, where no even plane is
enabled, stay green.

[`hrm/ch7-collision-playfield-mode`](../../hrm/ch7-collision-playfield-mode)
runs the same `CLXCON` setting and stimuli at `dblpf` 0 and 1. `CLXDAT & 0x7FFF` reads `$0000` in
single-playfield mode and `$0002`, `$0004`, `$0008` and `$0010` in
dual-playfield mode for the four playfield-1 pairs, so
`A1-PLAYFIELD-MODE-INVARIANCE PF1-SPR01` to `PF1-SPR67` are red against p. 214,
and `A3-PLAYFIELD-1-IS-ODD-BITPLANES dblpf=0` is red against p. 273 —
`16 passed, 5 failed`. The other eleven pairs of `A1`, `A2`, `A3` at `dblpf=1`
and `A4` in both modes are green.

Both packages can go green: with `(dblpf | evenmatch)` removed from line 62
and nothing else changed, in a modified copy of the RTL that `make test` does
not run, they read `36 passed, 0 failed` and `21 passed, 0 failed`. The four
other packages that build `denise_collision.v` assert nothing about the gate;
they are green at the pinned revision and stayed green in that copy.

## Risks and unknowns

- **One revision of silicon.** The 8373R4 dissection is of the ECS Denise, a work in
  progress; the original chip set's Denise is not dissected, and for it the four
  implementations, none of which conditions the gate on the chip set, are the only
  witnesses. The gate and the DPF latch are traced transistor by transistor (sheets
  7p and 7j); that CLXDAT bits 1 to 4 take the gated match is read from sheet 4hi,
  which draws bit 0's cell transistor by transistor and the others as blocks with
  their inputs, noting "layout of all cells is pretty similar"; the data-bus input is drawn for D0 alone (sheet 3a, where D0in
  follows D0) and D10in is taken to be alike; sheet 7n draws the match comparison
  for planes 1 and 2 only.
- **Both Commodore documents are table-level.** The 1988 ECS Denise specification
  does not carry the chapter-7 sentence about single versus dual playfield, and the
  patent's FIG. 12 gate diagram (PDF page 11) is illegible in the text layer; neither
  says what single-playfield mode does, so neither speaks for or against the gate.
- **No witness cites a Commodore document for the gate.** vAmiga's sprite–playfield check
  runs only under `if (config.clxSprPlf)`
  ([`Denise.cpp:1226`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L1226)),
  an option that defaults to `false`
  ([`Core/Infrastructure/Defaults.cpp:174`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Infrastructure/Defaults.cpp#L174));
  WinUAE's sprite–bitplane collisions need `currprefs.collision_level > 1`, default 3
  ([`cfgfile.cpp:8642`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L8642)).
- **The RTL comment's Rotor claim does not match its source.** WinUAE's changelog
  says Rotor "does require working CLXDAT collision bit 0 (="full collision")"
  ([`od-win32/winuaechangelog.txt:18408-18410`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/winuaechangelog.txt#L18408-L18410)),
  the playfield-1-to-playfield-2 bit, which needs both playfields in either mode;
  the single-playfield gate is a separate changelog entry (*Independent evidence*).

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`drawing.cpp:4518-4531`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L4518-L4531) | PF1 gated by PF2: with `bpldualpf` clear — BPLCON0 bit 10 as Denise holds it ([`drawing.cpp:2855`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L2855)) — the odd-plane test is reached only inside the even-plane one. Its changelog: `"collision detection does NOT change when you select either single- or dual-playfield mode" says HRM. BZZZT, WRONG!`, an entry that `Fixes Inferior and Sub Rally` ([`od-win32/winuaechangelog.txt:19144-19146`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/winuaechangelog.txt#L19144-L19146)) |
| **vAmiga** | [`Core/Components/Denise/Denise.cpp:1554-1560`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L1554-L1560) | PF1 gated by PF2: *"There is a hardware oddity in single-playfield mode. If PF2 doesn't match, PF1 doesn't match either."*, citing a forum post, then `if (!(zBuffer[pos] & Z_DPF)) continue;`. A single-playfield pixel carries no dual-playfield code ([`Denise.cpp:829`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L829)), so in single-playfield mode the test always skips PF1 when PF2 fails; it also skips PF1 in dual-playfield mode at pixels coded `Z_DPF1` and `Z_DPF12`, where PF1 is opaque and shown ([`Denise.h:494-499`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.h#L494-L499)) |
| **Denise** | [`emulation/libami/video/denise.cpp:478-480`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L478-L480) | `clxNoCol` bit 0 is the odd planes' mismatch and bit 1 the even planes' ([`denise.cpp:652-653`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L652-L653); shifter A takes ENBP1, ENBP3 and ENBP5, [`denise.cpp:226`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L226)), so the PF1 group without a PF2 match is set only through `else if (((clxNoCol & 1) == 0) && _doublePlayfield) clxDat \|= (v1 << 1);`. No comment gives a reason |
| **Copperline** | [`src/video/bitplane.rs:6797`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane.rs#L6797) | `let odd_match = odd_match_raw && (dual_playfield \|\| even_match);`, NanoMig's expression term for term, with a test named after the behaviour, `collision_pixel_single_playfield_odd_match_requires_even_match` ([`src/video/bitplane/tests.rs:6626`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/tests.rs#L6626)); the same expression is on the live `CLXDAT` read path ([`src/bus.rs:12281`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L12281)), and the comment on the plane match beside it names vAmiga's `checkS2PCollisions` as its reference ([`src/video/bitplane.rs:6812-6817`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane.rs#L6812-L6817)) |
| **MiSTer** | [`rtl/denise_collision.v:62-63`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_collision.v#L62-L63) | descendant — byte-identical |
| **TC64** | [`rtl/minimig/denise_collision.v:62-63`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise_collision.v#L62-L63) | descendant — the same two lines, unchanged; the whole file is identical on both lines |

### What other Commodore documents say

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 7 — agrees with the manual:

  > "NOTE: Playfield 1 is all odd numbered enabled bit planes."
  <!-- corpus: denise-spec-252126/p007.txt -->

  Its CLXDAT table lists bits 01-04 as 'Playfield 1 to Sprite 0 (or 1)' and so on with no mode condition; nothing in the 14 pages makes the PF1 bit depend on PF2 or on DPF.

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga, filed 1986, granted 1989 and 1992 — one shared specification, counted once**, PDF page 29 — agrees with the manual:

  > "Fifteen bits in the collision storage register are utilized to indicate whether any of several types of overlaps has occurred: e.g., one sprite to another sprite, even bitplanes to odd bitplanes, even bitplanes to a certain sprite, or odd bitplanes to a certain sprite."
  <!-- corpus: patent-us4874164-agnus-denise/p029.txt -->

  It describes the odd-bitplanes-to-sprite bit as an overlap of the odd bitplanes with a sprite, with no playfield-mode condition; p021 likewise defines objects as 'any non-zero bits used to display any of eight sprites or two playfields'.

- **6502.org dissection of Denise 8373R4, undated — third-party, the silicon of one revision**, schematic sheets 7p, 7j, 7n, 4hi and 3a — **agrees with NanoMig**:

  *Read from the drawing, not quoted:* sheet 7p, drawn transistor by transistor, forms one term per bitplane that is high when CLXCON enables the plane and the plane's bit differs from its match value (a node pulled low by ENBPn# or by MVBPn#). One pull-down network takes the terms of planes 1, 3 and 5, a second all six; a pass switch driven by DPF (through an inverter from DPF#) selects the first, one driven by DPF# the second, and the result, latched on C7M1, drives MBPB#. MBPA# takes the terms of planes 2, 4 and 6 alone. DPF is BPLCON0 bit 10 as written, latched from D10in on sheet 7j, and DPF# its complement; each MVBPn# compares a CLXCON match bit with the plane's shifter output, Bn_Q15 (sheet 7n). On sheet 4hi the cells of CLXDAT bits 1 to 4 take MBPB# with the sprite-pair signals S10C# to S76C#, the cells of bits 5 to 8 take MBPA#, and bit 0 takes both.
  <!-- drawing: denise-8373r4-dissection/sheet_053.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_049.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_051.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_017.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_006.txt -->

  With DPF clear, in single-playfield mode, the odd-plane match that CLXDAT bits 1 to 4 record fails wherever an enabled even plane misses its match value; with DPF set the even planes drop out. That is NanoMig's playfield-2 term on line 62, and not what pp. 214 and 273 print.

## Decision

**No RTL change.** NanoMig already gates playfield 1 on playfield 2 in
single-playfield mode, as all four independent implementations and the 8373R4
silicon do. Dropping the playfield-2 term from line 62,

```verilog
assign oddmatch = bm[6] & bm[4] & bm[2] & bm[0];
```

would satisfy pp. 214 and 273: it turns all nine reds green and leaves every
other assertion in the six packages that build `denise_collision.v` green
(*Measured*). It would also make NanoMig the only implementation read
here that registers a playfield-1 collision without playfield 2 in
single-playfield mode, depart from the silicon, and remove the behaviour the
RTL comment ties to Rotor, Spaceport and Archon-1 and WinUAE's changelog to
Inferior and Sub Rally.

The change, if one is made, belongs to the manual: a single-playfield condition
on p. 214's sentence and on p. 273's playfield-1 rows. The page closes as row 4
says.
