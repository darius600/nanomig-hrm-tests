# Specification: the RESET instruction and AUTOCONFIG base-address assignment

This specification was written from the package's claims (`claims-spec-stage.jsonl`) and the pages of the *Amiga Hardware Reference Manual*, 3rd edition, by an author who never saw the implementation or any test. Page numbers are printed page numbers. Quotations are from the printed pages. Pp. 46, 48, 123, 157, 223, 244, 263, 307, 339, 340, 351, 354, 370, 380, 386, 394, 432, 433 and 435 were checked against the page image, because their text copy is re-edited or damaged or because the quotation carries weight. Where the specification relies on 680x0 behaviour that the manual does not describe, it says so ("Motorola, not the manual").

## 1. Claims not turned into a proposition, or used only in part

| Claim | What is not used | Reason |
|---|---|---|
| opus:K-0164, sol:K-0155 (p.394) | The bus-line levels themselves: that /RST and /BUSRST are driven and /HLT is not. | No expansion-bus line is an output the test can see. Both claims enter P1, P2 and P6 only through the consequences that other pages name (drive motor and CIA reset, the CPU continuing, PICs returning to the unconfigured state). |
| opus:K-0014, sol:K-0008 (p.384) | "Addressing jumpers were eliminated". | A fact about how cards are built. This system has no jumper whose absence could be observed. The other half, "the card's address instead being assigned by software", is P3. |
| opus:K-0014, sol:K-0008, K-0840, K-0838 and P6 | Everything, unless an AUTOCONFIG device is present. | The system under test is described as the chip set, a 680x0 CPU and Chip memory. The description names no expansion device, and the test cannot plug one in. P3-P6 need a PIC that answers in the Zorro II configuration space $00E8xxxx. Gate G (§5.2) decides whether one does. If none does, P3-P6 are reported "not checkable: no AUTOCONFIG device", never as passed. |
| K-0838 (p.435) | Everything, unless a Zorro III PIC sits in the Zorro II block and the CPU drives 32-bit addresses. | The table that holds the claim is headed "For Zorro III boards configured in the Zorro II space". A27-A24 do not exist on a 24-bit bus: the Zorro II bus "provides 24 bits of addressing information" (p.391). P5 is specified for that case only. |

No claim of this package needs data read from or written to a disk surface.

Propositions:

| | Proposition | Needs |
|---|---|---|
| P1 | RESET asserts the system reset (drive LED and power LED, graded separately as P1a and P1b) | nothing beyond the system |
| P2 | RESET does not reset the CPU; the next instruction executes | nothing beyond the system |
| P3 | A PIC's base address is the one software writes | a PIC (gate G) |
| P4 | Register 4A's nybble supplies A19-A16, and the write to 48 configures | a PIC that is a memory board of 512 KB or less |
| P5 | Register 46's nybble supplies A27-A24 | a Zorro III PIC in the Zorro II block, the 68020 setting and 32-bit decoding |
| P6 | RESET returns a configured PIC to the unconfigured state | a PIC, and Chip memory answering after RESET |

## 2. Conventions for every run

### 2.1 Program environment

- The program runs in supervisor mode with SR = $2700 from its first instruction.
- It writes $7FFF to INTENA ($DFF09A) and to DMACON ($DFF096). With SET/CLR = 0, this clears every interrupt enable and every DMA enable. Custom-register addresses are "680x0 address = (chip address) + $DFF000" (p.263).
- Exception vectors 2, 3, 4, 8, 10 and 11 (addresses $000008, $00000C, $000010, $000020, $000028, $00002C) point to a handler H_E, which writes C_E to COLOR00, writes V_E to chip word MK_E, and then loops. The vector numbers are Motorola, not the manual. With the 68010 and 68020 settings, VBR = 0. With the 68020 setting the instruction cache stays disabled (CACR = 0), so every instruction fetch reaches the bus (Motorola, not the manual).
- Vector-table check: the program writes H_E's address into $000020/$000022 and reads it back. Both accesses must appear at the Chip-memory port with that value. This shows that Chip memory, not the ROM overlay (p.223), answers at the vectors. Otherwise the run stops as "vector table not in Chip memory".
- Placement: code and data sit in the top 64 KB of the configured Chip memory. p.223 says the overlay ROM copy "responds starting at memory location $00000000" but not where it ends. Placing the code high only makes a continuation after RESET possible (§2.3); nothing assumes it.
- Marker M0: the first action of the program writes V0 to MK0. All results are CPU writes to chip words, which the test sees at the Chip-memory port with address and data.
- CPU settings: every run of P1 and P2 is repeated for core A (68000), core B as 68000, core B as 68010 and core B as 68020. PAL and ECS are the defaults, and no page makes these propositions depend on either.
- Colour values: C1 = $0C30, C2 = $03AC, C_E = $0F0F. Bits 15-12 are zero ("Unused", p.46), the three values are pairwise different, and none is $000. The builder may choose others with these properties.

### 2.2 Unanswered-address rule

No page states what a CPU access to an address that no device answers does. p.385 names a time-out only for Zorro III cycles: "the cycle completes when a Zorro III slave responds or the bus times out, as driven by the motherboard logic". So every access to an address that may be unanswered is the last bus action of its run:

- all other results are stored first;
- a completion marker follows the access;
- if the marker never appears, the run records "no answer" for that access. "Never appears" means no CPU access at the Chip-memory port within 10 ms, a harness limit rather than a page value.

This covers configuration space before gate G has passed, a base address before its board is configured, and an address a board has been moved away from. Because of this rule, several checks below need a run of their own.

### 2.3 The RESET sequence (used by P1, P2 and P6)

Before the sequence: A0 = $00DFF180 (COLOR00), D0 = C2, A2 = MK1, D1 = V1. Then three one-word instructions at consecutive addresses, with RESET at a long-word-aligned address A:

```
A-2:  MOVE.W D1,(A2)    ; marker V1 -> MK1; its Chip-port write is t_M1
A:    RESET
A+2:  MOVE.W D0,(A0)    ; C2 -> COLOR00
A+4:  tail              ; MOVE.W #V2,MK2, then a branch to itself
                        ; (in P6: the post-RESET reads of section 5.2)
```

Why this shape: p.223 warns "Since RAM may not be available, special care is needed to write reboot code that will reliably reboot all Amiga models". Before that it says, of the reset state, "On some Amiga models, portions of the RAM still respond. On other models, no RAM responds. Software must assume that memory is not available."

The instruction at A+2 is fetched before RESET executes: on the 68000 and 68010 by the two-word prefetch, on the 68020 because it shares one long word with RESET (Motorola, not the manual). It needs no memory to execute, because its source is a register and its destination a custom register, and its effect is visible on the video output.

The tail runs only if Chip memory answers at A+4 onward after RESET. P1 and P2 depend on nothing in the tail. A run ends at t_V2 (§3) whether or not M2 appears.

## 3. Reference frames and their anchors

- **t_M1**: the time of the Chip-port write of V1 to MK1. Anchor check: exactly one such write in the run.
- **Frames**: a frame lies between two consecutive leading edges of vertical sync on the video output.
- **t_V2**: the second vertical-sync leading edge after t_M1, so (t_M1, t_V2] contains at least one complete frame. The only duration a page gives is "approximately 17 microseconds" (p.370, the DRESB- pulse for a RESET instruction); t_V2 is a harness bound that contains it many times over.
- **Visible pixel**: a pixel sample whose RGB is not (0,0,0).
- **Reference frame F_ref**: the last complete frame before t_M1. Anchor check: F_ref contains pixels of value C1, every pixel in it is either C1 or (0,0,0), and no pixel before t_M1 has the value C2. This shows the display shows COLOR00 and nothing else: "The background color shows in any area on the display where there is no other object present and is also displayed outside the defined display window, in the border area" (p.46). For BPLCON0 bits 14-12 = 000: "Shows only a background color; no playfield is visible" (p.48).
- **t_C2**: the first visible pixel of value C2 after t_M1, or t_V2 if there is none.
- **LED levels**: L_bright and L_dim for the power LED, and L_on and L_off for the drive LED, as calibrated in P1's preconditions.

## 4. Configuration-register conventions (for P3-P6)

- **Configuration space**: "The Zorro II configuration space is the 64K memory block $00E8xxxx" (p.431). "An unconfigured Zorro PIC responds to the 64K address space starting at $00E80000 if its /CFGIN signal is asserted" (p.395).
- **Pairs**: a register is named by the first address of its pair (p.432), and "Every AUTOCONFIG register is logically considered to be 8 bits wide; the 8 bits actually being nybbles from two paired addresses" (p.431).
  - Figure K-14 a) (p.432): bits 7-4 of register rr are at $00E800rr, and bits 3-0 at $00E800rr+2.
  - Each nybble is the upper nybble of the byte at that even address: "the upper nybbles of locations $00E8003C and $00E8003E" (p.432). In a 68000 word read that is bits 15-12. Bits 11-0 of a read are never compared: "All read registers physically return only the top 4 bits of data" (p.431).
- **Complement**: "all read registers except for the 00 register are physically complemented" (p.432). The logical value of any register other than 00 is the inverse of the physical nybbles.
- **Writes**:
  - A nybble write puts the nybble in bits 7-4 of a byte written to the register's address. The page shows no write bit position, so this is an interpretation by analogy with the reads and with Figure K-14 (§7).
  - A byte write to 48 carries A23-A16 (p.435 table).
- **IMG**: the 32 physical nybbles read at $00E80000, $00E80002, ... $00E8003E, which are read registers 00 to 3C. Two IMGs are equal if all 32 nybbles are equal.
- **Decoded fields** (p.432-433):
  - type = register 00 bits 7-6: "10 Zorro III", "11 Zorro II", 00 and 01 "Reserved".
  - memory = register 00 bit 5: "If this bit is set, the PIC's memory will be linked into the system free pool".
  - size = register 00 bits 2-0, from the "Unextended" column for Zorro II.
  - register 08 bit 6: "set to indicate that the board can't be shut up by software".
- **Choice of base address**:
  - Memory board: the board lies wholly inside Zorro II memory space, "the standard 8 megabyte space from $00200000-$009FFFFF" (p.388, Figure K-1 p.386), at a multiple of its size where the space allows. An 8 MB board can only sit at $00200000.
  - I/O board: inside "$00E80000-$00EFFFFF" (p.388), above the configuration block, that is from $00E90000.
  - Natural alignment is stated only for "new sized" Zorro III cards (p.433). The bases above satisfy any decoder.

## 5. Propositions

### 5.1 Unconditional

#### P1: The RESET instruction asserts the system reset

**1. Proposition.** Executing RESET asserts the system reset, and the devices on it are reset:

- P1a: the disk drive motor turns off, and with it the drive LED.
- P1b: CIA-A's port A reverts to inputs, and the power LED leaves its bright state.

Claims opus:K-0164 and sol:K-0155, p.394: "an I/O-only reset, such as initiated by the 680x0 RESET instruction, will drive only /RST and /BUSRST on the bus". The same page says of these two: "The bus supplies two versions of the system reset signal."

Supporting pages:

- p.370 (DRESB-): "This signal is a buffered version of the system reset signal. Three things can make it go active (low): ... System CPU executes a RESET instruction (DRESB- will go low for approximately 17 microseconds)".
- p.244: "All drive motors turn off after system reset." and "DSKMOTOR* also controls the activity light on the front of the disk drive."
- p.223: "The 68000 RESET instruction works much like external reset or power on."
- p.354 (8520 RES): "A low on the RES pin resets all internal registers. The port pins are set as inputs and port registers to zero (although a read of the ports will return all highs because of passive pull-ups)."
- p.351: "PA1..LED* led light (0=bright)". p.157: "the brightness of the red "power" LED (CIA A $BFE001 - Bit 1: /LED)".

P1a rests on explicit statements (p.370 with p.244). P1b rests on an inference: no page says in so many words that the 8520 RES input is on the reset that the RESET instruction drives. The page evidence is p.223: RESET "works much like external reset or power on", and the ROM overlay returns, which "The OVL bit in one of the 8520 Chips" controls.

**2. Stimulus (run R1).**

a. Environment of §2.1. Then:
   - BPLCON0 ($DFF100) ← $0000: no bitplanes (p.48), and bit 0 ENBPLCN3 = 0, so BPLCON3 is not in effect (p.307, "Enable new BLPCON3 register", misprint as printed).
   - SPR0CTL-SPR7CTL ($DFF142 + 8n) ← $0000: "The sprite can be terminated ("disarmed") by writing to the SPRxCTL register" (p.123).
   - COLOR00 ($DFF180) ← C1.

b. Power LED:
   - Read CIA-A ddra ($BFE201) and pra ($BFE001).
   - Set ddra bit 1 = 1 (p.339: "1=output"), leaving bit 0 (OVL) of ddra and pra as they are. OVL's polarity is not printed (§7).
   - pra bit 1 ← 1; wait one frame. Then pra bit 1 ← 0; wait one frame. This is the calibration, and it leaves the LED bright.

c. Drive motor, using CIA-B ddrb ($BFD300) and prb ($BFD100). Bits of prb (p.340): /MTR /SEL3 /SEL2 /SEL1 /SEL0 /SIDE DIR /STEP.
   - ddrb ← $FF (p.340, "set to 0xFF").
   - Latch the motor off: prb ← $FF, $F7, $FF. This is calibration step 1.
   - Latch the motor on:
     - prb ← $7F: /MTR low before selecting. "All software that selects drives must set up the motor signal before selecting any drives" (p.244).
     - prb ← $77: /SEL0 low. "Each drive will latch the motor signal at the time its select signal turns on" (p.244).
     - prb ← $7F, then prb ← $FF: /MTR high, all deselected. "The drive will "remember" the state of its motor when it is not selected" (p.244).

d. Wait for two complete frames. F_ref is the last of them.

e. The RESET sequence of §2.3.

**3. Observation.** The power-LED and drive-LED levels over [t_M1, t_C2].

**4. Expected.**

- P1a: the drive LED is L_on at t_M1. Its first change after t_M1 goes to L_off and lies in [t_M1, t_C2].
  - Source: "All drive motors turn off after system reset" (p.244), with the RESET instruction making the system reset active (p.370).
  - Quantifier: "All" covers every drive. The test sees one drive LED, so it establishes the statement for drive 0 only.
- P1b: the power LED is L_bright at t_M1. Its first change after t_M1 goes to L_dim and lies in [t_M1, t_C2].
  - Source: the port pins become inputs and are pulled high (p.354), and /LED = 1 is not bright (p.351).
- Why the window ends at t_C2: the instruction at A+2 runs once RESET has finished ("the CPU continues execution with the next instruction", p.223), so the reset's effects come before its visible result. Levels after t_C2 are not graded, because what the CPU fetches after A+2 is not guaranteed to be the program (§2.3).

**5. Preconditions.**

- Power-LED calibration: with pra bit 1 = 1 the LED shows L_dim for a full frame, with bit 1 = 0 it shows L_bright, and L_bright ≠ L_dim.
- Drive-LED calibration: after the "off" latch the LED is L_off, and after the "on" latch it is L_on while prb = $FF (so the /MTR line itself is high); L_on ≠ L_off. This shows the drive LED shows the latched motor state, which is what p.244 says a system reset turns off.
- Display anchor: F_ref as in §3.
- No exception: no pixel of value C_E and no write of V_E to MK_E anywhere in the run.
- The write V1→MK1 appears exactly once, and the write V0→MK0 appears exactly once.

**6. Contrasting cases.**

- R1a: RESET at A replaced by NOP ($4E71), everything else unchanged. The page predicts no system reset: both LEDs keep L_on and L_bright from t_M1 to t_V2. C2 appears (P2), the tail runs, M2 appears and the program finishes.
- For the power-LED calibration: its two halves are the contrast (bit 1 = 1 against 0, p.351).
- For the drive-LED calibration: latching with /MTR high against low (p.244).
- For the display anchor: C1 replaced by C1' = $0C3F in step a. F_ref then shows C1'.
- For "no exception": before step e the program switches to user mode with MOVE #$0700,SR, so RESET becomes a privilege violation (Motorola, not the manual). H_E runs, a C_E pixel and V_E appear, and neither LED changes.

**7. Reference frame.** t_M1, t_C2 and the calibrated LED levels (§3). The anchors are checked by the calibrations and by the uniqueness of the V1 write.

#### P2: RESET does not reset the CPU; execution continues with the next instruction

**1. Proposition.** RESET is an I/O-only reset: it leaves the CPU running, and the CPU executes the instruction that follows RESET.

Claims opus:K-0164 and sol:K-0155, p.394: "an I/O-only reset, such as initiated by the 680x0 RESET instruction, will drive only /RST and /BUSRST on the bus. A full-system reset, such as a powerup reset or a keyboard reset, drives /HLT low as well. PICs that wish to reset the system CPU as well as the bus and I/O devices drive /RST and /HLT".

p.223 says it outright for the 68000: "The difference is that the CPU continues execution with the next instruction." p.411 agrees: /HLT "is driven, along with /RESET, to assert a full-system reset. A full-system reset is asserted on a powerup reset or a keyboard reset".

**2. Stimulus.** Run R1, the same run as P1.

**3. Observation.** The video output over (t_M1, t_V2], and the Chip-port writes of V0 to MK0.

**4. Expected.**

- At least one visible pixel of value C2 in (t_M1, t_V2]: the MOVE.W D0,(A0) after RESET was executed.
- No write of V0 to MK0 after t_M1: the program was not started again.
- Quantifier: p.394 says "680x0 RESET instruction", so R1 runs on all four CPU settings (§2.1). p.223 names only the 68000; for the 68010 and the 68020 only p.394 speaks.
- Not graded:
  - whether C1 stays on screen until C2 appears;
  - whether M2 appears (RAM "may not be available", p.223);
  - the time from t_M1 to t_C2 ("approximately 17 microseconds", p.370, gives no bound).

**5. Preconditions.**

- All of P1's preconditions.
- A system reset took place in this run: at least one of P1's LED changes is observed in [t_M1, t_C2]. Without it, a C2 pixel shows only that an instruction following a RESET that did nothing was executed. P2 is then reported "not graded: reset not witnessed".

**6. Contrasting cases.**

- R1b: MOVE.W D0,(A0) at A+2 replaced by NOP. The page predicts the next instruction, now a NOP, runs and nothing writes COLOR00: no C2 pixel in (t_M1, t_V2]. P1's LED changes still occur.
- The full-system reset that p.394 contrasts with RESET is not usable:
  - the harness sends key codes, not the keyboard hard reset, which is KCLK held low for at least 500 ms (p.361, "Valid for all keyboards except the Amiga 500");
  - a power-up reset would end the program.
- For "reset witnessed": R1a (P1's contrast) shows this precondition failing.

**7. Reference frame.** t_M1, t_V2 and the visible-pixel anchor F_ref (§3).

### 5.2 Conditional: an AUTOCONFIG device answers in $00E8xxxx

#### Gate G: an unconfigured PIC is present

Run R_G is a run of its own, because its first read may be unanswered (§2.2). It reads IMG0, stores it, and writes the completion marker.

G passes when:

- G1: the run completes;
- G2: type ∈ {10, 11};
- G3: registers 0C, 34, 38 and 3C read logical 0, that is physical 1111 at both pair addresses. They are "Reserved, must be 0" (pp.434-435); p.432 gives this example for 3C. Registers 30 and 40 are not used, because the page doubts their status: "Unsupported base register reset register under Zorro II?" (p.435).

G does not separate a PIC from a data bus that floats to all ones, which also satisfies G2 and G3. Presence is established only by P3's second expected value (IMG2 ≠ IMG0).

The PIC must still be unconfigured when the program starts. If the harness's start-up has already configured it, G fails, and the builder records that as the reason.

Contrasting case for G (run R_G'): only if register 08 bit 6 is logical 0 ("can be shut up"). The program writes $00 to $00E8004C and then reads IMG, which is expected to differ from IMG0: "Anything written to 4C will cause a board that supports shut-up to completely disappear until the next reset" (p.436). Otherwise P3 serves as G's contrast.

If G fails, P3-P6 are "not checkable: no AUTOCONFIG device answers in $00E8xxxx".

#### P3: Software assigns the PIC's base address

**1. Proposition.** An unconfigured Zorro II PIC takes the base address that software writes to its base-address register. After the final write it leaves the configuration space and answers at that address.

Claims opus:K-0014 and sol:K-0008, p.384: "Addressing jumpers were eliminated, the card's address instead being assigned by software".

Supporting pages:

- p.391: "The software then tells each board where to reside in memory."
- p.431: "a board will always be assigned a base address or be told to shut up. The act of writing the final bit of base address, or writing anything to a shutup address, will cause the PIC to assert its /CFGOUTn, enabling the next board in the configuration chain."
- p.435 footnote 6: "the configuration registers generally disappear when a board is configured, and absolutely must move out of the $00E8xxxx space."
- p.436: "writing to register 48 actually configures the board for both Zorro II and Zorro III boards in the Zorro II configuration block."

Applies to type 11. A type-10 PIC belongs to P5.

**2. Stimulus (run R3).**

a. §2.1 environment; read IMG0.

b. Choose B1 per §4. Byte write to $00E8004A with bits 7-4 = B1 bits 19-16 (register 4A nybble, "A19-A16", p.435). Read IMG1.

c. Byte write to $00E80048 with the value B1 bits 23-16 (register 48, nybble "A23-A20" / byte "A23-A16", p.435). This is the order of the p.435 table: 4A before 48. Read IMG2.

d. Memory board only ("with memory starting at the base address", p.433):
   - write P0 = $A55A to B1 and P1 = $3CC3 to B1+2;
   - write $0000 to a chip scratch word;
   - read B1 and B1+2 and store them as R0 and R1.

e. Last action (§2.2), memory board only:
   - write P2 = $5AA5 to B2, the base that R3' uses;
   - write $0000 to the scratch word;
   - read B2 and store it as R2;
   - write the completion marker.

**3. Observation.** The stored IMG0, IMG1, IMG2, R0, R1 and R2 as Chip-port writes, and the Chip-port log from the first write of step d to the store of R1.

**4. Expected.**

- IMG1 = IMG0: the 4A write alone does not configure. Configuration happens on "writing the final bit of base address" (p.431), and "writing to register 48 actually configures the board" (p.436).
- IMG2 ≠ IMG0: the PIC leaves $00E8xxxx (p.435 fn.6, p.431). If the next PIC in the chain has an identical image, IMG2 = IMG0 would be legitimate. The builder therefore states the number of AUTOCONFIG devices from the system configuration, and this value is expected when that number is one.
- Memory board: R0 = P0 and R1 = P1.
- Memory board: before the store of R0, no CPU write with data P0 or P1 appears at the Chip-memory port. This excludes B1 aliasing Chip memory.
- Memory board: R2 ≠ P2, or "no answer". The board does not also answer at the base it was not given.
- Quantifier: "the card's address" is a statement about every card. The test covers the PIC or PICs present, with two bases (R3 and R3').

**5. Preconditions.**

- G passed.
- B1 and B2 are valid per §4 and do not overlap Chip memory.
- The scratch write appears at the port between the writes to B1 and the reads.
- The memory flag has been read. For an I/O board only the IMG expected values are graded, because the page gives no content to read back at its base.

**6. Contrasting cases.**

- R3': R3 with B1 and B2 exchanged. B2 = B1 + size (or B1 + $100000 for boards under 1 MB), with the whole board inside the space. The page predicts that the board answers at B2 and not at B1. For an 8 MB board no second base exists; the contrast is then unavailable and is reported as such.
- For G: R_G'.
- For the memory flag: an I/O board runs only the IMG part.

**7. Reference frame.** The register pairs of Figure K-14 and the complement of p.432. G3 checks the anchor: the reserved registers read physical 1111, which confirms the pair mapping and the complement on this PIC.

#### P4: Register 4A's nybble supplies A19-A16, and the write to 48 configures (K-0840)

**1. Proposition.** In the Zorro II configuration block:

- a nybble written to register 4A supplies base bits A19-A16;
- a byte written there is not applicable;
- the following write to register 48 (nybble A23-A20, byte A23-A16) completes the configuration.

Claim K-0840, p.435 table: "4A A19-A16 N/A" and "48 A23-A20 A23-A16".

The table is headed "For Zorro III boards configured in the Zorro II space". For a Zorro II PIC the same assignment follows from two things: the low-order register "takes bits 23-16" (p.435), and Figure K-14 puts bits 3-0 of register 48 at $00E8004A.

Whether a given PIC's register is nybble-wide or byte-wide is the designer's choice: "Write registers are written to support nybble, byte, and word registers for the same register, again based on what works best in hardware" (p.431).

Applicable when: a memory board of 512 KB or less (size codes 001-100), so that some of A19-A16 lie at or above the board's size.

**2. Stimulus.** Runs R4x and R4y, identical up to the last step.

a. §2.1 environment; read IMG0.

b. Choose H (A23-A20) and two nybbles X ≠ Y so that B_X = H·$100000 + X·$10000 and B_Y = H·$100000 + Y·$10000:
   - are valid bases per §4;
   - differ in at least one bit at or above the board's size.

   Example for a 64 KB board: H = 2, X = 3, Y = 5.

c. Byte write to $00E8004A with bits 7-4 = X. Byte write to $00E80048 with the value H·16 + Y. Read IMG2.

d. Last action:
   - R4x: write P0 to B_X, write the scratch word, read B_X into RX, write the completion marker.
   - R4y: the same at B_Y, into RY.

**3. Observation.** IMG2, and RX and RY (or "no answer").

**4. Expected.**

- IMG2 ≠ IMG0: the write to 48 configured the board.
- Exactly one of RX = P0 and RY = P0 holds:
  - RX = P0 if the PIC's register is nybble-wide (K-0840: 4A supplies A19-A16);
  - RY = P0 if it is byte-wide (the 48 byte supplies A23-A16, and 4A is "N/A").
- The page does not say which a given PIC is, so neither alone is expected. Both, or neither, is a failure.
- Only this write with X ≠ Y exercises K-0840. P3's consistent write cannot tell the two widths apart.

**5. Preconditions.**

- G passed.
- Memory board of 512 KB or less.
- B_X and B_Y are valid.
- The scratch write appears at the port.
- Before step c, IMG after the 4A write equals IMG0 (as in P3).

**6. Contrasting case.** X = Y. For both widths the page predicts the board at B_X = B_Y, so RX = RY = P0.

**7. Reference frame.** As P3.

#### P5: Register 46's nybble supplies A27-A24 (K-0838)

**1. Proposition.** For a Zorro III PIC configured in the Zorro II block, a nybble written to register 46 supplies A27-A24, and a byte write there is not applicable. The four registers are written in the table's order 46, 44, 4A, 48, and the 48 write configures the board.

Claim K-0838, p.435: "46 A27-A24 N/A", "44 A31-A28 A31-A24". p.436: "writing to register 48 actually configures the board for both Zorro II and Zorro III boards in the Zorro II configuration block".

Applicable when all three hold:

- G passes with type 10;
- the program runs with the 68020 setting;
- the system decodes addresses above 24 bits.

Otherwise it is not checkable: A27-A24 are not on a 24-bit bus (p.391).

**2. Stimulus.**

a. Probe run R5p (68020): write V to chip word W, then write V' ≠ V to W + $01000000 as the last action. If a write of V' to W appears at the Chip-memory port, the system ignores A31-A24 and P5 is not checkable.

b. Runs R5x and R5y (68020). Read IMG0. Choose a base in "Zorro III expansion space" ($10000000-$7FFFFFFF on the A3000 map, Figure K-1 p.386), aligned on the board's natural boundary: "all new sized cards get aligned on their natural boundaries" (p.433). The board must be smaller than 16 MB, so that A27-A24 select its base.

c. Choose A31-A28 = 1, X ≠ Y, B_X = $1X000000 and B_Y = $1Y000000. Then write, in this order:
   - $00E80046: nybble X;
   - $00E80044: byte $10 + Y;
   - $00E8004A: nybble 0;
   - $00E80048: byte $00.

   Read IMG2.

d. Last action:
   - R5x: write P0 to B_X, write the scratch word, read B_X into RX, write the completion marker.
   - R5y: the same at B_Y, into RY.

**3. Observation.** IMG2, RX and RY.

**4. Expected.**

- IMG2 ≠ IMG0.
- Exactly one of RX = P0 (nybble-wide: 46 supplies A27-A24) and RY = P0 (byte-wide: the 44 byte supplies A31-A24) holds. As in P4, the page leaves the width open.
- If the PIC's register 00 bit 5 is clear (not memory), only IMG2 ≠ IMG0 is graded.

**5. Preconditions.** G with type 10, the probe of step a, and the scratch write visible.

**6. Contrasting case.** X = Y: the page predicts RX = RY = P0.

**7. Reference frame.** As P3.

#### P6: RESET returns a configured PIC to the unconfigured state

**1. Proposition.** RESET asserts /BUSRST, on which every expansion device resets its autoconfiguration logic. A PIC configured before RESET therefore answers again, unconfigured, in the configuration space afterwards.

Claims opus:K-0164 and sol:K-0155, p.394: RESET "will drive only /RST and /BUSRST on the bus".

Supporting pages:

- p.394: "All expansion devices are required to reset their autoconfiguration logic when /BUSRST is asserted."
- p.431: "When an Amiga powers up or resets, every card in the system goes to its unconfigured state."
- p.223: "All memory and AUTOCONFIG(TM) cards disappear".

**2. Stimulus (run R6).**

a. §2.1 environment, and P1 steps a-d.

b. Read IMG0. Configure at B1 as in P3 steps b-c, and read IMG2. For a memory board, write P0 to B1, write the scratch word, and read B1 into R0.

c. The RESET sequence of §2.3. Its tail is:
   - MOVE.W #V2,MK2;
   - read IMG3 and store it;
   - memory board, as the last action: write P0' = $6996 to B1, write the scratch word, read B1 into R3, write the completion marker.

   Configuration space is at $00E8xxxx; the overlay is described as "starting at memory location $00000000" (p.223).

**3. Observation.** IMG3, R3 and M2.

**4. Expected.**

- IMG3 = IMG0.
- Memory board: R3 ≠ P0', or "no answer" ("All memory and AUTOCONFIG(TM) cards disappear", p.223).
- Quantifier: "All expansion devices" and "every card". After a reset only the first PIC in the chain answers ("The configuration chain simply ensures that only one unconfigured card will see an asserted /CFGINn at one time", p.431). The test sees that one device and does not establish "every".

**5. Preconditions.**

- G passed.
- P3's expected values hold in this run before RESET (IMG2 ≠ IMG0; R0 = P0 for a memory board).
- P1's reset witness: at least one LED change in [t_M1, t_C2].
- M2 appears at the Chip-memory port, and the CPU's instruction fetches from A+4 onward appear there. That is, Chip memory answers at the program's addresses after RESET. The page does not promise this (p.223). If it does not happen, P6 is "not checkable on this system".

**6. Contrasting cases.**

- R6a: RESET replaced by NOP. The page predicts IMG3 = IMG2 (≠ IMG0) and R3 = P0'.
- For the reset-witness precondition: R1a.
- For "Chip memory answers after RESET": no page predicts an outcome either way ("On some Amiga models, portions of the RAM still respond. On other models, no RAM responds", p.223), so there is no contrasting stimulus with a page-predicted result. This is stated here rather than invented.

**7. Reference frame.** As P1 and P3.

## 6. What the pages leave open (no expected value; a test must not invent one)

- Which chip-set registers the RESET instruction resets. Agnus has an RST* input (p.381) and Paula has RES*, "System reset" (p.380), but no page says what they reset. Denise's pin list (p.380) shows no reset input. No expected value for any custom register after RESET.
- Whether Chip memory answers after RESET, and where the ROM overlay ends (p.223).
- The duration of the reset: DRESB- goes low for "approximately 17 microseconds" (p.370).
- "some bus devices such as processor cards may internally reset only on full-system resets" (p.394): "may", and not applicable here.
- The width of a PIC's base-address register, nybble, byte or word (p.431).
- What a CPU access to an address that no device answers does (§2.2).
- The polarity of OVL (§7). The program therefore never changes OVL.
- The ROM's first word: "The value of the ID code may change in the future" (p.223). Not used.
- Any drive other than drive 0 (p.244 "All drive motors"): no LED is seen for it.

## 7. Page conflicts and ambiguities

1. **Scope of the RESET statement.** p.223 speaks of "The 68000 RESET instruction", p.394 of "the 680x0 RESET instruction". The two agree for the 68000. For the 68010 and 68020 only p.394 speaks, and P1 and P2 follow it.
2. **"All memory".** p.223, "All memory and AUTOCONFIG(TM) cards disappear", can be read as all memory cards or as all RAM. The same page says some models keep portions of RAM after reset and others none. Not graded.
3. **Data lines of a read.** p.431 says read registers return data "on D31-D28 for either bus mode", which is the top of the A3000's 32-bit bus. p.432 speaks of "the upper nybbles of locations". On this system's 16-bit bus that is D15-D12 of a word read, and §4 uses that interpretation.
4. **Figure number.** p.432 refers to "Figure 9-10", but the figure is captioned "Figure K-14".
5. **Scope of the nybble/byte table.** p.435's table is headed for Zorro III boards in the Zorro II space. For Zorro II boards the same 48/4A assignment is derived (P4), not printed.
6. **Registers 30 and 40.** p.435 describes them with question marks ("Unsupported base register reset register under Zorro II?", "Unsupported control state register under Zorro II?"). They are not used.
7. **Register 08 bit 7.** Its meaning differs between Zorro II ("place the card in the 8 megabyte Zorro II space") and Zorro III ("the board is basically a memory device") (p.433). Only the memory flag of register 00 is used.
8. **Alignment.** Natural alignment is stated only for new-sized Zorro III cards (p.433). An 8 MB Zorro II board in $00200000-$009FFFFF (p.388) cannot be naturally aligned, so these pages give no alignment rule for Zorro II boards.
9. **CIA name on p.244.** The printed p.244 says "Bits labeled "PB" are output bits located in CIAAPRB ($BFD100)". The name says CIA-A, but $BFD100 is CIA-B's prb (p.340), and the page's own heading reads "CIAAPRA/CIABPRB". The specification uses $BFD100 with p.340's bit layout.
10. **OVL polarity.** No page prints it (p.223, p.339, p.351). It can only be inferred: after reset the port pins read high (p.354) and the overlay is present (p.223). Not relied on.
11. **Write bit position.** No page shows which bits of a written byte a nybble-wide configuration register takes. §4 assumes bits 7-4, like the reads.
