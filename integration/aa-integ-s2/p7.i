; p7.i -- the Copper part of P7 (SPECIFICATION.md P7), included by blit.i
; when HOOK is defined.
;
; hook: build the twelve Copper lists in Chip memory, write base values into
; the twelve Blitter pointer and modulo registers, write COPCON, start the
; Copper, then for k = 1..12: wait for COPER, clear it, wait until the beam
; is at line 100 or later, point COP1LC at the next list, wait for VERTB.
; The outcome of the k-th COPER wait (1 seen, 0 timed out) is stored at
; RESULTS+$40+2*(k-1). COPEN is cleared before the subroutine returns.
;
; Symbols defined before blit.i is included select the variants:
;   COPCONV  the COPCON value (0, or 2 for CDANG)
;   COLOR    the MOVE to the Blitter register is a MOVE to COLOR00
;   NOCOPER  the MOVE to INTREQ is a MOVE to the no-operation register

LISTS   equ     $0A0000                 ; list k (1..12) at LISTS + 16*(k-1)

; base values (B_*) written by the CPU and values moved by the Copper (C_*), by register
        ifnd    B_BLTAPTH
B_BLTAPTH equ     $000E
        endc
        ifnd    C_BLTAPTH
C_BLTAPTH equ     $0011
        endc
        ifnd    B_BLTAPTL
B_BLTAPTL equ     $0000
        endc
        ifnd    C_BLTAPTL
C_BLTAPTL equ     $4000
        endc
        ifnd    B_BLTAMOD
B_BLTAMOD equ     8
        endc
        ifnd    C_BLTAMOD
C_BLTAMOD equ     24
        endc
        ifnd    B_BLTBPTH
B_BLTBPTH equ     $000D
        endc
        ifnd    C_BLTBPTH
C_BLTBPTH equ     $0012
        endc
        ifnd    B_BLTBPTL
B_BLTBPTL equ     $0000
        endc
        ifnd    C_BLTBPTL
C_BLTBPTL equ     $4000
        endc
        ifnd    B_BLTBMOD
B_BLTBMOD equ     8
        endc
        ifnd    C_BLTBMOD
C_BLTBMOD equ     24
        endc
        ifnd    B_BLTCPTH
B_BLTCPTH equ     $000B
        endc
        ifnd    C_BLTCPTH
C_BLTCPTH equ     $0013
        endc
        ifnd    B_BLTCPTL
B_BLTCPTL equ     $0000
        endc
        ifnd    C_BLTCPTL
C_BLTCPTL equ     $4000
        endc
        ifnd    B_BLTCMOD
B_BLTCMOD equ     8
        endc
        ifnd    C_BLTCMOD
C_BLTCMOD equ     24
        endc
        ifnd    B_BLTDPTH
B_BLTDPTH equ     $000F
        endc
        ifnd    C_BLTDPTH
C_BLTDPTH equ     $0014
        endc
        ifnd    B_BLTDPTL
B_BLTDPTL equ     $0000
        endc
        ifnd    C_BLTDPTL
C_BLTDPTL equ     $4000
        endc
        ifnd    B_BLTDMOD
B_BLTDMOD equ     8
        endc
        ifnd    C_BLTDMOD
C_BLTDMOD equ     24
        endc

CLIST   macro                           ; one list: WAIT, MOVE INTREQ, MOVE reg, WAIT
        dc.w    $3C01,$FFFE
        ifd     NOCOPER
        dc.w    $01FE,$0000
        else
        dc.w    $009C,$8010
        endc
        ifd     COLOR
        dc.w    $0180,\2
        else
        dc.w    \1,\2
        endc
        dc.w    $FFFF,$FFFE
        endm

hook:
        lea     lists_src(pc),a1
        lea     LISTS,a2
        move.w  #95,d0
.cp:    move.w  (a1)+,(a2)+
        dbra    d0,.cp
; base values: pointers C $0B0000, B $0D0000, A $0E0000, D $0F0000; modulos 8
        move.w  #B_BLTCPTH,BLTCPTH(a0)
        move.w  #B_BLTCPTL,BLTCPTL(a0)
        move.w  #B_BLTCMOD,BLTCMOD(a0)
        move.w  #B_BLTBPTH,BLTBPTH(a0)
        move.w  #B_BLTBPTL,BLTBPTL(a0)
        move.w  #B_BLTBMOD,BLTBMOD(a0)
        move.w  #B_BLTAPTH,BLTAPTH(a0)
        move.w  #B_BLTAPTL,BLTAPTL(a0)
        move.w  #B_BLTAMOD,BLTAMOD(a0)
        move.w  #B_BLTDPTH,BLTDPTH(a0)
        move.w  #B_BLTDPTL,BLTDPTL(a0)
        move.w  #B_BLTDMOD,BLTDMOD(a0)
        move.w  #COPCONV,COPCON(a0)
; start the Copper: location register, then its strobe, then Copper DMA
        move.w  #(LISTS>>16),COP1LCH(a0)
        move.w  #(LISTS&$FFFF),COP1LCL(a0)
        move.w  d0,COPJMP1(a0)
        move.w  #$8280,DMACON(a0)       ; SET, DMAEN, COPEN
        lea     RESULTS+$40,a3
        moveq   #0,d6
.loop:  move.w  #$0010,d2
        bsr     waitirq
        move.w  d0,(a3)+
        move.w  #$0010,INTREQ(a0)       ; clear COPER
.v:     move.w  VHPOSR(a0),d1
        lsr.w   #8,d1
        cmp.w   #100,d1
        bcs.s   .v
        move.w  d6,d1
        addq.w  #1,d1
        cmp.w   #12,d1
        bne.s   .nw
        moveq   #0,d1
.nw:    lsl.w   #4,d1
        move.w  #(LISTS>>16),COP1LCH(a0)
        move.w  d1,COP1LCL(a0)          ; LISTS has a zero low word
        move.w  #$0020,INTREQ(a0)
        move.w  #$0020,d2
        bsr     waitirq                 ; VERTB
        addq.w  #1,d6
        cmp.w   #12,d6
        bne.s   .loop
        move.w  #$0080,DMACON(a0)       ; clear COPEN
        rts

lists_src:
        CLIST   BLTAPTH,C_BLTAPTH
        CLIST   BLTAPTL,C_BLTAPTL
        CLIST   BLTAMOD,C_BLTAMOD
        CLIST   BLTBPTH,C_BLTBPTH
        CLIST   BLTBPTL,C_BLTBPTL
        CLIST   BLTBMOD,C_BLTBMOD
        CLIST   BLTCPTH,C_BLTCPTH
        CLIST   BLTCPTL,C_BLTCPTL
        CLIST   BLTCMOD,C_BLTCMOD
        CLIST   BLTDPTH,C_BLTDPTH
        CLIST   BLTDPTL,C_BLTDPTL
        CLIST   BLTDMOD,C_BLTDMOD
