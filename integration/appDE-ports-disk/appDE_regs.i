; appDE_regs.i -- register offsets and memory layout for this package's
; programs. Offsets are from CUSTOM ($DFF000), Appendix B (pp.289-293).
; Memory addresses below CFG_BASE are the harness's own layout, poked
; directly into chip RAM before boot() (see test_appDE_ports_disk.cpp) or
; written by the program; they carry no HRM significance themselves.

; CUSTOM, VPOSR, VHPOSR, DMACON, INTENA are already defined by nanomig.i.
JOY0DAT         equ $00A
JOY1DAT         equ $00C
POTGOR          equ $016
POTGO           equ $034
BPLCON0R        equ $100

; ---- Poked configuration words (read by the program, never written by it) --
CFG_POTGO       equ $0C0016     ; word: POTGO value (pen-pressed program)

; ---- Markers -----------------------------------------------------------
MARK0           equ $0C0020     ; fixture: a known constant, stored first
MARK_PHASE      equ $0C0022     ; written once, when the connector-identity
                                 ;   loop finishes and the sampling loop begins

; ---- Ring buffers (small; ram_log's write history carries every sample,
; so a ring of overwritten slots loses nothing the test needs). ---------
RING_S2         equ $0C0100     ; connector-identity samples: (JOY0DAT,JOY1DAT) pairs
RING_S2_BYTES   equ 32          ; 8 pairs * 4 bytes; must be a power of two

RING_POT        equ $0C1000     ; pen-pressed program: ring of (long VPOSR:VHPOSR, word POTGOR)
RING_POT_STRIDE equ 6
RING_POT_COUNT  equ 64

; ---- Light-pen program (prog_lpen.s) ---------------------------------
BEAMCON0        equ $1DC
CFG_BEAMCON0    equ $0C0010     ; word: BEAMCON0 value (ECS only)
CFG_BPLCON0_REF equ $0C0012     ; word: BPLCON0 value for the reference phase
CFG_BPLCON0     equ $0C0014     ; word: BPLCON0 value for the measurement phase
MARK_REFEND     equ $0C0024     ; written once, when the reference phase ends
SAMPLES         equ $100000     ; long (VPOSR:VHPOSR) samples, reference then measurement
