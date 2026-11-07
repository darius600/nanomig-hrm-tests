# ch2-interlace-fields: see SPECIFICATION.md.

TEST_CPP := test_ch2_interlace_fields.cpp
PROGS    := prog_i prog_i_a prog_i_b prog_i_c prog_i_d prog_i_e prog_i_f \
            prog_i_g prog_i_h prog_i_i prog_i_j prog_i_k prog_i_l prog_i_m prog_i_n prog_i_o prog_i_p prog_i_q prog_i_r prog_i_s prog_i_t prog_i_u prog_i_v prog_i_w prog_i_x \
            prog_t prog_t_a prog_t_c prog_t_d prog_t_e prog_t_f prog_t_g prog_t_h prog_t_i prog_t_j prog_t_k prog_t_l prog_t_m prog_t_n \
            prog_t_o prog_t_p prog_t_q prog_t_r prog_t_s prog_t_t

include ../harness/nanomig.mk

# Implementation demonstration: the same package against a private copy of the NanoMig chip-set RTL, made by
# applying longline.patch of this package to a copy of the pinned submodule's minimig-aga directory. The patch is
# a test construction, not a proposed fix: without VARBEAMEN, an NTSC line that long_line marks ends at $E3
# (228 colour clocks, p23) instead of $E2 (227), so the lines alternate 228 and 227. The submodule is never
# modified; the copy is made under its own directory (obj_dir_longline/), and each core builds in its own object
# directory (obj_dir_longline_<core>/) and writes its own output file (out_longline_<core>.txt), so obj_dir_<core>/
# and out_<core>.txt stay those of the unmodified build, which `make test` uses.
#   make longline-test-fx68k     builds and runs the fx68k build on the copy
#   make longline-test-tg68k     builds and runs the TG68K build on the copy
# A patched run prints every line of the package. The directory of the copy (obj_dir_longline/) is shared by its two
# cores and is removed and made again by longline-rtl: build the two cores one after the other (longline-test does),
# never in two shells at once.

LONGLINE_DIR   := $(PKG_DIR)/obj_dir_longline/minimig-aga
LONGLINE_FILES := $(patsubst $(MINIMIG_DIR)/%,$(LONGLINE_DIR)/%,$(MINIMIG_FILES))

.PHONY: longline-rtl longline-test longline-test-fx68k longline-test-tg68k
longline-rtl:
	rm -rf obj_dir_longline
	mkdir -p obj_dir_longline
	cp -r $(MINIMIG_DIR) obj_dir_longline/minimig-aga
	cd obj_dir_longline/minimig-aga && patch -p1 < $(PKG_DIR)/longline.patch

longline-test-fx68k: longline-rtl $(BINS)
	rm -rf obj_dir_longline_fx68k out_longline_fx68k.txt
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_longline_fx68k -cc --exe --build -j 0 \
		$(LONGLINE_FILES) $(FX68K_FILES) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_longline_fx68k/
	( cd obj_dir_longline_fx68k && ./Vnanomig $(PKG_DIR) ) > out_longline_fx68k.txt; rc=$$?; cat out_longline_fx68k.txt; exit $$rc

longline-test-tg68k: longline-rtl $(BINS)
	rm -rf obj_dir_longline_tg68k out_longline_tg68k.txt
	verilator $(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR \
		-I$(NANOMIG)/sim \
		--Mdir obj_dir_longline_tg68k -cc --exe --build -j 0 \
		$(call tg68k_files,$(LONGLINE_FILES)) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_longline_tg68k && ./Vnanomig $(PKG_DIR) ) > out_longline_tg68k.txt; rc=$$?; cat out_longline_tg68k.txt; exit $$rc

longline-test: longline-test-fx68k longline-test-tg68k
