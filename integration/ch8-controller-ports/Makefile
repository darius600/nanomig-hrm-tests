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
# copy is made under obj_dir_sm/.
#   make sm-test-fx68k    runs the fx68k build against the copy
#   make sm-test-tg68k    runs the TG68K build against the copy
.PHONY: sm-rtl sm-test sm-test-fx68k sm-test-tg68k
sm-rtl:
	rm -rf obj_dir_sm
	mkdir -p obj_dir_sm
	cp -r $(NANOMIG)/src/minimig-aga obj_dir_sm/minimig-aga
	cd obj_dir_sm/minimig-aga && patch -p1 < $(PKG_DIR)/start_model.patch

sm-test-fx68k: sm-rtl
	$(MAKE) -B test-fx68k MINIMIG_DIR=$(PKG_DIR)/obj_dir_sm/minimig-aga

sm-test-tg68k: sm-rtl
	$(MAKE) -B test-tg68k MINIMIG_DIR=$(PKG_DIR)/obj_dir_sm/minimig-aga

sm-test: sm-test-fx68k sm-test-tg68k
