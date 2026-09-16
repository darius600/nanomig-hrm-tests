# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

TOP := minimig_bankmapper
SRC := $(NANOMIG)/src/minimig-aga/minimig_bankmapper.v
TEST_SRC := test_ch1-chip-memory-banking.cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

# Pass MUT=MUT_<n> on the command line to build one mutation variant, e.g.
# `make MUT=MUT_0 test`. Unset/empty MUT builds the unmutated baseline. Each
# variant is a full separate build+run -- there is no shared state between
# them, and `test` always rebuilds from clean below (MUT is a #define, not a
# file, so make cannot see it as a dependency; without the unconditional
# `rm -rf $(OBJ_DIR)` here, `make MUT=X test` would silently reuse whatever
# binary -- baseline or a different variant -- happened to be sitting in
# $(OBJ_DIR) already and report its result as if it were MUT=X's).
MUT ?=
ifneq ($(MUT),)
FLAGS += -CFLAGS -D$(MUT)=1
endif

.PHONY: all test build clean

all: test

build: $(TEST_BIN)

$(TEST_BIN): $(SRC) $(TEST_SRC) Makefile
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR) --top-module $(TOP) $(SRC) $(TEST_SRC)

test: $(SRC) $(TEST_SRC)
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR) --top-module $(TOP) $(SRC) $(TEST_SRC)
	./$(TEST_BIN)

clean:
	rm -rf $(OBJ_DIR)
