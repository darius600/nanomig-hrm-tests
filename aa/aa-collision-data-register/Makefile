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

mutate: build
	MUTATE=1 ./$(TEST_BIN) || true

clean:
	rm -rf $(OBJ_DIR)
