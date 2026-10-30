# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = cpu_wrapper
# Fixture, harness and program-building approach reused from
# hrm/appK-autoconfig-space -- see SPECIFICATION.md.
# The TG68K the boards synthesize, translated by GHDL, and cpu_wrapper.v with
# the board's skipFetch connection, both generated at build time: see
# ../../integration/harness/tg68k.mk (TG68K_KERNEL=shipped compiles the
# submodule's sim/TG68KdotC_Kernel.v and unmodified cpu_wrapper.v instead).
include ../../integration/harness/tg68k.mk
SRC = $(call tg68k_files,$(NANOMIG)/src/minimig-aga/$(TOP_MODULE).v) $(TG68K_FILE)

# This package deliberately does NOT define ENABLE_RAM32. That macro selects
# the configuration of NanoMig's 32 MiB boards (src/lattice/icepi-zero,
# src/lattice/icesugar-pro, src/mistle/gw3a_20 and src/mistle/gw5a_25 each
# `define it in top.sv); hrm/appK-autoconfig-space builds with it and
# measures that configuration. Every other board target -- nano20k,
# primer25k, console60k, console138k, mega138kpro, gatemate -- builds
# without it, and there cpu_wrapper.v hard-wires cfg_z3 to 0 -- no Zorro III
# path is compiled -- and instead lets a write to register 48 chain up to
# three Zorro II boards when fastramcfg[1:0] == 2'b11 (cpu_wrapper.v:643-651,
# code that only exists in this configuration). This package measures that
# build.

MUTFLAGS := $(if $(MUT),-CFLAGS -DMUT_$(MUT)=1,)

test: $(TOP_MODULE)
	./obj_dir/V$(TOP_MODULE) > out.txt; rc=$$?; cat out.txt; exit $$rc

$(TOP_MODULE): test_$(TOP_MODULE).cpp $(SRC)
	verilator --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -DENABLE_TG68K=1 -Wall -Wno-fatal $(MUTFLAGS) $(SRC) test_$(TOP_MODULE).cpp

clean:
	rm -rf obj_dir out.txt
