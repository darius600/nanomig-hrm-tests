# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

COPPER_TOP = agnus_copper
COPPER_RTL = $(NANOMIG)/src/minimig-aga/agnus_copper.v
COPPER_TEST_SRC = test_agnus_copper_control.cpp
COPPER_OBJ_DIR = obj_dir_copper
COPPER_TEST_BIN = $(COPPER_OBJ_DIR)/V$(COPPER_TOP)

AGNUS_TOP = agnus
AGNUS_RTL = $(NANOMIG)/src/minimig-aga/agnus.v $(NANOMIG)/src/minimig-aga/agnus_copper.v $(NANOMIG)/src/minimig-aga/agnus_blitter.v $(NANOMIG)/src/minimig-aga/agnus_audiodma.v $(NANOMIG)/src/minimig-aga/agnus_beamcounter.v $(NANOMIG)/src/minimig-aga/agnus_bitplanedma.v $(NANOMIG)/src/minimig-aga/agnus_blitter_barrelshifter.v $(NANOMIG)/src/minimig-aga/agnus_blitter_fill.v $(NANOMIG)/src/minimig-aga/agnus_blitter_minterm.v $(NANOMIG)/src/minimig-aga/agnus_diskdma.v $(NANOMIG)/src/minimig-aga/agnus_refresh.v $(NANOMIG)/src/minimig-aga/agnus_spritedma.v $(NANOMIG)/src/minimig-aga/agnus_blitter_adrgen.v
AGNUS_TEST_SRC = test_agnus_top_copper_dma.cpp
AGNUS_OBJ_DIR = obj_dir_agnus
AGNUS_TEST_BIN = $(AGNUS_OBJ_DIR)/V$(AGNUS_TOP)

VERILATOR = verilator
COMMON_FLAGS = --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal
AGNUS_FLAGS = -Wno-pinmissing -Wno-lint -I$(NANOMIG)/src/minimig-aga

.PHONY: all test clean

all: test

test: $(COPPER_TEST_BIN) $(AGNUS_TEST_BIN)
	@status=0; ./$(COPPER_TEST_BIN) || status=1; ./$(AGNUS_TEST_BIN) || status=1; exit $$status

$(COPPER_TEST_BIN): $(COPPER_TEST_SRC) $(COPPER_RTL)
	rm -rf $(COPPER_OBJ_DIR)
	$(VERILATOR) $(COMMON_FLAGS) --Mdir $(COPPER_OBJ_DIR) --top-module $(COPPER_TOP) $(COPPER_RTL) $(COPPER_TEST_SRC)

$(AGNUS_TEST_BIN): $(AGNUS_TEST_SRC) $(AGNUS_RTL)
	rm -rf $(AGNUS_OBJ_DIR)
	$(VERILATOR) $(COMMON_FLAGS) $(AGNUS_FLAGS) --Mdir $(AGNUS_OBJ_DIR) --top-module $(AGNUS_TOP) $(AGNUS_RTL) $(AGNUS_TEST_SRC)

clean:
	rm -rf $(COPPER_OBJ_DIR) $(AGNUS_OBJ_DIR)
