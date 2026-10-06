# Specification: audio location registers, DMA control and the Copper danger bit (unit ch5)

This document specifies one integration test package for the 58 claims in
`claims-spec-stage.jsonl`: the audio location registers AUDxLCH/AUDxLCL, the
DMA control register pair DMACON/DMACONR, and the Copper danger bit as far as
it bears on the audio registers. It was written from those claims and from the
pages of the *Amiga Hardware Reference Manual*, 3rd edition, by an author who
never saw the implementation or any test. Page numbers are printed page
numbers. Quotations follow the printed scan; where the text layer of a page
differs from the scan, the scan is quoted and the difference is named.

## 1. Claims not turned into a proposition

| Claim(s) | Page | Why no proposition |
|---|---|---|
| `5-appA-p264.c2` | 263-264 | "If a register is marked as a write-only register, do not attempt to read from it, as this may trash the register and crash the system." The consequence is only possible ("may"), so there is no expected value. The page also forbids the read, and a read that did trash a register would spoil every other check of the run. No program in this package reads a write-only register. |
| `5-appA-p263.c7`, `5-appA-p263.c8`, `A-pre.c8` | 263 | "The custom chips can only access Chip memory; using a non-Chip address will fail". "Fail" names no observable outcome. The system under test has no memory besides Chip memory, so a "non-Chip address" could only be an address above the selected Chip-memory size, and no page says what the port, the video or the audio output then show (p.169 says of the blitter only that such use "may result in destruction of the contents of Chip memory", again a possibility). No expected value. |
| `5-appA-p263.c9`, `A-pre.c9` | 263 | "Disk data, sprite data, bitplane data, audio data, copper lists and anything that will be blitted or accessed by custom chip DMA must be located in chip memory." This is an instruction to the programmer. Every stimulus below obeys it, and P1 checks its positive side: audio DMA fetches its data from the Chip-memory location written. The page does not say what happens when the instruction is broken. |
| `5-appB-p287.c5` | 287 | "+ Address register pair. Must be an even address pointing to chip memory." Same kind of claim: every location and pointer written below is even and lies in Chip memory, and the page does not say what happens otherwise. |
| `5-appB-p290.c3` | 290 | "DMACON is assigned to Denise." The chip column says which chip decodes a register, and no output of the system shows that. The claim also comes from a text-layer error: the printed row on p.290 reads `DMACON 096 W A D P`, as p.278 does, but the text layer lost "A" and "P". |
| `C-interp.copcon`, `C-interp.copcon.ecs` | 310 | The claims name a whole range: "the Copper can access the address range from $DFF03E through $DFF07E" (standard chip set) and "all of the Amiga chip registers" (ECS). Grading them would need an observation of the Copper's write to every register of the range. P10 and P11 observe the eight audio location registers and `$054`, `$056`, `$058`; `$07E` (DSKSYNC) and the unassigned addresses have no output at the RAM port. The two ids are declined, and P10 and P11 carry none. |

## 2. Conventions shared by every proposition

### 2.1 Configuration and runs

- **Default configuration:** ECS, PAL, 2 MB Chip memory. A proposition that names
  OCS is run under both OCS and ECS, with the same expected values.
- **Fresh start:** every run starts from reset. The program sets the 68000
  status register to `$2700` first and uses no interrupts.
- **CPU:** every run is made once on each of the two CPU cores, both in
  68000 mode. The 68010/68020 settings are not used. The pages expect
  different CPUs to behave differently on the bus (p.263 on `CLR`, p.186 on a
  processor "running completely uninhibited out of Fast memory or its internal
  cache"), and P7 and P8 depend on the CPU needing Chip memory for every
  instruction. The expected values are the same on both cores.

### 2.2 Register access

- Addresses. p.263: "To write to these registers with the 680x0, calculate
  the 680x0 address using this formula: 680x0 address = (chip address) +
  $DFF000". Every register access below uses `$DFF000` plus the Appendix B
  address, with a word access (`MOVE.W`).
- Location and pointer pairs are written with one `MOVE.L` to the high
  register. p.263: "All of the "pointer" type registers are organized as 32
  bits on a long word boundary. These registers may be written with one MOVE.L
  instruction. The lowest bit of all pointers must be written as zero."
  p.139: "Since these two register addresses are contiguous, writing a long
  word into AUDxLCH moves the audio data address into both locations."
- Unused bits are written as 0 (p.264: "If a bit is described as unused in a
  write-only register, be sure to keep that bit clear when writing values to
  that register."). No DMACON write in this package sets bits 11-14:
  - bits 12 and 11 are "X" on p.278 and "Unassigned" on p.222;
  - bits 14 and 13 are read-only status bits.
- No program writes a read-only register or reads a write-only register (the
  warning on p.263-264).
- A strobe is written with `MOVE.W`. p.263: "When strobing any register which
  responds to either a read or a write, (for example copjmp2) be sure to use a
  MOVE.W, not CLR.W". p.287 defines "S Strobe (write address with no register
  bits). Writing the register causes the effect."

### 2.3 Memory map

All addresses are byte addresses. They are even. Except SA, SB and SC they lie
below `$80000`, so a 3-bit high location word (OCS, p.139 "high three bits")
reaches them as well as a 5-bit one (ECS, p.266, p.298, p.309). SA, SB and SC
are blitter source pointers used under the default ECS configuration only, where
a blitter pointer keeps its high 5 bits (p.309).

| Name | Address | Content |
|---|---|---|
| A_x (x = 0..3) | `$60000 + $1000*x` | audio data, 8 words (any values) |
| A_x^L | `A_x + $0100` | copy of the audio data (P1 contrast C2) |
| L | `$64000` | Copper list (at most 64 bytes) |
| S_0 | `$65000` | sprite 0 structure of p.109: `$6D60,$7200, $0990,$07E0, $13C8,$0FF0, $23C4,$1FF8, $13C8,$0FF0, $0990,$07E0, $0000,$0000` |
| N_k (k = 1..7) | `$65000 + $100*k` | null sprite `$0000,$0000` |
| F | `$66000` | 16 words `$FFFF` |
| Zb | `$66100` | 16 words `$0000` |
| P | `$68000` | bitplane area, `$8000` bytes, all zero |
| A_x^H | `$70000 + $1000*x` (= `A_x + $10000`) | copy of the audio data (P1 contrast C1) |
| B_x | `$70800 + $1000*x` | copy of the audio data (the location the Copper writes in P10) |
| Q | `$74000` | blitter destination area, `$2000` bytes, all zero before each run |
| Q2 | `$75000` | second blitter destination (P11), `$20` bytes |
| G | `$78000` | CPU log area, `$4000` bytes |
| R | `$7C000` | stored results (register values read by the program) |
| MK | `$7D000` | marker word |
| SA, SB, SC | `$A0000`, `$A2000`, `$A4000` | P7's blitter sources A, B and C, 4096 words each, all zero |

The program image and its stack lie in Chip memory outside `$60000-$7FFFF`
and outside SA-SC.
Every audio, sprite, bitplane, Copper and blitter-source area above is a
**DMA block**.

### 2.4 Attributing port accesses

- **Addresses:** the test converts the port's word address into the byte
  address of the word's first byte. "A read at X" means a port read whose byte
  address is X. "Reads in [X, Y)" means a half-open byte range.
- **DMA accesses** are attributed by block. In a run, each DMA block is used
  by one agent only, and the program does not touch a DMA block inside an
  observation window (precondition G3). A read in a DMA block inside a window
  therefore counts as a fetch by that block's agent, and a write in Q or Q2 as
  a blitter destination write.
- **CPU accesses** are the accesses to the program image, the stack, G, R and
  MK.
- **Refresh:** if the port record marks refresh accesses (or names the
  requester of each access), the test uses that mark to leave refresh out. If
  it does not, refresh accesses are not removed. A refresh access that falls
  into a block can then only turn a zero expectation red; it can never make a
  check green.
- Slot parity and the slot numbers of Figure 6-9 are not used as an anchor,
  for two reasons:
  - The figure itself says "Actual system hardware demands certain specific
    values for data fetch start and display start. Therefore this timing
    chart has been "adjusted" to match those requirements" (p.195).
  - p.20 and p.196 assign the even cycles differently (section 4).

### 2.5 Markers, windows and frames

- **Marker:** the program writes a distinct word to MK (`MOVE.W #id,MK`).
  The port shows that write with its time. An **observation window** runs
  from one marker write to a later one. A window that measures an enable is
  opened by a marker written *before* the enabling DMACON write, so that no
  fetch the enable causes can fall before the window.
- **Frames:** the test counts vertical-sync pulses on the video output. V(W)
  is the number of vertical-sync pulse starts inside window W. The program
  waits with a counted delay loop. The test does not assume the loop's
  length; it checks V(W).
- **Results:** a stored register value is read from the port record of the
  CPU write that stored it into R or G. That record also gives the store's
  time.

### 2.6 Common preconditions and their contrasting cases

| Id | The test asserts | Contrasting stimulus that turns the assertion red |
|---|---|---|
| G1 | The program finished: the final marker `END` was written, and every result word the proposition names was stored. | Insert a branch-to-self before `END`: G1 goes red. |
| G2 | The DMACONR value that the program reads and stores right after each enabling DMACON write shows the intended bits among bits 10 and 8-0, and DMAEN (bit 9) set (the readback is P2). | Leave bit 9 out of the enabling write: the stored value shows bit 9 = 0, G2 goes red. p.144, "For any audio DMA to occur (or any other DMA, for that matter), the DMAEN bit in DMACON must be set", also predicts that the effect is absent. |
| G3 | No CPU write falls into any DMA block during any window. | Add one `MOVE.W` into the block under test inside the window: G3 goes red. |
| G4 | V(W) is at least the value the proposition names. | Set the delay count to 0: V(W) falls short, G4 goes red. |
| G5 | Of DMACON bits 8-0, only the channels the proposition names are set during the window (from the same stored DMACONR value). | Also set SPREN (`$8020`) before the window: G5 goes red. |

Every contrasting case of a proposition that this table does not cover is
also graded in the normal run, as an assertion of its own (P3-OFF, P9-SWAP and
so on), because each is a prediction of the pages. The mutation of the main
assertion puts the contrasting program through the main assertion's unchanged
comparator, and the mutation of the contrasting assertion puts the main program
through the contrasting assertion's unchanged comparator. The preconditions are
graded over every run of the proposition, the contrasting runs included.

## 3. Propositions

### P1: The CPU writes AUDxLCH and AUDxLCL, and each channel fetches its data from the location written

**Claims:**
- p.266: `5-appA-p266.c2`, `5-appA-p266.c8`, `A-audxlch.acc`, `A-audxlcl.acc`
- p.290: `5-appB-p290.c20`, `.c27`, `.c55`, `.c62`, `.c90`, `.c97`, `.c125`,
  `.c132`, and `B-aud0lch.acc`, `B-aud0lcl.acc`, `B-aud1lch.acc`,
  `B-aud1lcl.acc`, `B-aud2lch.acc`, `B-aud2lcl.acc`, `B-aud3lch.acc`,
  `B-aud3lcl.acc`
- p.298: `5-appC-p298.c4`, `C-map.lch.acc`
- p.309: `5-appC-p309.c4`
- p.263, for the addresses: `5-appA-p263.c1`, `A-pre.c1`

**Proposition.** For x = 0..3, when the 68000 writes an even Chip address A
to AUDxLCH (`$DFF0A0 + $10*x`, high word) and AUDxLCL (`$DFF0A2 + $10*x`, low
word), channel x fetches its data from A once its DMA runs, returns to A after
every pass, and no other channel takes the value.

**What "W" lets a test grade.** p.287 reads "W=write-only". The write half is
graded here. The "only" half is not: the pages give no outcome for a read
except "may trash the register and crash the system" (p.264), so it has no
expected value, and no program reads these registers.

**Stimulus.** Run under OCS and ECS. The steps follow p.145, "Set the location
registers ... Set the length register ... Set the volume register ... Set the
period register ... Start the audio DMA".

1. `DMACON = $07FF` (all DMA off). `ADKCON = $00FF` clears attach bits 7-0
   (p.265, p.151), so each channel plays alone.
2. Fill A_x, A_x^L and A_x^H with 8 words each.
3. For x = 0..3:
   - `MOVE.L #A_x,$DFF0A0+$10*x`
   - `MOVE.W #8,$DFF0A4+$10*x` (AUDxLEN)
   - `MOVE.W #64,$DFF0A8+$10*x` (AUDxVOL)
   - `MOVE.W #200,$DFF0A6+$10*x` (AUDxPER; p.266: "the smallest number that
     should be placed in this register is 124 decimal")
4. For x = 0, 1, 2, 3 in turn:
   - write marker `O_x`;
   - `DMACON = $8200 | (1<<x)` (p.145 step 7);
   - read DMACONR into R;
   - delay;
   - write marker `C_x`;
   - `DMACON = (1<<x)` to stop the channel (p.145: "You can stop the channel
     by writing a 0 into the AUDxEN bit at any time.");
   - delay for a gap of at least one vertical-sync pulse before the next
     `O_(x+1)`. p.145: "If the channel is disabled for a very short time (less
     than two sampling periods) it may stay on".
5. Write marker `END`.

The window is W_x = [t(`O_x`), t(`C_x`)].

**Observation.** Port reads inside W_x at A_0..A_3, and the addresses the
contrasting cases name.

**Expected.**
- (a) At least 2 reads at A_x in W_x. The sources:
  - p.266: "This pair of registers contains the 18 bit starting address
    (location) of audio channel x (x=0,1,2,3) DMA data. This is not a pointer
    register and therefore needs to be reloaded only if a different memory
    location is to be outputted."
  - p.139: "Each time the system retrieves the last audio word from the data
    area, it uses the contents of these location registers to again find the
    start of the data."
  - p.144: "The system uses the value in the location registers each time it
    replays the waveform."

  With a length of 8 words and a period of 200, one pass takes far less than
  a frame (p.141: "Each audio channel is allocated one DMA slot per horizontal
  scan line"), so a window with V ≥ 2 holds many passes, and "at least 2" is
  a floor.
- (b) No read at A_y for y ≠ x in W_x. Channel y is off: p.144, "When both
  DMAEN and AUDxEN are set, the DMA will start for channel x", and p.145 for
  stopping it.
- (a) and (b) are graded for every x. That is the "every channel" of the
  claims' "x".

**Preconditions.**
- G1, G2 (bits 9 and x set), G3, G5 (no audio bit other than x, no other
  channel).
- G4 with V(W_x) ≥ 2, and at least one vertical-sync pulse start between
  `C_x` and `O_(x+1)`.

**Contrasting cases.** Each keeps the program and all preconditions intact.
- C1, the high word: write `A_x^H = A_x + $10000`. Only the word that goes to
  AUDxLCH changes. Expected: at least 2 reads at A_x^H in W_x and none at
  A_x.
- C2, the low word: write `A_x^L = A_x + $0100`. Only the word that goes to
  AUDxLCL changes. Expected: at least 2 reads at A_x^L and none at A_x.
- C3, register to channel: swap the values written to channel 0 and channel 1.
  Expected: in W_0, reads at A_1 and none at A_0; in W_1, the reverse.

C1 and C2 show that each of the two registers takes its own write. C3 shows
that the address (chip address + `$DFF000`) selects the channel. The contrasts
for the preconditions are those of section 2.6.

**Reference frame.** Only the marker windows and the vertical-sync count.

**Open, not graded.**
- The number of fetches in a window.
- Which address the first fetch after an enable uses. p.164: "Because of
  pipelining in Agnus, the first data word must be thrown away", without
  saying from where.
- The ECS extension of AUDxLCH to 5 bits. It belongs to other claims, and the
  addresses here stay below `$80000`.

### P2: DMACONR at $DFF002 reads back the DMA control bits held by DMACON

**Claims:**
- p.278: `5-appA-p278.c5`, `.c6`, `.c8`, `A-dmaconr.acc`,
  `A-dmaconr.addr`
- p.289: `5-appB-p289.c1`, `.c2`, `.c4`
- p.263: `5-appA-p263.c1`, `A-pre.c1`

**Proposition.** A word read at `$DFF002` returns, in bits 10 and 8-0, the
control bits last set or cleared through DMACON at `$DFF096`. Every one of
these bits reads 1 after a SET write of it and 0 after a CLR write of it,
whatever the other bits hold.

**Sources.**
- p.278: "DMACONR 002 R A P DMA control (and blitter status) read" and "This
  register controls all of the DMA channels and contains blitter DMA status
  bits."
- p.278, bit 15: "Set/clear control bit. Determines if bits written with a 1
  get set or cleared. Bits written with a zero are unchanged."
- p.222: "There is a read address as well as a write address to the DMA
  control register so you can tell which DMA channels are enabled." and "The
  contents of this register are shown in Table 7-6 (bit on if enabled)."

**Stimulus.** Run under OCS and ECS. First a setup that keeps every channel
harmless while it is enabled:

- `DMACON = $07FF`.
- Copper: list L holds only `$FFFF,$FFFE` (the end-of-list wait of p.30).
  Then `MOVE.L #L,$DFF080` (COP1LCH) and a `MOVE.W` to `$DFF088` (COPJMP1).
  p.30 requires a location register and its strobe to be written before
  Copper DMA is turned on.
- Bitplanes: `BPLCON0 ($100) = $0200`, zero bitplanes (bits 12-14, p.63).
- Sprites: all eight SPRkPT (`$120 + 4*k`) point at null sprites (N_k, and
  S_0 filled with `$0000,$0000` for k = 0). p.114: "all eight sprite pointers
  must be initialized to either a real sprite or a safe null sprite."
- Audio: as P1 step 3, but with AUDxVOL = 0.
- Disk: `DSKLEN ($024) = $4000`, which p.247 names as the write that turns
  disk DMA off. Disk DMA starts only after DSKLEN is written twice (p.247),
  and the program never does that.
- Blitter: BLTSIZE is never written.

Then the sequence below. After each write, the program reads `$DFF002` with
`MOVE.W` and stores the value in the next word of R.

| Step | DMACON write | Expected (read AND `$07FF`) |
|---|---|---|
| 1 | `$07FF` | `$0000` |
| 2 | `$8200` | `$0200` |
| 3 | for each b in 0, 1, 2, 3, 4, 5, 6, 7, 8, 10: first `$8000 + 2^b`, then `2^b` | `$0200 + 2^b` after the first write, `$0200` after the second |
| 4 | `$85FF` | `$07FF` |
| 5 | `$0200` | `$05FF` |
| 6 | `$8200` | `$07FF` |
| 7 | `$07FF` | `$0000` |

**Observation.** The 26 stored values.

**Expected.** Each stored value, masked with `$07FF`, equals its row. Step 3
grades every control bit on its own, which is the "all of the DMA channels"
of p.278. Step 5 grades "Bits written with a zero are unchanged".

**Not graded.**
- Bit 15: the pages define it for writes only.
- Bits 12 and 11: p.264, "do not rely on the values of unused bits when
  reading from a read only register".
- Bits 14 and 13: see P8 and P9. Before any blit, bit 13 has no stated value
  (p.187: "The Zero flag is only valid after the blitter has completed its
  operation").
- The "only" of "read-only": p.263, "Do not attempt to write to a read-only
  register, as this will cause unpredictable results". No program writes
  `$DFF002`.

**Preconditions.** G1: all 26 values were stored.

**Contrasting cases.**
- Omit the SET write for one bit b in step 3. The next stored value shows
  bit b = 0 and the row goes red; the program still finishes.
- For G1: the branch-to-self of section 2.6.

**Reference frame.** None.

### P3: BPLEN (bit 8) turns bitplane DMA on

**Claims:** p.278: `5-appA-p278.c17`, `A-dmacon.b08`.

**Proposition.** With DMAEN set, setting BPLEN makes bitplane DMA fetch from
the bitplane pointer. With BPLEN clear, no bitplane fetch happens.

**Sources.**
- p.278: "08 BPLEN Bitplane DMA enable".
- p.62: "You turn on bitplane DMA by writing a 1 to bit BPLEN in the DMACON
  (for DMA control) register."
- p.222, DMAEN: "It enables the DMA for all of the channels at bits 8-0."

**Stimulus.** Default configuration.

1. `DMACON = $07FF`.
2. Write the playfield registers:
   - `BPLCON0 ($100) = $1200`: one bitplane (bits 12-14, p.63);
   - `BPLCON1 ($102) = 0`;
   - `DIWSTRT ($08E) = $2C81` and `DIWSTOP ($090) = $2CC1`, the normal PAL
     values of p.59;
   - `DDFSTRT ($092) = $0038` and `DDFSTOP ($094) = $00D0`, the normal
     low-resolution values of p.60;
   - `BPL1MOD ($108) = 0` (p.62);
   - `MOVE.L #P,$DFF0E0` (BPL1PTH).
3. Write marker `O`. `DMACON = $8300`. Read DMACONR into R.
4. Delay. Write marker `C`. `DMACON = $07FF`. Write `END`.

**Observation.** Port reads in [P, P + `$8000`) inside [t(`O`), t(`C`)].

**Expected.** At least one read at P, the first word the pointer addresses,
and more than zero reads in the range.

**Preconditions.**
- G1, G2 (bits 9 and 8 set), G3, G5 (bit 8 is the only channel bit set).
- G4 with V ≥ 3, so that at least two whole frames, and with them the
  display-window lines, lie inside the window.

**Contrasting cases.**
- `DMACON = $8200` in step 3, so BPLEN stays clear. Expected: no read in
  [P, P + `$8000`). G2 then expects bit 9 set and bit 8 clear. This run is
  graded in the normal run as P3-OFF.
- For the preconditions: section 2.6.

**Reference frame.** Frames by vertical-sync pulses (section 2.5). The test
checks the anchor by counting them.

**Not graded.** How many words are fetched. That follows from DDFSTRT, DIWSTRT
and pointer behaviour, which are other claims.

### P4: COPEN (bit 7) turns Copper DMA on

**Claims:** p.278: `5-appA-p278.c18`, `A-dmacon.b07`.

**Proposition.** With DMAEN set, setting COPEN makes the Copper fetch its list
from COP1LC and fetch it again at every vertical blanking. With COPEN clear,
the Copper fetches nothing.

**Sources.**
- p.278: "07 COPEN Copper DMA enable".
- p.30: "You can also stop the Copper by disabling its ability to use DMA for
  retrieving instructions or placing data. The register called DMACON
  controls all of the DMA channels. Bit 7, COPEN, enables Copper DMA when set
  to 1."
- p.30: "Now, if the contents of COP1LC are not changed, every time vertical
  blanking occurs the Copper will restart at the same location for each
  subsequent video screen."

**Stimulus.** Default configuration, in the order p.30 prescribes.

1. `DMACON = $07FF`.
2. List at L: `$0180,$0F00` (MOVE to COLOR00), then `$FFFF,$FFFE`.
3. `MOVE.L #L,$DFF080` (COP1LCH), then `MOVE.W D0,$DFF088` (COPJMP1).
4. Write marker `O`. `DMACON = $8280`. Read DMACONR into R.
5. Delay. Write marker `C`. `DMACON = $07FF`. Write `END`.

**Observation.** Port reads in [L, L + 8) inside [t(`O`), t(`C`)].

**Expected.** At least 2 reads at L. Each frame period holds one vertical
blanking. A window that contains three vertical-sync pulse starts contains two
whole frame periods, and so at least two vertical blankings, each of which
restarts the Copper at L (p.30).

**Preconditions.** G1, G2 (bits 9 and 7 set), G3, G5, and G4 with V ≥ 3.

**Contrasting cases.**
- `DMACON = $8200` in step 4. Expected: no read in [L, L + 8). This run is
  graded in the normal run as P4-OFF.
- For the preconditions: section 2.6.

**Reference frame.** Frames by vertical-sync pulses. The relation to vertical
blanking uses only the fact that both occur once per frame.

**Not graded.** The effect of the MOVE on the video output. That belongs to
the colour-register claims.

### P5: BLTEN (bit 6) turns blitter DMA on

**Claims:** p.278: `5-appA-p278.c19`, `A-dmacon.b06`.

**Proposition.** With DMAEN set and BLTEN set, a BLTSIZE write makes the
blitter write its destination. With BLTEN clear, the blitter makes no memory
access.

**Sources.**
- p.278: "06 BLTEN Blitter DMA enable".
- p.186: "When the BLTSIZE register is written the blit is started."
- p.269, BLTSIZE: "Writing to this register will start the blitter, and
  should be done last, after all pointers and control registers have been
  initialized."

**Stimulus.** Default configuration.

1. `DMACON = $07FF`. The CPU clears [Q, Q + `$200`).
2. Set up the blit:
   - `BLTCON0 ($040) = $01FF`: USED only (bit 8, p.267); LF = `$FF`, so every
     line of the truth table gives D = 1 (p.174);
   - `BLTCON1 ($042) = 0`: area mode (p.267);
   - `BLTAFWM ($044) = BLTALWM ($046) = $FFFF`;
   - `BLTDMOD ($066) = 0`;
   - `MOVE.L #Q,$DFF054` (BLTDPTH).
3. Write marker `O`. `DMACON = $8240`. Read DMACONR into R.
4. `BLTSIZE ($058) = $0410`: height 16, width 16 (bit layout of p.269).
5. Delay. Write marker `C`. Write `END`. The program writes no blitter
   register after BLTSIZE (p.187) and does not poll the busy bit.

**Observation.** Port writes in [Q, Q + `$200`) inside [t(`O`), t(`C`)].

**Expected.** At least one write.

**Preconditions.** G1, G2 (bits 9 and 6 set), G3, G5, and G4 with V ≥ 1.
- G3 for the block under test. With BLTEN set: no write in [Q + `$200`,
  Q + `$2000`), and each of the 256 words of the blit's footprint
  [Q, Q + `$200`) written exactly once (BLTSIZE `$0410` is height 16 and
  width 16, p.269), so that no CPU write lies among the blitter's. With
  BLTEN clear: no write in [Q, Q + `$2000`).

**Contrasting cases.**
- For G3 inside the footprint: one extra CPU write onto the first word of
  [Q, Q + `$200`) inside the window. That word is then written twice and G3
  goes red.
- `DMACON = $8200` in step 3. Expected: no write in [Q, Q + `$200`), and no
  write anywhere outside the CPU areas. This run is graded in the normal run
  as P5-OFF.
- For the preconditions: section 2.6.

**Not graded.**
- The number and data of the writes. They follow from BLTSIZE and minterm
  claims.
- What becomes of a BLTSIZE write made while BLTEN is clear. The pages are
  silent, so the contrasting program never sets BLTEN afterwards and never
  waits on BBUSY.

### P6: SPREN (bit 5) turns sprite DMA on for all eight sprites

**Claims:** p.278: `5-appA-p278.c20`, `A-dmacon.b05`.

**Proposition.** With DMAEN set, setting SPREN makes each of the eight sprite
channels read the first two words of the structure its pointer addresses.
With SPREN clear, no sprite fetch happens.

**Sources.**
- p.278: "05 SPREN Sprite DMA enable".
- p.114: "When you enable sprite DMA for one sprite, you enable DMA for all
  the sprites and place them all in automatic mode."
- p.126: "the 18-bit sprite pointer register (composed of SPRxPTH and
  SPRxPTL) is used to read the first two words from the sprite data
  structure."

**Stimulus.** Default configuration, following the steps of p.109 ("2. Set
the sprite pointers ... 3. Turn on sprite direct memory access").

1. `DMACON = $07FF`.
2. `MOVE.L #S_0,$DFF120` and `MOVE.L #N_k,$DFF120+4*k` for k = 1..7. p.110:
   "As usual, you can write a long word into SPRxPTH." All eight pointers are
   set, as p.114 requires. The pointers are written while sprite DMA is off,
   so no DMA moves them before the enable. p.110's "During the vertical
   blanking interval before the first display of the sprite" therefore does
   not affect this check.
3. Write marker `O`. `DMACON = $8220`. Read DMACONR into R.
4. Delay. Write marker `C`. Write `END`.

Sprite DMA stays on to the end. p.109 recommends turning it off "only during
vertical blanking", and the program does not turn it off.

**Observation.** Port reads at S_0, S_0 + 2, N_k and N_k + 2 (k = 1..7), and
in the blocks [S_0, S_0 + `$100`) and [N_k, N_k + `$100`), inside
[t(`O`), t(`C`)].

**Expected.** Each of the 16 addresses is read at least once. That grades the
"all the sprites" of p.114 over all eight channels.

**Preconditions.**
- G1, G2 (bits 9 and 5 set), G3, G5, and G4 with V ≥ 3.
- Bitplane DMA is off (bit 8 clear in the stored value). p.114: "some sprites
  can become unusable when additional DMA cycles are allocated to displaying
  the screen".

**Contrasting cases.**
- `DMACON = $8200` in step 3. Expected: no read in any of the eight blocks.
  This run is graded in the normal run as P6-OFF.
- For the bitplane precondition: also set BPLEN. The stored value shows
  bit 8 set, and the precondition goes red.
- For the other preconditions: section 2.6.

**Reference frame.** Frames by vertical-sync pulses.

**Not graded.**
- When in the frame the control words are read.
- Any read beyond N_k + 2. p.126 says that after a zero-height sprite "The
  next data word pair will be fetched ... It will instead become the next
  pair of data words for SPRxPOS and SPRxCTL."

### P7: With BLTPRI (bit 10) set, the CPU gets no Chip-memory cycle while blitter DMA runs

**Claims:** p.278: `5-appA-p278.c15`, `A-dmacon.b10`.

**Proposition.** While blitter DMA runs with BLTPRI set, the 68000 makes no
Chip-memory access. With BLTPRI clear, it makes some.

**Sources.**
- p.278: "10 BLTPRI Blitter DMA priority (over CPU micro) (also called
  "blitter nasty") (disables /BLS pin, preventing micro from stealing any bus
  cycles while blitter DMA is running)."
- p.222: "When this is a 1, the blitter has full (instead of partial) priority
  over the 680x0."
- p.198: "If DMAF_BLITHOG is a 1, the blitter will keep the bus for every
  available Chip memory cycle." and "If DMAF_BLITHOG is a 0, the DMA manager
  will monitor the 68000 cycle requests. If the 68000 is unsatisfied for three
  consecutive memory cycles, the blitter will release the bus for one cycle."
- The printed p.198 gives no bit number. DMAF_BLITHOG, "the "blitter-nasty"
  bit", is tied to bit 10 by the name on p.278 and p.222.
- p.188, Table 6-2 ("Typical Blitter Cycle Sequence"), gives the cycle
  sequence of each USE code. USE code E (channels A, B and C, no D) is
  `A0 B0 C0 A1 B1 C1 A2 B2 C2`: every cycle of it is a blitter access. USE
  code 1 (D only) is `D0 - D1 - D2`: a cycle without a blitter access lies
  between every two destination writes. p.189: "This Table is only meant to be
  an illustration of the typical order of blitter cycles on the bus."
- p.194, on the devices with a higher priority than the 68000: "If a device
  does not request one of its allocated time slots, the slot is open for other
  uses."

**The blit used.** P7 uses USE code E, so that no cycle between its first and
last source read is one the blitter does not use. A blit whose sequence has a
cycle without a blitter access (D only, for example) is not graded. p.198 says
the blitter "will keep the bus for every available Chip memory cycle", and
p.194 says an unrequested slot "is open for other uses"; the pages do not say
whether BLTPRI withholds from the 68000 a cycle the blitter does not request.

**Stimulus.** Default configuration.

1. `DMACON = $07FF`. Set up the blitter:
   - `BLTCON0 ($040) = $0EFF`: USEA, USEB and USEC set and USED clear (bits
     11-8, p.267), LF = `$FF`;
   - `BLTCON1 ($042) = 0`: area mode;
   - `BLTAFWM ($044) = BLTALWM ($046) = $FFFF`;
   - `BLTAMOD ($064)`, `BLTBMOD ($062)`, `BLTCMOD ($060)` and
     `BLTDMOD ($066)` = 0;
   - `MOVE.L #SA,$DFF050`, `MOVE.L #SB,$DFF04C` and `MOVE.L #SC,$DFF048`:
     three blocks of 4096 words, zero-filled (section 2.3).
2. `DMACON = $8640` (SET, BLTPRI, DMAEN, BLTEN). Read DMACONR into R.
3. Write marker `O`. `BLTSIZE = $2020`: height 128, width 32, 4096 words per
   channel.
4. Straight after, the CPU runs this loop from Chip memory, with D2 = 2047
   and A4 = G:

   ```
   L: MOVE.W D1,(A4)+
      ADDQ.W #1,D1
      DBRA   D2,L
   ```

5. Write marker `C`. Write `END`. After `END` the program loops with
   `BRA.S *` and never executes `STOP`.

**Observation.**
- T_first and T_last are the times of the first and last port read in
  [SA, SC + `$2000`), the blitter's A, B and C fetches (the 68000 never reads
  there). The interval (T_first, T_last) is "while blitter DMA is running".
- The test counts the CPU accesses (section 2.4) whose time lies inside that
  interval.

**Expected.** Zero CPU accesses in (T_first, T_last). "Any" is graded over
every cycle of the interval. This is graded as P7-ON.

**Preconditions.**
- G1.
- The stored value shows bits 10, 9 and 6 set and no other bit among 8-0.
- The blit ran: at least 2 source reads, so T_first < T_last.
- The CPU still wanted the bus when the blit ended: at least one CPU write
  into G falls after T_last, with a loop count below 2048.

**Contrasting cases.**
- `DMACON = $8240` in step 2, so BLTPRI is clear. Expected: at least one CPU
  access in (T_first, T_last), by p.198. All preconditions still hold, with
  bit 10 clear in the stored value. This run is graded in the normal run as
  P7-OFF.
- For the comparators of P7-ON and P7-OFF: the CPU executes `STOP #$2700`
  straight after the BLTSIZE write. It makes no access while the blit runs, so
  the unchanged zero-access comparator of P7-ON holds on it and the unchanged
  at-least-one comparator of P7-OFF goes red.
- For the "blit ran" precondition: replace the BLTSIZE write by a write to
  COLOR00 (`$180`). No blit starts and the precondition goes red.
- For the "CPU still wanted the bus" precondition: the same program that
  executes `STOP #$2700` straight after the BLTSIZE write. No log write follows
  T_last, and the precondition goes red.

**Reference frame.** The blit interval is anchored on the first and last
source read at the port. The test checks the anchor by requiring at least two
such reads.

**Not graded.**
- The /BLS pin: it is inside the chips.
- CPU accesses before T_first or after T_last.
- How many cycles the CPU gets with BLTPRI clear. p.198 gives the release
  rule but not the 68000's request pattern, so the contrasting case grades
  only "at least one".
- A blit whose Table 6-2 sequence leaves a cycle without a blitter access (see
  "The blit used").

### P8: BBUSY (bit 14) reads 1 while a blit is in progress and 0 when no blit is in progress

**Claims:**
- p.278: `5-appA-p278.c13`, `A-dmacon.b14`
- status part: `5-appA-p278.c10`, `5-appA-p278.c8` (p.278) and
  `5-appB-p289.c4` (p.289)

**Proposition.** Every DMACONR read that the program makes while a blit is in
progress (other than the first read after BLTSIZE) shows bit 14 = 1. Reads
made before any blit show 0. After the blit, the bit reads 0 and stays 0.

**Sources.**
- p.278: "14 BBUSY Blitter busy status bit (read only)".
- p.186: "A blitter done flag, also called the blitter busy flag, is provided
  as DMAF_BLTDONE in DMACONR. This flag is set when a blit is in progress."
  The printed page gives no bit number, although the text layer adds
  "(bit 14)"; bit 14 comes from p.278 and p.222.
- p.186: "If a blit has just been started but has been locked out of memory
  access because of, for instance, display fetches, this bit may not yet be
  set." and "The first "test" of the blitter done bit may not return the
  correct result, but the second will."
- p.187: "You should also check the blitter done flag before using results of
  a blit. The blit may not be finished, so the data may not be ready yet."

**Stimulus.** Default configuration.

1. `DMACON = $07FF`. Set up the blitter as in P5 step 2, with BLTDPT = Q.
2. `DMACON = $8240`. BLTPRI stays clear, so the CPU gets cycles during the
   blit (p.198).
3. Four idle reads: `MOVE.W $DFF002,(A4)+` with A4 = G.
4. Write marker `O`. `BLTSIZE = $2020`.
5. Poll: `MOVE.W $DFF002,(A4)+` again and again. Stop after the first read,
   other than the first read after BLTSIZE, whose bit 14 is 0, or after 8192
   reads. Then make 8 more logged reads.
6. Write marker `C`. Write `END`.

**Observation.**
- Each logged read i gives a value v_i and the time T_w(i) of its store at
  the port. The read itself lies between T_w(i-1) and T_w(i).
- T_last is the time of the last write in [Q, Q + `$2000`).
- Reads after BLTSIZE are numbered 1, 2, and so on. The logged reads are
  collected from G alone, [G, G + `$4000`), so that R's stored word is not
  among them.

**Expected.**
- (a) The 4 idle reads show bit 14 = 0: no blit is in progress.
- (b) Every read i ≥ 2 with T_w(i) < T_last shows bit 14 = 1. It was made
  after BLTSIZE and before the last destination write, so the blit was in
  progress (p.186). "Every" is graded over all such reads.
- (c) The poll ends on a 0 before the 8192-read limit, and every read after
  that first 0 is also 0 (the 8 extra reads).
- (d) Read 1 is not graded. p.186 says the first test "may not return the
  correct result", and p.187's note ("Starting with the Fat Agnus the blitter
  busy bit has been "fixed" to be set as soon as you write to BLTSIZE") does
  not say which Agnus the OCS or ECS setting stands for.
- (e) A read whose interval straddles T_last, or falls just after it, is
  graded only through (c). The pages do not say how soon after the last
  destination write the flag clears.

**Preconditions.**
- G1.
- The stored values of (a) show bits 9 and 6 set, bit 10 clear, and no other
  bit among 8-0.
- The blit ran: T_last is defined.
- At least 2 reads with i ≥ 2 have T_w(i) < T_last, so that (b) is not
  vacuous.

**Contrasting cases.**
- Replace the BLTSIZE write of step 4 by the same `MOVE.W` to COLOR00
  (`$180`). No blit starts, so the reads at the indices graded 1 in the main
  run are predicted 0 (p.186 sets the flag only while a blit is in progress)
  and (b) goes red. (b) is graded on this run at the indices that the main run
  grades and this run holds. The program still finishes, because the poll
  stops at read 2, and G1 and the DMACONR precondition still hold.
- For the "at least 2 reads inside the blit" precondition: delay the first poll
  read until the blit has ended, with a delay loop between the BLTSIZE write
  and the poll. No read i ≥ 2 falls before T_last, and the precondition goes
  red. The blit still runs, so T_last is defined.
- For the "BLTPRI clear" part of the DMACONR precondition: set BLTPRI in
  step 2. The stored value shows bit 10 and the precondition goes red.

**Reference frame.**
- The blit interval is anchored on the port's destination writes: P8's blit
  writes D.
- Each read's time is bracketed by the port times of two consecutive log
  stores.

**Not graded.** The "only" of "read only": what a DMACON write with bit 14
set does. Bit 15 on p.278 says "bits written with a 1 get set or cleared",
the bit itself is "(read only)", and no page gives the outcome. Every DMACON
write here keeps bit 14 clear.

### P9: After a blit, BZERO (bit 13) reads 1 exactly when the blit's logic output was zero throughout

**Claims:**
- p.278: `5-appA-p278.c14`, `A-dmacon.b13`
- status part: `5-appA-p278.c10`, `5-appA-p278.c8` (p.278) and
  `5-appB-p289.c4` (p.289)

**Proposition.** After a blit has completed, DMACONR bit 13 is 1 if every word
of the blit's logic output was zero, and 0 otherwise, also when the
destination channel is off.

**Sources.**
- p.278: "13 BZERO Blitter logic zero status bit (read only)."
- p.222: "Blitter zero status - read-only. Remains 1 if, during a blitter
  operation, the blitter output was always zero."
- p.187: "A blitter zero flag is provided that can be tested to determine if
  the logic operation selected has resulted in zero bits for all destination
  bits, even if those destination bits are not written due to the D DMA
  channel being disabled." and "The Zero flag is only valid after the blitter
  has completed its operation and can be read from bit DMAF_BLTNZERO of the
  DMACONR register."
- The printed p.187 gives no bit number, although the text layer adds "(13)";
  bit 13 comes from p.278 and p.222.

**Stimulus.** Default configuration.

1. `DMACON = $07FF`, then `DMACON = $8240`.
2. Five blits follow. For each one:
   - write the registers;
   - `BLTSIZE = $0104` (height 4, width 4: 16 words);
   - wait for the end: one dummy read of DMACONR, then read until bit 14
     is 0 (p.186), and store the read that showed bit 14 = 0 into G;
   - read DMACONR once more and store the value as Z_k in R.

   All five use `BLTCON1 = 0`, `BLTDMOD = 0`, `BLTAMOD ($064) = 0` and
   `BLTAFWM = BLTALWM = $FFFF`.

| k | BLTCON0 | Pointer | Logic output | D written | Expected Z_k bit 13 |
|---|---|---|---|---|---|
| 1 | `$0100` (USED, LF `$00`) | BLTDPT = Q | all 0 | yes | 1 |
| 2 | `$01FF` (USED, LF `$FF`) | BLTDPT = Q + `$100` | all 1 | yes | 0 |
| 3 | `$0100` | BLTDPT = Q + `$200` | all 0 | yes | 1 |
| 4 | `$08F0` (USEA only, LF `$F0` "D = A", p.177) | BLTAPT ($050) = F | `$FFFF` | no | 0 |
| 5 | `$08F0` | BLTAPT = Zb | `$0000` | no | 1 |

**Observation.** The five stored values Z_k, and the port accesses of each
blit.

**Expected.** Bit 13 of Z_k as in the last column. Blits 1 to 3 show that the
flag describes the latest blit. Blits 4 and 5 grade the "even if those
destination bits are not written" of p.187.

**Preconditions.**
- G1, and G2 for `$8240`: every stored Z_k shows bits 9 and 6 set and no
  other bit among 10 and 8-0.
- Blits 1 and 3 made 16 writes in their ranges, all with data `$0000`.
- Blit 2 made 16 writes, all with data `$FFFF`.
- Blits 4 and 5 made 16 reads in F (respectively Zb) and no write outside the
  CPU areas.
- Each Z_k was stored after the last port access of its blit and after a read
  with bit 14 = 0 (p.187: "only valid after the blitter has completed").

**Contrasting cases.**
- Exchange BLTCON0 between blits 1 and 2, and exchange F and Zb between
  blits 4 and 5. The expected bits then change to 0, 1, 1, 1, 0. This run is
  graded in the normal run as P9-SWAP, with that table.
- For the "after completion" precondition: drop the wait before Z_1. Z_1's
  store may then fall before the blit's last write, and the precondition goes
  red.
- For the "no write outside the CPU areas" precondition: blit 4 also uses D
  (BLTCON0 = `$09F0`, BLTDPT = Q + `$300`). It writes outside the CPU areas
  and the precondition goes red.
- For G2: also set SPREN in the enabling write. The stored values show
  bit 5 and G2 goes red.
- For the other preconditions: section 2.6.

**Not graded.**
- Bit 13 before any blit (no stated value).
- The "only" of "read only" (see P8).

The name DMAF_BLTNZERO on p.187 reads like "not zero". The sense of the bit
is taken from p.222 ("Remains 1 if ... always zero"), which agrees with
p.187's "If the images do not overlap, the zero flag will stay true."

### P10: The Copper writes AUDxLCH and AUDxLCL whatever the danger bit, under OCS and ECS

**Claims:** none carried. P10 grades part of the two claims of p.310,
`C-interp.copcon` and `C-interp.copcon.ecs`, which name a whole range of
registers; they are declined in section 1.

**Proposition.** With COPCON's CDANG clear or set, under OCS and ECS, Copper
MOVEs to AUDxLCH and AUDxLCL (x = 0..3) set the location that channel x then
fetches from.

**What the pages say, and what they all agree on.**
- p.310: "This 1-bit register, the danger bit (CDANG), when set allows the
  Coprocessor to write to the Blitter hardware. In the standard chip set, if
  this is set, the Copper can access the address range from $DFF03E through
  $DFF07E. Now, in the ECS, if this bit is set, the Copper can access all of
  the Amiga chip registers. If this bit is clear, the Copper can access the
  address range from $DFF03E through $DFF07E, the same range as when the
  danger bit is set in the standard chip set."
- p.26: "The registers that the Copper can always affect are numbered $80
  through $FF inclusive."
- p.21: "Any register whose address is $20 or above."
- p.287 marks with "*" and "~" the addresses the Copper cannot write, or can
  write only with the danger bit. The audio rows `$0A0-$0DA` on p.290 carry
  neither mark.
- Every page agrees that `$0A0-$0D2` is writable by the Copper in all four
  combinations. None names a case in which it is not.

**Stimulus.** Four runs: OCS and ECS, each with `COPCON ($02E) = $0000` and
with `COPCON = $0002` (bit 1, CDANG, p.274).

1. `DMACON = $07FF`. `ADKCON = $00FF`. Write COPCON.
2. The CPU writes all four channels as in P1 step 3, with locations A_x.
3. List at L:
   - for x = 0..3: `$00A0+$10*x, hi(B_x)` and `$00A2+$10*x, lo(B_x)`
     (MOVE format of p.21);
   - then `$FFFF,$FFFE`.
4. `MOVE.L #L,$DFF080` and `MOVE.W D0,$DFF088` (p.30). Write marker `K_O`.
   `DMACON = $8280`. Read DMACONR into R.
5. Delay. `DMACON = $0080` turns Copper DMA off. Write marker `K_C`.
6. The four audio windows W_0..W_3, exactly as P1 step 4. Then `END`.

**Observation.**
- Port reads in [L, L + `$24`) inside [t(`K_O`), t(`K_C`)].
- Port reads at A_x and B_x inside each W_x.

**Expected.** In W_x:
- at least 2 reads at B_x;
- no read at A_x;
- no read at A_y or B_y for y ≠ x.

The location the channel fetches from is the one the Copper wrote, after the
CPU's (P1 sources, and p.147: "The location and length registers are read by
the DMA channel before audio output begins.").

**Preconditions.** Every one is graded on every run: the four combinations of
OCS or ECS and CDANG clear or set, each with the list L and with the list L'
of the contrasting case.
- G1, G3, G5. G5 holds for the Copper window (only COPEN among bits 8-0) and for
  each audio window (only channel x).
- G2 for the Copper enable (bits 9 and 7) and for each audio window (bits 9
  and x), from the five stored readbacks.
- G4: V ≥ 2 in [`K_O`, `K_C`] and in each W_x.
- The Copper ran the list: every word of [L, L + `$24`) was read at least
  once inside [`K_O`, `K_C`], the final WAIT at L + `$20` included. It comes
  after all eight MOVEs.

**Contrasting cases.**
- List L' has the same eight MOVEs addressed to COLOR00 (`$0180`) with the
  same data. Expected in W_x: at least 2 reads at A_x and none at B_x. This is
  graded in the normal run, in all four combinations, as P10-COLOR.
- For G1: a branch-to-self before `END`.
- For G2: leave DMAEN out of the Copper enable; and, separately, clear DMAEN
  with the Copper-off write and leave it out of channel 0's enabling write. The
  stored readbacks lack bit 9.
- For G3: one extra CPU write into B_0 inside W_0; and, separately, one extra
  CPU write into the last word of L inside [`K_O`, `K_C`].
- For G4: a delay count of 0, so V(W_x) falls short of 2.
- For G5: also set SPREN before the windows.
- For the "Copper ran the list" precondition: omit bit 7 from step 4's write.
  No list read happens, and the precondition goes red.

**The danger bit itself cannot be shown to have been set.** No register exists
on which every page agrees that CDANG changes the Copper's access:
- p.21 lets the Copper write every register from `$20` upward;
- p.26 and p.287 protect `$40-$7E`;
- p.310 (ECS, bit clear) opens `$03E-$07E`.

So the CDANG = 1 runs rest on the COPCON write alone, and the test does not
claim more.

**Quantifier.** p.310 says "all of the Amiga chip registers" and names
`$DFF03E` through `$DFF07E`. P10 grades the eight location registers only.
AUDxLEN, AUDxPER, AUDxVOL and AUDxDAT are not covered by this package, so the
two claims are declined (section 1).

### P11: With the danger bit set, the Copper writes the blitter registers at $054, $056 and $058

**Claims:** none carried. P11 grades part of the two claims of p.310,
`C-interp.copcon` and `C-interp.copcon.ecs`, which name a whole range of
registers; they are declined in section 1.

**Proposition.** With CDANG set, under OCS and ECS, Copper MOVEs to BLTDPTH
(`$054`), BLTDPTL (`$056`) and BLTSIZE (`$058`) take effect: a blit starts and
writes at the destination the Copper wrote.

**Sources.**
- p.310, quoted in P10.
- p.26: "When CDANG is 1, these registers can be written by the Copper."
- p.287: "~ Address not writable by the Copper unless the "copper danger bit",
  COPCON is set true."
- p.21: "Any register whose address is $20 or above."
- With the bit set, every page agrees that `$054-$058` is writable.

**Stimulus.** OCS and ECS.

1. `DMACON = $07FF`. `COPCON = $0002`.
2. The CPU sets up the blitter:
   - `BLTCON0 = $01FF`;
   - `BLTCON1 = 0`;
   - `BLTAFWM = BLTALWM = $FFFF`;
   - `BLTDMOD = 0`;
   - `MOVE.L #Q,$DFF054`, the CPU's own destination.
3. List at L:
   - `$0054, hi(Q2)`
   - `$0056, lo(Q2)`
   - `$0058, $0104`
   - `$FFFF, $FFFE`
4. `MOVE.L #L,$DFF080` and `MOVE.W D0,$DFF088`. Write marker `O`.
   `DMACON = $82C0` (SET, DMAEN, COPEN, BLTEN). Read DMACONR into R.
5. Delay. Write marker `C`. `DMACON = $07FF`. Write `END`.

The Copper restarts at every vertical blanking (p.30) and writes the pointer
again each time, so every frame's blit lands at Q2. A 16-word blit ends long
before the next frame, as p.187 asks ("When a blit is in progress, none of
the blitter registers should be written").

**Observation.** Port writes in [Q2, Q2 + `$20`) and in [Q, Q + `$20`) inside
[t(`O`), t(`C`)].

**Expected.** At least one write in the Q2 range and none in the Q range.

**Preconditions.** Every one is graded on every run: OCS and ECS, each with
the real list and with both lists of the contrasting cases.
- G1, G2 (bits 9, 7 and 6 set), G3, and G4 with V ≥ 3.
- G5: of DMACON bits 8-0 in the stored readback, only COPEN and BLTEN are set.
- The list's first 12 bytes were read at least once.

**Contrasting cases.**
- Replace the BLTSIZE MOVE by `$0180,$0000`. Expected: no write in either
  range. This is graded in the normal run as P11-NOBLTSIZE.
- Replace the two pointer MOVEs by MOVEs to `$0180`. Expected: writes in the
  Q range and none in the Q2 range. This is graded in the normal run as
  P11-NOPTR.
- For G1: a branch-to-self before `END`.
- For G2: leave DMAEN out of the enabling write.
- For G3: one extra CPU write into L, past its own content, inside the window.
- For G4: a delay count of 0.
- For G5: also set SPREN in the enabling write.
- For the list precondition: omit COPEN from the enabling write.

**Not specified, because the pages disagree.**
- *CDANG clear.* p.21 lets the Copper write `$054-$058` always. p.26 ("When
  CDANG is 0, these registers cannot be written by the Copper") and p.287
  forbid it. p.310 permits it under ECS. So no run in this package makes a
  Copper write into `$040-$07E` with the bit clear.
- *`$03E`.* p.310 opens it. p.26 ("Those it cannot affect at all are numbered
  $00 to $3E inclusive") and p.289 (`STRLONG & *03E`) close it. It is not
  used.
- *Registers below `$040` under ECS with the bit set* ("all of the Amiga chip
  registers") are not graded.

## 4. Page disagreements that shape this specification

1. **DMACON chip column, p.290.** The text layer gives `DMACON 096 W D`. The
   printed page gives `DMACON 096 W A D P`, which agrees with p.278. Claim
   `5-appB-p290.c3` rests on the text layer (section 1).
2. **ECS marks on the audio location rows, p.290.** Only AUD0LCH carries
   "A(E)" and "(high 3 bits, 5 if ECS)". AUD1LCH-AUD3LCH read "A" and "(high
   3 bits)". p.266 (AUDxLCH "A(E)", "5 bits if ECS") and p.298 and p.309
   (AUDxLCH "chg", "high 5 bits was 3 bits") cover every x. p.139 and p.145
   speak of "high three bits". This specification keeps every location below
   `$80000`, where all of these readings agree.
3. **Which registers the Copper may write.**
   - p.21: `$20` and up always; `$10-$20` with the danger bit; below `$10`
     never.
   - p.26: `$80-$FF` always; `$40-$7E` with the danger bit; `$00-$3E` never.
   - p.287 and p.289: "*" on `$000-$03E`, "~" on `$040-$07E`.
   - p.310: danger bit set (standard set) opens `$03E-$07E`; under ECS, set
     opens every register and clear opens `$03E-$07E`.

   The pages agree only on `$080` and up (audio included), and on
   `$040-$07E` with the bit set. P10 and P11 use only these.
4. **Bit numbers on the printed pages.** p.186, p.187 and p.198 name
   DMAF_BLTDONE, DMAF_BLTNZERO and DMAF_BLITHOG without bit numbers; the text
   layer adds them. The numbers used here come from p.278 and p.222.
5. **Writing a status bit.** p.278 bit 15 says "bits written with a 1 get set
   or cleared". Bits 14 and 13 are "(read only)". The outcome of writing 1
   there is not stated, so it is not graded.
6. **The first busy test.** p.186 says the first test may be wrong. p.187
   says Fat Agnus sets the bit at the BLTSIZE write. The pages do not map
   OCS/ECS onto these chips, so read 1 is not graded.
7. **Cycle parity.**
   - p.20: the Copper uses odd cycles, and audio, disk, refresh, sprites and
     "most low resolution display DMA" use even ones.
   - p.196: "The 68000 uses only the even-numbered memory access cycles".
   - Figure 6-10 (p.196): odd cycles assigned to other devices.

   With the "adjusted" note of Figure 6-9, this is why no check here anchors
   on a slot number or parity.
8. **Strobing COPJMP1.** p.30 strobes COPJMP1 with a read (`MOVE.W
   COPJMP1(a1),d0`). p.287 defines a strobe as "Writing the register causes
   the effect". p.263 says strobe registers respond to either. This
   specification strobes with a `MOVE.W` write.

## 5. Claim coverage

| Proposition | Claims |
|---|---|
| P1 | `5-appA-p266.c2`, `5-appA-p266.c8`, `A-audxlch.acc`, `A-audxlcl.acc`, `5-appB-p290.c20`, `5-appB-p290.c27`, `5-appB-p290.c55`, `5-appB-p290.c62`, `5-appB-p290.c90`, `5-appB-p290.c97`, `5-appB-p290.c125`, `5-appB-p290.c132`, `B-aud0lch.acc`, `B-aud0lcl.acc`, `B-aud1lch.acc`, `B-aud1lcl.acc`, `B-aud2lch.acc`, `B-aud2lcl.acc`, `B-aud3lch.acc`, `B-aud3lcl.acc`, `5-appC-p298.c4`, `C-map.lch.acc`, `5-appC-p309.c4`, `5-appA-p263.c1`, `A-pre.c1` |
| P2 | `5-appA-p278.c5`, `5-appA-p278.c6`, `5-appA-p278.c8`, `A-dmaconr.acc`, `A-dmaconr.addr`, `5-appB-p289.c1`, `5-appB-p289.c2`, `5-appB-p289.c4`, `5-appA-p263.c1`, `A-pre.c1` |
| P3 | `5-appA-p278.c17`, `A-dmacon.b08` |
| P4 | `5-appA-p278.c18`, `A-dmacon.b07` |
| P5 | `5-appA-p278.c19`, `A-dmacon.b06` |
| P6 | `5-appA-p278.c20`, `A-dmacon.b05` |
| P7 | `5-appA-p278.c15`, `A-dmacon.b10` |
| P8 | `5-appA-p278.c13`, `A-dmacon.b14`, `5-appA-p278.c10`, `5-appA-p278.c8`, `5-appB-p289.c4` |
| P9 | `5-appA-p278.c14`, `A-dmacon.b13`, `5-appA-p278.c10`, `5-appA-p278.c8`, `5-appB-p289.c4` |
| P10 | none (section 1) |
| P11 | none (section 1) |
| none (section 1) | `5-appA-p264.c2`, `5-appA-p263.c7`, `5-appA-p263.c8`, `A-pre.c8`, `5-appA-p263.c9`, `A-pre.c9`, `5-appB-p287.c5`, `5-appB-p290.c3`, `C-interp.copcon`, `C-interp.copcon.ecs` |

## 6. What the build adds

Nothing below changes a proposition or an expected value. `ch5.i` defines
the registers of section 2.2 not already in `nanomig.i`, the memory map of
section 2.3, and the mechanism items below.

- **Clearing the ROM overlay.** G, R and MK (section 2.3) sit at $78000,
  $7C000 and $7D000; every DMA block sits at $60000-$75FFF. All of it is
  below $80000, inside the ROM overlay window: "An additional copy of the
  system ROM responds starting at memory location $00000000. The system
  RAM that would normally be located at this address is not available. ...
  The OVL bit in one of the 8520 Chips disables the overlay" (p.223).
  Every program's first steps write $03 to CIA-A ddra ($BFE201) and then
  $00 to CIA-A pra ($BFE001), clearing OVL (bit 0) before any other write
  (p.223; Appendix F, p.339, gives the two addresses and "Direction for
  port A (BFE001);1=output (set to 0x03)"). Measured: every marker write,
  every DMACONR store into R, and the write of G3's mutation for P1 (a
  `MOVE.W` to the absolute address `$60000`), land at the RAM port at
  exactly the byte address the program names, on both cores.
- **Fixture.** Before any proposition, a program stores a known constant at
  RESULTS and, after the overlay is cleared, at R; the test reads the first
  by peeking memory and the second from the RAM-port record, which is how
  every proposition reads its results.
- **DMA-block content.** Every DMA block a proposition reads from (audio
  data, sprite structures, the Copper list, blit source patterns) is
  poked into chip RAM directly before `boot()`, the same mechanism
  `load_program()` itself uses; no program instruction writes it. Content
  is immaterial where section 2.3 says so ("any values"; the sprite
  and Copper-list content follow the pages P1/P6/P10/P11 cite).
- **R's stored values are read from the RAM port's own log** of the CPU's
  store instructions (the write's data field), not peeked from memory
  content: a value the program never actually stored is then absent, not
  stale. This is stricter than a memory peek and needed regardless, since
  R sits below $80000, outside the identity-mapped range the harness's
  `peek_logical()` resolves.
- **P5 step 1 ("The CPU clears [Q, Q + `$200`)")** is a CPU loop (256
  words), now that the overlay is cleared, rather than relying on a fresh
  instance's own zero-initialised memory.
- **Tuned constants.** The per-channel delay/gap counts (P1, P10) and the
  per-proposition delay counts (P3-P6, P11) are spin-loop counts with no
  value the page states; each is guarded by its own G4 (or, for P10/P11,
  the vertical-sync count the proposition itself names), which is what is
  graded, not the count. P10's run budget (40,000,000 cycles) is sized
  from measurement, not tuned to a pass: fx68k needs measurably more
  cycles than TG68K for the same program (P1's own run, common to both,
  finishes at cycle 22,616,005 on fx68k against 15,776,325 on TG68K), and
  P10 repeats that same run inside a larger program.
- **G3 (no CPU write falls into any DMA block during any window)** is
  mutated for every proposition that grades it (one extra CPU write into
  that proposition's own block under test, inside its window): A_0 for P1
  (an absolute `MOVE.W`), P for P3, L for P4, Q for P5 (outside the blit's
  destination footprint and, separately, onto its first word), S0 for P6,
  B_0 and, separately, L for P10, L for P11. Every other precondition of
  every proposition has a mutation of its own as well.
- **Markers.** P1 and P10 write `END`; P7, P9 and P11 write `END` after their
  last step. P3-P6 and P8 close with the window's closing marker and then
  `DONE`: their G1 grades that `DONE` was written and that the window's
  markers were found.
