# The same Makefile every package uses, with NANOMIG pointing two levels out
# because this package lives in aa/ rather than hrm/.

NANOMIG ?= ../../nanomig

TOP := paula_intcontroller
SRC := $(NANOMIG)/src/minimig-aga/paula_intcontroller.v
TEST_SRC := test_paula_intcontroller.cpp
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
# red run that also takes down another id would mean the mutation is not
# isolated to the assertion it names.
MUTATE_IDS := FIXTURE-DELIVERS-KNOWN-VALUE INTENA-AT-09A INTENA-NOT-AT-09C INTENAR-AT-01C INTREQ-AT-09C INTREQR-AT-01E INTENA-CLEARS-OR-SETS INTREQ-CLEARS-OR-SETS INTENAR-IS-READ-ONLY INTREQR-IS-READ-ONLY
mutate: build
	@for id in $(MUTATE_IDS); do \
		echo "=== MUTATE=$$id ==="; \
		MUTATE=$$id ./$(TEST_BIN) || true; \
		echo; \
	done

clean:
	rm -rf $(OBJ_DIR)
