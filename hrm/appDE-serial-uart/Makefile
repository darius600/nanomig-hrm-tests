# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

TOP := paula_uart
SRC := $(NANOMIG)/src/minimig-aga/paula_uart.v
TEST_SRC := test_paula_uart.cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

# Pass MUT=MUT_<n> on the command line to build one mutation variant, e.g.
# `make MUT=MUT_0 test`. Unset/empty MUT builds the unmutated baseline. `test`
# always rebuilds from clean below -- MUT is a #define, not a file, so make
# cannot see it as a dependency, and a stale binary from a previous variant
# would otherwise be silently reused.
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

test: $(SRC) $(TEST_SRC)
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR) --top-module $(TOP) $(SRC) $(TEST_SRC)
	./$(TEST_BIN)

clean:
	rm -rf $(OBJ_DIR)
