TOP_MODULE = agnus
SRC = ../../../src/minimig-aga/agnus.v ../../../src/minimig-aga/agnus_copper.v ../../../src/minimig-aga/agnus_blitter.v ../../../src/minimig-aga/agnus_audiodma.v ../../../src/minimig-aga/agnus_beamcounter.v ../../../src/minimig-aga/agnus_bitplanedma.v ../../../src/minimig-aga/agnus_blitter_barrelshifter.v ../../../src/minimig-aga/agnus_blitter_fill.v ../../../src/minimig-aga/agnus_blitter_minterm.v ../../../src/minimig-aga/agnus_diskdma.v ../../../src/minimig-aga/agnus_refresh.v ../../../src/minimig-aga/agnus_spritedma.v ../../../src/minimig-aga/agnus_blitter_adrgen.v
TESTBENCH = test_agnus_top_dmacon.cpp

VERILATOR = verilator
VFLAGS = --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal -Wno-pinmissing -Wno-lint -I../../../src/minimig-aga

all: test

test: $(TESTBENCH) $(SRC)
	rm -rf obj_dir
	$(VERILATOR) $(VFLAGS) $(SRC) $(TESTBENCH)
	./obj_dir/V$(TOP_MODULE)

clean:
	rm -rf obj_dir
