# Integration package aa-integ-s5: see SPECIFICATION.md.

TEST_CPP := test_aa_integ_s5.cpp
PROGS    := prog_run prog_run_nofix

include ../harness/nanomig.mk

# Implementation records: the same package against a private copy of the NanoMig
# chip-set RTL, made by applying a patch file of this package to a copy of the
# pinned submodule's minimig-aga directory. The submodule is never modified; each
# copy is made under its own directory (obj_dir_<patch>_<core>/) and the core builds
# in it (obj_dir_<patch>_<core>/v/), so the two cores of a patch can be built at the
# same time and obj_dir_<core>/ and out_<core>.txt stay those of the unmodified build.
#   make <patch>-test-fx68k ONLY=<propositions> SEL=<selector>
#   make <patch>-test-tg68k ONLY=<propositions> SEL=<selector>
# builds the core on the copy and runs it with NMH_ONLY and NMH_MUTATE set from ONLY
# and SEL (either may be left out); the output is out_<patch>_<core>.txt.
#
#   p6vol:   p6vol.patch -- the volume path takes a seventh bit: AUDxVOL bit 6 gives
#       the volume 64 in place of 63 (paula_audio_mixer.v, paula_audio_volume.v).
#   p16gap:  p16gap.patch -- after the third frame the transmitter waits one
#       complete bit cell, the line idle, before it moves a waiting word into the
#       shift register (paula_uart.v).
#   p17end:  p17end.patch -- the TBE request is set at the end of the frame, not at
#       the move of the buffer into the shift register (paula_uart.v).

define IMPL_RULES
$(1)_DIR_$(2)   := $$(PKG_DIR)/obj_dir_$(1)_$(2)/minimig-aga
$(1)_FILES_$(2) := $$(patsubst $$(MINIMIG_DIR)/%,$$($(1)_DIR_$(2))/%,$$(MINIMIG_FILES))

.PHONY: $(1)-test-$(2)
$(1)-test-$(2): $$(BINS)
	rm -rf obj_dir_$(1)_$(2) out_$(1)_$(2).txt
	mkdir -p obj_dir_$(1)_$(2)
	cp -r $$(MINIMIG_DIR) obj_dir_$(1)_$(2)/minimig-aga
	cd obj_dir_$(1)_$(2)/minimig-aga && patch -p1 < $$(PKG_DIR)/$(1).patch
	$$(call $(2)_build,$(1))
	( cd obj_dir_$(1)_$(2)/v && $$(if $$(ONLY),NMH_ONLY=$$(ONLY)) $$(if $$(SEL),NMH_MUTATE=$$(SEL)) ./Vnanomig $$(PKG_DIR) ) > out_$(1)_$(2).txt; rc=$$$$?; cat out_$(1)_$(2).txt; exit $$$$rc
endef

define fx68k_build
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert \
		--Mdir obj_dir_$(1)_fx68k/v -cc --exe --build -j 0 \
		$($(1)_FILES_fx68k) $(FX68K_FILES) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_$(1)_fx68k/v/
endef

define tg68k_build
	verilator $(COMMON_VFLAGS) \
		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR \
		-I$(NANOMIG)/sim \
		--Mdir obj_dir_$(1)_tg68k/v -cc --exe --build -j 0 \
		$(call tg68k_files,$($(1)_FILES_tg68k)) $(TG68K_FILE) $(TEST_CPP) \
		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
endef

$(foreach p,p6vol p16gap p17end,$(foreach c,fx68k tg68k,$(eval $(call IMPL_RULES,$(p),$(c)))))
