# Where the NanoMig checkout lives. Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = denise
SRC_DIR = $(NANOMIG)/src/minimig-aga
VERILOG_SOURCES = $(SRC_DIR)/denise.v $(SRC_DIR)/denise_bitplanes.v \
                   $(SRC_DIR)/denise_bitplane_shifter.v $(SRC_DIR)/bitplane_ram.v \
                   $(SRC_DIR)/denise_playfields.v \
                   $(SRC_DIR)/denise_sprites.v $(SRC_DIR)/denise_sprites_shifter.v \
                   $(SRC_DIR)/sprite_ram.v \
                   $(SRC_DIR)/denise_spritepriority.v \
                   $(SRC_DIR)/denise_colortable.v $(SRC_DIR)/denise_colortable_ram_mf.v \
                   $(SRC_DIR)/denise_hamgenerator.v \
                   $(SRC_DIR)/denise_collision.v
VERILATOR = verilator
CPP_TEST = test_$(TOP_MODULE).cpp
OBJ_DIR = obj_dir

VERILATOR_FLAGS = --no-timing --cc --exe --build -j 0 --Mdir $(OBJ_DIR) --top-module $(TOP_MODULE) -Wall -Wno-fatal -Wno-pinmissing -Wno-lint -I$(SRC_DIR)

# Pass MUT=MUT_<n> on the command line to build one mutation variant, e.g.
# `make MUT=MUT_0 test`. Unset/empty MUT builds the unmutated baseline.
# `test` always rebuilds from clean below (MUT is a #define, not a file,
# so make cannot see it as a dependency; without the unconditional
# `rm -rf $(OBJ_DIR)` here, `make MUT=X test` would silently reuse whatever
# binary -- baseline or a different variant -- happened to be sitting in
# $(OBJ_DIR) already and report its result as if it were MUT=X's).
MUT ?=
ifneq ($(MUT),)
VERILATOR_FLAGS += -CFLAGS -D$(MUT)=1
endif

.PHONY: all test clean

all: test

test: $(CPP_TEST) $(VERILOG_SOURCES)
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_FLAGS) $(VERILOG_SOURCES) $(CPP_TEST)
	./$(OBJ_DIR)/V$(TOP_MODULE)

clean:
	rm -rf $(OBJ_DIR)
