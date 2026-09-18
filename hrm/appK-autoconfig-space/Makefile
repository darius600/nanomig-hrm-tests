# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = cpu_wrapper
# Fixture inherited from hrm/appK-autoconfig-register-map, extended with
# the write capability hrm/appK-autoconfig-size-encoding added -- see
# SPECIFICATION.md.
SRC = $(NANOMIG)/src/minimig-aga/$(TOP_MODULE).v $(NANOMIG)/sim/TG68KdotC_Kernel.v

MUTFLAGS := $(if $(MUT),-CFLAGS -DMUT_$(MUT)=1,)

test: $(TOP_MODULE)
	./obj_dir/V$(TOP_MODULE) > out.txt; rc=$$?; cat out.txt; exit $$rc

$(TOP_MODULE): test_$(TOP_MODULE).cpp $(SRC)
	verilator --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -DENABLE_TG68K=1 -Wall -Wno-fatal $(MUTFLAGS) $(SRC) test_$(TOP_MODULE).cpp

clean:
	rm -rf obj_dir out.txt
