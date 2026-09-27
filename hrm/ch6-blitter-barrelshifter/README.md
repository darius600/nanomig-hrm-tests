# HRM test: Agnus blitter barrel shifter

This unit test checks only agnus_blitter_barrelshifter. Expected behavior comes from the *Amiga Hardware Reference Manual, Third Edition*, not from the RTL module body.

Primary HRM references:

- Shifts and Masks, printed pages 179 and 180
- Descending Mode, printed page 182
- BLTCON0 and BLTCON1 register descriptions, printed page 267

Run under Linux or WSL:

    make test

The test compiles only the selected RTL module and a C++ driver. It does not use NanoMig's existing tests, full-system testbench, ROMs, CPU model, SDL code, or storage simulation as an oracle.
