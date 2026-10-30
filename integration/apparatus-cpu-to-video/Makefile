# Makefile
#
# Builds and runs the cpu-to-video apparatus check twice: once against the
# fx68k core (the configuration nanomig/src/tang/nano20k/top.sv gives the
# design -- it defines none of the CPU/cache macros, so cpu_wrapper.v's
# own fallback picks ENABLE_FX68K) and once against the tg68k core (the
# configuration nanomig/src/tang/nano20k/top_020.sv gives the design:
# ENABLE_TG68K ENABLE_AGA CPU_SLOW14 ENABLE_CACHE CHIPRAM_CACHE NO_WS2812
# DENISE_EBR). Both builds also pass -DINFER_DPRAM: it is read only by
# minimig-aga/ide.v, which uses it to replace an FPGA RAM primitive with
# inferred RAM so the design can be simulated at all -- a simulation-only
# option, not part of either shipped configuration.
#
# File list: nanomig/sim/Makefile's MINIMIG_FILES, minus the files that
# only exist for that upstream testbench (nanomig_tb.v/.cpp, the sd card
# and sdram models under src/misc -- nanomig.v's sdc_*/ram_address ports
# are driven directly by the harness here instead).

NANOMIG      ?= ../../nanomig
MINIMIG_DIR  := $(NANOMIG)/src/minimig-aga
# absolute: verilator compiles the --exe sources from inside --Mdir, two
# levels below this Makefile, so a path relative to here would be wrong
HARNESS_DIR  := $(abspath ../harness)

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

include ../harness/tg68k.mk

COMMON_VFLAGS := -O3 -DINFER_DPRAM --no-timing -Wno-fatal --threads 1 --top-module nanomig
COMMON_CFLAGS := -std=c++17 -I$(HARNESS_DIR)

.PHONY: test clean test-fx68k test-tg68k

test: test-fx68k test-tg68k

# fx68k needs -Wno-BLKANDNBLK (else %Error-BLKANDNBLK at fx68k.sv:293) and
# --no-assert (else its "unique case" runtime checks stop the run at time
# 0 under Verilator). It $readmemb's microrom.mem/nanorom.mem from the
# current working directory, so those are copied into the object
# directory and the binary is run from there; output is teed back into
# this package directory.
test-fx68k:
	@echo "=== verilator: fx68k ==="
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_fx68k -cc --exe --build -j 0 \
		$(MINIMIG_FILES) $(FX68K_FILES) test_apparatus.cpp \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_fx68k/
	@echo "=== fx68k: normal run ==="
	( cd obj_dir_fx68k && ./Vnanomig ) | tee out_fx68k.txt
	@echo "=== fx68k: control run (MUTATE=nocolour) ==="
	( cd obj_dir_fx68k && MUTATE=nocolour ./Vnanomig ) | tee out_fx68k_control.txt
	@grep -q "^FAIL: video shows the written COLOR00 guns" out_fx68k_control.txt && \
		echo "CONTROL fx68k: colour check correctly failed under MUTATE=nocolour" || \
		( echo "CONTROL fx68k: colour check did NOT fail under MUTATE=nocolour -- control is broken"; exit 1 )
	@grep -q ", 0 failed" out_fx68k.txt || ( echo "fx68k: normal run had failing checks"; exit 1 )

# tg68k: the kernel the boards synthesize, translated by GHDL, and a copy of
# cpu_wrapper.v whose Verilator instance (:420-441) also connects skipFetch as
# the board instance does (:464); both are generated by ../harness/tg68k.mk.
# TG68K_KERNEL=shipped compiles nanomig/sim/TG68KdotC_Kernel.v and the
# unmodified cpu_wrapper.v instead.
test-tg68k:
	@echo "=== verilator: tg68k ==="
	verilator $(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR \
		-I$(NANOMIG)/sim \
		--Mdir obj_dir_tg68k -cc --exe --build -j 0 \
		$(call tg68k_files,$(MINIMIG_FILES)) $(TG68K_FILE) test_apparatus.cpp \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	@echo "=== tg68k: normal run ==="
	( cd obj_dir_tg68k && ./Vnanomig ) | tee out_tg68k.txt
	@echo "=== tg68k: control run (MUTATE=nocolour) ==="
	( cd obj_dir_tg68k && MUTATE=nocolour ./Vnanomig ) | tee out_tg68k_control.txt
	@grep -q "^FAIL: video shows the written COLOR00 guns" out_tg68k_control.txt && \
		echo "CONTROL tg68k: colour check correctly failed under MUTATE=nocolour" || \
		( echo "CONTROL tg68k: colour check did NOT fail under MUTATE=nocolour -- control is broken"; exit 1 )
	@grep -q ", 0 failed" out_tg68k.txt || ( echo "tg68k: normal run had failing checks"; exit 1 )

clean:
	rm -rf obj_dir_fx68k obj_dir_tg68k out_*.txt
