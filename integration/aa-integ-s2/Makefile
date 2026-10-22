# Integration package aa-integ-s2: see SPECIFICATION.md.

TEST_CPP := test_aa_integ_s2.cpp
PROGS    := $(basename $(wildcard prog_*.s))

include ../harness/nanomig.mk

# Private builds (SPECIFICATION.md 6.6): a copy of the chip set with one patch from private/,
# built and run on both cores in obj_dir_private_<name>/. The outputs are out_private_<name>_<core>.txt.
#   make private-uhres      the page-built chip set
#   make private-longline   the same with every line long (P14-0-LONGONLY)
#   make private-novbrelease   the same without the release of the light-pen latch at the end of the vertical blank (P8-VBREL)
#   make private-laterelease   the same with the latch released at line 45 in place of vertical position 20 (P8-VBREL)
.PHONY: private-uhres private-longline private-novbrelease private-laterelease private-build
private-uhres:
	$(MAKE) private-build PATCH=uhres
private-longline:
	$(MAKE) private-build PATCH=longline
private-novbrelease:
	$(MAKE) private-build PATCH=novbrelease
private-laterelease:
	$(MAKE) private-build PATCH=laterelease
private-build: private/$(PATCH).patch
	rm -rf obj_dir_private_$(PATCH)
	mkdir -p obj_dir_private_$(PATCH)/nanomig obj_dir_private_$(PATCH)/integration/aa-integ-s2
	cp -r $(NANOMIG)/src $(NANOMIG)/sim obj_dir_private_$(PATCH)/nanomig/
	cp -r ../harness obj_dir_private_$(PATCH)/integration/
	cp Makefile *.s *.i $(TEST_CPP) obj_dir_private_$(PATCH)/integration/aa-integ-s2/
	patch -s -p1 -d obj_dir_private_$(PATCH)/nanomig < private/$(PATCH).patch
	$(MAKE) -C obj_dir_private_$(PATCH)/integration/aa-integ-s2 -k -B test NANOMIG=$(abspath obj_dir_private_$(PATCH)/nanomig); \
	rc=$$?; \
	cp obj_dir_private_$(PATCH)/integration/aa-integ-s2/out_fx68k.txt out_private_$(PATCH)_fx68k.txt; \
	cp obj_dir_private_$(PATCH)/integration/aa-integ-s2/out_tg68k.txt out_private_$(PATCH)_tg68k.txt; \
	exit $$rc
