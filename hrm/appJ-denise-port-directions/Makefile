# Where the NanoMig checkout lives. Default is the submodule in this repo.
# Modelled on hrm/ch8-appb-chip-column-denise/Makefile, the accepted package
# that already tops `denise` alone with this same submodule set.
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
VERILATOR_FLAGS = --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal -Wno-pinmissing -Wno-lint -I$(SRC_DIR)
CPP_TEST = test_$(TOP_MODULE).cpp

.PHONY: all test clean

all: test

test: $(CPP_TEST) $(VERILOG_SOURCES)
	rm -rf obj_dir
	$(VERILATOR) $(VERILATOR_FLAGS) $(VERILOG_SOURCES) $(CPP_TEST)
	./obj_dir/V$(TOP_MODULE)

clean:
	rm -rf obj_dir
