# The same Makefile every package uses, with NANOMIG pointing two levels out
# because this package lives in aa/ rather than hrm/.

NANOMIG ?= ../../nanomig

TOP := agnus_copper
SRC := $(NANOMIG)/src/minimig-aga/agnus_copper.v
TEST_SRC := test_agnus_copper.cpp
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
# not isolated to the assertion it names. COP1-AND-COP2-ARE-DISTINCT is a
# control (cites no claim) and still carries its own id like the others.
MUTATE_IDS := FIXTURE-DELIVERS-KNOWN-VALUE COP1LC-AT-080-082 COP2LC-AT-084-086 COP1-AND-COP2-ARE-DISTINCT COPINS-AT-08C COPCON-AT-02E SPRHDAT-078-IS-RESTRICTED
mutate: build
	@for id in $(MUTATE_IDS); do \
		echo "=== MUTATE=$$id ==="; \
		MUTATE=$$id ./$(TEST_BIN) || true; \
		echo; \
	done

clean:
	rm -rf $(OBJ_DIR)
