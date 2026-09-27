# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = agnus_spritedma
SRC = $(NANOMIG)/src/minimig-aga/$(TOP_MODULE).v

# MUTFLAGS: passed through to the C++ build so the MUTATION LOG's compile-time
# guards (-DMUT_<id>=1) can be replayed without hand-editing and reverting the
# source file. Empty by default -- a plain "make" builds the committed baseline.
MUTFLAGS ?=

test: $(TOP_MODULE)
	./obj_dir/V$(TOP_MODULE)

$(TOP_MODULE): test_$(TOP_MODULE).cpp $(SRC)
	rm -rf obj_dir
	verilator --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal $(if $(MUTFLAGS),-CFLAGS "$(MUTFLAGS)") $(SRC) test_$(TOP_MODULE).cpp

clean:
	rm -rf obj_dir
