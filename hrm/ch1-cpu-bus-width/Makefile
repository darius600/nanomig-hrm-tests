# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

TOP_MODULE := minimig_m68k_bridge
SRC := $(NANOMIG)/src/minimig-aga/$(TOP_MODULE).v
TEST_SRC := test_ch1-cpu-bus-width.cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP_MODULE)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 --Mdir $(OBJ_DIR) --top-module $(TOP_MODULE) -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

# MUT=<NAME> builds the mutation guarded by -DMUT_<NAME>=1 in the testbench
# (e.g. `make MUT=A test`, guarded by `#if MUT_A` at the site). Baseline
# (unmutated) build: `make test`.
MUT ?=
ifneq ($(MUT),)
FLAGS += -CFLAGS -DMUT_$(MUT)=1
endif

.PHONY: all test clean

all: test

test: $(TEST_SRC) $(SRC) Makefile
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) $(SRC) $(TEST_SRC)
	./$(TEST_BIN)

clean:
	rm -rf $(OBJ_DIR)
