# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = minimig
SRC_DIR = $(NANOMIG)/src/minimig-aga

# The fixture is the whole minimig chipset module (minimig.v). It does NOT
# instantiate ide.v/gayle.v (only compiled in `ifndef DISABLE_IDE, which we
# do not define) nor cpu_wrapper.v/cpu_cache_new.v/amiga_clk.v (an actual CPU
# core and the on-FPGA clock generator, both of which live one level above
# minimig.v in the real SoC and are not part of this module's hierarchy at
# all -- minimig.v takes cpu_address/cpu_data/... and clk/clk7_en/c1/c3/cck/
# eclk as plain input ports, driven by the test harness instead). All other
# minimig-aga sources are pulled in via wildcard so nothing is missed. Copied
# from hrm/ch7-overlay-source/Makefile per the build brief; ENABLE_CART is
# deliberately NOT defined (that is chapter 7's subject, not this package's).
EXCLUDE_SOURCES = $(SRC_DIR)/ide.v $(SRC_DIR)/gayle.v $(SRC_DIR)/cpu_wrapper.v $(SRC_DIR)/cpu_cache_new.v $(SRC_DIR)/amiga_clk.v
VERILOG_SOURCES = $(filter-out $(EXCLUDE_SOURCES),$(wildcard $(SRC_DIR)/*.v))

VERILATOR = verilator
# --public-flat-rw added (not present in ch7-overlay-source's own Makefile):
# this package observes internal nets at the minimig.v:622/650 join
# (_ready/_track0/_wprot/_change and _motor/_sel3..0/side/direc/_step) via
# dut->rootp->minimig-><signal>, the same mechanism hrm/ch7-nmi-level-seven
# already uses at this identical TOP_MODULE=minimig boundary. Observation
# only -- nothing is ever forced.
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
