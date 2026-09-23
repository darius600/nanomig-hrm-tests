# NANOMIG points two levels out because this package lives in aa/ rather
# than hrm/. -y on the source directory: denise_bitplanes instantiates eight
# bitplane shifters, and Verilator finds them from the library path. Same
# TOP as aa-bitplane-data-registers -- this package shares its UUT and its
# fixture shape, grading BPLCON1's scroll range instead of the BPLxDAT
# addresses.

NANOMIG ?= ../../nanomig
SRC_DIR := $(NANOMIG)/src/minimig-aga

TOP := denise_bitplanes
SRC := $(SRC_DIR)/denise_bitplanes.v
TEST_SRC := test_denise_bitplanes.cpp
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
# red run that also takes down one of the OTHER two would mean the mutation
# is not isolated to the assertion it names.
MUTATE_IDS := SCROLLING-IS-IN-35NS-STEPS LORES-SCROLL-SPANS-SIXTEEN-LORES-PIXELS LORES-SCROLL-SPANS-SIXTY-FOUR-35NS-STEPS
mutate: build
	@for id in $(MUTATE_IDS); do \
		echo "=== MUTATE=$$id ==="; \
		MUTATE=$$id ./$(TEST_BIN) || true; \
		echo; \
	done

clean:
	rm -rf $(OBJ_DIR)
