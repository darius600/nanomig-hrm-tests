# Wo liegt der NanoMig-Checkout? Standard ist das Submodul in diesem Repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = agnus_bitplanedma
SRC = $(NANOMIG)/src/minimig-aga/$(TOP_MODULE).v

# Replay one mutation: `make clean && make MUT=MUT_NAME test`
MUT ?=
MUTFLAGS := $(if $(MUT),-CFLAGS "-D$(MUT)=1",)

test: $(TOP_MODULE)
	./obj_dir/V$(TOP_MODULE) > out.txt; rc=$$?; cat out.txt; exit $$rc

$(TOP_MODULE): test_$(TOP_MODULE).cpp $(SRC)
	verilator --no-timing --trace --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal $(MUTFLAGS) $(SRC) test_$(TOP_MODULE).cpp

clean:
	rm -rf obj_dir out.txt trace.vcd
