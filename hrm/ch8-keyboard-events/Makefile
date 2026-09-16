# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = hid
SRC_DIR = $(NANOMIG)/src/misc

# The fixture is `hid` alone, plus the `keymap` instance it contains
# (amiga_keymap.v). Nothing else in src/misc is part of hid's hierarchy --
# unlike ch7-overlay-source's minimig.v, a wildcard over the whole directory
# would pull in sd_card.v, sdram.sv and seven other unrelated peripherals
# that share the folder but not the module tree, so the two files are named
# explicitly instead.
VERILOG_SOURCES = $(SRC_DIR)/hid.v $(SRC_DIR)/amiga_keymap.v

VERILATOR = verilator
VERILATOR_FLAGS = --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal -Wno-pinmissing -Wno-lint -I$(SRC_DIR)

# Pass MUT=<NAME> on the make command line to build one mutation variant,
# e.g. `make MUT=MUT_7`. Unset/empty MUT builds the unmutated (baseline)
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
