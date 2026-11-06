# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = agnus_bitplanedma
SRC = $(NANOMIG)/src/minimig-aga/$(TOP_MODULE).v

# MUT=<NAME> replays one stimulus mutation (see the "// MUTATION <id> ..."
# records at the bottom of test_agnus_bitplanedma.cpp) by defining
# MUT_<NAME>=1 for the build. A selector changes the stimulus only.
MUTFLAGS := $(if $(MUT),-CFLAGS -DMUT_$(MUT)=1,)

test: $(TOP_MODULE)
	./obj_dir/V$(TOP_MODULE) > out.txt; rc=$$?; cat out.txt; exit $$rc

$(TOP_MODULE): test_$(TOP_MODULE).cpp $(SRC)
	verilator --no-timing --trace --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal $(MUTFLAGS) $(SRC) test_$(TOP_MODULE).cpp

clean:
	rm -rf obj_dir out.txt trace.vcd
