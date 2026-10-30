# Specification — `appK-autoconfig-space-decode`

Package: `hrm/appK-autoconfig-space-decode`
Fixture (top module, per task assignment): `cpu_wrapper`
Claims file: `hrm-testpipeline/work/appK-build/appK-autoconfig-space-decode.jsonl`
(23 claims, printed pages 386–412, Appendix K, on the AUTOCONFIG **space and its
decode** — not the register contents, which the sibling package
`hrm/appK-autoconfig-register-map` grades)

## Disclosure on the write-order

The task brief for this package names the sibling package
(`hrm/appK-autoconfig-register-map`) as the place the CPU-core boundary problem
was already solved, and instructs this package to inherit that solution rather
than rediscover it. That means RTL and boundary knowledge (which core
elaborates, which port is active-low, which ports are observable) already
existed before a single proposition below was written — the same break from
"write the spec blind, then open the RTL" that both `ch1-cpu-and-instruction-
cache` and `hrm/appK-autoconfig-register-map` already disclosed for the same
underlying reason. Re-confirmed directly against this checkout rather than
taken on faith from those documents (documents drift; the RTL does not):

- `cpu_wrapper.v:44-99`'s own port list — read in full for this package — has
  `output reg [23:1] chip_addr` at line 60. Bit width confirmed by direct
  reading, not inference: `chip_addr` cannot represent any address at or above
  $1000000 (max representable byte address is $FFFFFE). This matters below.
- `cpu_wrapper.v:403-470` — the `TG68KdotC_Kernel` instantiation this package
  reuses (via `-DENABLE_TG68K=1`, same as the sibling) — reaches no
  function-code (`FC`) port on either build path: the `` `ifdef
  VERILATOR`` port list (lines 420-441) makes no `fc` connection, and the
  non-`VERILATOR` (synthesis) instantiation leaves `.FC()` open (line 462).
  This is not because the kernel lacks an FC port: `nanomig/sim/
  TG68KdotC_Kernel.v` (35,330 lines) declares `output [2:0] fc,` (line 7453)
  and drives it (`assign fc = n9523_o;`, line 13746) — cpu_wrapper.v simply
  never wires it to either TG68K instance. `fc_o` (declared
  `cpu_wrapper.v:244`, a separate net) is read only inside the
  `` `ifdef ENABLE_FX68K`` block (lines 299, 497) and driven only inside the
  same block (lines 493-495) — with `ENABLE_FX68K` undefined in this build
  (only `ENABLE_TG68K` is), `fc_o` is neither driven nor read anywhere in the
  compiled design: a fully dangling wire. This matters below (Declined
  claims).
- `cpu_wrapper.v:34` `` `define TG68K_A24      // limit address space of
  TG68K to 24 bit`` is unconditional (not guarded by any `` `ifdef``), so it
  applies to this build too: `cpu_addr = {8'h00, cpu_addr_p[23:0]}`
  (`cpu_wrapper.v:279`) — the CPU itself can never generate an address with
  bits 31:24 set. This matters below (why the Zorro-III relocatable RAM base
  is out of scope for this package).

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
`hrm/appK-autoconfig-space-decode-fx68k`
builds the same test source with fx68k, which has one instance for
simulation and synthesis alike.

## Boundary re-confirmed for this package

`cpu_wrapper`'s bus ports are outputs driven by whichever CPU core it
instantiates; the sibling package's boundary investigation (re-read in full
before this file was written) builds `TG68KdotC_Kernel` from
the TG68K kernel of its Core section via `-DENABLE_TG68K=1`. The default core,
`fx68k`, fails this project's pinned Verilator under its default checks
(`%Error-BLKANDNBLK` at `fx68k.sv:293`); it builds with `-Wno-BLKANDNBLK`, as
NanoMig's own `sim/Makefile` does, and runs with `--no-assert`, and so built
it gives this package's every result line as TG68K does. This package uses
the sibling's build selection, inherited rather than re-derived. `reset` is active-low
(`cpu_wrapper.v: always @(posedge clk, negedge reset)`, `.nreset(reset)`) —
also inherited, and load-bearing below since this package, unlike the sibling,
deliberately re-asserts `reset` *mid-run* to test that it restores the
unconfigured state.

## What this package is about

Not the register contents (the sibling's job) but the AUTOCONFIG **space**
and its decode, per the task brief:

1. an unconfigured card answers in a 64K block starting at $00E80000
   (`cpu_wrapper.v:611`);
2. it answers there only while its configuration-chain input permits it
   (`autocfg_card`, gated into `sel_autoconfig` at the same line, cleared by
   the register-0x48 write handler at `cpu_wrapper.v:639-652` — the
   register-0x44 handler at `:653-663` is never reached in this build, see
   below);
3. reset returns the logic to unconfigured (`cpu_wrapper.v:617-623`);
4. a base address gets assigned per card (the same write handlers, observed
   through the RAM-request ports `ramreq`/`ramaddr`, `cpu_wrapper.v:81-88`,
   fed by `sel_z2ram`/`sel_z3ram0`/`sel_z3ram1` at `cpu_wrapper.v:115-118`).

## The four mechanisms, and where each is decoded (re-read directly, this file)

- **`sel_autoconfig` (`cpu_wrapper.v:611`)**: `|fastramcfg_d && chip_addr[23:16]
  == 8'b11101000 && |autocfg_card`. The window is exactly 64K
  ($E80000-$E8FFFF, the RTL's own comment on the same line) — decoded off
  `chip_addr[23:16]` alone; bits `[15:0]` never affect *whether* the window
  answers, only *which* nibble (`cpu_wrapper.v:546-609`, `autocfg_data`,
  indexed by `chip_addr[6:1]` — a 6-bit word index, so bits `[15:7]` of the
  byte offset alias every 128 bytes of the 64K block to the same case arms;
  unmapped offsets fall to the unconditional `autocfg_data = 4'b1111;` at
  line 547 (set before either case statement runs, not a case `default:`
  arm itself — both case statements' own `default:` arms, `:589`/`:605`,
  are empty and simply leave it standing) — still *intercepted*, just with
  the default nibble). Grep of the
  whole file for any other `chip_addr[23:16]` comparison found exactly one:
  this line. No other decode logic in this design references any `$E9xxxx`-
  `$EFxxxx` address.
- **`autocfg_card` reset (`cpu_wrapper.v:617-623`)**: on `~reset | ~reset_out`,
  `autocfg_card <= 2'b01` (unconfigured), `z2ram_ena <= 0`,
  `z3ram_ena0/1 <= 0`, `z3ram_base0/1 <= 1`. One `reset` port; no distinct
  `/BUSRST` or `/IORST` signal exists at this boundary (both page-394 and
  page-411 claims below are graded against this single mechanism — the
  signal-*name* distinction is not something this design can carry, as
  `classify.evidence` for `sol:K-0432`/`opus:K-0438` already states).
- **Register 0x48, Zorro II (`cpu_wrapper.v:639-652`)**: on a write to
  `chip_addr[6:1]==6'b100100` (byte $E80048) while `sel_autoconfig` and
  `~cfg_z3`: `z2ram_ena <= 1; autocfg_card <= 2'b00;`. This is the "software
  tells the board where to reside" write — data-independent (no field of the
  write is read for this register), it is a pure enable pulse. (This build
  also compiles a chain-advance sub-block, `cpu_wrapper.v:643-651`, that
  instead steps `autocfg_card` `2'b01`→`2'b10`→`2'b11` when
  `fastramcfg_d[1:0]==2'b11`; none of this package's harnesses set
  `fastramcfg` to such a value, so every write graded here ends
  configuration in this one step.)
- **The resulting fixed RAM window, Zorro II (`cpu_wrapper.v:117`)**:
  `sel_z2ram = (cpu_addr[31:24]==8'h00) && (cpu_addr[23] ^ |cpu_addr[22:21])
  && z2ram_ena`. Hand-expanded (all 8 values of `cpu_addr[23:21]`): true for
  `cpu_addr[23:21]` in `{1,2,3,4}`, i.e. exactly `$200000`-`$9FFFFF` (8 MB),
  matching the comment `// addr[23:21] = 1..4` and gated only by
  `z2ram_ena`. It feeds `sel_zram` (`:118`) → `ramsel` (`:167`) →
  `ramreq`/`ramaddr` (`:168`, `:208-220`), all genuine output ports.

## Why the Zorro-III relocatable base ($E80044, `z3ram_base0`/`z3ram_base1`)
## is out of scope here

In this build `cfg_z3` is wired to the constant `0` (`cpu_wrapper.v:
535-540`, no `ENABLE_RAM32`): no Zorro III board exists here, and the
register-44 handler (the `else if (chip_addr[6:1] == 6'b100010)` branch,
`cpu_wrapper.v:653-663`) is never reached by any stimulus, because
`if (~cfg_z3)` is always taken instead. A write to $E80044 has no effect at
all in this build — not a partial or unobservable one.

(In a build that does define `ENABLE_RAM32`, where `cfg_z3` can be driven
to `1`, the handler sets `autocfg_card <= {fastramcfg_d[0], 1'b0}` on the
first write [`autocfg_card==2'b01`, `cpu_wrapper.v:657`] or `autocfg_card
<= 2'b00` on the second [`cpu_wrapper.v:662`], and `sel_z3ram0`/
`sel_z3ram1` [`cpu_wrapper.v:115-116`] compare `cpu_addr[31:27]`/
`cpu_addr[31:28]` against the written base; under `` `TG68K_A24``
[unconditional, confirmed above] `cpu_addr[31:24]` is hardwired to `8'h00`
by this fixture's CPU core, so even there no *non-zero* written base could
ever be observed to matter.)

Claims about "software assigns a base" (`opus:K-0269`, `sol:K-0108`,
`opus:K-0123`) are graded using the Zorro-II mechanism instead — which one
of the three claims (`opus:K-0269`) explicitly invites ("...just as on the
Zorro II bus").

## Claim inventory (all 23)

| group | claim (restatement) | uids | page |
|---|---|---|---|
| SWEEP-begin | Upper Zorro II I/O region begins at $00E80000 | `sol:K-0041` | 386 |
| SWEEP-block | Unconfigured PIC (w/ CFGIN asserted) responds in 64KiB block at $00E80000 | `sol:K-0172`, `opus:K-0179` | 395 |
| SWEEP-interrogate | System software interrogates each board (real data present, not open bus) | `sol:K-0106`, `opus:K-0122` | 391 |
| SWEEP-typesize | AUTOCONFIG reports board type *and* required space, as distinguishable facts | `sol:K-0107` | 391 |
| DECLINE-wideio | Zorro II I/O labelled band spans $00E80000 up to $00F00000 / $00EFFFFF | `opus:K-0055`(386), `sol:K-0058`(388), `opus:K-0075`(388) | 386,388 |
| CONFIG-reside | AUTOCONFIG write tells the board where to reside (old window silenced, new window live) | `sol:K-0108`, `opus:K-0123` | 391 |
| CONFIG-unique | Process assigns a unique (bounded) address range per card, "just as on Zorro II" | `opus:K-0269` | 401 |
| CONFIG-8mb | Zorro II memory space is the 8 MB range $00200000-$009FFFFF | `opus:K-0052`(386), `sol:K-0057`(388), `opus:K-0074`(388) | 386,388 |
| RESET | Expansion device resets its AUTOCONFIG logic on bus reset (/BUSRST p394, /IORST p411 — one `reset` port here) | `sol:K-0153`(394), `opus:K-0162`(394), `sol:K-0432`(411), `opus:K-0438`(411) | 394,411 |
| DECLINE-fc | FC0-FC2 encode cycle type / memory space | `sol:K-0221`, `opus:K-0229` | 398 |
| DECLINE-zIII-ff | Unconfigured Zorro III PIC responds at $00E80000 **or** $FF000000 | `sol:K-0456`, `opus:K-0458` | 412 |

Count: SWEEP-begin=1, SWEEP-block=2, SWEEP-interrogate=2, SWEEP-typesize=1,
DECLINE-wideio=3, CONFIG-reside=2, CONFIG-unique=1, CONFIG-8mb=3, RESET=4,
DECLINE-fc=2, DECLINE-zIII-ff=2.
Sum = 1+2+2+1+3+2+1+3+4+2+2 = **23**. 16 graded, 7 declined.

## Declined claims (7), and the boundary that would answer them

- **`opus:K-0055`(p386), `sol:K-0058`(p388), `opus:K-0075`(p388)** — each
  claims the *whole* labelled "Zorro II I/O" band, $00E80000 up to
  $00F00000/$00EFFFFF (512 KB — room for up to eight 64K board slots on real
  Zorro II hardware). This design answers AUTOCONFIG at a single fixed,
  non-relocatable 64K window (`sel_autoconfig`, `cpu_wrapper.v:611`,
  confirmed by grep to be the *only* `chip_addr[23:16]` comparison in the
  file). In this build (no `ENABLE_RAM32`) up to three Zorro II cards
  configure through that *same* window one after another — the
  chain-advance at `cpu_wrapper.v:643-651` steps `autocfg_card` after each
  register-0x48 write instead of ending configuration, so a second and
  third card can present themselves at $00E80000-$00E8FFFF in turn, never
  side by side and never at a different address. There is no I/O-board
  configuration path (the write handlers at `cpu_wrapper.v:639-664` only
  ever enable RAM banks, never relocate an I/O decode), and nothing in this
  design ever answers, or could be made by any stimulus at this boundary to
  answer, anywhere in $E90000-$EFFFFF. Declined: `unreachable-behaviour`,
  named at `cpu_wrapper.v:611` (the fixed 64K window) and the absence of
  any other decode in the file. Each claim states strictly more than this
  design implements — the shared 64K start point *is* graded, under
  `sol:K-0041` above.
- **`sol:K-0221`, `opus:K-0229`** (p398) — FC0-FC2 as cycle-type/memory-space
  bits. `fc_o` (`cpu_wrapper.v:244`) is driven only inside the
  `` `ifdef ENABLE_FX68K`` block (`:493-495`) and read only inside the same
  block (`:299`, `:497`). This package builds `TG68KdotC_Kernel`; with
  `ENABLE_FX68K` undefined, `fc_o` is a fully undriven, unread wire in the
  compiled design. The shipped `nanomig/sim/TG68KdotC_Kernel.v` (35,330 lines) does
  declare its own function-code port — `output [2:0] fc,` (line 7453),
  driven by `assign fc = n9523_o;` (line 13746) — but cpu_wrapper.v never
  connects it: the `` `ifdef VERILATOR`` TG68K instantiation (`:420-441`)
  makes no `fc` connection, and the non-`VERILATOR` (synthesis)
  instantiation leaves `.FC()` open (`:462`). So FC reaches no port of
  `cpu_wrapper` in a TG68K build either, not because the kernel has no FC
  port. In an fx68k build (it builds with `-Wno-BLKANDNBLK` and runs with
  `--no-assert`) `fc_o` is driven but still reaches no port of
  `cpu_wrapper`: it leaves only as the two-bit cycle type on the `cpustate`
  output (`:299-303`: fetch, data read, write), which carries neither FC2
  nor CPU space, and as the autovector request (`:497`). No stimulus at
  this boundary reaches FC0-FC2 themselves in either build. Declined:
  `unreachable-port`, named at `cpu_wrapper.v:420-441` (the
  `` `ifdef VERILATOR`` TG68K instantiation, no `fc` connection) and `:462`
  (the synthesis TG68K instantiation, `.FC()` left open) vs. `:493-495`
  (fx68k-only). The boundary that would answer this is the CPU core's own
  FC outputs.
- **`sol:K-0456`, `opus:K-0458`** (p412) — an unconfigured Zorro III PIC
  responds at $00E80000 **or** $FF000000. In this build `cfg_z3` is wired
  to `0` (`cpu_wrapper.v:535-540`, no `ENABLE_RAM32`): no Zorro III case is
  reachable at all. `sel_autoconfig` (`cpu_wrapper.v:611`) does not consult
  `cfg_z3` — the window address does not change with the board type, only
  *which* `autocfg_data` case-branch would apply if `cfg_z3` were `1` — so
  the $00E80000 half is the *same* fact as `sol:K-0172`/`opus:K-0179`
  above regardless. The $FF000000 half has no carrier in this design, checked
  directly for this package: `chip_addr` is declared `[23:1]`
  (`cpu_wrapper.v:60`), so its maximum representable byte address is
  $FFFFFE — $FF000000 needs bit 24 and above, which do not exist on this
  port. (`` `TG68K_A24`` compounds this: even the CPU core internally cannot
  generate such an address.) Per the task brief: grading this claim green on
  only the implemented half, or red against an address the fixture cannot
  even present, would both misrepresent what was tested. Declined:
  `unreachable-address`, named at `cpu_wrapper.v:60`. The boundary that
  would answer the $FF000000 half is a 32-bit-address variant of this
  module's bus ports — none exists in this design.

## What this build actually does

Three independent fixture runs, each a real reset → 68000 program →
AUTOCONFIG/RAM-space bus-cycle sequence through `cpu_wrapper`'s own ports
only (never an internal signal read or forced):

1. **Sweep** (Zorro II, unconfigured, `cpucfg=0`, `fastramcfg=1`, no writes):
   reads five addresses — $E70000, $E80000 (offset 0), $E80002 (offset 1),
   $E8FFFE (offset 0x3F, the window's last word), $E90000 — each preset in
   the fixture's own chip-bus memory model to a **sentinel** word
   ($5xxx, top nibble `0x5`, confirmed absent from every literal
   `autocfg_data` assignment reachable with this package's harness
   [`fastramcfg=1`]; `cpu_wrapper.v:579` assigns `4'b0101` at index 1 after
   a register-0x48 write with `fastramcfg_d=3`, a value this harness's
   stimulus never produces) so that
   "intercepted" (nibble != sentinel) vs. "passed through" (nibble ==
   sentinel) is unambiguous regardless of which real nibble the RTL
   substitutes.
2. **Config** (Zorro II, `cpucfg=0`, `fastramcfg=1`): reads $E80000 (expect
   intercepted), writes $E80048 (the register-0x48 enable pulse), re-reads
   $E80000 (expect passed-through/silenced), then reads four addresses
   bounding the resulting RAM window — $1FFFFE, $200000, $9FFFFE, $A00000 —
   observed not by their (irrelevant) data content but by whether `ramreq`
   was seen asserted, logged every cycle against the concurrently-observed
   `chip_addr` (which reflects the CPU's requested address on this port
   regardless of which internal path serves it).
3. **Reset**: reads $E80000 (baseline), writes $E80048, re-reads $E80000
   (expect silenced) — then, externally, asserts `reset` for 20 cycles and
   releases it. `TG68KdotC_Kernel` restarts from the reset vectors exactly
   as at power-up and the *same* program runs again from $001000; the
   fixture discards its first capture of the $E80000 probe and waits for a
   fresh one, which (if reset genuinely restored `autocfg_card`) shows
   intercepted data again.

`ramready`/`ramdout` are served identically in all three harnesses (zero
added wait states, mirroring the sibling's `chip_dtack` tied constant
asserted): whenever `ramreq` is observed high, `ramready` is driven high and
`ramdout` supplies 0 — the RAM-space claims graded here depend only on
*whether* `ramreq` fires for a given `chip_addr`, never on the data returned.
