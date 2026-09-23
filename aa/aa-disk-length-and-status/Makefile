# NANOMIG points two levels out because this package lives in aa/ rather
# than hrm/. -y on the source directory: paula_floppy pulls in its fifo and
# the drive model, and Verilator finds them from the library path.

NANOMIG ?= ../../nanomig
SRC_DIR := $(NANOMIG)/src/minimig-aga

TOP := paula_floppy
SRC := $(SRC_DIR)/paula_floppy.v
TEST_SRC := test_paula_floppy.cpp
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
# not isolated to the assertion it names.
MUTATE_IDS := FIXTURE-DELIVERS-KNOWN-VALUE DSKBYTR-AT-01A DSKLEN-AT-024-SHOWS-ITS-DIRECTION DSKLEN-ARMS-ON-THE-SECOND-WRITE DSKBYTR-TAKES-NO-WRITES
mutate: build
	@for id in $(MUTATE_IDS); do \
		echo "=== MUTATE=$$id ==="; \
		MUTATE=$$id ./$(TEST_BIN) || true; \
		echo; \
	done

clean:
	rm -rf $(OBJ_DIR)
