# Specification: the RESET instruction and AUTOCONFIG base-address assignment

This specification was written from the package's claims (`claims-spec-stage.jsonl`) and the pages of the *Amiga Hardware Reference Manual*, 3rd edition, by an author who never saw the implementation or any test. Page numbers are printed page numbers. Quotations are from the printed pages. Pp. 46, 48, 123, 157, 223, 244, 263, 307, 339, 340, 351, 354, 370, 380, 386, 394, 432, 433 and 435 were checked against the page image, because their text copy is re-edited or damaged or because the quotation carries weight. Where the specification relies on 680x0 behaviour that the manual does not describe, it says so ("Motorola, not the manual").

## 1. Claims not turned into a proposition, or used only in part

| Claim | What is not used | Reason |
|---|---|---|
| opus:K-0164, sol:K-0155 (p.394) | The bus-line levels themselves: that /RST and /BUSRST are driven and /HLT is not. | No expansion-bus line is an output the test can see. No page identifies nanomig.v's `cpu_nrst_out` output with /RST, /BUSRST, /HLT or any other signal, so the specification gives it no meaning and grades nothing on it; a builder may record its level during RESET as a measurement at the ports, labelled as such. Both claims are graded whole by P7 (§5.3) through the consequences that other pages name: the system reset (P1's reset witness), no /HLT (the CPU continuing, P2), and /BUSRST (a PIC returning to the unconfigured state, P6). |
| opus:K-0014, sol:K-0008 (p.384) | The addressing-jumpers-eliminated half: "Addressing jumpers were eliminated". | A fact about how cards are built; this system has no jumper whose absence could be observed. The other half of each claim, "the card's address instead being assigned by software", is graded by P3-NOTOTHER and P3-BASE (§5.2), which carry both ids. |
| K-0840 (p.435) | Everything. | The table that holds the claim is headed "For Zorro III boards configured in the Zorro II space". Applying it to a Zorro II board is a derivation the page does not print, and the only expansion device the harness offers is Zorro II fast RAM (§5.2). P4 is therefore not specified. |
| P3, P6, and the /BUSRST part of P7 | Everything, unless an AUTOCONFIG device is present. | The system under test is the chip set, a 680x0 CPU and Chip memory. The harness offers one expansion device, Zorro II fast RAM, through the `fastram_config` input, whose default 0 means "no Zorro II fast RAM offered" (`nanomig_harness.h`, `Config`). Every run of gate G, P3 and P6 sets `fastram_config` to a value that offers it (§5.2). P3 and P6 need a PIC that answers in the Zorro II configuration space $00E8xxxx. Gate G (§5.2) decides whether one does. If none does, P3 and P6 are reported "not checkable: no AUTOCONFIG device", never as passed, and P7's two ids are withdrawn whole to a record (§5.3). |
| K-0838 (p.435) | Everything. | Not testable at this boundary. The table that holds the claim is headed "For Zorro III boards configured in the Zorro II space", so the claim needs a Zorro III PIC (type 10) and a CPU that drives A27-A24. The only expansion device the harness offers is "Zorro II fast RAM" (`Config.fastram_config`, `nanomig_harness.h`). nanomig.v's memory address ports are `ram_address[23:1]` and `fastram_addr[23:1]`, and the harness's `cpu_config` settings change the core's behaviour, not its bus (§2.1). A27-A24 do not exist on a 24-bit bus: the Zorro II bus "provides 24 bits of addressing information" (p.391). |

Two claims from a neighbouring unit's claim set are added to this package: sol:K-0837 and opus:K-0869 (p.435, "the low order register takes bits 23-16"), graded by P3-NOTOTHER and P3-BASE (§5.2) alongside the software-assignment half of opus:K-0014 and sol:K-0008 above. hrm/appK-autoconfig-base-address declined both as `unreachable-observable` at `cpu_wrapper`; the base-address comparator the sentence describes is observable at this boundary, through where the board answers.

No claim of this package needs data read from or written to a disk surface.

Propositions:

| | Proposition | Needs |
|---|---|---|
| P1 | RESET asserts the system reset (drive LED and power LED, graded separately as P1a and P1b) | nothing beyond the system |
| P2 | RESET does not reset the CPU; the next instruction executes | nothing beyond the system |
| P3 | A PIC's base address is the one software writes; P3-NOTOTHER and P3-BASE carry sol:K-0837, opus:K-0869 and the software-assignment half of opus:K-0014, sol:K-0008 | a PIC (gate G) |
| P4 | Register 4A's nybble supplies A19-A16: declined (§1, K-0840) | a Zorro III PIC in the Zorro II block, which this boundary does not offer |
| P5 | Register 46's nybble supplies A27-A24: declined (§1, K-0838) | a Zorro III PIC and a 32-bit address bus, which this boundary does not offer |
| P6 | RESET returns a configured PIC to the unconfigured state | a PIC, and Chip memory answering after RESET |
| P7 | RESET drives only /RST and /BUSRST: one assertion over P1's reset witness, P2 and P6, carrying opus:K-0164 and sol:K-0155 | what P1, P2 and P6 need |

## 2. Conventions for every run

### 2.1 Program environment

- The program runs in supervisor mode with SR = $2700 from its first instruction.
- It writes $7FFF to INTENA ($DFF09A) and to DMACON ($DFF096). With SET/CLR = 0, this clears every interrupt enable and every DMA enable. Custom-register addresses are "680x0 address = (chip address) + $DFF000" (p.263).
- ROM overlay: after reset "An additional copy of the system ROM responds starting at memory location $00000000. The system RAM that would normally be located at this address is not available", and "The OVL bit in one of the 8520 Chips disables the overlay" (p.223). OVL is CIA-A pra bit 0 (p.339). Before it writes a vector, the program writes ddra ($BFE201) ← $03 ("Direction for port A (BFE001);1=output (set to 0x03)", p.339), then pra ($BFE001) ← $02: bit 0 (OVL) = 0, bit 1 (/LED) = 1. No page prints which OVL level disables the overlay; 0 is the level §7.10 infers, and the vector-table check below confirms that RAM answers at the vectors. Every later write to pra keeps bit 0 at 0.
- Exception vectors 2, 3, 4, 8, 10 and 11 (addresses $000008, $00000C, $000010, $000020, $000028, $00002C) point to a handler H_E, which writes C_E to COLOR00, writes V_E to chip word MK_E, and then loops. The vector numbers are Motorola, not the manual. With the 68010 and 68020 settings, VBR = 0. With the 68020 setting the instruction cache stays disabled (CACR = 0), so every instruction fetch reaches the bus (Motorola, not the manual).
- Vector-table check: the program writes H_E's address into $000020/$000022, reads it back and stores what it read. The check is that the long reads back as written; it does not depend on the RAM-port address at which the harness places $000020. This shows that RAM, not the ROM overlay (p.223), answers at the vectors. A failed check is a fixture failure and stops the package: nothing is graded.
- Placement: the harness loads the program at PROGRAM_ORG = $080400, sets the supervisor stack to PROGRAM_STACK = $0FFF00, and keeps results at RESULTS = $0C0000 (`nanomig.i`). All three lie in $080000-$1FFFFF, which the harness maps one-to-one. MK0, MK1, MK2, MK_E, the scratch word and every stored result are distinct words in $0C0000-$0C0FFD. The completion marker is `nanomig.i`'s DONE (DONE_VALUE $600D to DONE_ADDR $0C0FFE). Below $080000 the program writes only the vectors. p.223 says the overlay ROM copy "responds starting at memory location $00000000" but not where it ends, so nothing assumes that the program's addresses answer after RESET (§2.3, P6's preconditions).
- Chip-memory port: nanomig.v's RAM port (`ram_address`, `ram_data`, `_ram_we`, `_ram_oe`, `_ram_bhe`, `_ram_ble`), logged by the harness's `ram_log()`. It carries more than Chip memory: the harness keeps the overlay's ROM copy on it at physical byte address $780000 (`nanomig_harness.h`), and an expansion board's memory may appear on it. A Chip-memory address on it is a byte address below $200000, the 2 MB of Chip memory the harness configures (`Config.memory_config` = $03); the harness maps $080000-$1FFFFF one-to-one.
- Marker M0: the first action of the program writes V0 to MK0. All results are CPU writes to chip words, which the test sees at the Chip-memory port with address and data.
- CPU settings: every run of P1, P2 and P6 (R1, R6 and R6m, and their contrasts) is repeated for core A (68000), core B as 68000, core B as 68010 and core B as 68020, because p.394 speaks of "the 680x0 RESET instruction" and P7 grades that sentence whole. The harness's `cpu_config` selects the core's behaviour, not its bus: with `turbo_config` = 0 "every CPU access uses the slow chip bus" (`nanomig_harness.h`), the 16-bit RAM port, on every setting. PAL and ECS are the defaults, and no page makes these propositions depend on either.
- Every program is assembled for the 68000, vasm's default, on every CPU setting. No stimulus needs a later instruction, and with `-m68010` or higher vasm assembles `MOVE #0,<ea>` as `CLR <ea>`, which on the 68000 reads before it writes (the zero writes to BPLCON0, SPRxCTL, the scratch word, and a zero nybble or byte to a configuration register).
- Floppy drive: the harness default `floppy_config` = 0 is documented as "one drive (DF0), write-enabled, normal speed" (`nanomig_harness.h`, `Config`). Every run that performs P1 step c uses this default; it is harness setup, not an expected value. P1's drive-LED calibration shows at the port whether the drive latches the motor. No disk image is needed.
- Colour values: C1 = $0C30, C2 = $03AC, C_E = $0F0F. Bits 15-12 are zero ("Unused", p.46), the three values are pairwise different, and none is $000. The builder may choose others with these properties.

### 2.2 Unanswered-address rule

No page states what a CPU access to an address that no device answers does. p.385 names a time-out only for Zorro III cycles: "the cycle completes when a Zorro III slave responds or the bus times out, as driven by the motherboard logic". So every access to an address that may be unanswered is the last bus action of its run:

- all other results are stored first;
- a completion marker follows the access;
- if the marker never appears, the run records "no answer" for that access. "Never appears" means no CPU access at the Chip-memory port between the access and the second vertical-sync leading edge after it (§3), a harness limit rather than a page value. It is counted in frames on the video output, so no conversion from `clk_sys` to time is needed.

This covers configuration space before gate G has passed, a base address before its board is configured, and an address a board has been moved away from, including configuration space once its board has been configured by the write to register 48 or shut up. Because of this rule, several checks below need a run of their own.

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

On a 68000 the instruction at A+2 is fetched before RESET executes, by the two-word prefetch (Motorola, not the manual). The harness's 68010 and 68020 settings do not change the bus (§2.1): every setting fetches over the 16-bit RAM port, and the long-word fetch of a real 68020 is not assumed. The instruction at A+2 needs no memory to execute, because its source is a register and its destination a custom register, and its effect is visible on the video output.

The tail runs only if Chip memory answers at A+4 onward after RESET. P1 and P2 depend on nothing in the tail. A run ends at t_V2 (§3) whether or not M2 appears.

## 3. Reference frames and their anchors

- **t_M1**: the time of the Chip-port write of V1 to MK1. Anchor check: exactly one such write in the run.
- **Frames**: a frame lies between two consecutive leading edges of vertical sync on the video output.
- **t_V2**: the second vertical-sync leading edge after t_M1, so (t_M1, t_V2] contains at least one complete frame. The only duration a page gives is "approximately 17 microseconds" (p.370, the DRESB- pulse for a RESET instruction); t_V2 is a harness bound that contains it many times over.
- **Visible pixel**: a pixel sample whose RGB is not (0,0,0).
- **Reference frame F_ref**: the last complete frame before t_M1. Anchor check: F_ref contains pixels of value C1, every pixel in it is either C1 or (0,0,0), and no pixel before t_M1 has the value C2. This shows the display shows COLOR00 and nothing else: "The background color shows in any area on the display where there is no other object present and is also displayed outside the defined display window, in the border area" (p.46). For BPLCON0 bits 14-12 = 000: "Shows only a background color; no playfield is visible" (p.48).
- **t_C2**: the first visible pixel of value C2 after t_M1, or t_V2 if there is none.
- **LED levels**: L_bright and L_dim for the power LED, and L_on and L_off for the drive LED, as calibrated in P1's preconditions.

## 4. Configuration-register conventions (for P3 and P6)

- **Configuration space**: "The Zorro II configuration space is the 64K memory block $00E8xxxx" (p.431). "An unconfigured Zorro PIC responds to the 64K address space starting at $00E80000 if its /CFGIN signal is asserted" (p.395).
- **Pairs**: a register is named by the first address of its pair (p.432), and "Every AUTOCONFIG register is logically considered to be 8 bits wide; the 8 bits actually being nybbles from two paired addresses" (p.431).
  - Figure K-14 a) (p.432): bits 7-4 of register rr are at $00E800rr, and bits 3-0 at $00E800rr+2.
  - Each nybble is the upper nybble of the byte at that even address: "the upper nybbles of locations $00E8003C and $00E8003E" (p.432). In a 68000 word read that is bits 15-12. Bits 11-0 of a read are never compared: "All read registers physically return only the top 4 bits of data" (p.431).
- **Complement**: "all read registers except for the 00 register are physically complemented" (p.432). The logical value of any register other than 00 is the inverse of the physical nybbles.
- **Writes**:
  - A nybble write puts the nybble in bits 7-4 of a byte written to the register's address. The page shows no write bit position, so this is an interpretation by analogy with the reads and with Figure K-14 (§7).
  - A byte write to 48 carries A23-A16: "the low order register takes bits 23-16" (p.435). By the pair mapping above, the nybble at 4A is its bits 3-0, A19-A16. This is how the stimulus writes a Zorro II base; no proposition grades which of the two a board uses.
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

p.394: "an I/O-only reset, such as initiated by the 680x0 RESET instruction, will drive only /RST and /BUSRST on the bus". The same page says of these two: "The bus supplies two versions of the system reset signal." P1 carries no claim id; its reset witness is part (i) of P7, which carries opus:K-0164 and sol:K-0155.

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
   - ddra ($BFE201) bit 1 is 1 from §2.1 (p.339: "1=output"), and every write to pra ($BFE001) keeps bit 0 (OVL) at 0, as §2.1 set it.
   - pra bit 1 ← 1 (pra ← $02); wait one frame. Then pra bit 1 ← 0 (pra ← $00); wait one frame. This is the calibration, and it leaves the LED bright.

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
- The write V1→MK1 appears exactly once, and the write V0→MK0 appears exactly once before t_M1. Writes of V0 after t_M1 are P2's record, not a precondition.

**6. Contrasting cases.**

- R1a: RESET at A replaced by NOP ($4E71), everything else unchanged. The page predicts no system reset: both LEDs keep L_on and L_bright from t_M1 to t_V2. C2 appears (P2), the tail runs, M2 appears and the program finishes.
- For the power-LED calibration: its two halves are the contrast (bit 1 = 1 against 0, p.351).
- For the drive-LED calibration: latching with /MTR high against low (p.244).
- For the display anchor: C1 replaced by C1' = $0C3F in step a. F_ref then shows C1'.
- For "no exception": before step e the program switches to user mode with MOVE #$0700,SR, so RESET becomes a privilege violation (Motorola, not the manual). H_E runs, a C_E pixel and V_E appear, and neither LED changes.

**7. Reference frame.** t_M1, t_C2 and the calibrated LED levels (§3). The anchors are checked by the calibrations and by the uniqueness of the V1 write.

#### P2: RESET does not reset the CPU; execution continues with the next instruction

**1. Proposition.** RESET is an I/O-only reset: it leaves the CPU running, and the CPU executes the instruction that follows RESET.

p.394: "an I/O-only reset, such as initiated by the 680x0 RESET instruction, will drive only /RST and /BUSRST on the bus. A full-system reset, such as a powerup reset or a keyboard reset, drives /HLT low as well. PICs that wish to reset the system CPU as well as the bus and I/O devices drive /RST and /HLT". P2 carries no claim id; its graded expected value is part (ii) of P7, which carries opus:K-0164 and sol:K-0155.

p.223 says it outright for the 68000: "The difference is that the CPU continues execution with the next instruction." p.411 agrees: /HLT "is driven, along with /RESET, to assert a full-system reset. A full-system reset is asserted on a powerup reset or a keyboard reset".

**2. Stimulus.** Run R1, the same run as P1.

**3. Observation.** The video output over (t_M1, t_V2], and the Chip-port writes of V0 to MK0.

**4. Expected.**

- At least one visible pixel of value C2 in (t_M1, t_V2]: the MOVE.W D0,(A0) after RESET was executed.
- Recorded, not graded: the number of writes of V0 to MK0 in (t_M1, t_V2], which is zero if the program was not started again. No contrasting input exists inside a window the page supports (item 6 below), so this is a measurement.
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
- For the recorded V0 count there is no contrasting input inside (t_M1, t_V2]. The full-system reset that p.394 contrasts with RESET would start the program again from the reset vectors ("The next two words are used as the initial program counter by the 680x0 processor", p.223), and nanomig.v has a `reset` input, which the harness drives in `boot()` to start the system. But no page says how long the system takes to restart after it: p.370 gives DRESB- "approximately one second" for power-up and "as long as keyboard reset is held down" for a keyboard reset. The restart may fall after t_V2, and a longer window would rest on no page value. The harness sends key codes, not the keyboard hard reset, which is KCLK held low for at least 500 ms (p.361, "Valid for all keyboards except the Amiga 500").
- For "reset witnessed": R1a (P1's contrast) shows this precondition failing.

**7. Reference frame.** t_M1, t_V2 and the visible-pixel anchor F_ref (§3).

### 5.2 Conditional: an AUTOCONFIG device answers in $00E8xxxx

The harness offers one expansion device, Zorro II fast RAM, through the `fastram_config` input; its default 0 means "no Zorro II fast RAM offered" (`nanomig_harness.h`, `Config`). Every run of gate G, P3 and P6, contrasts included, sets `fastram_config` to a value that offers it. The builder picks it from the harness's documented `Config` values and states it in the package; it is harness setup, not an expected value. Gate G decides whether a PIC answers.

#### Gate G: an unconfigured PIC is present

Run R_G is a run of its own, because its first read may be unanswered (§2.2). It reads IMG0, stores it, and writes the completion marker.

G passes when:

- G1: the run completes;
- G2: type ∈ {10, 11};
- G3: registers 0C, 34, 38 and 3C read logical 0, that is physical 1111 at both pair addresses. They are "Reserved, must be 0" (pp.434-435); p.432 gives this example for 3C. Registers 30 and 40 are not used: p.435 adds "Unsupported base register reset register under Zorro II." and "Unsupported control state register under Zorro II." with footnotes 6 and 7 (7 reads "IBID"), and footnote 6 says "The original Zorro specifications called for a few registers, like these, that remained active after configuration."

G does not separate a PIC from a data bus that floats to all ones, which also satisfies G2 and G3. Presence is established only by P3's second expected value (IMG2 ≠ IMG0).

The PIC must still be unconfigured when the program starts. If the harness's start-up has already configured it, G fails, and the builder records that as the reason.

GATE-G (run R_G'): writes $00 to the shut-up address $00E8004C, then reads IMG as the last action (§2.2), on the same "differs from IMG0, or no answer" comparator P3-IMG2 uses. "Anything written to 4C will cause a board that supports shut-up to completely disappear until the next reset" (p.436).

GATE-G's precondition, asserted before grading: register 08 bit 6 reads logical 0, "the board can be shut up" (p.433, §4). Where it does not hold, GATE-G is reported "not graded: register 08 bit 6 indicates the board cannot be shut up" rather than graded.

GATE-G's green control, on the same comparator, with a live input proven to go green: P3-IMG2 (§5.2 P3, run R3), which differs from IMG0 after the configuring write to register 48 (p.435 fn.6, p.431).

If G fails, P3 and P6 are "not checkable: no AUTOCONFIG device answers in $00E8xxxx", and P7's ids are withdrawn whole to a record (§5.3).

#### P3: Software assigns the PIC's base address

**1. Proposition.** An unconfigured Zorro II PIC takes the base address that software writes to its base-address register. After the final write it leaves the configuration space and answers at that address.

p.384: "the card's address instead being assigned by software". P3 itself carries no claim id; its two sub-checks P3-NOTOTHER and P3-BASE (below) carry opus:K-0014 and sol:K-0008 (the software-assignment half; the "Addressing jumpers were eliminated" half of each is declined, §1) together with sol:K-0837 and opus:K-0869 (p.435, "the low order register takes bits 23-16").

Supporting pages:

- p.391: "The software then tells each board where to reside in memory."
- p.431: "a board will always be assigned a base address or be told to shut up. The act of writing the final bit of base address, or writing anything to a shutup address, will cause the PIC to assert its /CFGOUTn, enabling the next board in the configuration chain."
- p.435 footnote 6: "the configuration registers generally disappear when a board is configured, and absolutely must move out of the $00E8xxxx space."
- p.436: "writing to register 48 actually configures the board for both Zorro II and Zorro III boards in the Zorro II configuration block."

Applies to type 11. A type-10 PIC is outside this specification (K-0838, §1).

**2. Stimulus (runs R3 and R3n, identical up to the last step).**

a. §2.1 environment; read IMG0.

b. Choose B1 per §4. Byte write to $00E8004A with bits 7-4 = B1 bits 19-16 (§4 Writes). Read IMG1.

c. Byte write to $00E80048 with the value B1 bits 23-16 (§4 Writes: "the low order register takes bits 23-16", p.435). 4A comes before 48 because "writing to register 48 actually configures the board for both Zorro II and Zorro III boards in the Zorro II configuration block" (p.436). After this write the PIC leaves configuration space (p.435 fn.6), which may then be unanswered, so configuration space is read again only as the last action (§2.2).

d. Memory board only ("with memory starting at the base address", p.433):
   - write P0 = $A55A to B1 and P1 = $3CC3 to B1+2;
   - write $0000 to the scratch word;
   - read B1 and B1+2 and store them as R0 and R1.

e. Last action (§2.2):
   - R3: read IMG2, store it, and write the completion marker.
   - R3n, memory board only: write P2 = $5AA5 to B2, the base that R3' uses; write $0000 to the scratch word; read B2 and store it as R2; write the completion marker.

**3. Observation.** The stored IMG0, IMG1, R0 and R1 of both runs, IMG2 (R3) and R2 (R3n) as Chip-port writes or "no answer", and the Chip-port log from the first write of step d to the store of R1.

**4. Expected.**

- IMG1 = IMG0: the 4A write alone does not configure. Configuration happens on "writing the final bit of base address" (p.431), and "writing to register 48 actually configures the board" (p.436).
- IMG2 ≠ IMG0, or "no answer": the PIC leaves $00E8xxxx (p.435 fn.6, p.431). If the next PIC in the chain has an identical image, IMG2 = IMG0 would be legitimate. The builder therefore states the number of AUTOCONFIG devices from the system configuration, and this value is expected when that number is one.
- Memory board: R0 = P0 and R1 = P1, in both runs.
- Memory board: before the store of R0, no CPU write with data P0 or P1 appears at a Chip-memory address of the Chip-memory port (§2.1). This excludes B1 aliasing Chip memory. A write of P0 or P1 at a Chip-memory-port address of $200000 or above is not in Chip memory; it is recorded with its address, not graded.
- P3-NOTOTHER, memory board, carrying opus:K-0014, sol:K-0008, sol:K-0837, opus:K-0869 [p391,p433-434]: R2 ≠ P2, or "no answer". "The software then tells each board where to reside in memory" (p.391); "the low order register takes bits 23-16" (p.435), read through the base-address comparator of p.433-434 ("110 2 megabytes" register 08 size field pairs the base with the board's size). The board does not also answer at the base it was not given.
- Quantifier: "the card's address" is a statement about every card. The test covers the PIC or PICs present, with two bases (R3 and R3').

**5. Preconditions.**

- G passed.
- B1 and B2 are valid per §4 and do not overlap Chip memory.
- The scratch write appears at the port between the writes to B1 and the reads.
- The memory flag has been read. For an I/O board only the IMG expected values are graded, because the page gives no content to read back at its base.

**6. Contrasting cases.**

- P3-BASE (R3' and R3n'), carrying opus:K-0014, sol:K-0008, sol:K-0837, opus:K-0869 [p388,p433]: R3 and R3n with B1 and B2 exchanged. B2 = B1 + size (or B1 + $100000 for boards under 1 MB), with the whole board inside the space. The page predicts that the board answers at B2 and not at B1: "The software then tells each board where to reside in memory" (p.391); "the low order register takes bits 23-16" (p.435). Where no such B2 satisfies §4 (an 8 MB board, which can only sit at $00200000; a 4 MB board, whose only multiple of its size inside the space is $00400000), the contrast is unavailable; the board size and the missing second base are recorded, not graded.
- Green controls for P3-NOTOTHER and P3-BASE, on the same "R2 ≠ P2, or no answer" comparator, with live inputs proven to go green: the board configured at B1, read at $00A00000 (Figure K-1, p.386: Zorro II I/O expansion space, above the 8 MB memory space the board can occupy); and the board left unconfigured (steps c and d skipped, as R3u), read with R3n's own write/readback pair at $00400000. Both are recorded with their per-core result (§8.4).
- For IMG1 = IMG0 (run R3c): step b writes step c's byte to $00E80048 in place of the 4A write, and IMG1 is read as the last action (§2.2). The page predicts IMG1 ≠ IMG0, or "no answer": "writing to register 48 actually configures the board" (p.436).
- For IMG2 ≠ IMG0 (run R3u): R3 without the write to 48 of step c and without step d. The page predicts IMG2 = IMG0: until register 48 is written the board is not configured (p.436) and stays in configuration space.
- For G: R_G'.
- For the memory flag: an I/O board runs only the IMG part.

**7. Reference frame.** The register pairs of Figure K-14 and the complement of p.432. G3 checks the anchor: the reserved registers read physical 1111, which confirms the pair mapping and the complement on this PIC.

#### P4: Register 4A's nybble supplies A19-A16 (K-0840)

Not specified: K-0840 is declined (§1). The table that assigns A19-A16 to a nybble at 4A is headed "For Zorro III boards configured in the Zorro II space" (p.435); for a Zorro II board the assignment is a derivation the page does not print. The harness offers only Zorro II fast RAM. P3 writes the base consistently through 4A and 48 (§4 Writes) and grades nothing that depends on which of the two a board uses.

#### P5: Register 46's nybble supplies A27-A24 (K-0838)

Not specified: K-0838 is declined (§1). A nybble written to register 46 supplies A27-A24 only for "Zorro III boards configured in the Zorro II space" (p.435), and only a CPU that drives A27-A24 can reach such a base. The harness offers only Zorro II fast RAM, and nanomig.v's memory address ports are 24 bits wide.

#### P6: RESET returns a configured PIC to the unconfigured state

**1. Proposition.** RESET asserts /BUSRST, on which every expansion device resets its autoconfiguration logic. A PIC configured before RESET therefore answers again, unconfigured, in the configuration space afterwards.

p.394: RESET "will drive only /RST and /BUSRST on the bus". P6 carries no claim id; its expected values are part (iii) of P7, which carries opus:K-0164 and sol:K-0155.

Supporting pages:

- p.394: "All expansion devices are required to reset their autoconfiguration logic when /BUSRST is asserted."
- p.431: "When an Amiga powers up or resets, every card in the system goes to its unconfigured state."
- p.223: "All memory and AUTOCONFIG(TM) cards disappear".

**2. Stimulus (runs R6 and R6m, identical up to the last action of the tail).**

a. §2.1 environment, and P1 steps a-d.

b. Read IMG0. Configure at B1 with the two writes of P3 steps b and c (4A, then 48). Configuration space is not read again before RESET: after the write to 48 it may be unanswered (§2.2). For a memory board, write P0 to B1, write the scratch word, and read B1 into R0.

c. The RESET sequence of §2.3. Its tail is MOVE.W #V2,MK2, then the last action (§2.2):
   - R6: read IMG3, store it, and write the completion marker;
   - R6m, memory board only: write P0' = $6996 to B1, write the scratch word, read B1 into R3, and write the completion marker.

   Configuration space is at $00E8xxxx; the overlay is described as "starting at memory location $00000000" (p.223).

**3. Observation.** IMG3 (R6), R3 (R6m) and M2, each as a Chip-port write or "no answer".

**4. Expected.**

- IMG3 = IMG0.
- Memory board: R3 ≠ P0', or "no answer" ("All memory and AUTOCONFIG(TM) cards disappear", p.223).
- Quantifier: "All expansion devices" and "every card". After a reset only the first PIC in the chain answers ("The configuration chain simply ensures that only one unconfigured card will see an asserted /CFGINn at one time", p.431). The test sees that one device and does not establish "every".

**5. Preconditions.**

- G passed.
- The configuration took effect before RESET: for a memory board, R0 = P0 in this run; for an I/O board, P3's IMG2 ≠ IMG0 in run R3, whose configuration writes are the same.
- P1's reset witness: at least one LED change in [t_M1, t_C2].
- M2 appears at the Chip-memory port, and the CPU's instruction fetches from A+4 onward appear there. That is, Chip memory answers at the program's addresses after RESET. The page does not promise this (p.223). If it does not happen, P6 is "not checkable on this system".

**6. Contrasting cases.**

- R6a and R6am: R6 and R6m with RESET replaced by NOP. The page predicts IMG3 ≠ IMG0, or "no answer", because the board stays configured and out of $00E8xxxx (p.435 fn.6; P3's note on a second PIC with an identical image applies), and R3 = P0'. This also removes P6's own reset-witness precondition (§5, "P1's reset witness: at least one LED change in [t_M1, t_C2]"): with RESET replaced by NOP no LED changes, so P6 graded directly against R6a/R6am reports "not graded", not red, through its own clause. P6's contrast is therefore carried by P7 part (iii) instead, whose precondition list (§5.3.5) excludes the reset witness.
- For the reset-witness precondition: R1a.
- For "Chip memory answers after RESET": no page predicts an outcome either way ("On some Amiga models, portions of the RAM still respond. On other models, no RAM responds", p.223), so there is no contrasting stimulus with a page-predicted result. This is stated here rather than invented.

**7. Reference frame.** As P1 and P3.

### 5.3 Combined: the claims of p.394

#### P7: RESET drives only /RST and /BUSRST

**1. Proposition.** The RESET instruction drives both versions of the system reset and not /HLT: it resets the system's I/O devices, leaves the CPU running, and makes the expansion devices reset their autoconfiguration logic.

Claims opus:K-0164 and sol:K-0155, p.394: "an I/O-only reset, such as initiated by the 680x0 RESET instruction, will drive only /RST and /BUSRST on the bus". P7 is the only assertion that carries these two ids. It grades the sentence whole through three parts, each a consequence that a page names:

- (i) /RST, the system reset: P1's reset witness, that is at least one of P1's LED changes (P1a or P1b) in [t_M1, t_C2] of R1. "The bus supplies two versions of the system reset signal" (p.394); the consequences are those of P1 (p.370, p.244, p.354).
- (ii) "only", that is not /HLT: P2's graded expected value, a visible pixel of value C2 in (t_M1, t_V2] of R1. "A full-system reset, such as a powerup reset or a keyboard reset, drives /HLT low as well. PICs that wish to reset the system CPU as well as the bus and I/O devices drive /RST and /HLT" (p.394); "the CPU continues execution with the next instruction" (p.223).
- (iii) /BUSRST: P6's expected values in R6 and R6m, that is IMG3 = IMG0, and for a memory board R3 ≠ P0' or "no answer". "All expansion devices are required to reset their autoconfiguration logic when /BUSRST is asserted" (p.394).

**2. Stimulus.** Runs R1, R6 and R6m, at every CPU setting of §2.1. No run of its own.

**3. Observation.** What P1, P2 and P6 observe in those runs.

**4. Expected.** On a core, P7 passes when parts (i), (ii) and (iii) all hold at every CPU setting that core runs (core A: 68000; core B: 68000, 68010 and 68020), and fails when any part fails at any of them. "680x0" is covered for the settings the harness offers (§7.1).

**5. Preconditions.**

- For parts (i) and (ii): P1's preconditions (the two LED calibrations, the display anchor, no exception, the V1 and V0 markers).
- For part (iii): gate G, and P6's preconditions other than the reset witness (the configuration took effect before RESET; Chip memory answers after RESET).
- The reset witness is part (i). It is not a precondition of P7, although P2 and P6 use it as one of theirs: as a precondition it would turn a RESET that resets nothing into "not graded".
- If gate G fails (no board, so /BUSRST has no witness), or any other of these preconditions fails at a setting, both ids are withdrawn whole to a record for that run, naming the precondition. They are never graded in part.

**6. Contrasting cases.** One per part; each changes only the run its part reads.

- Part (i): R1a (RESET replaced by NOP in R1). No LED change; part (i) fails while (ii) still holds.
- Part (ii): R1b (the MOVE.W D0,(A0) at A+2 replaced by NOP in R1). No C2 pixel; part (ii) fails while (i) still holds.
- Part (iii): R6a and R6am (RESET replaced by NOP in R6 and R6m). IMG3 ≠ IMG0 or "no answer", and R3 = P0'; part (iii) fails.
- For the withdrawal: `fastram_config` at the harness default 0 ("no Zorro II fast RAM offered"). Gate G fails, and both ids go to a record.

**7. Reference frame.** As P1, P2 and P6.

## 6. What the pages leave open (no expected value; a test must not invent one)

- Which chip-set registers the RESET instruction resets. Agnus has an RST* input (p.381) and Paula has RES*, "System reset" (p.380), but no page says what they reset. Denise's pin list (p.380) shows no reset input. No expected value for any custom register after RESET.
- Whether Chip memory answers after RESET, and where the ROM overlay ends (p.223).
- The duration of the reset: DRESB- goes low for "approximately 17 microseconds" (p.370).
- "some bus devices such as processor cards may internally reset only on full-system resets" (p.394): "may", and not applicable here.
- The width of a PIC's base-address register, nybble, byte or word (p.431).
- What a CPU access to an address that no device answers does (§2.2).
- The polarity of OVL (§7.10). The program drives OVL to 0 only to reach the vectors (§2.1), the vector-table check confirms that RAM answers there, and no expected value depends on the polarity.
- What nanomig.v's `cpu_nrst_out` output carries. No page identifies it, so it has no expected value; its level during RESET may be recorded as a measurement, labelled as such (§1).
- How long a full-system reset takes before the program starts again (p.370 gives DRESB- "approximately one second" for power-up). P2's V0 count after t_M1 is therefore recorded, not graded.
- The ROM's first word: "The value of the ID code may change in the future" (p.223). Not used.
- Any drive other than drive 0 (p.244 "All drive motors"): no LED is seen for it.

## 7. Page conflicts and ambiguities

1. **Scope of the RESET statement.** p.223 speaks of "The 68000 RESET instruction", p.394 of "the 680x0 RESET instruction". The two agree for the 68000. For the 68010 and 68020 only p.394 speaks, and P1, P2, P6 and P7 follow it.
2. **"All memory".** p.223, "All memory and AUTOCONFIG(TM) cards disappear", can be read as all memory cards or as all RAM. The same page says some models keep portions of RAM after reset and others none. Not graded.
3. **Data lines of a read.** p.431 says read registers return data "on D31-D28 for either bus mode", which is the top of the A3000's 32-bit bus. p.432 speaks of "the upper nybbles of locations". On this system's 16-bit bus that is D15-D12 of a word read, and §4 uses that interpretation.
4. **Figure number.** p.432 refers to "Figure 9-10", but the figure is captioned "Figure K-14".
5. **Scope of the nybble/byte table.** p.435's table is headed for Zorro III boards in the Zorro II space. For Zorro II boards its 48/4A rows are a derivation, not printed, so K-0840 is declined (§1); P3's stimulus writes the base through both registers (§4) and grades nothing that depends on the width. Its 46/44 rows (K-0838) need a Zorro III board and are declined (§1).
6. **Registers 30 and 40.** p.435 marks them "Unsupported base register reset register under Zorro II." and "Unsupported control state register under Zorro II.", with footnotes 6 and 7 ("IBID"). Footnote 6: "The original Zorro specifications called for a few registers, like these, that remained active after configuration." They are not used.
7. **Register 08 bit 7.** Its meaning differs between Zorro II ("place the card in the 8 megabyte Zorro II space") and Zorro III ("the board is basically a memory device") (p.433). Only the memory flag of register 00 is used.
8. **Alignment.** Natural alignment is stated only for new-sized Zorro III cards (p.433). An 8 MB Zorro II board in $00200000-$009FFFFF (p.388) cannot be naturally aligned, so these pages give no alignment rule for Zorro II boards.
9. **CIA name on p.244.** The printed p.244 says "Bits labeled "PB" are output bits located in CIAAPRB ($BFD100)". The name says CIA-A, but $BFD100 is CIA-B's prb (p.340), and the page's own heading reads "CIAAPRA/CIABPRB". The specification uses $BFD100 with p.340's bit layout.
10. **OVL polarity.** No page prints it (p.223, p.339, p.351). It can only be inferred: after reset the port pins read high (p.354) and the overlay is present (p.223), so OVL = 1 keeps the overlay and 0 disables it. §2.1 drives OVL to 0 to reach the vectors and checks the result at the port; no expected value relies on the inference.
11. **Write bit position.** No page shows which bits of a written byte a nybble-wide configuration register takes. §4 assumes bits 7-4, like the reads.

## 8. What the build added

### 8.1 Harness configuration

Every run sets `fastram_config` = 1 (the harness documents only its
default, 0, "no Zorro II fast RAM offered"); Gate G's own read of
register 00 is the measurement of what this setting produces (§8.3: type
= Zorro II, size field = 6, the p.433 "Unextended" code for 2 MB).
`floppy_config` is left at the harness's own documented default, 0, "one
drive (DF0), write-enabled, normal speed" -- the propositions need only
drive 0. Every run of P1, P2 and P6 (R1, R6 and R6m, and their contrasts)
is repeated at `cpu_config` = 0 on core A (fx68k) and at `cpu_config` = 0,
1 and 3 on core B (TG68K) -- the harness's own documented `cpu_config`
values ("00 = 68000, 01 = 68010, 11 = 68020"). Gate G and P3 run once, at
`cpu_config` = 0, on each core; they are not in P1/P2/P6's CPU-setting
list (§2.1).

The 2 MB board puts B1 = $00200000 and, where R3/R3n and their contrasts
need a second base, B2 = B1 + size = $00400000, both multiples of the
board's size inside the standard 8 MB Zorro II space (p.388), per §4's
"Choice of base address".

"Wait one frame" (P1's power-LED calibration and the two-frame anchor
wait, and P6's reuse of P1 steps a-d) polls INTREQR bit 5 (VERTB, "start
of vertical blank", printed page 280, Appendix A Register Summary) after
clearing it: a page-documented event, not an assumed cycle count.

### 8.2 Programs

- R1 and its contrasts: `prog_r1.s` (R1); `prog_r1a.s` (R1a, RESET
  replaced by NOP); `prog_r1b.s` (R1b, the A+2 MOVE replaced by NOP);
  `prog_r1_c1prime.s` (the display-anchor contrast, COLOR00 preset to
  C1'=$0C3F); `prog_r1_priv.s` (the "no exception" contrast, SR set to
  user mode before RESET).
- Gate G and its contrast: `prog_rg.s` (R_G); `prog_rg_shutup.s` (R_G').
- P3: `prog_r3.s` (R3, and R3c/R3u/R3' select from it via an assembly
  symbol); `prog_r3_n.s` (R3n); `prog_r3_swap.s` (R3'); `prog_r3_swap_n.s`
  (R3n'); `prog_r3_c48first.s` (R3c); `prog_r3_nocfg.s` (R3u);
  `prog_r3_no48.s` (P3-CONTENT's mutation: the 48 write left out, step d
  kept); `prog_r3_io.s` (green control: configured normally, read at
  $00A00000); `prog_r3_unconf_n.s` (green control: left unconfigured,
  read at $00400000 with R3n's own ending).
- P6: `prog_r6.s` (R6, and R6a select from it); `prog_r6m.s` (R6m);
  `prog_r6a.s` (R6a); `prog_r6am.s` (R6am).

Every program shares one environment (interrupts and DMA cleared, the ROM
overlay cleared, the exception vectors installed and the vector-table
check, the M0 marker) through a package-local macro (`common.i`), applied
once at the start of every run, including gate G's and P3's.

### 8.3 Measured values (not oracles)

- A frame, on this harness with this configuration, measures 568408
  `clk_sys` cycles (from the captured video trace's vsync edges); used
  only to size a run's cycle budget, never as an expected value.
- P2's V0-after-M1 write count: 0, on every CPU setting, both cores
  (recorded at P2, not graded: p.370 gives no bound on a full-system
  reset's own restart time, and RESET is not one).
- Gate G's own register 00 read: type = Zorro II (bits 7-6 = 11), memory
  flag set, size field = 6 (2 MB, p.433's "Unextended" column); registers
  0C, 34, 38 and 3C read physical 1111 at both pair addresses, as p.432's
  own example and p.434-435 state for the reserved registers.
- The second-vsync window of section 2.2 ("no CPU access ... between the
  access and the second vertical-sync leading edge after it") is checked
  against the video trace's own vsync edges, not assumed from a fixed
  cycle count: a run's completion marker (when one appears) trivially
  satisfies it, and where none appears the run is confirmed to have
  covered at least two vsync edges before concluding "no answer". The
  cycle count a run is given remains a safety ceiling only, never the
  oracle.
- Power-LED calibration settle: the CIAAPRA write that switches the LED
  and the MK_CALBRIGHT marker write that follows it are two separate
  instructions; measured skew on this RTL is under 100 `clk_sys` cycles
  (the port's own transition lands about 77 cycles before the marker's
  write reaches the Chip-memory port). The 500-cycle tolerance the
  calibration check allows around the marker absorbs this settle without
  masking a real instability elsewhere in either phase.

### 8.4 Mutations

Every mutation below was run once on both cores (`NMH_MUTATE=<value>`)
and compared against its record; results agreed between fx68k and TG68K
in every case.

| assertion(s) fed | `NMH_MUTATE` | predicted | measured, both cores |
|---|---|---|---|
| P1b, P7 part (i) | `P1A` | R1a (RESET replaced by NOP): the unchanged "power LED leaves bright" comparator goes red -- no LED change | P1b: FAIL (no LED change observed). P7: FAIL through part (i) |
| P2, P7 part (ii) | `P2` | R1b (the A+2 MOVE replaced by NOP): the unchanged "C2 pixel appears" comparator goes red -- no C2 pixel | P2: FAIL (no C2 pixel observed). P7: FAIL through part (ii) |
| P1-DISPANCHOR | `P1-DISPANCHOR` | COLOR00 preset to C1'=$0C3F; the expected colour stays C1: the unchanged anchor comparator goes red -- F_ref shows C1', not C1 | FAIL |
| P1-NOEXCEPTION | `P1-NOEXCEPTION` | SR set to user mode before RESET, so RESET is a privilege violation: the unchanged "no exception" comparator goes red -- H_E runs | FAIL |
| P3-IMG1 | `P3-IMG1` | register 48 written in step b's place configures immediately: IMG1 != IMG0 | FAIL (differs from IMG0) |
| P3-IMG2 | `P3-IMG2` | steps c and d skipped: IMG2 = IMG0 (never configured) | FAIL (equals IMG0) |
| P3-CONTENT | `P3-CONTENT` | step c (the register-48 write) left out, step d kept: R0/R1 read back at their unanswered default, not P0/P1V, with the population still present | FAIL (does not match P0/P1V) |
| GATE-G | `GATE-G` | the shut-up write skipped (prog_rg.bin re-run in its place): the same PIC answers identically twice | FAIL (identical to IMG0) -- GATE-G is already red on the unmutated stimulus (below); this mutation shows the same red through the same comparator |
| P7 part (iii) | `P6` | R6a/R6am (RESET replaced by NOP), fed to P7 part (iii) only (P6 itself keeps R6/R6m: section 5.2 P6, "6. Contrasting cases"): the board stays configured -- a differing image from IMG0, and the memory board still answers with P0' | P7: FAIL through part (iii). P6 itself: unaffected, still PASS |

Every run above also reprints GATE-G, P3-NOTOTHER and P3-BASE unchanged
(FAIL, FAIL, FAIL): a mutation gated behind `NMH_MUTATE` only changes the
one input it names, never the others.

P3-NOTOTHER and P3-BASE carry no dedicated mutation of their own: both
are already red on the unmutated stimulus (below), so each gets two green
controls instead, on the same "R2 != P2V, or no answer" comparator, run
unconditionally (not gated behind `NMH_MUTATE`) as their own assertions:

| control | comparator input | measured, both cores |
|---|---|---|
| board configured at $00200000, read at $00A00000 | Figure K-1 (p.386): Zorro II I/O expansion space, outside the 8 MB memory space the board can occupy | PASS (no answer) |
| board left unconfigured, read at $00400000 | R3n's own write/readback pair, with steps c and d skipped | PASS (no answer) |

GATE-G measures red on the unmutated stimulus, on both cores: after the
shut-up write, the PIC's register image is unchanged from IMG0, although
its own precondition holds (register 08 bit 6 reads logical 0, "the
board can be shut up"). Its green control is P3-IMG2 (section 5.2 P3, run
R3), on the same "differs from IMG0, or no answer" comparator, which
passes.

P3-NOTOTHER and P3-BASE measure red on the unmutated stimulus, on both
cores: the memory board answers with what was written at the base it was
not given, as well as at the base it was given.
