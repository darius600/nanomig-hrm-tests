; prog_r3.s -- SPECIFICATION.md section 5.2, P3 (runs R3 and R3n, and their
; contrasts). B1 and B2 are chosen per section 4 for the 2 MB Zorro II
; memory board this package's fastram_config offers (SPECIFICATION.md
; section 8.1): B1=$00200000, B2=B1+size=$00400000, both multiples of the
; board's size inside the standard 8 MB space.
;
; Independent points vary, each selected by a symbol an including source
; sets before this file; prog_r3.s itself sets none of them:
;   MUT_SWAP    -- R3'/R3n': configure at B2 instead of B1 (bases exchanged)
;   MUT_R3C     -- R3c: step b writes register 48 directly (step c's
;                  write), skipping the 4A write, steps c and d; IMG1 read
;                  last
;   MUT_R3U     -- R3u: skip step c (the 48 write) and step d; IMG2 read
;                  last after only steps a and b. Combined with MUT_R3N,
;                  the board is left unconfigured and the run ends with
;                  R3n's own write/readback at OTHER_BASE (or IOBASE)
;                  instead.
;   MUT_R3_NO48 -- P3-CONTENT's mutation: step c (the 48 write) is left
;                  out, step d (the content check) still runs
;   MUT_R3N     -- selects the R3n ending: after steps b-d (or after step
;                  b alone, with MUT_R3U), write/read at the *other* base
;                  instead of reading IMG2
;   MUT_IOBASE  -- with MUT_R3N, targets $00A00000 (Figure K-1, p.386:
;                  Zorro II I/O expansion space) instead of OTHER_BASE
;   MUT_SELF    -- with MUT_R3N, targets the configured base CONFIG_BASE
;                  itself instead of OTHER_BASE

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

        ifd     MUT_IOBASE
NOTHER_TARGET equ $00A00000    ; Figure K-1 (p.386): Zorro II I/O expansion space
        else
        ifd     MUT_SELF
NOTHER_TARGET equ CONFIG_BASE
        else
NOTHER_TARGET equ OTHER_BASE
        endif
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
        ifd     MUT_R3N
        bra     r3n_last                ; unconfigured control: no step c, no step d
        else
        bra     readimg2last            ; R3u: no step c, no step d
        endif
        else
        ifd     MUT_R3_NO48
        ; P3-CONTENT mutation: step c (the 48 write) left out, step d kept
        else
        move.b  #CONFIG_LO,AC_BASE+$48  ; step c: byte to 48 (configures)
        endif

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
        move.w  #P2V,NOTHER_TARGET
        move.w  #$0000,SCRATCH
        move.w  NOTHER_TARGET,R2V
        DONE

        H_E_HANDLER
