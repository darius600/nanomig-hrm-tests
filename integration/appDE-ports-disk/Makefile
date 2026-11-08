# appDE-ports-disk: the pen-pressed button (P5) and the light pen (P1-P4).
# See SPECIFICATION.md. Section 0.1's external-disk-identification claims
# carry no proposition and no program.

TEST_CPP := test_appDE_ports_disk.cpp
PROGS    := prog_pot prog_pot_nodone prog_pot_fix prog_lpen prog_lpen_ocs prog_lpen_nodone prog_lpen_pause prog_lpen_fix prog_lpen_v8 prog_lpen_vb prog_lpen_m8_fx prog_lpen_m8_tg prog_lpen_m4_fx prog_lpen_m4_tg prog_lpen_m10_fx prog_lpen_m10_tg

include ../harness/nanomig.mk

# The variants include their main program and the register file.
prog_pot_nodone.bin prog_pot_fix.bin: prog_pot.s appDE_regs.i
prog_lpen.bin prog_lpen_ocs.bin prog_lpen_nodone.bin prog_lpen_pause.bin prog_lpen_fix.bin prog_lpen_v8.bin prog_lpen_vb.bin prog_lpen_m8_fx.bin prog_lpen_m8_tg.bin prog_lpen_m4_fx.bin prog_lpen_m4_tg.bin prog_lpen_m10_fx.bin prog_lpen_m10_tg.bin: prog_lpen.s appDE_regs.i

# Implementation controls: each is a disposable copy of the chip-set RTL, made
# by applying patch files of this package to a copy of the pinned submodule's
# minimig-aga directory. The submodule is never modified. A copy is made under
# its own directory (obj_dir_<copy>/), each core builds in its own object
# directory (obj_dir_<copy>_<core>/) and writes its own output file
# (out_<copy>[_<SEL>]_<core>.txt), so obj_dir_<core>/ and out_<core>.txt stay
# those of the unmodified build, which `make test` uses.
#
#   latch   latch.patch -- a light-pen latch that follows pp.238-239 (the latch
#       of SPECIFICATION.md 4.6; the same file as ch8-light-pen-beam's
#       latch.patch): BPLCON0 bit 3 enables it; the counter entering line 20
#       releases any freeze and arms the capture; a falling edge of pin 6 of
#       either port while armed captures VPOSR and VHPOSR; a field end with
#       nothing latched latches the counters; reads return the latched words.
#   latch2  latch.patch, latch2.patch -- a pulse is captured whatever BPLCON0
#       bit 3 holds; the end-of-field latch is still gated by bit 3.
#   latch3  latch.patch, latch3.patch -- a freeze caused by a pulse is released
#       when the counter enters line 40; a freeze caused by the end of a field
#       is still released at line 20.
#   p5a     p5a.patch -- POTGOR bit 8 (DATLX) reads 1 whatever pin 5 does.
#   p5b     p5b.patch -- POTGOR bit 10 (DATLY) also follows the pin-5 button
#       (_mthird).
#
#   make <copy>-test-fx68k SEL=<value>   builds the copy for fx68k and runs the
#   make <copy>-test-tg68k SEL=<value>   package with NMH_MUTATE=<value> (no
#                                        SEL: the normal run)
#   make <copy>-mut-fx68k  SEL=<value>   runs the binary of an earlier build
#   make <copy>-mut-tg68k  SEL=<value>   with NMH_MUTATE=<value>
#   make <copy>-test                     both cores, normal run
# A copy's directory (obj_dir_<copy>/) is shared by its two cores and is removed
# and made again by <copy>-rtl: build the two cores of a copy one after the
# other (<copy>-test does), never in two shells at once.

SEL ?=
SELSFX := $(if $(SEL),_$(SEL),)

define PRIVATE_COPY
$(1)_DIR   := $$(PKG_DIR)/obj_dir_$(1)/minimig-aga
$(1)_FILES := $$(patsubst $$(MINIMIG_DIR)/%,$$($(1)_DIR)/%,$$(MINIMIG_FILES))

.PHONY: $(1)-rtl $(1)-test $(1)-test-fx68k $(1)-test-tg68k $(1)-mut-fx68k $(1)-mut-tg68k
$(1)-rtl:
	rm -rf obj_dir_$(1)
	mkdir -p obj_dir_$(1)
	cp -r $$(MINIMIG_DIR) obj_dir_$(1)/minimig-aga
	cd obj_dir_$(1)/minimig-aga && for p in $(2); do patch -p1 < $$(PKG_DIR)/$$$$p || exit 1; done

$(1)-test-fx68k: $(1)-rtl $$(BINS)
	rm -rf obj_dir_$(1)_fx68k out_$(1)$$(SELSFX)_fx68k.txt
	verilator $$(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_$(1)_fx68k -cc --exe --build -j 0 \
		$$($(1)_FILES) $$(FX68K_FILES) $$(TEST_CPP) \
		-CFLAGS "$$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $$(FX68K_DIR)/microrom.mem $$(FX68K_DIR)/nanorom.mem obj_dir_$(1)_fx68k/
	( cd obj_dir_$(1)_fx68k && NMH_MUTATE=$$(SEL) ./Vnanomig $$(PKG_DIR) ) > out_$(1)$$(SELSFX)_fx68k.txt; rc=$$$$?; cat out_$(1)$$(SELSFX)_fx68k.txt; exit $$$$rc

$(1)-test-tg68k: $(1)-rtl $$(BINS)
	rm -rf obj_dir_$(1)_tg68k out_$(1)$$(SELSFX)_tg68k.txt
	verilator $$(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR \
		-I$$(NANOMIG)/sim \
		--Mdir obj_dir_$(1)_tg68k -cc --exe --build -j 0 \
		$$(call tg68k_files,$$($(1)_FILES)) $$(TG68K_FILE) $$(TEST_CPP) \
		-CFLAGS "$$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_$(1)_tg68k && NMH_MUTATE=$$(SEL) ./Vnanomig $$(PKG_DIR) ) > out_$(1)$$(SELSFX)_tg68k.txt; rc=$$$$?; cat out_$(1)$$(SELSFX)_tg68k.txt; exit $$$$rc

$(1)-mut-fx68k:
	( cd obj_dir_$(1)_fx68k && NMH_MUTATE=$$(SEL) ./Vnanomig $$(PKG_DIR) ) > out_$(1)$$(SELSFX)_fx68k.txt; rc=$$$$?; cat out_$(1)$$(SELSFX)_fx68k.txt; exit $$$$rc

$(1)-mut-tg68k:
	( cd obj_dir_$(1)_tg68k && NMH_MUTATE=$$(SEL) ./Vnanomig $$(PKG_DIR) ) > out_$(1)$$(SELSFX)_tg68k.txt; rc=$$$$?; cat out_$(1)$$(SELSFX)_tg68k.txt; exit $$$$rc

$(1)-test: $(1)-test-fx68k $(1)-test-tg68k
endef

$(eval $(call PRIVATE_COPY,latch,latch.patch))
$(eval $(call PRIVATE_COPY,latch2,latch.patch latch2.patch))
$(eval $(call PRIVATE_COPY,latch3,latch.patch latch3.patch))
$(eval $(call PRIVATE_COPY,p5a,p5a.patch))
$(eval $(call PRIVATE_COPY,p5b,p5b.patch))
