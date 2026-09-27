; prog_diskrate_f1mut_ntsc.s -- prog_diskrate.s with MUT_F1 and MUT_F1_NTSC
; set: F1's and B4's own contrasting case under ECS/NTSC (SPECIFICATION.md
; 2.4: BEAMCON0 = $0080 in NTSC, vs $00A0 in PAL).
MUT_F1          equ     1
MUT_F1_NTSC     equ     1
        include "prog_diskrate.s"
