# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = cpu_wrapper
# Fixture inherited from hrm/appK-autoconfig-register-map, extended with
# the write capability hrm/appK-autoconfig-size-encoding added -- see
# SPECIFICATION.md.
# The TG68K the boards synthesize, translated by GHDL, and cpu_wrapper.v with
# the board's skipFetch connection, both generated at build time: see
# ../../integration/harness/tg68k.mk (TG68K_KERNEL=shipped compiles the
# submodule's sim/TG68KdotC_Kernel.v and unmodified cpu_wrapper.v instead).
include ../../integration/harness/tg68k.mk
SRC = $(call tg68k_files,$(NANOMIG)/src/minimig-aga/$(TOP_MODULE).v) $(TG68K_FILE)

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
