# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = agnus_audiodma
SRC = $(NANOMIG)/src/minimig-aga/$(TOP_MODULE).v

# Second fixture: the full Agnus top, where `ecs` is a real, drivable port
# (agnus_audiodma.v alone, above, has none) -- see
# test_agnus_top_audio_ecs.cpp's own header. Same RTL file list
# hrm/ch8-disk-pointer-ecs-and-copper's Makefile builds against. Built into
# obj_dir_top, so both binaries remain after a build: the tools that run
# every obj_dir*/V* binary directly (the coverage ledger, refresh_captures)
# read both fixtures, not only the one built last.
TOP_MODULE2 = agnus
SRC2 = $(NANOMIG)/src/minimig-aga/agnus.v $(NANOMIG)/src/minimig-aga/agnus_copper.v $(NANOMIG)/src/minimig-aga/agnus_blitter.v $(NANOMIG)/src/minimig-aga/agnus_audiodma.v $(NANOMIG)/src/minimig-aga/agnus_beamcounter.v $(NANOMIG)/src/minimig-aga/agnus_bitplanedma.v $(NANOMIG)/src/minimig-aga/agnus_blitter_barrelshifter.v $(NANOMIG)/src/minimig-aga/agnus_blitter_fill.v $(NANOMIG)/src/minimig-aga/agnus_blitter_minterm.v $(NANOMIG)/src/minimig-aga/agnus_diskdma.v $(NANOMIG)/src/minimig-aga/agnus_refresh.v $(NANOMIG)/src/minimig-aga/agnus_spritedma.v $(NANOMIG)/src/minimig-aga/agnus_blitter_adrgen.v

test:
	rm -rf obj_dir obj_dir_top
	verilator --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal $(SRC) test_$(TOP_MODULE).cpp
	./obj_dir/V$(TOP_MODULE) > out.txt; rc1=$$?; cat out.txt; \
	verilator --no-timing --cc --exe --build -j 0 --Mdir obj_dir_top --top-module $(TOP_MODULE2) -Wall -Wno-fatal -Wno-pinmissing -Wno-lint $(SRC2) test_agnus_top_audio_ecs.cpp; \
	./obj_dir_top/V$(TOP_MODULE2) > out_top.txt; rc2=$$?; cat out_top.txt; \
	if [ $$rc1 -ne 0 ] || [ $$rc2 -ne 0 ]; then exit 1; else exit 0; fi

clean:
	rm -rf obj_dir obj_dir_top out.txt out_top.txt
