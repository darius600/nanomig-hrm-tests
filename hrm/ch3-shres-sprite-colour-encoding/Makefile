NANOMIG ?= ../../nanomig
TOP_MODULE = denise
# PACKAGE.md's own fixture description names the full source set for the
# `denise` fixture (one Verilator build, top module denise): denise.v alone
# does not elaborate, since it instantiates these sub-modules. Same file
# list already used by the sibling denise-fixture packages hrm/denise/ and
# hrm/ch3-horizontal-display-window/ -- filled in from PACKAGE.md, not from
# opening denise.v.
SRC = $(NANOMIG)/src/minimig-aga/denise.v \
      $(NANOMIG)/src/minimig-aga/denise_bitplanes.v \
      $(NANOMIG)/src/minimig-aga/denise_bitplane_shifter.v \
      $(NANOMIG)/src/minimig-aga/denise_playfields.v \
      $(NANOMIG)/src/minimig-aga/denise_sprites.v \
      $(NANOMIG)/src/minimig-aga/denise_sprites_shifter.v \
      $(NANOMIG)/src/minimig-aga/denise_collision.v \
      $(NANOMIG)/src/minimig-aga/denise_hamgenerator.v \
      $(NANOMIG)/src/minimig-aga/denise_colortable.v \
      $(NANOMIG)/src/minimig-aga/denise_colortable_ram_mf.v \
      $(NANOMIG)/src/minimig-aga/denise_spritepriority.v \
      $(NANOMIG)/src/minimig-aga/bitplane_ram.v

test: $(TOP_MODULE)
	./obj_dir/V$(TOP_MODULE) > out.txt
	cat out.txt

$(TOP_MODULE): test_$(TOP_MODULE).cpp $(SRC)
	verilator --no-timing --trace --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal $(SRC) test_$(TOP_MODULE).cpp

clean:
	rm -rf obj_dir out.txt trace.vcd
