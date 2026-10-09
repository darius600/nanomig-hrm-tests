# Specification: chapter 7 system control — display priority, the light-pen read at the beam-position address, BEAMCON0

This specification was written from the package's claims (`claims-spec-stage.jsonl`) and the pages of the *Amiga Hardware Reference Manual*, 3rd edition, by an author who never saw the implementation or any test. Every expected value below comes from a quoted page; where the pages leave a value open, the specification says so and grades nothing there. Page numbers are printed page numbers.

## Claims that give no proposition

| Claim | Page | Reason |
|---|---|---|
| `opus:7-reset.c3` | 223 | The page makes the outcome model-dependent: "On some Amiga models, portions of the RAM still respond. On other models, no RAM responds. Software must assume that memory is not available." Both outcomes conform, so no expected value exists and a test must not invent one. |
| `sol:7-reset.c6` | 223 | Same sentence, first half. It says that *some* models behave this way. One system cannot confirm or refute a statement about some models, so there is no expected value. |
| `sol:7-reset.c7` | 223 | Same sentence, second half. It says that *other* models behave this way. As for c6, there is no expected value. |
| `opus:7-appC-beamcon0.c7` | 305 | BEAMCON0 bit 10 CSCBEN "Composite sync redirection". The page gives only this name. It says neither where composite sync is redirected to nor from where, and gives no pin, polarity or timing. Appendix J (pp. 379, 381) lists composite sync on a pin of its own (CSY\*), separate from HSY\* and VSY\*, and a test sees only horizontal and vertical sync. There is no stated effect and no observable output, so the claim cannot be checked. |
| `sol:7-appC-beamcon0.c7` | 305 | Same row of the same table, so the same reason applies. |

No claim in this package needs a disk image.

## Claims and the propositions they support

| Proposition | Claims | Page |
|---|---|---|
| P1: at every pixel, the colour of the highest-priority object is displayed | `opus:7-cd.c3`, `sol:7-collision.c5` | 213 |
| P2: bitplane and sprite data are combined into one video output | `opus:7-cd.c2` | 213 |
| P3: with a light pen in use, the beam-position address returns the light-pen position, not the beam position | `opus:7-bp.c3`, `sol:7-beam.c3`, `opus:7-bp.c11` | 216 |
| P4 (ECS): with LPENDIS set, a vertical-position read ignores the latched pen value | `opus:7-appC-beamcon0.c4`, `sol:7-appC-beamcon0.c4` | 305 |

---

## Common conventions

### Run matrix

Each proposition runs on both 68000 cores, one at a time. P1, P2 and P3 run under OCS and under ECS behaviour, and in PAL (required) and NTSC. P4 needs ECS, because BEAMCON0 is marked `A(E)` (ECS Agnus) on p. 293 and p. 266 and described as "A new register in the enhanced Agnus" on p. 305. P4 runs in PAL; NTSC is optional (see P4).

### Memory placement

The program keeps every Copper list, bitplane, sprite structure and result buffer below `$80000`. Under OCS the pointer registers hold "high 3 bits" (p. 25, p. 110), and p. 309 widens them only for ECS. Each data structure is word aligned, as p. 104 requires.

### Register addresses (Appendix B, pp. 287–293)

VPOSR `$004`, VHPOSR `$006`, INTENAR `$01C`, INTREQR `$01E`, COP1LCH `$080`, COPJMP1 `$088`, DIWSTRT `$08E`, DIWSTOP `$090`, DDFSTRT `$092`, DDFSTOP `$094`, DMACON `$096`, INTENA `$09A`, INTREQ `$09C`, BPLxPTH `$0E0 + 4(x−1)` (PTL at +2), BPLCON0 `$100`, BPLCON1 `$102`, BPLCON2 `$104`, BPL1MOD `$108`, BPL2MOD `$10A`, SPRxPTH `$120 + 4x` (PTL at +2), COLORnn `$180 + 2nn`, BEAMCON0 `$1DC`. All are offsets from `$DFF000`. CIA-A DDRA is at `$BFE201` (p. 339).

### Bit values used

- DMACON (p. 222, p. 278): bit 15 SET/CLR, 9 DMAEN, 8 BPLEN, 7 COPEN, 5 SPREN. The write `$7FFF` clears all enables. `$83A0` sets DMAEN, BPLEN, COPEN and SPREN. `$8280` sets DMAEN and COPEN.
- INTENA and INTREQ use the SET/CLR rule on p. 218: "When you wish to set a bit … a 1 into position 15 … to clear a bit … a 0 into position 15." `$7FFF` clears all. Writing `$0020` to INTREQ clears VERTB and writing `$0010` clears COPER. The Copper writes `$8010` to set COPER. The Copper may write INTREQ because "The registers that the Copper can always affect are numbered $80 through $FF inclusive" (p. 26).
- BPLCON0 (p. 48, p. 271): BPU in bits 14–12, bit 10 DBLPF, bit 9 COLOR, bit 3 LPEN. "The bits in the BPLCON0 register cannot be set independently" (p. 48), so every write is the full value. `$2600` is two lores planes in dual playfield. `$6600` is six lores planes in dual playfield. `$1200` is one lores plane in single playfield. `$0008` sets LPEN with no bitplanes. On ECS, bit 0 (ENBPLCN3, p. 307) stays 0.
- BPLCON2 (p. 210, Table 7-1): bit 6 PF2PRI, bits 5–3 PF2P, bits 2–0 PF1P, bits 15–7 "Not used (keep at 0)". The value is `PF2PRI<<6 | PF2P<<3 | PF1P`. On ECS the extra bits 14–9 (p. 307) stay 0.
- Copper WAIT (p. 22): the first word is `V<<8 | (H & $FE) | 1` and the second is `$FFFE`. A MOVE is `register offset, data` (p. 21). The list ends with `$FFFF,$FFFE` (p. 23, p. 29).

### Palette

The CPU writes the palette once, before DMA is enabled. Each value is 12 bits, with red in bits 11–8, green in 7–4 and blue in 3–0 (p. 46, Table 3-3). The video output's 4-bit R, G and B must equal these three nibbles.

| Register | Value | Role |
|---|---|---|
| COLOR00 | `$000` | background, BG (p. 46) |
| COLOR01 | `$F00` | PF1 value 1 (dual) or pixel value 1 (single); this colour is called PF1C |
| COLOR02 | `$A00` | PF1 value 2 (plane 3 alone, P2 only) |
| COLOR04 | `$500` | PF1 value 4 (plane 5 alone, P2 only) |
| COLOR08 | `$333` | PF2 "transparent" register (p. 67); must never appear |
| COLOR09 | `$0F0` | PF2 value 1; this colour is called PF2C |
| COLOR10 | `$0A0` | PF2 value 2 (plane 4 alone, P2 only) |
| COLOR12 | `$050` | PF2 value 4 (plane 6 alone, P2 only) |
| COLOR03, 05–07, 11, 13–15 | `$222` | not selected by any stimulus; must never appear |
| COLOR16, 20, 24, 28 | `$444` | "ignored by sprites" (p. 103); must never appear |
| COLOR19 | `$FF0` | SC0, the colour of sprite 0 (value 11) |
| COLOR17 = COLOR18 | `$F0F` | SC1, the colour of sprite 1 (value 01) |
| COLOR23 | `$0FF` | SC2, the colour of sprite 2 (value 11) |
| COLOR21 = COLOR22 | `$F80` | SC3, the colour of sprite 3 (value 01) |
| COLOR27 | `$08F` | SC4, the colour of sprite 4 (value 11) |
| COLOR25 = COLOR26 | `$8F0` | SC5, the colour of sprite 5 (value 01) |
| COLOR31 | `$FFF` | SC6, the colour of sprite 6 (value 11) |
| COLOR29 = COLOR30 | `$888` | SC7, the colour of sprite 7 (value 01) |

Even-numbered sprites draw every pixel with value 11: DATA (low word) `$FFFF` and DATB (high word) `$FFFF`. Odd-numbered sprites draw value 01: low `$FFFF`, high `$0000`. The register pairs 17/18, 21/22, 25/26 and 29/30 hold equal values, so the expected colour does not depend on which data word is the low digit. pp. 105 and 108 place the low word first and p. 126 stores the first word in SPRxDATA, but this specification relies on neither. Sprite colour registers follow Table 4-6 (p. 130) and Figure 4-6 (p. 103). The ATTACH bit (SPRxCTL bit 7, p. 128) is 0 for every sprite.

### Sprite data structures (pp. 105–108, 126–128)

Each sprite has its own block: SPRxPOS = `VSTART[7:0]<<8 | X[8:1]` and SPRxCTL = `VSTOP[7:0]<<8 | VSTART[8]<<2 | VSTOP[8]<<1 | X[0]`. Then come (VSTOP − VSTART) pairs of data words, then `$0000,$0000`. VSTOP is "the line AFTER the last displayed row" (p. 107). Every one of the eight pointers is written in every field, to a real block or to a null block of two zero words: "all eight sprite pointers must be initialized to either a real sprite or a safe null sprite" (p. 114).

### Playfield geometry and row tags (P1, P2)

The window is DIWSTRT `$2C81`, DIWSTOP `$2CC1` for PAL or `$F4C1` for NTSC (p. 59), DDFSTRT `$0038`, DDFSTOP `$00D0` (p. 60), BPL1MOD = BPL2MOD = 0 and BPLCON1 = 0. Each bitplane image is 320 × 256 lores pixels at 40 bytes per row, and row *y* is displayed on beam line 44 + *y*. p. 58 places the window top at `$2C`, and p. 100 uses line 44 as the top of the display window for sprites, so vertical sprite positions and bitmap rows share one frame. All content is lores. Sprites are lores too: "The shift occurs once during each low resolution pixel time" (p. 124). Every lores pixel therefore keeps at least one visible output sample through the line doubler.

Rows are identified from the video output by a tag drawn in bitplane 1 only, never under a sprite:

- x = 0–7 (block 0): always 1. It marks the left edge of the display window in the output, and its width in samples gives *k*, the number of output samples per lores pixel.
- x = 8–39 (blocks 1–4): the band id in binary, most significant bit in block 1. A block is 1 (all eight pixels set) or 0.
- x ≥ 40: content.

In both modes used here, plane 1 alone selects COLOR01 (Table 3-10 p. 70 for dual playfield, p. 42 for single playfield). A tag block therefore reads as PF1C or BG. Rows with id 0 carry block 0 only.

The pages disagree about the horizontal sprite frame. p. 99 uses "a window with horizontal positions from pixel 64 to pixel 383" and adds 64 to X, while p. 57 and p. 59 put the window start at `$81` (129). No stimulus depends on how sprite X maps onto bitmap x. Sprite positions lie at X 170–326, which is inside the window under either frame and clear of the tag region (x < 40) and of the P2 plane blocks (x ≥ 272) under either frame. The test locates sprites in the output instead of computing where they should be.

### Program skeleton (P1, P2)

1. Write INTENA `$7FFF`, INTREQ `$7FFF` and DMACON `$7FFF`.
2. Build the images, sprite blocks, null block and one Copper list per configuration.
3. Write the palette, DIWSTRT, DIWSTOP, DDFSTRT, DDFSTOP, the modulos and BPLCON1.
4. Clear VERTB (INTREQ `$0020`) and poll INTREQR bit 5 until it is set. VERTB "causes an interrupt at line 0" (p. 219).
5. Write COP1LC with the list of the first configuration (a long write to COP1LCH, p. 25) and strobe COPJMP1. Only then write DMACON `$83A0`. p. 30 prescribes this order: "you must initialize one of the Copper location registers … and write to its strobe address before Copper DMA is turned on."
6. For each configuration *k* in turn:
   - wait for VERTB;
   - write COP1LC with list *k*;
   - store the marker word `$C000 + k`;
   - stay for three fields.

   At every VERTB, store a tick word (the field counter). The ticks give field boundaries in memory-port time.
7. Store `$CFFF` and loop.

Each Copper list writes BPLCON0, BPLCON2, all BPLxPT used, and all eight SPRxPT, then ends with `$FFFF,$FFFE`.

The configuration is evaluated in its **second** field, the field that begins at the tick after marker *k*. p. 25 says the Copper restarts from COP1LC "At the start of each vertical blanking interval" and, a sentence later, "when the end of vertical blanking occurs". p. 32 says "at the start of the vertical blanking period". The first field after a COP1LC write may therefore still run the old list, and the second cannot.

---

## P1 — At every pixel, the colour of the highest-priority object is displayed

**Proposition.** At every pixel position of the display, the output shows the colour of the highest-priority object present there, and the background colour where no object is present. The claims are `opus:7-cd.c3` and `sol:7-collision.c5`, on p. 213.

> "For each of the pixel positions on the screen, the color of the highest priority object is displayed." (p. 213)

**What "priority" and "object" mean.** The pages that define them are:

- Sprites against each other: "They will always appear on the screen with the lower-numbered sprites appearing in front of (having higher screen priority than) the higher-numbered sprites." (p. 207; also p. 115 and p. 118.)
- Sprite groups: "For playfield priority and collision purposes only, sprites are treated as four groups of two sprites each." (p. 208) Sprite *i* is in group g(i) = ⌊i/2⌋.
- Playfield against sprite groups: Table 7-2 (p. 210) gives PF1P = 000 as `PF1 SP01 SP23 SP45 SP67`, 001 as `SP01 PF1 SP23 …`, and so on up to 100 as `SP01 SP23 SP45 SP67 PF1`. So PF1 is in front of group g exactly when g ≥ PF1P. "Bits PF2P2-PF2P0 let you position playfield 2 among the sprite priorities in exactly the same way" (p. 211), so PF2 is in front of group g exactly when g ≥ PF2P.
- Playfield against playfield: "Normally, playfield 1 appears in front of playfield 2. The PF2PRI bit reverses this relationship" (p. 210). Also: "it is the PF2PRI bit that determines which of the two playfields appears in front of the other" (p. 211), and p. 71.
- Transparency: a sprite value 00 "becomes transparent and shows the color of any other sprite or play-field that has lower video priority" (p. 103). In dual playfield, "Combination 000 selects transparent mode, to show the color of whatever object (the other playfield, a sprite, or the background color) may be 'behind' the playfield" (p. 71).
- Background: "The background color shows in any area on the display where there is no other object present and is also displayed outside the defined display window" (p. 46).

A sprite is present where its pixel value is non-zero, and a dual-playfield playfield is present where its value is non-zero. Stimuli never place a sprite over a single-playfield pixel of value 0, because the pages do not say whether such a pixel is an object (see *Left open*).

### Configurations

**Group A: sprite against sprite.** BPLCON0 is `$2600` and BPLCON2 is `$0024` (both playfields behind all groups). Plane 1 carries tags only and plane 2 is all zero.

Eight slots j = 0–7 sit at X = 200 + 8j, so neighbouring slots overlap by 8 pixels and no three overlap. Even slots have VSTART 60 and VSTOP 100; odd slots have VSTART 80 and VSTOP 120.

| Lines | Tag id | Band | Content |
|---|---|---|---|
| 60–79 | 1 | E | only the even slots, which abut without overlapping |
| 80–99 | 2 | X | all eight slots |
| 100–119 | 3 | O | only the odd slots |

| Variant | Sprite in slots 0–7 |
|---|---|
| A1 | 0, 1, 2, 3, 4, 5, 6, 7 |
| A2 | 7, 0, 6, 1, 5, 2, 4, 3 |

**Group B: sprites against dual playfields.** BPLCON0 is `$2600`. Sprite *i* sits at X = 170 + 20*i*, with VSTART 60 and VSTOP 140. The sprites are disjoint from each other, with 4-pixel gaps.

| Lines | Tag id | Band | Plane 1 (x ≥ 40) | Plane 2 (x ≥ 40) | Sprites |
|---|---|---|---|---|---|
| 60–79 | 1 | S | 0 | 0 | yes |
| 80–99 | 2 | P1 | 1 | 0 | yes |
| 100–119 | 3 | P2 | 0 | 1 | yes |
| 120–139 | 4 | T | 1 | 1 | yes |
| 140–149 | 5 | B0 | 0 | 0 | no |
| 150–169 | 6 | F1 | 1 | 0 | no |
| 170–189 | 7 | F2 | 0 | 1 | no |
| 190–209 | 8 | F12 | 1 | 1 | no |

Plane 2 is 0 in the tag region (x < 40) on every row.

| Config | PF1P (v) | PF2P (w) | PF2PRI | BPLCON2 |
|---|---|---|---|---|
| B1 | 0 | 4 | 0 | `$0020` |
| B2 | 1 | 0 | 1 | `$0041` |
| B3 | 2 | 1 | 1 | `$004A` |
| B4 | 3 | 2 | 1 | `$0053` |
| B5 | 4 | 3 | 1 | `$005C` |
| B6 | 2 | 2 | 0 | `$0012` |
| B7 | 2 | 2 | 1 | `$0052` |
| B8 | 0 | 2 | 1 | `$0050` (the example on p. 211) |

Every PF1P and every PF2P value 0–4 occurs, both PF2PRI values occur, and in B1–B5 and B8 v ≠ w, so an exchange of the two fields is visible.

**Group E: sprites against a single playfield.** BPLCON0 is `$1200`, and BPLCON2 = `v<<3 | v` for v = 0, 1, 2, 3, 4 (configurations E0–E4). The sprites have the Group B X positions, with VSTART 60 and VSTOP 100.

| Lines | Tag id | Band | Plane 1 (x ≥ 40) | Sprites |
|---|---|---|---|---|
| 60–99 | 2 | P | 1 | yes |
| 100–119 | 6 | F1 | 1 | no |

PF1P and PF2P are set equal because the pages leave open which field governs a single playfield (see *Left open*).

The schedule is A1, A2, B1–B8, E0–E4: fifteen configurations of three fields each.

### Stimulus

The program skeleton above runs this schedule. The Copper list of each configuration writes BPLCON0, BPLCON2, BPL1PT and BPL2PT (Group E: BPL1PT only), and SPR0PT–SPR7PT, then ends.

### Observation

The observation is the video output of the evaluated field of each configuration: every output row whose tag decodes, together with 8 lores pixels (8*k* samples) of border on each side of the window.

### Reference frames and how the test checks them

- **Window columns.** s0 is the first sample of tag block 0 in a row, and *k* is the width of block 0 in samples divided by 8. The lores x of a sample *s* is ⌊(s − s0)/k⌋, and the window spans x = 0–319.
  - Check: block 0 is exactly 8*k* samples wide, and s0 is the same sample index in every row of every evaluated field.
  - Contrast: DDFSTRT `$0030` instead of `$0038`. p. 60 ties the fetch start to the window start, so the image moves against the window, and the block 0 width or position check fails.
- **Rows.** A row belongs to the band its tag decodes to.
  - Check: the bands appear in the order and with the relative row counts of the band table, because every band of 20 lines has the same number of output rows.
  - Contrast: BPLEN clear (DMACON `$8280|$0020`). No tags appear, and the check fails.
- **Sprite columns.** A sprite is drawn with "the leftmost pixel of the sprite data placed at the position defined in the horizontal part of SPRxPOS" on each line (p. 126). Its columns are therefore the same on every line of its height. Sprites "are independent of playfield modes of display" (p. 101).
  - Group B: the reference set R(i) is the set of samples that show SC(i) in band S.
  - Group A: slot references come from band E (even slots) and band O (odd slots).
  - Group E: the Group B R(i) are reused, since the X positions are identical.
  - Check: in every row of the reference band, R(i) is one contiguous run of exactly 16*k* samples with colour SC(i). The runs are identical in every row of the band and ordered left to right as the X values are.
  - Contrast: SPREN clear (DMACON `$8000|$0200|$0100|$0080`). The runs are absent and the check fails.
- **Field association.** The evaluated field lies between the tick after marker *k* and the next tick.
  - Check: every Copper instruction fetch seen at the memory port in that field lies inside list *k*'s address range, and the list's last fetch (the ending WAIT) comes before the field's first bitplane fetch.
  - Contrast: the program omits the COP1LC write for one configuration. The fetches stay in the previous list and the check fails.

### Expected values

For every sample of every graded row, the objects present are the sprites whose reference set contains the sample (only in bands whose lines lie inside the sprites' VSTART–VSTOP), plus the playfields the band's data sets at that x. The expected colour is the colour of the object in front of all the others present, or BG if none is present. The rules below restate the pages quoted above.

**Group A, band X.** Column blocks are 8 lores pixels wide. Block 0 is covered by slot 0 alone, block 8 by slot 7 alone, and block *j* (1–7) by slots *j*−1 and *j*. The expected colour is that of the lower-numbered of the sprites covering the block (p. 207).

| Block | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
|---|---|---|---|---|---|---|---|---|---|
| A1 | SC0 | SC0 | SC1 | SC2 | SC3 | SC4 | SC5 | SC6 | SC7 |
| A2 | SC7 | SC0 | SC0 | SC1 | SC1 | SC2 | SC2 | SC3 | SC3 |

Outside the slot columns, samples in the window show BG, apart from the tag region.

**Group B, sprite columns.** For sprite *i* in group g = g(i):

| Band | Expected colour |
|---|---|
| S | SC(i) |
| P1 | SC(i) if g < v, else PF1C |
| P2 | SC(i) if g < w, else PF2C |
| T | SC(i) if g < v and g < w; otherwise PF1C if PF2PRI = 0 and g ≥ v; PF2C if PF2PRI = 1 and g ≥ w; otherwise not graded (see *Left open*) |

The ungraded T case arises only in B8 for groups 0 and 1.

**Group B, columns without a sprite, x ≥ 40.**

| Band | Expected colour |
|---|---|
| S, B0 | BG |
| P1, F1 | PF1C |
| P2, F2 | PF2C |
| T, F12 | PF1C if PF2PRI = 0, else PF2C |

**Group B, band T, worked out per configuration.**

| Config | Groups 0–3 |
|---|---|
| B1 | all PF1C |
| B2 | all PF2C |
| B3 | group 0 sprite, groups 1–3 PF2C |
| B4 | groups 0–1 sprite, 2–3 PF2C |
| B5 | groups 0–2 sprite, 3 PF2C |
| B6 | groups 0–1 sprite, 2–3 PF1C |
| B7 | groups 0–1 sprite, 2–3 PF2C |
| B8 | groups 2–3 PF2C; groups 0–1 not graded |

**Group E, band P.** Sprite columns show SC(i) if g(i) < v, else PF1C. Other columns (x ≥ 40) show PF1C. In band F1 every column x ≥ 40 shows PF1C.

**All groups, other samples.**

- The tag region shows PF1C where a block is 1 and BG where it is 0.
- Rows with id 0 show BG at x ≥ 40.
- The border margins show BG (p. 46, and p. 57: "You cannot display objects outside of the defined display window").
- COLOR08, COLOR16/20/24/28 and the `$222` registers never appear.

**Quantifier.** "For each of the pixel positions" is graded on every sample of every row of the evaluated field, across all fifteen configurations. One sample or one row does not establish it.

### Preconditions

Each precondition is checked before grading, and each has a contrast that makes it fail.

1. **Frame association**, as in the reference frames above. Contrast: omit a COP1LC write.
2. **Tags decode and the band order holds**, as above. Contrast: BPLEN clear.
3. **Sprite runs are present and complete in the reference band**, as above. Contrast: SPREN clear.
4. **Playfields are present.** In Group B, band F1 shows PF1C at every x ≥ 40 and band F2 shows PF2C. In Group E, band F1 shows PF1C. Contrast: point BPL2PT at an all-zero image; band F2 then shows BG and the precondition fails.
5. **Vertical extent of the sprites.** Sprites appear in every row of the bands inside their VSTART–VSTOP and in no row of the first band after VSTOP (Group B: B0; Group A: the rows after band O). The test evaluates Group B in B7 (the sprites of groups 0 and 1, which are in front of both playfields in every band) and Group A in A1 and A2 (the even slots in band E, every slot in band X, the odd slots in band O, and no sprite in the rows after band O). When a sprite reference is missing, the precondition prints NOT GRADED instead of FAIL. Contrasts: Group B sprites with VSTOP 130. p. 107 says VSTOP is "the line AFTER the last displayed row", so band T loses its sprites in its last ten lines and the precondition fails. Group A odd slots with VSTOP 130: ten rows after band O show sprites and the precondition fails.

### Contrasting cases (the check must go red)

- **A-mirror.** Run A1 with slot *j* holding sprite 7 − *j*. The page predicts other colours in blocks 1–7 (block 1: SC6 instead of SC0), so the check graded against A1's table fails.
- **B-swap.** Run B1–B8 with the PF1P and PF2P fields exchanged. The page predicts other colours in bands P1 and P2 of B1–B5 and B8 (B1, band P1: every sprite in front of PF1 instead of behind it), so the original expectations fail.
- **PF2PRI flip.** Run B1–B8 with bit 6 inverted. Band F12 changes colour in every configuration, so the check fails.
- **E-reverse.** Run E0–E4 with v replaced by 4 − v. Every configuration except E2 changes colour, so the check fails.

### Left open by the pages (not graded)

- **Three objects with no single highest.** In B8 (PF2PRI = 1, PF2P = 010, PF1P = 000), p. 211 gives `PF1 SP01 SP23 PF2 SP45 SP67` and says "playfield 1 is in front of sprite 0 or 1; and sprites 0 through 3 are in front of playfield 2. However, playfield 2 is in front of playfield 1 in any area where they overlap and where playfield 2 is not blocked by sprites 0 through 3." Where PF1, PF2 and a sprite of group 0 or 1 overlap, these rules form a cycle, so no object is the highest. The text reads as if PF2 is "blocked" there and PF1 shows. Figure 7-3 (p. 212), drawn for the same case, shows the sprite vanishing and says "even though you can't see the boundary, the sprite disappears 'behind' the invisible PF1 boundary", which reads as PF2's colour showing. Those samples are not graded. The colour seen may be recorded, but it carries no verdict.
- **Which field sets the priority of a single playfield.** p. 210's note says "PF2P2 - PF2P0, bits 5-3, are the priority bits for normal (non-dual) playfields", while Table 7-2 is described in terms of PF1P and "playfield 1". Group E sets both fields equal, so both readings give the same expectation.
- **Whether a single-playfield pixel of value 0 hides a sprite behind the playfield.** p. 46 calls COLOR00 the background shown "where there is no other object present". The pages do not say whether a value-0 pixel of a single playfield is an object. No stimulus places a sprite over such a pixel.

---

## P2 — Bitplane and sprite data are combined into one video output

**Proposition.** The data of all six bitplanes and of all eight sprites reach the one video output together, so a single output row shows the contribution of each. The claim is `opus:7-cd.c2`, on p. 213.

> "The video output is formed when the input data from all of the bitplanes and the sprites is combined into a common data stream for the display." (p. 213)

### Stimulus

The program skeleton runs with one configuration.

- **Mode.** BPLCON0 `$6600`: six lores planes in dual playfield. p. 48 says "Sixth bitplane is used only in dual-playfield mode". BPLCON2 is `$0000`.
- **Band.** Lines 60–79, tag id 1 in plane 1. In these rows, plane *k* (k = 1–6) is 1 exactly at x = 272 + 8(k−1) to 279 + 8(k−1), and every plane is 0 at every other x ≥ 40. Rows outside the band carry block 0 only.
- **Sprites.** Sprite *i* sits at X = 170 + 20*i*, with VSTART 60 and VSTOP 80.

### Observation

The observation is every output row with tag id 1 in the evaluated field. The reference frames for window columns, rows, field association and sprite runs are the same as in P1; the sprite runs are read in the band itself, where every plane is 0 beneath them.

### Expected values

Every row of the band contains all fourteen contributions:

- **Sprites.** Eight runs of 16*k* samples in the colours SC0–SC7, ordered left to right. Sprites are "Together with playfields … the graphics display" (p. 97), and their colours are given in Table 4-6.
- **Planes.** Six blocks of 8*k* samples. Plane 1 shows COLOR01 `$F00`, plane 3 COLOR02 `$A00` and plane 5 COLOR04 `$500`, from Table 3-10 (p. 70), where planes 5, 3, 1 form PF1's number from most to least significant. Plane 2 shows COLOR09 `$0F0`, plane 4 COLOR10 `$0A0` and plane 6 COLOR12 `$050`, from Table 3-11.
- **Elsewhere.** Other samples at x from 40 to 271 show BG, and the tag region shows as in P1.

"All of the bitplanes and the sprites" is graded as all six planes and all eight sprites, in every row of the band.

### Preconditions

- **Frame association, tags and window columns**, as in P1, with the same contrasts.
- **Each plane pointer points at its own image.** Check: the bitplane fetches seen at the memory port in the band's lines read from all six images.
  - The band lies on lines 60-79; the test counts the fetches of lines 62-77 of the evaluated field (VERTB marks line 0, p. 219; one line is 227 colour clocks, p. 23), two lines in from each end.
  - Contrast: point BPL4PT at plane 2's image. The fetch check fails, and so does the plane-4 block.

### Contrasting cases

- **SPREN clear.** The sprite runs show BG, so the check fails.
- **BPL4PT at an all-zero image.** The plane-4 block shows BG instead of `$0A0`, so the check fails. This case shows that each bitplane is checked separately, not only the set as a whole.

### Left open

No value on the page is left open for this stimulus.

---

## P3 — With a light pen in use, the beam-position address returns the light-pen position

**Proposition.** Once the light-pen input is enabled and a trigger arrives on the controller port's beam-trigger line, VPOSR and VHPOSR return the beam position latched at the trigger, not the moving beam position, until the end of internal vertical blanking of the next field. The claims are `opus:7-bp.c3`, `sol:7-beam.c3` and `opus:7-bp.c11`, on p. 216.

> "In addition, when you are using a light pen, this same address is used to read the light pen position rather than the beam position." (p. 216)
> VHPOSR: "Read vertical and horizontal position of the counter that is producing the beam on the screen (also reads the light pen)." (p. 216, Table 7-5)

p. 216 refers to Chapter 8 ("This is described fully in Chapter 8"), and these pages there qualify the claim:

- p. 238: "Just as the system exits vertical blank, the capture circuitry for the light pen is automatically enabled." Also: "The pen converts this light pulse into an electrical pulse on the 'Beam Trigger' line (pin 6)." And: "This trigger signal tells the internal circuitry to capture and save the current contents of the beam register, VPOSR."
- p. 239: "To enable the light pen input, write a 1 into bit 3 (LPEN) of BPLCON0. Once the light pen input is enabled and the light pen issues a trigger signal, the value in VPOSR is frozen. If no trigger is seen, the counters latch at the end of the display field. It is impossible to read the current beam location while the VPOSR register is latched. This freeze is released at the end of internal vertical blanking (vertical position 20)."
- p. 239: "If the VPOSR value is greater than $10500, the light pen has not triggered since the last top-of-screen. If the value is less, the light pen has triggered and the value read is the screen position of the light pen."
- p. 238: "A light pen can be connected to one of the controller ports. On the A1000, the light pen must be connected to port 1. … On the A500, A2000 and A3000 the default is port 2."

### Read value

Each read is a long read of VPOSR (p. 216: the pair "can be read from … as long words") followed by a word read of VHPOSR. The value graded is M = (VPOSR bit 0) × 2¹⁶ + VHPOSR, which is "mask off the upper 15 bits" (p. 239). From it, V = M >> 8 (V8–V0) and H = M & `$FF` (H8–H1, one unit per colour clock: "Horizontal resolution is 1/160th of the screen width", p. 216; "RESOLUTION = 1/160 of screen width (280 ns)", p. 285). VPOSR bits 15–1 are ignored: p. 216 calls bits 14–1 "Unused", p. 238 calls them "Chip ID code. Do not depend on value!", and bit 15 is LOF.

### Stimulus (program)

1. Write INTENA `$7FFF`, INTREQ `$7FFF` and DMACON `$7FFF`.
2. Read CIA-A DDRA. If bit 6 or bit 7 is 1, clear those two bits and write the result back with bits 0–5 unchanged. Read DDRA again and store that value. Pin 6 of both ports must be an input: p. 228 says "These pins may also be configured as outputs", p. 229 describes CIAAPRA as "Input and output for pin 6 (port 1 & 2 fire buttons)", and p. 351 assigns PA6 and PA7 to the fire buttons.
3. ECS only: store a tick at each of two successive VERTBs, which gives the field period before the write. Then write BEAMCON0 `$0020` (PAL) or `$0000` (NTSC). This makes LPENDIS 0 and keeps bit 5 ("Programmable pal mode enable", p. 305) at the selected standard. The pages give no reset value for BEAMCON0.
4. Write BPLCON0 `$0008` (LPEN = 1, no bitplanes) and COLOR00 `$000`.
5. Build the Copper list, in beam order as p. 27 requires. The column Ev names the event:

   | Wait | Move | Ev |
   |---|---|---|
   | `$2801,$FFFE` (line 40) | INTREQ ← `$8010` | E40 |
   | `$3C01,$FFFE` (line 60) | INTREQ ← `$8010` | E60 |
   | `$6441,$FFFE` (line 100, H `$40`) | COLOR00 ← `$F00` | anchor A1 |
   | `$64A1,$FFFE` (line 100, H `$A0`) | COLOR00 ← `$000` | anchor A2 |
   | `$6E01,$FFFE` (line 110) | INTREQ ← `$8010` | E110 |
   | `$8201,$FFFE` (line 130) | INTREQ ← `$8010` | E130 |
   | `$B441,$FFFE` (line 180, H `$40`) | COLOR00 ← `$00F` | anchor B1 |
   | `$B4A1,$FFFE` (line 180, H `$A0`) | COLOR00 ← `$000` | anchor B2 |
   | `$BE01,$FFFE` (line 190) | INTREQ ← `$8010` | E190 |
   | `$D201,$FFFE` (line 210) | INTREQ ← `$8010` | E210 |
   | `$FFFF,$FFFE` | — | end |

6. Wait for a VERTB (ECS: the one the second pre-write tick waited for), write COP1LC, strobe COPJMP1, then write DMACON `$8280` (the p. 30 order). The Copper then starts a few lines after the top of a field, and its first pass through the list runs in real time.
7. Run an event loop that polls INTREQR.
   - On bit 5 (VERTB), clear it with `$0020` and advance the field counter. Store the tick, wait about 2 lines, read (event V1), wait about 8 more lines, and read again (event V2). The waits are counted polls of INTREQR, never polls of VPOSR or VHPOSR: p. 239 says that with the light-pen latch enabled "the VPOSR register may be latched at any time, and cannot be relied on as a counter". The count is calibrated in the run: in the first pass of the Copper (before any VERTB has been seen) the same polling loop counts its iterations between the COPER signals of E110 and E130, which the list places 20 lines apart (WAIT lines 110 and 130, p. 22). The waits are one tenth and two fifths of that count.
   - On bit 4 (COPER), clear it with `$0010`, read, and number the event by its order in the field: E40, E60, E110, E130, E190, E210.
   - Each read stores a record: field number, event id, VPOSR, VHPOSR (both from the long read), and VHPOSR from the word read.
8. Run one settling field F−1 with LPEN already on, then fields F0–F6, then the V1 and V2 reads at the start of F7, then store an end marker.

### Stimulus (driven by the test)

The test drives a trigger on pin 6 of **both** controller ports at the same instant. The trigger is the level of a pressed fire button, held for 4 colour clocks, then released. At all other times pin 6 is at the released level.

Both ports are driven because p. 238 makes the port model-dependent. The pressed level is used because the chip's input is named LP\* (Appendix J, p. 379), and an asterisk marks an active-low line, as in "INT6\* becomes a logic 0" (p. 219). The pages give neither the pulse polarity nor the pulse width in words.

| Field | Trigger |
|---|---|
| F1 | t(A1) + 16 colour clocks |
| F3 | t(A1) + 56 colour clocks |
| F5 | t(B1) + 16 colour clocks |
| F0, F2, F4, F6 | none |

t(A1) is the memory-port time of the fetch of the MOVE that follows the WAIT for (100, `$40`) in the same field, and t(B1) likewise for (180, `$40`). One colour clock is one memory slot. p. 194 says "there are 227.5 'color clocks', or memory access cycles", and p. 285 gives 280 ns. Both triggers in F1 and F3 start at the same phase within a colour clock.

### Observation

The observation is the records in memory, their store times at the memory port, and the Copper fetch times.

### Reference frame and how the test checks it

- **Line numbers.** A WAIT holds the Copper "until the video beam counters are equal to (or greater than) the coordinates specified" (p. 22). The fetch after WAIT (L, H) therefore happens with the beam at or past (L, H). VERTB marks line 0 (p. 219).
- **Anchor check.**
  - In every field, t(A2) − t(A1) and t(B2) − t(B1) each equal `$A0` − `$40` = 96 colour clocks, within 2. Both anchors of a pair are then on the same line with equal Copper delay, and each trigger lies between them.
  - The field period t(A1, Fn+1) − t(A1, Fn) is the same for all fields.
  - Contrast: change the second anchor's WAIT to `$65A1` (line 101). The spacing becomes about 323 colour clocks and the check fails.
- **Read-time check.**
  - Each Copper-signalled read is stored within one line (227 colour clocks) after the fetch of its signalling MOVE.
  - V1 and V2 are stored less than 18 lines after the CPU leaves its VERTB poll loop (seen as instruction fetches at the memory port). This places them before vertical position 20.
  - Contrast: add a 30-line delay before V1. The check fails.

### Expected values (LPEN = 1)

Let L1, L3 and L5 be the values read at E110 of F1, E110 of F3 and E190 of F5.

1. **Latched after a trigger.**
   - In F1 and F3, the reads at E110, E130, E190 and E210, and at V1 and V2 of the following field, all return the same M.
   - In F5, the reads at E190 and E210, and at V1 and V2 of F6, all return the same M.
   - In every one of these reads, the word read of VHPOSR equals the low word of the long read.

   Source: "the value in VPOSR is frozen … This freeze is released at the end of internal vertical blanking (vertical position 20)" (p. 239).
2. **The latched value is the light-pen position.**
   - V(L1) = 100, V(L3) = 100 and V(L5) = 180.
   - H(L3) − H(L1) = 40 exactly. The two triggers are 40 colour clocks apart relative to the same anchor, and the latch saves "the current contents of the beam register" at the trigger (p. 238).

   Source: "the value read is the screen position of the light pen" (p. 239).
3. **Not the beam position.** Every read in item 1 returns V = 100 or 180 although the beam is at lines 110, 130, 190, 210 or the first lines of the next field. Source: "rather than the beam position" (p. 216) and "It is impossible to read the current beam location while the VPOSR register is latched" (p. 239).
4. **Before the trigger and after the release, the address reads the beam.**
   - In every field, E40 and E60 read V ∈ {40, 41} and V ∈ {60, 61}.
   - In fields without a trigger, E110 to E210 read V equal to their line or the next.
   - In F2, F4 and F6, E40 reads V ∈ {40, 41} even though the previous field latched. That latch was released at vertical position 20, or 25 under p. 219 (see *Left open*), and both are before line 40.

   Source: p. 216, Table 7-5, and the release sentence on p. 239.
5. **No trigger: latched at the end of the field.** V1 and V2 of F1, F3, F5 and F7 (after the fields without a trigger, F0, F2, F4, F6) are equal to each other. Their M is greater than `$10500` in PAL and not less than `$10500` in NTSC. Source: "If no trigger is seen, the counters latch at the end of the display field" and the `$10500` rule (p. 239).

**Quantifiers.** "Rather than the beam position" is graded on every read inside a latched interval, not on one read. The interval runs from the trigger to vertical position 20 of the next field. The reads are three to six per trigger, over three triggers on two lines and at two horizontal offsets.

### Preconditions

0. **Record layout.** All 72 records are present, and each carries the field and event words that its position implies: the first six are the first pass of the Copper (field word `$FFFF`, events E40 to E210 in order), then eight per field (V1, V2, E40 to E210), and two for F7. Contrast: the V1 record of every field carries event id 5.
1. **Anchor check**, as above. Contrast: move the second anchor to line 101.
2. **Read-time check**, as above. Contrast: add a 30-line delay before V1.
3. **Pin 6 is not driven by the CIA.** The DDRA read after step 2 has bits 6 and 7 at 0. Contrast: the program sets DDRA bits 6 and 7 to 1, and the check fails.
4. **The pulse was driven.** The test's own drive log shows each pulse at its scheduled time and nothing else on pin 6. Contrast: omit the F1 pulse, or drive it 30 colour clocks late. The log is always compared with the nominal schedule (three pulses, 16 colour clocks after the Copper's A1 and B1 fetches of F1, F3 and F5), so the check fails and the four graded lines print NOT GRADED.
5. **The beam reads live when not latched** (item 4). It shows that the register is not stuck. Contrast: the E40 WAIT at line 44 instead of line 40, so E40 reads V = 44 or 45 and the check fails (p. 22).
6. **ECS: the BEAMCON0 write changed nothing but LPENDIS.** The field period after step 3 equals the one measured before it (VERTB ticks), within one line (p. 23: 227 or 228 colour clocks, taken as 228). Contrast: write bit 5 opposite to the selected standard. p. 305 names bit 5 "Programmable pal mode enable", and p. 24 gives the line counts (312 and 262), so the period changes by fifty lines and the check fails.

Items 1, 2 and 5 are graded only when preconditions 0 to 6 hold; otherwise each prints NOT GRADED with the failing preconditions. The claims of P3 are credited by one line that holds when items 1 and 2 both hold; items 1, 2 and 5 are component lines and carry no claim id.

### Contrasting cases

- **LPEN clear.** BPLCON0 is `$0000` with the same pulses. Every read tracks the beam: E110 reads V ∈ {110, 111}, and V1 and V2 differ. Items 1, 2 and 5 fail.
- **B1 and B2 one line lower** (line 181 instead of 180). The F5 trigger follows B1 and lands on line 181, so V(L5) is 181 instead of 180 and item 2 fails, with every precondition holding.

### Left open by the pages

- **The absolute H of the latch.** The pages do not state the delay between a WAIT being satisfied and the Copper's next fetch, so the absolute H at the trigger cannot be derived to the colour clock. Only differences are graded.
- **Which port carries the pen.** It depends on the model (p. 238), so both ports are driven. A run with one port driven may be recorded without a verdict.
- **Pulse polarity and width.** The pages state neither in words.
- **The end of the latch in PAL.** p. 239 says the freeze ends at "vertical position 20", while p. 219 gives PAL vertical blanking as running "to line 25". Reads are placed before line 18 and triggers after line 26, so both readings agree.
- **The value at exactly `$10500`.** p. 239 is written for NTSC (`$10500` is V = 261) and says nothing about an equal value, so NTSC grades "not less than".

---

## P4 (ECS) — With LPENDIS set, a vertical-position read ignores the latched pen value

**Proposition.** On ECS, with BEAMCON0 bit 13 (LPENDIS) set, a read of the vertical position does not return the latched light-pen value, while the latch itself still holds it. The claims are `opus:7-appC-beamcon0.c4` and `sol:7-appC-beamcon0.c4`, on p. 305.

> "13 LPENDIS Ignore latched pen value on vertical pos read" (p. 305, BEAMCON0)

### What the page leaves open, and how the stimulus avoids depending on it

The page does not say:

- whether "vertical pos read" means VPOSR (V8), the V7–V0 byte of VHPOSR, or both;
- what the read returns instead of the latched value;
- whether the horizontal byte is affected.

No expected positive value is therefore graded. The graded relation is that the vertical pair (V8, V7–V0) read with LPENDIS set differs from the latched pair. To make that relation hold under every reading above:

- The trigger is placed on line 100, so the latched pair is (0, 100).
- The read is made on a line of 256 or more of the same field, still inside the latched interval. A live read there has V8 = 1 and a low byte different from 100.

Any reading that replaces at least one of the two vertical fields with the live counter therefore gives a pair different from (0, 100).

### Stimulus

The P3 program runs in ECS and PAL with these changes:

- BEAMCON0 starts at `$0020`.
- The Copper list gains `$FA01,$FFFE` (line 250) → INTREQ ← `$8010` (E250) before its end. Lines above 255 are not waited for by the Copper, because p. 24's method for them is unclear (see the ambiguities list).
- **F1.** At E60, the CPU writes BEAMCON0 `$2020` (LPENDIS = 1, PAL bit kept). The test drives the trigger at t(A1) + 16 colour clocks. At E250, the CPU waits about 20 lines and reads X. It then writes BEAMCON0 `$0020` and reads Y at once.
- **F3.** This is a control inside the same run. At E60 the CPU writes BEAMCON0 `$0020`. The trigger is at t(A1) + 16. At E250 the CPU waits about 20 lines, reads X′, writes `$0020` again, and reads Y′.
- F0, F2 and F4 have no trigger. BEAMCON0 stays `$0020` outside the F1 window.

NTSC is optional. The Copper waits for line 250 as in PAL, and the CPU waits about 8 lines, which puts the read in lines 256–261. The PAL bit is 0 throughout, so the writes are `$2000` and `$0000`.

### Observation

The observation is the records X, Y, X′ and Y′, their store times, the anchors, and the VERTB ticks.

### Reference frame

The anchors and the read-time check are those of P3. In addition, the store time of X must lie at least 156 × 228 colour clocks after t(A1) of the same field, which is line 256 or later. The store time of Y must lie at least 100 × 228 colour clocks before t(A1) of the next field, which is before line 0 of the next field. 228 is the long-line length on p. 23, used here as the bound. The latch is frozen until vertical position 20 of the next field (p. 239), so both reads are inside it. Contrast: wait only 2 lines after E250. X then falls before line 256 and the check fails.

### Expected values

- **Latch present (precondition, from p. 239 as in P3).** Y has V = 100 and H(Y) = H(Y′). X′ equals Y′.
- **Graded.** The pair (V8, V7–V0) of X is not (0, 100).
- **Left open, not graded.**
  - The value of X, including its horizontal byte.
  - The reads at E110, E130, E190 and E210 in F1 while LPENDIS is set, because under some readings they are the latched value and under others the live one.

### Preconditions

1. **Anchor check, the CIA pin 6 check and the drive log**, as in P3, with the same contrasts (the second anchor at line 101; DDRA bits 6 and 7 set; a third pulse in F2 after Y is stored, which the drive log shows against the nominal schedule of two pulses).
2. **Read time.** Y is stored within one line (p. 23) after X, and Y′ within one line after X′, because each second read follows its BEAMCON0 write at once. Contrast: 40 poll iterations between the write and the second read.
3. **Field period.** The field period is unchanged across all BEAMCON0 writes, measured from the VERTB ticks, within one line (p. 23, taken as 228 colour clocks). Contrast: write `$2000` in PAL. p. 305 bit 5 and p. 24 predict a change, and the check fails. The reference-frame check is NOT GRADED when this one fails, because a shorter field ends before the delay does.
4. **Reference frame**, as above. Contrast: wait only 10 poll iterations after E250.
5. **The latch is present in F1.** V(Y) = 100, and H(Y) = H(Y′). Contrast: BPLCON0 LPEN clear from E60 of F1 until Y is stored, the pulses unchanged. Y then reads the live beam (V8 = 1) and the check fails.
6. **The control field shows the latched value with LPENDIS clear.** In F3, X′ equals Y′ and V(X′) = 100. Contrast: write `$2020` at E60 of F3 as well. X′ then differs from Y′ and the check fails.

The graded relation is evaluated only when every precondition above holds, and the fixture checks (program loaded, A1 fetched at least five times, driven run reached DONE) hold; otherwise it prints NOT GRADED with the failing preconditions and credits no claim.

### Contrasting case

**LPENDIS never set.** At E60 of F1 the CPU writes `$0020` instead of `$2020`. X then equals Y, which is (0, 100), so the graded relation fails.

On OCS, BEAMCON0 does not exist ("A new register in the enhanced Agnus", p. 305). Selecting OCS behaviour with the F1 stimulus is expected to make X equal Y, which is a second contrast. It is optional, because the pages do not say what a write to that address does on OCS.

---

## Points where the pages are ambiguous or disagree

These are listed because they shaped the stimuli above.

1. **p. 25 and p. 32 on when the Copper restarts from COP1LC.** p. 25 says "At the start of each vertical blanking interval" and, in the next sentence, "when the end of vertical blanking occurs". p. 32 says "at the start of the vertical blanking period". The design holds each configuration for three fields, evaluates the second, and puts no Copper anchor in lines 0–25.
2. **p. 99 and p. 100 against p. 57 and p. 59 on horizontal coordinates.** Sprite examples use a window from X = 64, while the display window starts at H = `$81` (129). Vertically they agree (line 44 = `$2C`). The design locates sprites in the output.
3. **p. 210 on which bits govern a single playfield.** Table 7-2 is described with PF1P and playfield 1, and the "Be careful" note says PF2P governs normal (non-dual) playfields. The design sets both fields equal.
4. **p. 211 text against Figure 7-3 (p. 212) on the three-way overlap** in the p. 211 configuration. The text implies PF1 shows there, and the figure shows PF2's area with the PF1 boundary invisible. Those samples are not graded.
5. **p. 219 against p. 239 on the end of internal vertical blanking in PAL.** p. 219 gives line 25 for PAL and p. 239 gives line 20. The design avoids lines 18–26.
6. **p. 239 on NTSC and PAL.** The method is written for NTSC (`$10500` = V 261), says nothing about PAL, and does not say what an equal value means. The printed "0<V20" is presumably "0 < V < 20".
7. **p. 216 against p. 238 on VPOSR bits 14–1.** p. 216 calls them "Unused" and p. 238 calls them "Chip ID code. Do not depend on value!". They are masked.
8. **p. 238 on the pen's port.** The port depends on the model and a jumper. Pulse polarity and width are not stated.
9. **p. 305 on LPENDIS and BEAMCON0.** LPENDIS is described in one line and leaves open which register and bits "vertical pos read" means and what is returned. No reset value of BEAMCON0 is given.
10. **p. 304 against p. 23 on PAL line length.** p. 304 says "PAL … 227.5 colorclocks per scan line" and p. 23 says "In PAL, they are all 227 long". Only measured periods and differences within one line are used.
11. **p. 24 on waiting for lines above 255.** The method (WAIT for (0,255), then WAIT for lines 0 through 5) would, read literally, end the second wait at once, because line 255 already compares as greater than or equal to lines 0–5. The design does not have the Copper wait past line 255.

---

## Build notes

### Programs

- `prog_pri.s` is P1 and `prog_p2.s` is P2. Both include `pri_common.i` (register offsets, memory map, window setup) and `pri_routines.i` (the builders). The CPU builds every image, sprite block and Copper list below `$80000` before DMA is on, after releasing the boot overlay through CIA-A. Bitplane images are 256 rows of 40 bytes; a row carries byte 0 = `$FF` (tag block 0), bytes 1-4 = the band id's bits as `$FF` or `$00`, and bytes 5-39 as the band table says. Sprite block *s* of a group lies at its base + *s* × `$200`.
- P1 schedules the fifteen configurations as the skeleton says: wait VERTB (t0), write COP1LC, store marker *k*, wait VERTB (t1), wait VERTB (t2), so the evaluated field is [t1, t2). Every tick is stored at its own address and every marker at its own address, so the test reads their store cycles from the memory port.
- `prog_p4.s` counts its delay after E250 with a poll of INTREQR instead of a fixed constant. In F0, where no trigger has happened, the same loop counts its iterations between the COPER signals of E110 and E130, which the list places 20 lines apart (WAIT lines 110 and 130, p. 22); that count is the iteration count of the delay in F1 and F3. The reference-frame precondition of P4 (X stored at least 156 × 228 colour clocks after t(A1), Y before the next field's t(A1) minus 100 × 228) is what asserts the result in every run, and the test prints the calibration count, the Copper's own spacing of E110 and E130, and the length of the delay.
- `prog_lp.s` times the V1 and V2 reads of P3 with the same polling loop: in the first pass of the Copper, which starts right after a VERTB, it counts the iterations between the COPER signals of E110 and E130 (CALIB), and the waits after each VERTB are CALIB / 10 and 2 * CALIB / 5 iterations. The test prints CALIB and the Copper's own spacing of E110 and E130, and asserts the outcome through the read-time precondition (every V1 and V2 stored less than 18 lines after its VERTB tick). Both programs read CIAADDRA again after the fix-or-force branch and store that word; the DDRA precondition grades it.

### How the video output is read

- The scan doubler emits every beam line twice, so a field of 256 window lines is 512 output rows in PAL and 400 in NTSC (the window ends at line 244). A row is one span between two rising edges of `hs`.
- k, the number of output samples per lores pixel, is the smallest width of a run of PF1C that starts a row, divided by 8; it must be a whole number. s0 is the index of that run's first sample, counted from the row's own first sample. Both are measured in every run and printed (`MEASURED: P1 ... reference frame`); they are not constants of the test.
- The evaluated field of a configuration is the video frame (vs edge to vs edge) that contains the middle of [t1, t2).
- Every sample from 8k before to 8k after the window of every decoded row is compared with the expected colour. A sample whose expected colour the pages leave open (B8, where PF1, PF2 and a group-0 or group-1 sprite overlap) is counted as open and printed with the colour seen; it carries no verdict.
- A graded line is evaluated only when the preconditions of its group hold; otherwise it prints `RECORDED: ... NOT GRADED` with the failing preconditions. Preconditions are never folded into the graded expression.

### Contrasts as built

Each contrast changes the stimulus only; the comparator and the expected values are those of the unchanged run.

| Page contrast | Symbol (`NMH_MUTATE=`) | What changes |
|---|---|---|
| P1 frame association: omit a COP1LC write | `P1-NOCOP`, `P2-NOCOP` | no COP1LC write for configuration 4 (P2: the Copper starts from a copy of the list at another address and no write follows) |
| P1 rows: tags absent | `P1-TAGBAD`, `P2-TAGID` | one row of A1 and A2 whose tag block 2 is half set (P2: the band's id is 2) |
| P1 rows: band order and relative row counts | `P1-TAGCOUNT` | the first row of band X of A1 and A2 carries tag id 0, so every row still decodes and only the row count of band X differs from that of band E |
| P1 window columns: DDFSTRT `$0030` | `P1-SCROLL`, `P2-SCROLL` | BPLCON1 `$0011` from line 215 on, cleared after the field; p. 82: "For each pixel of delay, the on-screen data shifts one pixel to the right", so the rows below line 215 start one lores pixel later than the rows above it. DDFSTRT `$0030` and BPLEN clear, the contrasts the sections above name, empty the population of decoded rows altogether (no row decodes), so they cannot tell a failing window check from an empty observation; the scroll keeps every row decodable and moves the rows below line 215 by one lores pixel |
| P1 sprite columns: SPREN clear | `P1-SPRENCLR`, `P2-SPRENCLR` | DMACON `$8380` |
| P1 playfields: BPL2PT at a zero image | `P1-BPL2ZERO` | BPL2PT of B1-B8 at the all-zero image |
| P1 vertical extent: VSTOP 130 | `P1-VSTOP130` | the sprites of B1-B8 end at line 130 |
| P1 vertical extent, Group A | `P1-AVSTOP130` | the odd slots of A1 and A2 end at line 130 instead of 120, so ten rows after band O show sprites |
| P1 A-mirror, B-swap, PF2PRI flip, E-reverse | `P1-AMIRROR`, `P1-BSWAP`, `P1-PRIFLIP`, `P1-EREV` | as the page contrasts say |
| P2 plane fetch: BPL4PT at plane 2's image | `P2-BPL4PLANE2` | as said |
| P2: BPL4PT at an all-zero image | `P2-BPL4ZERO` | BPL4PT at the all-zero image inside plane 4's own area, so the fetch check still holds and only the plane-4 block changes |
| fixture | `P1-LATEMARK`, `P2-LATEMARK` | the marker is stored after t1 instead of before it |
| P4 | `P4-DDRA`, `P4-ANCHOR`, `P4-PERIOD`, `P4-SHORT` | DDRA bits forced to outputs; second anchor at line 101; every BEAMCON0 write with bit 5 clear; the delay after E250 is 10 poll iterations |

### Run matrix as built

P1, P2 and P3 run under OCS and ECS, in PAL and NTSC, on both cores. The standard is selected through `chipset_config` (bit 1 = NTSC, bit 3 = ECS), and NTSC runs use DIWSTOP `$F4C1`. P4 runs in ECS and PAL only; its NTSC run, which the specification calls optional, is not built.

### The B8 overlap

In B8 where PF1, PF2 and a group-0 or group-1 sprite overlap, the colour seen is `$0F0` (PF2C) on every combination and both cores. The pages leave this open (see "Left open by the pages"); it carries no verdict.

### Recorded

Both cores ran the final build (`make -k -B test`) and then every `NMH_MUTATE` selector below from the same object directories. `out_fx68k.txt`: 115 passed, 18 failed. `out_tg68k.txt`: 115 passed, 18 failed. Every line has the same verdict on both cores; what differs per core is only a measured number (listed below the table).

| Line | Cases | fx68k | TG68K |
|---|---|---|---|
| `P3-FIXTURE-LOADED` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P3-FIXTURE-TRIGGERS` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P3-FIXTURE-DRIVEN` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P3-PRE-RECORDS` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P3-PRE-ANCHOR` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P3-PRE-READTIME` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P3-PRE-DDRA` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P3-PRE-PULSE` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P3-PRE-LIVE` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P3-HELD` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | FAIL | FAIL |
| `P3-PEN-POSITION` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | FAIL | FAIL |
| `P3-LATCHED-PEN-POSITION` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | FAIL | FAIL |
| `P3-FIELD-END-LATCH` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | FAIL | FAIL |
| `P3-PRE-PERIOD` | ECS-PAL, ECS-NTSC | PASS | PASS |
| `P4-FIXTURE-LOADED` | ECS-PAL | PASS | PASS |
| `P4-FIXTURE-A1` | ECS-PAL | PASS | PASS |
| `P4-FIXTURE-DRIVEN` | ECS-PAL | PASS | PASS |
| `P4-PRE-DDRA` | ECS-PAL | PASS | PASS |
| `P4-PRE-ANCHOR` | ECS-PAL | PASS | PASS |
| `P4-PRE-READTIME` | ECS-PAL | PASS | PASS |
| `P4-PRE-PULSE` | ECS-PAL | PASS | PASS |
| `P4-PRE-PERIOD` | ECS-PAL | PASS | PASS |
| `P4-PRE-REFFRAME` | ECS-PAL | PASS | PASS |
| `P4-PRE-LATCHF1` | ECS-PAL | FAIL | FAIL |
| `P4-PRE-CONTROL` | ECS-PAL | FAIL | FAIL |
| `P4-LPENDIS-READ` | ECS-PAL | NOT GRADED | NOT GRADED |
| `P1-FIXTURE-RUN` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P1-PRE-FRAME` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P1-PRE-TAGS` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P1-PRE-WINDOW` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P1-PRE-SPRITES` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P1-PRE-PLAYFIELDS` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P1-PRE-VEXTENT` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P1-GROUP-A` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P1-GROUP-B` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P1-GROUP-E` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P1-COLOUR-OF-HIGHEST-PRIORITY` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P2-FIXTURE-RUN` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P2-PRE-FRAME` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P2-PRE-TAGS` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P2-PRE-WINDOW` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P2-PRE-PLANE-FETCH` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |
| `P2-COMBINE` | OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC | PASS | PASS |

The eighteen reds are the four P3 lines `P3-HELD`, `P3-PEN-POSITION`, `P3-LATCHED-PEN-POSITION` and `P3-FIELD-END-LATCH` (p. 216, p. 238, p. 239) in each of the four cases, and `P4-PRE-LATCHF1` and `P4-PRE-CONTROL` (p. 239): VPOSR and VHPOSR return the live beam counter on every case tried, and no read is held between a trigger and the end of internal vertical blanking. `P4-LPENDIS-READ` prints NOT GRADED because those two preconditions are red, and credits no claim.

A private copy of the RTL outside the repository, which implements pp. 238-239 and the LPENDIS bit of p. 305 (BPLCON0 bit 3 enables a latch of vpos and hpos that a falling edge of pin 6 sets after line 20, that the end of the field sets when no trigger came, that vertical position 20 releases, and that VPOSR and VHPOSR return unless LPENDIS is set), was built against the unchanged package: P3 and P4 give 66 passed and 0 failed on both cores in the run without P1 and P2, `P4-LPENDIS-READ` is PASS, and the four P3 lines are PASS in all four cases, in NTSC too (V1 and V2 of F1, F3, F5 and F7 are `$13801` in PAL and `$10601` in NTSC).

Measured values (not graded):

- P1, every case and both cores: k = 2 output samples per lores pixel, s0 = 141 samples from the row's first sample, 512 rows per field in PAL and 400 in NTSC, 5155840 samples compared in PAL and 4026880 in NTSC (P1 PAL: 688128 in A1 and A2, 2747392 in B1-B8, 1720320 in E0-E4); P2: 344072 samples (PAL), 268808 (NTSC).
- B8, the three-way overlap the pages leave open: 5120 samples per case are not graded; the colour seen there is `$0F0` (PF2C).
- P3 delay calibration, polls of INTREQR over the 20 lines E110..E130 (the Copper fetches them 36320 cycles apart): 164 (PAL) and 163 (NTSC) on fx68k, 221 on TG68K.
- P3 item 2: V(L1) = 110, V(L3) = 110 and V(L5) = 190 in every case on both cores (the live beam at E110 and E190); H(L3) - H(L1) is 6, 6, 16, 16 on fx68k and -4, -4, 2, -20 on TG68K in OCS-PAL, ECS-PAL, OCS-NTSC, ECS-NTSC.
- P3 item 5: V1 and V2 of F1, F3, F5 and F7 read the live beam on this RTL: V1 at line 2 (`$0029B` and `$0028A` on fx68k, `$00262` to `$00254` on TG68K) and V2 at line 11 or 10 (`$00B1F` to `$00A7E`), unequal, below `$10500`, in every case.
- P3 period of the field before and after the BEAMCON0 write: 568408 and 568360 (fx68k) and 568376 and 568568 (TG68K) cycles in ECS-PAL, 477640 and 477704 (fx68k) and 477592 and 477528 (TG68K) in ECS-NTSC; the bound is one line, 1824 cycles.
- P4 delay calibration: fx68k 166 poll iterations over the 20 lines between E110 and E130, the delay after E250 lasted 37060 cycles to X's store, X's store lies 308964 cycles after t(A1) (bound: more than 284544); TG68K 224 iterations, 36932 cycles, 308836 cycles. Y is stored 440 (fx68k) and 432 (TG68K) cycles after X. X reads V = 270 on both cores; Y reads V = 270, with H = 141 and H(Y') = 145 on fx68k and H = 124 and H(Y') = 128 on TG68K.

### Mutations replayed

Each selector was run on both cores from the final build; the table gives what turned red (other labels keep their verdict of the primary run) and what printed NOT GRADED. A selector of P3 runs the OCS-PAL case unchanged beside the mutated ECS-PAL case, so its summary counts the OCS-PAL lines too.

| Selector | fx68k | TG68K |
|---|---|---|
| P1-LATEMARK | FAIL: P1-FIXTURE-RUN; NOT GRADED: P1 (0 passed, 1 failed) | FAIL: P1-FIXTURE-RUN; NOT GRADED: P1 (0 passed, 1 failed) |
| P1-NOCOP | FAIL: P1-PRE-FRAME; NOT GRADED: P1-GROUP-B, P1-COLOUR-OF-HIGHEST-PRIORITY (8 passed, 1 failed) | FAIL: P1-PRE-FRAME; NOT GRADED: P1-GROUP-B, P1-COLOUR-OF-HIGHEST-PRIORITY (8 passed, 1 failed) |
| P1-TAGBAD | FAIL: P1-PRE-TAGS; NOT GRADED: P1-GROUP-A, P1-COLOUR-OF-HIGHEST-PRIORITY (8 passed, 1 failed) | FAIL: P1-PRE-TAGS; NOT GRADED: P1-GROUP-A, P1-COLOUR-OF-HIGHEST-PRIORITY (8 passed, 1 failed) |
| P1-TAGCOUNT | FAIL: P1-PRE-TAGS; NOT GRADED: P1-GROUP-A, P1-COLOUR-OF-HIGHEST-PRIORITY (8 passed, 1 failed) | FAIL: P1-PRE-TAGS; NOT GRADED: P1-GROUP-A, P1-COLOUR-OF-HIGHEST-PRIORITY (8 passed, 1 failed) |
| P1-SCROLL | FAIL: P1-PRE-WINDOW; NOT GRADED: P1-GROUP-A, P1-GROUP-B, P1-GROUP-E, P1-COLOUR-OF-HIGHEST-PRIORITY (6 passed, 1 failed) | FAIL: P1-PRE-WINDOW; NOT GRADED: P1-GROUP-A, P1-GROUP-B, P1-GROUP-E, P1-COLOUR-OF-HIGHEST-PRIORITY (6 passed, 1 failed) |
| P1-SPRENCLR | FAIL: P1-PRE-SPRITES; NOT GRADED: P1-PRE-VEXTENT, P1-GROUP-A, P1-GROUP-B, P1-GROUP-E, P1-COLOUR-OF-HIGHEST-PRIORITY (5 passed, 1 failed) | FAIL: P1-PRE-SPRITES; NOT GRADED: P1-PRE-VEXTENT, P1-GROUP-A, P1-GROUP-B, P1-GROUP-E, P1-COLOUR-OF-HIGHEST-PRIORITY (5 passed, 1 failed) |
| P1-BPL2ZERO | FAIL: P1-PRE-PLAYFIELDS; NOT GRADED: P1-GROUP-B, P1-COLOUR-OF-HIGHEST-PRIORITY (8 passed, 1 failed) | FAIL: P1-PRE-PLAYFIELDS; NOT GRADED: P1-GROUP-B, P1-COLOUR-OF-HIGHEST-PRIORITY (8 passed, 1 failed) |
| P1-VSTOP130 | FAIL: P1-PRE-VEXTENT; NOT GRADED: P1-GROUP-B, P1-COLOUR-OF-HIGHEST-PRIORITY (8 passed, 1 failed) | FAIL: P1-PRE-VEXTENT; NOT GRADED: P1-GROUP-B, P1-COLOUR-OF-HIGHEST-PRIORITY (8 passed, 1 failed) |
| P1-AVSTOP130 | FAIL: P1-PRE-VEXTENT; NOT GRADED: P1-GROUP-A, P1-COLOUR-OF-HIGHEST-PRIORITY (8 passed, 1 failed) | FAIL: P1-PRE-VEXTENT; NOT GRADED: P1-GROUP-A, P1-COLOUR-OF-HIGHEST-PRIORITY (8 passed, 1 failed) |
| P1-AMIRROR | FAIL: P1-GROUP-A, P1-COLOUR-OF-HIGHEST-PRIORITY (9 passed, 2 failed) | FAIL: P1-GROUP-A, P1-COLOUR-OF-HIGHEST-PRIORITY (9 passed, 2 failed) |
| P1-BSWAP | FAIL: P1-GROUP-B, P1-COLOUR-OF-HIGHEST-PRIORITY (9 passed, 2 failed) | FAIL: P1-GROUP-B, P1-COLOUR-OF-HIGHEST-PRIORITY (9 passed, 2 failed) |
| P1-PRIFLIP | FAIL: P1-GROUP-B, P1-COLOUR-OF-HIGHEST-PRIORITY (9 passed, 2 failed) | FAIL: P1-GROUP-B, P1-COLOUR-OF-HIGHEST-PRIORITY (9 passed, 2 failed) |
| P1-EREV | FAIL: P1-GROUP-E, P1-COLOUR-OF-HIGHEST-PRIORITY (9 passed, 2 failed) | FAIL: P1-GROUP-E, P1-COLOUR-OF-HIGHEST-PRIORITY (9 passed, 2 failed) |
| P2-LATEMARK | FAIL: P2-FIXTURE-RUN; NOT GRADED: P2 (0 passed, 1 failed) | FAIL: P2-FIXTURE-RUN; NOT GRADED: P2 (0 passed, 1 failed) |
| P2-NOCOP | FAIL: P2-PRE-FRAME; NOT GRADED: P2-COMBINE (4 passed, 1 failed) | FAIL: P2-PRE-FRAME; NOT GRADED: P2-COMBINE (4 passed, 1 failed) |
| P2-TAGID | FAIL: P2-PRE-TAGS; NOT GRADED: P2-COMBINE (4 passed, 1 failed) | FAIL: P2-PRE-TAGS; NOT GRADED: P2-COMBINE (4 passed, 1 failed) |
| P2-SCROLL | FAIL: P2-PRE-WINDOW; NOT GRADED: P2-COMBINE (4 passed, 1 failed) | FAIL: P2-PRE-WINDOW; NOT GRADED: P2-COMBINE (4 passed, 1 failed) |
| P2-BPL4PLANE2 | FAIL: P2-PRE-PLANE-FETCH; NOT GRADED: P2-COMBINE (4 passed, 1 failed) | FAIL: P2-PRE-PLANE-FETCH; NOT GRADED: P2-COMBINE (4 passed, 1 failed) |
| P2-SPRENCLR | FAIL: P2-COMBINE (5 passed, 1 failed) | FAIL: P2-COMBINE (5 passed, 1 failed) |
| P2-BPL4ZERO | FAIL: P2-COMBINE (5 passed, 1 failed) | FAIL: P2-COMBINE (5 passed, 1 failed) |
| P3-NOBIN | FAIL: P3-FIXTURE-LOADED; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (9 passed, 5 failed) | FAIL: P3-FIXTURE-LOADED; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (9 passed, 5 failed) |
| P3-NOCOPEN | FAIL: P3-FIXTURE-TRIGGERS; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (10 passed, 5 failed) | FAIL: P3-FIXTURE-TRIGGERS; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (10 passed, 5 failed) |
| P3-NODONE | FAIL: P3-FIXTURE-DRIVEN; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (11 passed, 5 failed) | FAIL: P3-FIXTURE-DRIVEN; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (11 passed, 5 failed) |
| P3-EVMIS | FAIL: P3-PRE-RECORDS; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (18 passed, 5 failed) | FAIL: P3-PRE-RECORDS; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (18 passed, 5 failed) |
| P3-ANCHOR | FAIL: P3-PRE-ANCHOR; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (18 passed, 5 failed) | FAIL: P3-PRE-ANCHOR; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (18 passed, 5 failed) |
| P3-READTIME | FAIL: P3-PRE-READTIME; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (18 passed, 5 failed) | FAIL: P3-PRE-READTIME; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (18 passed, 5 failed) |
| P3-DDRA | FAIL: P3-PRE-DDRA; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (18 passed, 5 failed) | FAIL: P3-PRE-DDRA; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (18 passed, 5 failed) |
| P3-NOPULSE | FAIL: P3-PRE-PULSE; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (18 passed, 5 failed) | FAIL: P3-PRE-PULSE; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (18 passed, 5 failed) |
| P3-E40LINE44 | FAIL: P3-PRE-LIVE; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (18 passed, 5 failed) | FAIL: P3-PRE-LIVE; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (18 passed, 5 failed) |
| P3-BEAMCON0 | FAIL: P3-PRE-PERIOD; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (11 passed, 1 failed) | FAIL: P3-PRE-PERIOD; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (11 passed, 1 failed) |
| P3-LPEN | no label turns red (20 passed, 8 failed) | no label turns red (20 passed, 8 failed) |
| P3-OFFSET | FAIL: P3-PRE-PULSE; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (18 passed, 5 failed) | FAIL: P3-PRE-PULSE; NOT GRADED: P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION, P3-FIELD-END-LATCH (18 passed, 5 failed) |
| P3-B1LINE181 | no label turns red (19 passed, 8 failed) | no label turns red (19 passed, 8 failed) |
| P4-NOBIN | FAIL: P4-FIXTURE-LOADED, P4-FIXTURE-A1 (0 passed, 2 failed) | FAIL: P4-FIXTURE-LOADED, P4-FIXTURE-A1 (0 passed, 2 failed) |
| P4-NOCOPEN | FAIL: P4-FIXTURE-A1 (1 passed, 1 failed) | FAIL: P4-FIXTURE-A1 (1 passed, 1 failed) |
| P4-NODONE | FAIL: P4-FIXTURE-DRIVEN (2 passed, 1 failed) | FAIL: P4-FIXTURE-DRIVEN (2 passed, 1 failed) |
| P4-DDRA | FAIL: P4-PRE-DDRA (8 passed, 3 failed) | FAIL: P4-PRE-DDRA (8 passed, 3 failed) |
| P4-ANCHOR | FAIL: P4-PRE-ANCHOR (8 passed, 3 failed) | FAIL: P4-PRE-ANCHOR (8 passed, 3 failed) |
| P4-YDELAY | FAIL: P4-PRE-READTIME (8 passed, 3 failed) | FAIL: P4-PRE-READTIME (8 passed, 3 failed) |
| P4-EXTRAPULSE-F2 | FAIL: P4-PRE-PULSE (8 passed, 3 failed) | FAIL: P4-PRE-PULSE (8 passed, 3 failed) |
| P4-PERIOD | FAIL: P4-PRE-PERIOD; NOT GRADED: P4-PRE-REFFRAME (7 passed, 3 failed) | FAIL: P4-PRE-PERIOD; NOT GRADED: P4-PRE-REFFRAME (7 passed, 3 failed) |
| P4-SHORT | FAIL: P4-PRE-REFFRAME (8 passed, 3 failed) | FAIL: P4-PRE-REFFRAME (8 passed, 3 failed) |
| P4-NOLPENDIS | no label turns red (9 passed, 2 failed) | no label turns red (9 passed, 2 failed) |
| P4-F1NOLPEN | no label turns red (9 passed, 2 failed) | no label turns red (9 passed, 2 failed) |
| P4-F3LPENDIS | no label turns red (9 passed, 2 failed) | no label turns red (9 passed, 2 failed) |

On the private copy of the RTL described above, every precondition holds and the line named moves, on both cores, under these selectors: P3-LPEN (`P3-HELD`, `P3-PEN-POSITION`, `P3-LATCHED-PEN-POSITION` and `P3-FIELD-END-LATCH` FAIL), P3-B1LINE181 (`P3-PEN-POSITION` and `P3-LATCHED-PEN-POSITION` FAIL, V(L5) = 181), P4-NOLPENDIS (`P4-LPENDIS-READ` FAIL, X = (0,100)), P4-F3LPENDIS (`P4-PRE-CONTROL` FAIL alone, `P4-LPENDIS-READ` NOT GRADED), P4-F1NOLPEN (`P4-PRE-LATCHF1` FAIL alone, `P4-LPENDIS-READ` NOT GRADED) and P4-EXTRAPULSE-F2 (`P4-PRE-PULSE` FAIL alone, `P4-LPENDIS-READ` NOT GRADED). The selectors P3-OFFSET and P3-NOPULSE fail `P3-PRE-PULSE` alone and print the four P3 lines NOT GRADED on both RTLs.

`P3-HELD`, `P3-PEN-POSITION`, `P3-LATCHED-PEN-POSITION`, `P3-FIELD-END-LATCH`, `P4-PRE-LATCHF1`, `P4-PRE-CONTROL` and `P4-LPENDIS-READ` carry implementation records. On the repository's RTL the selectors P3-LPEN, P3-B1LINE181, P4-F1NOLPEN, P4-F3LPENDIS and P4-NOLPENDIS leave the labels they name as they are, because those lines are already red or NOT GRADED there; the run prints a record (`P4-LPENDIS-READ-MUTATION`, for P4-NOLPENDIS) only where it says so, and only when a precondition of `P4-LPENDIS-READ` failed.
