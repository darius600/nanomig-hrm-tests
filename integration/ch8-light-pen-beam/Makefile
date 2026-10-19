# ch8-light-pen-beam: see SPECIFICATION.md.

TEST_CPP := test_ch8_light_pen_beam.cpp
PROGS    := prog prog_k1 prog_k3 prog_k4 prog_k5 prog_k13 prog_k14 prog_fixture prog_nodone

include ../harness/nanomig.mk

# Implementation demonstrations: the same package against a private copy of
# the NanoMig chip-set RTL, made by applying a patch file of this package to
# a copy of the pinned submodule's minimig-aga directory. The submodule is
# never modified; each copy is made under its own directory (obj_dir_latch/,
# obj_dir_late/), and each core builds in its own object directory
# (obj_dir_<copy>_<core>/) and writes its own output file
# (out_<copy>_<core>.txt), so obj_dir_<core>/ and out_<core>.txt stay those of
# the unmodified build, which `make test` uses.
#
#   latch: latch.patch -- a light-pen latch that follows pp.238-239: BPLCON0
#       bit 3 enables it (reset to 0); the counter entering line 20 releases
#       any freeze and arms the capture; a falling edge of pin 6 of either
#       port while armed captures VPOSR and VHPOSR; a field end with nothing
#       latched latches the counters; reads return the latched words.
#       LPENDIS is not modelled, so an ECS configuration behaves as OCS.
#         make latch-test-fx68k     builds and runs the fx68k build on the copy
#         make latch-test-tg68k     builds and runs the TG68K build on the copy
#   late: late.patch -- latch.patch with the release and the arming one line
#       late (the counter entering line 21).
#         make late-test-fx68k
#         make late-test-tg68k
# A patched run prints every line of the package, the page-model lines
# included; the page-model lines apply the page's latch to samples the copy
# has already produced and are not part of what the copy demonstrates. The
# directory of a copy (obj_dir_<copy>/) is shared by its two cores and is
# removed and made again by <copy>-rtl: build the two cores of a copy one
# after the other (<copy>-test does), never in two shells at once.

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
		$(LATCH_FILES) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_latch_tg68k && ./Vnanomig $(PKG_DIR) ) > out_latch_tg68k.txt; rc=$$?; cat out_latch_tg68k.txt; exit $$rc

latch-test: latch-test-fx68k latch-test-tg68k

LATE_DIR   := $(PKG_DIR)/obj_dir_late/minimig-aga
LATE_FILES := $(patsubst $(MINIMIG_DIR)/%,$(LATE_DIR)/%,$(MINIMIG_FILES))

.PHONY: late-rtl late-test late-test-fx68k late-test-tg68k
late-rtl:
	rm -rf obj_dir_late
	mkdir -p obj_dir_late
	cp -r $(MINIMIG_DIR) obj_dir_late/minimig-aga
	cd obj_dir_late/minimig-aga && patch -p1 < $(PKG_DIR)/late.patch

late-test-fx68k: late-rtl $(BINS)
	rm -rf obj_dir_late_fx68k out_late_fx68k.txt
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_late_fx68k -cc --exe --build -j 0 \
		$(LATE_FILES) $(FX68K_FILES) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_late_fx68k/
	( cd obj_dir_late_fx68k && ./Vnanomig $(PKG_DIR) ) > out_late_fx68k.txt; rc=$$?; cat out_late_fx68k.txt; exit $$rc

late-test-tg68k: late-rtl $(BINS)
	rm -rf obj_dir_late_tg68k out_late_tg68k.txt
	verilator $(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR \
		-I$(NANOMIG)/sim \
		--Mdir obj_dir_late_tg68k -cc --exe --build -j 0 \
		$(LATE_FILES) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_late_tg68k && ./Vnanomig $(PKG_DIR) ) > out_late_tg68k.txt; rc=$$?; cat out_late_tg68k.txt; exit $$rc

late-test: late-test-fx68k late-test-tg68k
