# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = cpu_wrapper
# Fixture inherited from hrm/appK-autoconfig-register-map -- see
# SPECIFICATION.md. This build additionally defines ENABLE_RAM32, a real
# per-board `define (icepi-zero, icesugar-pro, two mistle targets) that
# selects the "8MB" Zorro-II size-code arm (chip_addr index 1's `default`
# case, cpu_wrapper.v:575); without it fastramcfg=3 takes the "4MB" arm
# (:577) and the default is "0.5MB" (:580), so size code 000 (8MB) is never
# emitted in Zorro II mode and K-0796/K-0772's reachable half could not be
# graded. No file under nanomig/ is modified.
# The TG68K the boards synthesize, translated by GHDL, and cpu_wrapper.v with
# the board's skipFetch connection, both generated at build time: see
# ../../integration/harness/tg68k.mk (TG68K_KERNEL=shipped compiles the
# submodule's sim/TG68KdotC_Kernel.v and unmodified cpu_wrapper.v instead).
include ../../integration/harness/tg68k.mk
SRC = $(call tg68k_files,$(NANOMIG)/src/minimig-aga/$(TOP_MODULE).v) $(TG68K_FILE)

MUTFLAGS := $(if $(MUT),-CFLAGS -DMUT_$(MUT)=1,)

test: $(TOP_MODULE)
	./obj_dir/V$(TOP_MODULE) > out.txt; rc=$$?; cat out.txt; exit $$rc

$(TOP_MODULE): test_$(TOP_MODULE).cpp $(SRC)
	verilator --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -DENABLE_TG68K=1 -DENABLE_RAM32=1 -Wall -Wno-fatal $(MUTFLAGS) $(SRC) test_$(TOP_MODULE).cpp

clean:
	rm -rf obj_dir out.txt
