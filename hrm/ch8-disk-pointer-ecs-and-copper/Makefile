# Wo liegt der NanoMig-Checkout? Standard ist das Submodul in diesem Repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = agnus
RTL = $(NANOMIG)/src/minimig-aga/agnus.v $(NANOMIG)/src/minimig-aga/agnus_copper.v $(NANOMIG)/src/minimig-aga/agnus_blitter.v $(NANOMIG)/src/minimig-aga/agnus_audiodma.v $(NANOMIG)/src/minimig-aga/agnus_beamcounter.v $(NANOMIG)/src/minimig-aga/agnus_bitplanedma.v $(NANOMIG)/src/minimig-aga/agnus_blitter_barrelshifter.v $(NANOMIG)/src/minimig-aga/agnus_blitter_fill.v $(NANOMIG)/src/minimig-aga/agnus_blitter_minterm.v $(NANOMIG)/src/minimig-aga/agnus_diskdma.v $(NANOMIG)/src/minimig-aga/agnus_refresh.v $(NANOMIG)/src/minimig-aga/agnus_spritedma.v $(NANOMIG)/src/minimig-aga/agnus_blitter_adrgen.v
TESTBENCH = test_ch8_disk_pointer_ecs_and_copper.cpp
OBJ_DIR = obj_dir
TEST_BIN = $(OBJ_DIR)/V$(TOP_MODULE)

VERILATOR = verilator
COMMON_FLAGS = --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint -I$(NANOMIG)/src/minimig-aga

.PHONY: all test clean

all: test

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TESTBENCH) $(RTL)
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(COMMON_FLAGS) --Mdir $(OBJ_DIR) --top-module $(TOP_MODULE) $(RTL) $(TESTBENCH)

clean:
	rm -rf $(OBJ_DIR)
