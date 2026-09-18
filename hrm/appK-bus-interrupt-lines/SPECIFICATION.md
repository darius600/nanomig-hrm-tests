# appK-bus-interrupt-lines

Amiga Hardware Reference Manual, 3rd edition, Appendix K, pp.391, 394, 411
("Zorro II Bus Architecture" / "System Interrupts" / "System Control
Signals"). Claims file:
`hrm-testpipeline/work/appK-build/appK-bus-interrupt-lines.jsonl`, 7
claims, all `status: testable`, `boundary: paula_intcontroller`.

**Order of work, stated plainly.** The task assignment already named the
top module (`paula_intcontroller`) and gave specific RTL citations
(`paula_intcontroller.v:15,17,112,132,152-170`) before this file was
written, and stated as background fact that `int2` and `int6` are the only
externally sourced interrupt inputs besides `int3`. That context could not
be un-seen. What follows is nonetheless argued from each claim's own
`quote`/`restatement` fields; the register addresses, stimulus values and
pass/fail conditions below are step-3 work, decided only after this
section was drafted.

## What the seven claims assert

Three groups, by page:

- **p.391, "Zorro II Bus Architecture"** -- sol:K-0103 ("Zorro II discrete
  interrupt lines are combined into the 68000 encoded interrupt inputs.")
  and opus:K-0120 (same sentence, independent extraction). A general
  claim: discrete bus interrupt lines are *mixed* into the 68000's
  standard *encoded* interrupt levels.
- **p.394, "System Interrupts"** -- sol:K-0160 ("The supported Zorro II
  interrupt inputs are /INT2 and /INT6.") and opus:K-0168 (same sentence,
  plus a truncated remark that the A2000 hardware did not provide more).
  A naming/completeness claim: of the bus's interrupt lines, only /INT2
  and /INT6 are the ones actually supported.
- **p.411, "System Interrupts" / "System Control Signals"** --
  sol:K-0439 ("Two of the decoded, level-sensitive 680x0 interrupt inputs
  are available"), sol:K-0440 ("labeled as /INT2 and /INT6."), and
  opus:K-0444 (the same two facts combined in one sentence, plus "...on
  the expansion bus" and a truncated continuation about sharing). A
  specific claim: exactly two named, level-sensitive lines exist, and
  they decode to two distinct 68000 interrupt priority levels.

Taken together, all seven claims reduce to one testable proposition at
this boundary: **`paula_intcontroller` has exactly two externally-driven,
level-sensitive interrupt inputs among its `intN`-named ports --
`int2` and `int6` -- and driving each one alone produces its own,
distinct, encoded 68000 interrupt-priority level.** (`int3`, the third
`intN`-named port on this module, exists but -- per the task assignment's
own given fact -- is not one of the externally-sourced pair the page
means; nothing here grades a claim about `int3`.)

## Boundary

`opus:K-0444`'s own clause "...available on the expansion bus" describes
physical Zorro wiring outside this module -- `paula_intcontroller` has no
port that says where its `int2`/`int6` inputs are physically routed. That
half of the sentence is not exercised by this fixture; the "two lines,
two distinct decoded levels" half is, and that is what is graded under
this uid (see the test file's own note on this point).

The "only /INT2 and /INT6 ... actually supported" completeness half of
K-0160/K-0168 is partly a statement about which ports this module
declares at all (checkable: no `int1`/`int4`/`int5`/`int7` port exists on
`paula_intcontroller`, confirmed by the generated Verilator header
compiling only against the ports the source declares) and partly a
statement about which of the declared `intN` ports are externally-sourced
on real Zorro II hardware (not checkable from this module's ports alone --
that is board-level wiring, supplied as a given fact by the task
assignment, not re-derived here). Both claims are graded on the
behavioral half (driving `int2`/`int6` and reading the decode) with the
port-list fact noted for completeness, not separately declined, since the
uid's own restatement is about which lines are supported, not about
wiring topology.

## What will be driven and observed

Fixture: `paula_intcontroller` alone. `clk7_en` is held high for the
whole run (matching this suite's userio fixture, which does the same for
its own always-enabled clock-gate input) so every `tick()` is an active
edge. `reg_address_in`/`data_in` drive `INTENA`/`INTREQ`
(register addresses `$09A`/`$09C`, read back via `INTENAR`/`INTREQR` at
`$01C`/`$01E` -- the same four addresses the parameter block at
paula_intcontroller.v:27-30 names). All other hardware interrupt inputs
(`rxint`, `txint`, `vblint`, `int3`, `blckint`, `syncint`, `audint`) are
held low throughout, so only `int2`/`int6` can move `_ipl`.

To isolate one line: clear `INTENA`/`INTREQ`, enable only the master bit
and the one bit the line under test sets (`int2` sets `intreq[3]`, `int6`
sets `intreq[13]`, per the assignment's own citation), pulse that line
high then low, and read `_ipl`.

A liveness check with no claim uid (labelled `[not graded -- apparatus
check, no claim uid]`) confirms `INTENAR`/`INTREQR` readback changes with
a register write before any graded read is trusted.

## What would make each assertion fail

Driving `int2` alone (with only its own bit enabled) and not observing
`_ipl==2`; driving `int6` alone and not observing `_ipl==6`; or the two
readings coinciding (failing to show two *distinct* levels) -- any of
these would mean the RTL's discrete-to-encoded reduction does not match
what the page states for these two lines.

## Evidence not used until step 3

`classify.evidence` for all seven claims cites `paula_intcontroller.v:15`
(`int2` port), `:17` (`int6` port), `:112`/`:132` (the OR-in of each line
into `intreq`), and `:152-170` (the priority encoder); those exact lines
were opened only after the propositions above were fixed, to build the
fixture and to compute the correct register addresses.
