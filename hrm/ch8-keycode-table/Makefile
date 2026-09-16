# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP := keymap
# keymap is a single self-contained combinational module (one `assign`,
# amiga_keymap.v:15-142) -- it instantiates nothing, so it is the only
# source file this build needs.
SRC := $(NANOMIG)/src/misc/amiga_keymap.v
TEST_SRC := test_keymap.cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal

# Pass MUT=<NAME> to build one mutation variant, e.g. `make MUT=MUT_ZERO`.
# Unset/empty MUT builds the unmutated (baseline) binary.
MUT ?=
ifneq ($(MUT),)
FLAGS += -CFLAGS -D$(MUT)=1
endif

.PHONY: all test build clean

all: test

build: $(TEST_BIN)

$(TEST_BIN): $(SRC) $(TEST_SRC) Makefile
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR) --top-module $(TOP) $(SRC) $(TEST_SRC)

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf $(OBJ_DIR)
