# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = denise_spritepriority
SRC_DIR = $(NANOMIG)/src/minimig-aga
VERILOG_SOURCES = $(SRC_DIR)/denise_spritepriority.v
VERILATOR = verilator
VERILATOR_FLAGS = --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal -Wno-pinmissing -Wno-lint
CPP_TEST = test_$(TOP_MODULE).cpp

# MUT=<n> replays one guarded per-assertion mutation (see the numbered
# "MUT_ID" comments in test_denise_spritepriority.cpp) by defining
# MUT_ID=<n> for the build, so the exact stimulus change assertion <n>'s
# log entry describes can be reproduced later. `test` always removes
# obj_dir first, so no separate `make clean` is needed between MUT values.
MUTFLAGS := $(if $(MUT),-CFLAGS -DMUT_ID=$(MUT),)

.PHONY: all test clean

all: test

test: $(CPP_TEST) $(VERILOG_SOURCES)
	rm -rf obj_dir
	$(VERILATOR) $(VERILATOR_FLAGS) $(MUTFLAGS) $(VERILOG_SOURCES) $(CPP_TEST)
	./obj_dir/V$(TOP_MODULE)

clean:
	rm -rf obj_dir
