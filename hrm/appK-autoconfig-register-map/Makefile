# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = cpu_wrapper
# cpu_wrapper's own bus ports (chip_addr/chip_din/chip_rw/chip_uds/chip_lds/
# chip_as) are all outputs, generated internally by whichever CPU core the
# module instantiates -- see SPECIFICATION.md, "Boundary investigation".
# The default core (fx68k) fails this project's pinned Verilator under its
# default checks (measured: %Error-BLKANDNBLK on fx68k.sv:293); it builds
# with -Wno-BLKANDNBLK, as NanoMig's own sim/Makefile does, and runs with
# --no-assert (without it, its unique-case checks stop it at time 0) -- see
# SPECIFICATION.md, item 1. The other core cpu_wrapper.v can select,
# TG68KdotC_Kernel, elaborates from the Verilog translation of the board's
# VHDL (all-lowercase ports, the `ifdef VERILATOR branch of cpu_wrapper.v).
# Six of NanoMig's twelve board tops select it, among them
# all four that define ENABLE_RAM32, the configuration this package builds.
# This build selects it with -DENABLE_TG68K=1; no file under nanomig/ is
# modified.
# The TG68K the boards synthesize, translated by GHDL, and cpu_wrapper.v with
# the board's skipFetch connection, both generated at build time: see
# ../../integration/harness/tg68k.mk (TG68K_KERNEL=shipped compiles the
# submodule's sim/TG68KdotC_Kernel.v and unmodified cpu_wrapper.v instead).
include ../../integration/harness/tg68k.mk
SRC = $(call tg68k_files,$(NANOMIG)/src/minimig-aga/$(TOP_MODULE).v) $(TG68K_FILE)

# MUT=<NAME> replays one guarded mutation (see the "// MUTATION <NAME> ..."
# line in test_cpu_wrapper.cpp) by defining MUT_<NAME>=1 for the build, so the
# exact stimulus change that assertion's log entry describes can be
# reproduced later. Run `make clean` first when switching MUT values --
# obj_dir is reused otherwise and Verilator will not relink.
# ENABLE_RAM32 is the configuration of NanoMig's 32 MiB boards
# (src/lattice/icepi-zero, src/lattice/icesugar-pro, src/mistle/gw3a_20,
# src/mistle/gw5a_25 each `define it in top.sv), and the only one in which
# cpu_wrapper.v compiles a Zorro III path. Without it cfg_z3 is wired to 0
# and up to three Zorro II cards configure in turn -- the build of every other
# board target (nano20k, primer25k, console60k, console138k, mega138kpro,
# gatemate), which hrm/appK-autoconfig-card-chain measures. No board top
# drives fastram_config[2], so the fixture drives cpucfg and fastramcfg at
# cpu_wrapper's own ports.

MUTFLAGS := $(if $(MUT),-CFLAGS -DMUT_$(MUT)=1,)

test: $(TOP_MODULE)
	./obj_dir/V$(TOP_MODULE) > out.txt; rc=$$?; cat out.txt; exit $$rc

$(TOP_MODULE): test_$(TOP_MODULE).cpp $(SRC)
	verilator --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -DENABLE_TG68K=1 -DENABLE_RAM32=1 -Wall -Wno-fatal $(MUTFLAGS) $(SRC) test_$(TOP_MODULE).cpp

clean:
	rm -rf obj_dir out.txt
