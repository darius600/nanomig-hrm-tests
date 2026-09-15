# Wo liegt der NanoMig-Checkout? Standard ist das Submodul in diesem Repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = agnus_diskdma
SRC = $(NANOMIG)/src/minimig-aga/$(TOP_MODULE).v
TESTBENCH = test_ch8_disk_dma_pointer.cpp

# MUT=<name> builds the mutation guarded by -DMUT_<name> in the testbench,
# for the mutation sweep BUILD-BRIEF requires. Baseline: `make test`.
MUT ?=

.PHONY: all test clean

all: test

test: $(TESTBENCH) $(SRC)
	rm -rf obj_dir
ifeq ($(MUT),)
	verilator --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal $(SRC) $(TESTBENCH)
else
	verilator --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal -CFLAGS -DMUT_$(MUT) $(SRC) $(TESTBENCH)
endif
	./obj_dir/V$(TOP_MODULE)

clean:
	rm -rf obj_dir
