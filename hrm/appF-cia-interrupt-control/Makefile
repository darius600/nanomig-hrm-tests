# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

# cia_int is a leaf module -- it instantiates nothing else (read in full:
# 70 lines, ports only, no sub-instantiation) -- so the fixture is a single
# Verilated top with no sibling submodules to pull in.
TOP := cia_int
SRC := $(NANOMIG)/src/minimig-aga/cia_int.v
TEST_SRC := test_appF-cia-interrupt-control.cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

# Pass MUT=<NAME> on the make command line to build one mutation variant,
# e.g. `make MUT=MUT_FIXLIVE test`. Unset/empty MUT builds the unmutated
# (baseline) binary. Each variant is a full separate build+run -- there is
# no shared state between them.
MUT ?=
ifneq ($(MUT),)
FLAGS += -CFLAGS -D$(MUT)=1
endif

.PHONY: all test build clean

all: test

build: $(SRC) $(TEST_SRC) Makefile
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR) --top-module $(TOP) $(SRC) $(TEST_SRC)

# test always rebuilds from scratch, regardless of MUT, so a stale binary
# from a previous MUT value can never be silently re-run and reported as a
# different variant's result.
test: $(SRC) $(TEST_SRC) Makefile
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR) --top-module $(TOP) $(SRC) $(TEST_SRC)
	./$(TEST_BIN)

clean:
	rm -rf $(OBJ_DIR)
