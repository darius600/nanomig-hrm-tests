# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig
MADIR := $(NANOMIG)/src/minimig-aga

# RIG over three independent Verilated tops -- no shared Verilog parent, so
# each is elaborated separately and the joins (paula_floppy.index -> ciab.flag,
# ciab.irq -> paula_intcontroller.int6) are made in the C++ harness, per
# SPECIFICATION.md. paula_floppy_ebr.v is deliberately NOT in PF_SRC: it
# declares a second `module paula_floppy` (build brief rig hazard).
PF_SRC  := $(MADIR)/paula_floppy.v $(MADIR)/paula_floppy_fifo.v
CB_SRC  := $(MADIR)/ciab.v $(MADIR)/cia_int.v $(MADIR)/cia_timera.v \
           $(MADIR)/cia_timerb.v $(MADIR)/cia_timerd.v
PIC_SRC := $(MADIR)/paula_intcontroller.v

TEST_SRC := test_ch8-disk-index-interrupt-path.cpp

OBJ_DIR := obj_dir
OBJ_PF  := $(OBJ_DIR)/pf
OBJ_CB  := $(OBJ_DIR)/ciab
OBJ_PIC := $(OBJ_DIR)/pic
TEST_BIN := $(OBJ_DIR)/Vrig

VERILATOR ?= verilator
VFLAGS := --no-timing --cc -Wall -Wno-fatal -Wno-pinmissing -Wno-lint
VROOT := $(shell $(VERILATOR) --getenv VERILATOR_ROOT)

# MUT=<label> reruns the test binary (already built) with MUTATE=<label> in
# its environment -- the binary itself reads MUTATE and switches in the
# recorded stimulus mutation for that one label. `make test` / `make` run
# the baseline (MUTATE unset).
MUT ?=

.PHONY: all test build clean

all: test

build: $(TEST_BIN)

# verilator does not mkdir -p a nested --Mdir starting from nothing (it
# needs the parent to already exist) -- create it first, or the very first
# clean build fails with "Can't write file: obj_dir/pf/...".
$(OBJ_PF)/Vpaula_floppy__ALL.a: $(PF_SRC)
	rm -rf $(OBJ_PF)
	mkdir -p $(OBJ_PF)
	$(VERILATOR) $(VFLAGS) --Mdir $(OBJ_PF) --top-module paula_floppy $(PF_SRC)
	$(MAKE) -C $(OBJ_PF) -f Vpaula_floppy.mk Vpaula_floppy__ALL.a

$(OBJ_CB)/Vciab__ALL.a: $(CB_SRC)
	rm -rf $(OBJ_CB)
	mkdir -p $(OBJ_CB)
	$(VERILATOR) $(VFLAGS) --Mdir $(OBJ_CB) --top-module ciab $(CB_SRC)
	$(MAKE) -C $(OBJ_CB) -f Vciab.mk Vciab__ALL.a

$(OBJ_PIC)/Vpaula_intcontroller__ALL.a: $(PIC_SRC)
	rm -rf $(OBJ_PIC)
	mkdir -p $(OBJ_PIC)
	$(VERILATOR) $(VFLAGS) --Mdir $(OBJ_PIC) --top-module paula_intcontroller $(PIC_SRC)
	$(MAKE) -C $(OBJ_PIC) -f Vpaula_intcontroller.mk Vpaula_intcontroller__ALL.a

$(TEST_BIN): $(TEST_SRC) $(OBJ_PF)/Vpaula_floppy__ALL.a $(OBJ_CB)/Vciab__ALL.a $(OBJ_PIC)/Vpaula_intcontroller__ALL.a
	g++ -std=c++14 -O1 \
	  -I $(OBJ_PF) -I $(OBJ_CB) -I $(OBJ_PIC) \
	  -I $(VROOT)/include -I $(VROOT)/include/vltstd \
	  -DVM_COVERAGE=0 -DVM_SC=0 -DVM_TIMING=0 -DVM_TRACE=0 \
	  $(TEST_SRC) $(VROOT)/include/verilated.cpp $(VROOT)/include/verilated_threads.cpp \
	  $(OBJ_PF)/Vpaula_floppy__ALL.a $(OBJ_CB)/Vciab__ALL.a $(OBJ_PIC)/Vpaula_intcontroller__ALL.a \
	  -o $(TEST_BIN)

test: $(TEST_BIN)
ifeq ($(MUT),)
	./$(TEST_BIN)
else
	MUTATE=$(MUT) ./$(TEST_BIN)
endif

clean:
	rm -rf $(OBJ_DIR)
