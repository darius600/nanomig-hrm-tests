# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = paula_uart
SRC_DIR = $(NANOMIG)/src/minimig-aga
VERILOG_SOURCES = $(SRC_DIR)/paula_uart.v

VERILATOR = verilator
VERILATOR_FLAGS = --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

# Pass MUT=<NAME> on the make command line to build one mutation variant,
# e.g. `make MUT=MUT_A test`. Unset/empty MUT builds the unmutated
# (baseline) binary. Each variant is a full separate build+run -- there is
# no shared state between them, and rm -rf obj_dir below (inside the `test`
# recipe itself, not a file-target prerequisite) means `make MUT=X test`
# never re-runs a stale binary built under a different MUT value.
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
