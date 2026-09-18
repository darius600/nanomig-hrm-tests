# Wo liegt der NanoMig-Checkout? Standard ist das Submodul in diesem Repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = paula_uart
SRC = $(NANOMIG)/src/minimig-aga/paula_uart.v

# MUT=<NAME> replays one guarded mutation (see the "// MUTATION <NAME> ..."
# line in test_ch8-serdatr-read-word.cpp) by defining MUT_<NAME>=1 for the
# build. `rm -rf $(OBJ_DIR)` lives in the test recipe itself (not just
# clean) so `make MUT=X test` can never silently relink a stale binary
# whose prerequisites did not list MUT -- the build brief names this trap
# by name.
OBJ_DIR = obj_dir
MUTFLAGS := $(if $(MUT),-CFLAGS -DMUT_$(MUT)=1,)

test:
	rm -rf $(OBJ_DIR) out.txt
	verilator --no-timing --cc --exe --build -j 0 --Mdir $(OBJ_DIR) --top-module $(TOP_MODULE) -Wall -Wno-fatal -Wno-pinmissing -Wno-lint $(MUTFLAGS) $(SRC) test_ch8-serdatr-read-word.cpp
	./$(OBJ_DIR)/V$(TOP_MODULE) > out.txt; rc=$$?; cat out.txt; exit $$rc

clean:
	rm -rf $(OBJ_DIR) out.txt
