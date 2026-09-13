# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = denise
SRC_DIR = $(NANOMIG)/src/minimig-aga
VERILOG_SOURCES = $(SRC_DIR)/denise.v $(SRC_DIR)/denise_bitplanes.v $(SRC_DIR)/denise_bitplane_shifter.v $(SRC_DIR)/denise_playfields.v $(SRC_DIR)/denise_sprites.v $(SRC_DIR)/denise_sprites_shifter.v $(SRC_DIR)/denise_collision.v $(SRC_DIR)/denise_hamgenerator.v $(SRC_DIR)/denise_colortable.v $(SRC_DIR)/denise_colortable_ram_mf.v $(SRC_DIR)/denise_spritepriority.v $(SRC_DIR)/bitplane_ram.v
VERILATOR = verilator
VERILATOR_FLAGS = --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal -Wno-pinmissing -Wno-lint
VERILATOR_FLAGS += -I$(SRC_DIR)
CPP_TEST = test_$(TOP_MODULE).cpp

# MUT lets a single build re-run with one mutation flag turned on, e.g.:
#   make MUT=-DMUT_BIT15_OCS=1
# (an empty -CFLAGS "" confuses verilator's arg parser into losing the
# source file list, so only add the flag when MUT is actually set.)
ifneq ($(strip $(MUT)),)
VERILATOR_FLAGS += -CFLAGS "$(MUT)"
endif

.PHONY: all test clean

all: test

test: $(CPP_TEST) $(VERILOG_SOURCES)
	rm -rf obj_dir
	$(VERILATOR) $(VERILATOR_FLAGS) $(VERILOG_SOURCES) $(CPP_TEST)
	./obj_dir/V$(TOP_MODULE)

clean:
	rm -rf obj_dir
