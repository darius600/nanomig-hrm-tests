# appDE-ports-disk: the pen-pressed button (P5) and the light pen (P1-P4).
# See SPECIFICATION.md. Section 0.1's external-disk-identification claims
# carry no proposition and no program.

TEST_CPP := test_appDE_ports_disk.cpp
PROGS    := prog_pot prog_pot_nodone prog_pot_fix prog_lpen prog_lpen_ocs prog_lpen_nodone prog_lpen_slow prog_lpen_fix prog_lpen_v8 prog_lpen_hw prog_lpen_m8

include ../harness/nanomig.mk

# The variants include their main program and the register file.
prog_pot_nodone.bin prog_pot_fix.bin: prog_pot.s appDE_regs.i
prog_lpen.bin prog_lpen_ocs.bin prog_lpen_nodone.bin prog_lpen_slow.bin prog_lpen_fix.bin prog_lpen_v8.bin prog_lpen_hw.bin prog_lpen_m8.bin: prog_lpen.s appDE_regs.i
