# Specification — `appK-autoconfig-space`

Package: `hrm/appK-autoconfig-space`
Fixture (top module, per task assignment): `cpu_wrapper`
Claims file: `hrm-testpipeline/work/appK-build/appK-autoconfig-space.jsonl`
(30 claims, printed pages 431, 432, 435 — Appendix K, "THE AUTOCONFIG
MECHANISM" prose and the "Configuration Register Mapping" figure)

## Disclosure on the write-order

Reused wholesale from `hrm/appK-autoconfig-register-map`: fixture design,
core selection, active-low reset, direct/complement arithmetic — same
disclosure applies (the claims file's `classify.evidence` already carries
RTL citations, breaking "spec blind, then RTL").

This package needs one capability beyond a plain read: **writing** an
AUTOCONFIG register, to observe the configuration space actually falling
silent once a board completes configuration (`autocfg_card` clearing,
`sel_autoconfig` deasserting). `hrm/appK-autoconfig-size-encoding` already
built and measured this write path (`MOVE.W #imm,(abs).L`, opcode
`0x33FC`); reused here unchanged.

## What this package tests, by group

The 30 claims cluster into 8 testable groups (see the claims file for the
full id list; only the mechanism is summarised here):

1. **Lifecycle** (8 claims: `sol:K-0727,K-0734,K-0739,K-0842,K-0843`;
   `opus:K-0755(p431),K-0879,K-0880`) — a card responds in $E8xxxx while
   unconfigured (`autocfg_card` reset value `1`,
   `cpu_wrapper.v:607-608`), and stops responding once the final
   base-address write completes (`cpu_wrapper.v:630-632` register 48 for
   Zorro II — the configuration used here). Tested by reading register 00
   before any write (expect the real type nibble) and again after writing
   register 48 (expect silence: `chip_data`'s passthrough, since `mem` has
   no entry at `$E80000` other than what the fixture's own memory model
   supplies, which defaults to 0).
2. **Handoff** (1 claim: `sol:K-0733`) — `autocfg_card`'s three reachable
   values (`1` → `{fastramcfg_d[0],0}` → `0`, `cpu_wrapper.v:634-644`)
   model a single unconfigured responder handed off in sequence: after the
   first Zorro III board's base-address write, the space keeps responding
   (for a still-unconfigured second region) until the second write
   silences it. Tested with two successive writes to register 44 under
   Zorro III, reading register 00 between and after each.
3. **Read/write split** (2 claims: `sol:K-0735`, `opus:K-0762`) — register
   44 (a write-only control register) reads as the unmapped default before
   any write, and a write to register 00 (a read-only identification
   register) has no configuring effect. Tested directly.
4. **Read-register content** (2 claims: `sol:K-0737`, `opus:K-0764`) —
   read registers carry real, board-dependent attributes (type nibble
   differs Zorro II vs Zorro III). Reuses the Zorro-II/Zorro-III type
   contrast `hrm/appK-autoconfig-register-map`'s Group A already
   established.
5. **Address window** (4 claims: `sol:K-0742,K-0743,K-0744`;
   `opus:K-0767`) — the 64 KiB block at `$00E8xxxx`
   (`cpu_wrapper.v:601`'s `chip_addr[23:16]==8'b11101000` check, which
   only examines the top byte) is the whole decoded window: an address one
   byte outside it (`$E90000`) never gets the AUTOCONFIG override, and an
   address *inside* it but past the 6-bit `chip_addr[6:1]` case range
   (`$E80100`) aliases back to register 00 rather than falling silent —
   confirming the decode covers the full 64 KiB, not just the first
   0x7E bytes some registers are mapped at.
6. **One nybble only** (2 claims: `sol:K-0750`, `opus:K-0771`) —
   `cpu_wrapper.v:235`'s `{sel_autoconfig ? autocfg_data :
   chip_data[15:12], chip_data[11:0]}` substitutes exactly 4 bits; the
   other 12 pass through the chip bus untouched. Tested by presetting a
   recognisable low-12-bit pattern on the responding `chip_dout` and
   confirming it survives unmodified alongside the overridden top nybble.
   Graded on "returns exactly one nybble of real content"; the page's
   `D31-D28` bit-lane language names a 32-bit physical bus this design's
   16-bit chip bus does not carry lines for — noted, not declined (the
   claim's substantive content, "only one nybble is live," is fully
   testable and is what is graded; see `note()` in the test output).
7. **Register width / paired addressing** (6 claims: `sol:K-0752,K-0753,
   K-0757,K-0760`; `opus:K-0773,K-0779`) — every logical 8-bit register is
   two nybbles at two consecutive word addresses (register 00 at
   `$E80000`/`$E80002`, established by `hrm/appK-autoconfig-register-map`;
   register 3C at `$E8003C`/`$E8003E`, both falling to the unmapped
   default per `hrm/appK-autoconfig-reserved-registers`'s own index
   arithmetic, reused here rather than recomputed).
8. **Complement rule** (5 claims: `sol:K-0755(p432),K-0756,K-0759`;
   `opus:K-0776,K-0777`) — every register except 00 is physically
   complemented (`cpu_wrapper.v:541` default `4'b1111` reads as logical
   `0`; register 00 at index 0 *and* index 1 both read direct, per
   `hrm/appK-autoconfig-register-map`'s own established rule — "the 00
   register" names the whole two-address logical register, not just
   index 0). Register 3C is the worked example: both its raw nybbles read
   `1111`, both its logical value is `0`.

## Declined claims

None. All 30 are directly observable at `cpu_wrapper`'s own ports.

## What this build actually does

Three harness configurations, reusing the write-capable Harness from
`hrm/appK-autoconfig-size-encoding`:

- **ZII** (`cpucfg=0`, `fastramcfg=1`): reads register 00 (before), 3C
  (both nybbles), 44 (write-only, read as default); writes register 00
  (no-op check) then reads it again; writes register 48 (completes
  configuration) then reads register 00 again (expect silence); reads
  `$E90000` (outside) and `$E80100` (inside, aliased) with presets.
- **ZIII-handoff** (`cpucfg=2`, `fastramcfg=5`): reads register 00, writes
  register 44 twice (one per board), reading register 00 between and
  after.
- **Nybble-only probe** (`cpucfg=0`, `fastramcfg=1`, dedicated): reads
  register 00 with `chip_dout` preset to a recognisable low-12-bit pattern
  at `$E80000`, capturing the full 16-bit word.
