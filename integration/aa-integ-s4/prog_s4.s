; prog_s4.s -- the program of every proposition of SPECIFICATION.md.
;
; The program interprets a command list that the test places in Chip memory at
; PARAM before the program starts. Each command is a sequence of words; the
; first is the opcode. Every custom register is written by the program with a
; MOVE.W to its address, and every stimulus of the specification (register
; values, Copper lists, sprite data, blits, the vertical-blank loop of 2.2) is a
; command list of this program.
;
; Register use: a0 command pointer, a1 SAMP / SCR store pointer, a2 MARK copy
; pointer, a3 custom chip base, a4 scratch pointer.
;
; Commands (opcode, operands):
;   0 END                      stop (DONE)
;   1 W reg,val                MOVE.W #val,reg(a3)
;   2 WVB                      wait for V8 = 0 and 0x04 <= V7-V0 <= 0x14
;   3 WL30                     wait for V7-V0 >= 0x30
;   4 MARK idx                 MOVE.W #idx,MARK
;   5 CPYD                     MOVE.W DMACONR,(a2)+
;   6 SAMPLE n                 n times  MOVE.W $DFF002,(a1)+
;   7 SSTORE val               MOVE.W #val,(a1)+
;   8 STORE hi,lo,val          MOVE.W #val,<long address>
;   9 COPYW hi,lo,n,w1..wn     copy n words from the list to <long address>
;  10 FILLL hi,lo,n,phi,plo    n longwords from <long address>, first value p, step $00010001
;  11 RUN20 size,n            MOVE.W #$5A01,MARK+6, BLTSIZE <- size, then n times
;                             MOVE.W d0,(a1)+ ; ADDQ.W #1,d0 ; DBRA, then MOVE.W #$5A02,MARK+8
;  12 WBLT reg,val,idx        MOVE.W #val,reg(a3) then MOVE.W #idx,MARK
;  13 WSAMP reg,val,n         MOVE.W #val,reg(a3) then n times MOVE.W $DFF002,(a1)+
;  14 ZERO hi,lo,n            n longwords of zero from <long address>
;  15 WLN n                   wait for V7-V0 >= n

                include "nanomig.i"

SAMP            equ     $0C2000
SCR             equ     $0A0000
MARK            equ     $0C0000
PARAM           equ     $0F0000
VPOSR_          equ     $004
VHPOSR_         equ     $006
INTENA_         equ     $09A
INTREQ_         equ     $09C
DMACON_         equ     $096
DMACONR_        equ     $002
BLTSIZE_        equ     $058

                org     PROGRAM_ORG
start:          move.w  #$2700,sr
                lea     CUSTOM,a3
                move.w  #$7FFF,INTENA_(a3)
                move.w  #$7FFF,INTREQ_(a3)
                move.w  #$7FFF,DMACON_(a3)
                lea     SAMP,a1
                lea     MARK+$100,a2
                lea     PARAM,a0
next:           move.w  (a0)+,d0
                add.w   d0,d0
                lea     tab(pc),a4
                move.w  0(a4,d0.w),d0
                jmp     0(a4,d0.w)

tab:            dc.w    op_end-tab,op_w-tab,op_wvb-tab,op_wl30-tab
                dc.w    op_mark-tab,op_cpyd-tab,op_sample-tab,op_sstore-tab
                dc.w    op_store-tab,op_copyw-tab,op_fill-tab,op_run20-tab
                dc.w    op_wblt-tab,op_wsamp-tab,op_zero-tab,op_wln-tab

op_end:         DONE

op_w:           move.w  (a0)+,d0
                move.w  (a0)+,d1
                move.w  d1,0(a3,d0.w)
                bra     next

op_wvb:         move.w  VPOSR_(a3),d0
                btst    #0,d0
                bne.s   op_wvb
                move.w  VHPOSR_(a3),d0
                lsr.w   #8,d0
                cmp.b   #$04,d0
                blo.s   op_wvb
                cmp.b   #$14,d0
                bhi.s   op_wvb
                bra     next

op_wl30:        move.w  VHPOSR_(a3),d0
                lsr.w   #8,d0
                cmp.b   #$30,d0
                blo.s   op_wl30
                bra     next

op_mark:        move.w  (a0)+,MARK
                bra     next

op_cpyd:        move.w  DMACONR_(a3),(a2)+
                bra     next

op_sample:      move.w  (a0)+,d1
                subq.w  #1,d1
.s:             move.w  $DFF002,(a1)+
                dbra    d1,.s
                bra     next

op_sstore:      move.w  (a0)+,(a1)+
                bra     next

op_store:       move.w  (a0)+,d1
                swap    d1
                move.w  (a0)+,d1
                movea.l d1,a4
                move.w  (a0)+,(a4)
                bra     next

op_copyw:       move.w  (a0)+,d1
                swap    d1
                move.w  (a0)+,d1
                movea.l d1,a4
                move.w  (a0)+,d1
                subq.w  #1,d1
.c:             move.w  (a0)+,(a4)+
                dbra    d1,.c
                bra     next

op_fill:        move.w  (a0)+,d1
                swap    d1
                move.w  (a0)+,d1
                movea.l d1,a4
                move.w  (a0)+,d3
                subq.w  #1,d3
                move.w  (a0)+,d2
                swap    d2
                move.w  (a0)+,d2
.f:             move.l  d2,(a4)+
                add.l   #$00010001,d2
                dbra    d3,.f
                bra     next

op_run20:       move.w  (a0)+,d3
                move.w  (a0)+,d1
                subq.w  #1,d1
                lea     SCR,a1
                moveq   #0,d0
                move.w  #$5A01,MARK+6
                move.w  d3,BLTSIZE_(a3)
.r:             move.w  d0,(a1)+
                addq.w  #1,d0
                dbra    d1,.r
                move.w  #$5A02,MARK+8
                bra     next

op_wblt:        move.w  (a0)+,d0
                move.w  (a0)+,d1
                move.w  (a0)+,d2
                move.w  d1,0(a3,d0.w)
                move.w  d2,MARK
                bra     next

op_wsamp:       move.w  (a0)+,d0
                move.w  (a0)+,d1
                move.w  (a0)+,d3
                subq.w  #1,d3
                move.w  d1,0(a3,d0.w)
.w:             move.w  $DFF002,(a1)+
                dbra    d3,.w
                bra     next

op_zero:        move.w  (a0)+,d1
                swap    d1
                move.w  (a0)+,d1
                movea.l d1,a4
                move.w  (a0)+,d3
                subq.w  #1,d3
                moveq   #0,d2
.z:             move.l  d2,(a4)+
                dbra    d3,.z
                bra     next

op_wln:         move.w  (a0)+,d3
.l:             move.w  VHPOSR_(a3),d0
                lsr.w   #8,d0
                cmp.b   d3,d0
                blo.s   .l
                bra     next
