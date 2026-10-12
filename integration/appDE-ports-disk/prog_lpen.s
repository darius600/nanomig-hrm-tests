; prog_lpen.s -- the light-pen sampling program (SPECIFICATION.md section
; 2.3, steps S1-S8). Every configuration value is written with a move from
; its memory word, so the Chip-memory port shows the CPU reading the value
; just before the write.
;
; NO_S3    (define before including): step S3 (the BEAMCON0 write) is left
;          out. This is the OCS build.
; NO_DONE  (define before including): step S8's done mark is skipped; the
;          program hangs instead.
; NO_FIX   (define before including): the fixture constant stored first
;          differs.
; PAUSE    (define before including): the measurement loop runs in 92 chunks
;          of 500 reads with a short delay loop between the chunks; the read
;          count stays 46000.
; VARBEAM  (define before including): after S3 the program writes HTOTAL ($E3)
;          and BEAMCON0 with VARBEAMEN set ($00A0), so the lines have 228
;          colour clocks.
; W_AT, W_LEN (define before including, with W_V8): the reference loop is
;          split after W_AT reads. W_V8 sets the V8 bit of the vertical counter
;          (VPOSW) for the next W_LEN reads and clears it again. The beam is
;          between lines 57 and 255 there: V8 is clear, and with it set the
;          counter is above the field's last line.
; M_AT, M_LEN (define before including): the same V8 set and clear inside the
;          measurement loop, after M_AT of its reads, for M_LEN reads. M_ON
;          (define before including) replaces the VPOSW word that sets V8; the
;          default is $8001.

        include "nanomig.i"
        include "appDE_regs.i"
        org     PROGRAM_ORG

V8_ON           macro                   ; V8 of the vertical counter set (VPOSW); bit 15 keeps LONG
        move.w  d3,VPOSW(a6)
        endm

V8_OFF          macro                   ; V8 of the vertical counter clear (VPOSW)
        move.w  d4,VPOSW(a6)
        endm

S2_ITERS        equ 3000        ; connector-identity loop iterations
N_REF           equ 19999       ; DBF count of the reference loop (20000 reads)
N_MEAS          equ 45999       ; DBF count of the measurement loop (46000 reads)

start:
        move.w  #$2700,sr               ; supervisor, all interrupt levels masked
        lea     CUSTOM,a6
        ifd     W_V8
        move.w  #$8001,d3               ; VPOSW word with V8 set (bit 15 = LONG stays set)
        move.w  #$8000,d4               ; VPOSW word with V8 clear
        endif
        ifd     M_AT
        ifd     M_ON
        move.w  #M_ON,d3
        else
        move.w  #$8001,d3
        endif
        move.w  #$8000,d4
        endif

        ifd     NO_FIX
        move.w  #$BEEF,MARK0            ; fixture mutation: a different constant
        else
        move.w  #$5A5A,MARK0            ; fixture: known constant, stored first
        endif

; ---- S1: clear every enable (bit 15 = 0 clears, p.280) -------------------
        move.w  #$7FFF,INTENA(a6)
        move.w  #$7FFF,DMACON(a6)

; ---- S2: connector identity: JOY0DAT and JOY1DAT while the harness drives
; pin 4 of one connector low and then high ---------------------------------
        lea     RING_S2,a5
        moveq   #0,d6
        move.l  #S2_ITERS,d5
.s2loop:
        move.w  JOY0DAT(a6),0(a5,d6.w)
        move.w  JOY1DAT(a6),2(a5,d6.w)
        addq.w  #4,d6
        andi.w  #(RING_S2_BYTES-4),d6
        subq.l  #1,d5
        bne.s   .s2loop

        move.w  #$B5B5,MARK_PHASE       ; boundary: connector-identity loop done

; ---- S3 (ECS only): BEAMCON0 <- cfg_BEAMCON0 -------------------------------
        ifnd    NO_S3
        move.w  CFG_BEAMCON0,$DFF1DC
        endif
        ifd     VARBEAM
        move.w  #$00E3,$DFF1C0          ; HTOTAL: lines of 228 colour clocks
        move.w  #$00A0,$DFF1DC          ; BEAMCON0: VARBEAMEN and PAL
        endif

; ---- S4: BPLCON0 <- cfg_BPLCON0_ref ----------------------------------------
        move.w  CFG_BPLCON0_REF,$DFF100

; ---- S5: reference phase ---------------------------------------------------
        lea     SAMPLES,a0
        ifd     W_AT
        move.w  #(W_AT-1),d0
.refa:
        move.l  $DFF004,(a0)+
        dbf     d0,.refa
        ifd     W_V8
        V8_ON
        endif
        move.w  #(W_LEN-1),d0
.refb:
        move.l  $DFF004,(a0)+
        dbf     d0,.refb
        ifd     W_V8
        V8_OFF
        endif
        move.w  #(N_REF-W_AT-W_LEN),d0
.refc:
        move.l  $DFF004,(a0)+
        dbf     d0,.refc
        else
        move.w  #N_REF,d0
.ref:
        move.l  $DFF004,(a0)+
        dbf     d0,.ref
        endif

        move.w  #$A001,MARK_REFEND      ; boundary: reference phase done

; ---- S6: BPLCON0 <- cfg_BPLCON0 --------------------------------------------
        move.w  CFG_BPLCON0,$DFF100

; ---- S7: measurement phase -------------------------------------------------
        ifd     M_AT
        move.w  #(M_AT-1),d0
.meas1:
        move.l  $DFF004,(a0)+
        dbf     d0,.meas1
        V8_ON
        move.w  #(M_LEN-1),d0
.meas2:
        move.l  $DFF004,(a0)+
        dbf     d0,.meas2
        V8_OFF
        move.w  #(N_MEAS-M_AT-M_LEN),d0
.meas3:
        move.l  $DFF004,(a0)+
        dbf     d0,.meas3
        else
        ifd     PAUSE
        move.w  #91,d7                  ; 92 chunks of 500 reads = 46000 reads
.mchunk:
        move.w  #499,d0
.mread:
        move.l  $DFF004,(a0)+
        dbf     d0,.mread
        move.w  #12,d1                  ; a pause of about 70 colour clocks between chunks
.mpause dbf     d1,.mpause
        dbf     d7,.mchunk
        else
        move.w  #N_MEAS,d0
.meas:
        move.l  $DFF004,(a0)+
        dbf     d0,.meas
        endif
        endif

; ---- S8: done mark ---------------------------------------------------------
        ifd     NO_DONE
.hang   bra.s   .hang
        else
        DONE
        endif
