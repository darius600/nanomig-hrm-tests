; prog_chipid.s -- reads VPOSR ($DFF004) once and stores it in R. The test
; prints bits 14-8 (the Agnus identification of p.299) under OCS and ECS as a
; MEASURED line; nothing is graded.

        include "ch5.i"

        org     PROGRAM_ORG


start:
        move.w  #$2700,sr
        CLEAR_OVL
        lea     CUSTOM,a0
        lea     RSTORE,a3
        move.w  VPOSR(a0),(a3)+
        DONE
