# Specification: the 68000-family CPU in the whole system (HRM 3rd edition, chapter 1)

This document specifies one integration test package. It was written from the
package's claims (`claims-spec-stage.jsonl`) and the printed pages of the
*Amiga Hardware Reference Manual*, 3rd edition, by an author who never saw the
implementation or any test. Every expected value below comes from a quoted
sentence of the manual. Where the manual leaves a value open, this
specification says so and grades nothing there.

Claim ids are written as they appear in the claims file; where both readings
of a sentence exist (`opus:` and `sol:`), both are listed. Page numbers are
printed page numbers. Quotations from p.13, p.186, p.263, p.278 and p.280
follow the printed scan.

The package has seven propositions:

- **P1**: on the 68000, CLR reads its operand, then writes it.
- **P2**: the 68000's registers hold 32 bits.
- **P3**: every available family member reads instruction words ahead of the
  one it executes.
- **P4**: on the 68020, CLR makes a single write access.
- **P5**: the exception stack frame differs between members.
- **P6**: the 68020 prefetches more words than the 68000.
- **P7**: MOVE SR,\<dest\> acts differently on the 68010 and the 68020 than on
  the 68000.

---

## 1. Claims not turned into a proposition

| Claim ids | Page | What the claim states | Why there is no proposition |
|---|---|---|---|
| `opus:1-guidelines.c69`, `sol:1-guidelines.c69` | 13 | The 68020, 68030 and 68040 have instruction caches. | The second core in 68020 mode is available, but the page gives no expected value a test could see. It does not say whether the cache is active after reset, nor how to activate it: the only CACR bit it names is `CACRF_ClearI EQU $0008 ;Bit for clear instruction cache`. Every consequence the page names needs an active cache: instructions served from the cache, stale instructions after a write ("do not monitor writes"), and the need to flush. A core whose cache is inactive shows none of them, and so does a core without one. The 68030 and 68040 are not available. |
| `opus:1-guidelines.c49`, `sol:1-guidelines.c49` | 12 | The 68020, 68030 and 68040 use all 32 address bits. | On the second core in 68020 mode, the page's statement concerns the address the processor puts out. A test sees only Chip-memory accesses. Whether an access through a pointer whose upper 8 bits are not zero reaches Chip memory depends on how the system decodes address bits 31 to 24, and no page states that for this system. The A1000/A500/A2000 map (p.314) is a 24-bit map. The only 32-bit map (p.316) is the A3000's, a machine with a 68030, not the system under test. So no page gives an expected value at the port. (p.2's "The 68000 has an address space of 16 megabytes" would support an addressing check on the 68000, but that sentence is not a claim of this package.) |
| `opus:1-guidelines.c66`, `sol:1-guidelines.c66` | 13 | If code modifies itself just ahead of the program counter, the prefetched instructions *may* not match. | "may" describes a possibility. The page does not say whether the old word or the modified word executes, so a test must not grade either. |
| `opus:1-guidelines.c77` | 13 | The ClearICache listing. | With the second core in 68020 mode the routine can run, but the page states no observable result for it. Its effect is to clear an instruction cache whose activity the page gives no way to know (see c69). The only value it implies is that bit 15 of CACR reads as 0 on a 68020, since the listing sends only "A 68040 with enabled cache!" down the CPUSHA path. That is an inference from a comment in example code, not a stated value. The text layer scrambles the listing; on the scan it reads: `CACRF_ClearI EQU $0008`; `ClearICache: dc.w $4E7A,$0002 ;MOVEC CACR,D0` / `tst.w d0 ;movec does not affect CC's` / `bmi.s cic_040 ;A 68040 with enabled cache!` / `ori.w #CACRF_ClearI,d0` / `dc.w $4E7B,$0002 ;MOVEC D0,CACR` / `bra.s cic_exit` / `cic_040: dc.w $f4b8 ;CPUSHA (IC)` / `cic_exit:`. This matches the claim's reading. |
| `opus:1-blockdiag.c3`, `sol:1-blockdiag.c3` | 8 | Figure 1-1 labels the CPU "(68000)". | Which processor runs is fixed by the test setup (the core, and the second core's mode). It is not something the system outputs. What the pages say each processor does is checked in P1 to P6. |
| `opus:1-components.c24`, `sol:1-components.c24` | 2 | The MC68000 is the CPU of the A1000, A500 and A2000. | Assigns processors to machine models. The system under test is none of these models and shows no model identity. Its configuration (2 MB of Chip memory, with a 68000, 68010 or 68020) matches no row of Table 1-1 (p.5): the only model listed with 2 MB of Chip RAM is the A3000, which p.1 and p.2 give a 68030. |
| `opus:1-components.c8`, `sol:1-components.c8` | 1 | The A1000, A500 and A2000 contain the 68000; the A3000 uses the 68030. | Same reason as c24. There is no 68030 in the system. |
| `opus:1-components.c21`, `sol:1-components.c21` | 2 | Video ports by model (RGB, monochrome, composite, multiscan). | These are the connectors of machine models. The test sees one video output (4-bit RGB and sync). The pages define no signal for a monochrome, composite or multiscan port, and do not say which port the test's output corresponds to. |
| `opus:1-components.c119`, `sol:1-components.c119` | 6 | The disk controller reads and writes 320/360K and 640/720K IBM PC formats. | **Needs data read from and written to a disk surface.** The harness cannot serve a floppy disk yet, so this claim cannot be checked yet. Beyond that, the page states a capability without a mechanism: no encoding, sync word, data rate or sector layout for these formats. |

**Parts of used claims that are not checked**

- `opus:1-components.c25`, `sol:1-components.c25` (p.2): "while the data bus
  and ALU are 16 bits". The only bus a test sees is the Chip-memory port,
  which carries one word per access whoever the requester is. A CPU with a
  wider data bus would look the same there. The page states no observable
  consequence of the ALU's width.
- `opus:1-components.c6`, `sol:1-components.c6` (p.1): that the main
  processor *is* an MC68000. The reason is the same as for c3. The claim's
  "16/32-bit" is defined on p.2 by c25, and P2 uses it for the 32-bit
  registers.
- `opus:1-guidelines.c64`, `sol:1-guidelines.c64` (p.13): "All 68000 family
  processors". The 68000, 68010 and 68020 are checked (P3); the 68030 and
  68040 are not available.
- `opus:1-guidelines.c52`, `sol:1-guidelines.c52` (p.13): the 68030 and
  68040 are not available. The sentences after the claim are not graded:
  "The type identification in the frame must be checked!" does not say where
  that identification lies or what values it takes, and the autovectors
  "may" lie elsewhere.
- `opus:1-guidelines.c67`, `sol:1-guidelines.c67` (p.13): "The more advanced
  processors prefetch more words." Only the 68020 against the 68000 is
  graded (P6), and that line carries no claim id: the claim speaks of every
  more advanced processor, the 68030 and 68040 are not available, and the
  page does not say whether the 68010 counts as "more advanced". The ids
  stand on a RECORDED line with that reason (section 8). The graded
  relation is the page's sentence for the one pair the system can run.
- `opus:1-guidelines.c59`, `sol:1-guidelines.c59` (p.13, p.263): "The 68020
  CLR instruction does a single Write access." P4 grades the byte and word
  operands, where one access at the operand's word is observable, and its
  line carries both ids. The long-word operand is graded on a separate line
  without ids (no read, both words written): the pages state no data-bus
  width for the 68020, so "a single Write access" is not established for a
  long operand at the 16-bit Chip-memory port (section 6).
- `opus:1-guidelines.c56`, `sol:1-guidelines.c56` (p.13): "MOVE SR,\<dest\>
  ... acts differently on other members of the 68000 family." P7 grades the
  difference between the 68000 and each of the 68010 and the 68020, on a
  line without ids: the claim covers every other member, and the 68030 and
  68040 are not available. The ids stand on a RECORDED line with that reason
  (section 9). The page names no observable in which the difference shows;
  P7 therefore compares everything the program makes the CPU write (section
  9).

---

## 2. Common frame (all propositions)

### 2.1 Configuration, cores and modes

- Default configuration: PAL, ECS, 2 MB of Chip memory. None of the
  sentences used depends on the video standard, the chip set revision or the
  memory size, so the propositions are graded in the default configuration
  only.
- The first core is a 68000. The second core runs as a 68000 by default, and
  the test can set it to behave as a 68010 or a 68020. A proposition is
  graded only on the core and mode its page speaks of. Each graded run gets
  its own verdict.

| Proposition | first core (68000) | second core, 68000 mode | second core, 68010 mode | second core, 68020 mode |
|---|---|---|---|---|
| P1 | graded | graded | not run (the pages are silent on the 68010 CLR) | not graded here; P4 applies |
| P2 | graded | graded | not run (p.2 speaks of the 68000) | not run |
| P3 | graded | graded | graded | graded |
| P4 | cannot run it | contrasting case (that is P1's run) | not run | graded |
| P5 | contrasting case only | graded (as the 68000 member) | graded | graded |
| P6 | contrasting case only | graded (as the 68000 side) | measured, not graded | graded |
| P7 | contrasting case only | graded (as the 68000 side) | graded | graded |

- Each proposition has its own program (P1, P2, P3, P5, P6, P7; P4 reuses
  P1's). Every contrast in this document is a variant of that program.
- The second core's mode is the `cpu_config` value the test sets: 00 for the
  68000, 01 for the 68010, 11 for the 68020. Every run asserts that
  `ports()->cpu_config` shows the requested value (C5, 2.5).

### 2.2 Memory layout

- The program lies in Chip memory and runs in supervisor mode, because it
  writes the status register and (in P2) the user stack pointer. That comes
  from the 68000 instruction set, not from these pages. If the harness starts
  it in user mode, the builder must enter supervisor mode before the setup
  sequence.
- Every data location (operands, sentinels, results, the stack) lies at a
  lower address than the lowest program word, at least 512 bytes below it,
  and above the exception vector area ($000000 to $0003FF). The reason: P3's
  premise is that the CPU reads program words ahead of the instruction it
  executes, and the page does not say how far ahead (p.13: "The more
  advanced processors prefetch more words"). A data word placed after code
  could be read by a prefetch and spoil a count. The one deliberate
  exception is P5, which writes five exception vectors (6.2).
- P1's absolute-short cases need their operand below $8000.
- Layout used below (the builder may shift it if the rules above still
  hold):
  - supervisor stack: A7 set to $3FF0 at program start, growing downward;
  - P5's user stack pointer: $3E00;
  - sentinels and results: $4000 to $40FF;
  - P1 operands: $4110 to $425F;
  - P1 readback: $4300 to $4357;
  - instruction-address tables: P1 $4400 to $4453, P3 $4550 to $455F,
    P6 $4570 to $458F, P7 $4590 to $4597;
  - P7 result words: $4040 to $4046;
  - P2 store area: $4500 to $4543;
  - program: from $8000 upward;
  - the far word Z for P3 and P6: $1FFFFE, the highest word of 2 MB,
    never referenced.

| Name | Address | Written by |
|---|---|---|
| S_A | $4000 | window-open sentinel (P1, P2, P5, P7) |
| S_B | $4002 | window-close sentinel (P1, P2, P5, P7) |
| S_START | $4004 | run start (all) |
| S_END | $4006 | run end (all) |
| S_FAIL | $4008 | failure path (all) |
| S_P | $400A | progress sentinel (P3, P6) |
| S_L | $400C | landing sentinel (P3, P6) |
| S_X | $400E | guard (P3, P6; must never be written) |
| R_INT | $4010 | stored INTENAR |
| R_DMA | $4012 | stored DMACONR |
| SAVE_SP | $4014 (long) | P2 saved stack pointer |
| PRE_SP | $4018 (long) | P5 stack pointer before each exception |
| S_KIND | $401C | P1 table marker: $C1A0 when the case table names CLR, $C1A1 when it names MOVE |
| POST_SP_e | $4020 + 4·e (long, e = 1 to 5) | P5 stack pointer on handler entry |
| DEST | $4040 | P7 destination of MOVE SR (prefilled $5555) |
| MRK_PRIV | $4042 | P7 privilege-violation handler marker ($7001) |
| MRK_TRAP | $4044 | P7 TRAP handler marker ($7002) |
| XTAB7 | $4590 (two longs) | P7 addresses X_w and X_m |

Every sentinel is written with `MOVE.W #imm,abs`. The assembler chooses the
absolute-short form for the sentinels in P1, P2, P3 and P6 (their addresses
are below $8000), except for the guard word of P3 and P6, whose S_X write is
forced to absolute long so that the guard is six words (section 5).

### 2.3 Setup sequence (start of every program)

Register addresses follow p.263: "680x0 address = (chip address) + $DFF000".
The chip addresses and bits come from p.278 (DMACON $096, DMACONR $002) and
p.280 (INTENA $09A, INTENAR $01C), checked on the scan and against
Appendix B (p.289, p.290). The program sets both registers explicitly
because of p.14: "Never assume that a hardware register will be initialized
to any particular value."

1. `MOVE.L #$3FF0,A7`, then `MOVE.W #$0001,S_START`.
2. `MOVE.W #$7FFF,$DFF09A` (INTENA). p.10: "move.w #$7FFF,intena(a0) ; to
   disable all interrupts". p.280: bit 15 SET/CLR "Determines if bits
   written with a 1 get set or cleared"; bit 14 INTEN is the "Master
   interrupt"; bits 13 to 0 are the sources.
3. Set the processor's interrupt mask to 7 (`MOVE.W #$2700,SR`; 68000
   instruction set).
4. `MOVE.W #$07FF,$DFF096` (DMACON). p.278: with SET/CLR = 0, bits written
   as 1 are cleared. Bits 10 to 0 are BLTPRI, DMAEN, BPLEN, COPEN, BLTEN,
   SPREN, DSKEN and AUD3EN to AUD0EN. Bits 12 and 11 are "X" and are written
   as zero, per p.14: "all undefined bits must be set to zeros on writes".
   Bits 14 and 13 are read-only status bits and are also written as zero.
5. Read INTENAR ($DFF01C) and store it at R_INT. Read DMACONR ($DFF002) and
   store it at R_DMA.

### 2.4 Access attribution and time

The test reads the port's record of each access: its time in system clock
cycles, word address, data, and read or write. The record carries no
requester (the CPU, a DMA channel, refresh). Below, "CPU write" means a write
at a sentinel or result address carrying the value the program stores there:
C2 holds the DMA channels off, and C4 requires every sentinel write to appear
exactly once. A read at an address is not attributed to a requester by the
record, so a CPU instruction fetch cannot be told from another read at the
port. Every line that reads an opcode word at an instruction address (P1.c,
P3.d, P7.b and P7.c) is therefore the proposition "a port read at that address
carries the opcode word": an address and data anchor for the image, not a
witness that the CPU made the read. P3's graded line, whose claim rests on a
CPU instruction fetch, says so (section 5). P1's graded c60 line keeps its claim ids: its contrast P1-MOVE
measures no read at T_c inside W_c on either core. "Before" and "after" compare
port times. A byte address B belongs to the word at B with bit 0 cleared.

### 2.5 Common preconditions

A precondition that fails makes the affected check **not evaluated**, never
green.

C1 to C5 and the fixture are asserted in every run of every program,
including the P4 run (section 6), which runs P1's program on the second core.
The setup and the ending are shared macros, so each contrast below exists in
every program and is replayed per program (the S_END ordering contrast of C4
exists in P1 only).

- **C1, interrupts off.** The CPU's write to R_INT carries bits 14 to 0 all
  zero (p.280). Bit 15 is masked, because p.280 defines it only as a write
  control, and p.14 says undefined bits "must be masked out on reads".
  - Contrast: add `MOVE.W #$C000,$DFF09A` after step 2 (SET/CLR = 1,
    INTEN = 1, no source bit). R_INT then shows bit 14 set, so C1 goes red.
    No source is enabled (p.280: request bits "may cause processor
    interrupts" only "if enabled"), and the mask is 7, so the program still
    finishes.
- **C2, DMA off.** The CPU's write to R_DMA carries bits 10 to 0 all zero
  (p.278). Bits 15 to 11 are masked: 15 is the write control, 14 and 13 are
  blitter status, and 12 and 11 are unused (p.14).
  - Contrast: add `MOVE.W #$8240,$DFF096` after step 4 (SET/CLR = 1, DMAEN,
    BLTEN). R_DMA then shows bits 9 and 6 set, so C2 goes red. BLTSIZE is
    never written, and p.186 (scan) says "When the BLTSIZE register is
    written the blit is started", so no blit runs and the program finishes
    as before.
- **C3, completion.** The CPU writes $E0D0 to S_END exactly once and never
  writes S_FAIL (that S_END comes after every window is graded in C4). The failure path
  writes $FA11 to S_FAIL and then continues to the S_END write, so every
  variant ends.
  - Contrast: write S_END twice. It is no longer written exactly once, so C3
    goes red; the failure path is not taken.
  - Contrast (S_FAIL, P1 only): branch to the failure path just before the
    S_END write. S_FAIL is written and S_END is still written once, so C3
    goes red on the S_FAIL clause alone.
- **C4, sentinels are seen as CPU writes.** Every sentinel write the
  program executes appears as exactly one CPU write with the sentinel's
  data. The population is the program's own: the window-open sentinel (S_A,
  or S_P in P3 and P6) takes the values of that program's windows (P1
  $A001 to $A015, P2 $A200, P3 $0001 to $0004, P5 $C001 to $C005, P6 $0001
  to $0008, P7 $E701 and $E702), and the window-close sentinel (S_B, or S_L)
  takes that program's closing values (P1 $B001 to $B015, P2 $B200, P3 $0001
  to $0004, P5 $D001 to $D005, P6 $0001 to $0008, P7 $F701 and $F702). C4 is
  evaluated only when the log holds at least one opener and one closer; with
  an empty opener or closer population it is **not evaluated**, never
  green. When it is evaluated, every opener and closer write in the log
  carries a value of the program's own set, and S_START appears once with
  its data (S_END's count is C3's). S_START comes before every window (the
  first window-open sentinel write is later than the S_START write), and
  S_END comes after them (the S_END write is later than the last
  window-close sentinel write). Whether each window appears, and how often,
  is P1.a's, P2.a's, P3.a's, P5.a's and P7.a's business, not C4's.
  - Contrast (empty population): leave out every S_A and S_B write of P2
    (and every S_P and S_L write of P3). The program finishes, C4 prints
    not evaluated, and P2.a (respectively P3.a) is red.
  - Contrast: write S_START twice. It is no longer written exactly once, C4
    goes red, and S_START still reads back $0001.
  - Contrast (ordering, S_START): write S_START after the first window has
    opened. S_START is still written once with its data, and C4 goes red
    because it does not come before every window.
  - Contrast (ordering, S_END): in P1, write S_END once, before case 21's
    S_B write, and leave out the ending's own S_END write. S_END is written
    once with its data, so C3 holds, and C4 goes red because S_END does not
    come after every window.
- **C5, the requested mode reaches the input.** `cpu_config` is a top-level
  input of the design and the port `ports()->cpu_config` shows the value the
  test drove. Before every measurement, the run asserts that the port shows
  the value the test requested for that run (00, 01 or 11), both after the
  boot and after the run. Every relation that depends on a run's mode
  (P1, P3, P4, P5, P6 and P7 on the second core, and every run of the first
  core) is gated on this line. C5 proves the input, not the core's internal
  mode: the record names no witness of the mode the core runs in, and a run
  labelled 68010 or 68020 grades what the program does under that
  requested value.
  - Contrast (wrong mode): the run is driven with another `cpu_config` value
    than the one it requests (00 for a 68010 or 68020 run, 11 for a 68000
    run). The port shows the driven value, C5 goes red, and the relations
    that depend on the run are not evaluated; the program finishes.
- **Fixture.** Every run's program is loaded, the CPU is released, and the
  program reaches DONE; S_START reads back $0001 through the memory
  inspection before any other inspected value is trusted.
  - Contrast: the program stops in place without writing DONE; the first
    clause goes red. Contrast: S_START is overwritten with $0000 after the
    S_END write (the CPU write of $0001 stays in the log); the readback goes
    red.

---

## 3. P1: CLR reads its operand, then writes it (68000)

**Proposition.** On the 68000, every CLR with a memory operand makes a read
access to the operand before the write access that clears it. For a byte or
word operand this is exactly one read followed by exactly one write.
Claims: `opus:1-guidelines.c60`, `sol:1-guidelines.c60`, p.13. Graded on
the first core and on the second core in 68000 mode.

**Source sentences**

- p.13: "Do not use the CLR instruction on a hardware register which is
  triggered by Write access. The 68020 CLR instruction does a single Write
  access. The 68000 CLR instruction does a Read access first, then a Write
  access. This can cause a hardware register to be triggered twice. Use
  MOVE.x #0, \<address\> instead."
- p.263 (same point, Appendix A): "When strobing any register which
  responds to either a read or a write, (for example copjmp2) be sure to use
  a MOVE.W, not CLR.W. The CLR instruction causes a read and a clear (two
  accesses) on a 68000, but only a single access on 68020 processors. This
  will give different results on different processors."

Both pages agree on read first, then write, and two accesses on the 68000.
The sentence describes the instruction, not the kind of location. The
operand is placed in Chip memory because the Chip-memory port is where a
test sees CPU accesses. A test is not told that CPU accesses to custom
registers appear at that port. CLR with a data-register operand makes no
memory access and is not tested.

**Stimulus.** Program P1: the setup sequence (2.3), then 21 cases in the
order c = 1 to 21, then the S_END write. The cases cover every size and
every memory-alterable destination mode:

| c | size | mode | target |   | c | size | mode | target |
|---|---|---|---|---|---|---|---|---|
| 1 | .B | (An) | T+1 | | 12 | .W | d8(An,Xn) | T |
| 2 | .B | (An)+ | T | | 13 | .W | abs.W | T |
| 3 | .B | -(An) | T+1 | | 14 | .W | abs.L | T |
| 4 | .B | d16(An) | T | | 15 | .L | (An) | T |
| 5 | .B | d8(An,Xn) | T+1 | | 16 | .L | (An)+ | T |
| 6 | .B | abs.W | T | | 17 | .L | -(An) | T |
| 7 | .B | abs.L | T+1 | | 18 | .L | d16(An) | T |
| 8 | .W | (An) | T | | 19 | .L | d8(An,Xn) | T |
| 9 | .W | (An)+ | T | | 20 | .L | abs.W | T |
| 10 | .W | -(An) | T | | 21 | .L | abs.L | T |
| 11 | .W | d16(An) | T | | | | | |

In the table, T = T_c = $4100 + 16·c (an even address). A long operand
occupies T_c to T_c+3. Only A0 to A6 are used as An, and D1 as Xn. Each
case runs these steps:

1. Prefill: `MOVE.W #$A5A5,T_c`. For .L cases, also `MOVE.W #$5A5A,T_c+2`.
2. Load the address and index registers so that the effective address is
   the target in the table. For (An)+, An = target. For -(An), An = target
   plus the operand size. For d16(An) and d8(An,Xn), choose An, Xn and the
   displacement so that their sum is the target.
3. `MOVE.W #($A000+c),S_A`.
4. `CLR.x <ea>`, located at program address X_c.
5. `MOVE.W #($B000+c),S_B`.
6. Readback: `MOVE.W T_c,R_c` with R_c = $4300 + 4·c. For .L cases, also
   `MOVE.W T_c+2,R_c+2`.

**Observation.** The Chip-memory port. Window W_c is the open interval
between the CPU write of $A000+c to S_A and the CPU write of $B000+c to S_B.
The test counts CPU accesses at the operand's word address(es) that fall
inside W_c: the word containing the target byte for .B, T_c for .W, and T_c
and T_c+2 for .L.

**Expected relation**

- .B and .W cases: exactly two CPU accesses at the operand's word address
  in W_c; the earlier is a read and the later is a write. This comes from
  p.13 ("does a Read access first, then a Write access") and p.263 ("a read
  and a clear (two accesses) on a 68000").
- .L cases: the earliest CPU access to T_c or T_c+2 in W_c is a read, and
  at least one CPU write to each of the two words follows it. The number of
  read cycles is not graded. The pages speak of "a Read access" and "two
  accesses" without saying how a 32-bit operand crosses the 16-bit data bus
  (p.2), and "two accesses" cannot hold literally for a long operand on a
  16-bit bus.
- Quantifier: "The 68000 CLR instruction" covers every CLR, so all 21 cases
  must be green for P1 to be green on a core. One case establishes nothing
  about the others.
- A 68000 run that shows a single write without a preceding read (what p.13
  attributes to the 68020; P4) fails P1.

**Preconditions** (asserted per case, before the relation is graded)

- **P1.a, window.** For case c there is exactly one CPU write of $A000+c to
  S_A and exactly one CPU write of $B000+c to S_B, the first before the
  second. The 21 windows appear in case order and do not overlap: for every
  adjacent pair, open[c] < close[c] < open[c+1].
  - Contrast: remove step 3 from case 5. P1.a goes red for case 5 only, and
    the program finishes.
  - Contrast (ordering): move case 4's S_B write and its readback behind
    case 5's S_A write. Every case still has one ordered S_A/S_B pair,
    windows 4 and 5 overlap, P1.a goes red, and P1.b and P1.c hold.
- **P1.b, the target was the operand and was cleared.** Each T_c and R_c
  belongs to case c alone, so the first CPU write to each operand word in
  the log is its prefill step, and it carries the prefill. The readback after
  W_c (the CPU write to R_c, and to R_c+2 for .L) is $0000 for .W, $0000
  and $0000 for .L, and for .B one byte $00 with the other byte $A5.
  - Contrast: in case 8, point the effective address at T_c+8 (a word the
    program does not prefill). The clear lands there, T_c keeps its prefill,
    R_c stays $A5A5, so P1.b goes red for case 8, and the program finishes.
- **P1.c, the instruction in the window is the one the table names.** The
  program writes a table marker (S_KIND) once in its setup: the case table
  names CLR when the marker is $C1A0 and MOVE when it is $C1A1, and the test
  takes the expected opcodes from that logged write (no marker, no
  expectation: P1.c is red). Each X_c is executed once, so the last read at
  X_c in the log (a port read, 2.4) carries the opcode word the assembler
  produced for that case's instruction.
  - Contrast: assemble case 11 as `CLR.L` while the case table still names
    `CLR.W`. The opcode read at X_c differs, so P1.c goes red for case 11,
    and the program finishes.
  - Contrast (MOVE variant, below): assemble case 11 as `MOVE.L #0` while the
    table names `MOVE.W #0`; P1.c, graded against the MOVE opcodes, goes red.
  - Contrast (no marker): leave out the table marker's write. The program
    names no table, so P1.c has no expected opcodes and goes red alone, and
    the program finishes.
- C1 to C5 and the fixture (2.5).

**Contrasting case.** Variant P1-MOVE replaces each `CLR.x <ea>` with
`MOVE.x #0,<ea>`, keeping the same size, mode and target. The case table
names MOVE, so P1.c holds. p.13 ("Use MOVE.x #0, \<address\> instead") and
p.263 ("be sure to use a MOVE.W, not CLR.W") offer this form as the one that
avoids the double triggering caused by CLR's read. The pages therefore
predict that it makes no read access at the operand. Expected: no CPU read
at the operand word(s) in any W_c, so P1's relation goes red in all 21
cases. P1.a and P1.b still hold, because the operand is still cleared, and
the program finishes.

**Reference frame.** The only frame is W_c, bounded by the program's own
sentinel writes. The anchors are P1.a (the sentinels are seen once each, in
order) and P1.c (the instruction at X_c is the one under test).

**Not graded**

- The data returned by the read. The page says nothing about it; it is
  whatever memory holds.
- The time between the read and the write, and any program-word reads that
  fall between them.
- The effect of CLR on a register. p.13 says the double access "can cause a
  hardware register to be triggered twice", which is a possibility. p.263
  says it "will give different results on different processors" without
  saying which. Neither page says what a second strobe does to anything a
  test can see, so no case on a custom register is specified (see 9).
- The 68010. Neither page says what CLR does on a 68010.

---

## 4. P2: the data and address registers hold 32 bits (68000)

**Proposition.** Every data register, every address register and the user
stack pointer of the 68000 holds 32 bits: a 32-bit pattern loaded into the
register is stored back with all 32 bits. Claims:
`opus:1-components.c25`, `sol:1-components.c25` (p.2), and
`opus:1-components.c6`, `sol:1-components.c6` (p.1, whose "16/32-bit" p.2
defines). Graded on the first core and on the second core in 68000 mode.

**Source sentence.** p.2: "The 68000 is a 16/32-bit microprocessor;
internal registers are 32 bits wide, while the data bus and ALU are 16
bits."

The page does not list which "internal registers" it means. The test
grades the seventeen registers a 68000 program can load and store as long
words: D0 to D7, A0 to A7 (A7 is the supervisor stack pointer while the
program runs), and the user stack pointer (USP). Other registers, such as
the status register and the program counter, are not graded, because the
page gives no way to see their width.

**Stimulus.** Program P2: the setup sequence, then:

1. `MOVE.L A7,SAVE_SP`.
2. `MOVEA.L #$C0D0E0F0,A0`, then `MOVE.L A0,USP`.
3. Load each register with its pattern, A7 last:

   | reg | pattern | reg | pattern |
   |---|---|---|---|
   | D0 | $10203040 | A0 | $8090A0B0 |
   | D1 | $11213141 | A1 | $8191A1B0 |
   | D2 | $12223242 | A2 | $8292A2B0 |
   | D3 | $13233343 | A3 | $8393A3B0 |
   | D4 | $14243444 | A4 | $8494A4B0 |
   | D5 | $15253545 | A5 | $8595A5B0 |
   | D6 | $16263646 | A6 | $8696A6B0 |
   | D7 | $17273747 | A7 | $8797A7B0 |
   | | | USP | $C0D0E0F0 |

   Every pattern has two different 16-bit halves, and no two patterns share
   a half.
4. `MOVE.W #$A200,S_A`.
5. For r = D0 to D7 and A0 to A7: `MOVE.L r,M_r` (absolute long), with
   M_r = $4500 + 4·index (D0 = 0 ... A7 = 15).
6. `MOVEA.L SAVE_SP,A7`.
7. `MOVE.L USP,A0`, then `MOVE.L A0,M_USP` with M_USP = $4540.
8. `MOVE.W #$B200,S_B`, then the S_END write.

**Observation.** The Chip-memory port. Window W is the open interval
between the CPU write of $A200 to S_A and the CPU write of $B200 to S_B.
The test reads the words at M_r and M_r+2 through the memory inspection
after the run; P2.b ties each of them to exactly one CPU write inside W.

**Expected relation.** For each of the 17 registers, the two words written
at M_r and M_r+2 in W are the pattern's two halves:
{data(M_r), data(M_r+2)} = {upper 16 bits, lower 16 bits}. The order of the
halves is not graded. Which half goes to the lower address is a property of
the 68000 instruction set, not of these pages, and the width claim does not
depend on it. Quantifier: "internal registers are 32 bits wide" covers all
17 registers. P2 is green on a core only if all 17 are green.

**Preconditions**

- **P2.a, window.** Exactly one CPU write of $A200 to S_A and one of $B200
  to S_B, in that order.
  - Contrast: write step 8's S_B marker twice. P2.a goes red, the window up
    to the first S_B stays usable, and P2.b, P2.c and the program's end
    hold.
- **P2.b, every store happened inside the window.** Exactly one CPU write
  at each of the 34 words M_r and M_r+2 inside W.
  - Contrast: remove the store of D3. P2.b goes red for D3, so D3 is not
    silently passed, and the program finishes.
- **P2.c, no exception while the registers held patterns.** Every CPU
  access inside W is one of these: a read inside the program image, the
  read of SAVE_SP (step 6), a write to one of the 34 store words (M_USP
  included), or the S_B write. A write outside the list, or a read outside
  the program image and SAVE_SP, fails the check. Reads of word 0 are not
  covered: the record names no requester (2.4), and the baseline log of both
  cores shows four reads of word 0 inside W, two memory cycles apart and at
  the same port times on both cores, that the program does not issue (it
  addresses nothing in the vector area); a run that continues past S_END
  repeats the group once per 227-cycle line. Every other read in the vector
  area fails the check. An exception adds stacking writes and a vector read
  outside the list. Also, while A7 holds a pattern, the handler could not
  return, so C3 would go red.
  - Contrast: before W, install a trap-0 handler that only executes `RTE`.
    Add `TRAP #0` between step 6 and step 7, after A7 has been restored to
    the stack in Chip memory. The exception's stacking writes then appear
    inside W, so P2.c goes red, and the program finishes.
  - Contrast (read clause): add `TST.W $4110` between step 6 and step 7. The
    word lies outside the program image and is not SAVE_SP. W holds a read
    that is on neither list and no new write, so P2.c goes red alone: P2.b
    holds, the relation is not evaluated, and the program finishes.
  - Contrast (vector area): add `TST.W $0010` between step 6 and step 7. The
    word is an exception vector other than word 0, so P2.c goes red alone:
    P2.b holds, the relation is not evaluated, and the program finishes.
- C1 to C5 and the fixture. C1 matters here: A7 holds a pattern during steps 3 to 6.

**Contrasting case.** Variant P2-LOW loads every register with its
pattern's lower half, zero-extended: for example `MOVE.L #$00003040,D0`,
and $0000E0F0 for USP. Because the registers hold 32 bits, the page
predicts that the stored words are {$0000, lower half}. That differs from
the expected {upper half, lower half}, so P2 goes red for all 17 registers,
while P2.a to P2.c hold and the program finishes.

**Reference frame.** W, bounded by the program's own sentinel writes. The
anchors are P2.a and P2.b.

---

## 5. P3: every family member reads instruction words ahead of the one it executes

**Proposition.** A 68000-family processor loads instructions ahead of the
one it is executing. This is observable as a CPU read of a program word
that follows an executed instruction and is itself never executed and never
accessed as data. Claims: `opus:1-guidelines.c64`, `sol:1-guidelines.c64`,
`opus:1-guidelines.c65`, `sol:1-guidelines.c65`, p.13. Graded on the first
core, and on the second core in 68000, 68010 and 68020 mode, as the
package's property and without claim ids: the port's record carries no
requester (2.4), so a read at a guard word is not shown to be the CPU's
instruction fetch. `opus:1-guidelines.c65` and `sol:1-guidelines.c65` are
RECORDED with that boundary, not confirmed.

**Source sentences.** p.13: "Self-modifying code is strongly discouraged.
All 68000 family processors have a pre-fetch feature. This means the CPU
loads instructions ahead of the current program counter. Hence, if your
code modifies or decrypts itself just ahead of the program counter, the
pre-fetched instructions may not match the modified instructions. The more
advanced processors prefetch more words."

Why this observation: a CPU that did not load ahead would read only the
words it executes and the words it accesses as data. A read of a word that
is neither can only come from loading ahead.

**Stimulus.** Program P3: the setup sequence, then four blocks, k = 1 to 4,
then the S_END write, with the failure path placed after the end. Each
block ends in a one-word instruction that, in this run, never continues to
the word after it:

| k | preparation | stop instruction at X_k |
|---|---|---|
| 1 | none | `BRA.S L_1` |
| 2 | `LEA L_2,A0` | `JMP (A0)` |
| 3 | `PEA L_3` | `RTS` |
| 4 | `CMP.W D0,D0` (sets Z) | `BEQ.S L_4` |

Block k, in address order:

```
        MOVE.W  #k,S_P          ; progress sentinel
        <preparation k>
X_k:    <stop instruction k>    ; one word
W_k:    MOVE.W  #$DEAD,S_X      ; guard: 4 words (S_X as absolute long), never executed
        BRA.W   FAIL            ; 2 words, never executed
L_k:    MOVE.W  #k,S_L          ; landing sentinel; block k+1 follows
```

The preparation comes after the progress sentinel because a `MOVE` sets the
condition codes (68000 instruction set). Placed between `CMP.W D0,D0` and
`BEQ.S`, it would clear Z and let block 4 fall into its guard.

W_k = X_k + 2 is the first word after the stop instruction. The run window
R is the interval between the CPU write of $0001 to S_START and the CPU
write of $E0D0 to S_END. Every instruction in P3 exists on the 68000, the
68010 and the 68020, so the same image runs in every mode.

**Observation.** The Chip-memory port: the number of reads at each W_k
inside R (the record names no requester, 2.4).

**Expected relation.** At least one read at some W_k (k = 1 to 4) inside R,
which no executed instruction and no data access explains. The test reports
the count for each k but grades only this existence. The page states the feature and gives neither a depth nor a
moment. Its own consequence sentence says that code modified just ahead
"may" not match, so the page does not promise, for any particular
instruction, that the word just ahead has been loaded.

Quantifier: "All 68000 family processors" covers every member available.
P3 is graded separately on the first core, and on the second core in each
of its three modes. The 68030 and 68040 are absent (see 1).

**Preconditions**

- **P3.a, every stop instruction executed and transferred control.** For
  each k in order, exactly one CPU write of k to S_P, followed by exactly one
  CPU write of k to S_L, and no other write to either: the writes to S_P and
  S_L, in time order, are exactly P1, L1, P2, L2, ..., P4, L4.
  - Contrast: write block 2's landing marker twice. S_L then has two writes
    of 2, P3.a goes red, and the stop instructions, the guards, the opcode
    anchors and the end are as in the baseline.
  - Contrast (order): write block 2's progress marker inside block 1, so
    the writes run P1, P2, L1, L2, ... Every block still has one S_P and one
    S_L write in its own order, the complete order is broken, and P3.a goes
    red.
- **P3.b, no guard word was executed.** S_X is never written, and S_FAIL is
  never written.
  - Contrast: aim block 1's `BRA.S` at an instruction behind its first guard
    word, so that the guard's `S_X` store runs and the guard branches to
    L_1. The stop instruction is still a `BRA.S` with a non-zero
    displacement, the landing marker is written once, the failure path is not
    taken and the program finishes; P3.b goes red alone.
- **P3.c, nothing else reads a guard word.** First, no instruction of the
  assembled program accesses data at an address in W_k to W_k+11 on any
  effective-address path: absolute, PC-relative, or through an address
  register whose value the listing's instructions set. The check decodes
  every instruction from the assembler listing (68000 mode), which must
  reproduce the loaded image byte for byte and name the run's own X_k;
  `LEA` and `PEA` compute an address without accessing it. The check has
  three outcomes. A data access inside a guard range makes the line red. An
  instruction the check does not decode, or an access whose address it
  cannot resolve (an index register it does not track, a stack pointer it
  does not track), makes the analysis **unsupported**: the line prints not
  evaluated and the main relation is not evaluated; an unsupported analysis
  is not a guard read. The check tracks the data registers that `MOVEQ`
  sets (each forgotten at any other write, at a label and after an
  unconditional transfer) so that it resolves indexed addresses of the form
  d8(An,Dn). Second, C2 holds (DMA off). The requester of a read at the port
  is not part of this check (2.4).
  - Contrast: add `TST.W W_3` (absolute long) before block 3. The static
    check goes red, and the program finishes.
  - Contrast (PC-relative): add `MOVE.W W_3(PC),D2` before block 3. The
    static check goes red.
  - Contrast (register-indirect): add `LEA W_3(PC),A1` and `TST.W (A1)`
    before block 3. The static check goes red.
  - Contrast (indexed): add `LEA W_3(PC),A1`, `MOVEQ #0,D1` and
    `TST.W 0(A1,D1.W)` before block 3. The indexed read lands on a guard
    word, so the static check goes red.
  - Contrast (read-modify-write): add `ADDQ.W #1,W_3` (absolute long) before
    block 3. The instruction reads and writes a guard word, so the static
    check goes red.
  - Contrast (unsupported analysis): add `SWAP D2` before block 3. The
    check does not decode `SWAP`, the line prints not evaluated, the main
    relation is not evaluated, no line is red, and the program finishes.
- **P3.d, address anchor.** For each k, a port read at X_k inside R carries
  the opcode word the image holds for block k's stop instruction. This is
  the unattributed port-read proposition (2.4): it anchors the image
  addresses (and therefore W_k) to the addresses the record shows, and
  shows that block k's address is read inside R. It is not a witness that
  the CPU made the read. In 68020 mode, the opcode may arrive as part of a
  read that also carries a neighbouring word. What counts is a read whose
  word address is X_k. That block k ran is P3.a's.
  - Contrast: assemble block 1 with `BRA.W L_1` while the block table still
    names `BRA.S`. The opcode read at X_1 differs, so P3.d goes red, and the
    program finishes.
  - Contrast (window): run the four blocks before S_START is written, and
    write the markers P1, L1, ..., P4, L4 and then S_END after them. P3.a
    and C4 still hold, but no port read of X_k lies inside R, so P3.d goes
    red alone, the relation is not evaluated, and the program finishes.
- C1 to C5 and the fixture.

**Contrasting case.** None exists on the pages. The manual names no
stimulus under which a family member does not load ahead, and it states no
depth, so no change of the stimulus has a page-predicted outcome of "no read
at W_k". To show that the counter can report absence, the test also counts
reads at the far word Z ($1FFFFE) inside R and expects zero. Z is never
referenced, and it lies more than 1.9 MB above every program word. This is
a calibration of the measurement, not a page prediction: the pages set no
maximum prefetch distance.

The relation has no contrast. p.13: "All 68000 family processors have a
pre-fetch feature." Every processor the system can run is a member of that
family and prefetches, so no program makes the CPU skip loading ahead, and
every change of the program that leaves the preconditions true leaves a
read at some W_k inside R. The package records this as `none impossible`
for the relation. A `TST.W` of Z before the ending makes the calibration
red.

**Reference frame.** "Ahead" is an address relation: W_k = X_k + 2, taken
from the assembled image. The anchor is P3.d.

**Not graded**

- Which W_k is read, how often, and when relative to other accesses.
- Whether words beyond W_k are read. P6 compares that depth between the
  68020 and the 68000.
- What executes when code is modified just ahead of the program counter
  (c66: "may").

---

## 6. P4: on the 68020, CLR makes a single write access

**Proposition.** On the 68020, every CLR with a memory operand makes no
read access to the operand. For a byte or word operand it makes exactly one
access, a write. Claims: `opus:1-guidelines.c59`,
`sol:1-guidelines.c59`, p.13 (on the byte and word line). Graded on the
second core in 68020 mode only. The first core cannot run it: it has no
68020 mode.

**Source sentences.** p.13: "The 68020 CLR instruction does a single Write
access. The 68000 CLR instruction does a Read access first, then a Write
access." p.263: "The CLR instruction causes a read and a clear (two
accesses) on a 68000, but only a single access on 68020 processors." The
pages agree: on the 68020, a single access, which p.13 calls a write.

**Stimulus.** Program P1 (section 3) unchanged, run on the second core in
68020 mode. Every instruction in it exists on the 68020.

**Observation.** As in P1: the CPU accesses at the operand's word
address(es) inside W_c.

**Expected relation**

- .B and .W cases (14): exactly one CPU access at the operand's word address
  in W_c, and it is a write. This comes from p.13 ("a single Write access")
  and p.263 ("only a single access on 68020 processors"). This is the graded
  line, and it carries both c59 ids. All 14 cases must be green.
- .L cases (7): no CPU read at T_c or T_c+2 in W_c, and at least one CPU
  write to each of the two words. The number of write cycles is not graded.
  These pages state no data-bus width for the 68020, nor how a 32-bit access
  of it reaches the 16-bit Chip-memory port, so this line establishes "no
  read, both words written", not "a single Write access". It is graded on a
  line of its own, without claim ids.
- Quantifier: "The 68020 CLR instruction" covers every CLR, so every case of
  a line must be green for that line to be green.
- A RECORDED line states per core the number of byte and word cases that
  showed one write (of 14), the number of long cases that showed no read
  and both writes (of 7), and the access log of case 1.

**Preconditions.** The fixture (DONE and the S_START readback), C1 to C5 and
P1.a, P1.b and P1.c (section 3), each asserted on the P4 run's own log. The
relation lines are not evaluated when any of them fails. The second core's
mode is the `cpu_config` value the test sets, and C5 asserts that the port
shows 11.

**Contrasting case.** The same program on the second core in its default
68000 mode, which is P1's graded run on that core. p.13 predicts "a Read
access first, then a Write access" there, so P4's relation (no read) goes
red in all 21 cases, while P1.a to P1.c and C1 to C5 hold and the program
finishes. A failed mode setting therefore makes P4 go red, never falsely
green.
- Contrast (68000 setting): the P4 run requests and is driven with
  `cpu_config` 00. C5 holds, the byte and word line goes red in all 14
  cases, and the program finishes. This is the equal control the contrasting
  case above names.
- Contrast (wrong mode): the P4 run requests 11 but is driven with 00. C5
  goes red and the relation lines are not evaluated.
- Contrast (live comparator): the P4 run loads P1's `MOVE.x #0` variant
  (the table marker names MOVE) under 11. `MOVE.x #0,<ea>` makes a single
  write and no read (the page's own offer of the single-access form, p.13
  and p.263), so the byte and word line, through the same comparator,
  is green.

**Reference frame.** W_c as in P1, with the same anchors.

---

## 7. P5: the exception stack frame differs between members

**Proposition.** The exception stack frame depends on the processor: for
each pair of the members the second core can be (68000, 68010, 68020), at
least one of five exceptions, taken from the same program state, writes a
different frame. Claims: `opus:1-guidelines.c52`, `sol:1-guidelines.c52`,
p.13. Graded on the second core in its three modes. The first core cannot
run the 68010 or 68020 side; it serves only the contrasting case.

**Source sentence.** p.13: "The stack frame used for exceptions is different
on each member of the 68000 family. The type identification in the frame
must be checked! In addition, the interrupt autovectors may reside in a
different location on processors with a VBR register."

**Reading.** The sentence names an observable (the frame the processor
writes to the stack) and a relation (different on each member). It gives no
frame layout and does not say which exceptions it means. It can be read
two ways:

1. Every exception's frame differs between every two members.
2. Each member has its own set of frame formats.

Both readings agree only on this: for every pair of members, at least one
exception writes different frames. That is what P5 grades. The test also
reports each exception's comparison, but does not grade it.

**Stimulus.** Program P5.

1. The setup sequence.
2. Install one handler per exception by writing its address (long) into
   the vector: $000010 for illegal instruction, $00001C for TRAPV, $000020
   for privilege violation, $000028 for line 1010, and $000080 for TRAP #0.
   The vector numbers and addresses come from the 68000 instruction set,
   not these pages.
3. In the 68010 and 68020 runs: `MOVEQ #0,D0`, then `MOVEC D0,VBR`, which
   places the vectors at address 0 (68010 instruction set). The page says
   only that the autovectors "may reside in a different location on
   processors with a VBR register". In the 68000-mode runs, the same two
   MOVEC words are two `NOP`s. The images therefore differ only in those
   two words, and every other address is the same in every run.
4. `MOVEA.L #$3E00,A0`, then `MOVE.L A0,USP`.
5. Five cases, e = 1 to 5, in order:

   | e | exception | condition codes set in step c | extra step d | instruction at Y_e |
   |---|---|---|---|---|
   | 1 | TRAP #0 | `MOVE.W #$0000,CCR` | none | `TRAP #0` |
   | 2 | illegal instruction | `MOVE.W #$0000,CCR` | none | `ILLEGAL` ($4AFC) |
   | 3 | line 1010 | `MOVE.W #$0000,CCR` | none | `DC.W $A000` |
   | 4 | TRAPV | `MOVE.W #$0002,CCR` (V set) | none | `TRAPV` |
   | 5 | privilege violation | `MOVE.W #$0000,CCR` | `ANDI.W #$DFFF,SR` (enter user mode; the condition codes are kept) | `MOVE.W #$2700,SR` |

   Each case runs these steps:

   a. `MOVE.L A7,PRE_SP` (supervisor mode, so this is the supervisor stack
      pointer).
   b. `MOVE.W #($C000+e),S_A`.
   c. Set the condition codes as in the table. This comes after every
      `MOVE`, because a `MOVE` sets the condition codes (68000 instruction
      set): after the S_A write, V would be clear and TRAPV would not trap.
   d. The extra step, if any.
   e. The instruction at Y_e.
   f. `BRA.W FAIL`, reached only if no exception is taken.

   Steps c and d write nothing to memory, so the window opened in step b
   still holds only the frame.

   Handler e runs `MOVE.W #($D000+e),S_B`, then `MOVE.L A7,POST_SP_e`,
   then `MOVEA.L PRE_SP,A7`, then `JMP CONT_e`, where CONT_e is the next
   case. The handler never returns through the frame, so it does not need
   to know the frame's layout.

   The five exceptions were chosen because the instruction that raises each
   one leaves the condition codes as the program set them (68000
   instruction set), so the status register saved in the frame is the same
   in every run. CHK and divide-by-zero are left out: their condition codes
   are undefined, which could make frames differ for a reason the page does
   not speak of. Line-1111 opcodes are left out because on a 68020 they
   address a coprocessor, and the pages do not say how the system answers
   that cycle.
6. The S_END write.

**Observation.** The Chip-memory port. Window W_e is the open interval
between the CPU write of $C000+e to S_A and the CPU write of $D000+e to S_B.
The frame F_e is the set of pairs (address − PRE_SP, data), one for each
word address of a CPU write inside W_e; where a word is written twice, the
last write counts.

**Expected relation.** For each of the three pairs of modes on the second
core, (68000, 68010), (68000, 68020) and (68010, 68020), there is at least
one e for which F_e differs between the two runs. Two frames differ if
their sets are not equal, which includes a difference in length. All three
pairs must hold.

**Preconditions**

- **P5.a, every exception was taken, through the vector installed.** For
  each e in each run: one CPU write of $C000+e to S_A, then one of $D000+e
  to S_B. Inside W_e, a CPU read at the vector address of e (the long word
  at the address in step 2). No write to S_FAIL.
  - Contrast: case 1 executes `TRAP #1` instead of `TRAP #0`, with its
    handler installed at vector 33 as well as at vector 32. The exception is
    taken, the window holds the frame and the handler's S_B write follows,
    but no read at the installed vector (32) lies inside W_1, so P5.a goes
    red and the program finishes without S_FAIL. The contrast is replayed on
    each image kind: the 68000-mode image for the 68000-mode run and the VBR
    image (the same change) for the 68010 and 68020 runs, so each run's P5.a
    line has its own contrast.
- **P5.b, the same starting state in every run.** The CPU writes to PRE_SP
  carry the same value in all runs. The two images have the same length and
  every differing byte lies inside step 3's four bytes (a static check on
  the assembler listings, which locate step 3 and reproduce their images).
  Zero differences are valid: byte-identical images are the strongest
  same-program-state setup the comparison can have, and the pair relations
  are graded on them.
  - Contrast: in the 68010 run, replace step 3's `MOVEC D0,VBR` by
    `SUBQ.L #4,A7` and a `NOP`. The image still differs from the 68000-mode
    image only inside step 3's four bytes, PRE_SP differs from the other
    runs, so P5.b goes red through its dynamic half, and the program
    finishes.
  - Contrast (static, outside step 3): the 68010 run loads the VBR image
    (step 3's `MOVEC D0,VBR` kept) that also differs from the 68000-mode
    image at one byte (image offset $FB) inside case 3's four-byte
    `MOVE.W #imm,CCR` (`#1` for `#0`), outside step 3. The image differs
    inside step 3 as every VBR image does, so only the clause "every
    differing byte lies inside step 3" fails, and P5.b goes red. The static
    half compares the images the runs loaded, not fixed files.
- **P5.c, the window holds only the frame.** Every CPU write inside W_e
  lies at an address from POST_SP_e (the value the handler stores) up to
  PRE_SP minus 2.
  - Contrast: in case 2, add `MOVE.W D0,S_P` between step d and step e. A
    write outside the stack appears in W_2, so P5.c goes red, and the
    program finishes. The contrast is replayed on each image kind (the
    68000-mode image, and the VBR image with the same change for the 68010
    and 68020 runs), so each run's P5.c line has its own contrast. A red
    P5.c leaves the three pair lines not evaluated.
- C1 to C5 and the fixture. C1 matters here: an interrupt inside W_e would
  add a second frame.

Each pair's relation also has its own contrast: one of the two runs of the
pair uses the other member's `cpu_config` value with its own image
(the 68000-mode image under the 68010 value, the 68000-mode image under the
68020 value, the VBR image of the 68010 run under the 68020 value), so that
those two runs write the same frames. That pair's relation goes red alone,
while P5.a to P5.c, P5.b and the other two pairs hold.

A run whose fixture fails (the program does not reach DONE) turns only its
fixture line red: its P5.a and P5.c lines and the pair relations that depend
on it are not evaluated.

**Contrasting case.** Compare the first core (a 68000, running the
68000-mode image) with the second core in 68000 mode. The page ties the
frame to the member ("The stack frame used for exceptions is different on
each member"), so two 68000s taking the same exception from the same state
are predicted to write the same frame. F_e is then equal for every e, and
the relation "at least one e differs", evaluated for this pair, goes red.
P5.a to P5.c hold on both cores, and both programs finish.

**Reference frame.** Frame offsets are measured from PRE_SP, which the
program stores before each exception. The anchors are P5.a (the vector read
and the handler's sentinel) and P5.b (equal PRE_SP and equal images).

**Not graded**

- Which exceptions differ, and each frame's length or content.
- The "type identification in the frame": the page says it "must be
  checked", but not where it lies or what values it takes.
- Where the autovectors lie: the page says "may", and does not say which
  members have a VBR.

---

## 8. P6: the 68020 prefetches more words than the 68000

**Proposition.** The 68020 prefetches more words than the 68000: with the
same program on the second core, the largest number of never-executed
instruction words read after a stop instruction is larger in 68020 mode
than in 68000 mode. Claims: `opus:1-guidelines.c67`,
`sol:1-guidelines.c67`, p.13 (RECORDED, not on the graded line, section 1);
the observable is the one P3 establishes (c64, c65). Graded on the second
core only. The first core cannot run the 68020 side; it serves only the
contrasting case.

**Source sentence.** p.13: "All 68000 family processors have a pre-fetch
feature. This means the CPU loads instructions ahead of the current program
counter. ... The more advanced processors prefetch more words."

**Which processors are "more advanced".** The page does not define this.
It groups the 68020 with the 68030 and 68040 as the processors that "use all
32 bits for addressing" (p.12) and "have instruction caches" (p.13). The
68010 appears on these pages only in the heading "FOR
68010/68020/68030/68040 COMPATIBILITY" (p.12). Every reading therefore
agrees that the 68020 is more advanced than the 68000, so only that pair is
graded. The 68010-mode run is measured and reported, not graded.

**Stimulus.** Program P6 has the same structure, sentinels and guards as P3,
with eight blocks: the four stop kinds of P3, each twice. In one copy, X_k
lies at an address divisible by 4; in the other, at an address that leaves
remainder 2. This keeps the result independent of where a stop falls
relative to a long-word boundary. The builder pads with `MOVEQ #0,D2`
placed before the block's S_P write, followed by a `NOP` for the first three
pairs (blocks 1 to 6): their preparations leave the gap between the two
copies a multiple of four bytes, so one word would leave both copies at the
same parity. The `BEQ.S` pair (blocks 7 and 8) prepares with the one-word
`CMP.W D0,D0`, so its pad stays the single `MOVEQ`.

In each block, the order is: the S_P write, then `LEA` or `PEA` (for k = 2
and 3), then `MOVE.L #$00001000,D0` and `DIVU.W D1,D0` (with D1 = 3, loaded
once after the setup sequence), then `CMP.W D0,D0` for k = 4 only, then the
stop instruction. The flag-setting `CMP` stays last, directly before
`BEQ.S`. DIVU is register-only and takes long to execute, so loading ahead
has time to go as far as it goes before the stop instruction runs. Every
instruction exists on the 68000 and the 68020, and the same image runs in
both modes.

**Observation.** The Chip-memory port, in window R (as in P3), for each run:

- g_k is the number of distinct words among the six guard words W_k to
  W_k+10 that the CPU reads inside R.
- D is the largest g_k over the eight blocks.

**Expected relation.** D(68020 mode) > D(68000 mode), both measured on the
second core running the same image. The page gives no number for either
processor, so only the order is graded. D is the best lower bound a run
gives on how many words the processor prefetches.

**Preconditions**

- The fixture (DONE and the S_START readback), C1 to C5 and the asserted
  mode (C5: the port shows 00 for the 68000-mode run and 11 for the
  68020-mode run), in both runs.
- P3.a to P3.d, for all eight blocks, in both runs. The contrasts are the
  same as in P3: a landing marker written twice, a guard reached by a
  `BRA.S`, a PC-relative read of a guard word, and block 1 assembled with
  `BRA.W` (with the pair's pad dropped so the pair keeps opposite parity).
- **P6 alignment.** Within each pair, the two copies' X_k lie at addresses
  of different parity mod 4.
  - Contrast: leave out the first pair's extra pad word. X_1 and X_2 land at
    the same parity, and the alignment line goes red.
- **P6.e, the guard is long enough to show "more".** D(68000 mode) is less
  than the guard length (six words). If the 68000-mode run already reads
  every guard word, a larger count cannot be seen.
  - Contrast: shorten every guard to the single word `ILLEGAL` ($4AFC),
    which is still never executed, and add one `NOP` word after it so that
    every pair keeps X_k at opposite parity mod 4. D(68000 mode) then equals
    the guard length (P3 requires at least one read), so P6.e goes red, and
    the program finishes.
- P6.e gates the relation: it is asserted on the 68000-mode run.
- The relation line carries no claim id (section 1). It is not evaluated
  when any of the lines above fails.

**Contrasting case.** Evaluate the same relation with the second core in
68000 mode on the larger side and the first core (a 68000) on the smaller
side. The page ties the amount of prefetch to the processor ("The more
advanced processors prefetch more words"), and neither side is more
advanced, so it does not predict "more". The relation goes red. The
preconditions hold on both cores, and both programs finish.
- Contrast (68000 setting): the second run requests and is driven with
  `cpu_config` 00. C5 holds, both runs are 68000-mode runs of the same
  image, the relation D(second run) > D(first run) goes red, and the program
  finishes. This is the equal-68000 control the contrasting case names; both
  sides are the same processor, and the page predicts no "more".
- Contrast (wrong mode): the 68020-mode run requests 11 but is driven with
  00. C5 goes red and the relation is not evaluated.
- No stimulus is known on which the relation is green: the page states no
  prefetch depth and names no program under which the 68020 reads more
  words, so the relation has no green control of its own. P6.e's
  saturation contrast and the far-word calibration of P3 show that the
  measurement reports saturation and absence.

**Reference frame.** As in P3: guard words are addresses X_k + 2 to
X_k + 12, taken from the image. The anchor is P3.d. The far word Z is
counted as in P3, as a calibration.

**Not graded**

- D's value on either processor. g_k per block is reported only.
- The 68010 against the 68000 (see "Which processors are 'more advanced'").

---

## 9. P7: MOVE SR,\<dest\> acts differently on the 68010 and the 68020 than on the 68000

**Proposition.** The same program, started from the same state and executing
MOVE SR,\<dest\> in user mode, makes the system behave differently under the
68010 and the 68020 than under the 68000: for each of the pairs (68000,
68010) and (68000, 68020), the port's record of the window differs. Claims:
`opus:1-guidelines.c56`, `sol:1-guidelines.c56`, p.13, which stand on a
RECORDED line (section 1), not on a graded one. Graded on the second core in
its three modes. The first core serves only to record the 68000 side.

**Source sentence.** p.13: "Do not use the MOVE SR,\<dest\> instruction! This
68000 instruction acts differently on other members of the 68000 family."

**Reading.** The sentence compares the 68000's instruction with the other
members' and says that the instruction "acts differently". It names no
observable in which the difference shows and no direction. The page therefore
supplies the inequality and nothing else. P7 grades that inequality on
everything the port shows, so that a difference in the stored value, in an
exception, in the bus accesses or in their timing all count, and no
implementation-derived direction enters.

**Stimulus.** Program P7, one image for every mode.

1. The setup sequence (2.3). Install the handler HW at the privilege-violation
   vector ($000020) and the handler HT at the TRAP #0 vector ($000080); set
   the user stack pointer to $3E00 (`MOVE.L` through `A0`, `MOVE.L A0,USP`);
   write $5555 to DEST.
2. Witness window W_w. `MOVE.L A7,PRE_SP`; `MOVE.W #$E701,S_A`;
   `ANDI.W #$DFFF,SR` (enter user mode; the condition codes are kept); at
   X_w the instruction `MOVE.W #$2700,SR`, which is privileged in user mode
   (68000 instruction set, as in P5 case 5); then `BRA.W FAIL`, reached only
   if no exception is taken. HW writes $F701 to S_B, restores the supervisor
   stack pointer from PRE_SP and jumps to step 3.
3. Moved-SR window W_m. Install the handler HM at the privilege-violation
   vector; `MOVE.L A7,PRE_SP`; `MOVE.W #$E702,S_A`; `ANDI.W #$DFFF,SR`; at
   X_m the instruction `MOVE.W SR,DEST` (absolute short, opcode $40F8); then
   `TRAP #0` (reached only if MOVE SR did not raise an exception), then
   `BRA.W FAIL`. HM writes $7001 to MRK_PRIV, HT writes $7002 to MRK_TRAP;
   both write $F702 to S_B, restore the supervisor stack pointer from PRE_SP
   and jump to step 4. No handler returns through its frame.
4. Each of X_w and X_m is stored in XTAB7 with a PC-relative `LEA`. The
   ending (S_END).

The instructions exist on every member the system can run (`MOVE.W SR,<ea>` is
a 68000 instruction; the page's subject), so the same image runs in every
mode.

**Observation.** The Chip-memory port. W_m is the open interval between the
CPU write of $E702 to S_A and the CPU write of $F702 to S_B. For each run,
the sequence Q of the port's accesses inside W_m, each as (time since the
write of $E702 to S_A, word address, data, read or write), in time order.
The test also records, for each run, the CPU writes inside W_m (the word at
DEST, the markers and the frame words) and the three result words after the
run, as RECORDED lines. They are not graded.

**Expected relation.** For each of the pairs (68000, 68010) and (68000,
68020), Q differs between the two runs: the sequences differ in length or in
at least one entry. Both pairs must hold. p.13 states the difference for "other
members" than the 68000 and nothing about 68010 against 68020, which is
reported and not graded.

**Preconditions**

- **P7.a, the moved-SR window.** Exactly one CPU write of $E702 to S_A and
  one of $F702 to S_B, in that order, in each run; no write to S_FAIL.
  - Contrast: leave out the `MOVE.W #$E702,S_A` write. P7.a goes red and the
    program finishes.
- **P7.b, the instruction at X_m is the one under test.** A port read at
  X_m inside the run window carries $40F8 (2.4: an address and data anchor).
  - Contrast: assemble the instruction as `MOVE.W SR,D2` (opcode $40C2). The
    opcode at X_m differs, P7.b goes red and the program finishes.
- **P7.c, user mode was entered.** Inside W_w (between the CPU writes of
  $E701 to S_A and $F701 to S_B, once each) there is a read at the
  privilege-violation vector, and a port read at X_w carries $46FC
  (`MOVE.W #imm,SR`): the privileged instruction raised the exception, which
  it does only from user mode.
  - Contrast: execute `TRAP #1` instead of the privileged instruction, with
    HW installed at vector 33 as well as at vector 32. The exception is
    taken and the window closes, but no read at the privilege vector lies
    inside W_w and no port read of X_w carries $46FC, so P7.c goes red and
    the program finishes.
- **P7 control, the sequence is a property of the mode.** A second run of
  the 68000 mode produces the same Q as the first.
  - Contrast: the second 68000-mode run loads a variant whose W_m executes
    `MOVE.W D1,DEST` in place of `MOVE.W SR,DEST` (the exception route is
    still not taken, the stores differ), so Q differs and the control goes
    red; the pair lines do not depend on that run.
- C1 to C5 and the fixture, in every run.

**Contrasting case.** A pair of runs under the same mode. p.13 states a
difference between members, so two runs of one member are predicted to give
the same Q, and the relation evaluated on them goes red.
- Contrast (pair 68000, 68010): the 68010 run requests and is driven with
  00. C5 holds, the two Q are equal, and that pair's relation goes red alone;
  the other pair holds.
- Contrast (pair 68000, 68020): the 68020 run requests and is driven with
  00. The pair (68000, 68020) goes red alone.

**Reference frame.** W_m, bounded by the program's own sentinel writes; the
anchors are P7.a and P7.b.

**Not graded**

- Which of the observables differs, and how. The page says only "acts
  differently".
- 68010 against 68020, and the 68030 and 68040 (not available).
- The stored status register value at DEST.

---

## 10. Where the pages differ or leave the point open

- **Which registers the CLR warning concerns.** p.13 names "a hardware
  register which is triggered by Write access". p.263 names "any register
  which responds to either a read or a write, (for example copjmp2)". p.14
  says "All custom chip registers are Read-only or Write-only", and p.263's
  own warning a few lines later repeats "Registers are either read-only or
  write-only". The pages disagree about whether a register can respond to
  both a read and a write. What every page agrees on is graded, and only
  that: on a 68000, CLR reads before it writes, with two accesses (P1); on
  a 68020, it makes a single access (P4). No double-trigger outcome is
  specified.
- **"Internal registers"** (p.2) are not listed. P2 grades the seventeen
  registers a program can load and store as long words, and nothing else.
- **Prefetch depth and timing** (p.13) are not stated. P3 grades only
  existence. P6 grades only the order between the 68020 and the 68000.
- **"More advanced"** (p.13) is not defined. Only the 68020 against the
  68000 is graded.
- **"Different on each member"** (p.13) can mean every exception or the
  set of formats. P5 grades only what both readings share.
- **Long operands.** Neither p.13 nor p.263 says how many bus accesses a
  CLR.L makes: the 68000's bus is 16 bits (p.2), and no width is stated for
  the 68020. P1 grades only that a read comes first; P4 grades only that no
  read occurs.
- **Byte order of a long store** is not on these pages. P2 grades the halves
  as an unordered pair.
- **How MOVE SR "acts differently"** (p.13) is not stated: not the stored
  value, not the exception, not the timing. P7 grades only that the port's
  record of the window differs between the 68000 and each of the other two
  available members.

---

## 11. Build

- **Addresses.** `PROGRAM_ORG` ($080400) is the harness's own fixed code
  entry; every program's first instruction clears the boot overlay
  (`CIAAPRA`, $BFE001, any write) so that the identity-mapped range
  extends down to $000000, making 2.2's own literal addresses ($3E00, $3FF0,
  $4000-$458F, $8000 taken as "wherever code starts") real, writable chip
  RAM, including P1's abs.W cases and P5's exception vectors.
- **X_c/X_k self-report.** Every case/block stores the address of its own
  CLR/MOVE/stop instruction into a table (XTAB, XTAB3, XTAB6) with a
  PC-relative LEA, so the C++ side never hand-counts instruction lengths.
- **Measured, P1's own contrast (P1-MOVE, section 3).** MOVE.B #0,(a0)
  (case 1, .B (An)) makes a single write, no read, on both cores (first
  core: one write; second core: one write) -- the page's own contrast
  holds. P1's relation (read then write) is graded through the same,
  unchanged comparator as the normal run, and goes red under this input on
  both cores, exactly as p.13 and p.263 predict for MOVE against CLR.
- **Measured, P4 (section 6).** The run prints, for the second core under
  cpu_config=11, the number of byte and word cases that showed one write, the
  number of long cases that showed no read and both writes, the access log
  of case 1, and the run's RAM-port log compared entry for entry (time,
  address, data, direction) with the same program under cpu_config=00. The
  first core has no 68020 mode and does not run it.
- **Measured, P6 (section 8).** The guard is 6 words long (W_k to W_k+10).
  The run prints D(68000 mode) on both cores and D(68020 mode) on the second
  core, the same image, and the 68020-mode RAM-port log compared entry for
  entry with the 68000-mode log of the same image.
- **Measured, P7 (section 9).** The run prints, per mode, the CPU writes
  inside W_m and the three result words, and each pair's first differing
  entry.
- **P5's and P6's own "contrasting case"** (first core vs. second core in
  68000 mode) spans both cores' separate executables; neither build can
  read the other's data, so each core prints its own RECORDED frame/depth
  data (F_1..F_5 for P5, D for P6) and the comparison is drawn once both
  outputs exist, not as a `checks.expect` inside either binary. Measured:
  the first core's F_1..F_5 and the second core's own 68000-mode F_1..F_5
  are byte-for-byte identical, and both cores' D(68000 mode) = 1 -- both
  contrasts hold exactly as the page predicts (two 68000s write the same
  frame; neither side is "more advanced").
