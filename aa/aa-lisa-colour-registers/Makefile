# The same Makefile every package uses, with NANOMIG pointing two levels out
# because this package lives in aa/ rather than hrm/.
#
# -y on the source directory rather than a list of files: denise instantiates
# the colour table, the HAM generator, the bitplane shifters, the sprites and
# the collision unit, and Verilator finds them all from the library path. The
# blitter package lists five sources by hand; this is the shorter way and
# ch3-bplcon0-display-modes was already using it.

NANOMIG ?= ../../nanomig
SRC_DIR := $(NANOMIG)/src/minimig-aga

TOP := denise
SRC := $(SRC_DIR)/denise.v
TEST_SRC := test_denise.cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

.PHONY: all test build mutate clean

all: test

build:
	$(VERILATOR) $(FLAGS) --top-module $(TOP) -y $(SRC_DIR) $(SRC) $(TEST_SRC) -o V$(TOP)

test: build
	./$(TEST_BIN)

# Prove every assertion can go red. A green here means an expect() is not
# measuring what it claims to.
mutate: build
	MUTATE=1 ./$(TEST_BIN) || true

clean:
	rm -rf $(OBJ_DIR)
