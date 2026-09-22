# The same Makefile every package uses, with NANOMIG pointing two levels out
# because this package lives in aa/ rather than hrm/.

NANOMIG ?= ../../nanomig

TOP := paula_floppy
# paula_floppy instantiates paula_floppy_fifo, so both files are named.
# paula_floppy_ebr.v declares a SECOND module called paula_floppy and
# must never be compiled beside these two.
SRC := $(NANOMIG)/src/minimig-aga/paula_floppy.v \
       $(NANOMIG)/src/minimig-aga/paula_floppy_fifo.v
TEST_SRC := test_paula_floppy.cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

.PHONY: all test build mutate clean

all: test

build:
	$(VERILATOR) $(FLAGS) --top-module $(TOP) $(SRC) $(TEST_SRC) -o V$(TOP)

test: build
	./$(TEST_BIN)

# Prove every assertion can go red. A green here means an expect() is not
# measuring what it claims to.
mutate: build
	MUTATE=1 ./$(TEST_BIN) || true

clean:
	rm -rf $(OBJ_DIR)
