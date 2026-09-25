# apparatus-vasm-program

Apparatus check for 68000 test programs assembled with vasm and run through
`integration/harness/nanomig_harness.h`. It proves, on both CPU cores, the
four things every integration package built on `integration/harness/nanomig.mk`
relies on:

1. `load_program()` places the assembled binary at `kProgramOrg` ($080400, in
   the chip RAM `nanomig.v` maps one-to-one) and points the reset vectors at
   it, and the CPU really starts there: after reading its reset vectors from
   the overlay shadow, its next fetch on the RAM port is at `kProgramOrg`.
2. The program's `DONE` mark (`integration/harness/nanomig.i`) is seen by
   `run_until_write()`.
3. Values the program stores in `RESULTS` read back through `peek_logical()`
   exactly as written: a word, a long split into two words, and the five
   values of a `DBRA` loop.
4. The value check can fail: `prog_control.s` assembles `prog.s` with
   `BROKEN` set, which stores `$1235` instead of `$1234`, and the check must
   report the mismatch.

No check here is an HRM conformance claim. Every PASS/FAIL line says
`[not graded -- apparatus check, no claim uid]`. The `MEASURED` line records
DMACONR and VPOSR as the program read them and the clk_sys cycle of `DONE`;
none of them is compared with anything.

The program runs from chip RAM rather than from the overlay window because a
write to CIA-A clears the overlay (`minimig.v:862`), and a program running
from $000000-$07FFFF would lose its own code at that moment.

`make test` needs Verilator and vasm (`vasmm68k_mot`) on the PATH; see
`integration/harness/nanomig.mk`.
