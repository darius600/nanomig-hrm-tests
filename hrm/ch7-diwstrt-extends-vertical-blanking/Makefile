# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = agnus
# SRC/VFLAGS copied verbatim from hrm/ch7-dmacon-register-bits/Makefile: same fixture
# (agnus.v, top-level chipset module, with its submodules), same build flags.
SRC = $(NANOMIG)/src/minimig-aga/agnus.v $(NANOMIG)/src/minimig-aga/agnus_copper.v $(NANOMIG)/src/minimig-aga/agnus_blitter.v $(NANOMIG)/src/minimig-aga/agnus_audiodma.v $(NANOMIG)/src/minimig-aga/agnus_beamcounter.v $(NANOMIG)/src/minimig-aga/agnus_bitplanedma.v $(NANOMIG)/src/minimig-aga/agnus_blitter_barrelshifter.v $(NANOMIG)/src/minimig-aga/agnus_blitter_fill.v $(NANOMIG)/src/minimig-aga/agnus_blitter_minterm.v $(NANOMIG)/src/minimig-aga/agnus_diskdma.v $(NANOMIG)/src/minimig-aga/agnus_refresh.v $(NANOMIG)/src/minimig-aga/agnus_spritedma.v $(NANOMIG)/src/minimig-aga/agnus_blitter_adrgen.v
TESTBENCH = test_agnus.cpp

VERILATOR = verilator
VFLAGS = --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal -Wno-pinmissing -Wno-lint -I$(NANOMIG)/src/minimig-aga

# MUT=<NAME> replays one guarded mutation (see the "// replay: make ... MUT=<NAME>"
# line above each // MUTATION record in test_agnus.cpp) by defining
# MUT_<NAME>=1 for the build, so the
# exact stimulus change that assertion's log entry describes can be
# reproduced later. Run `make clean` first when switching MUT values --
# obj_dir is reused otherwise and Verilator will not relink.
MUTFLAGS := $(if $(MUT),-CFLAGS -DMUT_$(MUT)=1,)

all: test

test: $(TESTBENCH) $(SRC)
	rm -rf obj_dir
	$(VERILATOR) $(VFLAGS) $(MUTFLAGS) $(SRC) $(TESTBENCH)
	./obj_dir/V$(TOP_MODULE)

clean:
	rm -rf obj_dir
