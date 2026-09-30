; prog_lpen_core.s -- P3 (BPLCON0 bit 3, LPEN) and P4 (LPEN after power-up).
; Included by one-line shim sources, each setting a subset of these flags:
;
;   ECS_MODE           write BEAMCON0 (ECS configurations only, per spec)
;   NTSC_MODE          (with ECS_MODE) BEAMCON0 carries the NTSC value;
;                       otherwise the PAL value
;   LPEN1              BPLCON0 = $0008 (LPEN=1); otherwise $0000, unless
;                       OMIT_BPLCON0 is set
;   OMIT_BPLCON0       skip the BPLCON0 write entirely (P4's base program:
;                       LPEN is left at its power-up value)
;   MUT_EXTRA_WRITE    (with LPEN1) P3-d contrast: one extra BPLCON0=$0000
;                       write after the first VERTB detection
;   MUT_LPENDIS        (with LPEN1, ECS_MODE) P3-c contrast: BEAMCON0 bit 13
;                       (LPENDIS) is set together with bit 5
;   MUT_WRONGSTD       (with ECS_MODE) P3-c/P4-c contrast: BEAMCON0 bit 5 is
;                       the opposite of the configured standard
;   MUT_REDUCE_BURST   P3-a/P4-a contrast: 20 reads per window instead of 600
;   MUT_COPPER_BPLCON0 (with OMIT_BPLCON0) P4-b contrast, Copper part: before
;                       the main loop, point COP1LC at COP_LIST_ADDR (already
;                       holding a MOVE BPLCON0,$0008 / WAIT $FFFF,$FFFE list,
;                       preloaded by the test) and start the Copper
;
; Five times: poll INTREQR bit 5 (VERTB), clear it, store the iteration
; index as a marker, then read VPOSR/VHPOSR (as one long word, p.216) a
; fixed number of times in a row, storing every value. See
; SPECIFICATION.md P3/P4 for the window each burst is graded over.

        include "nanomig.i"
        include "playfield.i"

        ifd     MUT_REDUCE_BURST
BURSTLEN        equ     20
        else
BURSTLEN        equ     600
        endc

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,INTREQ(a0)

        ifd     MUT_COPPER_BPLCON0
        move.l  #COP_LIST_ADDR,d0
        move.w  d0,COP1LCL(a0)
        swap    d0
        move.w  d0,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)         ; strobe: load Copper PC from COP1LC
        move.w  #$8280,DMACON(a0)      ; SET/CLR, COPEN, DMAEN (no BPLEN)
        endc

        ifd     ECS_MODE
        ifd     MUT_WRONGSTD
        ifd     NTSC_MODE
        move.w  #$0020,BEAMCON0(a0)    ; wrong: PAL bit set, configured NTSC
        else
        move.w  #$0000,BEAMCON0(a0)    ; wrong: PAL bit clear, configured PAL
        endc
        else
        ifd     MUT_LPENDIS
        ifd     NTSC_MODE
        move.w  #$2000,BEAMCON0(a0)    ; LPENDIS=1, bit5=0 (NTSC)
        else
        move.w  #$2020,BEAMCON0(a0)    ; LPENDIS=1, bit5=1 (PAL)
        endc
        else
        ifd     NTSC_MODE
        move.w  #$0000,BEAMCON0(a0)
        else
        move.w  #$0020,BEAMCON0(a0)
        endc
        endc
        endc
        endc

        ifnd    OMIT_BPLCON0
        ifd     LPEN1
        move.w  #$0008,BPLCON0(a0)
        else
        move.w  #$0000,BPLCON0(a0)
        endc
        endc

        lea     MEASURE_BASE,a1
        moveq   #0,d6                  ; iteration index, 0..4 ("detection" 1..5)
.iter:
.poll:  move.w  INTREQR(a0),d0   ; BTST on a memory operand tests a BYTE (the
        btst    #5,d0           ; high byte at an even address); load the
        beq.s   .poll           ; word into a register first to test bit 5 of it.
        move.w  #$0020,INTREQ(a0)      ; clear VERTB (bit 5), SET/CLR=0

        ifd     MUT_EXTRA_WRITE
        tst.w   d6
        bne.s   .noextra
        move.w  #$0000,BPLCON0(a0)     ; P3-d contrast: an extra write to $DFF100
.noextra:
        endc

        move.w  d6,(a1)+               ; marker: the iteration index
        move.w  #BURSTLEN-1,d5
.burst: move.l  VPOSR(a0),(a1)+        ; long read of VPOSR,VHPOSR (p.216)
        dbra    d5,.burst

        addq.w  #1,d6
        cmp.w   #5,d6
        bne.s   .iter

        DONE
