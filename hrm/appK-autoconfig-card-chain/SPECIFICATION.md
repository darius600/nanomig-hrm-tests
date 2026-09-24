# Specification — `appK-autoconfig-card-chain`

Package: `hrm/appK-autoconfig-card-chain`
Fixture (top module): `cpu_wrapper`
Claims file: `hrm-testpipeline/work/appK-build/appK-autoconfig-space.jsonl`
(30 claims, printed pages 431, 432, 435 — Appendix K, "THE AUTOCONFIG
MECHANISM" prose and the "Configuration Register Mapping" figure) — the
same claims file `hrm/appK-autoconfig-space` cites.

## What configuration this package builds

`hrm/appK-autoconfig-space` builds `cpu_wrapper` with `-DENABLE_RAM32=1`,
the configuration of NanoMig's 32 MiB boards
(`src/lattice/icepi-zero`, `src/lattice/icesugar-pro`, `src/mistle/gw3a_20`
and `src/mistle/gw5a_25` each `define it in `top.sv`). This package builds
without it — every other board target builds without `ENABLE_RAM32` — and
measures the same 30 claims in that configuration.

The macro changes the design under test in two places this package's
groups depend on directly:

- `cpu_wrapper.v:535-539`: with `ENABLE_RAM32`, `cfg_z3` is
  `fastramcfg_d[2] & cpucfg[1]`, so a Zorro III board can exist; without
  it, `cfg_z3` is hard-wired `0`. No Zorro III board exists in this build,
  and no assertion in this file exercises one.
- `cpu_wrapper.v:643-651`: this block — the case that advances
  `autocfg_card` from `2'b01` to `2'b10` to `2'b11` on successive
  register-48 writes when `fastramcfg_d[1:0] == 2'b11` — is compiled only
  `ifndef ENABLE_RAM32`. Writing register 48 repeatedly chains up to
  **three** Zorro II boards before the space falls silent.

This build's Zorro II chain is the mechanism the central claim,
`sol:K-0733` (the configuration-chain hand-off), is tested against. The
claim itself does not name Zorro III.

## Disclosure on the write-order

Fixture design, core selection, active-low reset, direct/complement
arithmetic, and the register-48-write capability (`MOVE.W #imm,(abs).L`,
opcode `0x33FC`) are reused from `hrm/appK-autoconfig-space`. RTL line
citations in this package's own comments cite `cpu_wrapper.v`; the claims
file's `classify.evidence` text cites different line numbers for the same
constructs.

## What this package tests, by group

1. **Lifecycle** (8 claims: `sol:K-0727,K-0734,K-0739,K-0842,K-0843`;
   `opus:K-0755(p431),K-0879,K-0880`) — a board responds in `$E8xxxx`
   while unconfigured and stops responding once the final base-address
   write completes. `sol:K-0727` and `opus:K-0755` name RESET as well as
   power-up ("When an Amiga powers up OR RESETS..."); `ResetProbe`
   configures all three boards of the chain, confirms silence, pulses
   `reset` again mid-run, then WALKS THE CHAIN AGAIN post-reset (reads
   register 00, then twice writes register 48 and reads register 00),
   requiring all three boards to answer again, so "every card" is
   observed re-arming rather than inferred from board 1's own return. The very first post-reset reading is also
   compared, byte for byte, against register 00's own reading from before
   any write (measured, not an assumed constant). The mutation splices its
   probe program at wherever the CPU will actually fetch from next —
   `$1000` if reset genuinely redirects it there, or its own current
   self-loop location if the pulse is skipped — so the no-pulse case is a
   genuine observation of the chain staying silent, not an apparatus
   failure that happens to read as red.
2. **Handoff** (1 claim: `sol:K-0733`) — graded on the FIRST hand-off
   only, at `fastramcfg=3`: after the configuring write to register 48,
   the `ramreq` output port (cpu_wrapper.v:81/168) is asserted for an
   access into board 1's own Zorro II RAM range ($200000,
   cpu_wrapper.v:117's `sel_z2ram`), proving board 1 is now configured, AND
   register 00 still answers, proving a still-unconfigured board remains
   visible. Whether board 1 SPECIFICALLY has left $00E8xxxx (as opposed to
   still answering while also configured) is not independently observed
   here — it is inferred from Group 1's lifecycle claims, measured
   separately at `fastramcfg=1`; the test file's own note says so. `z2ram_ena`
   (cpu_wrapper.v:117/641) is a single sticky enable covering the whole
   $200000-$9FFFFF range, latched once and never cleared, so `ramreq`
   cannot corroborate the SECOND hand-off (board 2 to board 3) — an
   observability limit of this one corroborating signal, recorded
   `none impossible`. "Exactly one board answers at a time" is structural
   (one `autocfg_data` mux driven by one `autocfg_card` register) and
   cannot be independently falsified by stimulus either; both limits have
   their own record in the test file. A separate, four-write measurement
   of the whole chain (used by `sol:K-0744`/`opus:K-0767` below) reports
   the size nibbles as MEASURED lines only — they are NanoMig's own
   implementation choice, not a value graded against, per `sol:K-0737`'s
   own withdrawal reasoning.
3. **Read/write split** (2 claims: `sol:K-0735`, `opus:K-0762`) —
   register 00 (read-only) ignores a write; register 48 (write-only, the
   one write register this build's Zorro II path acts on) completes
   configuration on a write. Tested directly, `fastramcfg=1`.
4. **Read-register content** (2 claims: `sol:K-0737`, `opus:K-0764`) —
   NOT GRADED. See the record at the claim's grading point: the sentence
   bundles device type, size and other requirements, and only the type
   field's manual-defined coding is checkable against an independent
   reference here.
5. **Address window** (4 claims: `sol:K-0742,K-0743,K-0744`;
   `opus:K-0767`) — probes at $E7FFFE (one word below the window) and
   $E90000 (one byte above it) confirm no override outside $00E8xxxx;
   $E8FFFE (the top of the window) and $E80100 (past the 6-bit
   `chip_addr[6:1]` case range) each carry their OWN preset and are
   graded on "reads something OTHER than the preset" (an override
   happened), not against an assumed RTL constant. `sol:K-0744`'s "every
   card configures there" requires all three boards the chain presents
   (`fastramcfg=3` in this build, cpu_wrapper.v:643-651) to answer before
   the space falls silent, PLUS an outside-window probe read while each of
   boards 1, 2 and 3 is actually the responder (not after the chain has
   already fallen silent). `sol:K-0743`'s "16-bit cycles" is graded on the
   chip-bus strobes themselves — `chip_uds`/`chip_lds` sampled at every
   cycle `chip_as` is asserted AND the address is inside $E8xxxx (excluding
   the reset-vector fetch, instruction fetches and the write-back to
   scratch) during a read of register 00, both required low throughout —
   rather than on an address outside configuration space. Bus width
   itself is fixed by the port declaration (`chip_dout`/`chip_din`,
   `cpu_wrapper.v:61-62`); that sub-clause's own mutation record is
   `none impossible`, the only record this claim carries.
6. **One nybble only** (1 claim graded: `sol:K-0750`; `opus:K-0771` NOT
   GRADED) — `cpu_wrapper.v:236`'s `chip_data[11:0]` is used in BOTH arms
   of the read-data mux, so the low-12-bit pass-through half of this
   claim is graded against a value computed from the preset itself and
   cannot be broken by any stimulus (its own record says so); the
   top-nybble-override half is graded on the manual-defined type-code
   bits (3-2 == `11`), not the specific `0xE` constant, and broken by
   disabling `sel_autoconfig` (`fastramcfg=0`). Page 438's pin table
   (physical pins 63/65/67/69) shows Zorro II's D15-D12 and Zorro III's
   D31-D28 (data phase) are the same physical lines, so this design's
   `[15:12]` bits ARE the page's "D31-D28" for a Zorro II board, not a
   different lane numbering.
7. **Register width / paired addressing** (5 claims graded:
   `sol:K-0752,K-0753,K-0760`; `opus:K-0773,K-0779`; `sol:K-0757` NOT
   GRADED) — graded structurally: register 00's type address ($E80000)
   must carry the SAME nibble at `fastramcfg=1` and `fastramcfg=2`, while
   its size address ($E80002) must carry a DIFFERENT one, proving both
   addresses are genuinely and independently read rather than one
   supplying an assumed constant.
8. **Complement rule** (4 claims graded: `sol:K-0755(p432),K-0756,K-0759`;
   `opus:K-0776`; `opus:K-0777` NOT GRADED) — register 3C, plus a sweep of
   the manual's own "Reserved, must be 0" registers ($0C, p.434; $30, $34,
   $38, $40, p.435 — not $04 or $08, whose logical values the manual
   leaves to the manufacturer or the board): all read raw `1111`,
   complementing to logical 0, evidence for "EVERY read register except
   00" rather than register 3C alone. Register 00 is the sole exception,
   graded on the manual-defined type-code bit pattern (bits 3-2 == `11`)
   rather than "not all ones", since a complemented register could fail to
   read all-ones by coincidence without actually being direct. All three
   conjuncts (register 3C, the sweep, and register 00's own bits) carry
   their own stimulus mutation; none is unfalsifiable.

## NOT GRADED claims

- `sol:K-0737`, `opus:K-0764` (p431) — `reason_kind: source-silent`. Bundle
  device type, size and other requirements. Only the type field's coding
  (bits 3-2 == `11` for a Zorro-II add-memory board) is checkable against
  an independent reference in this configuration; NanoMig's declared sizes
  for boards 2 and 3 of the chain are its own implementation choice, and
  the manual prints no expected value to check the SIZE or OTHER
  REQUIREMENTS clauses against, even though both are observable at this
  boundary.
- `opus:K-0771` (p431) — `reason_kind: absent`. Its own quote claims the
  one-nybble rule "for either bus mode" (Zorro II's 16-bit bus and Zorro
  III's 32-bit bus); the second mode has no signal path in this build at
  all (`cfg_z3` is a compiled-out constant). The Zorro-II-only equivalent
  is graded (`sol:K-0750`).
- `opus:K-0777` (p432) — `reason_kind: absent`. Its own quote names
  `$FF00003C`/`$FF00013C`, Zorro III's addressing form for register 3C;
  no signal path for it exists in this build. The Zorro-II addressing and
  value are graded (`sol:K-0756`, `sol:K-0759`).
- `sol:K-0757` (p432) — `reason_kind: insufficient`. Names $E8003C/$E8003E
  as register 3C's location. The port is not missing — the address reads
  fine — but register 3C's logical value is 0, and this design's
  complement rule makes EVERY undriven register, at that address pair and
  at any other reserved index, read the identical raw `0xF`/`0xF`. No
  stimulus can distinguish this specific address from any other undriven
  one by value, so its location is not established at this boundary, even
  though the manual's own worked example names exactly this pair and this
  value.

Five ids across five `record()` calls (printed `RECORDED:`) are withdrawn
this way, at the point in `test_cpu_wrapper.cpp` where each claim would
otherwise be graded. `reason_kind` is drawn from the pipeline's controlled
list (`absent`, `insufficient`, `not-hardware`, `outside-uut`,
`source-silent`; hrm-testpipeline/docs/aa-runbook.md). Where a withdrawal
cites a uid that IS graded elsewhere in the package, the citation carries
the pipeline's cross-reference token (`graded: <uid>`) so the coverage
checker reads it as a reference, not a second decline of that uid.

## What this build actually does

Harness configurations (fixture mechanism as in `hrm/appK-autoconfig-space`):

- **Lifecycle** (`fastramcfg=1`): reads register 00 before any write;
  writes register 48; reads register 00 again. `ResetProbe` separately
  configures all three boards of the chain (`fastramcfg=3`), confirms
  silence, pulses `reset` mid-run, reads register 00's two addresses,
  writes register 48 once more, and reads register 00 a further two
  addresses.
- **Handoff** (`fastramcfg=3`): reads register 00 (both addresses)
  before and after the FIRST register-48 write; two further harnesses
  probe `ramreq` before and after that same write. A separate,
  independently-built harness (unaffected by this claim's own mutation)
  issues all four writes, reads register 00 at all five points, and reads
  $E90000 while boards 1, 2 and 3 are each the responder, for
  `sol:K-0744`/`opus:K-0767`'s own evidence.
- **Read/write split** (`fastramcfg=1`): reads register 00, writes
  register 00 (no-op check), reads register 00 again, writes register 48,
  reads register 00 a third time.
- **Address window** (`fastramcfg=1`): reads register 00, $E90000
  (outside), $E80100 (inside, past the case range), $E7FFFE (below the
  window) and $E8FFFE (the top of the window), each in-window probe with
  its own preset. A dedicated probe reads register 00 once and samples
  `chip_uds`/`chip_lds` throughout, filtered to $E8xxxx addresses only,
  for the word-cycle claim.
- **Nybble-only probe** (`fastramcfg=1`, dedicated): reads register 00
  with `chip_dout` preset to a recognisable low-12-bit pattern at
  $E80000.
- **Register width** (`fastramcfg=1` and `fastramcfg=2`, two harnesses):
  each reads register 00's two addresses.
- **Register 3C / complement** (`fastramcfg=1`): reads register 3C's two
  addresses; a dedicated harness reads register 00 alone for its own
  type-code bits; a further harness sweeps the manual's "Reserved, must be
  0" registers.

## What is not tested and why

Zorro III behaviour is out of scope for this fixture: `cfg_z3` is
hard-wired `0` in this build (`cpu_wrapper.v:539`), so no Zorro III board
is ever instantiated, and no signal at the `cpu_wrapper` boundary can
produce one. That behaviour is measured in `hrm/appK-autoconfig-space`.
