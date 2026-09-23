# The same Makefile every package uses, with NANOMIG pointing two levels out
# because this package lives in aa/ rather than hrm/.

NANOMIG ?= ../../nanomig

TOP := agnus_bitplanedma
SRC := $(NANOMIG)/src/minimig-aga/agnus_bitplanedma.v
TEST_SRC := test_agnus_bitplanedma.cpp
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

# Prove every assertion can go red ALONE. MUTATE names one assertion's own
# label at a time (mutating(id) in the source); a green run for an id below
# means that assertion's expect() is not measuring what it claims to, and a
# red run that also takes down another assertion would mean the mutation is
# not isolated to the one it names.
MUTATE_IDS := FIXTURE-REACHES-THE-POINTER-BANK BPLPT-BANK-IS-ADDRESS-SELECTED BPL8PT-IS-AT-0F4-AND-0F6 BPLPT-HALVES-TAKE-DB0-DB4-AND-DB1-DB15
mutate: build
	@for id in $(MUTATE_IDS); do \
		echo "=== MUTATE=$$id ==="; \
		MUTATE=$$id ./$(TEST_BIN) || true; \
		echo; \
	done

clean:
	rm -rf $(OBJ_DIR)
