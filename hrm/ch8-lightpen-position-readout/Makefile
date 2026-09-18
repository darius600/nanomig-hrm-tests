# Wo liegt der NanoMig-Checkout? Standard ist das Submodul in diesem Repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = agnus_beamcounter
SRC = $(NANOMIG)/src/minimig-aga/$(TOP_MODULE).v

# Optional: -DMUTATE_<label> for a mutation-verification build, e.g.
#   make MUTATE=MUTATE_A1
MUTATE ?=

test: $(TOP_MODULE)
	./obj_dir/V$(TOP_MODULE) > out.txt; rc=$$?; cat out.txt; exit $$rc

$(TOP_MODULE): test_$(TOP_MODULE).cpp $(SRC)
	verilator --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal $(if $(MUTATE),-CFLAGS -D$(MUTATE)) $(SRC) test_$(TOP_MODULE).cpp

clean:
	rm -rf obj_dir out.txt
