# Test package aa-s3: bit-plane and sprite pointers and data, sprite position and control, UHRES sprite window, collision control

This document specifies an integration test package that checks an FPGA re-implementation of the Amiga chip set against Commodore's *Functional Specification for the Advanced Amiga Chip Set (AA)*. It was written from the package's claims (`claims-spec-stage.jsonl`, 249 records from the specification's pages 15, 18 and 28) and from the specification's pages, with the Hardware Reference Manual (3rd edition) consulted only where the AA specification builds on the original chip set, by an author who never saw the implementation or any test. The oracle is the AA specification: every expected value below comes from an AA page and is quoted with it. The builder implements the package as specified here and may not change what it specifies.

Conventions of this document:

- "p.N" is a page of the AA specification. The document prints no page numbers; N is the PDF page, which is also the corpus page (`corpus\pNN.txt`). "HRM p.N" is a printed page of the Hardware Reference Manual.
- Quotations join the page's line breaks with one space and keep its spelling. Where the text layer was checked against the scan (`bands\pNN_bandK.png`), this is said.
- Register names are followed by the offset the AA address list prints (pp.6-10). The 68000 reaches offset `r` at `0xDFF000 + r` (HRM p.263: "680x0 address = (chip address) + $DFF000").
- Numbers written `0x...` are hexadecimal. "Line n" is the vertical beam position n. A "lores pixel" is 140 ns wide, an "output pixel" is 70 ns (see 2.11).

---

## 1. Claims that give no proposition

Each claim below is declined: no assertion carries its id. Where a proposition checks part of what a claim states, the row names that part; the id is still declined whole, because no assertion grades all of it. The reason is given, and the executable prints it on a `RECORDED:` line for each id; a test must not grade what these claims state.

| Claims | Reason |
|---|---|
| opus:AA-sprhstop.c6; sol:AA-sprhstop.c7, c8, c9, c10 (SPRHSTOP bits 14-11 printed `x`); sol:AA-sprhstrt.c6, c7, c8, c9, c10 (SPRHSTRT bits 15-11 printed `x`) | p.28 prints `x` and does not say what it means. Where other pages define `x` it is a don't-care bit to be written 0: p.16 "x= don't care; but drive to 0 for upward compatibility !", p.22 "Don't care (x) bits should always be written to 0 to maintain upwards compatibility", p.14 "x s should be written to 0 for upward compatibility". A don't-care bit has no expected effect. The stimulus writes these bits as 0 and no test reads or grades them. |
| opus:AA-clxcon2.c8, c11; sol:AA-clxcon2.c8, c14 (CLXCON2 bits 15-08 and 05-02 "unused") | An unused bit gives no expected value. The stimulus writes them as 0. |
| sol:AA-clxcon2.c9, c15 (the unused bits are "initialized by reset") | A bit with no function in a write-only register cannot reach any output, so its reset value cannot be seen. |
| sol:AA-clxcon2.c17 (MVBP8 initialized by reset) | MVBP8 matters only while ENBP8 is 1, and ENBP8 can only become 1 through a write to CLXCON2, which rewrites MVBP8 in the same write. The reset value of MVBP8 can never reach an output. (The reset value of ENBP7 and ENBP8 can be seen; see C5.) |
| opus:AA-clxcon2.c13; sol:AA-clxcon2.c18 (the line `**k**XRESET BY ECSENA*****` after CLXCON2's table) | The printed page does not carry this line. The scan (`bands\p18_band3.png`) ends with the row "01 MVBP8 Match value for Bit Plane 8 collision" and the frame of the box; nothing follows. The claim itself records that the line comes from one OCR reading only. Even read as "RESET BY ECSENA" it does not say which bits it governs, and the pages that list what ECSENA controls (p.4, p.17) name only five BPLCON3 bits: "BRDRBLNK,BRDNTRAN,ZDCLKEN,BRDSPRT, and EXTBLKEN" (p.17). No expected value. |
| opus:AA-sprxdatb.c12; sol:AA-sprxdata.c19 (the sentence "In lowres mode," continues on p.29) | These records state only that a sentence breaks across a page. Its continuation (p.29, "1 sprite pixel is 1 bitplane pixel wide. In HRES and SHRES mode, 1 sprite pixel is 2 bitplane pixels.") is not a claim of this package. |
| sol:AA-bplxpth.c11 (misprint `x=1,2,3,45,6,7,8`) | A record of a misprint (confirmed on the scan, `bands\p15_band2.png`). It changes nothing a test can see: the entry prints eight addresses and BPLxPTL prints "x=1,2,3,4,5,6,7,8". The addresses are used in B1. |
| opus:AA-bplxptl.c7; sol:AA-bplxptl.c12 (BPLxPTL "must be reinitalized by the processor or coprocessor to point to the beginning of Bit Plane data every vertical blank time") | p.15 states an obligation for every plane and every vertical blank. B2 performs two writes of plane 1's pointer (one in each of two vertical blanks) and grades their address effect. That establishes neither the universal programming obligation nor a consequence of omitting a vertical-blank re-initialisation, and it covers plane 1 only. B2 remains as a check of the pointer effect and carries no claim id. |
| opus:AA-sprxptl.c7 (the sprite pointer registers "must be initialized by the processor or coprocessor every vertical blank time") | p.28 states an obligation for every sprite and every vertical blank. S2 performs writes in two vertical blanks and grades the address effect for sprite 0. That establishes neither the universal obligation nor a consequence of omitting an initialisation. S2 remains as a check of the pointer effect and carries no claim id. |
| opus:AA-sprxdatb.c6; sol:AA-sprxdata.c13 (the image data registers "are usually loaded by the Sprite DMA channel") | One DMA load of sprite 0 shows that the load is possible and that data flow, not "usually": the page defines no population or threshold for it. S13 remains as a mechanism check and carries no claim id. |
| opus:AA-sprxctl.c19; sol:AA-sprxctl.c33; opus:AA-sprxdatb.c7; sol:AA-sprxdata.c14 (either processor may load the registers "any time", "at any time") | S14 samples a finite set of line-start, mid-line and vertical-blank writes by both processors; it does not establish every instant. The latency "takes effect from the next line on" is not stated by the page. S14 grades the value in effect on the last line before the next event, and the sampled behaviour from the next line on is MEASURED. S14 and S14-B carry no claim id. |
| opus:AA-sprxctl.c18; sol:AA-sprxctl.c32 (POS and CTL "are usually loaded by the Sprite DMA channel, during horizontal blank") | S13 checks the loading by the Sprite DMA channel only. "Usually ... during horizontal blank" grades nothing ("usually" admits exceptions), and the chip's horizontal blanking interval is not visible: the video output passes a line doubler, and the pages give no fixed horizontal-blank position for the default beam. |
| opus:AA-sprhstop.c9 (SPRHWRM "(ECS and later chips only)") | S16 checks the positive part only: SPRHWRM acts with ECS and with AA behaviour. The "only" cannot be checked: with OCS behaviour there are no UHRES pointers at all (p.16, UHRES "(hires chips only)"), so there is no UHRES cycle whose direction could be seen. |
| opus:AA-sprhstop.c8 (SPRHWRM swaps ARW* "so that external devices can detect the RGA and put things into memory") | S16 checks the swapped ARW* polarity only; sol:AA-sprhstop.c22 states that part alone and is graded by S16. The register address bus (RGA) and the SPRHDAT identifier on it are not visible to a test. |
| opus:AA-clxcon2.c7; sol:AA-clxcon2.c7 (CLXCON2's bits are "INITIALIZED BY RESET") | C5 checks the reset value of ENBP8 and ENBP7 only (sol:AA-clxcon2.c11, c13). The reset values of the unused bits and of MVBP7 and MVBP8 cannot reach any output (rows above). |
| opus:AA-clxcon.c6; sol:AA-clxcon.c6; opus:AA-clxcon2.c6; sol:AA-clxcon2.c6 (a write to CLXCON resets the bits of CLXCON2) | C4 checks that a CLXCON write clears ENBP7 and ENBP8 only. That it clears MVBP7, MVBP8 and the unused bits cannot be seen: a match value acts only while its enable is 1, the enable can only become 1 again through a CLXCON2 write, and that write rewrites the match value; the unused bits have no function. C4 remains a check and carries no claim id. |

No claim in this package needs data read from or written to a disk surface.

---

## 2. Common setup

### 2.1 System and runs

- AA chip set selected (harness `Config.chipset_config = 0x10`), 2 MB Chip memory (harness `Config.memory_config = 0x03`), 68000 CPU behaviour (`Config.cpu_config = 0x00`). The 68010/68020 settings add nothing here: none of these claims concerns the CPU type. S16 also runs with ECS behaviour selected (stated there).
- **Graded cores.** Every proposition, precondition and anchor of this package is graded (PASS or FAIL, or NOT GRADED where a precondition prints FAIL) on both cores, fx68k and TG68K. Both builds run the same programs; the chip set is selected by `chipset_config`, an input of `nanomig.v`, and not by the build, so every run of both builds selects AA (except S16's ECS runs).
- **PAL.** The harness has no PAL setting (its `Config` fields are memory, fast RAM, CPU, chip set, floppy, video, turbo and IDE), so the program selects PAL itself: the first write of its setup is BEAMCON0 (1DC) = `0x0020`. p.12: "5 PAL", "PAL= Set appropriate decodes (in normal mode) for PAL." The long frame LF writes its own BEAMCON0 value in place of this one, with PAL also set (2.9). The PAL field is needed: D-STD's window, S8 and S15 (k = 8) use lines above 0x105, the last line of an NTSC field (HRM p.79 table: vertical blank stop "$15 (21)" NTSC, "$1D (29)" PAL, then 241 and 283 displayable lines), and 2.11 uses the PAL line length.
- Every run starts from system reset, and the test program is the only software. Each run lasts until the last frame the proposition observes has ended; the program then executes `DONE` (`nanomig.i`), which writes `DONE_VALUE` to `DONE_ADDR` and stops in place.
- The program writes custom registers with word writes. A pointer pair is written high word (PTH) first, then the low word (PTL).
- Every program is assembled for the 68000 (vasm's default CPU, no `-m68010` or higher). With `-m68010` or higher vasm assembles `MOVE #0,<ea>` as `CLR`, and on the 68000 CLR reads its operand before writing it. B2, B4, S2, S3, S7 and C2-C5 write 0 to write-only registers (BPL1PTL, SPR0PTL, BPLxDAT, SPRxCTL, CLXCON) and specify exactly one write each time.
- **Setup marker.** When setup is complete, the program writes `0x5E7F` to the setup-marker word (2.3). This write is seen at the Chip-memory port.

### 2.2 Pointer value

A pointer pair addresses a byte address `A(PTH, PTL) = ((PTH & 0x001F) << 16) | (PTL & 0xFFFE)`. The Chip-memory port reports the word address `A / 2` for every address in the identity-mapped Chip memory, where all regions of 2.3 lie. This follows from:

- p.6: "+=Address register pair.Low word uses DB1-DB15,High word DB0-DB4."
- p.15 and p.28: "(High 5 bits)", "(Low 15 bits)".
- p.6: "PTL,PTH=20 bit Pointer that addresses DMA data."

The program always writes PTH bits 15-5 and PTL bit 0 as 0. The pages give these bits no function, and no test depends on them.

### 2.3 Memory map (byte addresses)

Every region lies in the Chip memory the harness maps one-to-one, `0x080000-0x1FFFFF` (`nanomig_harness.h`: `kChipIdentityLo`, `kChipIdentityHi`). Only there is the word address at the Chip-memory port the logical address / 2; `peek_logical()` states: "Outside that range the physical and logical addresses differ". Below `0x080000` the ROM overlay is active after reset (HRM p.223: "The OVL bit in one of the 8520 Chips disables the overlay"), and the harness states: "A test program must not write inside the overlay window." The program does not clear the overlay and uses no address below `0x080000`. The harness's memory model starts as zeros (`NanomigHarness` constructor, `mem_(kRamWordCount, 0)`), so Z, the T regions and every L_x the program does not write hold zeros.

| Region | Address | Use |
|---|---|---|
| Program | from `0x080400` (`PROGRAM_ORG`, `nanomig.i`), ending below `0x090000` | test program; the harness loads it there and places the reset vectors itself |
| Stack | down from `0x0FFF00` (`PROGRAM_STACK`) | no other region lies in `0x0F0000-0x0FFFFF` |
| P_k, k = 1..8 | `0x090000 + 0x4000*(k-1)` | bit-plane data for plane k |
| L_x, x = 0..7 | `0x0B0000 + 0x1000*x` | sprite x's DMA list; alternative lists at `0x0B8000 + 0x1000*x` |
| RES | `0x0C0000` (`RESULTS`, `nanomig.i`) | results: setup marker at `0x0C0000`; stored register reads and beam re-reads from `0x0C0010` on, below `DONE_ADDR` (`0x0C0FFE`) |
| U | `0x0D0000`; region `[0x0D0000, 0x0E0000)` | UHRES sprite pointer target |
| U2 | `0x0E0000`; region `[0x0E0000, 0x0F0000)` | UHRES bit-plane pointer target |
| CL | `0x100000` (16 KB) | Copper list |
| Z | `0x104000` (16 KB) | zeros; bit-plane data for planes that are not under test |
| T_i, i = 1..8 | see below | wide-address regions, used only by B1 and S1 |

The table regions put each of the five PTH bits and each of PTL bits 15-1 at 1 in some region and at 0 in another. Inside the identity-mapped range PTH is at least 0x08; PTH bit 4 is 0 in T_5, and PTH bit 3 is 0 in T_1, T_2, T_3 and T_6.

| i | h_i (PTH) | l_i (PTL) | A_i |
|---|---|---|---|
| 1 | 0x11 | 0x0000 | 0x110000 |
| 2 | 0x12 | 0x5554 | 0x125554 |
| 3 | 0x14 | 0x2AAA | 0x142AAA |
| 4 | 0x18 | 0x8000 | 0x188000 |
| 5 | 0x0C | 0xC002 | 0x0CC002 |
| 6 | 0x13 | 0x4444 | 0x134444 |
| 7 | 0x1C | 0x1110 | 0x1C1110 |
| 8 | 0x1F | 0xA000 | 0x1FA000 |

Region T_i spans `[A_i, A_i + 0x3000)`. B1 and S1 use no other data region except CL, RES and Z. No two regions of this section overlap. One plane's data for one frame of D-STD (a 320-pixel window, 0x81-0x1C1, of 16-pixel words on the 256 lines 0x2C-0x12B: `0x2800` bytes) fits in every T, P and Z region.

### 2.4 Standard display D-STD

The program writes the following during setup. Values are from the pages cited.

| Register | Value | Basis |
|---|---|---|
| DIWSTRT (08E) | `0x2C81` | p.22: bits "V7 ... V0 H9 ... H2"; start at line 0x2C, horizontal 0x81 |
| DIWSTOP (090) | `0x2CC1` | p.22: "DIWSTOP is vertically restricted to the lower 1/2 of the display (V8=/=V7)" gives line 0x12C; "(H8=1)" gives horizontal 0x1C1 |
| DDFSTRT (092) | `0x0038` | p.21 table, "normal" H8-H4 = 0 0 1 1 1 |
| DDFSTOP (094) | `0x00D0` | p.21 table, "normal" H8-H4 = 1 1 0 1 0 |
| BPLCON0 (100) | BPU code: `n << 12` for n = 0..7, `0x0010` for n = 8 | p.16: "BPUx =Bit plane use code 0000-1000 (NONE thru 8 inclusive)"; BPU3 is bit 4 |
| BPLCON1 (102) | `0x0000` | no scroll |
| BPLCON2 (104) | `0x0000` | keeps RDRAM at 0; p.17: "RDRAM= causes color table addresses to read the color table instead of writing to it." |
| BPLCON3 (106) | `0x0C00` | the values p.16 marks as initialised (PF2OF = 011, everything else 0); sprite resolution is then the "ECS defaults" (p.17), i.e. 140 ns sprites on a lores display |
| BPLCON4 (10C) | `0x0011` | initialised values: BPLAM = 0, ESPRM = 0001, OSPRM = 0001 (p.16; p.18 "Default value is 0001 binary") |
| FMODE (1FC) | `0x0000` | reset value (p.4: "RST_ pin resets all bits in all registers new to AA") |
| BPL1MOD, BPL2MOD (108, 10A) | `0x0000` | |
| SPR0CTL ... SPR7CTL | written once, value 0 | disarms all sprites (p.28: "Writing to SPRxCTL disables the corresponding sprite."). Not written where a proposition forbids it (S7). |

DMACON (096) is written per test with the SET bit (bit 15) and the enables of p.22: bit 9 DMAEN, 8 BPLEN, 7 COPEN, 5 SPREN.

The bit-plane pointers of the planes in use are rewritten at the top of the Copper list in every frame (p.15 requires it; see B2). They point to Z unless a proposition says otherwise.

### 2.5 Colours

Colour registers COLOR00-COLOR31 are at `0x180 + 2*i` (p.9). With LOCT = 0 a write sets the 4 most significant bits of each gun: p.19 table, row "LOCT=0", bits 11-8 R7-R4, 7-4 G7-G4, 3-0 B7-B4. p.19 also says "the 4 bit values are automatically extended to 8 bits". The video output shows 4 bits per gun, so it shows the written value.

Registers 32-255 are reached through BANK2-0 (BPLCON3 bits 15-13; p.5 table "BANK2 | BANK1 | BANK0 || Color Address Range"). The program restores BANK to 000 after loading.

Colour values used:

- **cON** = `0xFFF`.
- **cS** = `0x0F0`.
- **cE** = `0x0F0` and **cO** = `0xF0F` (used only by S10).
- **tag(n)** = `0x100 | n` for n = 0x2C..0xFF (red = 1). This is distinct from every other colour used.

Palettes:

- **PAL-ON**: registers 1-255 = cON (8 banks). Used for bit-plane tests.
- **PAL-SPR**: registers 1-15 = cON, 16-31 = cS. With ESPRM = OSPRM = 0001, every sprite colour comes from registers 16-31. p.18: "ESPRMx= 4 bit field provides the 4 high order color table address bits for even sprites"; OSPRMx the same "for odd sprites". So every non-transparent sprite pixel shows cS, whatever the sprite's number.
- **PAL-ATT**: registers 16-31 = cE, 32-47 = cO, with BPLCON4 = `0x0012` (ESPRM = 0001, OSPRM = 0010). Used only by S10.

COLOR00 carries the line tag (2.6).

### 2.6 Copper list CL-STD

Setup follows p.21: "It is important that one of the jump registers be initalized and it's jump strobe address hit, after power up but before Coprocessor DMA is initalized." The program writes COP1LCH/COP1LCL (080/082) = `0x100000` (CL), writes the strobe COPJMP1 (088), and only then sets COPEN and DMAEN in DMACON.

The encodings are from p.20:

- MOVE: first word = register offset, bit 0 = 0; second word = the data.
- WAIT: first word = VP7-VP0 in bits 15-8, HP8-HP2 in bits 7-1, bit 0 = 1; second word `0xFFFE` (BFD = 1, all compare-enable bits set).

The list consists of:

1. **Vertical-blank block.** MOVE COLOR00 = `0x000`; MOVEs of the pointers the test uses (bit-plane, sprite, UHRES); any other per-test MOVE said to be "in the VB block". Whenever SPREN is set, all eight sprite pointers are written in every frame: a sprite pointer that the test does not load otherwise gets the address of that sprite's list L_x, which holds only the end pair (2.7). p.28: "These address registers must be initialized by the processor or coprocessor every vertical blank time."
2. **Tag block.** For n = 0x2C .. 0xFF in order: WAIT(VP = n, HP = 0), then MOVE COLOR00 = tag(n). Per-test Copper events at (line e, H = h) are placed in line order as WAIT(e, h) followed by their MOVEs.
3. **End.** WAIT with first word `0xFFFF` and second word `0xFFFE`. It is never satisfied before the next restart.

The Copper fetches both words of each instruction from Chip memory. p.20: "RD =RAM Data moved by MOVE instruction at IR2 time directly from RAM". So every MOVE's data word is seen at the port with its time and value.

### 2.7 Sprite DMA lists

The list format is that of the original chip set (HRM p.105, Table 4-1; HRM p.126: "The first word is stored in SPRxDATA and the second word in SPRxDATB"):

- word 0: POS
- word 1: CTL
- then h pairs (DATA word, DATB word), one pair per line
- then the end pair `0x0000, 0x0000` (HRM p.127: "To stop the sprite after that first data set, the pair of words should contain all zeros")

The height is `h = stop - start` (HRM p.107: "The value (VSTOP - VSTART) defines how many scan lines high the sprite will be").

A sprite that is not used has a list consisting only of the end pair. POS and CTL encode (p.28):

- POS bits 15-8 = start line bits 7-0; POS bits 7-0 = SH10-SH3.
- CTL bits 15-8 = stop line bits 7-0; CTL bit 7 = ATT, bit 6 = SV9, bit 5 = EV9, bit 4 = SH1, bit 3 = SH0, bit 2 = SV8, bit 1 = EV8, bit 0 = SH2.

The horizontal start value SH10-SH0 is counted in 35-ns units. A lores x-coordinate X corresponds to SH = 4*X.

Every sprite list keeps start and stop of the same parity, as p.4 requires: "NOTE: Sprite vertical start and stop positions must be of the same parity, i.e. both odd or both even."

Standard columns: sprite x at lores X_x = `0x90 + 0x18*x` (x = 0..7), which lies inside the window 0x81-0x1C1.

### 2.8 Manual sprites

A manual sprite is driven by the CPU or the Copper with SPREN clear. It is loaded in this order:

1. SPRxPOS
2. SPRxCTL
3. SPRxDATB
4. SPRxDATA, the write that arms it (p.28: "Note: Writing to the A buffer enables (arms) the sprite.")

The order DATB-then-DATA follows HRM p.123.

**Manual band.** The CPU loads the sprite at line a (H = 0x08) and writes SPRxCTL at line a + 0x10 (H = 0x08) to disarm it. The sprite is then shown only on the lines between.

### 2.9 Long frame LF

For bit 9 (and, for S15, bit 10) of a vertical value, a field longer than 512 (1024) lines is needed. The CPU writes during setup:

- HTOTAL (1C0) = `0x00E2` (p.24: "Horiz line has this many + 1 280ns increments")
- VTOTAL (1C8) = `0x05FF` (p.29: "It's the line number to reset the counter, so there's this many + 1 in a field")
- BEAMCON0 (1DC) = `0x08A0`, in place of the `0x0020` of 2.1 (p.12: bit 11 LOLDIS, bit 7 VARBEAMEN, bit 5 PAL; p.24: "If the PAL bit & LOLDIS are not high, long line/short line toggle will occur")

p.29 gives the field as VTOTAL + 1 = 1536 lines with LACE clear. The number of lines is measured and printed against that sentence (2.12, PC-FRAME); the propositions that use LF ask only for the length their lists need. The Copper tag block still covers lines 0x2C-0xFF only.

### 2.10 Collision frame procedure CF

In each observed frame the CPU does the following:

1. At line 0x40 (H = 0x08), it reads CLXDAT (00E), which clears it. p.19: "This address reads (and clears) the collision detection register." It stores the value (discarded) and a marker.
2. At line 0x90 (H = 0x08), it reads CLXDAT again and stores the value. This stored value is the result.

Each step is a beam marker (2.11). Sprites under test lie on lines 0x60-0x6F. The CLXDAT bit meanings are those of p.19:

- 14: "Sprite 4 (or 5) to Sprite 6 (or 7)"
- 11: "Sprite 0 (or 1) to Sprite 6 (or 7)"
- 10: "Sprite 0 (or 1) to Sprite 4 (or 5)"
- 09: "Sprite 0 (or 1) to Sprite 2 (or 3)"
- 05: "Playfield 2 to Sprite 0 (or 1)"
- 01: "Playfield 1 to Sprite 0 (or 1)"

p.19 also says: "NOTE: Playfield 1 is all odd numbered enabled bit planes. Playfield 2 is all even numbered enabled bit planes".

### 2.11 Reference frames and measures

- **Frame anchor.** t_F(i) is the time of the Copper's i-th instruction fetch at the COP1LC address after the setup marker. p.21: "In addition COP1LC is automatically used at the beginning of each vertical blank time." Frame F_i is `[t_F(i), t_F(i+1))`. Unless stated otherwise the observed frame is F2.
- **Line anchor.** t_tag_i(n) is the time of the fetch of the first word of the MOVE COLOR00 that follows WAIT(n) in frame i. Chip line n of frame i is `[t_tag_i(n), t_tag_i(n+1))`. An access in that interval is "on line n". This attribution is used only for accesses well inside a line (bit-plane fetches), never for accesses at the very start of a line.
- **Line period.** P = t_tag(n+1) - t_tag(n), in system clock cycles.
- **Output lines.** The output of chip line n in frame i is the first pair of consecutive output lines that shows background tag(n) after t_tag_i(n). The line doubler shows each chip line twice; the first of the two is graded. Pixel columns are counted from horizontal sync.
- **Output pixel.** The unit is measured at the ports, not assumed. A PAL line is 227 colour clocks (HRM p.23: "In PAL, they are all 227 long"), and a colour clock is 280 ns (p.21: "memory cycles are 280 ns"; p.29, VHPOSR: "(280 NS)"). So a chip line is 908 intervals of 70 ns, and it lasts P clk_sys cycles. The video port is sampled once per clk_sys cycle (`nanomig_harness.h`, `VideoSample`), and the scan doubler shows each chip line twice within those P cycles (`find_lines()`: "hs toggles at roughly twice the rate of the chipset's own horizontal sync"). So 70 ns of chip time occupies u = P / 1816 video samples of the output. One **output pixel** is u samples: a lores pixel (140 ns) covers exactly two output pixels, and a shift by 70 ns moves an edge by exactly one output pixel. PC-PIXEL (2.12) measures u. A 35-ns pixel is half an output pixel; whether and how it reaches the output is measured where it matters (S6).
- **Beam marker.** The CPU polls VPOSR (004; bits 2-0 = V10, V9, V8, p.29) and VHPOSR (006; bits 15-8 = V7-V0, bits 7-0 = H8-H1, p.29) until the target line is reached and H is at least the target. It then performs the action, re-reads VPOSR/VHPOSR, and writes the two readings and a marker word to RES. These writes are seen at the port with their times.
- **Line-offset measure LOM.** For an event seen in two runs (or two frames), with t measured from the t_F of its own frame: `Δ = round((t2 - t1) / P)`. The result is valid only if `|(t2 - t1) - Δ*P| ≤ P/8`; otherwise the comparison is invalid, not red. LOM compares positions without needing to know where in a line a DMA slot lies, which the pages do not state.

### 2.12 Common preconditions (each with the change that makes it fail)

| Id | Precondition asserted before grading | Stimulus change that makes it fail |
|---|---|---|
| PC-FRAME | t_F(i) exists for every frame used. Consecutive t_F are equally spaced (within P/8). In the LF configuration the field also lasts at least as many lines as the longest list of the proposition needs: 0x249 lines for S8 and S9 (the stop line 0x248 of the k = 9 list lies inside the field), 0x449 lines for S15 (k = 10). The exact length of the field is graded by the proposition LF; it is not a precondition of any other proposition. | Omit setting COPEN: no fetch at COP1LC. In LF, omit VARBEAMEN: the field is a normal PAL field, shorter than the least length. |
| PC-TAG | In every graded frame, t_tag(n) exists for n = 0x2C..0xFF with equal spacing P (within P/32). The output shows tag(n) for each n in increasing order. | Remove the MOVE COLOR00 instructions from the tag block: no tags. |
| PC-BPL(set) | In the graded frame, the port shows reads in the region of every plane in the set on every graded line, and no reads in the regions of planes outside it. | Clear BPLEN: no reads. |
| PC-SPR(x) | In the graded frame, list x is read in order: word 0, word 1 once, then its data pairs, then the end pair. | Clear SPREN: no reads. |
| PC-MOVE | Each Copper MOVE that writes a pointer or register under test is fetched in the intended frame and line with the intended data word (seen at the port). | Put a wrong data word into the list: the fetched value differs. |
| PC-BEAM | Every beam marker's re-read shows the target line. Not used by C2, C3 and C5, which rest on their window precondition; there the re-read line is a MEASURED line. | Target H = 0xE0: the action spills into the next line and the re-read shows line + 1. |
| PC-STATIC(reg) | The program and the Copper list contain no write to reg before the check (checked on the program image). | Add such a write: the static check flags it. |
| PC-SETUP | The setup marker `0x5E7F` is seen as a write to `0x0C0000` at the port. t_F(1) follows it by the definition of t_F (2.11). | Leave out the marker write: no marker is seen, and no frame can be anchored. |
| PC-PIXEL | In the graded frame, t_tag(0xFF) - t_tag(0x2C) = 211 * 1816 * u clk_sys cycles for a whole number u ≥ 1 (2.11, "Output pixel"). Every count of output pixels uses this u. It is a precondition of every proposition that reads a pixel pattern or counts output pixels: B3, B4, S3-S7, S10-S14, C1. | With BEAMCON0 = `0x08A0` and HTOTAL (1C0) = `0x00E3` every line is 228 colour clocks (p.24: "Horiz line has this many + 1 280ns increments"), and the tag spacing is no whole multiple of 211 * 1816. |

---

## 3. Propositions

There are 27 propositions: B1-B5 (bit planes), S1-S16 (sprites), C1-C5 (collision control) and LF (field length). For each one, "Claims" lists every claim it rests on. Five joint assertions J1-J5 follow them: each carries claims that no single proposition grades whole, and passes only if every proposition grading a part of them passes.

### B1. Bit-plane pointers: addresses, and the address they hold

**Claims (p.15):** opus:AA-bplxpth.c1, c2, c4, c6, c9; sol:AA-bplxpth.c1-c9, c22; opus:AA-bplxptl.c1, c2, c4, c6, c10; sol:AA-bplxptl.c1-c9, c11, c23.

**Proposition.** For each x = 1..8:

- BPLxPTH (write-only, at `0x0E0 + 4*(x-1)`) holds the high 5 bits of the address from which bit plane x's DMA data are fetched.
- BPLxPTL (write-only, at `0x0E2 + 4*(x-1)`) holds the low 15 bits of that address.

**Stimulus.**

- D-STD with BPU = 8 (BPLCON0 = `0x0010`); DMACON = `0x8380` (SET, DMAEN, BPLEN, COPEN). The video is not graded.
- The Copper VB block writes BPLxPTH = h_x and BPLxPTL = l_x for x = 1..8 (region T_x, 2.3) in every frame. The regions hold zeros.
- Second series: BPU = n for n = 0..8 in nine runs, with the same pointers.
- Addresses are those of p.15, confirmed row by row by p.8: BPL1PTH 0E0, BPL1PTL 0E2, ..., BPL8PTH 0FC, BPL8PTL 0FE, all "W".

**Observation.** Port accesses in F2. For each x, the first access in F2 to region T_x.

**Expected.**

(a) With BPU = 8, for every x = 1..8, the first T_x access in F2 is a read at word address `A_x / 2`. Page: "BPLxPTH 0E0 W A Bit plane x pointer (High 5 bits)", "BPLxPTL 0E2 W A Bit plane x pointer (Low 15 bits) ... Address of Bit plane x (x=1,2,3,4,5,6,7,8) DMA data."

(b) With BPU = n, T_x is read in F2 if and only if x ≤ n. The number of planes, n, is from p.16: "BPUx =Bit plane use code 0000-1000 (NONE thru 8 inclusive)". That the n planes are planes 1..n is the original chip set's definition (HRM p.48, Table 3-5: "001 1 PLANE 1", "010 2 PLANES 1 and 2", ...), which p.16 extends to 8 without redefining. Part (b) is the only part that ties a pointer pair to a plane number, and it rests on the HRM for that tie.

Quantifier "x=1,...,8": all eight are graded. The table makes each PTH bit and each PTL bit 15-1 both 1 and 0 across the eight planes. One address per plane does not establish every address value.

**Preconditions.**

- PC-SETUP, PC-FRAME.
- PC-MOVE: the sixteen pointer MOVEs are fetched in F2's VB block with the table values, before the first T_x read.
- PC-BPL({1..n}): B1-PC-BPL for the BPU = 8 run (reads in every plane's region on every graded line, none outside), B1-PC-BPLN for the BPU = n runs, n = 0..7 (reads in the regions of planes 1..n on every graded line; the planes above n are left to (b), which grades them).
- PC-STATIC(no CPU write to 0E0-0FE after setup).

**Contrasting case.** Use `l_3 + 0x0100` in the list: the first T_3 read moves up by `0x80` words.

For the preconditions: clear BPLEN (PC-BPL fails; in the BPU = 3 run, PC-BPLN fails); give a MOVE a wrong data word (PC-MOVE fails).

**Reference frame.** F2 from t_F(2). "First" means earliest after t_F(2).

---

### B2. Bit-plane pointer re-initialisation in vertical blank

**Claims (p.15):** none. opus:AA-bplxptl.c7 and sol:AA-bplxptl.c12 are declined (section 1); B2 checks the pointer effect of the writes without carrying them.

**Proposition.** A bit-plane pointer written by the processor or by the coprocessor during vertical blank determines where that plane's fetch begins in the frame that follows.

**Stimulus.** D-STD, BPU = 1, DMACON = `0x8380`. A_a = `0x090000`, A_b = `0x0A8000`. Two variants:

- **Copper variant.** The VB block writes BPL1PTH/PTL. The list holds A_a until, in F2 after line 0x100, the CPU replaces the two data words by A_b. The Copper therefore writes A_a in F2 and A_b in F3.
- **CPU variant.** The VB block has no BPL1PT MOVE. In every frame the CPU:
  1. polls INTREQR (01E) bit 5 (p.25: "05 VERTB 3 Start of Vertical blank");
  2. clears it by writing INTREQ (09C) = `0x0020`;
  3. writes BPL1PTH/PTL (A_a in F1 and F2, A_b in F3 and F4);
  4. writes a marker.

**Observation.** The first read in F2 and in F3 in either of the two areas `[A_a, A_a + 0x3000)` and `[A_b, A_b + 0x3000)`.

**Expected.** F2's first read is at `A_a / 2` and F3's first read is at `A_b / 2`. Page: "This pointer must be reinitalized by the processor or coprocessor to point to the beginning of Bit Plane data every vertical blank time."

"Every vertical blank" is followed in the stimulus (every frame is re-initialised) and its effect is established for F2 and F3 only.

What happens in a frame where the pointer is not re-initialised is left open: the page states an obligation, not an outcome. No test may check it.

**Preconditions.**

- PC-FRAME, PC-BPL({1}).
- Copper variant: PC-MOVE; the fetch of the data word of the BPL1PTL MOVE in F_i lies after t_F(i) and before F_i's first plane-1 read, and carries A_a (F2) or A_b (F3).
- CPU variant: the marker of F_i lies after t_F(i) and before F_i's first plane-1 read. VERTB is the page's own name for the start of vertical blank.

**Contrasting case.** Set A_b := A_a: F3's first read is at A_a.

For the preconditions: CPU variant with the write delayed to line 0x80 (by beam marker), so the marker falls after the first read; Copper variant with the new words written after F3's VB block has been fetched, so F3 still fetches A_a.

**Reference frame.** t_F. Vertical-blank timing is anchored on the Copper restart (p.21) and on VERTB (p.25).

---

### B3. Bit-plane data registers receive the DMA data, 16-pixel words, MSB first

**Claims (p.15; the sentence ends on p.16):** opus:AA-bplxdat.c4, c6, c11; sol:AA-bplxdat.c21, c22, c27. The clause "16/32/64 pixels" of opus:AA-bplxdat.c10 and sol:AA-bplxdat.c26 is carried by the joint assertion J5; B3 is its 16-pixel part (FMODE = 0), B5 its 32- and 64-pixel parts.

**Proposition.** For each plane x = 1..8, the words that plane x's pointer fetches reach the display as 16-pixel words (FMODE = 0), most significant bit first, that is, leftmost.

**Stimulus.**

- D-STD with BPU = 8, FMODE = 0, PAL-ON; DMACON = `0x8380`.
- One run per x. Plane x points to P_x, filled with `W(i) = 0x8000 | ((i & 0x3FFF) << 1)` for word index i = 0, 1, 2, .... Every word is unique, has bit 15 = 1 and bit 0 = 0, so a reversed bit order cannot match.
- All other planes point to Z.

**Observation.** In F2, for each chip line n = 0x30..0xFF:

- w_n = the sequence of reads in P_x on line n (data values, in time order);
- the output of line n, with each output pixel classed as cON or not cON.

**Expected.** On every such line, the cON/not-cON sequence contains, as one contiguous run, the bits of all words of w_n except its first and last, in read order, each word from bit 15 down to bit 0, and each bit occupying exactly 2 output pixels.

Page quotes:

- "These registers receive the DMA data fetched from RAM by the Bit Plane address pointers described above."
- "... for that word (16/32/64 pixels). The MSB is output first, and" (p.15), continued "is therefore always on the left." (p.16; checked on `bands\p16_band1.png`).

The 16-pixel width in FMODE = 0 is the first row of p.23's table: "0 0 by 2 bytes (as before) normal CAS 16".

Quantifier: every graded line of every plane. The first and last word of a line may be cut by the window edge, and the page says nothing about that edge, so they are not graded. Lines 0x100-0x12B are inside the window but carry no tag and are not graded.

**Preconditions.** PC-FRAME, PC-TAG, PC-BPL({1..8}); P_x is read on every graded line, the other seven planes only in Z; w_n has at least 3 words.

**Contrasting case.** Fill P_x with zeros: no cON pixel appears, and the expected run is absent.

For the preconditions: clear BPLEN (PC-BPL fails); remove the tag MOVEs (PC-TAG fails).

**Reference frame.** Chip lines by line anchor (bit-plane reads lie inside the line); output lines by tag.

---

### B4. Bit-plane data registers written by the processors; the plane-1 write triggers the conversion

**Claims (p.15):** opus:AA-bplxdat.c1, c2, c7, c8, c9, c14; sol:AA-bplxdat.c1, c2, c3, c4, c5, c6, c7, c8, c9, c23, c24, c25.

**Proposition.** BPL1DAT..BPL8DAT (write-only, at `0x110`..`0x11E`, two apart) can be written by the 68000 and by the Copper. They hold one word for each of up to eight planes. The words are converted to pixels, together and MSB first, when BPL1DAT is written, not when BPL2DAT-BPL8DAT are written.

**Stimulus.**

- D-STD window, BPU = 8, FMODE = 0, PAL-ON. DMACON = `0x8280` (DMAEN, COPEN): no bit-plane DMA.
- Patterns `Q_k = 0xC000 | (1 << k)` for k = 1..8.
- **Clear.** In F2's vertical blank, BPL8DAT..BPL2DAT = 0, then BPL1DAT = 0.
- **Events E_k** (k = 1..8), at line `e_k = 0x40 + 0x0C*(k-1)`, H = 0x60: write BPL8DAT..BPL2DAT (Q_k into BPLkDAT, 0 into the others; all 0 for k = 1), then BPL1DAT (Q_1 for k = 1, otherwise 0).
- **Event A**, at line 0xA0: BPL8DAT..BPL2DAT = Q_8..Q_2, then BPL1DAT = Q_1.
- **Delayed triggers D_k** (k = 2..8), at line `g_k = 0xAC + 0x08*(k-2)`: write BPL8DAT..BPL2DAT with Q_k into BPLkDAT and 0 elsewhere, and no BPL1DAT. At line g_k + 4 write BPL1DAT = 0. Both at H = 0x60.
- **CPU run:** the 68000 performs every event as a beam marker.
- **Copper run:** the same events as Copper MOVEs after WAIT(line, 0x60), with the clear placed in the VB block. The CPU does only the setup.

**Observation.** Outputs of the event lines and of the lines between them in F2.

**Expected.**

1. On the output of line e_k, the line of the trigger, Q_k's pattern occurs at least once. A word's **pattern** is a run of 32 consecutive output pixels, bit 15 leftmost, each bit 2 output pixels, cON for a 1 and non-cON for a 0. The pixels on either side of the run are not graded: the page does not say what the output shows before the trigger or after the 16 bits. On the output of line 0xA0 the pattern of `Q_1|...|Q_8 = 0xC1FE` occurs at least once.
2. For each D_k, Q_k's pattern occurs on none of the outputs of lines g_k .. g_k + 3, and it occurs on the output of line g_k + 4.
3. Lines 0x30 .. e_1 - 1 show no cON pixel (the buffers were cleared).

Page: "They may also be written by either micro. They act as a 8 word parallel to serial buffer for up to 8 memory "Bit Planes". (x=1-8 The parallel to serial conversion is triggered whenever bit plane #1 is written, indicing the completion of all bit planes for that word (16/32/64 pixels). The MSB is output first". "Either micro" is read as the 68000 and the Copper, which the pages call "processor or coprocessor".

Quantifiers:

- "whenever": 16 triggers per writer are graded. That does not establish every possible trigger.
- "up to 8": each of the eight words alone (E_k, D_k) and all eight together (A).

The page does not say what the output shows after the 16 bits have gone out, or on a line with no trigger. So expected 1 grades only the occurrence of Q_k after the trigger, and expected 2 grades only the absence of Q_k before its trigger.

**Preconditions.**

- PC-FRAME, PC-TAG; no read in any P_k or Z region (no bit-plane DMA).
- CPU run: PC-BEAM for every event.
- Copper run: PC-MOVE; each event's MOVEs are attributed to the event line.

**Contrasting case.** (1) Omit BPL1DAT at E_3: Q_3 does not appear on line e_3. (2) Move D_k's BPL1DAT write to line g_k: Q_k appears on line g_k.

For the preconditions: set BPLEN (reads appear); PC-BEAM with target H = 0xE0 (the re-read shows the next line).

**Reference frame.** Tags. Event lines come from beam markers or Copper WAITs.

---

### B5. Bit-plane words of 32 and 64 pixels

**Claims (p.15):** none directly. The clause "(16/32/64 pixels)" of opus:AA-bplxdat.c10 and sol:AA-bplxdat.c26 is carried by the joint assertion J5, whose parts are B3 (16 pixels), B5-32 and B5-64.

**Proposition.** For each plane x = 1..8:

- **B5-32.** With FMODE = `0x0001` (p.23: BPAGEM = 0, BPL32 = 1, "by 4 bytes", "normal CAS", bus width 32) the fetch of plane x advances by 4 bytes, and the 4 bytes fetched reach the display as one 32-pixel word, MSB first. The word at the lower address holds the more significant half, so a fetch at byte address A shows the words at A and A+2 in this order, leftmost first.
- **B5-64.** With FMODE = `0x0003` (p.23: BPAGEM = 1, BPL32 = 1, "by 8 bytes", "double CAS", bus width 32) the fetch of plane x advances by 8 bytes, and the 8 bytes fetched reach the display as one 64-pixel word, MSB first: a fetch at byte address A shows the words at A, A+2, A+4 and A+6 in this order, leftmost first.

The order of the words within a fetch is the byte order of the 68000, whose long word keeps its high-order word at the lower address; p.15 says the MSB is output first.

**Stimulus.**

- D-STD with BPU = 8, PAL-ON, DMACON = `0x8380`, and FMODE written in the setup with the value above. DDFSTRT and DDFSTOP are those of D-STD: the pages give no other fetch window for the wide modes, so the number of fetches per line is measured, not specified.
- One run per plane x and per FMODE value. Plane x points to P_x, filled with `W(i) = 0x8000 | ((i & 0x3FFF) << 1)` for word index i = 0, 1, 2, ... as in B3: every word is unique, so the words at A, A+2, A+4 and A+6 are four distinct values and the display shows which memory word came where.
- All other planes point to Z.
- The harness supplies the three words that follow the primary read address through its `chip48` path, wrapping inside the aligned 16-byte block; the Chip-memory port reports the primary address only (`nanomig_harness.h`).

**Observation.** In F2, for each chip line n = 0x30..0xFF: R_n = the reads in P_x on line n (word addresses, in time order), and the output of line n.

**Expected.** Let g = 2 words per fetch (FMODE = `0x0001`) or 4 words per fetch (FMODE = `0x0003`). On every such line, with i the word index of a read in R_n, the cON/not-cON sequence of the line contains, as one contiguous run, the bits of the words W(i), W(i+1), ..., W(i+g-1) of every read of R_n except its first and last, in read order, each word from bit 15 down to bit 0, and each bit occupying exactly 2 output pixels.

Page quotes:

- "... for that word (16/32/64 pixels). The MSB is output first, and" (p.15).
- p.23's table: "0 1 by 4 bytes normal CAS 32" and "1 1 by 8 bytes double CAS 32" (BPAGEM, BPL32, "Bitplane Fetch Increment", "Memory Cycle", "Bus Width").

Quantifier: every graded line of every plane. The first and last fetch of a line may be cut by the window edge, and the pages say nothing about that edge, so they are not graded.

**Preconditions.** PC-SETUP, PC-FRAME, PC-TAG and PC-PIXEL as in B3, and:

- B5-PC-STRIDE-32 and B5-PC-STRIDE-64: on every graded line the reads of P_x lie at byte offsets from P_x that are multiples of the fetch increment (4 resp. 8 bytes), and consecutive reads differ by exactly that increment (p.23, "Bitplane Fetch Increment": the pointer starts at P_x and advances by the increment per fetch).
- B5-PC-WORDS-32 and B5-PC-WORDS-64: at least 3 reads of P_x on every graded line.
- B5-PC-BPL-32 and B5-PC-BPL-64 are PC-BPL({1..8}) of the run of each FMODE value.

**Contrasting case.** Fill P_x with zeros: no cON pixel appears, and the expected run is absent.

For the preconditions: clear BPLEN (PC-BPL fails); FMODE = 0 in the run of that FMODE value (the reads step by 2 bytes: PC-STRIDE fails); DDFSTOP = `0x0040` (two words per line: PC-WORDS fails); remove the tag MOVEs (PC-TAG fails).

**Reference frame.** Chip lines by line anchor; output lines by tag.

---

### S1. Sprite pointers: addresses, and the address they hold

**Claims (p.28):** opus:AA-sprxpth.c1, c2, c4; sol:AA-sprxpth.c1, c2, c12; opus:AA-sprxptl.c1, c2, c4; sol:AA-sprxptl.c1, c2, c12. S1 is also a part of the joint assertion J4 (opus:AA-sprxptl.c6), which carries that id.

**Proposition.** For each x = 0..7:

- SPRxPTH (write-only, at `0x120 + 4*x`) holds the high 5 bits of the 20-bit address of sprite x's DMA data.
- SPRxPTL (write-only, at `0x122 + 4*x`) holds the low 15 bits.

**Stimulus.**

- D-STD, BPU = 1 (plane 1 in Z), PAL-SPR; DMACON = `0x83A0` (SET, DMAEN, BPLEN, COPEN, SPREN).
- Sprite x's list lies at `A_(x+1)` (region T_(x+1)): start 0x60, stop 0x64, lores X_x, four rows DATA = DATB = `0x8000`, then the end pair.
- The VB block writes SPRxPTH = h_(x+1), SPRxPTL = l_(x+1) for all x.
- Addresses per p.28 ("SPRxPTH 120 W A", "SPRxPTL 122 W A") and p.8 (SPR0PTH 120 ... SPR7PTL 13E).

**Observation.** For each x, the first access in F2 to `[A_(x+1), A_(x+1) + 0x20)`.

**Expected.** For every x, a read at word address `A_(x+1) / 2` (word 0 of list x). Page: "SPRxPTH 120 W A Sprite x pointer (High 5 bits)", "SPRxPTL 122 W A Sprite x pointer (Low 15 bits) This pair of registers contains the 20 bit address of Sprite x (x=0,1,2,3,4,5,6,7) DMA data."

That the list read through pointer x is sprite x's is graded in S14(b). There, a write to SPRxPOS moves exactly the sprite whose list is read through pointer x.

**Preconditions.** PC-SETUP, PC-FRAME, PC-MOVE (the 16 pointer MOVEs in F2's VB block), PC-SPR(x) for all x.

**Contrasting case.** Use `l_5 + 0x0100` for sprite 4: its first read moves up by `0x80` words.

For the preconditions: clear SPREN (PC-SPR fails).

**Reference frame.** F2 from t_F(2).

---

### S2. Sprite pointer initialisation in vertical blank

**Claims (p.28):** none. opus:AA-sprxptl.c7 is declined (section 1); S2 checks the pointer effect of the writes without carrying it.

**Proposition.** A sprite pointer written by the processor or the coprocessor during vertical blank determines where sprite x's list is read in the following frame.

**Stimulus.** As B2, for sprite 0 (D-STD, BPU = 1, PAL-SPR, DMACON = `0x83A0`):

- list A at L_0 (`0x0B0000`), list B at `0x0B8000`; both start 0x60, stop 0x68, X = 0x90, rows `0x8000`;
- Copper variant: the CPU replaces the MOVE data words in F2 after line 0x100;
- CPU variant: VERTB-polled writes of SPR0PTH/PTL (list A in F1 and F2, list B in F3 and F4) plus a marker.

**Observation.** The first read in F2 and in F3 in either list area.

**Expected.** F2 at list A's word 0 and F3 at list B's word 0. Page: "These address registers must be initialized by the processor or coprocessor every vertical blank time."

What is read when the pointer is not re-initialised is left open; it is not checked.

**Preconditions.** PC-FRAME; as in B2 (the write is fetched, or its marker falls, between t_F(i) and the first list read of F_i); PC-SPR(0).

**Contrasting case.** List B := list A: F3 reads list A.

For the preconditions: the same delayed-write changes as in B2.

**Reference frame.** t_F; VERTB.

---

### S3. Sprite register addresses; the A-buffer write arms, the SPRxCTL write disables

**Claims (p.28):** opus:AA-sprxpos.c1, c2; sol:AA-sprxpos.c1, c2; opus:AA-sprxctl.c1, c2, c20; sol:AA-sprxctl.c1, c2, c34, c35; opus:AA-sprxdata.c1, c2; sol:AA-sprxdata.c1, c2, c17; opus:AA-sprxdatb.c1, c2, c9, c10; sol:AA-sprxdatb.c1, c2.

**Proposition.** For each sprite x = 0..7:

- SPRxPOS (`0x140 + 8*x`) positions sprite x;
- a write to SPRxDATA (`0x144 + 8*x`) arms it;
- a write to SPRxDATB (`0x146 + 8*x`) alone does not arm it;
- a write to SPRxCTL (`0x142 + 8*x`) disables sprite x and no other sprite.

All four registers are write-only.

**Stimulus.**

- Manual sprites (2.8): D-STD, BPU = 1 (Z), PAL-SPR, DMACON = `0x8380` (no SPREN).
- Setup, for x = 0..7: SPRxPOS = `0x4000 | (X_x >> 1)`, SPRxCTL = `0x0000` (X_x is even, so SH2 = 0), SPRxDATB = `0xE001`, SPRxDATA = `0xE001`.
- In F2, for each x, at line `d_x = 0x50 + 0x14*x`, H = 0x10: SPRxCTL = `0x0000`. At d_x + 4: SPRxDATB = `0xE001` only. At d_x + 8: SPRxDATA = `0xE001`. All are beam markers. d_7 + 8 = 0xE4.
- Addresses are those of p.28 and of the address list pp.8-9 (SPR0POS 140, SPR0CTL 142, SPR0DATA 144, SPR0DATB 146, ..., SPR7DATB 17E).

**Observation.** Outputs of F1 and F2. **Column x** is the 32-output-pixel span that begins at the left edge of the x-th cS pattern from the left in F1.

**Expected.** In F2, column x:

- shows the 0xE001 pattern on lines 0x30 .. d_x - 1;
- shows no cS pixel on lines d_x + 1 .. d_x + 7;
- shows the pattern again on lines d_x + 9 .. 0xFF.

Lines d_x and d_x + 8 are left open, because the write falls inside the line. On lines d_x + 1 .. d_x + 7 every other column keeps its pattern.

Page:

- "Writing to SPRxCTL disables the corresponding sprite."
- "Note: Writing to the A buffer enables (arms) the sprite. Writing to the SPRxCTL register disables the sprite."

Quantifier "corresponding": each of the eight sprites is disabled while the other seven are observed.

**Preconditions.**

- PC-FRAME, PC-TAG, PC-BEAM for all 24 writes.
- F1: all eight columns show the pattern on every graded line (see S4).
- PC-STATIC(SPREN is never set).

**Contrasting case.** (1) Omit the SPRxCTL write at d_x: column x is present on lines d_x + 1 .. d_x + 7. (2) Omit the SPRxDATA write at d_x + 8: column x stays absent until the end of F2. (3) Write SPRxDATA instead of SPRxDATB at d_x + 4: column x is present on lines d_x + 5 .. d_x + 7, which the "DATB alone does not arm" clause forbids.

For the preconditions: SPRxDATA = SPRxDATB = 0 at setup, so column x is absent in F1; the PC-BEAM change.

**Reference frame.** Tags; beam markers.

---

### S4. Output at horizontal coincidence: every line while enabled, MSB first on the left

**Claims (p.28):** opus:AA-sprxdatb.c5, c8, c11; sol:AA-sprxdata.c12, c15, c16, c18.

**Proposition.** While sprite x is enabled, the words in its A and B buffers are shifted out, MSB first and leftmost, whenever the beam reaches the horizontal position in SPRxPOS, which means on every line.

**Stimulus.** The S3 setup. F1 is graded, with no event in it.

**Observation.** Outputs of lines 0x30..0xFF in F1.

**Expected.** On every graded line, each column x shows exactly 6 cS, 24 non-cS and 2 cS output pixels. This is 0xE001: 3 pixels on, 12 off, 1 on, with each lores sprite pixel 2 output pixels wide (sprites are 140 ns on a lores display, p.17: "0 | 0 | ECS defaults(LORES,HIRES=140nS,SHRES=70nS"). The pattern begins at the same output pixel on every line.

Page:

- "These registers buffer the Sprite image data. ... When a horizontal coincidence occurs the buffers are dumped into shift registers and serially outputed to the display, MSB first on the left."
- "If enabled, data in the A and B buffers will be output whenever the beam counter equals the sprite horizontal position value in the SPRxPOS register."

Quantifier "whenever": every graded line of the window, for all eight sprites. Lines outside the display window are not graded: p.4 says "BRDRSPRT, when high, allows sprites to be visible in border areas", and BRDRSPRT is 0.

**Preconditions.** PC-FRAME, PC-TAG, PC-SETUP.

**Contrasting case.** DATA = DATB = `0x8007`: the page predicts 2 cS, 24 non-cS, 6 cS, so the 0xE001 check goes red.

For the preconditions: as S3.

**Reference frame.** Tags.

---

### S5. Horizontal start bits SH10-SH1

**Claims (p.28):** two graded lines carry them:

- the SH1 line, **S5-SH1**: opus:AA-sprxctl.c10; sol:AA-sprxctl.c24;
- the SH10-SH2 line, **S5**: opus:AA-sprxpos.c7; opus:AA-sprxctl.c14; sol:AA-sprxctl.c30.

A third line, **S5-SH10-SH3**, carries no claim id: it is the comparator of S5 for k = 3..10 alone, SPRxPOS bits 7-0, and is the part of J1 that concerns SPRxPOS. S5-SH1, S5 and S5-SH10-SH3 are also parts of the joint assertions J1, J2 and J3 (section 3, end), which carry sol:AA-sprxpos.c4; opus:AA-sprxpos.c8; sol:AA-sprxpos.c14; opus:AA-sprxctl.c17; sol:AA-sprxctl.c31.

**Proposition.** The sprite's horizontal start is one value, SH10..SH0:

- SH10-SH3 are SPRxPOS bits 7-0;
- SH2 is SPRxCTL bit 0 (140 ns);
- SH1 is SPRxCTL bit 4 (70 ns).

Bit SHk moves the sprite by 35 * 2^k ns.

**Stimulus.**

- D-STD, BPU = 1 (Z), PAL-SPR, DMACON = `0x83A0`, FMODE = 0.
- **DMA bands.** Sprite x's list: start `b_x = 0x40 + 0x18*x`, stop b_x + 0x10, 16 rows DATA = `0x8000`, DATB = `0x0000`.
- All eight sprites use the same horizontal value H: POS bits 7-0 = H >> 3, CTL bit 0 = (H >> 2) & 1, bit 4 = (H >> 1) & 1, bit 3 = H & 1.
- Runs: references with H = B for B in {0x440, 0x480, 0x240}, and one run for each k = 1..10 with H = B_k + 2^k. B_k = 0x480 for k = 6, 0x240 for k = 10, and 0x440 otherwise. Bit k of B_k is 0, so exactly one bit changes.
- In lores pixels B/4: 272, 288 and 144, all inside the window.

**Observation.** In F2, on every output line of band x that shows the sprite in the reference run: X = the output-pixel index of the leftmost cS pixel.

**Expected.** `X(B_k + 2^k) - X(B_k) = 2^(k-1)` output pixels, for every sprite x and every such line: S5-SH1 for k = 1, S5 for k = 2..10, S5-SH10-SH3 for k = 3..10. One comparator grades all three.

Page:

- "07-00 SH10-SH3 Sprite horizontal start value. Low-order 3 bits are in SPRxCTL register below."
- "04 SH1=0 start horiz. value, 70nS increment"
- "00 SH2 start horiz. value, 140nS increment"
- "These 2 registers work together as position, size and feature Sprite control registers."

SH3-SH10 continue the binary value whose low bits are 35, 70 and 140 ns. p.2: "Resolution of ... horizontal sprite position has been improved to 35ns".

Quantifier "x": all eight sprites in every run.

**Preconditions.**

- PC-FRAME, PC-TAG, PC-SPR(x).
- In every run, each band shows exactly one 2-output-pixel cS run on the same lines as in its reference run.

**Contrasting case.** S5 and S5-SH10-SH3: set bit k+1 instead of bit k (k = 3): the shift is 2^k output pixels, and the check goes red. S5-SH1: its comparator is that of S5, which is live on k = 2..10; with the k = 1 run setting bit 2 (SH2) in place of bit 1 and the comparator applied with k = 2, S5-SH1 passes.

For the preconditions: clear SPREN (no reads, no pixel).

**Reference frame.** Tags. Only differences between runs are graded, so the relation between beam counter and output pixel column need not be known.

---

### S6. SH0: the 35-ns step

**Claims (p.28):** opus:AA-sprxctl.c11; sol:AA-sprxctl.c26.

**Proposition.** SPRxCTL bit 3 (SH0) moves the sprite by 35 ns.

**Stimulus.**

- As S5, but BPLCON3 = `0x0CC0` (SPRES1 = SPRES0 = 1). p.18: "1 | 1 | SHRES(35nS)", the last row of the SPRES table begun on p.17. p.31 confirms "eg 35ns sprites on a lores screen".
- Rows DATA = DATB = Π with Π ∈ {`0xAAAA`, `0x5555`, `0xFFFF`}.
- H = `0x440` (SH0 = 0) or `0x441` (SH0 = 1).
- Six runs.

**Observation.** V(Π, SH0) = the number of cS output pixels, per band line, within the 12 output pixels that begin 2 pixels left of the band's cS run in the `0xFFFF` run with the same SH0.

**Expected.**

- `{V(0xAAAA, 0), V(0xAAAA, 1)} = {0, 8}`: one of the two settings shows all eight alternate pixels, the other shows none.
- `V(0x5555, 1) = V(0xAAAA, 0)`.

The reasoning: a 16-pixel sprite of 35-ns pixels covers 8 output pixels, two 35-ns pixels per output pixel (2.11). The precondition below measures that exactly one of each pair reaches the output. A 35-ns shift then moves an alternating pattern from the shown pixels to the hidden ones. Page: "03 SH0=0 start horiz. value, 35nS increment".

**Preconditions.**

- PC-FRAME, PC-TAG, PC-SPR(x).
- `V(0xFFFF, 0) = V(0xFFFF, 1) = 8`: the sprite is shown and is 8 output pixels wide.
- `V(0xAAAA, s) + V(0x5555, s) = 8` for s = 0, 1: exactly one of each pair of 35-ns pixels reaches the output. This is the output property the check rests on; it is measured here, not assumed.

**Contrasting case.** SH0 = 0 in both runs: V is equal, and the first relation goes red.

For the preconditions: SPRES = 01 (140-ns sprites) makes `V(0xFFFF)` exceed 8; clearing SPREN makes it 0.

**Reference frame.** Tags; the `0xFFFF` run locates the span.

---

### S7. Reset values SH1 = 0 and SH0 = 0

**Claims (p.28):** opus:AA-sprxctl.c15, c16; sol:AA-sprxctl.c25, c27.

**Proposition.** After reset, before any write to SPRxCTL, bits SH1 and SH0 of every SPRxCTL are 0.

**Stimulus.** Manual sprites, DMACON = `0x8380` (no SPREN), D-STD, BPU = 1 (Z), PAL-SPR. All eight sprites, column X_x. The runs are:

| Run | BPLCON3 | Setup writes, for x = 0..7 | SPRxCTL write |
|---|---|---|---|
| R0 | `0x0C40` (SPRES = 01, LORES 140 ns, p.17) | SPRxPOS = `0x4000 \| (X_x >> 1)`, SPRxDATB = 0, SPRxDATA = `0x8000` | none |
| R1 | as R0 | as R0 | `0x0000`, written before DATB/DATA |
| R2 | as R0 | as R0 | `0x0010` (SH1 = 1) |
| R0s | `0x0CC0` (SPRES = 11) | as R0, but DATA = DATB = `0xAAAA` | none |
| R1s | as R0s | as R0s | `0x0000` |
| R2s | as R0s | as R0s | `0x0008` (SH0 = 1) |
| R1f | as R0s | as R0s, but DATA = DATB = `0xFFFF` | `0x0000` |

D-STD's SPRxCTL write is left out in R0 and R0s.

**Observation.** In F1: X per column (R0, R1, R2) and V per column (R0s, R1s, R2s, R1f). X is the output-pixel index of the leftmost cS pixel (as in S5). V is the number of cS output pixels, per graded line, within the 16 output pixels that begin 4 output pixels left of column x's leftmost cS pixel in R1f. R1f locates the sprite, because with `0xAAAA` a column may show no cS pixel at all. The window holds the 8-output-pixel SHRES sprite at every position the unknown reset values of SH2 and SH1 and the reset value of SH0 can give it (at most 2 + 1 + 1/2 output pixels right of R1f), and it lies inside column x's 48-output-pixel spacing.

**Expected.**

- `X(R0) - X(R1) ∈ {0, 2}` output pixels. SH1 is 0; SH2, whose reset value no page states, adds 0 or 2 pixels.
- `V(R0s) = V(R1s)`. SH0 is 0; SH2 and SH1 move by 4 or 2 35-ns pixels, which keeps the parity.

Page: "04 SH1=0 start horiz. value, 70nS increment", "03 SH0=0 start horiz. value, 35nS increment". The "=0" is read with the legend on p.16: "=0/=1 bit values initialized by RST_ pin going low." The legend is printed under the BPLCON table. p.28 prints no legend of its own; see section 4.

The reset values of SH2, SV8, SV9, EV8, EV9, ATT and EV7-EV0 are not stated anywhere. No test may grade them.

**Preconditions.**

- The run starts from reset (2.1). PC-STATIC(SPR0CTL..SPR7CTL) in R0 and R0s.
- `X(R2) - X(R1) = 1`, i.e. SH1 acts (S5). `V(R1s) ≠ V(R2s)`, i.e. SH0 acts (S6). `V(R1f) = 8` in every column: the SHRES sprite is shown and is 8 output pixels wide.
- The SH0 relation must hold before the SH1 relation is graded. If SH0 were 1, the lores X in R0 could move by half a pixel, and the set {0, 2} would not apply.

**Contrasting case.** R0 with SPRxCTL = `0x0010` written: `X - X(R1) ∈ {1, 3}`, red. R0s with SPRxCTL = `0x0008` written: V equals V(R2s), red.

For the preconditions: add a SPR0CTL write in R0 (the static check fails).

**Reference frame.** Tags; the reference runs R1 and R1s.

---

### S8. Vertical start bits SV9-SV0

**Claims (p.28):** opus:AA-sprxpos.c5, c6; sol:AA-sprxpos.c13; opus:AA-sprxctl.c8, c12; sol:AA-sprxctl.c22, c28.

**Proposition.** The vertical start is one 10-bit value:

- SV7-SV0 are SPRxPOS bits 15-8;
- SV8 (the 9th bit) is SPRxCTL bit 2;
- SV9 (the 10th bit) is SPRxCTL bit 6.

Bit SVk moves the sprite's start by 2^k lines.

**Stimulus.**

- DMA sprites, all eight in one run, at columns X_x, rows `0x8000`. D-STD, BPU = 1, PAL-SPR, DMACON = `0x83A0`.
- Run pairs (reference, shifted) from the table below, with lists of height stop - start. Start and stop keep the same parity (p.4 note).
- Start-only runs for k = 1..3: reference (0x40, 0x50), shifted (0x40 + 2^k, 0x50).
- k = 9 runs in the long frame LF.

| k | reference (start, stop) | shifted |
|---|---|---|
| 0 | 0x40, 0x48 | 0x41, 0x49 |
| 1 | 0x40, 0x48 | 0x42, 0x4A |
| 2 | 0x40, 0x48 | 0x44, 0x4C |
| 3 | 0x40, 0x50 | 0x48, 0x58 |
| 4 | 0x40, 0x48 | 0x50, 0x58 |
| 5 | 0x40, 0x48 | 0x60, 0x68 |
| 6 | 0x30, 0x38 | 0x70, 0x78 |
| 7 | 0x40, 0x48 | 0xC0, 0xC8 |
| 8 | 0x28, 0x30 | 0x128, 0x130 |
| 9 (LF) | 0x40, 0x48 | 0x240, 0x248 |

In every reference value bit k is 0, so the shifted value changes exactly bit k.

**Observation.** T_first(x) = the time of the read of list x's word 2 (the first DATA word) minus t_F of its frame, in F2 (in LF, the second long frame). Compared with LOM.

**Expected.**

- Joint runs: Δ(T_first) = 2^k lines for every x.
- Start-only runs: Δ(T_first) = 2^k.

Page:

- "15-08 SV7-SV0 Start vertical value. High bit(SV8) is in SPRxCTL reg below."
- "02 SV8 Start vert. value 9th bit"
- "06 SV9 start vert. value 10th bit"

Which line a start value names in absolute terms, and on which line the first data word is read, is not stated. Only the shifts are graded.

**Preconditions.** PC-FRAME (with its LF form for k = 9), PC-TAG, PC-SPR(x) with the right number of data pairs, LOM valid, PC-STATIC(start and stop of equal parity in every list).

**Contrasting case.** Shift by 2^(k+1): Δ = 2^(k+1), red.

For the preconditions: LF without VARBEAMEN (the field is a normal PAL field, shorter than 0x249 lines, so PC-FRAME fails); clear SPREN.

**Reference frame.** t_F and P (2.11). Line numbers are counted relative to the reference run only.

---

### S9. Vertical end bits EV9-EV0

**Claims (p.28):** opus:AA-sprxctl.c6, c9, c13; sol:AA-sprxctl.c20, c23, c29.

**Proposition.** The vertical end (stop) is one 10-bit value:

- EV7-EV0 are SPRxCTL bits 15-8;
- EV8 is bit 1;
- EV9 is bit 5.

Bit EVk moves the sprite's end by 2^k lines.

**Stimulus.** The S8 runs (joint shifts), plus end-only runs for k = 1..3: reference (0x40, 0x50), shifted (0x40, 0x50 + 2^k).

**Observation.** T_end(x) = the time of the read of list x's end pair (its first word) minus t_F. Compared with LOM.

**Expected.**

- Joint runs: Δ(T_end) = 2^k for every x.
- End-only runs: Δ(T_end) = 2^k and Δ(T_first) = 0.
- Start-only runs of S8: Δ(T_end) = 0.

The end-only and start-only runs for k = 1..3 show that start and end are separate fields.

Page:

- "15-08 EV7-EV0 End (stop) vert. value. low 8 bits"
- "01 EV8 End (stop) vert. value 9th bit"
- "05 EV9 end (stop) vert. value 10th bit"

Whether the stop line itself is displayed is not stated and is not graded.

For k = 0 start and end can only move together (p.4 parity note). So an exchange of SV0 and EV0 cannot be seen within the page's rules.

**Preconditions.** As S8.

**Contrasting case.** An end-only run shifted by 2^(k+1): red.

For the preconditions: as S8.

**Reference frame.** As S8.

---

### S10. The attach bit, odd sprites only

**Claims (p.28):** opus:AA-sprxctl.c7; sol:AA-sprxctl.c21.

**Proposition.** SPRxCTL bit 7 (ATT) attaches an odd sprite to its even partner. The same bit in an even sprite's SPRxCTL attaches nothing.

**Stimulus.**

- Manual sprites, D-STD, BPU = 1, PAL-ATT (BPLCON4 = `0x0012`), DMACON = `0x8380`.
- Four pairs p = 0..3 at lores `X_p = 0x90 + 0x30*p`.
- Even sprite 2p: POS for X_p, CTL, DATB = `0x0000`, DATA = `0xFFFF`.
- Odd sprite 2p+1: the same POS, its own CTL, DATB = `0x0000`, DATA = `0x0000` (armed and transparent).
- The CTL values give four runs:

| Run | CTL bit 7 = 1 in |
|---|---|
| a | no sprite |
| b | SPR1CTL and SPR5CTL |
| c | SPR3CTL and SPR7CTL |
| d | SPR0CTL, SPR2CTL, SPR4CTL, SPR6CTL (even sprites only) |

**Observation.** In F1, the colour of each pair's 32-output-pixel span on the graded lines.

**Expected.**

- A pair shows cO if its odd sprite's CTL has bit 7 set, and cE otherwise. So run b shows cO for pairs 0 and 2, run c for pairs 1 and 3.
- Run d shows cE for all four pairs.

Page: "07 ATT Sprite attach control bit(odd sprites only)". The colour follows from:

- p.18: ESPRM "provides the 4 high order color table address bits for even sprites" (16-31 here, all cE);
- p.4: "In the case of attached sprites OSPRM bits are used." (32-47 here, all cO).

The page states no function for bit 7 of an even sprite. The expected value for run d is the display of a pair that is not attached.

**Preconditions.** PC-FRAME, PC-TAG. In run a, every even sprite is visible as a 32-output-pixel cE span.

**Contrasting case.** Run b against run a: pairs 0 and 2 change from cE to cO.

For the preconditions: even DATA = 0, so nothing is visible.

**Reference frame.** Tags.

---

### S11. SSCAN2 disables the SH10 horizontal compare

**Claims (p.28):** opus:AA-sprxpos.c9; sol:AA-sprxpos.c15.

**Proposition.** With FMODE bit 15 (SSCAN2) set, SH10 takes no part in the horizontal coincidence.

**Stimulus.**

- Manual bands (2.8), no sprite DMA: sprite x on lines `[0x40 + 0x18*x, +0x10)`, data `0x8000`.
- H = `0x240` (SH10 = 0; POS bits 7-0 = `0x48`) or `0x640` (SH10 = 1; POS bits 7-0 = `0xC8`).
- FMODE = `0x0000` or `0x8000` (p.23: "15 SSCAN2 global enable for sprite scan-doubling").
- Four runs: (SSCAN2, SH10) ∈ {0,1} x {0,1}.
- D-STD, BPU = 1, PAL-SPR, DMACON = `0x8380`.

**Observation.** Outputs of the band lines in F2.

**Expected.**

- SSCAN2 = 0: `X(SH10 = 1) - X(SH10 = 0) = 512` output pixels in every band.
- SSCAN2 = 1: the band lines of the two runs are identical, pixel for pixel.

Page: "If SSCAN2 bit in FMODE is set, then disable SH10 horizontal coincidence detect." p.5 agrees: "LISA will ignore SH10 compare bit in SPRxPOS when scan-doubling".

Where the sprite then appears (at the position without SH10, and possibly a second time) is not stated. Only the identity is graded.

Quantifier: all eight sprites.

**Preconditions.** PC-FRAME, PC-TAG, PC-BEAM. Every band shows the sprite in the run SSCAN2 = 0, SH10 = 0. Every band shows at least one cS pixel in each of the two SSCAN2 = 1 runs: p.28 disables the SH10 compare, not the sprite, so the identity is never graded on two empty bands.

**Contrasting case.** SSCAN2 = 0 in the identity pair: the lines differ, red.

For the preconditions: data 0, so no sprite.

**Reference frame.** Tags; the other run.

---

### S12. With SSCAN2, SH10 enables scan-doubling of the individual sprite

**Claims (p.28):** opus:AA-sprxpos.c10; sol:AA-sprxpos.c16.

**Proposition.** With SSCAN2 set, SH10 of a sprite switches that sprite's scan-doubling on or off. With SH10 = 1 the sprite's DMA runs only on every second line, and the line in between repeats the previous line's data.

**Stimulus.**

- DMA bands: sprite x starts at `v_x = 0x40 + 0x14*x`, stop = v_x + 16, lores X = 0x90 (H = `0x240`; POS bits 7-0 = `0x48 | (SH10 << 7)`).
- Rows: DATA = DATB = `0x8000 | (r << 4)` for row r (unique per row).
- A sprite with SH10 = 1 has 8 rows. A sprite with SH10 = 0 has 16 rows. This follows p.4: "When sprites are scan-doubled, only the position and control registers need be modified by the programmer; the data registers need no modification."
- FMODE = `0x8000`.
- Run A: even x have SH10 = 1, odd x have SH10 = 0. Run B: the other way round.
- Runs A' and B': the same with every start and stop one line later (odd parity).
- D-STD, BPU = 1, PAL-SPR, DMACON = `0x83A0`.

**Observation.** In F2: the times of the reads of list x's data pairs, and the outputs of each band.

**Expected.**

- SH10 = 1: list x's 8 data pairs are read with consecutive reads exactly 2 lines apart (LOM), then the end pair. The output shows each row on two consecutive lines, and the sequence of rows is r = 0..7.
- SH10 = 0: 16 pairs, 1 line apart, each row on one line.
- A' and B' against A and B: every read and every displayed row moves by exactly one line.

Page (p.28): "This bit is then free to be used by ALICE as an individual scan double enable." It is completed by p.4: "When enabled, individual SH10 bits in SPRxPOS registers control whether or not a given sprite is to be scan-doubled. When V0 bit of SPRxPOS register matches V0 bit of vertical beam counter, the given sprite's DMA is allowed to proceed as before. If they don't match, then sprite DMA is disabled and LISA reuses the sprite data from the previous line."

The absolute parity of the fetch lines (V0 of the line equal to SV0) is not graded. The pages do not say where in the line a sprite's DMA slot lies, so a sprite read at the very start of a line cannot be tied to its line number. The relative relations above follow from the same sentence.

**Preconditions.** PC-FRAME, PC-TAG, PC-SPR(x) (the list is read to its end pair), LOM valid. Every band is visible.

**Contrasting case.** FMODE = 0 with the SH10 = 1 sprites given 16-row lists: every line is read (1 line apart), and the doubling relation goes red.

For the preconditions: clear SPREN.

**Reference frame.** P; tags; the other runs.

---

### S13. The sprite DMA channel loads POS, CTL, DATA and DATB

**Claims (p.28):** none. opus:AA-sprxdatb.c6 and sol:AA-sprxdata.c13 are declined (section 1), as are the claims that place the POS/CTL load "during horizontal blank". S13 remains as a mechanism check.

**Proposition.** With sprite DMA, the words read from sprite x's list become its position and control (POS, CTL) and its image data (DATA, DATB).

**Stimulus.**

- Run D: DMA sprite 0, start 0x60, stop 0x70, lores X = 0x90, 16 rows with `DATA_r = 0x8000 | (r << 4)` and `DATB_r = 0x0001 | (r << 8)`. D-STD, BPU = 1, PAL-SPR, DMACON = `0x83A0`.
- Run M: SPREN clear. The CPU writes SPR0POS and SPR0CTL with the same two words as list word 0 and word 1, then SPR0DATB, SPR0DATA = `0x0001`, `0x8000` (row 0), before F1.

**Observation.** In F2 of run D: the sequence of data pairs read from list 0, and the output lines. In F1 of run M: the left edge X.

**Expected.**

- Run D: over consecutive output lines, the sprite shows `DATA_r | DATB_r`, MSB first, for r = 0..15 in the order read, one row per line.
- The left edge of run D equals that of run M.

Page:

- "They are usually loaded by the Sprite DMA channel, during horizontal blank" (POS, CTL).
- "They are usually loaded by the Sprite DMA channel" (DATA, DATB).

PAL-SPR shows any non-zero pixel as cS, so the OR of DATA and DATB is what is visible. Which read word goes to A and which to B is not graded.

**Preconditions.** PC-FRAME, PC-TAG, PC-SPR(0).

**Contrasting case.** Change row 5's words: the sixth displayed line changes accordingly.

For the preconditions: clear SPREN (no reads, no image).

**Reference frame.** Tags; the read order. No absolute line is used.

---

### S14. Either processor may load the sprite registers at any time

**Claims (p.28):** none. opus:AA-sprxctl.c19, sol:AA-sprxctl.c33, opus:AA-sprxdatb.c7 and sol:AA-sprxdata.c14 are declined (section 1).

**Proposition.** The 68000 and the Copper can each write SPRxPOS, SPRxCTL, SPRxDATA and SPRxDATB, and the write takes effect, both for a manual sprite and during DMA use. From which line on it takes effect is MEASURED: the page states no latency.

**Stimulus.**

**(a) Manual.** Sprite 0 manual; SPREN clear; D-STD, BPU = 1, PAL-SPR. The Copper VB block writes SPR0POS (X = 0x90), SPR0CTL = 0, SPR0DATB = SPR0DATA = `0xE001` in every frame. In F2:

| Event | Writer | Line, H | Write |
|---|---|---|---|
| e1 | CPU | 0x50, 0x08 | SPR0POS for X = 0xA0 |
| e2 | CPU | 0x70, 0xB0 | SPR0POS for X = 0xB0 (after the column on that line) |
| e3 | Copper | 0x90, 0x20 | SPR0POS for X = 0xC0 |
| e4 | CPU | 0xB0, 0x08 | SPR0CTL = `0x0001` (SH2 = 1, 140 ns), then SPR0DATA = `0xE001` (the CTL write disables, S3) |
| e5 | CPU | 0xC0, 0x08 | SPR0DATB = SPR0DATA = `0x8007` |
| e6 | Copper | 0xD0, 0x20 | SPR0DATB = SPR0DATA = `0xF00F` |

**(b) During DMA use.** DMA sprites x = 0..7, start 0x60, stop 0xA0, columns X_x, rows `0xE001`, DMACON = `0x83A0`. At line 0x80, H = 0x08, SPRxPOS is written with the same start byte and X_x + 8: by the CPU for even x, by the Copper for odd x.

**Observation.** Outputs in F2.

**Expected (a).** The column shows, on the graded lines:

| Graded lines | Position | Pattern |
|---|---|---|
| 0x30-0x4F | X = 0x90 | 0xE001 |
| 0x6F | X = 0xA0 | 0xE001 |
| 0x8F | X = 0xB0 | 0xE001 |
| 0xAF | X = 0xC0 | 0xE001 |
| 0xBF | two output pixels right of 0xAF | 0xE001 |
| 0xCF | same position | 0x8007 |
| 0xFF | same position | 0xF00F |

Lines 0x30-0x4F precede every write. Each other graded line is the last line before the next event: the new value is in effect there. The event lines themselves, and the lines between an event line and the graded line of its band (0x51-0x6E, 0x71-0x8E, 0x91-0xAE, 0xB1-0xBE, 0xC1-0xCE, 0xD1-0xFE), are not graded: for those lines the build MEASURES how many show the new position and pattern, and from which line on.

**Expected (b).** Column x is at X_x on the lines up to 0x7F where the reference shows it, and at X_x + 8 (16 output pixels right) on line 0x9F. Only column x moves when SPRxPOS is written, which ties list x (S1) to register set x. Lines 0x81-0x9E are MEASURED, not graded.

**Graded lines.** S14 (a and b together, no claim id) and S14-B (part (b) alone, no claim id; it is the part of J4). The effect of SPRxCTL bit 4 (SH1) is not part of S14: e4 writes the SH2 bit, whose effect S5 establishes, and SH1 is graded by S5-SH1.

Page:

- "however they may be loaded by either processor any time" (POS, CTL).
- "but may be loaded by either processor at any time" (DATA, DATB).

Quantifier "any time": sampled at line start, in mid-line, in vertical blank, by both processors, with and without DMA. That does not establish every instant, and the ids that state it are declined (section 1).

**Preconditions.** PC-FRAME, PC-TAG, PC-BEAM, PC-MOVE; PC-SPR(x) in (b).

**Contrasting case.** Omit e3: line 0xAF stays at X = 0xB0. Write SPR0CTL = `0x0000` at e4: lines 0xBF, 0xCF and 0xFF stay at the position of 0xAF. Omit the line-0x80 write for x = 3: column 3 stays at X_3 on line 0x9F.

For the preconditions: the PC-BEAM change; clear SPREN in (b).

**Reference frame.** Tags; beam markers; Copper WAIT lines.

---

### S15. SPRHSTRT and SPRHSTOP: the lines of the UHRES sprite

**Claims (p.28):** sol:AA-sprhstop.c5; opus:AA-sprhstop.c7; sol:AA-sprhstop.c11, c12, c13, c14, c15, c16, c17, c18, c19, c20, c21; sol:AA-sprhstrt.c5; opus:AA-sprhstrt.c6; sol:AA-sprhstrt.c11, c12, c13, c14, c15, c16, c17, c18, c19, c20, c21.

**Proposition.**

- SPRHSTRT (1D0) bits 10-0 = v10-v0 set the line where the UHRES sprite starts.
- SPRHSTOP (1D2) bits 10-0 = v10-v0 set the line where it stops.

Outside these lines the UHRES sprite pointer makes no memory access.

**Stimulus.**

- D-STD with BPLCON0 = `0x1080` (BPU = 1, UHRES bit 7). p.16: "UHRES= ultrahi res enables the UHRES pointers (for 1k*1k) (also needs bits in DMACON) (hires chips only)".
- DMACON = `0x83A0`. p.22, DMAEN: "Enable all DMA below (also UHRES dma)".
- The VB block writes SPRHPTH/SPRHPTL (1E8/1EA) = U, and BPLHPTH/BPLHPTL (1EC/1EE) = U2. p.27 on SPRHPTH/L: "It increments for the next line", so it is a pointer and is re-initialised each frame. BPLHSTRT = BPLHSTOP = 0; BPLHMOD = 0.
- SPRHSTRT = start and SPRHSTOP = stop (bit 15 = 0, bits 14-11 = 0) from the S8 table, extended with k = 10 (LF): reference (0x40, 0x48), shifted (0x440, 0x448).
- Also start-only and stop-only runs for k = 1..3 as in S8/S9.
- k = 9 and k = 10 run in LF.

**Observation.** Accesses in U in the observed frame, grouped by line: accesses closer together than P/2 form one group, and groups lie one line apart.

**Expected.**

- The U accesses form one run of consecutive-line groups, and there is no U access in the frame outside that run.
- Shifting start by 2^k moves the first group by 2^k lines (LOM). Shifting stop by 2^k moves the last group by 2^k lines. This holds for k = 0..10.
- A start-only shift reduces the number of groups by 2^k. A stop-only shift increases it by 2^k.

Page:

- "SPRHSTRT H 1D0 W A UHRES sprite vertical display start ... x x x x x v10 v9 v8 v7 v6 v5 v4 v3 v2 v1 v0"
- "SPRHSTOP H 1D2 W A UHRES sprite vertical display stop ... SPRHWRM x x x x v10 v9 v8 v7 v6 v5 v4 v3 v2 v1 v0" (checked on `bands\p28_band1.png`)

The UHRES sprite's data go to external logic, not to the display. p.27: "SPRHDAT H 078 W ext logic UHRES sprite identifier & data". So the only thing a test can see of its "display" is its pointer's accesses. This reading follows the twin register on p.15: "BPLHSTRT ... This controls the line when the data fetch starts for the BPLHPTH,L pointers."

Whether the stop line itself carries accesses is not stated and is not graded.

**Preconditions.**

- PC-FRAME (with its LF form), PC-TAG, PC-MOVE (SPRHPTH/PTL = U in every VB block).
- In the reference run there are at least two U groups. This is the precondition that the UHRES sprite pointer comes out at all.

**Contrasting case.** Shift start by 2^(k+1): red.

For the preconditions: clear the BPLCON0 UHRES bit, so there are no U accesses.

**Reference frame.** t_F, P, LOM. UHRES accesses come "after horiz line start" (p.27, "in the 1st & 3rd 'free' cycles (see BPLHPTH,L) after horiz line start"), so they are never tied to absolute lines by the line anchor.

---

### S16. SPRHWRM swaps the read/write direction of the UHRES sprite cycle

**Claims (p.28):** sol:AA-sprhstop.c6, c22. The ECS runs carry no claim id: the claim that states the chip sets says "only" and is declined (section 1).

**Proposition.** With SPRHSTOP bit 15 (SPRHWRM) set, the UHRES sprite's memory cycles become writes. With the bit clear they are reads. This holds with ECS and with AA behaviour.

**Stimulus.**

- The S15 reference run, plus BPLHSTRT = `0x0040` and BPLHSTOP = `0x0080` (bit 15 BPLHWRM = 0), so that U2 accesses appear too.
- Runs with SPRHSTOP = `0x0048` and `0x8048`.
- Each run is made once with AA behaviour selected and once with ECS behaviour selected. The ECS runs leave out the writes to BPLCON4, FMODE and CLXCON2, which the AA pages call new (p.2).

**Observation.** The direction of every access in U and U2 in F2.

**Expected.**

- SPRHWRM = 1: every U access is a write.
- SPRHWRM = 0: every U access is a read.
- In both runs every U2 access is a read, and the U groups lie on the same lines (LOM Δ = 0; bit 15 is not part of v10-v0).

The data a swapped cycle writes come from an external device and have no expected value.

Page: "SPRHWRM= Swaps the polarity of ARW* when the SPRHDAT comes out so that external devices can detect the RGA and put things into memory. (ECS  and later chips only)". The twin bit on p.15 reads the same: "BPLHWRM= ... (ECS and later versions)". ARW* is the chip's RAM write strobe: HRM p.379, Agnus pin 16, "ARW* Agnus RAM write O" (read on the scan, `appJ-pinlist\figures\p379.png`; the text layer displaces that column). A swapped ARW* therefore turns the cycle's read into a write, which the Chip-memory port shows as a write.

The "only" is not graded (section 1).

**Preconditions.** As S15, in both chip-set configurations. U accesses and U2 accesses are present.

**Contrasting case.** The SPRHWRM = 0 run against the SPRHWRM = 1 run: the directions differ.

For the preconditions: clear UHRES (no U accesses); BPLHSTRT = BPLHSTOP = 0 (no U2 accesses).

**Reference frame.** Region U; t_F; P.

---

### C1. CLXCON bits 15-12: odd sprites included, ORed with their even partner

**Claims (p.18):** opus:AA-clxcon.c5, c7, c8, c9, c10; sol:AA-clxcon.c5, c7, c8, c9, c10.

**Proposition.** CLXCON (098) bits 15, 14, 13 and 12 (ENSP7, ENSP5, ENSP3, ENSP1) each include one odd sprite in collision detection, where it counts together with (ORed with) its even partner. Each bit includes only its own sprite.

**Stimulus.**

- D-STD, BPU = 1 (Z), PAL-SPR, DMACON = `0x83A0`, CF.
- The sprites lie on lines 0x60-0x6F with DATA = `0xFFFF`, DATB = 0. "Sprite A over sprite B" means A at lores X = 0xA0 and B at X = 0xA8 (8 pixels overlap).
- Every run also has a control pair in the band 0x78-0x87: sprite 4 over sprite 6.
- All other sprites have empty lists.

| Bit | Scenario | CLXDAT bit graded |
|---|---|---|
| ENSP1 | sprite 1 over sprite 2 | 09 |
| ENSP3 | sprite 3 over sprite 0 | 09 |
| ENSP5 | sprite 5 over sprite 0 | 10 |
| ENSP7 | sprite 7 over sprite 0 | 11 |

- For each bit, two runs: CLXCON with only that ENSP bit set, and CLXCON with the other three ENSP bits set and that one clear. ENBP and MVBP are 0.
- OR runs for ENSP1:
  - (i) sprite 0 alone at X = 0xD0, sprite 1 over sprite 2;
  - (ii) sprite 0 over sprite 2, sprite 1 alone at X = 0xD0.
  Each with ENSP1 = 1 and with ENSP1 = 0.

**Observation.** The result value of CF in F2.

**Expected.**

- The graded bit is 1 when the scenario's ENSP bit is set and 0 when it is clear (while the other three are set).
- OR run (i): bit 9 = ENSP1.
- OR run (ii): bit 9 = 1 for both values of ENSP1.

Page:

- "It also controls the individual inclusion of odd numbered sprites in the collision detection, by logically ORing them with their corresponding even numbered sprite."
- "15 ENSP7 Enable Sprite 7 (ORed with Sprite 6)", "14 ENSP5 Enable Sprite 5 (ORed with Sprite 4)", "13 ENSP3 Enable Sprite 3 (ORed with Sprite 2)", "12 ENSP1 Enable Sprite 1 (ORed with Sprite 0)".

The CLXDAT bit names are from p.19.

**Preconditions.**

- PC-FRAME, PC-SPR for every displayed sprite, the CF window (the first CF read's marker lies inside the frame and comes before every involved sprite's first data read, and the result marker lies inside the frame and after every end-pair read), PC-BEAM.
- Result bit 14 = 1 in every run: the control pair of two even sprites, which are always included, collides, so collision detection is running.
- The union of each overlapping pair is visible as a 24-lores-pixel (48 output pixels) cS span on lines 0x60-0x6F.

**Contrasting case.** The runs with the bit clear are the contrast for the runs with it set. The OR run (i) with ENSP1 = 0 is the contrast for (i) with ENSP1 = 1.

For the preconditions: move sprite 6 to X = 0xD0 (bit 14 = 0); move the result read to line 0x68 (the CF window fails).

**Reference frame.** The CF window, anchored by markers and by the sprites' list reads.

---

### C2. CLXCON bits 11-0: enable and match value for bit planes 1-6

**Claims (p.18):** opus:AA-clxcon.c4, c11, c12, c13, c14, c15, c16, c17, c18, c19, c20, c21, c22; sol:AA-clxcon.c4, c11, c12, c13, c14, c15, c16, c17, c18, c19, c20, c21, c22.

**Proposition.** For each k = 1..6:

- CLXCON bit (5 + k) (ENBPk) includes bit plane k in collision detection;
- CLXCON bit (k - 1) (MVBPk) is the value plane k must have for a collision.

An included plane that does not match its value prevents the collision.

**Stimulus.**

- D-STD, BPU = 8, DMACON = `0x83A0`, CF.
- Sprite 0 on lines 0x60-0x6F at X = 0xA0, DATA = `0xFFFF`, DATB = 0.
- Plane k points to P_k, filled entirely with `0xFFFF` ("data 1"), or to Z ("data 0"). All other planes point to Z.
- The CPU writes CLXCON in each frame's vertical blank (VERTB), then runs CF.

| Frame | Data | ENBPk | MVBPk | Expected group bit |
|---|---|---|---|---|
| F2 | 1 | 1 | 1 | 1 |
| F3 | 1 | 1 | 0 | 0 |
| F4 | 1 | 0 | 0 | 1 |
| F5 | 1 | 0 | 1 | 1 |
| F2 (second run) | 0 | 1 | 0 | 1 |
| F3 (second run) | 0 | 1 | 1 | 0 |

- AND runs: planes 1 and 3 both enabled, both with data 1, MVBP1 = 1:
  - MVBP3 = 0 gives bit 1 = 0;
  - MVBP3 = 1 gives bit 1 = 1.

**Observation.** The CF result of each frame.

**Expected.** The "group bit" is CLXDAT bit 1 (Playfield 1 to Sprite 0) for odd k, and bit 5 (Playfield 2 to Sprite 0) for even k. It takes the values in the table.

Page:

- "This register controls which Bitplanes are included (enabled) in collision detection, and their required state if included."
- Rows "11 ENBP6 Enable Bit Plane 6 (Match reqd. for collision)" .. "06 ENBP1 ...", "05 MVBP6 Match value for Bit Plane 6 collision" .. "00 MVBP1 ..." (checked on `bands\p18_band3.png`).
- p.19: "Note: Disabled Bit Planes cannot prevent collisions. Therefore if all bitplanes are disabled, collisions will be continuous, regardless of the match values." This gives the value 1 when ENBPk = 0.
- p.19: "Playfield 1 is all odd numbered enabled bit planes."

Quantifier: all six planes, every combination of enable and match value, both data polarities.

**Preconditions.**

- PC-FRAME, PC-BPL({1..8}), PC-SPR(0), the CF window. The line on which the re-read of VPOSR/VHPOSR after each CF read lands is a MEASURED line; the window (both markers inside the frame, the first CF read before the sprite's first data read, the result read after its end pair) is what the proposition needs.
- PC-OTHER: the other group's bit (5 for odd k, 1 for even k) is 1 in every frame for odd k, and for even k in every frame in which plane k is disabled or matches its value. That group has all its planes disabled, so its collision is continuous (p.19); this shows the sprite was displayed and detection ran. In a frame in which an enabled even plane does not match, nothing is demanded of the odd group's bit: p.19's note ("Playfield 1 is all odd numbered enabled bit planes. Playfield 2 is all even numbered enabled bit planes") does not say whether the playfield-1 collision bit may fall together with a mismatching playfield-2 plane.

**Contrasting case.** The F2 and F3 rows differ only in MVBPk, and F3 and F4 only in ENBPk. Each is the other's contrast.

For the preconditions: remove sprite 0's data (DATA = 0), so the other-group bit becomes 0.

**Reference frame.** The CF window of each frame.

---

### C3. CLXCON2: enable and match value for bit planes 7 and 8

**Claims (p.18):** opus:AA-clxcon2.c5, c9, c10, c12; sol:AA-clxcon2.c5, c10, c12, c16.

**Proposition.**

- CLXCON2 bit 7 (ENBP8) includes plane 8 in collision detection, and bit 6 (ENBP7) includes plane 7.
- Bit 1 (MVBP8) is plane 8's required value, and bit 0 (MVBP7) is plane 7's.

**Stimulus.**

- As C2, for k = 7 and k = 8. In each frame the CPU first writes CLXCON = `0x0000`, then CLXCON2. The order matters, because a CLXCON write resets CLXCON2 (C4).
- CLXCON2 values: k = 7 uses ENBP7 = `0x0040`, MVBP7 = `0x0001`; k = 8 uses ENBP8 = `0x0080`, MVBP8 = `0x0002`.
- **Address.** The pages disagree: p.8 lists "CLXCON2 p 10E W D", while p.18 prints "CLXCON2 P 10C", the address p.8 and p.16 give to BPLCON4 (section 4). Every C3 run is therefore made twice, each time in its own run: once with CLXCON2 written at `0x10E` (p.8), once at `0x10C` (p.18). Neither address's runs depend on the other's result. A write at `0x10C` also sets BPLCON4 on pages 8 and 16, which changes only colour-table addresses; no collision observable depends on it.

**Observation.** As C2 (group bit 1 for plane 7, bit 5 for plane 8), for the runs at each address.

**Expected.** The C2 table with k = 7 and k = 8, whole: every row, both planes, both data polarities. C3 passes if the runs at one of the two addresses satisfy the whole table, and fails if the runs at neither address do. The record states which address satisfied it (or that both did).

Page (p.18):

- "This register controls when bitplanes 7 & 8 are included in collision detection, and their required state if included."
- "07 ENBP8 Enable Bit Plane 8 (Match reqd. for collision)", "06 ENBP7 Enable Bit Plane 7 (Match reqd. for collision)", "01 MVBP8 Match value for Bit Plane 8 collision".

The row for bit 00 is printed at the top of p.19: "00 MVBP7 Match value for Bit Plane 7 collision" (checked on `bands\p19_band1.png`). Claim opus:AA-clxcon2.c12 is right that p.18 alone prints no row for bit 00.

**Preconditions.** As C2, for the runs at each address. No precondition concerns whether the CLXCON2 write has an effect: that effect is the table under test.

**Contrasting case.** As C2. In addition: leave out the CLXCON2 write at both addresses; the rows that expect 0 then show group bit 1 at both, and C3 fails.

For the preconditions: as C2.

**Reference frame.** The CF window.

---

### C4. A write to CLXCON resets CLXCON2

**Claims:** none. The four claims that state this rule (opus:AA-clxcon.c6; sol:AA-clxcon.c6; opus:AA-clxcon2.c6; sol:AA-clxcon2.c6) say that all bits of CLXCON2 are reset; C4 sees only ENBP7 and ENBP8, so they are declined (section 1). C4 is kept as a check.

**Proposition.** Any write to CLXCON clears the bits of CLXCON2. After it, planes 7 and 8 no longer take part in collision detection.

**Stimulus.**

- D-STD, BPU = 8, sprite 0 as in C2, CF.
- Planes 7 and 8 point to regions filled with `0xFFFF`; planes 1-6 point to Z.
- The CPU writes in the vertical blank of each frame:

| Frame | CPU writes |
|---|---|
| F2 | CLXCON = `0x0000`, then CLXCON2 = `0x00C0` (ENBP8 = ENBP7 = 1, both match values 0) |
| F3 | CLXCON = `0x0000` only |
| F4 | CLXCON2 = `0x00C0` only |
| F5 | CLXCON = `0xF000` only |

ENSP bits concern only odd sprites, none of which are displayed. So `0xF000` changes nothing else.

**Observation.** The CF result bits 1 and 5 in F2-F5.

**Expected.**

- F2: bits 1 and 5 = 0.
- F3: bits 1 and 5 = 1.
- F4: bits 1 and 5 = 0.
- F5: bits 1 and 5 = 1.

Page:

- p.18 CLXCON: "Writing to this register resets the bits in CLXCON2."
- p.18 CLXCON2: "Contents of this register are reset by a write to CLXCON."
- p.4 agrees: "CLXCON2 is reset by a write to CLXCON, so that old game programs will be able to correctly detect collisions."

Only ENBP7 and ENBP8 are observed. MVBP7 and MVBP8 cannot be observed after the reset, because turning ENBP on again rewrites them (section 1).

Quantifier "Writing to this register": two different written values are graded.

**Address.** As in C3, every C4 run is made twice, each time in its own run: with CLXCON2 written at `0x10E` and at `0x10C`. C4 is graded at each address where F2 and F4 give 0, i.e. where CLXCON2 is in effect; that is C3's clause, not C4's. C4 passes if F3 and F5 give 1 at every such address, and fails otherwise. The record states the address or addresses.

**Preconditions.** PC-FRAME, PC-BPL, PC-SPR(0), the CF window, PC-BEAM. At one address at least, F2 and F4 give 0 (CLXCON2 is in effect there). If neither address gives it, C4 is not graded; C3 then fails.

**Contrasting case.** Leave out the CLXCON writes in F3 and F5: bits 1 and 5 stay 0.

For the preconditions: leave out the CLXCON2 write in F2, so the bits are 1 already in F2.

**Reference frame.** The CF windows.

---

### C5. CLXCON2 is initialised by reset

**Claims (p.18):** sol:AA-clxcon2.c11, c13. The claims that all bits of CLXCON2 are initialised by reset are declined (section 1).

**Proposition.** After reset, and with neither CLXCON nor CLXCON2 written, planes 7 and 8 are not included in collision detection (ENBP7 = ENBP8 = 0).

**Stimulus.**

- D-STD without its BPLCON4 write (BPLCON4 keeps the value p.16 marks as initialised), BPU = 8, sprite 0 as in C2, CF. The BPLCON4 write is left out because p.18 puts CLXCON2 at 0x10C, the address where BPLCON4 is written (section 4).
- The program writes neither CLXCON nor CLXCON2: a CLXCON write would reset CLXCON2 through C4 and hide the reset value.
- CLXCON's own content after reset is not stated (it is not among the registers p.2 and p.4 name for RST_). So the plane data make the result independent of it. Every line of every plane repeats one 16-pixel pattern:
  - planes 1, 3, 5 = `0x3333`, `0x0F0F`, `0x00FF`;
  - planes 2, 4, 6 = the same three words;
  - planes 7 and 8 = `0xFFFF` in run A, and `0x0000` in run B.
  Any 16 consecutive pixels then contain all eight value combinations of planes 1, 3, 5 (and of planes 2, 4, 6). Whatever CLXCON's ENBP and MVBP bits for planes 1-6 are, some pixel under sprite 0 matches them.
- Two runs, because the reset values of MVBP7 and MVBP8 are not seen (section 1): an ENBP7 or ENBP8 left at 1 by reset prevents the collision in run A if its match value is 0, and in run B if its match value is 1. Together the two runs show both enables at 0 whatever the match values are.
- The odd sprites have empty lists and are disarmed by the SPRxCTL writes of D-STD.

**Observation.** The CF result in F2 of each run.

**Expected.** Bits 1 and 5 = 1 in run A and in run B. p.19: "Disabled Bit Planes cannot prevent collisions." Page: "***** BITS INITIALIZED BY RESET *****" (p.18). The value is 0: p.4 "RST_ pin resets all bits in all registers new to AA. These registers include: BPLCON3,BPLCON4,CLXCON2,DIWHIGH,FMODE", and p.2 says the same.

**Preconditions.** The run starts from reset. PC-STATIC(no write to `0x098`, `0x10C` or `0x10E`). PC-FRAME, PC-BPL, PC-SPR(0), the CF window.

**Contrasting case.**

- Run A with CLXCON2 = `0x00C0` written (no CLXCON write): bits 1 and 5 = 0, since planes 7 and 8 are then included with the match value 0 that their data never have.
- Run B with CLXCON2 = `0x00C3` written (no CLXCON write): bits 1 and 5 = 0, since planes 7 and 8 are then included with the match value 1 that their data never have.

Each contrast run is made twice, each time in its own run, with CLXCON2 written at `0x10E` and at `0x10C` (as in C3). It shows bits 1 and 5 = 0 at every address where C3 found CLXCON2 in effect.

For the preconditions: add a CLXCON write (the static check fails).

**Reference frame.** The CF window.

---

### LF. Field length of the long frame

**Claims:** none. The sentence graded is p.29's, which no claim of this package states.

**Proposition.** With VARBEAMEN set, VTOTAL = `0x05FF` and LACE clear, a field has VTOTAL + 1 = 1536 lines. Page (p.29, VTOTAL): "It's the line number to reset the counter, so there's this many + 1 in a field. The exception is if the LACE bit is set (BPLCON0), in which case every other field is this many +2 & the short field is this many + 1."

**Stimulus.** The LF configuration of 2.9 (HTOTAL = `0x00E2`, VTOTAL = `0x05FF`, BEAMCON0 = `0x08A0`), D-STD with BPU = 1 (BPLCON0 = `0x1000`, so LACE is clear), the plane pointer to Z, DMACON = `0x8380`. The Copper tag block and the vertical-blank block are those of 2.6.

**Observation.** t_F(1), t_F(2), t_F(3) and the tag spacing P of F1 and F2 (2.11).

**Expected.** For F1 and for F2 the field has N = (t_F(i+1) - t_F(i)) / P lines, N a whole number within P/8 (2.11, line-offset measure), and N = VTOTAL + 1 = 1536.

**Control.** LF-CONTROL: the same stimulus with VTOTAL = `0x04FF`. The page's sentence ties the field length to VTOTAL line for line, so the field is exactly 256 lines shorter than with `0x05FF`: N(`0x05FF`) - N(`0x04FF`) = 256. The control passes on a field length that does not equal VTOTAL + 1; it shows that the measure follows the stimulus.

**Preconditions.** PC-SETUP; PC-FRAME (t_F(1)..t_F(3) exist and are equally spaced, and the spacing is a whole number of lines within P/8, so that the length is a measure, 2.11); PC-TAG.

**Contrasting case.** VTOTAL = `0x04FF`: the expected count is 1280 in place of 1536 (LF-FRAME with the stimulus's own VTOTAL).

For the preconditions: omit COPEN (PC-FRAME fails); remove the tag MOVEs (PC-TAG fails); leave out the setup marker (PC-SETUP fails).

**Reference frame.** t_F and tags.

---

### J1-J5. Joint assertions

Each claim below states something that two or more propositions grade in parts. Its id is carried by one joint assertion, and by nothing else: the part propositions carry no part of these ids. A joint assertion adds no stimulus and no observation. It passes only if every part proposition passes (on the core that runs it), and fails if any part fails or is not graded. Its record names the verdict of each part.

| Id | Claims | What the claim states | Parts, and what each grades |
|---|---|---|---|
| J1 | sol:AA-sprxpos.c4 | SPRxPOS is the "Sprite x Vert-Horiz start position data" (p.28) | S5-SH10-SH3: horizontal start, SPRxPOS bits 7-0 = SH10-SH3. S8: vertical start, SPRxPOS bits 15-8 = SV7-SV0. |
| J2 | opus:AA-sprxpos.c8; sol:AA-sprxpos.c14 | SPRxPOS bits 7-0 are SH10-SH3; the "Low-order 3 bits are in SPRxCTL register below" (p.28) | S5: SH10-SH2. S5-SH1: SH1. S6: SH0. |
| J3 | opus:AA-sprxctl.c17; sol:AA-sprxctl.c31 | "These 2 registers work together as position, size and feature Sprite control registers." (p.28) | Position: S5 (SH10-SH2), S5-SH1 (SH1), S6 (SH0), S8 (SV9-SV0). Size: S9 (EV9-EV0). Feature: the two functions p.28 gives these registers beyond position and size, S10 ("07 ATT Sprite attach control bit") and S12 (SH10 "free to be used by ALICE as an individual scan double enable"). |
| J4 | opus:AA-sprxptl.c6 | "This pair of registers contains the 20 bit address of Sprite x (x=0,1,2,3,4,5,6,7) DMA data." (p.28) | S1: the 20-bit address each pair holds. S14-B: part (b) of S14 ties the list read through pointer x to sprite x (a write to SPRxPOS moves exactly that column). |
| J5 | opus:AA-bplxdat.c10; sol:AA-bplxdat.c26 | "for that word (16/32/64 pixels). The MSB is output first, and" (p.15) | B3: the 16-pixel word (FMODE = 0). B5-32: the 32-pixel word (FMODE = 0x0001). B5-64: the 64-pixel word (FMODE = 0x0003). |

**Contrasting case.** The contrasting case of any part makes that part fail, and with it the joint assertion.

---

## 4. Where the pages disagree or leave something open

1. **CLXCON2's address.** p.18 prints "CLXCON2 P 10C W D Extended Collision Control". p.8 prints "BPLCON4 p 10C W D ..." and "CLXCON2 p 10E W D Extended collision control register". p.16 prints "BPLCON4 P 10C W D". All three are confirmed on the scans. Two pages give 10C to BPLCON4, and one gives it to CLXCON2. C3 grades its whole table at both addresses, each in its own run, with no gate: it passes if either address satisfies the table, fails if neither does, and records which address did. C4 is graded at every address where CLXCON2 is in effect, and C5's contrast runs are made at both addresses. None of them grades the address itself.
2. **Which bit enables plane 8.** p.5: "ENBP7 and ENBP6 are the enable bits for bitplanes 7 and 8,respectively. Similarly, MVBP7 and MVBP8 are their match value bits." (confirmed on the scan). p.18 names plane 8's enable ENBP8 (CLXCON2 bit 07), and ENBP6 is CLXCON bit 11 for plane 6. p.5 gives no bit positions, so the only difference is a name. The tests use p.18's positions.
3. **CLXCON2 bit 00.** p.18's table ends at bit 01. The row "00 MVBP7" is printed at the top of p.19.
4. **The line after CLXCON2's table.** The line `**k**XRESET BY ECSENA*****` is not on the printed page (section 1).
5. **"=0" in SPRxCTL.** The legend "=0/=1 bit values initialized by RST_ pin going low." is printed on p.16, under the BPLCON table. p.28 carries "SH1=0" and "SH0=0" without a legend. p.2 and p.4 list the registers RST_ resets ("include: BPLCON3,BPLCON4,CLXCON2,DIWHIGH,FMODE") without SPRxCTL. That list is not exhaustive: BPLCON0-2 also carry "=0" marks for new bits. S7 reads "SH1=0/SH0=0" as reset values under the p.16 legend.
6. **Scope of the parity note.** p.4's "Sprite vertical start and stop positions must be of the same parity" stands in the scan-doubling paragraph, but its words are general. The package obeys it everywhere. As a result, SV0 and EV0 can only be moved together (S8, S9).
7. **Stop line.** "End (stop) vert. value" (p.28) and "UHRES sprite vertical display stop" do not say whether the stop line itself is displayed or fetched. The start value is not tied to an absolute line either. Only shifts are graded.
8. **What SPRHSTRT/SPRHSTOP govern.** The UHRES sprite's data go to external logic (p.27). The "display start/stop" is read as the lines on which its pointer is active, by analogy with BPLHSTRT/BPLHSTOP (p.15: "This controls the line when the data fetch starts for the BPLHPTH,L pointers.").
9. **UHRES enables.** p.16 says UHRES "also needs bits in DMACON" without naming them. p.22 names only DMAEN "(also UHRES dma)". S15 sets DMAEN, BPLEN and SPREN.
10. **`x` bits.** `x` is not defined on p.28. It is defined elsewhere as don't care, to be written 0 (section 1).
11. **Misprints in the claims' quotations**, all on the print as well: "x=1,2,3,45,6,7,8" (p.15), the unclosed "(x=1-8" (p.15), "indicing" (p.15), "reinitalized" (p.15), "outputed" (p.28). None changes a test.
12. **The MSB sentence** of BPLxDAT continues on p.16: "is therefore always on the left." Claim opus:AA-bplxdat.c11 says p.16 was not in its workspace; it is in the corpus and on the scan.
13. **Attach bit in even sprites.** "(odd sprites only)" (p.28) gives bit 7 no function in even sprites. S10 expects an even sprite with bit 7 set to display as unattached.
14. **Sprite list format and the order of DATA and DATB fetches.** These are not on the AA pages. They are taken from HRM pp.105-127 for the stimulus only; no expected value depends on which word lands in A or B.


---

## 5. Build

This section records what the build adds to the specification above: the programs, the run machinery, the values it measured (labelled as measurements) and the mutations. Nothing in sections 1-4 is changed.

### 5.1 Chip set and cores

Every run sets `chipset_config = 0x10` (AA selected), except the ECS runs of S16, which set `0x08`. The chip set is an input of `nanomig.v`; it is not a property of the build. Both cores therefore run every program and both are graded: every assertion, precondition and anchor prints PASS or FAIL on the fx68k build and on the TG68K build, and a line whose precondition does not hold prints NOT GRADED.

### 5.2 Files

| File | Content |
|---|---|
| `prog_main.s` | the stimulus program: the register writes of a setup table, the setup marker, then an event table (below) |
| `prog_nomarker.s` | `prog_main.s` without the setup-marker store (the mutation of PC-SETUP) |
| `test_aa_integ_s3.cpp` | `main()`, the header with the `// MUTATION` records |
| `s3_run.h`, `s3_std.h` | run description, Copper list and sprite list builders, execution on the harness, frames, tags, output lines |
| `s3_check.h`, `s3_pre.h`, `s3_vid.h`, `s3_spr.h` | the grading scaffolding, the common preconditions PC-SETUP, PC-FRAME, PC-TAG, PC-PIXEL, pixel and column analysis |
| `s3_props_*.h` | the propositions B1-B5, S1-S16, C1-C5, LF and the fixture and joint assertions |
| `s3_withdrawn.h` | the runtime `RECORDED:` line of every claim id declined in section 1, with its reason |

### 5.3 Programs and tables

- The setup table (at `0x088000`) lists register writes; the program performs each with a word write from a data register, in the order of 2.1/2.4/2.6 (BEAMCON0 first, the display registers, the colour palette, COP1LC and COPJMP1, DMACON last), stores `0x5E7F` to `0x0C0000` and then works through the event table (at `0x08A000`). Both tables lie in the program region `0x080400-0x08FFFF` of 2.3.
- An event is a wait for a number of vertical-blank flags (INTREQR bit 5, cleared by a write of `0x0020` to INTREQ), a wait for a beam position (VPOSR and VHPOSR read as one long; the line and H of the specification), a routine of 68000 instructions placed in the table, and a record of the re-read of VPOSR/VHPOSR and a marker word at `0x0C0010 + 0x10 * k`. The routine holds one `MOVE.W` per register write (a data register or an immediate source, so each register is written exactly once and never read before), per register read and per Chip-memory write. The values written most often are held in data registers before the beam wait. The first event of every run waits for one vertical-blank flag: a flag that is already set is consumed there, so that the next event counts the flags of the frames after the setup marker.
- The Copper list is the one of 2.6 (vertical-blank block, tag block, end), with the Copper events of a proposition placed after the tag MOVE of their line.
- The preconditions PC-STATIC are evaluated on the tables the program is given (the setup table and the event routines); the Copper list is not part of them.

### 5.4 Measured values

These are measurements of the final run, not oracles:

- A chip line lasts P = 1816 clk_sys cycles, the video port shows 908 samples per output line, and 70 ns of chip time is one sample (u = 1, PC-PIXEL). The visible part of an output line is the samples 65-818.
- A normal PAL frame (BEAMCON0 = `0x0020`) lasts 313 lines (568,408 cycles) in every run.
- With HTOTAL = `0x00E2`, VTOTAL = `0x05FF` and BEAMCON0 = `0x08A0` the Copper restarts 2,791,192 cycles apart, which is 1537 lines of 1816 cycles; p.29 gives "there's this many + 1 in a field", 1536 lines, and the PC-FRAME line prints the measured length beside it.
- With the SSCAN2 bit of FMODE set a sprite is shown a second time 512 output pixels to the right of its first image (S11); S12 reads the first image.
- SPRxCTL bit 4 (SH1): the k = 1 pair of S5 shows the leftmost cS pixel moved by 0 output pixels on all 128 band lines, where p.28 ("04 SH1=0 start horiz. value, 70nS increment") asks for 1. The pairs k = 2..10 move by exactly 2^(k-1) output pixels.

### 5.5 Readings the build had to fix

- F1 is the first frame whose Copper restart follows the setup marker; the restart that the DMACON write itself causes comes before the marker.
- The columns of S3 and S4 are placed at 48 output pixels (24 lores pixels) from the leftmost cS pixel of the first graded line; a column pattern is compared pixel for pixel inside its 32 pixels.
- PC-SPR(x) accepts reads after the end pair of a list: with the long frame the sprite pointer is read again at line 1024; only the reads up to the end pair are the list.
- C1-C5 read CLXDAT with a type of event whose routine consists of one `MOVE.W CLXDAT(A1),<result>` so that the read lies near H = `0x08`; with eight bit planes and the DMA of the specification the 68000 is slow enough that a longer routine reads one line late.
- J1-J5 take the verdicts of the parts from the same run. J-CONTROL applies the same conjunction to S1-S4, which are graded and green.
- Every claim id declined in section 1 is printed on a `RECORDED:` line of every normal run, with its reason (`s3_withdrawn.h`); no assertion carries one.

### 5.6 Mutations

Each assertion and each precondition has `// MUTATION` records in the header of `test_aa_integ_s3.cpp`: the selector `NMH_MUTATE=<value>` changes the stimulus only (a table, a Copper list, a program variant or a register value) and runs the unchanged comparator; the record states what changed and which lines printed FAIL on each core. A mutation run grades the part of the proposition that its selector names; where that is a sub-population of the normal run (S8 and S9 grade the k = 2 pair, C2 plane 3, C3 plane 7 with data 1, S5 and S5-SH10-SH3 the k = 3 pair), the record says so.

Every red line has a green control through the same comparator, named in the records: S5 for S5-SH1 (the comparator for k = 2..10; the record S5-SH1 runs it green on the k = 1 pair), S6-CONTROL for S6 (the data word shifted by one 35-ns pixel), S7-CONTROL-SH1 and S7-CONTROL-SH0 for the preconditions S7-PC-SH1 and S7-PC-SH0 (SH2 in place of SH1; a data word shifted by one 35-ns pixel in place of SH0), S7-CONTROL and S7-CONTROL-V for the comparators of S7 itself, S15-CONTROL-GROUPS and S15-CONTROL-LOM for S15-PC-GROUPS and S15-PC-LOM and S16-CONTROL-ACCESS for S16-PC-ACCESS (each runs the comparator on a region that bit-plane DMA reaches: plane 1's data in the region of P_1 and plane 5's in that of P_5, two words per line; the LOM control compares two runs whose DIWSTRT differs by two lines), C5-CONTROL for C5 and J-CONTROL for the joint assertions. A record of kind `stimulus impossible` states a stimulus that runs while its line prints NOT GRADED because named preconditions print FAIL, and names the control on which the comparator is live; a record of kind `none impossible` states why no stimulus applies.

### 5.7 Results of the final run

Final run of `make -k -B test`: fx68k 179 passed, 9 failed; TG68K 179 passed, 9 failed.

| Line | fx68k | TG68K |
|---|---|---|
| FIXTURE | PASS | PASS |
| B1 | PASS | PASS |
| B2 | PASS | PASS |
| B3 | PASS | PASS |
| B4 | PASS | PASS |
| S1 | PASS | PASS |
| S2 | PASS | PASS |
| S3 | PASS | PASS |
| S4 | PASS | PASS |
| S5-SH1 | FAIL | FAIL |
| S5 | PASS | PASS |
| S5-SH10-SH3 | PASS | PASS |
| S6 | FAIL | FAIL |
| S6-CONTROL | PASS | PASS |
| S7 | NOT GRADED | NOT GRADED |
| S7-CONTROL | PASS | PASS |
| S7-CONTROL-SH1 | PASS | PASS |
| S7-CONTROL-SH0 | PASS | PASS |
| S7-CONTROL-V | PASS | PASS |
| S8 | PASS | PASS |
| S9 | PASS | PASS |
| S10 | PASS | PASS |
| S11 | PASS | PASS |
| S12 | PASS | PASS |
| S13 | PASS | PASS |
| S14 | PASS | PASS |
| S14-B | PASS | PASS |
| S15-CONTROL-GROUPS | PASS | PASS |
| S15-CONTROL-LOM | PASS | PASS |
| S15 | NOT GRADED | NOT GRADED |
| S16-CONTROL-ACCESS | PASS | PASS |
| S16 | NOT GRADED | NOT GRADED |
| C1 | PASS | PASS |
| C2 | PASS | PASS |
| C3 | PASS | PASS |
| C4 | PASS | PASS |
| C5 | PASS | PASS |
| C5-CONTROL | PASS | PASS |
| J1 | PASS | PASS |
| J2 | FAIL | FAIL |
| J3 | FAIL | FAIL |
| J4 | PASS | PASS |
| J-CONTROL | PASS | PASS |

S5-SH1 and S6 print FAIL on both cores, and J2 and J3 with them; S7, S15 and S16 print NOT GRADED because a precondition prints FAIL (S7-PC-SH1 and S7-PC-SH0; S15-PC-GROUPS and S15-PC-LOM; S16-PC-ACCESS); the preconditions that print FAIL are listed in `out_fx68k.txt` and `out_tg68k.txt`.
