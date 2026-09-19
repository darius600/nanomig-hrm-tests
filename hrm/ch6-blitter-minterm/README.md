# HRM test: Agnus blitter minterm generator

This unit test checks only `agnus_blitter_minterm`. Expected behavior was fixed from the *Amiga Hardware Reference Manual, Third Edition* before the RTL module was inspected.

Primary HRM references:

- Function Generator, printed page 174
- Designing the LF Control Byte with Minterms, printed page 175
- Table 6-1, printed page 177
- Figure 6-3, printed page 178

The source specification is maintained in the FPGA-Lab workspace as `work/2026-08-15_fpga-003_blitter-minterm_testspezifikation.md`, initially fixed in commit `9074d9f`.

Run under Linux or WSL:

```sh
make test
```

The test compiles only the selected RTL module and a C++ driver. It does not use NanoMig's existing tests, full-system testbench, ROMs, CPU model, SDL code, or storage simulation as an oracle.
