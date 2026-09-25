# nanomig.mk -- the build every integration package shares.
#
# A package's Makefile sets
#
#   TEST_CPP := test_<name>.cpp        the C++ test, which includes nanomig_harness.h
#   PROGS    := prog_a prog_b           68000 programs, one <name>.s each
#
# and then `include ../harness/nanomig.mk`. `make test` assembles every
# program with vasm into <name>.bin, builds nanomig.v once per CPU core -- the
# defines of nano20k/top.sv (fx68k) and of nano20k/top_020.sv (TG68K), plus
# the simulation-only INFER_DPRAM -- and runs the test on each, from its object
# directory: fx68k reads its microcode from the working directory, and a
# binary started anywhere else runs without it. The test gets the directory
# holding the .bin files as its first argument, and writes out_<core>.txt.
# `make test` fails if either run fails.
#
# Requires Verilator and vasm (vasmm68k_mot, http://sun.hasenbraten.de/vasm/),
# both on PATH.

NANOMIG      ?= $(abspath ../../nanomig)
MINIMIG_DIR  := $(NANOMIG)/src/minimig-aga
HARNESS_DIR  := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
VASM         ?= vasmm68k_mot
PKG_DIR      := $(abspath .)

MINIMIG_FILES := \
  $(NANOMIG)/src/nanomig.v \
  $(MINIMIG_DIR)/amiga_clk.v $(MINIMIG_DIR)/cpu_wrapper.v $(MINIMIG_DIR)/cpu_cache_new.v \
  $(MINIMIG_DIR)/dpram.v $(MINIMIG_DIR)/dpram_be_1024x16.v $(MINIMIG_DIR)/minimig.v \
  $(MINIMIG_DIR)/bitplane_ram.v $(MINIMIG_DIR)/sprite_ram.v \
  $(MINIMIG_DIR)/ciaa.v $(MINIMIG_DIR)/ciab.v $(MINIMIG_DIR)/cia_int.v \
  $(MINIMIG_DIR)/cia_timera.v $(MINIMIG_DIR)/cia_timerb.v $(MINIMIG_DIR)/cia_timerd.v \
  $(MINIMIG_DIR)/paula.v $(MINIMIG_DIR)/paula_uart.v $(MINIMIG_DIR)/paula_audio_channel.v \
  $(MINIMIG_DIR)/paula_audio_mixer.v $(MINIMIG_DIR)/paula_audio.v $(MINIMIG_DIR)/paula_audio_volume.v \
  $(MINIMIG_DIR)/paula_floppy_fifo.v $(MINIMIG_DIR)/paula_floppy.v $(MINIMIG_DIR)/paula_intcontroller.v \
  $(MINIMIG_DIR)/agnus.v $(MINIMIG_DIR)/agnus_audiodma.v $(MINIMIG_DIR)/agnus_blitter_adrgen.v \
  $(MINIMIG_DIR)/agnus_blitter_minterm.v $(MINIMIG_DIR)/agnus_diskdma.v $(MINIMIG_DIR)/agnus_beamcounter.v \
  $(MINIMIG_DIR)/agnus_blitter_barrelshifter.v $(MINIMIG_DIR)/agnus_blitter.v $(MINIMIG_DIR)/agnus_refresh.v \
  $(MINIMIG_DIR)/agnus_bitplanedma.v $(MINIMIG_DIR)/agnus_blitter_fill.v $(MINIMIG_DIR)/agnus_copper.v \
  $(MINIMIG_DIR)/agnus_spritedma.v \
  $(MINIMIG_DIR)/denise.v $(MINIMIG_DIR)/denise_bitplane_shifter.v $(MINIMIG_DIR)/denise_collision.v \
  $(MINIMIG_DIR)/denise_colortable.v $(MINIMIG_DIR)/denise_playfields.v $(MINIMIG_DIR)/denise_sprites_shifter.v \
  $(MINIMIG_DIR)/denise_bitplanes.v $(MINIMIG_DIR)/denise_hamgenerator.v $(MINIMIG_DIR)/denise_spritepriority.v \
  $(MINIMIG_DIR)/denise_sprites.v $(MINIMIG_DIR)/denise_colortable_ram_mf.v \
  $(MINIMIG_DIR)/gary.v $(MINIMIG_DIR)/gayle.v $(MINIMIG_DIR)/ide.v \
  $(MINIMIG_DIR)/minimig_m68k_bridge.v $(MINIMIG_DIR)/minimig_bankmapper.v \
  $(MINIMIG_DIR)/minimig_sram_bridge.v $(MINIMIG_DIR)/minimig_syscontrol.v \
  $(NANOMIG)/src/minimig/Amber.v $(MINIMIG_DIR)/userio.v

FX68K_DIR   := $(NANOMIG)/src/fx68k
FX68K_FILES := $(FX68K_DIR)/fx68k.sv $(FX68K_DIR)/fx68kAlu.sv $(FX68K_DIR)/uaddrPla.sv
TG68K_FILE  := $(NANOMIG)/sim/TG68KdotC_Kernel.v

COMMON_VFLAGS := -O3 -DINFER_DPRAM --no-timing -Wno-fatal --threads 1 --top-module nanomig
COMMON_CFLAGS := -std=c++17 -I$(HARNESS_DIR)

BINS := $(addsuffix .bin,$(PROGS))

.PHONY: test test-fx68k test-tg68k progs clean

test: test-fx68k test-tg68k

progs: $(BINS)

%.bin: %.s $(HARNESS_DIR)/nanomig.i
	$(VASM) -quiet -Fbin -I$(HARNESS_DIR) $(VASMFLAGS) -o $@ $<

# fx68k: -Wno-BLKANDNBLK and --no-assert are its upstream simulation options
# (without them it does not elaborate, or stops at time 0); its microcode is
# copied next to the binary, which is run from there.
test-fx68k: $(BINS)
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_fx68k -cc --exe --build -j 0 \
		$(MINIMIG_FILES) $(FX68K_FILES) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_fx68k/
	( cd obj_dir_fx68k && ./Vnanomig $(PKG_DIR) ) > out_fx68k.txt; rc=$$?; cat out_fx68k.txt; exit $$rc

test-tg68k: $(BINS)
	verilator $(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR \
		-I$(NANOMIG)/sim \
		--Mdir obj_dir_tg68k -cc --exe --build -j 0 \
		$(MINIMIG_FILES) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_tg68k && ./Vnanomig $(PKG_DIR) ) > out_tg68k.txt; rc=$$?; cat out_tg68k.txt; exit $$rc

clean:
	rm -rf obj_dir_fx68k obj_dir_tg68k out_*.txt $(BINS)
