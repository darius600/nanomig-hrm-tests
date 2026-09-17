# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

TOP := agnus
SRC_DIR := $(NANOMIG)/src/minimig-aga

# Agnus's own module plus every submodule it instantiates (agnus.v:319-490),
# named explicitly rather than wildcarded so this Makefile can never
# accidentally pull in an unrelated *.v file. Reused verbatim from
# hrm/ch1-agnus-what-it-contains's Makefile, the model this package's build
# task named -- the only other package in hrm/ that tops the full `agnus`
# (TOP := agnus, not TOP_MODULE = agnus with a hand-copied SRC list) and
# carries MUT= pass-through in its `test` recipe.
SRC := $(SRC_DIR)/agnus.v \
       $(SRC_DIR)/agnus_refresh.v \
       $(SRC_DIR)/agnus_diskdma.v \
       $(SRC_DIR)/agnus_audiodma.v \
       $(SRC_DIR)/agnus_bitplanedma.v \
       $(SRC_DIR)/agnus_spritedma.v \
       $(SRC_DIR)/agnus_copper.v \
       $(SRC_DIR)/agnus_blitter.v \
       $(SRC_DIR)/agnus_blitter_adrgen.v \
       $(SRC_DIR)/agnus_blitter_barrelshifter.v \
       $(SRC_DIR)/agnus_blitter_fill.v \
       $(SRC_DIR)/agnus_blitter_minterm.v \
       $(SRC_DIR)/agnus_beamcounter.v

TEST_SRC := test_agnus.cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 --Mdir $(OBJ_DIR) --top-module $(TOP) \
         -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

# Pass MUT=<NAME> on the make command line to build one mutation variant,
# e.g. `make MUT=MUT_RESET test`. Unset/empty MUT builds the unmutated
# (baseline) binary. `test`'s own recipe removes $(OBJ_DIR) unconditionally
# before building, every invocation -- so a MUT value that changes only a -D
# define (not $(SRC)/$(TEST_SRC)/Makefile) still forces a real rebuild
# instead of silently re-running a stale binary from the previous MUT value.
MUT ?=
MUTFLAGS := $(if $(MUT),-CFLAGS -D$(MUT)=1,)

.PHONY: all test build clean

all: test

build:
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) $(MUTFLAGS) $(SRC) $(TEST_SRC)

test: build
	./$(TEST_BIN)

clean:
	rm -rf $(OBJ_DIR)
