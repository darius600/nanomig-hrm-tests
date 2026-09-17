# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = minimig
SRC_DIR = $(NANOMIG)/src/minimig-aga

# The fixture is the whole minimig chipset module (minimig.v), exactly as
# hrm/ch7-overlay-source and hrm/ch8-cia-disk-signal-bindings already build
# it at this identical TOP_MODULE=minimig boundary (Makefile structure
# copied from hrm/ch8-cia-disk-signal-bindings/Makefile per the build
# brief). It does NOT instantiate ide.v/gayle.v (only compiled in `ifndef
# DISABLE_IDE, which we do not define) nor cpu_wrapper.v/cpu_cache_new.v/
# amiga_clk.v (an actual CPU core and the on-FPGA clock generator, both of
# which live one level above minimig.v in the real SoC and are not part of
# this module's hierarchy at all -- minimig.v takes cpu_address/cpu_data/...
# and clk/clk7_en/c1/c3/cck/eclk as plain input ports, driven by the test
# harness's ClkGen instead). All other minimig-aga sources are pulled in via
# wildcard so nothing is missed. ENABLE_CART is deliberately NOT defined
# (not this package's subject).
EXCLUDE_SOURCES = $(SRC_DIR)/ide.v $(SRC_DIR)/gayle.v $(SRC_DIR)/cpu_wrapper.v $(SRC_DIR)/cpu_cache_new.v $(SRC_DIR)/amiga_clk.v
VERILOG_SOURCES = $(filter-out $(EXCLUDE_SOURCES),$(wildcard $(SRC_DIR)/*.v))

VERILATOR = verilator
# --public-flat-rw: this package only READS internal/top-level nets it
# already has real ports for (dtr/rts/cd/cts/dsr are minimig.v's own
# top-level ports; the CIA registers are reached through real CPU bus
# cycles) -- no internal net is ever forced. Kept for consistency with this
# fixture's other packages and because Vminimig_minimig.h's generated
# accessors (used by the reused ClkGen/Harness plumbing) require it.
VERILATOR_FLAGS = --public-flat-rw --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal -Wno-pinmissing -Wno-lint -DDISABLE_IDE -I$(SRC_DIR)

# Pass MUT=<NAME> on the make command line to build one mutation variant,
# e.g. `make MUT=MUT_A1`. Unset/empty MUT builds the unmutated (baseline)
# binary. Each variant is a full separate build+run -- there is no shared
# state between them.
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
