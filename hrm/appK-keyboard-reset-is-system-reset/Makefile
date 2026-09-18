# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = hid
SRC_DIR = $(NANOMIG)/src/misc
# hid instantiates `keymap` (amiga_keymap.v) internally -- both files must
# be named explicitly; a directory wildcard would also pull in sd_card.v,
# sdram.sv and other unrelated src/misc peripherals that share the folder
# but not hid's own module tree (same reasoning as ch8-keyboard-events'
# Makefile, which builds this same top module).
SRC = $(SRC_DIR)/hid.v $(SRC_DIR)/amiga_keymap.v

# MUT=<NAME> replays one guarded mutation (see the "// MUTATION <NAME> ..."
# line in test_hid.cpp) by defining MUT_<NAME>=1 for the build, so the
# exact stimulus change that assertion's log entry describes can be
# reproduced later. Run `make clean` first when switching MUT values --
# obj_dir is reused otherwise and Verilator will not relink.
MUTFLAGS := $(if $(MUT),-CFLAGS -DMUT_$(MUT)=1,)

test: $(TOP_MODULE)
	./obj_dir/V$(TOP_MODULE) > out.txt; rc=$$?; cat out.txt; exit $$rc

$(TOP_MODULE): test_$(TOP_MODULE).cpp $(SRC)
	verilator --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal -I$(SRC_DIR) $(MUTFLAGS) $(SRC) test_$(TOP_MODULE).cpp

clean:
	rm -rf obj_dir out.txt
