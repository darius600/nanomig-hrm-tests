; ---- the Copper marks of the lines 255 ... 312 (SPECIFICATION.md 2.7, 4.2) ----
; p24: "WAIT for position (0,255)" ... "WAIT for any horizontal position with vertical position 0
; through 5, covering the last 6 lines of the scan before vertical blanking occurs" ... "Thus the
; total of 256 + 6 = 262 lines of video beam travel during which Copper instructions can be
; executed." The list continues behind the pair of line 254: WAIT (255,$E0) and its mark, three
; more MOVEs that carry the Copper past the end of line 255, and then one WAIT (v - 256, $E0) and
; one MOVE COLOR00 (the mark) for each of the lines 256 ... 312: NTSC fields have 262 or 263
; lines (0 ... 261 or 262) and PAL fields 312 or 313, and the Copper reaches the last line of
; every one of them. A field has no line past its last: its restart comes right behind the mark
; of its last line, before the next WAIT is read.
; T_LINE256_40 (T-o): the WAIT of line 256 uses H=$40 instead of $E0.
        dc.w    $FFE1,$FFFE
        dc.w    COLOR00,$0000                  ; mark, line 255
        dc.w    COLOR00,$0000                  ; three MOVEs behind it: the Copper is in line 256
        dc.w    COLOR00,$0000                  ; when it reads the next WAIT
        dc.w    COLOR00,$0000
        ifd     T_LINE256_40
        dc.w    $0041,$FFFE
        else
        dc.w    $00E1,$FFFE
        endif
        dc.w    COLOR00,$0000                  ; mark, line 256
HV      set     257
        rept    56
        dc.w    ((HV&255)<<8)|$E1,$FFFE
        dc.w    COLOR00,$0000                  ; mark, line HV
HV      set     HV+1
        endr
