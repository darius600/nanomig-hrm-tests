# AA conformance package. It lives under aa/ and not hrm/ because hrm/ is the
# Hardware Reference Manual's suite: run_all.sh and check_published_counts.py
# both count the directories there, and a second oracle's packages are not part
# of that count.

NANOMIG ?= ../../nanomig

TOP := denise_collision
SRC := $(NANOMIG)/src/minimig-aga/denise_collision.v
TEST_SRC := test_denise_collision.cpp
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
# not isolated to the assertion it names.
MUTATE_IDS := CLXCON2-AT-10E CLXCON2-NOT-AT-10C CLXCON2-CLEARED-BY-CLXCON CLXDAT-AT-00E CLXCON-AT-098
mutate: build
	@for id in $(MUTATE_IDS); do \
		echo "=== MUTATE=$$id ==="; \
		MUTATE=$$id ./$(TEST_BIN) || true; \
		echo; \
	done

clean:
	rm -rf $(OBJ_DIR)
