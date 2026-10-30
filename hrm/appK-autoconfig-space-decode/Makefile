# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = cpu_wrapper
# cpu_wrapper's own bus ports (chip_addr/chip_din/chip_rw/chip_uds/chip_lds/
# chip_as/ramreq/ramaddr) are all outputs, generated internally by whichever
# CPU core the module instantiates -- see SPECIFICATION.md, "Boundary
# re-confirmed for this package" (inherited from hrm/appK-autoconfig-register-
# map's own boundary investigation, not rediscovered here). The default core
# (fx68k) fails this project's pinned Verilator under its default checks
# (measured there: %Error-BLKANDNBLK on fx68k.sv:293); it builds with
# -Wno-BLKANDNBLK, as NanoMig's own sim/Makefile does, and runs with
# --no-assert, and so built it gives this package's every result line as
# TG68K does. The other core cpu_wrapper.v can
# select, TG68KdotC_Kernel, elaborates from the Verilog translation of the
# board's VHDL (all-lowercase ports, the `ifdef VERILATOR branch of
# cpu_wrapper.v). This build selects it with
# -DENABLE_TG68K=1; no file under nanomig/ is modified.
# The TG68K the boards synthesize, translated by GHDL, and cpu_wrapper.v with
# the board's skipFetch connection, both generated at build time: see
# ../../integration/harness/tg68k.mk (TG68K_KERNEL=shipped compiles the
# submodule's sim/TG68KdotC_Kernel.v and unmodified cpu_wrapper.v instead).
include ../../integration/harness/tg68k.mk
SRC = $(call tg68k_files,$(NANOMIG)/src/minimig-aga/$(TOP_MODULE).v) $(TG68K_FILE)

# MUT=<NAME> replays one guarded mutation (see the "// MUTATION <NAME> ..."
# lines in test_cpu_wrapper.cpp) by defining MUT_<NAME>=1 for the build, so
# the exact stimulus change each assertion's log entry describes can be
# reproduced later. Run `make clean` first when switching MUT values --
# obj_dir is reused otherwise and Verilator will not relink.
MUTFLAGS := $(if $(MUT),-CFLAGS -DMUT_$(MUT)=1,)

test: $(TOP_MODULE)
	./obj_dir/V$(TOP_MODULE) > out.txt; rc=$$?; cat out.txt; exit $$rc

$(TOP_MODULE): test_$(TOP_MODULE).cpp $(SRC)
	verilator --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -DENABLE_TG68K=1 -Wall -Wno-fatal $(MUTFLAGS) $(SRC) test_$(TOP_MODULE).cpp

clean:
	rm -rf obj_dir out.txt
