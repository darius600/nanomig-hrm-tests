# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = gary
SRC_DIR = $(NANOMIG)/src/minimig-aga
# gary.v is self-contained -- it instantiates no submodules, so no other
# file is compiled alongside it (matching ch1-memory-map-and-two-buses,
# ch7-custom-address-space and ch7-reset-overlay, the three accepted
# packages that already build gary standalone in this suite).
SRC = $(SRC_DIR)/gary.v
VERILATOR = verilator
FLAGS = --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal -Wno-pinmissing -Wno-lint -I$(SRC_DIR)
TEST_SRC = test_$(TOP_MODULE).cpp

# MUT=<NAME> replays one guarded mutation (see the "// MUTATION <NAME> ..."
# line in test_gary.cpp) by defining MUT_<NAME>=1 for the build, so the
# exact stimulus change that assertion's log entry describes can be
# reproduced later. Run `make clean` first when switching MUT values --
# obj_dir is reused otherwise and Verilator will not relink.
MUT ?=
MUTFLAGS := $(if $(MUT),-CFLAGS -DMUT_$(MUT)=1,)

.PHONY: all test clean

all: test

test: $(TEST_SRC) $(SRC)
	rm -rf obj_dir
	$(VERILATOR) $(FLAGS) $(MUTFLAGS) $(SRC) $(TEST_SRC)
	./obj_dir/V$(TOP_MODULE) > out.txt; rc=$$?; cat out.txt; exit $$rc

clean:
	rm -rf obj_dir out.txt
