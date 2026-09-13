# Wo liegt der NanoMig-Checkout? Standard ist das Submodul in diesem Repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = rig_copper_intreq
SRC = $(NANOMIG)/src/minimig-aga/agnus_copper.v $(NANOMIG)/src/minimig-aga/paula_intcontroller.v rig_copper_intreq.v
TESTBENCH = test_ch7_copper_interrupt_source.cpp

VERILATOR = verilator
VFLAGS = --trace --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

.PHONY: all test clean

all: test

test: $(TESTBENCH) $(SRC)
	rm -rf obj_dir
	$(VERILATOR) $(VFLAGS) $(SRC) $(TESTBENCH)
	./obj_dir/V$(TOP_MODULE)

clean:
	rm -rf obj_dir
