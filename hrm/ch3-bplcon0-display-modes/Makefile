NANOMIG ?= ../../nanomig
TOP_MODULE = denise
SRC = $(NANOMIG)/src/minimig-aga/$(TOP_MODULE).v

# MUTANT=M<n> (M1..M6; see MUTATIONS.md and mutations/M<n>.patch) rebuilds
# against a disposable mutant copy of the one or two RTL file(s) that patch
# touches. The mutant copy is seeded by reading (never writing) the real
# file(s) out of the read-only nanomig/ submodule into obj_dir/mutant_M<n>/,
# then applying mutations/M<n>.patch there with `patch`. That directory is
# put FIRST on Verilator's -y search path (and, when the mutation touches
# $(TOP_MODULE).v itself, used directly as the top-level source), so the
# mutant resolves and every untouched sibling module still resolves from the
# real nanomig/src/minimig-aga/ directory. Leaving MUTANT unset builds the
# plain sources unchanged: no mutant dir is created or searched.
MUTANT ?=
MUTANT_FILES_M1 = denise.v
MUTANT_FILES_M2 = denise.v
MUTANT_FILES_M3 = denise_bitplane_shifter.v
MUTANT_FILES_M4 = denise.v
MUTANT_FILES_M5 = denise.v
MUTANT_FILES_M6 = denise.v

ifneq ($(MUTANT),)
MUTDIR = obj_dir/mutant_$(MUTANT)
MUTFILES = $(MUTANT_FILES_$(MUTANT))
YFLAGS = -y $(MUTDIR) -y $(dir $(SRC))
ifneq ($(filter $(TOP_MODULE).v,$(MUTFILES)),)
EFF_SRC = $(MUTDIR)/$(TOP_MODULE).v
else
EFF_SRC = $(SRC)
endif
else
YFLAGS = -y $(dir $(SRC))
EFF_SRC = $(SRC)
endif

test: $(TOP_MODULE)
	./obj_dir/V$(TOP_MODULE) > out.txt; rc=$$?; cat out.txt; exit $$rc

$(TOP_MODULE): test_$(TOP_MODULE).cpp $(SRC) mutant-stage
	verilator --no-timing --trace --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal $(YFLAGS) $(EFF_SRC) test_$(TOP_MODULE).cpp

.PHONY: mutant-stage
mutant-stage:
ifneq ($(MUTANT),)
	mkdir -p $(MUTDIR)
	rm -f $(MUTDIR)/*.v
	for f in $(MUTFILES); do cp $(NANOMIG)/src/minimig-aga/$$f $(MUTDIR)/$$f; done
	patch -p0 -d $(MUTDIR) < mutations/$(MUTANT).patch
endif

clean:
	rm -rf obj_dir out.txt trace.vcd
