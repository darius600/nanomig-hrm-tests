# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

CIA_SUBMODULES := $(NANOMIG)/src/minimig-aga/cia_int.v \
                   $(NANOMIG)/src/minimig-aga/cia_timera.v \
                   $(NANOMIG)/src/minimig-aga/cia_timerb.v \
                   $(NANOMIG)/src/minimig-aga/cia_timerd.v

# Two independent fixtures, one per physical 8520 -- ciaa and ciab -- each
# its own top, its own object directory, its own binary. The claim graded
# here ("the 8520 I/O chips provide timers") names both chips, so both must
# be built and run; the suite's capture tooling runs every binary a package
# builds, which is why two OBJ_DIRs are used instead of one shared one.
SRC_A := $(NANOMIG)/src/minimig-aga/ciaa.v $(CIA_SUBMODULES)
SRC_B := $(NANOMIG)/src/minimig-aga/ciab.v $(CIA_SUBMODULES)

TEST_SRC_A := test_ciaa.cpp
TEST_SRC_B := test_ciab.cpp

OBJ_DIR_A := obj_dir_ciaa
OBJ_DIR_B := obj_dir_ciab

TEST_BIN_A := $(OBJ_DIR_A)/Vciaa
TEST_BIN_B := $(OBJ_DIR_B)/Vciab

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

# Pass MUT=<NAME> on the make command line to build one mutation variant,
# e.g. `make MUT=MUT_NOTICK_CIAA test`. Unset/empty MUT builds the
# unmutated (baseline) pair. The flag is passed to both fixtures' builds;
# each test source only reacts to the #define it names, so a flag meant for
# one fixture is inert (unused macro) in the other's compile. Each variant
# is a full separate build+run of both fixtures -- no shared state.
MUT ?=
ifneq ($(MUT),)
FLAGS += -CFLAGS -D$(MUT)=1
endif

.PHONY: all test build clean

all: test

build: $(SRC_A) $(TEST_SRC_A) $(SRC_B) $(TEST_SRC_B) Makefile
	rm -rf $(OBJ_DIR_A) $(OBJ_DIR_B)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR_A) --top-module ciaa $(SRC_A) $(TEST_SRC_A)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR_B) --top-module ciab $(SRC_B) $(TEST_SRC_B)

# test always rebuilds both fixtures from scratch, regardless of MUT, so a
# stale binary from a previous MUT value can never be silently re-run and
# reported as a different variant's result. Both binaries always run, even
# if the first one exits non-zero -- otherwise a mutation that (correctly)
# fails ciaa would silently hide ciab's result, and a real defect in one
# fixture would mask whether the other fixture even ran.
test: $(SRC_A) $(TEST_SRC_A) $(SRC_B) $(TEST_SRC_B) Makefile
	rm -rf $(OBJ_DIR_A) $(OBJ_DIR_B)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR_A) --top-module ciaa $(SRC_A) $(TEST_SRC_A)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR_B) --top-module ciab $(SRC_B) $(TEST_SRC_B)
	@ec=0; \
	./$(TEST_BIN_A) || ec=1; \
	./$(TEST_BIN_B) || ec=1; \
	exit $$ec

clean:
	rm -rf $(OBJ_DIR_A) $(OBJ_DIR_B)
