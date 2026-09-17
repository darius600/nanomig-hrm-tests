# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

TOP := ciaa
SRC := $(NANOMIG)/src/minimig-aga/ciaa.v \
       $(NANOMIG)/src/minimig-aga/cia_int.v \
       $(NANOMIG)/src/minimig-aga/cia_timera.v \
       $(NANOMIG)/src/minimig-aga/cia_timerb.v \
       $(NANOMIG)/src/minimig-aga/cia_timerd.v
TEST_SRC := test_appDE-cia-register-select.cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

# Pass MUT=<NAME> on the make command line to build one mutation variant,
# e.g. `make MUT=MUT_FIXLIVE test`. Unset/empty MUT builds the unmutated
# (baseline) pair. Each test source only reacts to the #define it names, so
# an unrecognised flag is inert.
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

test: $(SRC) $(TEST_SRC) Makefile
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR) --top-module $(TOP) $(SRC) $(TEST_SRC)
	./$(TEST_BIN)

clean:
	rm -rf $(OBJ_DIR)
