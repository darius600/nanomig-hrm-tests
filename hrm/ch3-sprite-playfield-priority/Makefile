# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = denise_spritepriority
SRC_DIR = $(NANOMIG)/src/minimig-aga
VERILOG_SOURCES = $(SRC_DIR)/denise_spritepriority.v
VERILATOR = verilator
VERILATOR_FLAGS = --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal -Wno-pinmissing -Wno-lint
CPP_TEST = test_$(TOP_MODULE).cpp

# MUT=<NAME> replays one guarded mutation (see the "// MUTATION <NAME> ..."
# lines in test_denise_spritepriority.cpp) by defining MUT_<NAME>=1 for the
# build, so the exact stimulus change that assertion's log entry describes
# can be reproduced later. The `test` target below always removes obj_dir
# first, so no separate `make clean` is needed when switching MUT values.
MUTFLAGS := $(if $(MUT),-CFLAGS -DMUT_$(MUT)=1,)

.PHONY: all test clean

all: test

test: $(CPP_TEST) $(VERILOG_SOURCES)
	rm -rf obj_dir
	$(VERILATOR) $(VERILATOR_FLAGS) $(MUTFLAGS) $(VERILOG_SOURCES) $(CPP_TEST)
	./obj_dir/V$(TOP_MODULE)

clean:
	rm -rf obj_dir
