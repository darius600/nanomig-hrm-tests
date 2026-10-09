# Specification: the RESET instruction and AUTOCONFIG base-address assignment

This specification was written from the package's claims (`claims-spec-stage.jsonl`) and the pages of the *Amiga Hardware Reference Manual*, 3rd edition, by an author who never saw the implementation or any test. Page numbers are printed page numbers. Quotations are from the printed pages. Pp. 46, 48, 123, 157, 223, 244, 263, 307, 339, 340, 351, 354, 370, 380, 386, 394, 432, 433 and 435 were checked against the page image, because their text copy is re-edited or damaged or because the quotation carries weight. Where the specification relies on 680x0 behaviour that the manual does not describe, it says so ("Motorola, not the manual").

## 1. Claims not turned into a proposition, or used only in part

| Claim | What is not used | Reason |
|---|---|---|
| opus:K-0164, sol:K-0155 (p.394) | Everything: both ids are withdrawn whole to a record (§5.3). | The claims state the levels of the expansion-bus lines /RST, /BUSRST and /HLT. No expansion-bus line is an output of this boundary, and no page identifies nanomig.v's `cpu_nrst_out` output with /RST, /BUSRST, /HLT or any other signal. The consequences that other pages name are graded without these ids: the system reset (P1b), the CPU continuing (P2) and a PIC returning to the unconfigured state (P6). A downstream consequence does not establish a claimed bus-line level, and a transient /HLT level is not excluded by any of them. |
| opus:K-0014, sol:K-0008 (p.384) | Everything: both ids are withdrawn whole to a record. | Each claim states two things: that addressing jumpers were eliminated and that the card's address is instead assigned by software. This system has no jumper whose absence could be observed, and P3-NOTOTHER and P3-BASE establish only the software assignment of the base address; they carry no id of these two claims. |
| K-0840 (p.435) | Everything. | The table that holds the claim is headed "For Zorro III boards configured in the Zorro II space". Applying it to a Zorro II board is a derivation the page does not print, and the only expansion device the harness offers is Zorro II fast RAM (§5.2). P4 is therefore not specified. |
| P3 and P6 | Everything, unless an AUTOCONFIG device is present. | The system under test is the chip set, a 680x0 CPU and Chip memory. The harness offers one expansion device, Zorro II fast RAM, through the `fastram_config` input, whose default 0 means "no Zorro II fast RAM offered" (`nanomig_harness.h`, `Config`). Every run of gate G, P3 and P6 sets `fastram_config` to a value that offers it (§5.2). P3 and P6 need a PIC that answers in the Zorro II configuration space $00E8xxxx. Gate G (§5.2) decides whether one does. If none does, P3 and P6 are reported "not checkable: no AUTOCONFIG device", never as passed. |
| K-0838 (p.435) | Everything. | Not testable at this boundary. The table that holds the claim is headed "For Zorro III boards configured in the Zorro II space", so the claim needs a Zorro III PIC (type 10) and a CPU that drives A27-A24. The only expansion device the harness offers is "Zorro II fast RAM" (`Config.fastram_config`, `nanomig_harness.h`). nanomig.v's memory address ports are `ram_address[23:1]` and `fastram_addr[23:1]`, and the harness's `cpu_config` settings change the core's behaviour, not its bus (§2.1). A27-A24 do not exist on a 24-bit bus: the Zorro II bus "provides 24 bits of addressing information" (p.391). |

The claims sol:K-0837 and opus:K-0869 (p.435, "the low order register takes bits 23-16") are graded by P3-NOTOTHER and P3-BASE (§5.2). The base-address comparator the sentence describes is observable at this boundary, through where the board answers.

No claim of this package needs data read from or written to a disk surface.

Propositions:

| | Proposition | Needs |
|---|---|---|
| P1 | RESET asserts the system reset (drive LED and power LED, graded separately as P1a and P1b) | nothing beyond the system |
| P2 | RESET does not reset the CPU; the next instruction executes | nothing beyond the system |
| P3 | A PIC's base address is the one software writes; P3-NOTOTHER and P3-BASE carry sol:K-0837 and opus:K-0869 | a PIC (gate G) |
| P4 | Register 4A's nybble supplies A19-A16: declined (§1, K-0840) | a Zorro III PIC in the Zorro II block, which this boundary does not offer |
| P5 | Register 46's nybble supplies A27-A24: declined (§1, K-0838) | a Zorro III PIC and a 32-bit address bus, which this boundary does not offer |
| P6 | RESET returns a configured PIC to the unconfigured state | a PIC, and Chip memory answering after RESET |
| P7 | RESET drives only /RST and /BUSRST: withdrawn whole to a record (§5.3), opus:K-0164 and sol:K-0155 | not applicable |

## 2. Conventions for every run

### 2.1 Program environment

- The program runs in supervisor mode with SR = $2700 from its first instruction.
- It writes $7FFF to INTENA ($DFF09A) and to DMACON ($DFF096). With SET/CLR = 0, this clears every interrupt enable and every DMA enable. Custom-register addresses are "680x0 address = (chip address) + $DFF000" (p.263).
- ROM overlay: after reset "An additional copy of the system ROM responds starting at memory location $00000000. The system RAM that would normally be located at this address is not available", and "The OVL bit in one of the 8520 Chips disables the overlay" (p.223). OVL is CIA-A pra bit 0 (p.339). Before it writes a vector, the program writes ddra ($BFE201) ← $03 ("Direction for port A (BFE001);1=output (set to 0x03)", p.339), then pra ($BFE001) ← $02: bit 0 (OVL) = 0, bit 1 (/LED) = 1. No page prints which OVL level disables the overlay; 0 is the level §7.10 infers, and the vector-table check below confirms that RAM answers at the vectors. Every later write to pra keeps bit 0 at 0.
- Exception vectors 2, 3, 4, 8, 10 and 11 (addresses $000008, $00000C, $000010, $000020, $000028, $00002C) point to a handler H_E, which writes C_E to COLOR00, writes V_E to chip word MK_E, and then loops. The vector numbers are Motorola, not the manual. With the 68010 and 68020 settings, VBR = 0. With the 68020 setting the instruction cache stays disabled (CACR = 0), so every instruction fetch reaches the bus (Motorola, not the manual).
- Vector-table check: the program writes H_E's address into $000020/$000022, reads it back and stores what it read. The check is that the long reads back as written; it does not depend on the RAM-port address at which the harness places $000020. This shows that RAM, not the ROM overlay (p.223), answers at the vectors. A failed check is a fixture failure and stops the package: nothing is graded.
- Placement: the harness loads the program at PROGRAM_ORG = $080400, sets the supervisor stack to PROGRAM_STACK = $0FFF00, and keeps results at RESULTS = $0C0000 (`nanomig.i`). All three lie in $080000-$1FFFFF, which the harness maps one-to-one. MK0, MK1, MK2, MK_E, the scratch word and every stored result are distinct words in $0C0000-$0C0FFD. The completion marker is `nanomig.i`'s DONE (DONE_VALUE $600D to DONE_ADDR $0C0FFE). Below $080000 the program writes only the vectors. p.223 says the overlay ROM copy "responds starting at memory location $00000000" but not where it ends, so nothing assumes that the program's addresses answer after RESET (§2.3, P6's preconditions).
- Chip-memory port: nanomig.v's RAM port (`ram_address`, `ram_data`, `_ram_we`, `_ram_oe`, `_ram_bhe`, `_ram_ble`), logged by the harness's `ram_log()`. It carries more than Chip memory: the harness keeps the overlay's ROM copy on it at physical byte address $780000 (`nanomig_harness.h`), an expansion board's memory does not appear on it: the CPU reaches the board through nanomig.v's `fastram_*` port (`cpu_wrapper.v` `sel_z2ram`, `nanomig.v` `fastram_sel`), which the harness serves from its own model and does not log. A Chip-memory address on it is a byte address below $200000, the 2 MB of Chip memory the harness configures (`Config.memory_config` = $03); the harness maps $080000-$1FFFFF one-to-one.
- Marker M0: the first action of the program writes V0 to MK0. All results are CPU writes to chip words, which the test sees at the Chip-memory port with address and data.
- CPU settings: every run of P1, P2 and P6 (R1, R6 and R6m, and their contrasts) is repeated for core A (68000), core B as 68000, core B as 68010 and core B as 68020, because p.394 speaks of "the 680x0 RESET instruction". The harness's `cpu_config` selects the core's behaviour, not its bus: with `turbo_config` = 0 "every CPU access uses the slow chip bus" (`nanomig_harness.h`), the 16-bit RAM port, on every setting. PAL and ECS are the defaults, and no page makes these propositions depend on either.
- Every program is assembled for the 68000, vasm's default, on every CPU setting. No stimulus needs a later instruction, and with `-m68010` or higher vasm assembles `MOVE #0,<ea>` as `CLR <ea>`, which on the 68000 reads before it writes (the zero writes to BPLCON0, SPRxCTL, the scratch word, and a zero nybble or byte to a configuration register).
- Floppy drive: the harness default `floppy_config` = 0 is documented as "one drive (DF0), write-enabled, normal speed" (`nanomig_harness.h`, `Config`). Every run that performs P1 step c uses this default; it is harness setup, not an expected value. P1's drive-LED calibration shows at the port whether the drive latches the motor. No disk image is needed.
- Colour values: C1 = $0C30, C2 = $03AC, C_E = $0F0F. Bits 15-12 are zero ("Unused", p.46), the three values are pairwise different, and none is $000. The builder may choose others with these properties.

### 2.2 Unanswered-address rule

No page states what a CPU access to an address that no device answers does. p.385 names a time-out only for Zorro III cycles: "the cycle completes when a Zorro III slave responds or the bus times out, as driven by the motherboard logic". So every access to an address that may be unanswered is the last bus action of its run:

- all other results are stored first;
- in gate G's shut-up run and every P3 run, a distinct pre-access marker is written to Chip memory immediately before the access (the word PRE of the program, one marker value per kind of final access), so the window below is anchored at the access and not at the start of the run; R_G, R6 and R6m carry none, and are graded only when the completion marker appears (G1, P6-CHIPMEM);
- a completion marker follows the access;
- if the completion marker never appears, the run records "no answer" for that access. "Never appears" means no completion marker at the Chip-memory port within the second vertical-sync leading edge counted from the pre-access marker (t_N, §3), a harness limit rather than a page value. It is counted in frames on the video output, so no conversion from `clk_sys` to time is needed. A completion marker that appears earlier ends the run as DONE, and the line grades the stored result; the printed label and a MEASURED line name which of the two ended the run, DONE or expiry of the window.

A run is accepted as "no answer" only when all of these hold, each measured in the run and none assumed:

- the pre-access marker was written, exactly once;
- the M0 marker was written exactly once (the program did not restart), and no exception handler ran (no write of V_E to MK_E, no pixel of C_E);
- after the pre-access marker the Chip-memory port saw no write other than the program's scratch write, and the `fastram_*` port, which serves the expansion board, saw no access (`fastram_sel` stays low): no result was stored, no completion marker appeared and the board took no access. CPU instruction fetches at the Chip-memory port are not examined;
- no equal result was stored, even if the completion marker is absent (for a write/readback, no store of the written value into the R2 slot; for an image read, an image equal to IMG0 in the IMG slot);
- where the program has a scratch write between the access and its readback, that write appears after the pre-access marker;
- the window expired.

Where one of these fails, the "no answer" alternative of the line is false. A run that wrote neither the completion marker nor the pre-access marker, or whose window had not expired, did not run past the access and is not graded.

This covers configuration space before gate G has passed, a base address before its board is configured, and an address a board has been moved away from, including configuration space once its board has been configured by the write to register 48 or shut up. Because of this rule, several checks below need a run of their own.

### 2.3 The RESET sequence (used by P1, P2 and P6)

Before the sequence: A0 = $00DFF180 (COLOR00), D0 = C2, A2 = MK1, D1 = V1. Then three one-word instructions at consecutive addresses, with RESET at a long-word-aligned address A:

```
A-2:  MOVE.W D1,(A2)    ; marker V1 -> MK1; its Chip-port write is t_M1
A:    RESET
A+2:  MOVE.W D0,(A0)    ; C2 -> COLOR00
A+4:  tail              ; MOVE.W #V2,MK2, then the completion marker, then a branch to itself
                        ; (in P6: the post-RESET reads of section 5.2)
```

Why this shape: p.223 warns "Since RAM may not be available, special care is needed to write reboot code that will reliably reboot all Amiga models". Before that it says, of the reset state, "On some Amiga models, portions of the RAM still respond. On other models, no RAM responds. Software must assume that memory is not available."

On a 68000 the instruction at A+2 is fetched before RESET executes, by the two-word prefetch (Motorola, not the manual). The harness's 68010 and 68020 settings do not change the bus (§2.1): every setting fetches over the 16-bit RAM port, and the long-word fetch of a real 68020 is not assumed. The instruction at A+2 needs no memory to execute, because its source is a register and its destination a custom register, and its effect is visible on the video output.

The tail runs only if Chip memory answers at A+4 onward after RESET. P1 and P2 depend on nothing in the tail. A run ends at t_V2 (§3) whether or not M2 appears.

## 3. Reference frames and their anchors

- **t_M1**: the time of the Chip-port write of V1 to MK1. Anchor check: exactly one such write in the run.
- **Frames**: a frame lies between two consecutive leading edges of vertical sync on the video output.
- **t_V2**: the second vertical-sync leading edge after t_M1, so (t_M1, t_V2] contains at least one complete frame. The only duration a page gives is "approximately 17 microseconds" (p.370, the DRESB- pulse for a RESET instruction); t_V2 is a harness bound that contains it many times over.
- **t_N**: the second vertical-sync leading edge after a run's pre-access marker (§2.2), so a window of at least one complete frame follows the final risky access. It bounds "no answer" for gate G's shut-up run and for every P3 run; a run without a pre-access marker has no t_N.
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

p.394: "an I/O-only reset, such as initiated by the 680x0 RESET instruction, will drive only /RST and /BUSRST on the bus". The same page says of these two: "The bus supplies two versions of the system reset signal." P1 carries no claim id.

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
- P1b: the power LED is L_bright at t_M1. Its first change after t_M1 goes to L_dim and lies in [t_M1, t_C2]. L_bright and L_dim are the levels calibrated by the run itself (the power-LED calibration below), so the target of the change is compared with the calibrated L_dim and no polarity of the port is assumed.
  - Source: the port pins become inputs and are pulled high (p.354), and /LED = 1 is not bright (p.351).
- Why the window ends at t_C2: the instruction at A+2 runs once RESET has finished ("the CPU continues execution with the next instruction", p.223), so the reset's effects come before its visible result. Levels after t_C2 are not graded, because what the CPU fetches after A+2 is not guaranteed to be the program (§2.3).

**5. Preconditions.** Each is printed on its own line, over every CPU setting, before the proposition that rests on it. Where one prints FAIL, the proposition is recorded as not graded and is never printed as PASS or FAIL.

- P1-LEDCAL, power-LED calibration: the power LED makes exactly one transition in (t_CALDIM, t_M1), where t_CALDIM is the Chip-port write of the dim-phase marker; the level before that transition is L_dim (pra bit 1 = 1), the level after it is L_bright (pra bit 1 = 0), and L_bright ≠ L_dim. No timing margin around the bright-phase marker is assumed.
- P1a-FDDCAL, drive-LED calibration: after the "off" latch the LED is L_off, and after the "on" latch it is L_on while prb = $FF (so the /MTR line itself is high); L_on ≠ L_off. This shows the drive LED shows the latched motor state, which is what p.244 says a system reset turns off. The run prints it as a MEASURED line before P1a, from the drive-LED trace between the bright-phase marker and t_M1 (whether the drive-LED port changes level at least once on every CPU setting), and a MEASURED line gives the level changes per setting. It calibrates P1a's observable and grades no claim: P1a has no assertion here, so no graded line rests on it, and it carries no MUTATION record. P1a is recorded as not graded, and the record states what the calibration measured: in the unmutated run the drive-LED port does not follow the motor latch (the p.244 sentence is graded as opus:8-cia.c26 / sol:8-cia.c24 in hrm/ch8-drive-select-motor-led, NM-HRM-046).
- P1-DISPANCHOR, display anchor: F_ref as in §3.
- P1-NOEXCEPTION, no exception: no pixel of value C_E and no write of V_E to MK_E anywhere in the run.
- P1-MARKERS: the run loads, t_V2 is found, the write V1→MK1 appears exactly once, and the write V0→MK0 appears exactly once before t_M1. Writes of V0 after t_M1 are P2's record, not a precondition.

**6. Contrasting cases.**

- R1a: RESET at A replaced by NOP ($4E71), everything else unchanged. The page predicts no system reset: both LEDs keep L_on and L_bright from t_M1 to t_V2. C2 appears (P2), the tail runs, M2 appears and the program finishes.
- For the power-LED calibration: its two halves are the contrast (bit 1 = 1 against 0, p.351). The contrast program writes bit 1 = 1 in place of the write that switches to bright, so both phases show L_dim, and replaces RESET by NOP so that the run completes.
- For the drive-LED calibration: latching with /MTR high against low (p.244).
- For the display anchor: C1 replaced by C1' = $0C3F in step a, and RESET replaced by NOP so that the run completes. F_ref then shows C1'. F_ref ends before t_M1, so the NOP does not reach the anchor comparator.
- For "no exception": unavailable from this oracle. The only contrast, RESET executed in user mode, rests on 680x0 privilege-violation behaviour that no cited page predicts (Motorola, not the manual).
- For the markers: no page predicts a contrast; the uniqueness of the V1 write and of the V0 write before t_M1 are apparatus anchors.

**7. Reference frame.** t_M1, t_C2 and the calibrated LED levels (§3). The anchors are checked by the calibrations and by the uniqueness of the V1 write.

#### P2: RESET does not reset the CPU; execution continues with the next instruction

**1. Proposition.** RESET is an I/O-only reset: it leaves the CPU running, and the CPU executes the instruction that follows RESET.

p.394: "an I/O-only reset, such as initiated by the 680x0 RESET instruction, will drive only /RST and /BUSRST on the bus. A full-system reset, such as a powerup reset or a keyboard reset, drives /HLT low as well. PICs that wish to reset the system CPU as well as the bus and I/O devices drive /RST and /HLT". P2 carries no claim id.

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

**5. Preconditions.** Printed before P2, each on its own line.

- All of P1's preconditions (P1-MARKERS, P1-LEDCAL, P1-DISPANCHOR, P1-NOEXCEPTION).
- P2-WITNESS: a system reset took place in this run: at least one of P1's LED changes is observed in [t_M1, t_C2]. Without it, a C2 pixel shows only that an instruction following a RESET that did nothing was executed. P2 is then reported "not graded: precondition failed".

**6. Contrasting cases.**

- No stimulus of this boundary contradicts the clause while keeping its witness. p.223 predicts that the instruction after RESET executes, so a program whose instruction at A+2 is replaced by NOP leaves the clause true and removes only the witness (the C2 write). The clause is unfalsifiable by a live page-predicted stimulus at this boundary, and P2 carries no mutation.
- For the recorded V0 count there is no contrasting input inside (t_M1, t_V2]. The full-system reset that p.394 contrasts with RESET would start the program again from the reset vectors ("The next two words are used as the initial program counter by the 680x0 processor", p.223), and nanomig.v has a `reset` input, which the harness drives in `boot()` to start the system. But no page says how long the system takes to restart after it: p.370 gives DRESB- "approximately one second" for power-up and "as long as keyboard reset is held down" for a keyboard reset. The restart may fall after t_V2, and a longer window would rest on no page value. The harness sends key codes, not the keyboard hard reset, which is KCLK held low for at least 500 ms (p.361, "Valid for all keyboards except the Amiga 500").
- For P2-WITNESS: R1a (P1's contrast) shows this precondition failing.

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

The run prints G as GATE-G-PRESENCE, PASS or FAIL, before GATE-G-PRECOND. A mutation for it reads 32 words that no AUTOCONFIG device answers in place of $00E80000 (`prog_rg_nodev.s`: the program zeroes 32 words of its own results page and reads them), on the unchanged G1-G3 comparator.

The PIC must still be unconfigured when the program starts. If the harness's start-up has already configured it, G fails, and the builder records that as the reason.

GATE-G (run R_G'): writes $00 to the shut-up address $00E8004C, then reads IMG as the last action (§2.2), on the same "differs from IMG0, or no answer" comparator P3-IMG2 uses. "Anything written to 4C will cause a board that supports shut-up to completely disappear until the next reset" (p.436).

GATE-G's precondition GATE-G-PRECOND, printed before GATE-G: register 08 bit 6 reads logical 0, "the board can be shut up" (p.433, §4). It is printed as PASS or FAIL only when gate G supplied a register image; where gate G failed it is recorded as unavailable. Where it does not hold, or gate G failed, GATE-G is recorded "not graded" and the shut-up run is not graded.

GATE-G's green control, on the same comparator, with a live input proven to go green: P3-IMG2 (§5.2 P3, run R3), which differs from IMG0 after the configuring write to register 48 (p.435 fn.6, p.431).

If G fails, P3 and P6 are "not checkable: no AUTOCONFIG device answers in $00E8xxxx".

#### P3: Software assigns the PIC's base address

**1. Proposition.** An unconfigured Zorro II PIC takes the base address that software writes to its base-address register. After the final write it leaves the configuration space and answers at that address.

p.384: "the card's address instead being assigned by software". P3 itself carries no claim id; its two sub-checks P3-NOTOTHER and P3-BASE (below) carry sol:K-0837 and opus:K-0869 (p.435, "the low order register takes bits 23-16"). They carry no id of opus:K-0014 or sol:K-0008, which are withdrawn whole (§1).

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
- P3-NOTOTHER-ASSIGNED, memory board, no claim id: in run R3n, R0 = P0 and R1 = P1 at B1, the base written to register 48.
- P3-NOTOTHER, memory board, carrying sol:K-0837, opus:K-0869 [p391,p433-435]: the aggregate R0 = P0 and R1 = P1 at B1 and (R2 ≠ P2, or "no answer"), in run R3n. "The software then tells each board where to reside in memory" (p.391); "the low order register takes bits 23-16" (p.435), read through the base-address comparator of p.433-434 ("110 2 megabytes" register 00 size field pairs the base with the board's size). The board answers at the base it was given and does not also answer at the base it was not given.
- Quantifier: "the card's address" is a statement about every card. The test covers the PIC or PICs present, with two bases (R3 and R3').

**5. Preconditions.** Each is printed on its own line before the propositions that rest on it (P3-IMG-PRECOND, P3-CONTENT-PRECOND, P3-NOTOTHER-PRECOND, P3-BASE-PRECOND, P3-CONTROL-IO-PRECOND, P3-CONTROL-UNCONF-PRECOND); where one prints FAIL, the propositions after it are recorded as not graded.

- G passed.
- Every run lets register 00 be read as type 11 (Zorro II), memory flag set and size field 110 (2 megabytes, the "Unextended" column of p.433), before the two 2 MB bases B1 and B2 are used. G itself accepts type 10 or 11; P3 applies to type 11 only.
- Every run loaded and ran to the completion marker or to the expiry of the no-answer window counted from its pre-access marker (§2.2, t_N).
- B1 and B2 are valid per §4 and do not overlap Chip memory.
- The scratch write appears at the port between the writes to B1 and the reads. For the readback of R2 the scratch write after the pre-access marker is measured in every run, before the store of R2 where one exists. A run in which it does not appear leaves the precondition FAIL and the lines after it not graded: a write that never completes is not an answered write, and it is not assumed to have happened.
- The memory flag has been read. For an I/O board only the IMG expected values are graded, because the page gives no content to read back at its base.
- P3-BASE reports, as P3-BASE-ASSIGNED, that the board answers at the base it was given in the exchanged run (R0 = P0 and R1 = P1 at B2), and grades the aggregate P3-BASE only when both exchanged runs are live.

**6. Contrasting cases.**

- P3-BASE (R3' and R3n'), carrying sol:K-0837, opus:K-0869 [p388,p391,p433,p435]: R3 and R3n with B1 and B2 exchanged. B2 = B1 + size (or B1 + $100000 for boards under 1 MB), with the whole board inside the space. The page predicts that the board answers at B2 and not at B1: "The software then tells each board where to reside in memory" (p.391); "the low order register takes bits 23-16" (p.435). Where no such B2 satisfies §4 (an 8 MB board, which can only sit at $00200000; a 4 MB board, whose only multiple of its size inside the space is $00400000), the contrast is unavailable; the board size and the missing second base are recorded, not graded.
- Green controls for P3-NOTOTHER and P3-BASE, on the same "R2 ≠ P2, or no answer" comparator, with live inputs proven to go green: P3-CONTROL-IO, the board configured at B1, read at $00A00000 (Figure K-1, p.386: Zorro II I/O expansion space, above the 8 MB memory space the board can occupy); and P3-CONTROL-UNCONF, the board left unconfigured (steps c and d skipped, as R3u), read with R3n's own write/readback pair at $00400000. Each is shown red by a stimulus mutation (§8.4): the write/readback aimed at the configured base itself, where a configured memory board answers (p.433). Both controls are recorded with their per-core result (§8.4).
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

p.394: RESET "will drive only /RST and /BUSRST on the bus". P6 carries no claim id.

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

**5. Preconditions.** P6 is graded on a core only when both runs R6 and R6m pass every precondition at every CPU setting that core offers; otherwise it is recorded as not graded, naming the precondition. Each is printed before P6.

- G passed.
- P6-CONFIG: both runs load, no exception handler runs in either, and the configuration took effect before RESET: for a memory board, R0 = P0 in this run; for an I/O board, P3's IMG2 ≠ IMG0 in run R3, whose configuration writes are the same.
- P6-WITNESS: P1's reset witness, observed in runs R6 and R6m themselves (not R1): at least one LED change in (t_M1, t_V2].
- P6-CHIPMEM: M2 appears at the Chip-memory port, and the CPU's instruction fetches from A+4 onward appear there; both runs write the completion marker. That is, Chip memory answers at the program's addresses after RESET. The page does not promise this (p.223). The run prints it as a PASS or FAIL line before P6. If it does not happen, P6 is "not checkable on this system", recorded and not graded.

**6. Contrasting cases.**

- R6a and R6am: R6 and R6m with RESET replaced by NOP. The page predicts IMG3 ≠ IMG0, or "no answer", because the board stays configured and out of $00E8xxxx (p.435 fn.6; P3's note on a second PIC with an identical image applies), and R3 = P0'. This also removes P6's own reset-witness precondition P6-WITNESS: with RESET replaced by NOP no LED changes, so P6 graded directly against R6a/R6am reports "not graded", not red, through its own clause. Both runs complete, and P6-WITNESS prints FAIL on them. P6 itself is unfalsifiable by a live page-predicted stimulus at this boundary and carries no mutation, so it is green with no live mutation: a contrast that keeps the witness would need a RESET that does not drive /BUSRST, which no input of this boundary offers.
- For P6-WITNESS: R6a and R6am (above), and R1a for the same precondition in P2.
- For P6-CONFIG: no contrast is driven in the R6 family; a program that leaves out the write to 48 (P3-CONTENT's contrast) would falsify it.
- For "Chip memory answers after RESET": no page predicts an outcome either way ("On some Amiga models, portions of the RAM still respond. On other models, no RAM responds", p.223), so there is no contrasting stimulus with a page-predicted result. This is stated here rather than invented. The line itself is falsified by a stimulus that is not a page-predicted contrast: R6 and R6m with the instruction stream stopping at the tail (`prog_r6_hang.s`, `prog_r6m_hang.s`), so neither M2 nor the completion marker is written.

**7. Reference frame.** As P1 and P3.

### 5.3 Withdrawn: the claims of p.394

#### P7: RESET drives only /RST and /BUSRST

Claims opus:K-0164 and sol:K-0155, p.394: "an I/O-only reset, such as initiated by the 680x0 RESET instruction, will drive only /RST and /BUSRST on the bus". Both ids are withdrawn whole to a record and are carried by no assertion.

The claims state the levels of three expansion-bus lines: /RST and /BUSRST driven, /HLT not driven. None of the three is an output of this boundary, and no documented top-level observable exists for all three. P1b (the system reset), P2 (the CPU continues) and P6 (a PIC returns to the unconfigured state) grade the consequences that other pages name (p.370, p.244, p.354, p.223, p.394). A consequence does not establish the claimed bus-line level, and a transient /HLT level is not excluded by the CPU continuing. The ids therefore stay on a record; those three lines carry no id.

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

1. **Scope of the RESET statement.** p.223 speaks of "The 68000 RESET instruction", p.394 of "the 680x0 RESET instruction". The two agree for the 68000. For the 68010 and 68020 only p.394 speaks, and P1, P2 and P6 follow it.
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

## 8. Harness configuration, programs, measured values and mutations

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
list (§2.1). Under a mutation selector (`NMH_MUTATE`) the P1, P2 and P6
program sets run at the first setting of the core only.

The 2 MB board puts B1 = $00200000 and, where R3/R3n and their contrasts
need a second base, B2 = B1 + size = $00400000, both multiples of the
board's size inside the standard 8 MB Zorro II space (p.388), per §4's
"Choice of base address".

"Wait one frame" (P1's power-LED calibration and the two-frame anchor
wait, and P6's reuse of P1 steps a-d) polls INTREQR bit 5 (VERTB, "start
of vertical blank", printed page 280, Appendix A Register Summary) after
clearing it: a page-documented event, not an assumed cycle count.

### 8.2 Programs

- R1 and its contrasts: `prog_r1.s` (R1, which ends with the completion
  marker after the tail); `prog_r1a.s` (R1a, RESET replaced by NOP);
  `prog_r1_c1prime.s` (the display-anchor contrast, COLOR00 preset to
  C1'=$0C3F, RESET replaced by NOP); `prog_r1_nocal.s` (the power-LED
  calibration contrast, the write that switches to bright keeps bit 1 = 1,
  RESET replaced by NOP).
- Gate G and its contrasts: `prog_rg.s` (R_G); `prog_rg_shutup.s` (R_G', which writes the pre-access marker B4B4 before its image read); `prog_rg_nodev.s` (GATE-G-PRESENCE's mutation: the image read from 32 zeroed words of the results page).
- Pre-access markers: every final risky access of `prog_r3.s` and its includers is preceded by a write to the word PRE ($0C00CE): $B1B1 before the IMG1 read, $B2B2 before the IMG2 read, $B3B3 before the write/readback at the other base. `prog_rg_shutup.s` writes $B4B4 to the same word before its image read.
- No-answer contrasts (apparatus, not page-predicted): `prog_rg_shutup_hang.s`, `prog_r3_hang_img2.s`, `prog_r3_io_hang.s`, `prog_r3_io_eqstore.s`, `prog_r3_unconf_hang.s`, `prog_r3_n_hang.s` (§8.4).
- P3: `prog_r3.s` (R3, and R3c/R3u/R3' select from it via an assembly
  symbol); `prog_r3_n.s` (R3n); `prog_r3_swap.s` (R3'); `prog_r3_swap_n.s`
  (R3n'); `prog_r3_c48first.s` (R3c); `prog_r3_nocfg.s` (R3u);
  `prog_r3_no48.s` (P3-CONTENT's mutation: the 48 write left out, step d
  kept); `prog_r3_swap_no48.s` (P3-BASE-ASSIGNED's mutation: the same in
  R3'); `prog_r3_io.s` (green control: configured normally, read at
  $00A00000); `prog_r3_io_self.s` (its mutation: read at the configured
  base itself); `prog_r3_unconf_n.s` (green control: left unconfigured,
  read at $00400000 with R3n's own ending); `prog_r3_unconf_self.s` (its
  mutation: configured at $00400000 and read there).
- P6: `prog_r6.s` (R6, and R6a select from it); `prog_r6m.s` (R6m);
  `prog_r6a.s` (R6a); `prog_r6am.s` (R6am); `prog_r6_hang.s` and
  `prog_r6m_hang.s` (P6-CHIPMEM's mutation: the stream stops at the tail).

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
- The no-answer window of section 2.2 is two vertical-sync leading edges
  counted on the run's video trace from the pre-access marker the program
  writes immediately before its final risky access; it is not assumed from a
  fixed cycle count. A run ends at the completion marker (DONE), at the
  expiry of the window, or at a cycle ceiling that is a safety bound only.
  Every line that grades the result of a run's final access (GATE-G, P3-IMG2, P3-NOTOTHER, P3-BASE, P3-CONTROL-IO, P3-CONTROL-UNCONF) names which of the first two ended it, in
  its label and in a MEASURED line. Where the run ended at the window, the
  MEASURED line also gives the provenance of section 2.2: the marker written
  once, no restart, no exception, no Chip-port write after the marker but the
  scratch write, no access at the board's `fastram_*` port, the window expired. On every graded P3 and gate G run of
  both cores the run ended at the completion marker.
- Power-LED calibration: the check counts the power-LED transitions between the dim-phase marker and t_M1 in the port trace and compares the level before with the level after. It carries no tolerance window.
- Every P3 run's register 00 read: type 11, memory flag set, size field 110
  (P3-IMG-PRECOND, P3-CONTENT-PRECOND, P3-NOTOTHER-PRECOND, P3-BASE-PRECOND and
  the two control preconditions print PASS on both cores).
- Drive-LED port, run R1: no level change between the bright-phase marker
  and t_M1 on any CPU setting of either core (P1a-FDDCAL measures "no").

### 8.4 Mutations

Every mutation below was run on both cores (`NMH_MUTATE=<value>`) and
compared against its record. Each mutated program set is printed with a
MEASURED line stating that its run wrote the completion marker. Results
agreed between fx68k and TG68K in every case. "Newly red" means beyond the
three red lines of the unmutated run (GATE-G, P3-NOTOTHER, P3-BASE).

| line(s) fed | `NMH_MUTATE` | stimulus change | measured, both cores |
|---|---|---|---|
| P1b, P2-WITNESS | `P1A` | RESET at A replaced by NOP (R1a): no LED change occurs | P1b: FAIL. P2-WITNESS: FAIL. P2: not graded (P2-WITNESS) |
| P1-LEDCAL | `P1-LEDCAL` | the write that switches the power LED to bright keeps pra bit 1 = 1; RESET replaced by NOP | P1-LEDCAL: FAIL. P1b and P2: not graded (P1-LEDCAL) |
| P1-DISPANCHOR | `P1-DISPANCHOR` | COLOR00 preset to C1'=$0C3F, the expected colour stays C1; RESET replaced by NOP | P1-DISPANCHOR: FAIL. P1b and P2: not graded (P1-DISPANCHOR) |
| P3-IMG1 | `P3-IMG1` | register 48 written in step b's place configures immediately | P3-IMG1: FAIL (IMG1 differs from IMG0) |
| P3-IMG2 | `P3-IMG2` | steps c and d skipped (never configured) | P3-IMG2: FAIL (IMG2 equals IMG0) |
| P3-NOTOTHER-ASSIGNED | `P3-NOTOTHER-ASSIGNED` | step c (the register-48 write) left out of R3n, step d kept | P3-NOTOTHER-ASSIGNED: FAIL (R0/R1 at B1 are not P0/P1V); no other line of the run set moves |
| P3-CONTENT | `P3-CONTENT` | step c (the register-48 write) left out, step d kept | P3-CONTENT: FAIL (R0/R1 are not P0/P1V) |
| P3-BASE-ASSIGNED | `P3-BASE-ASSIGNED` | step c left out of R3', step d kept | P3-BASE-ASSIGNED: FAIL (R0/R1 at B2 are not P0/P1V) |
| P3-CONTROL-IO | `P3-CONTROL-IO` | the write/readback aimed at the configured base B1 itself | P3-CONTROL-IO: FAIL (a configured memory board answers at its base, p.433) |
| P3-CONTROL-UNCONF | `P3-CONTROL-UNCONF` | the board configured at $00400000 and the write/readback aimed there | P3-CONTROL-UNCONF: FAIL (same) |
| P6-WITNESS | `P6` | RESET replaced by NOP in R6 and R6m (R6a, R6am) | P6-WITNESS: FAIL. P6: not graded (P6-WITNESS) |
| GATE-G-PRESENCE | `GATE-G-PRESENCE` | the image read from 32 zeroed words of the results page in place of $00E80000 (`prog_rg_nodev`) | GATE-G-PRESENCE: FAIL (the zeroed words fail G2 and G3; the decoded type is not printed); the run completes; GATE-G-PRECOND unavailable; GATE-G, P3 and P6 not checkable |
| P6-CHIPMEM | `P6-CHIPMEM` | the stream stops at the tail in R6 and R6m (`prog_r6_hang`, `prog_r6m_hang`) | P6-CHIPMEM: FAIL (neither run writes the completion marker); P6: not graded (P6-CHIPMEM) |

Every run above also reprints GATE-G, P3-NOTOTHER and P3-BASE
unchanged (FAIL, FAIL, FAIL) except where the selector feeds them
(GATE-G-PRESENCE leaves the last three not checkable): a mutation gated behind
`NMH_MUTATE` only changes the one input it names, never the others.

Lines without a mutation, each with its record in the test source:

| line | reason |
|---|---|
| FIXTURE | apparatus check with no claim and no page-predicted contrast |
| P1-MARKERS | apparatus anchors (uniqueness of the V1 write and of the V0 write before t_M1); no page predicts a contrast |
| P1-NOEXCEPTION | unavailable from this oracle: the only contrast is RESET executed in user mode, which rests on 680x0 privilege-violation behaviour that no cited page predicts |
| P2 | unfalsifiable by a live page-predicted stimulus at this boundary: p.223 predicts that the instruction after RESET executes, so removing the witness instruction leaves the clause true and turns only the witness red |
| GATE-G-PRECOND | unavailable from this oracle: register 08 bit 6 is a property of the board the harness offers |
| P3-IMG-PRECOND, P3-CONTENT-PRECOND, P3-NOTOTHER-PRECOND, P3-BASE-PRECOND, P3-CONTROL-IO-PRECOND, P3-CONTROL-UNCONF-PRECOND | not exercised, by a scope decision: run liveness, board type and size, memory flag and scratch-write order are anchors of the offered board and of the program order; no page predicts a contrast |
| P6-CONFIG | not exercised, by a scope decision: a program that leaves out the write to 48 in the R6 family would falsify it |
| P6 | unfalsifiable by a live page-predicted stimulus at this boundary: the page's only contrasting case, RESET replaced by NOP, removes P6's own reset-witness precondition P6-WITNESS, so P6 against it is not graded rather than red; P6 is green with no live mutation |

GATE-G, P3-NOTOTHER and P3-BASE are red on the unmutated stimulus
and have no mutation of their own: a stimulus mutation cannot make a red line
more red. Each has green controls on the same comparator.

| red line | green control, same comparator | measured, both cores |
|---|---|---|
| GATE-G ("differs from IMG0, or no answer") | P3-IMG2: after the configuring write to register 48 the image differs from IMG0 (p.435 fn.6, p.431) | PASS |
| P3-NOTOTHER, P3-BASE ("R2 != P2V, or no answer") | P3-CONTROL-IO: board configured at $00200000, read at $00A00000 (Figure K-1, p.386: Zorro II I/O expansion space, outside the 8 MB memory space the board can occupy) | PASS: completion marker written, R2 = $0000, not equal to P2 |
| P3-NOTOTHER, P3-BASE | P3-CONTROL-UNCONF: board left unconfigured (steps c and d skipped), read with R3n's own write/readback pair at $00400000 | PASS: completion marker written, R2 = $0000, not equal to P2 |

No-answer contrasts. No unmutated run ends at the window: every unanswered access in this system completes. These runs exercise the no-answer branch of §2.2. They are apparatus contrasts, not page-predicted ones, and each is measured on both cores:

| line | `NMH_MUTATE` | stimulus | measured, both cores |
|---|---|---|---|
| GATE-G | `GATE-G-NOANSWER` | the shut-up run stops right after its pre-access marker (`prog_rg_shutup_hang`) | PASS, ended by expiry of the no-answer window, every provenance item holding |
| P3-IMG2 | `P3-IMG2-NOANSWER` | R3 stops right after its pre-access marker (`prog_r3_hang_img2`) | PASS, ended by expiry of the window |
| P3-CONTROL-IO | `P3-CONTROL-IO-NOANSWER` | stops after the write and the scratch write, before the readback at $00A00000 (`prog_r3_io_hang`) | PASS, ended by expiry of the window |
| P3-CONTROL-UNCONF | `P3-CONTROL-UNCONF-NOANSWER` | the same at $00400000, board unconfigured (`prog_r3_unconf_hang`) | PASS, ended by expiry of the window |
| P3-CONTROL-IO | `P3-CONTROL-IO-EQSTORE` | P2 stored into the R2 slot, then the stream stops (`prog_r3_io_eqstore`) | FAIL: an equal result was stored and no completion marker followed |
| P3-NOTOTHER | `P3-NOTOTHER-BOARD` | R3n stops after the write at $00400000 and the scratch write, before the readback (`prog_r3_n_hang`) | FAIL (already red): the board took the write, which the `fastram_*` port shows; no completion marker, no stored result |

GATE-G measures red on the unmutated stimulus, on both cores: after the
shut-up write, the PIC's register image is unchanged from IMG0, although
its own precondition holds (register 08 bit 6 reads logical 0, "the
board can be shut up"; p.436: "Anything written to 4C will cause a board
that supports shut-up to completely disappear until the next reset").

P3-NOTOTHER and P3-BASE measure red on the unmutated stimulus, on both
cores, with every precondition printed PASS (type 11, size 110, memory
flag set, scratch write before the readback): the memory board answers
with what was written at the base it was not given, as well as at the
base it was given.
