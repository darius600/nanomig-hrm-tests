# DMA priority under load: specification

This specification is written from the 22 claims in `claims-spec-stage.jsonl` and from the printed pages of the *Amiga Hardware Reference Manual*, 3rd edition, that it cites: pp. 19-26, 30, 32-33, 35 (Chapter 2); 58-60, 63-64, 80, 82 (Chapter 3); 105, 107, 109-110, 114, 126 (Chapter 4); 141, 145 (Chapter 5); 169-173, 186, 193-198 with Figures 6-9, 6-10, 6-11 and 6-12 (Chapter 6); 216-219, 221-222 (Chapter 7); 246-249 (Chapter 8); 266-270, 273-278 (Appendix A); 287-293 (Appendix B); 310 (Appendix C). Its scope is one integration package that runs a 68000 program on the whole system and grades the accesses at the Chip-memory port: whether the Copper keeps its cycles against the blitter and the 68000; whether disk, audio, sprite and bitplane DMA keep their slots while a blit with the blitter-nasty bit set runs, and while the Copper or the 68000 is busy; whether the Copper is free of the channels it is said not to collide with; and what a four-plane high-resolution display does to the Copper and the blitter, as far as the pages agree on it.

## Claims not turned into a full proposition

No claim is left out entirely. Three groups are used only in part:

1. **sol:6-system-dma.c73, opus:ch6-sysdma.c43** (p.197): a four-plane high-resolution display "effectively locks out the 68000 (as well as the blitter or Copper) from any memory access during the display". p.275 says the opposite for the Copper: "This prevents collisions with display, audio, disk, refresh, and sprites, all of which use only even cycles." The pages therefore disagree on whether that display uses the cycles the Copper requests. Only what every page agrees on is specified (P6): the Copper and the blitter never take a slot from bitplane DMA, and both still get slots between the lines' data fetches. Whether they get any slot *during* the fetch is measured and reported, not graded.
2. **The Copper and 68000 parts of "the highest priority level"** (sol:6-system-dma.c5, sol:6-system-dma.c6, sol:6-system-dma.c8; opus:ch6-sysdma.c2, opus:ch6-sysdma.c3, opus:ch6-sysdma.c5) and **the disk, audio and sprite part of "the Copper has the next priority"** (sol:6-system-dma.c11; opus:ch6-sysdma.c9): used in P5, but no test can show that an arbitration took place. p.20 and p.275 say the Copper requests only cycles that disk, audio, refresh and sprite DMA do not use; p.196 and Figure 6-10 say the same of the 68000. P5 checks that these channels keep every slot while the Copper or the 68000 is busy. It cannot check that they won a contested slot, because by the pages no slot is contested.
3. **Refresh**, named in the reason given with opus:ch2-about-the-copper.c29 and opus:ch2-appa-copins.c133 ("This prevents collision with audio, disk, refresh, ..."): refresh cannot be switched off, so it cannot be separated from the background. It is present, unchanged, in every run (precondition C7), and P3 covers it only in that sense.

## Propositions at a glance

| | Proposition | Claims | Pages |
|---|---|---|---|
| P1 | While a blitter-nasty blit runs, the blitter never delays a Copper fetch. | sol:COP2-copins-arbitration.c12, opus:ch2-appa-copins.c131, opus:ch2-about-the-copper.c28, opus:ch2-about-the-copper.c17.s2 | 275, 20, 19 |
| P2 | While the 68000 runs a loop of Chip-memory accesses, the 68000 never delays a Copper fetch. | sol:COP2-copins-arbitration.c14, opus:ch2-appa-copins.c132, opus:ch2-about-the-copper.c27, opus:ch2-about-the-copper.c17.s2 | 275, 20, 19 |
| P3 | Disk, audio, sprite DMA and a four-plane low-resolution display never delay a Copper fetch. | opus:ch2-about-the-copper.c29, opus:ch2-appa-copins.c133 (and the "only" of sol:COP2-copins-arbitration.c12, sol:COP2-copins-arbitration.c14, opus:ch2-appa-copins.c131, opus:ch2-appa-copins.c132) | 20, 275, 196 |
| P4 | While a blitter-nasty blit runs, disk, audio, sprite and bitplane DMA keep every one of their slots. | sol:6-blitter-priority.c4, sol:6-blitter-priority.c5; opus:ch6-sysdma.c55; sol:6-system-dma.c5, sol:6-system-dma.c6, sol:6-system-dma.c8; opus:ch6-sysdma.c2, opus:ch6-sysdma.c3, opus:ch6-sysdma.c5 | 198, 194 |
| P5 | While the Copper runs a MOVE stream, and while the 68000 runs a memory loop, disk, audio, sprite and bitplane DMA keep every one of their slots. | sol:6-system-dma.c5, sol:6-system-dma.c6, sol:6-system-dma.c8, sol:6-system-dma.c11; opus:ch6-sysdma.c2, opus:ch6-sysdma.c3, opus:ch6-sysdma.c5, opus:ch6-sysdma.c9 | 194 |
| P6 | In a four-plane high-resolution display, neither the Copper nor the blitter takes a slot from bitplane DMA, and both get slots between the lines' fetches. | sol:6-system-dma.c73, opus:ch6-sysdma.c43 (agreed part), sol:6-system-dma.c11, opus:ch6-sysdma.c9, opus:ch6-sysdma.c55 | 197, 275, 194, 198 |

## 1 Common frame

### 1.1 Machine

The CPU runs as a 68000 (on both cores; neither is switched to 68010 or 68020). PAL, Enhanced Chip Set, 2 MB of Chip memory, unless a contrasting case says otherwise. Every comparison below pairs two runs on the same core and the same machine configuration; every proposition is evaluated separately on each core.

### 1.2 Why the expected values are relations between runs

The pages give priorities, not timings a test may use as absolute values. Figure 6-9 (p.195) draws slot positions but says of itself: "Actual system hardware demands certain specific values for data fetch start and display start. Therefore this timing chart has been 'adjusted' to match those requirements." The pages also disagree on which cycles are called odd and which even (section 3). So no proposition uses a drawn slot number. Each one compares the port trace of a *load run*, in which a competitor requests the bus, with a *reference run* that differs from it in that competitor alone, and expects the subject's accesses to be identical. "Priority over X" is graded as "X's presence changes nothing about the subject's accesses".

### 1.3 Memory regions

All regions are disjoint. The addresses are choices of this specification, not values from the manual; any disjoint placement in Chip memory works, provided the attribution below uses it.

| Region | Bytes | Contents | Used by |
|---|---|---|---|
| V | $00000-$003FF | exception vectors; the level-3 autovector at $6C | 68000 |
| X | $80000 or above | program code, stack, the loop word at A2, result words (the harness loads the program at $080400, with the stack at $0FFF00 and the results at $0C0000) | 68000 |
| K | $10000-$10FFF | Copper lists L0 at $10000, L1 at $10100 | Copper |
| BA | $20000-$20FFF | blitter source A (2048 words) | blitter |
| BD | $28000-$28FFF | blitter destination D (2048 words) | blitter |
| P | $40000-$53FFF | four bitplanes (layouts in 1.4) | bitplane DMA |
| S | $60000-$61FFF | sprite x data structure at $60000 + $400·x, x = 0..7 | sprite DMA |
| AU | $62000-$65FFF | audio channel x data at $62000 + $1000·x, x = 0..3 | audio DMA |
| DK | $66000-$69FFF | disk write buffer (8000 words at $66000) | disk DMA |

Every word of BA, P, S (outside the control and end-of-data words), AU and DK holds a value that differs from its neighbours (for example, the low 16 bits of its own address, exclusive-or a per-region constant), so that a fetch's data identifies the word.

### 1.4 Data the program prepares

- **Bitplanes, low resolution** (layout "lowres"): plane p (p = 1..4) at $40000 + (p-1)·$2800, 40 bytes per line, 256 lines.
- **Bitplanes, high resolution** (layout "hires"): plane p at $40000 + (p-1)·$5000, 80 bytes per line, 256 lines.
- **Sprites** (p.105, p.107): for each x = 0..7 a structure of control word 1 = $3000 + ($30 + 8·x) (VSTART $30, HSTART $60 + 16·x, whose high 8 bits are bits 7-0), control word 2 = $F000 (VSTOP $F0; bits 2, 1, 0 zero), 192 pairs of data words, then the two end-of-data words $0000, $0000. Each sprite thus covers beam lines $30 to $EF.
- **Audio**: 2048 data words per channel.
- **Disk**: 8000 words.
- **Blit source**: 2048 words in BA.
- **Copper lists** (1.6). L1 contains words that differ between runs (1.7). These words are preloaded image words only; the CPU never copies them. **No branch taken by the program before step S5 depends on a variant value**, so that every run executes the same instructions at the same times up to S5; C1 therefore holds up to item 4's first word (precondition C1 depends on it).

### 1.5 The program

All register names and addresses are those of Appendix B (pp.289-293). Writes to DMACON, INTENA, INTREQ and ADKCON use the SET/CLR convention of p.218 and p.249.

- **S1.** Release the boot overlay (p.223: "An additional copy of the system ROM responds starting at memory location $00000000 ... Software must assume that memory is not available. The OVL bit in one of the 8520 Chips disables the overlay."; p.339, OVL): DDRA ($BFE201) <- $03, then PRA ($BFE001) <- $00. Status register $2700. INTENA = $7FFF, INTREQ = $7FFF, DMACON = $7FFF (bit 15 clear: every bit written as 1 is cleared, p.218, p.278).
- **S2.** Prepare the data of 1.4, filling each region completely with its pattern whatever the variant (the variant decides only which part the display uses, through the pointer values). Write the address of the interrupt handler (S5) to the level-3 autovector at $6C (the manual assigns the Copper interrupt COPER to level 3, p.219, p.221), read $6C back and store the value in result word R_VEC. The readback of a word written below $80000 shows that the overlay is released (C6).
- **S3.** Poll VPOSR/VHPOSR (p.216) until the beam is on line $20 (VHPOSR bits 15-8 = $20, VPOSR bit 0 = 0).
- **S4.** Registers, in this order:
  1. Display: DIWSTRT = $2C81, DIWSTOP = $2CC1 (the normal PAL values, p.59); DDFSTRT, DDFSTOP and BPLCON0 from the variant (1.7; the normal values $0038/$00D0 low resolution and $003C/$00D4 high resolution are those of p.60 and p.80; BPLCON0 bit 15 HIRES and bits 14-12 BPU, p.63); BPLCON1 = 0, BPLCON2 = 0; BPL1MOD = BPL2MOD = 0 (p.64); BPL1PT-BPL4PT to the plane addresses of the variant's layout, each written as a long word to BPLxPTH (p.63).
  2. Sprites: SPR0PT-SPR7PT to the eight structures, as long words to SPRxPTH (p.110). All eight are initialized, as p.114 requires.
  3. Audio, channels 0..3, in the order of p.145: AUDxLC = $62000 + $1000·x, AUDxLEN = 2048, AUDxVOL = 32, AUDxPER = 124 (at least 124, p.141). (A contrasting case changes AUD0PER.)
  4. Disk: DSKPT = $66000 as one long word to DSKPTH (p.246); ADKCON = $8100 (set FAST, two microseconds per bit cell, p.249).
  5. Blitter, everything except BLTSIZE (p.200: "Write BLTSIZE last; writing this register starts the blit"): BLTCON0 = $09F0 (USEA, USED, LF = $F0, i.e. D = A; p.267), BLTCON1 = $0000 (area mode, ascending), BLTAFWM = BLTALWM = $FFFF, BLTAMOD = BLTDMOD = 0, BLTAPT = $20000, BLTDPT = $28000 (p.266, p.270).
  6. COPCON = $0002 (CDANG). With this bit set every page agrees that the Copper may write BLTSIZE ($058): p.26 (registers $40-$7E need CDANG), p.274, p.310 (ECS: "if this bit is set, the Copper can access all of the Amiga chip registers"); see section 3.
  7. Copper: COP1LC = L1, COP2LC = L0, then a write to the strobe COPJMP2 (p.25, p.276). This loads L0 now and leaves L1 for the next vertical blank (p.25: "At the start of each vertical blanking interval, COP1LC is automatically used to start the program counter"). p.30 requires exactly this order: "you must initialize one of the Copper location registers (COP1LC or COP2LC) and write to its strobe address before Copper DMA is turned on."
  8. INTENA = $C010 (SET, INTEN, COPER; p.218-219).
  9. DMACON = the variant's value (1.7). It follows the location register and strobe (p.30), the sprite pointers (p.109, step 3), and the audio registers (p.145, step 7); it precedes the DSKLEN writes, as p.247 step 1 requires.
  10. Read DMACONR and store it in result word R_DMA.
  11. Disk start, p.247 steps 2-4: DSKLEN = $4000, then DSKLEN = $DF40 twice (DMAEN, WRITE, length 8000).
- **S5.** Read the CPU variant word and branch:
  - **STOP**: execute STOP #$2000. The Copper's INTREQ write (L1 item 8) raises the level-3 interrupt; the handler writes INTREQ = $0010 (clear COPER) and returns with RTE; execution continues at S6.
  - **LOOP**: keep the status register at $2700, so no interrupt is taken, and run: eight pairs of MOVE.W D0,(A2) and MOVE.W (A2),D1, where A2 is a fixed word in X; then read INTREQR and repeat while bit 4 (COPER) is 0. On exit write INTREQ = $0010 and write a loop-exit marker word in X.
- **S6.** DMACON = $7FFF; DSKLEN = $4000 (p.247 step 5); INTENA = $7FFF; finish as the harness expects. S6 is reached on line $F8 of field F and must disable DMA before the next vertical blank, which starts L1 (and a second blit) again.

### 1.6 Copper lists

L0 (at $10000): `$FFFF,$FFFE` (wait for a position that never occurs, p.23, p.30).

L1 (at $10100; byte offsets from L1 in brackets):

| Item | Words | Purpose |
|---|---|---|
| 1 [+0] | 16 MOVEs to SPR0PTH..SPR7PTL | rewrite the sprite pointers in vertical blank (p.110) |
| 2 [+64] | 8 MOVEs to BPL1PTH..BPL4PTL, values of the variant's layout | rewrite the bitplane pointers (p.27) |
| 3 [+96] | WAIT `$4001,$FF00` | line $40, horizontal position ignored (p.22-23, p.32) |
| 4 [+100] | MOVE to BLTSIZE ($058) or to NO-OP ($1FE), data $0800 | starts the blit: 32 rows of 64 words (p.171, p.269); variant BLIT |
| 5 [+104] | WAIT `$5001,$FF00` or `$E001,$FF00` | start of the stream; variant STREAM |
| 6 [+108] | 256 MOVEs to NO-OP ($1FE), data i = 0..255 | "the stream" |
| 7 [+1132] | WAIT `$F801,$FF00` | after every window |
| 8 [+1136] | MOVE to INTREQ ($09C), data $8010 | Copper interrupt, as in p.33 and p.35 |
| 9 [+1140] | `$FFFF,$FFFE` | end of list |

All WAIT second words have bit 15 (BFD) set, so the blitter-finished flag plays no part (p.22, p.275). The destinations SPRxPT, BPLxPT, INTREQ and NO-OP lie at $80 or above, which every page lets the Copper write.

### 1.7 Variants and runs

| Variant word | Values |
|---|---|
| FIX (DMACON value, display registers, L1 item 2 data) | **lowres**: DMACON $87FF (SET, BLTPRI, DMAEN, BPLEN, COPEN, BLTEN, SPREN, DSKEN, AUD3-0EN), BPLCON0 $4200, DDFSTRT $0038, DDFSTOP $00D0, lowres layout. **none**: DMACON $86C0 (SET, BLTPRI, DMAEN, COPEN, BLTEN), otherwise as lowres. **hires**: DMACON $87C0 (none plus BPLEN), BPLCON0 $C200, DDFSTRT $003C, DDFSTOP $00D4, hires layout. |
| BLIT (L1 item 4 destination) | BLTSIZE or NO-OP |
| STREAM (L1 item 5) | $50 or $E0 |
| CPU (S5) | STOP or LOOP |

BLTPRI is DMACON bit 10, "Blitter priority. Also known as 'blitter-nasty'" (p.222; p.278); p.198 calls the same bit DMAF_BLITHOG.

| Run | FIX | BLIT | STREAM | CPU |
|---|---|---|---|---|
| REF | lowres | NO-OP | $50 | STOP |
| BLT | lowres | BLTSIZE | $50 | STOP |
| CPU | lowres | NO-OP | $50 | LOOP |
| NOCOP | lowres | NO-OP | $E0 | STOP |
| NOFIX | none | NO-OP | $50 | STOP |
| BLT-NOCOP | lowres | BLTSIZE | $E0 | STOP |
| BLT-NOFIX | none | BLTSIZE | $50 | STOP |
| CPU-NOCOP | lowres | NO-OP | $E0 | LOOP |
| HIRES-REF | hires | NO-OP | $E0 | STOP |
| HIRES-LOAD | hires | BLTSIZE | $50 | STOP |

Contrasting runs are named in each proposition as a base run plus one change.

### 1.8 What the test extracts from the port

- **Refresh accesses.** `nanomig.v` has a top-level output `refresh` (current bus cycle is refresh), a system-boundary port. The test samples it once per clk_sys step. The set R is the accesses of NOFIX's field F (below) that fall in a bus cycle in which `refresh` is high (p.194: "4 cycles for memory refresh" per line). Before any attribution, every run's trace has the elements of R (same time, same address) removed. The Figure 6-9 anchor is asserted against that port: in every line of NOFIX's field F, `refresh` is high in four bus cycles, on every other memory cycle (the refresh slots of Figure 6-9), and the refresh accesses of R are the accesses in those cycles.
- **Attribution.** An access belongs to the channel whose region holds its word address: K = Copper; BA, BD = blitter; P = bitplane DMA, and within P, plane p by its plane range; S = sprite x by its structure; AU = audio channel x; DK = disk; V and X = 68000. If the harness tags port accesses by channel, the test uses the tag and asserts that it agrees with the region.
- **Trace.** For a channel set G, a run r and a time interval W, T_G(r, W) is the time-ordered list of (time, word address, data, read or write) of r's accesses attributed to G with time in W. Times are in system clock cycles from reset; the two runs of a comparison are both started from reset and are aligned by C1.
- **Field F**: from the first fetch of L1's first word (the Copper's restart at the vertical blank that follows S4, p.25) to the fetch of the second word of item 8. Written W_F(r).
- **Stream span** W_stream(r): from the first to the last fetch of an item-6 word in field F.
- **Blit span** W_blit(r): from the first to the last access to BA or BD in field F.
- **Pre-stream interval** S_pre(r): from the fetch of item 4's second word to the first item-6 fetch, in field F.
- **Display lines.** Display line k (k = 0..255) is the line the beam draws at $2C + k (VSTART $2C, p.58-59). Its fetches are the bitplane fetches whose address lies within line k's words of any of the four planes (base_p + w·k to base_p + w·k + w - 2, w = 40 for lowres and 80 for hires, plus k times the plane's modulo when a contrasting case sets one). Its fetch interval I_k runs from the earliest to the latest of them; the gap g_k is the open interval between the end of I_k and the start of I_(k+1). I_k is tied to beam line $2C+k: the beam position (VPOSR/VHPOSR, p.216) read by the program at the start of field F and the line count of `refresh` cycles (four per line) place the first fetch of line k on beam line $2C+k, and the test asserts it.
- **Fixed channels**: planes 1-4, sprites 0-7, audio 0-3, disk; 17 channels in all.
- **"At least one fetch" of a fixed channel in an interval** counts only if the channel has at least two fetches there at consecutive word addresses of its data, so that a stray access cannot satisfy it.
- **Interval borrowing.** When a precondition counts accesses of two runs "over the same interval", the interval is taken from the run named first and applied to both as absolute times. The two runs must then satisfy C1 (up to their first difference by construction) and C7.
- **Display-line frame check.** Wherever display lines are used, the test first checks the frame on the run it takes them from: for each plane, each of the line's words (20 in low resolution, 40 in high resolution; p.60, "DDFSTRT = DDFSTOP - (8 * (word count - 1)) for low resolution", "DDFSTRT = DDFSTOP - (4 * (word count - 2)) for high resolution") is fetched, all of them inside I_k, and I_k ends before I_(k+1) starts. The frame check also asserts that I_k lies on beam line $2C+k, counted from the start of field F in lines of four `refresh` cycles. If the check fails, the lines are undefined and the proposition reports that instead of a result.

### 1.9 Common preconditions

Each proposition lists which of these it asserts. Each has its contrasting case.

- **C1 Alignment.** The two runs of the comparison have identical port traces (time, address, data, direction) from reset up to the first access at which they differ by construction; the proposition names that access. In a contrasting run that changes a value the CPU writes (AUD0PER, BPL1MOD, DDFSTRT, BPLCON0, the DMACON value), that access is the CPU's read of the changed value. This holds because the program executes the same instructions up to S5 (1.4).
  *Contrast (C1)*: the load run with one extra NOP before S3; the CPU's accesses from there on come one bus cycle later than in the reference run, the traces differ before the named access, and the program still finishes. The NTSC run is the contrast of C7 only.
- **C2 DMA settings took.** R_DMA bits 10-0 equal bits 10-0 of the DMACON value the run wrote (p.278: DMACONR reads the control bits; p.222). In particular, bit 10 (BLTPRI) reads 1 in every run that blits.
  *Contrast*: the BLT run in which the program writes $83FF while the test still declares $87FF; R_DMA bits 10-0 then differ from the declared value, and the clause "bit 10 reads 1 in every run that blits" fails. The program finishes.
- **C3 CPU quiet.** In a STOP run, no access to V or X lies within field F.
  *Contrast*: the same run with CPU = LOOP; loop accesses to X fill field F.
- **C4 Stream complete.** Each of the 512 item-6 words is fetched in field F, with the data placed there, in ascending address order.
  *Contrast*: the run with the last stream MOVE replaced by the WAIT `$F801,$FF00`. The fetched data at those two words differ from the MOVE's; item 8 still follows, so the program finishes.
- **C5 Setup over.** The CPU's read of the CPU variant word (S5) precedes the start of field F.
  *Contrast*: insert, between S4.10 and S4.11 (Copper DMA already on), a poll that waits for two vertical blanks (VPOSR/VHPOSR wrapping to line 0 twice, p.216). Field F then starts while the CPU is still polling, and the disk start and the S5 read fall after it. The program writes INTREQ <- $0010 before STOP, so that COPER, left pending by field F's item 8 (INTENA COPER is set at S4.8), does not end the STOP at once; the interrupt then comes from the next field's item 8. The program still finishes.
- **C6 Vector in place.** R_VEC equals the handler address, read back from $6C after the write of S2: the word written below $80000 reads back, so the overlay is released (S1). Asserted for every STOP run.
  *Contrast*, in a LOOP run, which does not need the vector: write the handler address to $6E instead of $6C; R_VEC then differs. The program finishes.
- **C7 Refresh unchanged.** Every element of R appears, with the same time and address, in the run.
  *Contrast*: the load run with NTSC selected. Lines then alternate between 227 and 228 colour clocks instead of PAL's 227 (p.23), so the refresh accesses move and the end of the S3 poll moves. The program still finishes; NTSC has lines up to 262 (p.24), so every WAIT in L1 still occurs.

## 2 Propositions

### P1 The Copper's priority over the blitter

1. **Proposition.** While a blit with the blitter-nasty bit set runs, the blitter never delays the Copper: every Copper fetch in the blit's span happens at the same time, from the same address, with the same data, as in a run without the blit. Claims sol:COP2-copins-arbitration.c12 and opus:ch2-appa-copins.c131 (p.275), opus:ch2-about-the-copper.c28 (p.20), opus:ch2-about-the-copper.c17.s2 (p.19, blitter half).
2. **Stimulus.** Load run BLT, reference run REF (1.7); they differ only in the destination of L1 item 4. In BLT the Copper writes BLTSIZE after the CPU wrote every other blitter register (p.200), which starts an A-to-D copy of 32 rows of 64 words (p.169, p.171) with DMACON bit 10 set (p.198, p.222). p.35 describes the Copper starting a blit; CDANG is set (S4.6). The demand precondition also uses BLT-NOCOP.
3. **Observation.** Chip-memory port, Copper fetches (region K). Interval: W_blit(BLT). The page's condition is "while a blit runs"; the test knows the blit runs from the blitter's own accesses (first to last access to BA or BD) and asserts that the whole stream lies inside that span (P1-a).
4. **Expected relation.** T_K(BLT, W_blit(BLT)) = T_K(REF, W_blit(BLT)), element by element: time, address, direction, and data for every element except the first word of item 4, whose data differ by construction ($0058 against $01FE). From p.275: "It therefore needs (and has) priority over only the blitter and microprocessor." From p.19: "When the WAIT condition has been satisfied, the Copper steals memory cycles from either the blitter or the 680x0 to move the specified data into the selected special-purpose register." From p.194: "The Copper has the next priority ... The lowest priorities are assigned to the blitter and the 68000, in that order." The relation covers every Copper fetch in the span; one fetch out of place fails the check. The check does not depend on how many cycles a MOVE takes, which is graded elsewhere.
5. **Preconditions.**
   - C1 for BLT against REF, up to the fetch of item 4's first word. C2 (BLT, REF). C3, C6 (BLT, REF). C4 (BLT, REF). C7 (BLT, REF).
   - **P1-a, blit covers the stream:** in BLT, the first access to BA or BD precedes the first item-6 fetch, and the last follows the last item-6 fetch.
   - **P1-b, the blitter demanded the Copper's slots:** in BLT-NOCOP, over W_stream(REF), at least one BA or BD access lies at the time of an item-6 fetch of REF. In BLT-NOCOP the Copper waits there and "does not use the memory bus at all" (p.19), and "the blitter will keep the bus for every available Chip memory cycle" (p.198); the precondition is measured in the unopposed run, at the subject's slot times, so it does not depend on the arbitration under test.
6. **Contrasting cases.**
   - *Check*: BLT with item 5 = `$5101,$FF00`. By p.22 the Copper waits until the beam reaches line $51, so the stream's fetches come at least one line later than in REF. The red is read off the stream fetch times: the data comparison exempts item 5's first word, which differs by construction. The blit (about 4096 accesses) still covers the stream, the stream is complete, and the stream still overlaps W_stream(REF), so P1-a and P1-b hold.
   - *P1-a*: BLT with item 4 data $0100 (4 rows, p.171). The blit of 512 accesses ends long before line $50, so P1-a fails; everything else is unchanged.
   - *P1-b*: BLT-NOCOP with BLIT = NO-OP (the unopposed run has no blit). No BA or BD access lies at the time of any item-6 fetch of REF, and P1-b fails.
   - C1-C7: section 1.9.
7. **Reference frame.** Absolute time from reset, shared by both runs; anchored by C1 (identical traces up to item 4) and C7 (refresh unchanged). Field F and the spans are taken from the Copper's and the blitter's own accesses, and so from the addresses the program assigned, not from slot numbers.

### P2 The Copper's priority over the 68000

1. **Proposition.** While the 68000 runs a loop of Chip-memory accesses, it never delays the Copper: every Copper fetch of field F happens at the same time, address and data as when the 68000 is stopped. Claims sol:COP2-copins-arbitration.c14 and opus:ch2-appa-copins.c132 (p.275), opus:ch2-about-the-copper.c27 (p.20), opus:ch2-about-the-copper.c17.s2 (p.19, 680x0 half).
2. **Stimulus.** Load run CPU, reference run REF; they differ only in the CPU variant (S5 LOOP against STOP). The demand precondition also uses CPU-NOCOP.
3. **Observation.** Copper fetches, interval W_F(CPU). The loop runs from S5 until the Copper's item 8; the test knows this from the loop's own accesses (P2-a).
4. **Expected relation.** T_K(CPU, W_F(CPU)) = T_K(REF, W_F(CPU)), every element including data. Sentences: p.275 "It therefore needs (and has) priority over only the blitter and microprocessor."; p.20 "The Copper, therefore, needs priority over only the 680x0 and the blitter"; p.19 as in P1. Every fetch of the field is graded, the vertical-blank pointer MOVEs, the WAITs and the stream alike.
5. **Preconditions.**
   - C1 for CPU against REF, up to the CPU's read of the CPU variant word. C2, C4, C7 (CPU, REF). C3, C6 (REF). C5 (CPU).
   - **P2-a, 68000 busy:** in CPU, at least one loop access (to the loop's code words or to the word at A2) lies between the start of field F and the first bitplane fetch of the field; at least one in every display line (from the start of I_k to the start of I_(k+1)) of field F that does not overlap W_stream(CPU); at least one in the display line just before W_stream(CPU) and in the one just after; and the loop-exit marker is written after the end of field F.
   - **P2-b, the 68000 demanded the Copper's slots:** in CPU-NOCOP, over W_stream(REF), at least one loop access lies at the time of an item-6 fetch of REF (in CPU-NOCOP the Copper waits there, p.19: the Copper "steals memory cycles from either the blitter or the 680x0").
6. **Contrasting cases.**
   - *Check*: CPU with item 5 = `$5101,$FF00`; the stream comes a line late (p.22) and the check fails through the fetch times (item 5's first word is exempt from the data comparison); P2-a and P2-b still hold.
   - *P2-a*: CPU with the CPU variant set to STOP (that is, REF itself): no loop accesses, so P2-a fails.
   - *P2-b*: CPU-NOCOP with CPU = STOP; no loop access lies at the time of any item-6 fetch of REF, and P2-b fails.
   - C1-C7: section 1.9.
7. **Reference frame.** As P1; display lines (1.8) serve only to state P2-a.

### P3 No collision between the Copper and the other DMA channels

1. **Proposition.** Disk, audio and sprite DMA and a four-plane low-resolution display never delay the Copper: its fetches in field F are identical whether these channels run or not. Claims opus:ch2-about-the-copper.c29 (p.20) and opus:ch2-appa-copins.c133 (p.275); it is also the content of "only" in sol:COP2-copins-arbitration.c12, sol:COP2-copins-arbitration.c14, opus:ch2-appa-copins.c131 and opus:ch2-appa-copins.c132 (p.275).
2. **Stimulus.** Load run REF (FIX = lowres: four low-resolution bitplanes, eight sprites, four audio channels, disk write DMA), reference run NOFIX (FIX = none). The display has four planes because the pages agree only for four or fewer low-resolution planes (item 4).
3. **Observation.** Copper fetches, interval W_F(REF).
4. **Expected relation.** T_K(REF, W_F(REF)) = T_K(NOFIX, W_F(REF)), every element including data. From p.20: "The Copper is a two-cycle processor that requests the bus only during odd-numbered memory cycles. This prevents collision with audio, disk, refresh, sprites, and most low resolution display DMA access, all of which use only the even-numbered memory cycles. The Copper, therefore, needs priority over only the 680x0 and the blitter". From p.275: "This prevents collisions with display, audio, disk, refresh, and sprites, all of which use only even cycles. It therefore needs (and has) priority over only the blitter and microprocessor." For the display, p.20 says only "most low resolution display DMA access"; p.196 makes it precise for this case: "If the display contains four or fewer low resolution bitplanes, the 68000 can be granted alternate memory cycles", and Figure 6-9 marks those alternate slots as "Slots available for Blitter, Copper and 68000". Larger displays are outside this proposition (section 3). Quantifier "only ... nothing other than": every Copper fetch of the field, against all 17 fixed channels at once.
5. **Preconditions.**
   - C1 for REF against NOFIX, up to the CPU's read of the DMACON value (S4.9). C2, C3, C4, C6, C7 (REF, NOFIX).
   - **P3-a, the channels ran while the Copper fetched:** in REF, within W_stream(REF), each of the 17 fixed channels has at least one fetch (1.8), and in every display line overlapping W_stream(REF) at least one bitplane fetch lies between two consecutive item-6 fetches.
   - **P3-b, the reference is quiet:** in NOFIX, no access is attributed to P, S, AU or DK within field F.
   - No contention precondition is asserted: the claim is that there is no contention. A green result says the Copper never had to wait for these channels, which is the claim.
6. **Contrasting cases.**
   - *Check*: REF with item 5 = `$5101,$FF00`, against NOFIX; by p.22 the stream starts a line later and the check fails through the fetch times (item 5's first word is exempt from the data comparison). P3-a and P3-b still hold.
   - *P3-a*: REF with FIX = none; no fixed fetches, so P3-a fails. (This is NOFIX against itself; the other preconditions hold.)
   - *P3-b*: NOFIX with FIX = lowres; fixed fetches appear in field F.
   - C1-C7: section 1.9.
7. **Reference frame.** As P1.

### P4 Disk, audio, sprite and bitplane DMA during a blitter-nasty blit

1. **Proposition.** While a blit with the blitter-nasty bit set runs, disk, audio, sprite and bitplane DMA keep every one of their slots: each of their fetches in the blit's span happens at the same time, address and data as in a run without the blit. Claims sol:6-blitter-priority.c4, sol:6-blitter-priority.c5 and opus:ch6-sysdma.c55 (p.198); sol:6-system-dma.c5, sol:6-system-dma.c6, sol:6-system-dma.c8 and opus:ch6-sysdma.c2, opus:ch6-sysdma.c3, opus:ch6-sysdma.c5 (p.194), for the blitter's part of "highest priority level".
2. **Stimulus.** Load run BLT, reference run REF (as P1). The display starts its data fetch at $38, the normal value, so it takes no sprite slot (p.80: "using a data-fetch start earlier than ($38) will disable some sprites"; Figure 6-9). The demand precondition also uses BLT-NOFIX.
3. **Observation.** For each of the 17 fixed channels c: T_c over W_blit(BLT). The interval is the one the page names, "while the blit runs", known from the blitter's accesses.
4. **Expected relation.** T_c(BLT, W_blit(BLT)) = T_c(REF, W_blit(BLT)) for every channel c, every element including data. From p.198: "Display, disk, and audio DMA take precedence over the blitter, so it cannot block them from bus access." From p.194: "Disk DMA, audio DMA, display DMA, and sprite DMA all have the highest priority level. ... These devices are given first priority because missed DMA cycles can cause lost data, noise in the sound output, or on-screen interruptions." Quantifier "cannot block": every fetch of every channel in the span. A fetch that moves by one slot, or goes missing, fails the check. Sprites are graded under the qualification p.194 gives at once ("Display DMA has priority over sprite DMA under certain circumstances") and p.197 restates ("larger displays may block out one or more of the highest-numbered sprites"): only with the normal data-fetch start, which this stimulus uses.
5. **Preconditions.**
   - C1 for BLT against REF, up to the fetch of item 4's first word. C2 (bit 10 set in BLT), C3, C4, C6, C7 (BLT, REF).
   - **P4-a, the channels ran during the blit:** in REF, within W_blit(BLT), each of the 17 channels has at least one fetch; in every display line inside W_blit(BLT) each plane 1-4 and each sprite 0-7 has fetches (sprite 7 among them, which shows that the display left the sprite slots alone).
   - **P4-b, the blitter demanded their slots:** in BLT-NOFIX, over S_pre(BLT) (lines $40 to $4F, the blit running and the Copper waiting), at least one BA or BD access lies at the time of a fixed-channel fetch of REF (p.198: "the blitter will keep the bus for every available Chip memory cycle").
6. **Contrasting cases.**
   - *Check*: BLT with AUD0PER = 200. The period register sets when the channel retrieves its next data (p.140: "each time the register counts down to 0, another sample is retrieved"), so audio channel 0's fetches lie elsewhere than in REF and the check fails for that channel. This contrast shows the comparator's sensitivity only: no page predicts that a lower-priority requester delays these channels. All preconditions still hold.
   - *P4-a*: BLT and REF with DDFSTRT = $0030; p.82: "Incidentally, this will disable sprite 7." Sprite 7 has no data fetches and P4-a fails; the program finishes.
   - *P4-b*: BLT-NOFIX with BLIT = NO-OP; no BA or BD access lies at the time of any fixed-channel fetch of REF, and P4-b fails.
   - C1-C7: section 1.9.
7. **Reference frame.** As P1. Display lines (1.8) serve P4-a.

### P5 Disk, audio, sprite and bitplane DMA against the Copper and the 68000

1. **Proposition.** While the Copper runs a stream of MOVEs, and while the 68000 runs a loop of Chip-memory accesses, disk, audio, sprite and bitplane DMA keep every one of their slots. Claims sol:6-system-dma.c5, sol:6-system-dma.c6, sol:6-system-dma.c8 and opus:ch6-sysdma.c2, opus:ch6-sysdma.c3, opus:ch6-sysdma.c5 (p.194), for the Copper's and the 68000's part of "highest priority level"; sol:6-system-dma.c11 and opus:ch6-sysdma.c9 (p.194), "The Copper has the next priority".
2. **Stimulus.** (a) Copper: load run REF, reference NOCOP (they differ only in item 5). (b) 68000: load run CPU, reference REF.
3. **Observation.** For each fixed channel c: (a) T_c over W_stream(REF), the interval in which the Copper is busy; (b) T_c over W_F(CPU), the interval in which the loop runs, for the channels disk, audio 0-3 and sprites 0-7. Planes 1-4 are measured, not graded: none of the 22 claims carries display DMA's priority over the 68000.
4. **Expected relation.** (a) T_c(REF, W_stream(REF)) = T_c(NOCOP, W_stream(REF)); (b) T_c(CPU, W_F(CPU)) = T_c(REF, W_F(CPU)); for every graded channel, every element including data. The planes' counts and equality are printed as measurements. From p.194: "Disk DMA, audio DMA, display DMA, and sprite DMA all have the highest priority level." "The Copper has the next priority ..." "The lowest priorities are assigned to the blitter and the 68000, in that order." With P4 this grades "highest" against all three lower-priority requesters the page names.
5. **Preconditions.**
   - (a) C1 for REF against NOCOP, up to the fetch of item 5's first word; C2, C3, C6, C7 (REF, NOCOP); C4 (REF). (b) C1 for CPU against REF, up to the CPU's read of the CPU variant word; C2, C7 (CPU, REF); C3, C6 (REF); C5 (CPU); P2-a.
   - **P5-a, the channels ran:** in the load run, each of the 17 channels has at least one fetch in the interval of item 3.
   - **Limitation (no contention precondition).** p.20 and p.275 say the Copper requests only the cycles these channels do not use, and p.196 ("The 68000 uses only the even-numbered memory access cycles") and Figure 6-10 ("odd cycle, assigned to other devices") say the same of the 68000. By the pages no slot is contested, so no precondition can show an arbitration. A green result shows that the Copper and the 68000 never take one of these channels' slots, not that a contested slot was won.
6. **Contrasting cases.**
   - *Check*: (a) REF with AUD0PER = 200, against NOCOP; (b) CPU with AUD0PER = 200, against REF. Audio channel 0's fetches move (p.140: "each time the register counts down to 0, another sample is retrieved") and the check fails. This contrast shows the comparator's sensitivity only: no page predicts that a lower-priority requester delays these channels. The preconditions hold.
   - *P5-a*: the load run with FIX = none; no fixed fetches.
   - C1-C7 and P2-a: as cited.
7. **Reference frame.** As P1.

### P6 A four-plane high-resolution display against the Copper and the blitter

1. **Proposition.** In a four-plane high-resolution display, neither the Copper nor a blitter-nasty blit ever takes a slot from bitplane DMA, and both still get slots between the lines' data fetches. Claims sol:6-system-dma.c73 and opus:ch6-sysdma.c43 (p.197), as far as every page agrees with them; sol:6-system-dma.c11 and opus:ch6-sysdma.c9 (p.194, "The Copper has the next priority"), and opus:ch6-sysdma.c55 (p.198, "Display ... DMA take[s] precedence over the blitter"), for the display.
2. **Stimulus.** Load run HIRES-LOAD (four high-resolution planes, BPLCON0 $C200, DDFSTRT $003C, DDFSTOP $00D4 (p.60), no sprite, audio or disk DMA; the stream from line $50 and the blit from line $40). Reference HIRES-REF (the same display; stream at $E0, no blit). The contention precondition also uses BLT-NOFIX.
3. **Observation.** (a) Bitplane fetches, T_P over W_H = from the earlier of the first item-6 fetch and the first blitter access to the later of the last item-6 fetch and the last blitter access, in HIRES-LOAD. "During the display" is, per display line k, the fetch interval I_k (1.8), found from the line's own bitplane addresses in HIRES-REF. (b) Every gap g_k of HIRES-LOAD inside the span in which both requesters are pending (see Expected relation).
4. **Expected relation.**
   - (a) T_P(HIRES-LOAD, W_H) = T_P(HIRES-REF, W_H), every element including data. Every page agrees on this ordering: p.194 places display DMA among "the highest priority level" and the Copper next, below it the blitter; p.275 says the Copper "needs (and has) priority over only the blitter and microprocessor"; p.198 "Display, disk, and audio DMA take precedence over the blitter, so it cannot block them from bus access"; p.197 "This effectively locks out ... the blitter or Copper"; p.186 speaks of a blit "locked out of memory access because of, for instance, display fetches".
   - (b) In HIRES-LOAD, every gap g_k that lies inside the span in which both requesters are pending (from the later of the first item-6 fetch and the first blitter access to the earlier of the last item-6 fetch and the last blitter access) holds at least one item-6 fetch and at least one access to BA or BD. Every page agrees with this: p.197 confines the lock-out to the display, "except during horizontal and vertical blanking", and p.275 says the display never collides with the Copper at all. Each gap contains the horizontal blanking range $0F to $35 (p.23), since the data fetch starts at $3C. The number of gaps graded is printed. Figure 6-9 shows free slots ("Slots available for Blitter, Copper and 68000") outside the data fetch, and in these runs no disk, audio or sprite DMA takes them.
   - **Reported, not graded:** the number of item-6 fetches and of blitter accesses in HIRES-LOAD that lie strictly inside some I_k within W_H; and, per display line of HIRES-REF within W_H, the number of bitplane fetches in I_k and the duration of I_k. p.197 says bitplane DMA "needs all of the available memory time slots during the display time just to fetch the 40 data words for each line of the four bitplanes (40 * 4 = 160 time slots)", which would make the first number zero; p.275 says the display uses only the cycles the Copper does not request, which would leave the Copper's fetches unhindered. The pages disagree here (section 3), so no expected value is given for it.
5. **Preconditions.**
   - C1 for HIRES-LOAD against HIRES-REF, up to the fetch of item 4's first word. C2 (HIRES-LOAD, HIRES-REF), C3, C6, C7 (both), C4 (HIRES-LOAD).
   - **P6-a, the display ran and the reference shows it alone:** in HIRES-REF, every display line within W_H has fetches from each of the four planes at that line's addresses, and no access within W_H is attributed to item 6, BA or BD.
   - **P6-b, Copper and blitter pending across gaps:** in HIRES-LOAD, the span in which both are pending contains at least three gaps g_k (and the test prints their number); the first item-6 fetch and the first blitter access precede the span's first gap, and the last of each follows its last gap. (256 MOVEs of four memory cycle times each, p.20 and p.275, need about four and a half lines of 227 cycles even if nothing delays them, so this holds whether or not the display hinders the Copper.)
   - **P6-c, the blitter demanded the display's slots:** in BLT-NOFIX, over S_pre(HIRES-LOAD), at least one BA or BD access lies at the time of a bitplane fetch of HIRES-REF.
   - No precondition asserts that the Copper wanted a slot the display used: whether the display uses the Copper's cycles at all is what p.197 and p.275 disagree on.
6. **Contrasting cases.**
   - *Check (a)*: HIRES-LOAD with BPL1MOD = 2. BPL1MOD is the odd planes' modulo (p.291), added to their pointers at the end of each line (p.60), so from the second line on the odd planes fetch other addresses than in HIRES-REF, and the check fails. The preconditions, evaluated on HIRES-REF and on the gaps (whose lines the test finds with the modulo the run set), still hold.
   - *Check (b)*: HIRES-LOAD with item 5 = `$E001,$FF00` and item 4 data $0040 (one row). While waiting, the Copper "does not use the memory bus at all" (p.19), and a one-row blit is over long before line $50, so no gap holds an item-6 fetch or a blitter access and the check fails. P6-b fails with it, necessarily: nothing on these pages keeps a pending Copper or a blitter-nasty blit out of the free slots of the gaps, so (b) can only fail when nothing is pending.
   - *P6-a*: HIRES-REF with BPLCON0 = $8200 (no planes, p.63); no bitplane fetches.
   - *P6-b*: as the second contrast for the check.
   - *P6-c*: BLT-NOFIX with BLIT = NO-OP; no BA or BD access lies at the time of any bitplane fetch of HIRES-REF, and P6-c fails. P6-a, evaluated on HIRES-REF, still holds.
   - C1-C7: section 1.9.
7. **Reference frame.** Absolute time as in P1. The display-line frame: line k is the line drawn at $2C + k (VSTART $2C, p.58-59), and its words are those the plane pointers and modulos of p.60 and p.63 put on that line. The test checks the frame (1.8) on HIRES-REF, and on HIRES-LOAD for the gaps of (b); if it fails, the gaps are undefined and P6 reports that, not a result.

## 3 Disagreements and open points on the pages

1. **Which displays collide with the Copper.** p.275: "This prevents collisions with display, audio, disk, refresh, and sprites, all of which use only even cycles." p.20: "This prevents collision with audio, disk, refresh, sprites, and most low resolution display DMA access". p.197: four high-resolution planes need "all of the available memory time slots during the display time", which "effectively locks out the 68000 (as well as the blitter or Copper)"; Figure 6-12 draws all eight slots of the timing cycle taken. p.196 and Figure 6-11: more than four low-resolution planes "steal cycles from the 68000", and planes 5 and 6 occupy two of the alternate slots. All of them agree only for four or fewer low-resolution planes (P3). For the four-plane high-resolution display only the ordering is agreed (P6); the lock-out is not graded.
2. **Odd and even cycles.** p.20 and p.275 put the Copper on the odd memory cycles and refresh, disk, audio, sprites (and display) on the even ones. p.196 puts the 68000 on the even cycles, and Figure 6-10 assigns the odd cycle "to other devices". Figure 6-9, with its tick marks read as the start of each colour clock, draws refresh, disk, audio and sprite slots, and low-resolution planes 1-4, at odd positions ($01, $03, ...), and leaves the even ones as "Slots available for Blitter, Copper and 68000". The names cannot all hold under one numbering. The pages agree only that the Copper and the 68000 use the cycles refresh, disk, audio and sprite DMA leave free, and nothing here depends on which are called odd.
3. **Which registers the Copper may write.** p.21: any register at $20 or above; $10 to $20 only with the danger bit; none below $10. p.26 and the markers of Appendix B (p.287, p.289): $80 to $FF always; $40 to $7E only with CDANG; $00 to $3E never. p.310 (ECS): with CDANG set, all registers; with it clear, "the address range from $DFF03E through $DFF07E". This specification sets CDANG, so that every page lets the Copper write BLTSIZE, and has it write nothing else below $80.
4. **The blitter-nasty bit.** The printed p.198 names it "DMAF_BLITHOG" and gives no bit number; p.222 gives bit 10 of DMACON as BLTPRI: "Also known as 'blitter-nasty.'" (p.278 words it differently). The identification rests on the shared name. p.198 also says that with the bit set "the blitter will keep the bus for every available Chip memory cycle"; this package does not grade that. It uses it only to explain why the contention preconditions P1-b, P4-b and P6-c hold, and they test only that the blitter lost some slots, not all.
5. **Values the pages leave open.** How many cycles after a WAIT the Copper's first fetch comes; how many disk and audio fetches a line carries (that depends on the bit-cell time, p.249, and the audio period, p.141, not on these pages); and the absolute slot of any channel (Figure 6-9 is "adjusted"). No proposition gives an expected value for any of these; each compares two runs.
6. **Figure 6-9 and the start of the line.** The figure's footnote "This cycle 0 appears to exclude one of the memory refresh cycles. This is not the case" leaves the fourth refresh slot's position unstated; this package does not need it, because refresh is identified from the `refresh` port (1.8).
