# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = cpu_wrapper
# cpu_wrapper's own bus ports (chip_addr/chip_din/chip_rw/chip_uds/chip_lds/
# chip_as) are all outputs, generated internally by whichever CPU core the
# module instantiates -- see SPECIFICATION.md and the sibling package
# hrm/appK-autoconfig-register-map/SPECIFICATION.md ("Boundary
# investigation"), whose fixture this package reuses wholesale. The default
# core (fx68k) does not elaborate under this project's pinned Verilator
# (measured there: %Error-BLKANDNBLK on fx68k.sv:293). The other core
# cpu_wrapper.v can select, TG68KdotC_Kernel, does elaborate when built from
# the sim/ tree cpu_wrapper.v's own `ifdef VERILATOR branch already names.
# This build selects it with -DENABLE_TG68K=1; no file under nanomig/ is
# modified.
SRC = $(NANOMIG)/src/minimig-aga/$(TOP_MODULE).v $(NANOMIG)/sim/TG68KdotC_Kernel.v

# MUT=<NAME> replays one guarded mutation (see the "// MUTATION <NAME> ..."
# line in test_cpu_wrapper.cpp) by defining MUT_<NAME>=1 for the build, so the
# exact stimulus change that assertion's log entry describes can be
# reproduced later. Run `make clean` first when switching MUT values --
# obj_dir is reused otherwise and Verilator will not relink.
MUTFLAGS := $(if $(MUT),-CFLAGS -DMUT_$(MUT)=1,)

test: $(TOP_MODULE)
	./obj_dir/V$(TOP_MODULE) > out.txt; rc=$$?; cat out.txt; exit $$rc

$(TOP_MODULE): test_$(TOP_MODULE).cpp $(SRC)
	verilator --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -DENABLE_TG68K=1 -Wall -Wno-fatal $(MUTFLAGS) $(SRC) test_$(TOP_MODULE).cpp

clean:
	rm -rf obj_dir out.txt
