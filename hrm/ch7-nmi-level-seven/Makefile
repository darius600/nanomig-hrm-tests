# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = minimig
SRC_DIR = $(NANOMIG)/src/minimig-aga

# The fixture is the whole minimig chipset module (minimig.v), same boundary
# as the ch7-overlay-source package's Makefile (copied from there per the
# task sheet). It does NOT instantiate ide.v/gayle.v (only compiled in
# `ifndef DISABLE_IDE, which we DO define, same as the sibling) nor
# cpu_wrapper.v/cpu_cache_new.v/amiga_clk.v (an actual CPU core and the
# on-FPGA clock generator, both of which live one level above minimig.v in
# the real SoC and are not part of this module's hierarchy at all --
# minimig.v takes cpu_address/cpu_data/... and clk/clk7_en/c1/c3/cck/eclk as
# plain input ports, driven by the test harness instead). All other
# minimig-aga sources, including cart.v, are pulled in via wildcard so
# nothing is missed.
#
# UNLIKE the sibling package, this Makefile also defines ENABLE_CART. This
# package's own subject (Figure 7-4's NMI row) lives entirely behind that
# define: minimig.v:754 ties the int7 signal that carries it to a constant
# 1'b0 when ENABLE_CART is not set, and no build anywhere else in this tree
# sets it (STEP3-NOTES-ch7-nmi-level-seven.md, SPECIFICATION.md "Scope and
# limits"). A build of this package is therefore evidence about the
# define-enabled RTL only, never about NanoMig as it ships -- see this
# package's own test_minimig.cpp file header for the full statement.
EXCLUDE_SOURCES = $(SRC_DIR)/ide.v $(SRC_DIR)/gayle.v $(SRC_DIR)/cpu_wrapper.v $(SRC_DIR)/cpu_cache_new.v $(SRC_DIR)/amiga_clk.v
VERILOG_SOURCES = $(filter-out $(EXCLUDE_SOURCES),$(wildcard $(SRC_DIR)/*.v))

VERILATOR = verilator
VERILATOR_FLAGS = --public-flat-rw --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal -Wno-pinmissing -Wno-lint -DDISABLE_IDE -DENABLE_CART -I$(SRC_DIR)

# Pass MUT=<NAME> on the make command line to build one mutation variant,
# e.g. `make MUT=MUT_NOFREEZE`. Unset/empty MUT builds the unmutated
# (baseline) binary. Each variant is a full separate build+run -- there is
# no shared state between them.
MUT ?=
ifneq ($(MUT),)
VERILATOR_FLAGS += -CFLAGS -D$(MUT)=1
endif

CPP_TEST = test_$(TOP_MODULE).cpp

.PHONY: all test clean

all: test

test: $(CPP_TEST) $(VERILOG_SOURCES)
	rm -rf obj_dir
	$(VERILATOR) $(VERILATOR_FLAGS) $(VERILOG_SOURCES) $(CPP_TEST)
	./obj_dir/V$(TOP_MODULE)

clean:
	rm -rf obj_dir
