# ch8-light-pen-beam: see SPECIFICATION.md.

TEST_CPP := test_ch8_light_pen_beam.cpp
PROGS    := prog prog_ecs_pal prog_ecs_ntsc prog_k21 prog_k1 prog_k3 prog_k4 prog_k5 prog_k13 prog_k14 prog_fixture prog_nodone

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
# A patched run prints every line of the package. The directory of a copy (obj_dir_<copy>/) is shared by its two cores and is
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
		$(call tg68k_files,$(LATCH_FILES)) $(TG68K_FILE) $(TEST_CPP) \
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
		$(call tg68k_files,$(LATE_FILES)) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_late_tg68k && ./Vnanomig $(PKG_DIR) ) > out_late_tg68k.txt; rc=$$?; cat out_late_tg68k.txt; exit $$rc

late-test: late-test-fx68k late-test-tg68k

# Implementation contrasts, each a disposable copy of the chip-set RTL under its
# own directory, never the submodule. Their runs select one contrasting run
# (NMH_MUTATE) of the package, in OCS/PAL on the main schedule.
#   refresh3: refresh3.patch applied to the unpatched copy: the fourth refresh
#       slot of every line is removed (CP6 / A3 reads three accesses, K22).
#   k9: latch.patch, then k9.patch: pin 1 (up) of either port joins the
#       trigger input of the latch (P2 (i), K9).
# The stimulus contrasts that need a latch (K1-K4, K6-K8, K9b, K16-K20) run the
# binary of the latch build with NMH_MUTATE set:
#   make latch-mut-fx68k SEL=K1     make latch-mut-tg68k SEL=K1
.PHONY: latch-mut-fx68k latch-mut-tg68k
latch-mut-fx68k:
	( cd obj_dir_latch_fx68k && NMH_MUTATE=$(SEL) ./Vnanomig $(PKG_DIR) ) > out_latch_$(SEL)_fx68k.txt; rc=$$?; cat out_latch_$(SEL)_fx68k.txt; exit $$rc
latch-mut-tg68k:
	( cd obj_dir_latch_tg68k && NMH_MUTATE=$(SEL) ./Vnanomig $(PKG_DIR) ) > out_latch_$(SEL)_tg68k.txt; rc=$$?; cat out_latch_$(SEL)_tg68k.txt; exit $$rc

REFRESH3_DIR   := $(PKG_DIR)/obj_dir_refresh3/minimig-aga
REFRESH3_FILES := $(patsubst $(MINIMIG_DIR)/%,$(REFRESH3_DIR)/%,$(MINIMIG_FILES))

.PHONY: refresh3-rtl refresh3-test refresh3-test-fx68k refresh3-test-tg68k
refresh3-rtl:
	rm -rf obj_dir_refresh3
	mkdir -p obj_dir_refresh3
	cp -r $(MINIMIG_DIR) obj_dir_refresh3/minimig-aga
	cd obj_dir_refresh3/minimig-aga && patch -p1 < $(PKG_DIR)/refresh3.patch

refresh3-test-fx68k: refresh3-rtl $(BINS)
	rm -rf obj_dir_refresh3_fx68k out_refresh3_fx68k.txt
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_refresh3_fx68k -cc --exe --build -j 0 \
		$(REFRESH3_FILES) $(FX68K_FILES) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_refresh3_fx68k/
	( cd obj_dir_refresh3_fx68k && NMH_MUTATE=K22 ./Vnanomig $(PKG_DIR) ) > out_refresh3_fx68k.txt; rc=$$?; cat out_refresh3_fx68k.txt; exit $$rc

refresh3-test-tg68k: refresh3-rtl $(BINS)
	rm -rf obj_dir_refresh3_tg68k out_refresh3_tg68k.txt
	verilator $(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR \
		-I$(NANOMIG)/sim \
		--Mdir obj_dir_refresh3_tg68k -cc --exe --build -j 0 \
		$(call tg68k_files,$(REFRESH3_FILES)) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_refresh3_tg68k && NMH_MUTATE=K22 ./Vnanomig $(PKG_DIR) ) > out_refresh3_tg68k.txt; rc=$$?; cat out_refresh3_tg68k.txt; exit $$rc

refresh3-test: refresh3-test-fx68k refresh3-test-tg68k

K9_DIR   := $(PKG_DIR)/obj_dir_k9/minimig-aga
K9_FILES := $(patsubst $(MINIMIG_DIR)/%,$(K9_DIR)/%,$(MINIMIG_FILES))

.PHONY: k9-rtl k9-test k9-test-fx68k k9-test-tg68k
k9-rtl:
	rm -rf obj_dir_k9
	mkdir -p obj_dir_k9
	cp -r $(MINIMIG_DIR) obj_dir_k9/minimig-aga
	cd obj_dir_k9/minimig-aga && patch -p1 < $(PKG_DIR)/latch.patch && patch -p1 < $(PKG_DIR)/k9.patch

k9-test-fx68k: k9-rtl $(BINS)
	rm -rf obj_dir_k9_fx68k out_k9_fx68k.txt
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_k9_fx68k -cc --exe --build -j 0 \
		$(K9_FILES) $(FX68K_FILES) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_k9_fx68k/
	( cd obj_dir_k9_fx68k && NMH_MUTATE=K9 ./Vnanomig $(PKG_DIR) ) > out_k9_fx68k.txt; rc=$$?; cat out_k9_fx68k.txt; exit $$rc

k9-test-tg68k: k9-rtl $(BINS)
	rm -rf obj_dir_k9_tg68k out_k9_tg68k.txt
	verilator $(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR \
		-I$(NANOMIG)/sim \
		--Mdir obj_dir_k9_tg68k -cc --exe --build -j 0 \
		$(call tg68k_files,$(K9_FILES)) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_k9_tg68k && NMH_MUTATE=K9 ./Vnanomig $(PKG_DIR) ) > out_k9_tg68k.txt; rc=$$?; cat out_k9_tg68k.txt; exit $$rc

k9-test: k9-test-fx68k k9-test-tg68k

#   p13fix: latch.patch, then p13fix.patch: port 2's pin 5 (joystick0 bit 6) is
#       wired to the right port's X pot line, which the unpatched RTL leaves
#       without an external input. The main run is made on it (the green control
#       of P13 (port 2)); NMH_MUTATE=K23 holds that pin low throughout phase D.

P13FIX_DIR   := $(PKG_DIR)/obj_dir_p13fix/minimig-aga
P13FIX_FILES := $(patsubst $(MINIMIG_DIR)/%,$(P13FIX_DIR)/%,$(MINIMIG_FILES))

.PHONY: p13fix-rtl p13fix-test p13fix-test-fx68k p13fix-test-tg68k
p13fix-rtl:
	rm -rf obj_dir_p13fix
	mkdir -p obj_dir_p13fix
	cp -r $(MINIMIG_DIR) obj_dir_p13fix/minimig-aga
	cd obj_dir_p13fix/minimig-aga && patch -p1 < $(PKG_DIR)/latch.patch && patch -p1 < $(PKG_DIR)/p13fix.patch

p13fix-test-fx68k: p13fix-rtl $(BINS)
	rm -rf obj_dir_p13fix_fx68k out_p13fix_fx68k.txt
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_p13fix_fx68k -cc --exe --build -j 0 \
		$(P13FIX_FILES) $(FX68K_FILES) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_p13fix_fx68k/
	( cd obj_dir_p13fix_fx68k && ./Vnanomig $(PKG_DIR) ) > out_p13fix_fx68k.txt; rc=$$?; cat out_p13fix_fx68k.txt; exit $$rc

p13fix-test-tg68k: p13fix-rtl $(BINS)
	rm -rf obj_dir_p13fix_tg68k out_p13fix_tg68k.txt
	verilator $(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR \
		-I$(NANOMIG)/sim \
		--Mdir obj_dir_p13fix_tg68k -cc --exe --build -j 0 \
		$(call tg68k_files,$(P13FIX_FILES)) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_p13fix_tg68k && ./Vnanomig $(PKG_DIR) ) > out_p13fix_tg68k.txt; rc=$$?; cat out_p13fix_tg68k.txt; exit $$rc

p13fix-test: p13fix-test-fx68k p13fix-test-tg68k
