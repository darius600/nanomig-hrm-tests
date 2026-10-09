; prog_lp.s -- P3/P4 body: the light-pen stimulus common to every variant.
; See SPECIFICATION.md, "P3 -- Stimulus (program)" and "P4 (ECS)". A wrapper
; file sets zero or more of ECS_BUILD, NTSC_BUILD, DDRAFORCE, LPENCLEAR,
; ANCHORMUT, V1DELAY, then includes this file.
;
;   ECS_BUILD   -- run under ECS: measure the field period before the write
;                  (two ticks), then write BEAMCON0 (P3 precondition 6's own
;                  fixture). Combined with NTSC_BUILD selects which value.
;   NTSC_BUILD  -- BEAMCON0 <- $0000 instead of $0020. Also reused, applied
;                  to a PAL-configured run, as the precondition-6 contrast
;                  (bit 5 opposite the selected standard).
;   DDRAFORCE   -- precondition-3 contrast: force CIAADDRA bits 6-7 to
;                  outputs instead of fixing them to inputs.
;   LPENCLEAR   -- contrasting case: BPLCON0 <- $0000 (LPEN clear) instead
;                  of $0008.
;   ANCHORMUT   -- precondition-1 (anchor check) contrast: the second A
;                  anchor's WAIT targets line 101 instead of line 100.
;   V1DELAY     -- precondition-2 (read-time check) contrast: the V1 read
;                  of every field is moved about 30 lines later.
;   E40LINE44   -- the live-read precondition's contrast: the E40 WAIT
;                  targets line 44 instead of line 40.
;   B1LINE181   -- the pen-position line's contrast: the B1/B2 WAIT pair
;                  sits at line 181 instead of line 180.
;   NOCOPEN     -- the Copper trigger-plan fixture's contrast: DMACON is
;                  written without COPEN, so the Copper never fetches.
;   NODONE      -- the driven-run fixture's contrast: the program never
;                  stores the DONE mark.
;   EVMIS       -- the record-layout precondition's contrast: the V1 record
;                  of every field carries event id 5 instead of 6.
;
; The CPU's waits after VERTB (about 2 lines before V1, about 8 more before
; V2) are counted polls of INTREQR, never polls of VPOSR or VHPOSR, the
; registers under test: a light-pen latch may hold them at any time (p. 239).
; The count is calibrated in the run: in the first Copper pass, which starts
; right after a VERTB, the same polling loop (spin) counts its iterations
; between the COPER signals of E110 and E130, which the Copper list places 20
; lines apart (WAIT lines 110 and 130, p. 22); CALIB holds that count.
;
; Record format (test_ch7_system_control.cpp's own kRecordWords): each
; record is 5 words -- field index (0=F-1,1=F0,...,8=F7), event id
; (0=E40,1=E60,2=E110,3=E130,4=E190,5=E210,6=V1,7=V2), VPOSR from the long
; read (high word), VHPOSR from the long read (low word), VHPOSR from the
; separate word read -- exactly the fields p.216/p.239's own read
; convention names (see SPECIFICATION.md, "Read value").

        include "nanomig.i"

BEAMCON0        equ     $1DC
COP1LCH         equ     $080
COP1LCL         equ     $082
COPJMP1         equ     $088
BPLCON0         equ     $100

CIAADDRA        equ     $BFE201         ; p.339

COP_LIST        equ     $0C1000         ; 84 bytes, see the WAIT/MOVE table below
PRETICK0        equ     $0C1100
PRETICK1        equ     $0C1102
MAINTICK        equ     $0C1104         ; 9 words, one per VERTB (F-1..F7)
RECORDS         equ     $0C2000         ; 72 records * 5 words
CALIB           equ     $0C1180         ; long: poll iterations over the 20 lines E110..E130

        org     PROGRAM_ORG

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,INTREQ(a0)
        move.w  #$7FFF,DMACON(a0)

        ; ---- CIA-A DDRA (p.228,p.229,p.351): pin 6 of both ports must be
        ; an input, so the harness's own drive is not fought by the CIA's
        ; output latch. DDRAFORCE is precondition 3's own contrast: force
        ; both bits to outputs instead.
        move.b  CIAADDRA.l,d0           ; d0.b = original DDRA
        ifd     DDRAFORCE
        or.b    #$C0,d0
        move.b  d0,CIAADDRA.l
        else
        move.b  d0,d1
        and.b   #$C0,d1
        beq.s   .ddra_done
        and.b   #$3F,d0
        move.b  d0,CIAADDRA.l
.ddra_done:
        endif
        moveq   #0,d2
        move.b  CIAADDRA.l,d2           ; DDRA read again after the fix-or-force branch
        move.w  d2,RESULTS.l            ; RESULTS+0: that DDRA, zero-extended

        ifd     ECS_BUILD
        ; ---- P3 precondition 6's own fixture: measure the field period
        ; from two ticks before the BEAMCON0 write, so
        ; test_ch7_system_control.cpp can compare it with the period
        ; measured from MAINTICK afterwards.
        bsr     wait_vertb
        move.w  #$AAAA,PRETICK0.l
        bsr     wait_vertb
        move.w  #$BBBB,PRETICK1.l
        ifd     NTSC_BUILD
        move.w  #$0000,BEAMCON0(a0)     ; LPENDIS=0, bit5=0 (NTSC)
        else
        move.w  #$0020,BEAMCON0(a0)     ; LPENDIS=0, bit5=1 (PAL)
        endif
        endif

        ifd     LPENCLEAR
        move.w  #$0000,BPLCON0(a0)      ; contrast: LPEN clear
        else
        move.w  #$0008,BPLCON0(a0)      ; LPEN=1, no bitplanes
        endif
        move.w  #$0000,COLOR00(a0)

        ; ---- Copper list, in beam order (p.27), see SPECIFICATION.md's
        ; own WAIT/MOVE table (P3, "Stimulus (program)", step 5).
        ifd     E40LINE44
        move.w  #$2C01,COP_LIST.l               ; contrast: WAIT line 44 instead of 40
        else
        move.w  #$2801,COP_LIST.l               ; WAIT line 40
        endif
        move.w  #$FFFE,COP_LIST+2.l
        move.w  #INTREQ,COP_LIST+4.l             ; E40
        move.w  #$8010,COP_LIST+6.l

        move.w  #$3C01,COP_LIST+8.l              ; WAIT line 60
        move.w  #$FFFE,COP_LIST+10.l
        move.w  #INTREQ,COP_LIST+12.l            ; E60
        move.w  #$8010,COP_LIST+14.l

        move.w  #$6441,COP_LIST+16.l             ; WAIT line 100, H $40
        move.w  #$FFFE,COP_LIST+18.l
        move.w  #COLOR00,COP_LIST+20.l           ; anchor A1
        move.w  #$0F00,COP_LIST+22.l

        ifd     ANCHORMUT
        move.w  #$65A1,COP_LIST+24.l             ; contrast: line 101 instead of 100
        else
        move.w  #$64A1,COP_LIST+24.l             ; WAIT line 100, H $A0
        endif
        move.w  #$FFFE,COP_LIST+26.l
        move.w  #COLOR00,COP_LIST+28.l           ; anchor A2
        move.w  #$0000,COP_LIST+30.l

        move.w  #$6E01,COP_LIST+32.l             ; WAIT line 110
        move.w  #$FFFE,COP_LIST+34.l
        move.w  #INTREQ,COP_LIST+36.l            ; E110
        move.w  #$8010,COP_LIST+38.l

        move.w  #$8201,COP_LIST+40.l             ; WAIT line 130
        move.w  #$FFFE,COP_LIST+42.l
        move.w  #INTREQ,COP_LIST+44.l            ; E130
        move.w  #$8010,COP_LIST+46.l

        ifd     B1LINE181
        move.w  #$B541,COP_LIST+48.l             ; contrast: WAIT line 181 instead of 180, H $40
        else
        move.w  #$B441,COP_LIST+48.l             ; WAIT line 180, H $40
        endif
        move.w  #$FFFE,COP_LIST+50.l
        move.w  #COLOR00,COP_LIST+52.l           ; anchor B1
        move.w  #$000F,COP_LIST+54.l

        ifd     B1LINE181
        move.w  #$B5A1,COP_LIST+56.l             ; contrast: WAIT line 181 instead of 180, H $A0
        else
        move.w  #$B4A1,COP_LIST+56.l             ; WAIT line 180, H $A0
        endif
        move.w  #$FFFE,COP_LIST+58.l
        move.w  #COLOR00,COP_LIST+60.l           ; anchor B2
        move.w  #$0000,COP_LIST+62.l

        move.w  #$BE01,COP_LIST+64.l             ; WAIT line 190
        move.w  #$FFFE,COP_LIST+66.l
        move.w  #INTREQ,COP_LIST+68.l            ; E190
        move.w  #$8010,COP_LIST+70.l

        move.w  #$D201,COP_LIST+72.l             ; WAIT line 210
        move.w  #$FFFE,COP_LIST+74.l
        move.w  #INTREQ,COP_LIST+76.l            ; E210
        move.w  #$8010,COP_LIST+78.l

        move.w  #$FFFF,COP_LIST+80.l             ; end: WAIT forever
        move.w  #$FFFE,COP_LIST+82.l

        ; The Copper starts a few lines after a VERTB (ECS builds: the one
        ; the second pre-write tick waited for, a few instructions ago; other
        ; builds: the next one), so its first pass, the one that carries no
        ; field number, runs in real time from the top of the field and the
        ; E110..E130 calibration below measures 20 lines.
        clr.l   CALIB.l
        ifnd    ECS_BUILD
        bsr     wait_vertb
        endif
        move.l  #COP_LIST,d3
        move.w  d3,COP1LCL(a0)
        swap    d3
        move.w  d3,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)          ; strobe before DMA is turned on (p.30)

        ifd     NOCOPEN
        move.w  #$8200,DMACON(a0)       ; contrast: SET DMAEN only, no COPEN
        else
        move.w  #$8280,DMACON(a0)       ; SET DMAEN,COPEN
        endif

        ; ---- Event loop. d0 = field index (0=F-1 .. 8=F7), d1 = per-field
        ; COPER counter (0..5), a4 = MAINTICK pointer, a5 = RECORDS pointer.
        lea     MAINTICK,a4
        lea     RECORDS,a5
        moveq   #-1,d0
        moveq   #0,d1
main_loop:
        move.w  INTREQR(a0),d7
        btst    #5,d7
        beq.s   .check_coper
        move.w  #$0020,INTREQ(a0)       ; clear VERTB
        addq.w  #1,d0
        moveq   #0,d1
        move.w  d0,(a4)+

        ; V1: about 2 lines after the VERTB the CPU saw (CALIB / 10 polls);
        ; V1DELAY: about 30 lines (3 * CALIB / 2 polls).
        move.l  CALIB.l,d5
        ifd     V1DELAY
        move.l  d5,d2
        lsr.l   #1,d2
        add.l   d2,d5
        else
        divu    #10,d5
        and.l   #$FFFF,d5
        endif
        moveq   #0,d6
        bsr     spin
        ifd     EVMIS
        moveq   #5,d4                   ; contrast: V1 stored with event id 5
        else
        moveq   #6,d4                   ; V1
        endif
        bsr     read_and_store

        ; V2: about 8 lines later (2 * CALIB / 5 polls).
        move.l  CALIB.l,d5
        add.l   d5,d5
        divu    #5,d5
        and.l   #$FFFF,d5
        moveq   #0,d6
        bsr     spin
        moveq   #7,d4                   ; V2
        bsr     read_and_store

        cmp.w   #8,d0
        beq     all_done
        bra     main_loop
.check_coper:
        btst    #4,d7
        beq.s   main_loop
        move.w  #$0010,INTREQ(a0)       ; clear COPER
        move.w  d1,d4
        bsr     read_and_store
        cmp.w   #2,d1                   ; E110 ...
        bne.s   .coper_done
        cmp.w   #-1,d0                  ; ... of the first pass, before any VERTB was seen
        bne.s   .coper_done
        moveq   #0,d6
        move.l  #$7FFFFFFF,d5           ; no cap: the loop ends at E130's COPER
        bsr     spin
        move.w  #$0010,INTREQ(a0)       ; clear E130's COPER
        move.l  d6,CALIB.l
        addq.w  #1,d1                   ; E130 was consumed here
        move.w  d1,d4
        bsr     read_and_store
.coper_done:
        addq.w  #1,d1
        bra     main_loop

all_done:
        ifd     NODONE
.never: bra.s   .never                  ; contrast: no DONE mark
        else
        DONE
        endif

; ---- wait_vertb: spins until INTREQR bit 5, then clears it. Used by the
; ECS_BUILD pre-write ticks and once before the Copper is started, before
; the main loop (and its own d0/d1) is initialised.
wait_vertb:
.wv:    move.w  INTREQR(a0),d7
        btst    #5,d7
        beq.s   .wv
        move.w  #$0020,INTREQ(a0)
        rts

; ---- spin: polls INTREQR; d6 counts the iterations. Ends when d6 reaches
; the cap in d5 or when COPER (bit 4) is set, whichever comes first. The
; calibration (cap never reached, ends at a COPER) and the delays (COPER
; never set, end at the cap) run this same instruction sequence.
spin:
.sp:    move.w  INTREQR(a0),d7
        addq.l  #1,d6
        cmp.l   d5,d6
        bhs.s   .sp_out
        btst    #4,d7
        beq.s   .sp
.sp_out:
        rts

; ---- read_and_store: stores one record (see the header comment) at (a5)+,
; field id in d0, event id in d4. Clobbers d2/d3/d6/d7.
read_and_store:
        move.l  VPOSR(a0),d2             ; d2 = {VPOSR,VHPOSR} from one long read
        move.w  VHPOSR(a0),d3            ; d3 = VHPOSR from a separate word read
        move.w  d2,d6                    ; d6 = VHPOSR (low word of the long read)
        swap    d2                       ; d2.w = VPOSR (from the long read)
        move.w  d0,(a5)+
        move.w  d4,(a5)+
        move.w  d2,(a5)+
        move.w  d6,(a5)+
        move.w  d3,(a5)+
        rts
