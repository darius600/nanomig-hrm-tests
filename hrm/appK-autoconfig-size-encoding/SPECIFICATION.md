# Specification — `appK-autoconfig-size-encoding`

Package: `hrm/appK-autoconfig-size-encoding`
Fixture (top module, per task assignment): `cpu_wrapper`
Claims file: `hrm-testpipeline/work/appK-build/appK-autoconfig-size-encoding.jsonl`
(21 claims, printed pages 433-434, Appendix K, "REGISTER BIT ASSIGNMENTS" —
register 00 bits 2:0 size encoding, register 08 bit 5 as the size-extension
selector, and register 08's sub-size nybble)

## Disclosure on the write-order

This package reuses `hrm/appK-autoconfig-register-map`'s fixture wholesale
(the `Harness` mechanism, `TG68KdotC_Kernel` core selection, active-low
`reset`, the direct/complement register arithmetic) exactly as
`hrm/appK-autoconfig-reserved-registers` does, and inherits the same
write-order disclosure: the claims file's `classify.evidence` already
carries RTL citations, so "spec blind, then RTL" does not hold here either.
Additionally, this package's own fixture needs a capability the register-map
and reserved-registers fixtures did not: **writing** an AUTOCONFIG register
(to advance `autocfg_card` from its reset value to the "second Zorro III
board" state, the only way to observe the 128 MB size code at all, since it
is never emitted until after the first board's base-address write
completes). That write path was investigated directly against the RTL text
(`cpu_wrapper.v:613-666`) before this SPECIFICATION was finished, for the
same reason the register-map package's boundary investigation had to
precede its own spec.

## Core

This package builds `cpu_wrapper` with TG68K (`-DENABLE_TG68K=1`), through
the instance `cpu_wrapper.v` compiles under Verilator (`:420-441`). By
default the build compiles the kernel the boards synthesize, translated
from `nanomig/src/tg68k/*.vhd` by GHDL with the generics of the board
instance (`:405-417`), and a copy of `cpu_wrapper.v` that connects the
kernel's `skipFetch` output in that instance as the board instance does
(`:464`), so `cpu_req` (`:101`) drops the bus request on a skipped fetch,
as on the board. `TG68K_KERNEL=shipped` compiles
`nanomig/sim/TG68KdotC_Kernel.v` instead, a netlist of an older kernel
revision, with the unmodified `cpu_wrapper.v`, whose Verilator instance
leaves `skipFetch` unconnected: `skip_fetch` is then 0 and `cpu_req`
requests bus cycles the synthesized TG68K skips.
`hrm/appK-autoconfig-size-encoding-fx68k`
builds the same test source with fx68k, which has one instance for
simulation and synthesis alike.

## The register-map arithmetic this package relies on

Inherited unchanged from `hrm/appK-autoconfig-register-map`: one 4-bit
nibble per read, `chip_addr[6:1]` index 0/1 direct, every other index
complemented.

## Boundary investigation

**Register 00 bits 2:0 (size code) live at `chip_addr[6:1]` index 1**,
direct (no complement) — same index Group D of the register-map package
already reads for bit 3 ("next board related"); this package reads bits
2:0 of the same nibble.

**Register 08 bit 5 (size-extension selector) lives at index 4**, bit 1 of
the nibble after the complement rule — the same index the register-map
package's Group F already reads for bits 7:6:4; this package reads bit 5
(nibble bit 1) of the same value.

**Register 08 bits 3:0 (sub-size) live at index 5**, `cpu_wrapper.v:599`
(`6'b000101: autocfg_data = 4'b1111; // 0000 - logical size matches
physical size`) — **not** index 4. `classify.evidence` for `sol:K-0793`
names `chip_addr[6:1]==6'b000101` explicitly; index 4 (the flags nibble,
bits 7:4) is a different logical nybble entirely and is not reused here.

**Reading the case statement in full** (`cpu_wrapper.v:568-606`):

- Zorro II (`~cfg_z3`), index 1: keyed on `{autocfg_card, fastramcfg_d}`
  (`cpu_wrapper.v:571`); with `autocfg_card` at its reset value `2'b01`
  (true for every Zorro II harness this package runs, since none of them
  write register 48), this `ENABLE_RAM32` build wires three values:
  `5'b01001` → `4'b0110` (code `110`, comment `// 2MB`, `:572`), `5'b01010`
  → `4'b0111` (code `111`, comment `// 4MB`, `:573`), and `default` →
  `4'b0000` (code `000`, comment `// 8MB`, `:575`). **Code `011` (256 KiB)
  is never emitted in Zorro II mode by this build** — no
  `{autocfg_card, fastramcfg_d}` combination this package can drive
  produces it. (Without `ENABLE_RAM32` the same key also wires `5'b01011`
  → `4'b0111` (`:577`, 4MB again), `5'b10011` → `4'b0101` (`:579`, 1MB
  extra stolen from SlowRAM), and a different `default` → `4'b0100`
  (`:580`, 0.5MB extra stolen from SlowRAM); this package's Makefile
  defines `ENABLE_RAM32`, so that branch is not compiled here.)
- Zorro III (`cfg_z3`), index 1: `autocfg_card[1] ? 4'b0011 : 4'b0100` —
  only codes `011` (128 MB, comment `// 128MB or 256MB, extended`, the
  `autocfg_card[1]=1` arm) and `100` (256 MB, the `autocfg_card[1]=0` arm,
  which is `autocfg_card`'s reset value `2'b01`) are ever emitted. **Codes
  `000`, `110` and `111` are never emitted in Zorro III mode** — there is
  no `autocfg_card` value that reaches them.
- In this `ENABLE_RAM32` build, `autocfg_card[1]` only becomes `1` after
  the *first* Zorro III base-address write completes
  (`cpu_wrapper.v:653-663`, register 44): `autocfg_card <=
  {fastramcfg_d[0], 1'b0}`. With `fastramcfg_d[0]=1` this is `2'b10`
  (`[1]=1`, still nonzero so `sel_autoconfig` keeps responding for a
  still-unconfigured "second" region); with `fastramcfg_d[0]=0` it is
  `2'b00` (done, silenced). So the 128 MB code is reachable only by
  **actually writing register 44 once**, with `fastramcfg[0]=1` supplied
  at elaboration. (Without `ENABLE_RAM32`, `autocfg_card[1]` can also
  become `1` from a register-48 write, via the Zorro II chain-advance at
  `cpu_wrapper.v:646` — an unrelated mechanism this build does not
  compile.)
- **`ENABLE_RAM32`** is a real per-board `` `define`` (`icepi-zero`,
  `icesugar-pro`, and two `mistle` targets set it, per
  `hrm/appK-autoconfig-register-map`'s own board-top survey), not an
  artifact invented for this test — the same category of decision as that
  package's own `ENABLE_TG68K` core selection. This package's Makefile
  defines it (`-DENABLE_RAM32=1`) so the `000`/"8MB" code, which no board
  reaches without it, can be graded rather than declined outright; this is
  disclosed here rather than silently baked into the build.

**Consequence for grading**: several printed rows name a code's meaning in
*both* the unextended and extended columns (e.g. "110 → 2 megabytes /
1 gigabyte"). For codes `000`, `011`, `100`, `110`, **exactly one of the two
columns is ever reachable in this `ENABLE_RAM32` build** (the other value
is never produced by any case arm this build compiles, at any input this
fixture — or any fixture built on `cpu_wrapper`'s own ports — can drive). Per the build brief's own rule
("where a claim states more than your package tests, the uid goes on the
`record()` line and NOT on the graded one"), the `sol`-reader claims that
state *both* columns in one restatement (`K-0772`, `K-0775`, `K-0776`,
`K-0778`) are **declined**, each naming the untested half explicitly; the
`opus`-reader claims that were already scoped to the one reachable column
(`K-0796`, `K-0803`, `K-0805`, `K-0808`) are graded. `K-0779`/`K-0810`
(code `111`) are the exception: the page itself says the extended column is
"RESERVED", and this design's own Zorro III case list never emits `111`
either — the claim and the RTL agree that the extended half is unused, so
both halves of `K-0779` are graded (not declined).

## Declined claims (6)

- **`sol:K-0772`** (code `000`, "8 MiB unextended or 16 MiB extended") —
  the extended half is never emitted (no Zorro III case arm produces
  `000`); the unextended half is graded separately under `opus:K-0796`.
  `reason_kind: partial-untested-half`.
- **`sol:K-0775`** (code `011`, "256 KiB unextended or 128 MiB extended")
  — the unextended half is never emitted (no Zorro II `fastramcfg_d` value
  produces `011`); the extended half is graded under `opus:K-0803`.
  `reason_kind: partial-untested-half`.
- **`sol:K-0776`** (code `100`, "512 KiB unextended or 256 MiB extended")
  — the unextended half is never emitted in this `ENABLE_RAM32` build (it
  is reachable only via the Zorro II chain-advance default at
  `cpu_wrapper.v:580`, which is not compiled here); the extended half is
  graded under `opus:K-0805`. `reason_kind: partial-untested-half`.
- **`sol:K-0778`** (code `110`, "2 MiB unextended or 1 GiB extended") —
  the extended half is never emitted (Zorro III index 1 only ever produces
  `011`/`100`); the unextended half is graded under `opus:K-0808`.
  `reason_kind: partial-untested-half`.
- **`sol:K-0789`, `opus:K-0820`** ("aligned on their natural boundaries")
  — the base-address registers (44/48) are write-only with no read-back
  (confirmed in `hrm/appK-autoconfig-base-address`'s own boundary
  investigation); the only way to observe whether a written base is
  actually forced onto its natural alignment boundary would be either an
  internal probe of `z3ram_base0`/`z3ram_base1` (forbidden — no debug
  ports) or a live RAM-model fixture driving `ramsel`/`ramready` so a
  matching CPU access can be observed reaching the internal RAM path
  instead of the chip bus — which this fixture's permanently-tied-low
  `ramready` cannot safely exercise without hanging the CPU indefinitely
  waiting for a `ramready` pulse that never comes (the two-hangs history
  `hrm/appK-autoconfig-register-map`'s own SPECIFICATION.md records is the
  reason this is not attempted here). `reason_kind: unreachable-observable`,
  boundary named: an internal `z3ram_base0`/`z3ram_base1` probe, or a
  RAM-model fixture driving `ramreq`/`ramready`.

## What this build actually does

Four harness configurations:

- **H1** Zorro II, `fastramcfg=1` ("2MB" arm)
- **H2** Zorro II, `fastramcfg=2` ("4MB" arm)
- **H3** Zorro II, `fastramcfg=3` (falls to `default`; `-DENABLE_RAM32=1`
  makes this "8MB")
- **H4** Zorro III, `fastramcfg=5` (`cpucfg=2`, `fastramcfg[2]=1` for
  `cfg_z3`, `fastramcfg[0]=1` so the post-write `autocfg_card` state is
  `2'b10` rather than `0`) — reads index 1 (and 0, 4) before any write,
  then writes register 44 once (`MOVE.W #imm,(target).L`, the standard
  68000 encoding `0x33FC`), then reads index 1 again to capture the
  "second board" 128 MB code.

Every read/write is executed by the same real reset → 68000 program →
bus-cycle sequence as the sibling packages; the write to register 44 is
observed only by its effect on the *next* AUTOCONFIG read, never by
touching `autocfg_card` directly.
