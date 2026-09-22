# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP := userio
SRC := $(NANOMIG)/src/minimig-aga/$(TOP).v
TEST_SRC := test_$(TOP).cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal

# Pass MUT=<NAME> to build one mutation variant, e.g. `make MUT=FIXTURE_LIVENESS`.
# Unset/empty MUT builds the unmutated (baseline) binary.
MUT ?=
ifneq ($(MUT),)
FLAGS += -CFLAGS -DMUT_$(MUT)=1
endif

.PHONY: all test build clean

all: test

build: $(TEST_BIN)

$(TEST_BIN): $(SRC) $(TEST_SRC) Makefile
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR) --top-module $(TOP) $(SRC) $(TEST_SRC)

# `test` deliberately does NOT depend on $(TEST_BIN) as a file target: MUT
# is not a file, so it can never make a file target look out of date, and
# `make MUT=X test` on an already-built tree would otherwise silently reuse
# a STALE binary from a previous (possibly different-MUT) build and report
# its result as the new variant's. Always clean and rebuild instead.
test: $(SRC) $(TEST_SRC) Makefile
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR) --top-module $(TOP) $(SRC) $(TEST_SRC)
	./$(TEST_BIN) > out.txt; rc=$$?; cat out.txt; exit $$rc

clean:
	rm -rf $(OBJ_DIR) out.txt
