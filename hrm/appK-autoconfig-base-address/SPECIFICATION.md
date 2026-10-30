# Specification — `appK-autoconfig-base-address`

Package: `hrm/appK-autoconfig-base-address`
Fixture (top module, per task assignment): `cpu_wrapper`
Claims file: `hrm-testpipeline/work/appK-build/appK-autoconfig-base-address.jsonl`
(23 claims, printed pages 431, 435-436, Appendix K, "Base Address
Registers" / "Shut Up Register" / the Zorro-II-configuration-block prose
that separates registers 44/48/4C)

## Disclosure on the write-order

Reused wholesale from `hrm/appK-autoconfig-register-map`,
`hrm/appK-autoconfig-size-encoding` and `hrm/appK-autoconfig-space`: the
`Harness` mechanism (including its write capability, `MOVE.W #imm,
(abs).L`), core selection, active-low reset, direct/complement arithmetic.
Same write-order disclosure applies.

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
`hrm/appK-autoconfig-base-address-fx68k`
builds the same test source with fx68k, which has one instance for
simulation and synthesis alike.

## The candidate finding — read this before the rest

Printed page 436 separates two configuration blocks. In the **Zorro II**
block: *"writing to register 48 actually configures the board for both
Zorro II and Zorro III boards in the Zorro II configuration block"*
(`K-0844`, `K-0881`). Only in the separately-described **Zorro III** block
does register **44** complete configuration.

`cpu_wrapper.v:629-665` implements exactly one write-completion path,
gated on `cfg_z3` (`cpu_wrapper.v:536`, `fastramcfg_d[2] & cpucfg[1]`;
this package builds with `ENABLE_RAM32`, the one configuration in which
`cpu_wrapper` compiles a Zorro III path):

```
if (~cfg_z3) begin
    if (chip_addr[6:1] == 6'b100100) begin // Register 0x48 - config, ZII RAM
        z2ram_ena <= 1;
        autocfg_card <= 2'b00;
`ifndef ENABLE_RAM32
        if (fastramcfg_d[1:0] == 2'b11) begin
            case (autocfg_card)
                2'b01: autocfg_card <= 2'b10;  // Extra 1MiB of RAM stolen from SlowRAM
                2'b10: autocfg_card <= 2'b11;  // Extra 0.5MiB of RAM stolen from SlowRAM
                default: ;
            endcase
        end
`endif
    end
end else if (chip_addr[6:1] == 6'b100010) begin // Register 0x44, assign base address to ZIII RAM.
    ...
end
```

The `` `ifndef ENABLE_RAM32 `` block is not compiled in this build.

When `cfg_z3=1` (this design's Zorro III mode — the only mode in which the
page's claim about register 48 would matter, since the page is explicit
that register 48 configures Zorro III boards too), a write to `6'b100100`
(register 48, `$E80048`) matches **neither** branch: the `if (~cfg_z3)`
guard is false, so the `else if` alternative (register 44 only) is the
only path evaluated. **Register 48 has no effect at all while
`cfg_z3=1`.** This directly contradicts the page: NanoMig's Zorro III
board is configured *only* by writing register 44; writing register 48
while configured for Zorro III does nothing. Tested directly in the
Harness below (`Group Finding`) — **graded as the page states it; the RTL
disagrees, and the assertion goes red and stays red.**

`hrm/appK-autoconfig-register-map`'s own boundary investigation already
established that NanoMig's Zorro III board configures in the *same*
`$E80000-$E8007E` window as Zorro II (`cfg_z3` gates content, not a
distinct `$E80100+` decode) — so "the Zorro II configuration block" is, in
this design, the *only* block a Zorro III card is ever configured through.
The page's own claim that register 48 works there for both board types is
therefore not testing an edge case this design routes around a different
way; it is the only path by which `cpu_wrapper` configures a Zorro III
board, though no board top enables that board.

## Two further divergences, and one more they lead to

- **Register 44 latches only `cpu_dout[15:12]`/`cpu_dout[15:11]`** — 4 or
  5 bits — **not the full A31-A24 (8 bits) the page describes**
  (`cpu_wrapper.v:655,660`).
- **Register 48 latches no address at all** (`cpu_wrapper.v:640-642` sets
  `z2ram_ena <= 1` and clears `autocfg_card`; no `cpu_dout` bits are
  captured anywhere in that branch).

Both of these are claims about the *specific bit content* of a
write-only register with no read-back path anywhere in this design (no
case arm in either `autocfg_data` branch reads back `$E80044`/`$E80048`;
confirmed by reading `cpu_wrapper.v:565-608` in full). Observing what was
actually latched would require either an internal probe of
`z3ram_base0`/`z3ram_base1`/`z2ram_ena` (forbidden — no debug ports) or a
live RAM-model fixture driving `ramsel`/`ramready` so a matching CPU
access could be observed reaching the internal RAM path instead of the
chip bus — the same boundary `hrm/appK-autoconfig-size-encoding` already
declined the "natural boundary alignment" claim over, for the same reason
(this fixture's permanently-low `ramready` cannot safely exercise that
path without risking the kind of hang `hrm/appK-autoconfig-register-map`'s
own SPECIFICATION.md records from this fixture's early development).
**Every claim whose entire content is the specific bit-range latched by
register 44/48 is declined** on this boundary (`reason_kind:
unreachable-observable`) — 11 of the 23 claims. The claims that assert
only the *register's role* (write-only, and that writing it completes
configuration in its own primary context) remain gradeable: the
completion event is directly observable via `sel_autoconfig`, independent
of what specific bits were captured.

- **Register 4C (shut-up) has no handler at all.** Confirmed by reading
  `cpu_wrapper.v:629-665` in full: the write-detect process recognizes
  only `6'b100100` (register 48) and `6'b100010` (register 44) — `6'b100110`
  (register 4C, `$E8004C`) never appears anywhere in it. The Zorro II
  card (the configuration this design's own default `fastramcfg`/`cpucfg`
  reach) reads register 08 bit 6 as `0` (register-map package's own
  Group F-bit6: unmapped index 4 falls to the `4'b1111` default at
  `cpu_wrapper.v:547`, which complements to logical `0000`, so bit 6
  — nibble bit 2 — is `0`; the *Zorro III* card (index 4 *is* mapped,
  `cpu_wrapper.v:598`) complements to bit6=`1` ("not silenceable").
  Re-derived directly here rather than assumed:
  `hrm/appK-autoconfig-register-map`'s own Group F-bit6 measured this
  exactly — `sol:K-0785(p433)`: "register08 bit6 clear -> can be shut up
  by software (Zorro II default)" graded PASS there. So the Zorro II
  card *does* advertise shut-up support (bit6=0), and this design
  implements no mechanism that honors it. Tested directly (`Group
  Shutup`) — **graded as the page states it; red and stays red.**

## Claim inventory (23)

| group | claims | grading |
|---|---|---|
| Finding (reg48-completes-ZIII) | `sol:K-0844`, `opus:K-0881` | **graded, expected RED** |
| Shut-up (write to 4C silences a bit6=0 board) | `sol:K-0740,K-0849,K-0850`; `opus:K-0889` | **graded, expected RED** |
| Shut-up identity (4C is *functionally* the shut-up register) | `sol:K-0848`; `opus:K-0888` | **graded, expected RED** (no code path at all treats `$E8004C` specially — a strictly stronger non-implementation than register 44/48's narrower-than-described one) |
| Register identity (44/48 write-only, complete config in own primary context) | `sol:K-0834,K-0835`; `opus:K-0866,K-0867` | graded, expected PASS |
| Register bit-content (which bits, nybble vs byte width) | `sol:K-0836,K-0837,K-0839,K-0841`; `opus:K-0868,K-0869,K-0870,K-0873,K-0874,K-0877,K-0878` | **declined**, `unreachable-observable` |

8 + 4 + 11 = 23.

## A note on the MUTATION requirement for already-red assertions

The build brief's `MUTATION` template ("naming a stimulus change that
turns it red") presumes a passing baseline. Eight of this package's
graded assertions are **expected to fail already, in the baseline build**
— that is the finding. For those eight, this file's test uses the
inverse sanity check instead: a guarded mutation (`GREENCHECK_*`) that
swaps in the *one* context where the same underlying mechanism genuinely
does succeed (Zorro II for the register-48 claims; register 48 itself, in
place of 4C, for the shut-up claims), showing the check reads green
there. This demonstrates the assertion is a real, discriminating
measurement — not a label that would print `FAIL` unconditionally
regardless of the RTL — without violating "never adjust an expected value
to make a test pass": the *graded* assertions' expected values are never
touched; only a `MUT_GREENCHECK_*`-guarded diagnostic build exists to
prove the check apparatus itself is sound. This departure from the literal
template is disclosed here rather than silently forced into the "turns it
red" shape where it does not fit.

## What this build actually does

- **Group Finding**: Zorro III harness (`cpucfg=2`, `fastramcfg=5`);
  read register 00 (before), write register 48, read register 00 again.
  Page says the board should now be silent (configured); RTL leaves it
  responding.
- **Group Shutup**: Zorro II harness (`cpucfg=0`, `fastramcfg=1`); read
  register 08 (confirm bit6=0, "can be shut up" is advertised), read
  register 00 (before), write register 4C, read register 00 again. Page
  says a bit6=0 board must now be silent; RTL leaves it responding.
- **Group Identity**: register 44 tested for write-only + configures under
  Zorro III (its own primary context, per the RTL's own comment); register
  48 tested for write-only + configures under Zorro II (its own primary
  context).
