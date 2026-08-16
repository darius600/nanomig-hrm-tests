# HRM test: Agnus blitter area fill

This unit test checks only `agnus_blitter_fill`. Expected behavior was fixed from the *Amiga Hardware Reference Manual, Third Edition* before the RTL module body was inspected.

Primary HRM references:

- Area Fill Mode, printed pages 184-186
- BLTCON0 and BLTCON1 register descriptions, printed page 267

The source specification is maintained in the FPGA-Lab workspace as `work/2026-08-16_fpga-008_blitter-fill_testspezifikation.md` and was fixed in commit `9c7d16c`.

The test includes the four printed HRM examples, explicitly exercises six named boundary patterns, exhaustively checks all 262,144 combinations of 16-bit input, fill carry-in, and one valid fill mode, and separately checks the module's disabled bypass path. Simultaneously enabling inclusive and exclusive fill is not specified by the HRM and is not tested as valid behavior. The contradictory FCI bit number on printed page 185 is outside this module-level test; register decoding is not under test.

Run under Linux or WSL:

    make test

The test compiles only the selected RTL module and a C++ driver. It does not use NanoMig's existing tests, full-system testbench, or RTL implementation as an oracle.
