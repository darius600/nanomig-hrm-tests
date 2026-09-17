# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

TOP := ciab
SRC := $(NANOMIG)/src/minimig-aga/ciab.v \
       $(NANOMIG)/src/minimig-aga/cia_int.v \
       $(NANOMIG)/src/minimig-aga/cia_timera.v \
       $(NANOMIG)/src/minimig-aga/cia_timerb.v \
       $(NANOMIG)/src/minimig-aga/cia_timerd.v
TEST_SRC := test_appF-ciab-port-assignments.cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

# Pass MUT=<NAME> to build one mutation variant, e.g. `make test MUT=MUT_A8_PBON`.
# Unset/empty MUT builds the unmutated baseline. `test`'s recipe always
# deletes $(OBJ_DIR) and rebuilds from scratch, so switching MUT (which
# touches no source file's mtime) never gets served a stale cached binary.
MUT ?=
ifneq ($(MUT),)
FLAGS += -CFLAGS -D$(MUT)=1
endif

.PHONY: all test clean

all: test

test: $(SRC) $(TEST_SRC)
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR) --top-module $(TOP) $(SRC) $(TEST_SRC)
	./$(TEST_BIN)

clean:
	rm -rf $(OBJ_DIR)
