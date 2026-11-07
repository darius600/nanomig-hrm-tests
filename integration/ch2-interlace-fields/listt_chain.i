; ---- one Copper list of T-n and of its stimulus records (SPECIFICATION.md 4.4) ----
; Included once per list. The list writes HTOTAL (BEAMCON0 VARBEAMEN is set by the CPU) in every
; line 0 ... 262, early in its own line at (v,$40): $E3 (228 colour clocks, 0-$E3, p23) or $E2
; (227), alternating from line to line, and marks every line as the normal list does, with
; WAIT (v,$E0) and MOVE COLOR00. Lines 255 ... 262 are waited for as in listt_high.i. The
; Copper reaches every line (p24: 262 lines of beam travel); nothing of this list is written by
; the CPU.
; The phase is carried from one field into the next through COP1LC, which the Copper writes
; itself: at the start of the list COP1LC is set to CH_SHORT, the list the Copper restarts with
; when this field ends without a line 262 (the Copper then waits for it until the restart); in
; a field that has a line 262 the Copper passes the WAIT (6,$40), which is the one place where
; this list writes COP1LC again, to CH_LONG. A field of 262 lines (lines 0 ... 261) ends on the
; other length than it began with, so the next field starts with the same phase; a field of 263
; lines ends on the length it began with, so the next field starts with the other phase.
; The includer sets, with SET, in front of the include:
;   CH_PH     0: line 0 is $E3 (228), 1: line 0 is $E2 (227)
;   CH_SHORT  address of the list that follows a field without a line 262
;   CH_LONG   address of the list that follows a field with a line 262
; and may define:
;   CH_LAST_E8  line 262 writes HTOTAL = $E8 (233 colour clocks) instead of $E3 or $E2 (T-t)
;   CH_SKIP     lines 51 ... 60 write the value of the line before them (T-u)
CHL     set     0
        dc.w    $0041,$FFFE                    ; WAIT (0,$40)
        dc.w    $01C0,$E3-((CHL+CH_PH)&1)      ; MOVE HTOTAL
        dc.w    $00E1,$FFFE                    ; WAIT (0,$E0)
        dc.w    COLOR00,$0000                  ; mark, line 0
        dc.w    BPL1PTH,(PLANE>>16)&$FFFF      ; pointer rewrite during vertical blanking (p27)
        dc.w    BPL1PTL,PLANE&$FFFF
        dc.w    COP1LCH,(CH_SHORT>>16)&$FFFF
        dc.w    COP1LCL,CH_SHORT&$FFFF
CHL     set     1
        rept    262
CHU     set     CHL
        ifd     CH_SKIP
        ifge    CHL-51
        ifle    CHL-60
CHU     set     CHL-1
        endif
        endif
        endif
        dc.w    ((CHL&255)<<8)|$41,$FFFE       ; WAIT (v,$40)
        ifd     CH_LAST_E8
        ifeq    CHL-262
        dc.w    $01C0,$00E8                    ; MOVE HTOTAL
        else
        dc.w    $01C0,$E3-((CHU+CH_PH)&1)
        endif
        else
        dc.w    $01C0,$E3-((CHU+CH_PH)&1)      ; MOVE HTOTAL
        endif
        ifeq    CHL-262
        dc.w    COP1LCH,(CH_LONG>>16)&$FFFF    ; a field that has a line 262
        dc.w    COP1LCL,CH_LONG&$FFFF
        endif
        dc.w    ((CHL&255)<<8)|$E1,$FFFE       ; WAIT (v,$E0)
        dc.w    COLOR00,$0000                  ; mark, line v
        ifeq    CHL-255
        dc.w    COLOR00,$0000                  ; three MOVEs behind the mark of line 255 (listt_high.i)
        dc.w    COLOR00,$0000
        dc.w    COLOR00,$0000
        endif
CHL     set     CHL+1
        endr
