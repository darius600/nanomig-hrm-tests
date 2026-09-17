# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

# Two independent fixtures -- minimig_m68k_bridge (the task sheet's own
# `top`, carries the R/W assertions plus this package's four declined
# citations) and amiga_clk (the E-clock proposition). Neither paula_uart
# nor agnus is built: both of this package's citations naming them are pure
# duplicate declines that need no fresh measurement (see SPECIFICATION.md),
# so building either would add a compiled fixture with zero test content.
SRC_BRIDGE := $(NANOMIG)/src/minimig-aga/minimig_m68k_bridge.v
SRC_CLK    := $(NANOMIG)/src/minimig-aga/amiga_clk.v

TEST_SRC_BRIDGE := test_minimig_m68k_bridge.cpp
TEST_SRC_CLK    := test_amiga_clk.cpp

OBJ_DIR_BRIDGE := obj_dir_minimig_m68k_bridge
OBJ_DIR_CLK    := obj_dir_amiga_clk

TEST_BIN_BRIDGE := $(OBJ_DIR_BRIDGE)/Vminimig_m68k_bridge
TEST_BIN_CLK    := $(OBJ_DIR_CLK)/Vamiga_clk

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

# Pass MUT=<FULL_MACRO_NAME> to build one mutation variant, e.g.
# `make MUT=MUT_FIXLIVE test`. Applied to both fixtures' builds; each test
# source only reacts to the #define it names, so a flag meant for one
# fixture is an inert unused macro in the other's compile.
MUT ?=
ifneq ($(MUT),)
FLAGS += -CFLAGS -D$(MUT)=1
endif

.PHONY: all test build clean

all: test

build: $(SRC_BRIDGE) $(TEST_SRC_BRIDGE) $(SRC_CLK) $(TEST_SRC_CLK) Makefile
	rm -rf $(OBJ_DIR_BRIDGE) $(OBJ_DIR_CLK)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR_BRIDGE) --top-module minimig_m68k_bridge $(SRC_BRIDGE) $(TEST_SRC_BRIDGE)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR_CLK) --top-module amiga_clk $(SRC_CLK) $(TEST_SRC_CLK)

# `test` deliberately does NOT depend on the binaries as file targets, and
# always rebuilds both fixtures from scratch regardless of MUT -- see
# appF-cia-control-registers/Makefile for why (a stale binary from a
# previous MUT value must never be silently reused). Both binaries always
# run, even if the first exits non-zero, so a red in one fixture can never
# mask whether the other even ran.
test: $(SRC_BRIDGE) $(TEST_SRC_BRIDGE) $(SRC_CLK) $(TEST_SRC_CLK) Makefile
	rm -rf $(OBJ_DIR_BRIDGE) $(OBJ_DIR_CLK)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR_BRIDGE) --top-module minimig_m68k_bridge $(SRC_BRIDGE) $(TEST_SRC_BRIDGE)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR_CLK) --top-module amiga_clk $(SRC_CLK) $(TEST_SRC_CLK)
	@ec=0; \
	./$(TEST_BIN_BRIDGE) || ec=1; \
	./$(TEST_BIN_CLK) || ec=1; \
	exit $$ec

clean:
	rm -rf $(OBJ_DIR_BRIDGE) $(OBJ_DIR_CLK)
