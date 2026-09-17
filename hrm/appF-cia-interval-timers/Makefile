# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

CIA_SUBMODULES := $(NANOMIG)/src/minimig-aga/cia_int.v \
                   $(NANOMIG)/src/minimig-aga/cia_timera.v \
                   $(NANOMIG)/src/minimig-aga/cia_timerb.v \
                   $(NANOMIG)/src/minimig-aga/cia_timerd.v

# One fixture, ciaa only. Every claim in this package is about the interval
# timer register map and control logic ciaa.v decodes and cia_timera.v/
# cia_timerb.v implement; ciaa alone already carries one instance of each
# timer, which is enough to grade the linked (Timer B counts Timer A
# underflows) proposition without a second chip. cia_timerd.v is present
# only so Verilator can elaborate ciaa (TOD/alarm is a different package's
# subject, appF-cia-tod-clock-and-alarm).
SRC := $(NANOMIG)/src/minimig-aga/ciaa.v $(CIA_SUBMODULES)

TEST_SRC := test_ciaa.cpp
OBJ_DIR := obj_dir_ciaa
TEST_BIN := $(OBJ_DIR)/Vciaa

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

# Pass MUT=<NAME> to build one mutation variant, e.g. `make MUT=MUT_NOTICK
# test`. Unset/empty MUT builds the unmutated (baseline) fixture. The macro
# is passed as a build flag, never matched against a label string at
# runtime.
MUT ?=
ifneq ($(MUT),)
FLAGS += -CFLAGS -D$(MUT)=1
endif

.PHONY: all test build clean

all: test

build: $(SRC) $(TEST_SRC) Makefile
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR) --top-module ciaa $(SRC) $(TEST_SRC)

# test always rebuilds from scratch, regardless of MUT, so a stale binary
# from a previous MUT value can never be silently re-run and reported as a
# different variant's result.
test: $(SRC) $(TEST_SRC) Makefile
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR) --top-module ciaa $(SRC) $(TEST_SRC)
	./$(TEST_BIN)

clean:
	rm -rf $(OBJ_DIR)
