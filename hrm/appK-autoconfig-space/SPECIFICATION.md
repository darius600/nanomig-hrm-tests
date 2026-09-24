# Specification — `appK-autoconfig-space`

Package: `hrm/appK-autoconfig-space`
Fixture (top module): `cpu_wrapper`
Claims file: `hrm-testpipeline/work/appK-build/appK-autoconfig-space.jsonl`
(30 claims, printed pages 431, 432, 434, 435, 438 — Appendix K, "THE
AUTOCONFIG MECHANISM" prose, the "Configuration Register Mapping" figure
and the pin table)

## What configuration this package builds

This package builds `cpu_wrapper` with `-DENABLE_RAM32=1`, the
configuration of NanoMig's 32 MiB boards (`src/lattice/icepi-zero`,
`src/lattice/icesugar-pro`, `src/mistle/gw3a_20` and `src/mistle/gw5a_25`
each `define it in `top.sv`) — the only configuration in which
`cpu_wrapper.v` compiles a Zorro III path at all: `cfg_z3 =
fastramcfg_d[2] & cpucfg[1]` (cpu_wrapper.v:536), a real wire this build
can drive to 1. Every other board target builds without `ENABLE_RAM32`;
there `cfg_z3` is a hard-wired constant `0`, no Zorro III path exists, and
a write to register 48 instead chains up to three Zorro II boards —
measured in `hrm/appK-autoconfig-card-chain`.

## Core

This package builds `cpu_wrapper` with TG68K (`-DENABLE_TG68K=1`), through
the instance `cpu_wrapper.v` compiles under Verilator (`:420-441`,
`nanomig/sim/TG68KdotC_Kernel.v`). That instance leaves the core's
`skipFetch` output unconnected, which the synthesis instance connects
(`:464`), so `skip_fetch` is 0 here and `cpu_req` (`:101`) requests bus
cycles the synthesized TG68K skips. `hrm/appK-autoconfig-space-fx68k`
builds the same test source with fx68k, which has one instance for
simulation and synthesis alike.

## Disclosure on the write-order

Fixture design, core selection, active-low reset, direct/complement
arithmetic, and the register-44/48-write capability (`MOVE.W
#imm,(abs).L`, opcode `0x33FC`) are reused from
`hrm/appK-autoconfig-register-map` and `hrm/appK-autoconfig-size-encoding`.

## What this package tests, by group

1. **Lifecycle** (8 claims: `sol:K-0727,K-0734,K-0739,K-0842,K-0843`;
   `opus:K-0755(p431),K-0879,K-0880`) — a card responds in `$E8xxxx`
   while unconfigured and stops responding once the final base-address
   write completes. `sol:K-0727` and `opus:K-0755` name RESET as well as
   power-up ("When an Amiga powers up OR RESETS..."); `ResetProbe`
   configures both boards of the Zorro III chain, confirms silence,
   pulses `reset` again mid-run, then WALKS THE CHAIN AGAIN post-reset
   (reads register 00, writes register 44, reads register 00 once more),
   requiring at least a second responding point — the same basis Group 2
   uses — so "every card" is observed re-arming rather than inferred from
   board 1's own return. The very first post-reset reading is also
   compared, byte for byte, against register 00's reading from before any
   write. The mutation splices its probe program at wherever the CPU will
   actually fetch from next — `$1000` if reset genuinely redirects it
   there, or its own current self-loop location if the pulse is skipped —
   so the no-pulse case is a genuine observation of the chain staying
   silent, not an apparatus failure that happens to read as red.
2. **Handoff** (1 claim: `sol:K-0733`) — Zorro III: two successive
   register-44 writes, each completing one board. `ramreq`
   (cpu_wrapper.v:81/168, `assign ramreq = ramsel & ram_dtack;`, an
   existing module output, not a debug port) IS reachable here:
   `sel_z3ram1` compares `cpu_addr[31:28]` — always `0000` on this
   fixture's 24-bit core (`TG68K_A24`, cpu_wrapper.v:34, `cpu_addr =
   {8'h00, cpu_addr_p[23:0]}` at cpu_wrapper.v:279) — against
   `z3ram_base1`, which is set from the configuring write's OWN data
   (`cpu_dout[15:12]`, cpu_wrapper.v:655), not a fixed value. Writing base
   `0x0000` makes `z3ram_base1` also `0000`, which DOES match
   `cpu_addr[31:28]` and asserts `ramreq`; a base of `0x1000` (upper
   nibble `0x1`) does not. Graded on: `ramreq` asserted after a
   dedicated write with base `0x0000` completing board 1 (a harness with
   no further chip-bus traffic for that same decode to intercept);
   register 00 still answering after a SEPARATE write with base `0x1000`
   (safe — does not redirect the following read); the space silent after
   a second write; board 2's own completion corroborated the same way
   (base `0x0000` on write 2). Size nibbles are reported as MEASURED lines
   only, not graded against — NanoMig's own board sizes are not a
   manual-prescribed value (see `sol:K-0737`'s own withdrawal reasoning).
   "Exactly one board answers at a time" is structural (one
   `autocfg_data` mux driven by one `autocfg_card` register) and cannot be
   independently falsified by stimulus either.
3. **Read/write split** (2 claims: `sol:K-0735`, `opus:K-0762`) — in this
   harness (`cpucfg=0`, Zorro II context) register 44 is inert: nothing is
   mid-configuration to interpret `chip_addr[6:1]==6'b100010`, so it reads
   whatever every other reserved register reads (the unassigned default,
   cpu_wrapper.v:547) — that raw value does not, by itself, distinguish a
   write-only register from a read register, so it is not graded on.
   Graded instead on the two registers this harness's Zorro II path
   actually distinguishes: register 00 (read-only) ignores a write
   (exercised by `MUT_WRITETOWRITEREG`, which redirects that write to
   register 48, which DOES have an effect); register 48 (write-only)
   completes configuration on a write (exercised by `MUT_REG48NOEFFECT`,
   which diverts that write to $E90000, outside the decoded window, so it
   no longer has any effect).
4. **Read-register content** (2 claims: `sol:K-0737`, `opus:K-0764`) —
   NOT GRADED. This configuration CAN contrast the type field against an
   independent reference for both devices it implements (Zorro II reads
   bits 3-2 == `11`, Zorro III reads `10`, matching Appendix K's own
   register-map table, measured as MEASURED-only context) — but the
   sentence bundles type with size and other requirements, and the
   declared size is NanoMig's own implementation choice with no
   manual-prescribed value to check it against; "other requirements" is
   not observable at this boundary at all. The stronger type evidence
   this configuration offers narrows what is undecided; it does not
   remove it.
5. **Address window** (4 claims: `sol:K-0742,K-0743,K-0744`;
   `opus:K-0767`) — probes at $E7FFFE (one word below the window) and
   $E90000 (one byte above it) confirm no override outside $00E8xxxx;
   $E8FFFE (the top of the window) and $E80100 (past the 6-bit
   `chip_addr[6:1]` case range) each carry their OWN preset and are
   graded on "reads something OTHER than the preset" (an override
   happened), not against an assumed RTL constant. `sol:K-0743`'s "16-bit
   cycles" is graded on the chip-bus strobes themselves —
   `chip_uds`/`chip_lds` sampled at every cycle `chip_as` is asserted AND
   the address is inside $E8xxxx (excluding the reset-vector fetch,
   instruction fetches and the write-back to scratch) during a read of
   register 00, both required low throughout — rather than on an address
   outside configuration space. Bus width itself is fixed by the port
   declaration; that sub-clause's own mutation record is `none
   impossible`, the only record this claim carries. `sol:K-0744` — "every
   Zorro II card configures there" — is graded on window confinement
   alone: this build's own Zorro II path never chains beyond one board
   (the chain-extension case is compiled out `ifndef ENABLE_RAM32`), so
   there is no second Zorro II board in this configuration to require a
   second responding point from.
6. **One nybble only** (1 claim graded: `sol:K-0750`; `opus:K-0771` NOT
   GRADED) — `cpu_wrapper.v:236`'s `chip_data[11:0]` is used in BOTH arms
   of the read-data mux, so the low-12-bit pass-through half is graded
   against a value computed from the preset itself and cannot be broken by
   any stimulus (its own record says so); the top-nybble override half is
   graded on the manual-defined type-code bits, not a specific constant,
   and broken by disabling `sel_autoconfig` (`fastramcfg=0`). A Zorro III
   probe (`cpucfg=2`, `fastramcfg=4`) is also measured for context, but
   `opus:K-0771`'s own "for either bus mode" clause (p.431: "A Zorro III
   PIC can configure in Zorro II or Zorro III configuration space ... on
   D31-D28 for either bus mode") names the CONFIGURATION SPACE and cycle
   type a board configures in, not which board type answers — NanoMig's
   Zorro III board still configures in the $E8xxxx Zorro II configuration
   space over this design's one 16-bit chip bus (`sol:K-0743`'s own
   `StrobeProbe`; `opus:K-0777`'s own withdrawal establishes no Zorro III
   configuration space is reachable at all), so both probes measure the
   SAME bus mode and "either" cannot be decided; graded instead on the
   single reachable mode (`sol:K-0750`). Page 438's pin table (physical
   pins 63/65/67/69) shows Zorro II's D15-D12 and Zorro III's D31-D28
   (data phase) are the same physical lines, so this design's `[15:12]`
   bits ARE the page's "D31-D28" for either card, not a different lane
   numbering.
7. **Register width / paired addressing** (5 claims graded:
   `sol:K-0752,K-0753,K-0760`; `opus:K-0773,K-0779`; `sol:K-0757` NOT
   GRADED) — graded structurally: register 00's type address ($E80000)
   must carry the SAME nibble at `fastramcfg=1` and `fastramcfg=2`, while
   its size address ($E80002) must carry a DIFFERENT one (2 MiB vs 4 MiB,
   cpu_wrapper.v:572-573), proving both addresses are genuinely and
   independently read rather than one supplying an assumed constant.
8. **Complement rule** (4 claims graded: `sol:K-0755(p432),K-0756,K-0759`;
   `opus:K-0776`; `opus:K-0777` NOT GRADED) — register 3C, plus a sweep of
   the manual's own "Reserved, must be 0" registers ($0C, p.434; $30,
   $34, $38, $40, p.435 — not $04 or $08, whose logical values the manual
   leaves to the manufacturer or the board): all read raw `1111`,
   complementing to logical 0, evidence for "EVERY read register except
   00" rather than register 3C alone. Register 00 is the sole exception,
   graded on the manual-defined type-code bit pattern (bits 3-2 == `11`
   in this Zorro II context) rather than "not all ones". All three
   conjuncts carry their own stimulus mutation; none is unfalsifiable.

## NOT GRADED claims

- `sol:K-0737`, `opus:K-0764` (p431) — `reason_kind: source-silent`. The
  type sub-clause is observable and independently checkable here (bits
  3-2 == `11`/`10` for Zorro II/Zorro III, matching Appendix K's own
  register-map table), but the sentence bundles it with size and other
  requirements, and the manual prints no expected value to check either
  of those against, even though both are observable.
- `opus:K-0771` (p431) — `reason_kind: absent`. Its own quote reads "A
  Zorro III PIC can configure in Zorro II or Zorro III configuration space
  ... on D31-D28 for either bus mode" — bus mode names the configuration
  space and cycle type a board configures in, not which board type
  answers. This fixture never reaches a genuine second bus mode: no Zorro
  III configuration space is decoded at all (`opus:K-0777`'s own
  withdrawal), so the second of the two modes this claim names has no
  signal path here, and both the Zorro II and the Zorro III-board probes
  measure the same (Zorro II) bus mode. The single reachable mode is
  graded (`sol:K-0750`).
- `sol:K-0757` (p432) — `reason_kind: insufficient`. Names $E8003C/$E8003E
  as register 3C's location. The port is not missing, but every undriven
  register in this design, at that address pair and at any other reserved
  index, reads the identical raw `0xF`/`0xF`. No stimulus can distinguish
  this specific address from any other undriven one by value, so its
  location is not established at this boundary, even though the manual's
  own worked example names exactly this pair and this value.
- `opus:K-0777` (p432) — `reason_kind: absent`. Its own quote names
  `$FF00003C`/`$FF00013C`, Zorro III's addressing form using the physical
  connector's full address width. `chip_addr` is declared `[23:1]`
  (cpu_wrapper.v:34/60) — a 24-bit byte address space that cannot
  represent an address needing bits beyond 23, whatever `fastramcfg` or
  `cpucfg` selects. The Zorro-II addressing and value are graded
  (`sol:K-0756`, `sol:K-0759`).

Five ids across five `record()` calls (printed `RECORDED:`) are withdrawn
this way, at the point in `test_cpu_wrapper.cpp` where each claim would
otherwise be graded. `reason_kind` is drawn from the pipeline's controlled
list (`absent`, `insufficient`, `not-hardware`, `outside-uut`,
`source-silent`; hrm-testpipeline/docs/aa-runbook.md). Where a withdrawal
cites a uid that IS graded elsewhere in the package, the citation carries
the pipeline's cross-reference token (`graded: <uid>`) so the coverage
checker reads it as a reference, not a second decline of that uid.

## What this build actually does

Harness configurations:

- **Lifecycle** (`cpucfg=0`, `fastramcfg=1`): reads register 00 before
  any write; writes register 48; reads register 00 again. `ResetProbe`
  separately configures both boards of the Zorro III chain (`cpucfg=2`,
  `fastramcfg=5`), confirms silence, pulses `reset` mid-run, reads
  register 00's two addresses, writes register 44 once more, and reads
  register 00 a further address.
- **Handoff** (`cpucfg=2`, `fastramcfg=5`, four dedicated harnesses): one
  writes register 44 with base `0x0000` (completing board 1) then reads
  register 00, sampling `ramreq` (`max_cyc_override=3000`, no further
  chip-bus traffic after the write); one reads register 00, writes
  register 44 with base `0x1000` (safe — does not redirect subsequent
  reads), and reads register 00 again; one reads register 00 before and
  after each of two register-44 writes (bases `0x1000` then `0x2000`) to
  confirm final silence; one writes register 44 with base `0x1000` then
  `0x0000` (completing board 2) and reads register 00, again sampling
  `ramreq`.
- **Read/write split** (`cpucfg=0`, `fastramcfg=1`): reads register 00,
  writes register 00 (no-op check), reads register 00, writes register 48
  (configuring effect check), reads register 00 again.
- **Read-register content** (`cpucfg=0/fastramcfg=1` and
  `cpucfg=2/fastramcfg=4`, MEASURED only): reads register 00 in each
  context.
- **Address window** (`cpucfg=0`, `fastramcfg=1`): reads register 00,
  $E90000 (outside), $E80100 (inside, past the case range), $E7FFFE
  (below the window) and $E8FFFE (the top of the window), each in-window
  probe with its own preset. A dedicated probe reads register 00 once and
  samples `chip_uds`/`chip_lds` throughout, filtered to $E8xxxx
  addresses only, for the word-cycle claim.
- **Nybble-only probe** (`cpucfg=0/fastramcfg=1` and `cpucfg=2/fastramcfg=4`,
  dedicated): reads register 00 with `chip_dout` preset to a recognisable
  low-12-bit pattern at $E80000, in both configurations.
- **Register width** (`fastramcfg=1` and `fastramcfg=2`, two harnesses):
  each reads register 00's two addresses.
- **Register 3C / complement** (`cpucfg=0`, `fastramcfg=1`): reads
  register 3C's two addresses; a dedicated harness reads register 00
  alone for its own type-code bits; a further harness sweeps the
  manual's "Reserved, must be 0" registers.

## What is not tested and why

Nothing in this build is out of scope for the fixture: this is the one
configuration in which `cpu_wrapper.v` compiles both the Zorro II and the
Zorro III paths, and every testable clause that names either is graded or
carries its own NOT GRADED record above.
