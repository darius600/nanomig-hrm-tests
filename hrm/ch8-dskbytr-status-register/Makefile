# Wo liegt der NanoMig-Checkout? Standard ist das Submodul in diesem Repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = paula_floppy
SRC = $(NANOMIG)/src/minimig-aga/paula_floppy.v \
      $(NANOMIG)/src/minimig-aga/paula_floppy_fifo.v
# paula_floppy_ebr.v declares a SECOND `module paula_floppy` and must never
# be compiled alongside the above (build brief rig hazard) -- deliberately
# not listed here.

# MUT=<NAME> replays one guarded mutation (see the "// MUTATION <NAME> ..."
# line in test_paula_floppy_dskbytr.cpp) by defining MUT_<NAME>=1 for the
# build. Run `make clean` first when switching MUT values -- obj_dir is
# reused otherwise and Verilator will not relink.
MUTFLAGS := $(if $(MUT),-CFLAGS -DMUT_$(MUT)=1,)

test: $(TOP_MODULE)
	./obj_dir/V$(TOP_MODULE) > out.txt
	cat out.txt

$(TOP_MODULE): test_paula_floppy_dskbytr.cpp $(SRC)
	verilator --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal -Wno-pinmissing -Wno-lint $(MUTFLAGS) $(SRC) test_paula_floppy_dskbytr.cpp

clean:
	rm -rf obj_dir out.txt
