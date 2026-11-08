# ch7-system-control: see SPECIFICATION.md.

TEST_CPP := test_ch7_system_control.cpp
PROGS    := prog_lp_ocs_pal prog_lp_ecs_pal prog_lp_ocs_ntsc prog_lp_ecs_ntsc \
            prog_lp_ecs_pal_lpenclear prog_lp_ecs_pal_anchormut \
            prog_lp_ecs_pal_v1delay prog_lp_ecs_pal_ddraforce \
            prog_lp_ecs_pal_nocopen prog_lp_ecs_pal_nodone prog_lp_ecs_pal_evmis \
            prog_p4 prog_p4_nolpendis prog_p4_f3lpendis prog_p4_ddraforce prog_p4_anchormut prog_p4_periodmut prog_p4_shortspin \
            prog_p4_ydelay prog_p4_nocopen prog_p4_nodone prog_p4_f1nolpen \
            prog_pri_pal prog_pri_ntsc \
            prog_pri_pal_amirror prog_pri_pal_bswap prog_pri_pal_priflip prog_pri_pal_erev \
            prog_pri_pal_scroll prog_pri_pal_tagbad prog_pri_pal_sprenclr prog_pri_pal_nocop \
            prog_pri_pal_bpl2zero prog_pri_pal_vstop130 prog_pri_pal_latemark \
            prog_pri_pal_tagcount prog_pri_pal_avstop130 \
            prog_p2_pal prog_p2_ntsc \
            prog_p2_pal_sprenclr prog_p2_pal_scroll prog_p2_pal_tagid prog_p2_pal_nocop \
            prog_p2_pal_bpl4plane2 prog_p2_pal_bpl4zero prog_p2_pal_latemark

include ../harness/nanomig.mk

# Implementation records and contrasts: the same package against a private copy
# of the NanoMig chip-set RTL, made by applying patch files of this package to a
# copy of the pinned submodule's minimig-aga directory. The submodule is never
# modified; each copy is made under its own directory (obj_dir_latch/,
# obj_dir_norel/), and each core builds in its own object directory
# (obj_dir_<copy>_<core>/) and writes its own output file
# (out_<copy>_<core>.txt), so obj_dir_<core>/ and out_<core>.txt stay those of
# the unmodified build, which `make test` uses.
#
#   latch: latch.patch -- a light-pen latch that follows pp.238-239 and p.305:
#       BPLCON0 bit 3 enables it (reset to 0); a falling edge of pin 6 of
#       either port (the AND of both fire inputs) after vertical position 20
#       latches vpos and hpos; a field end with nothing latched latches them;
#       vertical position 20 releases the latch; VPOSR and VHPOSR return the
#       latched value unless ECS LPENDIS (BEAMCON0 bit 13) is set.
#         make latch-test-fx68k     builds and runs the fx68k build on the copy
#         make latch-test-tg68k     builds and runs the TG68K build on the copy
#         make latch-mut-fx68k SEL=P4-F3LPENDIS   runs that selector on the
#         make latch-mut-tg68k SEL=P4-F3LPENDIS   binary of the latch build
#   norel: latch.patch, then norel.patch -- the latch releases at vertical
#       position 50 instead of 20. The run selects NMH_MUTATE=P3-RELEASE50.
#         make norel-test-fx68k
#         make norel-test-tg68k
# The directory of a copy (obj_dir_<copy>/) is shared by its two cores and is
# removed and made again by <copy>-rtl: build the two cores of a copy one
# after the other (<copy>-test does), never in two shells at once.

LATCH_DIR   := $(PKG_DIR)/obj_dir_latch/minimig-aga
LATCH_FILES := $(patsubst $(MINIMIG_DIR)/%,$(LATCH_DIR)/%,$(MINIMIG_FILES))

.PHONY: latch-rtl latch-test latch-test-fx68k latch-test-tg68k latch-mut-fx68k latch-mut-tg68k
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

latch-mut-fx68k:
	( cd obj_dir_latch_fx68k && NMH_MUTATE=$(SEL) ./Vnanomig $(PKG_DIR) ) > out_latch_$(SEL)_fx68k.txt; rc=$$?; cat out_latch_$(SEL)_fx68k.txt; exit $$rc
latch-mut-tg68k:
	( cd obj_dir_latch_tg68k && NMH_MUTATE=$(SEL) ./Vnanomig $(PKG_DIR) ) > out_latch_$(SEL)_tg68k.txt; rc=$$?; cat out_latch_$(SEL)_tg68k.txt; exit $$rc

NOREL_DIR   := $(PKG_DIR)/obj_dir_norel/minimig-aga
NOREL_FILES := $(patsubst $(MINIMIG_DIR)/%,$(NOREL_DIR)/%,$(MINIMIG_FILES))

.PHONY: norel-rtl norel-test norel-test-fx68k norel-test-tg68k
norel-rtl:
	rm -rf obj_dir_norel
	mkdir -p obj_dir_norel
	cp -r $(MINIMIG_DIR) obj_dir_norel/minimig-aga
	cd obj_dir_norel/minimig-aga && patch -p1 < $(PKG_DIR)/latch.patch && patch -p1 < $(PKG_DIR)/norel.patch

norel-test-fx68k: norel-rtl $(BINS)
	rm -rf obj_dir_norel_fx68k out_norel_fx68k.txt
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_norel_fx68k -cc --exe --build -j 0 \
		$(NOREL_FILES) $(FX68K_FILES) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_norel_fx68k/
	( cd obj_dir_norel_fx68k && NMH_MUTATE=P3-RELEASE50 ./Vnanomig $(PKG_DIR) ) > out_norel_fx68k.txt; rc=$$?; cat out_norel_fx68k.txt; exit $$rc

norel-test-tg68k: norel-rtl $(BINS)
	rm -rf obj_dir_norel_tg68k out_norel_tg68k.txt
	verilator $(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR \
		-I$(NANOMIG)/sim \
		--Mdir obj_dir_norel_tg68k -cc --exe --build -j 0 \
		$(call tg68k_files,$(NOREL_FILES)) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_norel_tg68k && NMH_MUTATE=P3-RELEASE50 ./Vnanomig $(PKG_DIR) ) > out_norel_tg68k.txt; rc=$$?; cat out_norel_tg68k.txt; exit $$rc

norel-test: norel-test-fx68k norel-test-tg68k
