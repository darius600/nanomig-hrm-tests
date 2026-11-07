# ch3-playfield-system: see SPECIFICATION.md.

TEST_CPP := test_ch3_playfield_system.cpp
PROGS    := prog_lpen_ecs_pal_l0 prog_lpen_ecs_pal_l1 \
            prog_lpen_ecs_ntsc_l0 prog_lpen_ecs_ntsc_l1 \
            prog_lpen_ocs_l0 prog_lpen_ocs_l1 \
            prog_lpen_p4_ecs_pal prog_lpen_p4_ecs_ntsc prog_lpen_p4_ocs \
            prog_lpen_ecs_pal_l1_mutd prog_lpen_ecs_pal_l1_burst20 \
            prog_lpen_ecs_pal_l1_lpendis prog_lpen_ecs_pal_l0_wrongstd \
            prog_lpen_ecs_pal_l0_nopoll prog_lpen_p4_ecs_pal_nopoll \
            prog_lpen_p4_ecs_pal_burst20 prog_lpen_p4_ecs_pal_wrongstd \
            prog_lpen_p4_copper prog_lpen_ecs_pal_l0_late \
            prog_p1_2c_pal prog_p1_2c_ntsc prog_p1_16c_pal prog_p1_16c_ntsc \
            prog_p1_2c_pal_narrow prog_p1_16c_pal_narrow \
            prog_p1_2c_pal_badstart prog_p1_16c_pal_badstart \
            prog_lpen_ecs_pal_l1_badstart prog_lpen_ecs_pal_l1_badmark \
            prog_lpen_p4_ecs_pal_badstart prog_lpen_p4_ecs_pal_badmark \
            prog_p5_pal_v1_badstart prog_p5_pal_v1_badmark \
            prog_p1_2c_pal_mod2 prog_p1_2c_pal_rightedge \
            prog_p1_2c_pal_vshift prog_p1_16c_pal_vshift prog_p1_2c_pal_over \
            prog_p1_2c_ntsc_wrongstd \
            prog_p5_pal_v1 prog_p5_pal_v2 prog_p5_ntsc_v1 prog_p5_ntsc_v2 \
            prog_p5_pal_v1_novarvben prog_p5_pal_v1_latediw prog_p5_pal_v1_wrongstd \
            prog_p5_pal_v1_oneskip prog_p5_pal_v1_value prog_p5_pal_v1_vshift \
            prog_lpen_ecs_pal_l1_burst150 prog_lpen_p4_ecs_pal_burst150 \
            prog_lpen_ecs_pal_l1_fourblocks prog_lpen_ecs_pal_l1_sixblocks \
            prog_lpen_ecs_pal_l0_selfmod prog_lpen_ecs_pal_l1_secondbeam \
            prog_lpen_ecs_pal_l0_vhposw prog_lpen_ecs_pal_l1_copper \
            prog_lpen_p4_ecs_pal_nopfirst

include ../harness/nanomig.mk

# Implementation records: the package against a private copy of the NanoMig chip-set
# RTL, made by applying a patch file of this package to a copy of the pinned
# submodule's minimig-aga directory. The submodule is never modified; each copy is
# made under its own directory (obj_dir_latch/, obj_dir_latchrst/), and each core
# builds in its own object directory (obj_dir_<copy>_<core>/) and writes its own
# output file (out_<copy>_<core>.txt), so obj_dir_<core>/ and out_<core>.txt stay those
# of the unmodified build, which `make test` uses.
#
#   latch: latch.patch -- (1) BPLCON0 bit 3 enables a latch of the beam position at the
#       end of the field, held until the counter enters line 20, and BEAMCON0 bit 13
#       makes a read ignore it (reset value 0); (2) the vertical position counts at the
#       clock edge at which the horizontal position wraps; (3) with BEAMCON0 bit 12 set,
#       VBSTRT starts and VBSTOP ends vertical blanking. The blanking signal of (3) is
#       the one that also stops bitplane DMA, which the earlier records do not say.
#         make latch-test-fx68k     builds and runs the fx68k build on the copy
#         make latch-test-tg68k     builds and runs the TG68K build on the copy
#   latchrst: latchrst.patch -- latch.patch with the enable of the latch set after reset.
#         make latchrst-test-fx68k
#         make latchrst-test-tg68k
# The records name the selector and the groups of the run, passed as environment
# variables of the make command, for example
#   NMH_ONLY=P3P4P5 make latch-test-fx68k
#   NMH_MUTATE=P3MAIN make latch-test-tg68k
# The directory of a copy (obj_dir_<copy>/) is shared by its two cores and is removed
# and made again by <copy>-rtl: build the two cores of a copy one after the other
# (<copy>-test does), never in two shells at once.

LATCH_DIR   := $(PKG_DIR)/obj_dir_latch/minimig-aga
LATCH_FILES := $(patsubst $(MINIMIG_DIR)/%,$(LATCH_DIR)/%,$(MINIMIG_FILES))

.PHONY: latch-rtl latch-test latch-test-fx68k latch-test-tg68k
latch-rtl:
	rm -rf obj_dir_latch
	mkdir -p obj_dir_latch
	cp -r $(MINIMIG_DIR) obj_dir_latch/minimig-aga
	cd obj_dir_latch/minimig-aga && patch -p1 < $(PKG_DIR)/latch.patch

latch-test-fx68k: latch-rtl $(BINS)
	rm -rf obj_dir_latch_fx68k out_latch_fx68k.txt
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_latch_fx68k -cc --exe --build -j 0 \
		$(LATCH_FILES) $(FX68K_FILES) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_latch_fx68k/
	( cd obj_dir_latch_fx68k && ./Vnanomig $(PKG_DIR) ) > out_latch_fx68k.txt; rc=$$?; cat out_latch_fx68k.txt; exit $$rc

latch-test-tg68k: latch-rtl $(BINS)
	rm -rf obj_dir_latch_tg68k out_latch_tg68k.txt
	verilator $(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR \
		-I$(NANOMIG)/sim \
		--Mdir obj_dir_latch_tg68k -cc --exe --build -j 0 \
		$(call tg68k_files,$(LATCH_FILES)) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_latch_tg68k && ./Vnanomig $(PKG_DIR) ) > out_latch_tg68k.txt; rc=$$?; cat out_latch_tg68k.txt; exit $$rc

latch-test: latch-test-fx68k latch-test-tg68k

LATCHRST_DIR   := $(PKG_DIR)/obj_dir_latchrst/minimig-aga
LATCHRST_FILES := $(patsubst $(MINIMIG_DIR)/%,$(LATCHRST_DIR)/%,$(MINIMIG_FILES))

.PHONY: latchrst-rtl latchrst-test latchrst-test-fx68k latchrst-test-tg68k
latchrst-rtl:
	rm -rf obj_dir_latchrst
	mkdir -p obj_dir_latchrst
	cp -r $(MINIMIG_DIR) obj_dir_latchrst/minimig-aga
	cd obj_dir_latchrst/minimig-aga && patch -p1 < $(PKG_DIR)/latchrst.patch

latchrst-test-fx68k: latchrst-rtl $(BINS)
	rm -rf obj_dir_latchrst_fx68k out_latchrst_fx68k.txt
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_latchrst_fx68k -cc --exe --build -j 0 \
		$(LATCHRST_FILES) $(FX68K_FILES) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_latchrst_fx68k/
	( cd obj_dir_latchrst_fx68k && ./Vnanomig $(PKG_DIR) ) > out_latchrst_fx68k.txt; rc=$$?; cat out_latchrst_fx68k.txt; exit $$rc

latchrst-test-tg68k: latchrst-rtl $(BINS)
	rm -rf obj_dir_latchrst_tg68k out_latchrst_tg68k.txt
	verilator $(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR \
		-I$(NANOMIG)/sim \
		--Mdir obj_dir_latchrst_tg68k -cc --exe --build -j 0 \
		$(call tg68k_files,$(LATCHRST_FILES)) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_latchrst_tg68k && ./Vnanomig $(PKG_DIR) ) > out_latchrst_tg68k.txt; rc=$$?; cat out_latchrst_tg68k.txt; exit $$rc

latchrst-test: latchrst-test-fx68k latchrst-test-tg68k
