# tg68k.mk -- which TG68K every TG68K build in this repository compiles.
#
# Included by integration/harness/nanomig.mk, by integration/apparatus-cpu-to-video/Makefile and by the
# hrm/appK-autoconfig-* packages that build cpu_wrapper.v against the TG68K. The includer sets NANOMIG; it may set
# MINIMIG_DIR (default $(NANOMIG)/src/minimig-aga).
#
#   TG68K_KERNEL=board     the default. The kernel the boards synthesize: $(NANOMIG)/src/tg68k/*.vhd, translated by
#                          GHDL's synthesis (the generics of the board instance in cpu_wrapper.v), and a copy of
#                          cpu_wrapper.v that connects the kernel's skipFetch output in the Verilator instance the way
#                          the board instance does (.skipFetch(skip_fetch_i)), so that cpu_req drops the bus request
#                          on a skipped fetch. Both are generated at build time by tg68k-board.sh into
#                          build/tg68k/<key>/ at the repository root; nothing under nanomig/ is modified.
#   TG68K_KERNEL=shipped   the files as the submodule has them: sim/TG68KdotC_Kernel.v, a Verilog netlist of an
#                          older revision of the kernel with its generics built in, and cpu_wrapper.v, whose
#                          Verilator instance leaves skipFetch unconnected.
#
# The build tool is GHDL (GHDL=ghdl by default; last verified with 6.0.0, mcode build). Only the TG68K builds
# use the generated files; the fx68k builds compile the files of the submodule as they are.
#
# What a build takes from here:
#   $(TG68K_FILE)               the kernel file to compile
#   $(call tg68k_files,<list>)  <list> of Verilog sources with its cpu_wrapper.v replaced by the TG68K one; a
#                               private copy of minimig-aga/ (obj_dir_*/minimig-aga) gets its cpu_wrapper.v replaced too
#   $(TG68K_WRAPPER)            that wrapper, for a build that names it directly
# Using any of them generates the board files if they are not there yet.

TG68K_KERNEL ?= board
TG68K_HARNESS_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
MINIMIG_DIR ?= $(NANOMIG)/src/minimig-aga

ifeq ($(TG68K_KERNEL),shipped)

TG68K_FILE    := $(NANOMIG)/sim/TG68KdotC_Kernel.v
TG68K_WRAPPER := $(MINIMIG_DIR)/cpu_wrapper.v
tg68k_files    = $(1)

else ifeq ($(TG68K_KERNEL),board)

export GHDL
GHDL          ?= ghdl
TG68K_GEN     := sh $(TG68K_HARNESS_DIR)/tg68k-board.sh
# The files are made when make first needs their directory (so not by `make clean`), once per make run and, under
# a lock, once per set of inputs however many packages build at the same time: the directory is named by a hash of
# the inputs and the script prints it, generating first if it is not there. A failure leaves the name empty.
TG68K_DIR      = $(or $(__tg68k_dir),$(eval __tg68k_dir := $(shell $(TG68K_GEN) generate $(abspath $(NANOMIG)) $(abspath $(MINIMIG_DIR))))$(if $(__tg68k_dir),,$(error tg68k-board.sh failed, see above))$(__tg68k_dir))
TG68K_FILE     = $(TG68K_DIR)/TG68KdotC_Kernel.v
TG68K_WRAPPER  = $(TG68K_DIR)/cpu_wrapper.v
tg68k_files    = $(patsubst %/cpu_wrapper.v,$(TG68K_WRAPPER),$(1))

else
$(error TG68K_KERNEL must be board or shipped, not '$(TG68K_KERNEL)')
endif
