# NANOMIG points two levels out because this package lives in aa/ rather
# than hrm/. -y on the source directory: paula instantiates the floppy
# controller, the UART, the interrupt controller and four audio channels, and
# Verilator finds them all from the library path.

NANOMIG ?= ../../nanomig
SRC_DIR := $(NANOMIG)/src/minimig-aga

TOP := paula
SRC := $(SRC_DIR)/paula.v
TEST_SRC := test_paula.cpp
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

# Prove every assertion can go red ALONE. MUTATE names one assertion's own
# label at a time (mutating(id) in the source); a green run for an id below
# means that assertion's expect() is not measuring what it claims to, and a
# red run that also takes down another assertion would mean the mutation is
# not isolated to the one it names.
MUTATE_IDS := FIXTURE-DELIVERS-KNOWN-VALUE ADKCON-AT-09E-ADKCONR-AT-010 ADKCON-BIT-15-SETS-OR-CLEARS ADKCON-ZERO-BITS-UNCHANGED ADKCONR-BIT-15-IS-NOT-STORED ADKCON-BIT-11-CLEARS-TXD DMACON-AT-096-GATES-THE-DISK DMACON-CLEARS-WHAT-IT-SET
mutate: build
	@for id in $(MUTATE_IDS); do \
		echo "=== MUTATE=$$id ==="; \
		MUTATE=$$id ./$(TEST_BIN) || true; \
		echo; \
	done

clean:
	rm -rf $(OBJ_DIR)
