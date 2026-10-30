# ch8-controller-ports: see SPECIFICATION.md.

TEST_CPP := test_ch8_controller_ports.cpp
PROGS    := prog_fixture prog_fixture_mut \
            prog_p1 prog_p1_k1 prog_p1_k3 prog_p1_k4 prog_p1_k6 prog_p1_k7 prog_p1_outlow \
            prog_joy

include ../harness/nanomig.mk

# Page-model run: the same package against a private copy of the NanoMig
# chip-set RTL in which POTGOR reads 0 on every pot line that is an input
# for the eight lines after a POTGO write with START, as p.235 and p.240
# describe (start_model.patch). The pinned submodule is never modified; the
# copy is made under obj_dir_sm/. The two builds have their own object
# directories (obj_dir_sm_fx68k/, obj_dir_sm_tg68k/) and output files
# (out_sm_fx68k.txt, out_sm_tg68k.txt): nothing in obj_dir_fx68k/ or
# obj_dir_tg68k/, which `make test` builds against the repository's RTL, is
# touched or reused.
#   make sm-test-fx68k    builds and runs the fx68k build against the copy
#   make sm-test-tg68k    builds and runs the TG68K build against the copy
SM_DIR   := $(PKG_DIR)/obj_dir_sm/minimig-aga
SM_FILES := $(patsubst $(MINIMIG_DIR)/%,$(SM_DIR)/%,$(MINIMIG_FILES))

.PHONY: sm-rtl sm-test sm-test-fx68k sm-test-tg68k
sm-rtl:
	rm -rf obj_dir_sm
	mkdir -p obj_dir_sm
	cp -r $(NANOMIG)/src/minimig-aga obj_dir_sm/minimig-aga
	cd obj_dir_sm/minimig-aga && patch -p1 < $(PKG_DIR)/start_model.patch

sm-test-fx68k: sm-rtl $(BINS)
	rm -rf obj_dir_sm_fx68k out_sm_fx68k.txt
	verilator $(COMMON_VFLAGS) -Wno-BLKANDNBLK --no-assert 		--Mdir obj_dir_sm_fx68k -cc --exe --build -j 0 		$(SM_FILES) $(FX68K_FILES) $(TEST_CPP) 		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=fx68k"
	cp $(FX68K_DIR)/microrom.mem $(FX68K_DIR)/nanorom.mem obj_dir_sm_fx68k/
	( cd obj_dir_sm_fx68k && ./Vnanomig $(PKG_DIR) ) > out_sm_fx68k.txt; rc=$$?; cat out_sm_fx68k.txt; exit $$rc

sm-test-tg68k: sm-rtl $(BINS)
	rm -rf obj_dir_sm_tg68k out_sm_tg68k.txt
	verilator $(COMMON_VFLAGS) 		-DENABLE_TG68K -DENABLE_AGA -DCPU_SLOW14 -DENABLE_CACHE -DCHIPRAM_CACHE -DNO_WS2812 -DDENISE_EBR 		-I$(NANOMIG)/sim 		--Mdir obj_dir_sm_tg68k -cc --exe --build -j 0 		$(call tg68k_files,$(SM_FILES)) $(TG68K_FILE) $(TEST_CPP) 		-CFLAGS "$(COMMON_CFLAGS) -DNANOMIG_CORE_NAME=tg68k"
	( cd obj_dir_sm_tg68k && ./Vnanomig $(PKG_DIR) ) > out_sm_tg68k.txt; rc=$$?; cat out_sm_tg68k.txt; exit $$rc

sm-test: sm-test-fx68k sm-test-tg68k
