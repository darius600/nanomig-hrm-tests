# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = cpu_wrapper
FX68K_DIR = $(NANOMIG)/src/fx68k

# This package is hrm/appK-autoconfig-base-address's test built with fx68k
# instead of TG68K: test_cpu_wrapper.cpp is kept byte-identical to that
# package's, the only build-time difference visible to it being CORE_FX68K
# (below). fx68k is cpu_wrapper.v's default core (selected whenever
# ENABLE_TG68K is not defined, cpu_wrapper.v:27-32) and the core six of
# NanoMig's twelve board tops build with, the Tang Nano 20K's default
# top.sv among them (its own ENABLE_TG68K `define is commented out there).
#
# fx68k fails this project's pinned Verilator under its default checks
# (measured: %Error-BLKANDNBLK, fx68k.sv:293); -Wno-BLKANDNBLK builds it, as
# NanoMig's own sim/Makefile does. Without --no-assert its `unique case`
# checks stop the run at time 0 (fx68kAlu.sv:313).
#
# ENABLE_RAM32 is kept, matching the sibling TG68K package -- it is the
# configuration of NanoMig's 32 MiB boards and the only one in which
# cpu_wrapper.v compiles a Zorro III path; see
# hrm/appK-autoconfig-base-address/Makefile and SPECIFICATION.md.
SRC = $(NANOMIG)/src/minimig-aga/$(TOP_MODULE).v $(FX68K_DIR)/fx68k.sv $(FX68K_DIR)/fx68kAlu.sv $(FX68K_DIR)/uaddrPla.sv

# MUT=<NAME> replays one guarded mutation (see the "// MUTATION <NAME> ..."
# line in test_cpu_wrapper.cpp) by defining MUT_<NAME>=1 for the build, so the
# exact stimulus change that assertion's log entry describes can be
# reproduced later. Run `make clean` first when switching MUT values --
# obj_dir is reused otherwise and Verilator will not relink.
MUTFLAGS := $(if $(MUT),-CFLAGS -DMUT_$(MUT)=1,)

test: $(TOP_MODULE)
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir/
	(cd obj_dir && ./V$(TOP_MODULE)) > out.txt; rc=$$?; cat out.txt; exit $$rc

$(TOP_MODULE): test_$(TOP_MODULE).cpp $(SRC)
	verilator --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -DENABLE_RAM32=1 -Wno-BLKANDNBLK --no-assert -Wall -Wno-fatal $(MUTFLAGS) -CFLAGS -DCORE_FX68K=1 $(SRC) test_$(TOP_MODULE).cpp

clean:
	rm -rf obj_dir out.txt
