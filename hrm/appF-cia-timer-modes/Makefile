# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

# Two independent fixtures, one per timer submodule -- cia_timera and
# cia_timerb -- each its own top, its own object directory, its own binary.
# Neither module instantiates anything else (both are leaf modules), so
# each fixture's SRC list is just the one file.
SRC_A := $(NANOMIG)/src/minimig-aga/cia_timera.v
SRC_B := $(NANOMIG)/src/minimig-aga/cia_timerb.v

TEST_SRC_A := test_cia_timera.cpp
TEST_SRC_B := test_cia_timerb.cpp

OBJ_DIR_A := obj_dir_cia_timera
OBJ_DIR_B := obj_dir_cia_timerb

TEST_BIN_A := $(OBJ_DIR_A)/Vcia_timera
TEST_BIN_B := $(OBJ_DIR_B)/Vcia_timerb

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

# Pass MUT=<NAME> on the make command line to build one mutation variant,
# e.g. `make MUT=MUT_NOTICK_CIA_TIMERA test`. Unset/empty MUT builds the
# unmutated (baseline) pair. The flag is passed to both fixtures' builds;
# each test source only reacts to the #define it names, so a flag meant for
# one fixture is inert (unused macro) in the other's compile.
MUT ?=
ifneq ($(MUT),)
FLAGS += -CFLAGS -D$(MUT)=1
endif

.PHONY: all test build clean

all: test

build: $(SRC_A) $(TEST_SRC_A) $(SRC_B) $(TEST_SRC_B) Makefile
	rm -rf $(OBJ_DIR_A) $(OBJ_DIR_B)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR_A) --top-module cia_timera $(SRC_A) $(TEST_SRC_A)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR_B) --top-module cia_timerb $(SRC_B) $(TEST_SRC_B)

# test always rebuilds both fixtures from scratch, regardless of MUT, so a
# stale binary from a previous MUT value can never be silently re-run and
# reported as a different variant's result. Both binaries always run, even
# if the first one exits non-zero.
test: $(SRC_A) $(TEST_SRC_A) $(SRC_B) $(TEST_SRC_B) Makefile
	rm -rf $(OBJ_DIR_A) $(OBJ_DIR_B)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR_A) --top-module cia_timera $(SRC_A) $(TEST_SRC_A)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR_B) --top-module cia_timerb $(SRC_B) $(TEST_SRC_B)
	@ec=0; \
	./$(TEST_BIN_A) || ec=1; \
	./$(TEST_BIN_B) || ec=1; \
	exit $$ec

clean:
	rm -rf $(OBJ_DIR_A) $(OBJ_DIR_B)
