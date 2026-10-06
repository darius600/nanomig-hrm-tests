; regs.i -- register-list stimulus (SPECIFICATION.md section 4, "Programs").
; The test preloads a list of (register offset, value) word pairs into chip
; RAM at RLIST, terminated by an offset of $FFFF, before boot(). The CPU reads
; each pair and writes the value to the custom register with one MOVE.W, in
; list order: every register is written by a genuine CPU bus cycle, exactly
; as a MOVE.W #imm would. The list's order carries the S1 steps: clear the
; DMA and interrupt enables first, the proposition's registers next, the
; Copper location register and its strobe before DMA is enabled, DMACON last.
;
; A program includes this file and then supplies what runs after the list.
; NOFIX, defined before the include, leaves out the fixture store.

        include "nanomig.i"

RLIST           equ     $0B8000
LISAID          equ     $07C

        org     PROGRAM_ORG

start:
        ifnd    NOFIX
        move.w  #$BEEF,RESULTS          ; the fixture: a known constant, stored first
        endc
        lea     CUSTOM,a0
        lea     RLIST,a1
.next:  move.w  (a1)+,d0
        cmp.w   #$FFFF,d0
        beq.s   .listend
        move.w  (a1)+,d1
        move.w  d1,0(a0,d0.w)
        bra.s   .next
.listend:
        move.w  LISAID(a0),RESULTS+4    ; the chip set's identity, read once the list is done
