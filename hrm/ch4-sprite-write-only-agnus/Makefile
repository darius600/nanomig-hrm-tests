# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = agnus
SRC = $(NANOMIG)/src/minimig-aga/agnus.v $(NANOMIG)/src/minimig-aga/agnus_copper.v $(NANOMIG)/src/minimig-aga/agnus_blitter.v $(NANOMIG)/src/minimig-aga/agnus_audiodma.v $(NANOMIG)/src/minimig-aga/agnus_beamcounter.v $(NANOMIG)/src/minimig-aga/agnus_bitplanedma.v $(NANOMIG)/src/minimig-aga/agnus_blitter_barrelshifter.v $(NANOMIG)/src/minimig-aga/agnus_blitter_fill.v $(NANOMIG)/src/minimig-aga/agnus_blitter_minterm.v $(NANOMIG)/src/minimig-aga/agnus_diskdma.v $(NANOMIG)/src/minimig-aga/agnus_refresh.v $(NANOMIG)/src/minimig-aga/agnus_spritedma.v $(NANOMIG)/src/minimig-aga/agnus_blitter_adrgen.v
TESTBENCH = test_ch4_sprite_write_only_agnus.cpp

VERILATOR = verilator
VFLAGS = --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal -Wno-pinmissing -Wno-lint -I$(NANOMIG)/src/minimig-aga

all: test

test: $(TESTBENCH) $(SRC)
	rm -rf obj_dir
	$(VERILATOR) $(VFLAGS) $(SRC) $(TESTBENCH)
	./obj_dir/V$(TOP_MODULE)

clean:
	rm -rf obj_dir
