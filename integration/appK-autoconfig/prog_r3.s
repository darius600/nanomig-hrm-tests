; prog_r3.s -- SPECIFICATION.md section 5.2, P3 (runs R3 and R3n, and their
; contrasts). B1 and B2 are chosen per section 4 for the 2 MB Zorro II
; memory board this package's fastram_config offers (SPECIFICATION.md
; addendum): B1=$00200000, B2=B1+size=$00400000, both multiples of the
; board's size inside the standard 8 MB space.
;
; Four independent points vary, each selected by a symbol an including
; source sets before this file; prog_r3.s itself sets none of them:
;   MUT_SWAP  -- R3'/R3n': configure at B2 instead of B1 (bases exchanged)
;   MUT_R3C   -- R3c: step b writes register 48 directly (step c's write),
;                skipping the 4A write, steps c and d; IMG1 read last
;   MUT_R3U   -- R3u: skip step c (the 48 write) and step d; IMG2 read
;                last after only steps a and b
;   MUT_R3N   -- selects the R3n ending: after steps a-d, write/read at
;                the *other* base instead of reading IMG2

        include "nanomig.i"
        include "common.i"

B1       equ     $00200000
B2       equ     $00400000
B1_HI    equ     $00             ; B1 bits19-16 nybble (section 4 writes)
B1_LO    equ     $20             ; B1 bits23-16 byte
B2_HI    equ     $00             ; B2 bits19-16 nybble
B2_LO    equ     $40             ; B2 bits23-16 byte

        ifd     MUT_SWAP
CONFIG_BASE equ  B2
CONFIG_HI   equ  B2_HI
CONFIG_LO   equ  B2_LO
OTHER_BASE  equ  B1
        else
CONFIG_BASE equ  B1
CONFIG_HI   equ  B1_HI
CONFIG_LO   equ  B1_LO
OTHER_BASE  equ  B2
        endif

P0       equ     $A55A
P1V      equ     $3CC3
P2V      equ     $5AA5

IMG0     equ     MK_NEXT
IMG1     equ     MK_NEXT+64
IMG2     equ     MK_NEXT+128
R0V      equ     MK_NEXT+192
R1V      equ     MK_NEXT+194
SCRATCH  equ     MK_NEXT+196
R2V      equ     MK_NEXT+198

        org     PROGRAM_ORG

start:
        ENV_SETUP

        ; step a: read IMG0
        lea     AC_BASE,a0
        lea     IMG0,a2
        moveq   #15,d3
.imgl0: move.w  (a0)+,(a2)+
        move.w  (a0)+,(a2)+
        dbra    d3,.imgl0

        ifd     MUT_R3C
        move.b  #CONFIG_LO,AC_BASE+$48 ; R3c: register 48 written in step b's place
        bra     readimg1last
        else
        move.b  #CONFIG_HI,AC_BASE+$4A ; step b: nybble to 4A

        lea     AC_BASE,a0
        lea     IMG1,a2
        moveq   #15,d3
.imgl1: move.w  (a0)+,(a2)+
        move.w  (a0)+,(a2)+
        dbra    d3,.imgl1

        ifd     MUT_R3U
        bra     readimg2last            ; R3u: no step c, no step d
        else
        move.b  #CONFIG_LO,AC_BASE+$48  ; step c: byte to 48 (configures)

        move.w  #P0,CONFIG_BASE         ; step d: memory-board content check
        move.w  #P1V,CONFIG_BASE+2
        move.w  #$0000,SCRATCH
        move.w  CONFIG_BASE,R0V
        move.w  CONFIG_BASE+2,R1V

        ifd     MUT_R3N
        bra     r3n_last
        else
        bra     readimg2last
        endif
        endif
        endif

readimg1last:
        lea     AC_BASE,a0
        lea     IMG1,a2
        moveq   #15,d3
.imgl1z: move.w (a0)+,(a2)+
        move.w  (a0)+,(a2)+
        dbra    d3,.imgl1z
        DONE

readimg2last:
        lea     AC_BASE,a0
        lea     IMG2,a2
        moveq   #15,d3
.imgl2z: move.w (a0)+,(a2)+
        move.w  (a0)+,(a2)+
        dbra    d3,.imgl2z
        DONE

r3n_last:
        move.w  #P2V,OTHER_BASE
        move.w  #$0000,SCRATCH
        move.w  OTHER_BASE,R2V
        DONE

        H_E_HANDLER
