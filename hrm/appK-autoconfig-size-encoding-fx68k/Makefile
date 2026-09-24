# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = cpu_wrapper
FX68K_DIR = $(NANOMIG)/src/fx68k

# This package is hrm/appK-autoconfig-size-encoding's test built with fx68k
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
# ENABLE_RAM32 is kept, matching the sibling TG68K package -- it is a real
# per-board `define (icepi-zero, icesugar-pro, two mistle targets) needed
# to reach the "8MB" Zorro-II size code at all in this design; see
# hrm/appK-autoconfig-size-encoding/Makefile and SPECIFICATION.md.
# Disclosure: no NanoMig board top builds both ENABLE_RAM32 and fx68k --
# all four RAM32 tops (lattice/icepi-zero, lattice/icesugar-pro,
# mistle/gw3a_20, mistle/gw5a_25) also define ENABLE_TG68K, and the six
# fx68k tops define no ENABLE_RAM32. This package therefore measures the
# sibling's ENABLE_RAM32 configuration on the other core -- a pairing no
# shipped bitstream contains.
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
