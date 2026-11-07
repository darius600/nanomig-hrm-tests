; ---- generated: the lists of T-v (SPECIFICATION.md 4.4), a chain of ten states of listt_chain.i ----
; State s and phase p are one list, at LISTT for (0,0) and at $0A0000 + (2 s + p - 1) * $1200 for the
; others. In the states 0 ... 7 a field without a line 262 is followed by the list of the same phase in the
; next state and a field with a line 262 by the list of the other phase (as T-n does). The state 8 does
; the opposite: its lists restart with the phase of the length they ended on, once. The state 9 is
; T-n again and stays in itself.
        ifnd    CH_PART2
CH_PH          set     0
CH_SHORT       set     $0A1200
CH_LONG        set     $0A2400
        include "listt_chain.i"
        else
        org     $0A0000                  ; state 0, phase 1
CH_PH          set     1
CH_SHORT       set     $0A2400
CH_LONG        set     $0A1200
        include "listt_chain.i"
        dc.w    $FFFF,$FFFE
        org     $0A1200                  ; state 1, phase 0
CH_PH          set     0
CH_SHORT       set     $0A3600
CH_LONG        set     $0A4800
        include "listt_chain.i"
        dc.w    $FFFF,$FFFE
        org     $0A2400                  ; state 1, phase 1
CH_PH          set     1
CH_SHORT       set     $0A4800
CH_LONG        set     $0A3600
        include "listt_chain.i"
        dc.w    $FFFF,$FFFE
        org     $0A3600                  ; state 2, phase 0
CH_PH          set     0
CH_SHORT       set     $0A5A00
CH_LONG        set     $0A6C00
        include "listt_chain.i"
        dc.w    $FFFF,$FFFE
        org     $0A4800                  ; state 2, phase 1
CH_PH          set     1
CH_SHORT       set     $0A6C00
CH_LONG        set     $0A5A00
        include "listt_chain.i"
        dc.w    $FFFF,$FFFE
        org     $0A5A00                  ; state 3, phase 0
CH_PH          set     0
CH_SHORT       set     $0A7E00
CH_LONG        set     $0A9000
        include "listt_chain.i"
        dc.w    $FFFF,$FFFE
        org     $0A6C00                  ; state 3, phase 1
CH_PH          set     1
CH_SHORT       set     $0A9000
CH_LONG        set     $0A7E00
        include "listt_chain.i"
        dc.w    $FFFF,$FFFE
        org     $0A7E00                  ; state 4, phase 0
CH_PH          set     0
CH_SHORT       set     $0AA200
CH_LONG        set     $0AB400
        include "listt_chain.i"
        dc.w    $FFFF,$FFFE
        org     $0A9000                  ; state 4, phase 1
CH_PH          set     1
CH_SHORT       set     $0AB400
CH_LONG        set     $0AA200
        include "listt_chain.i"
        dc.w    $FFFF,$FFFE
        org     $0AA200                  ; state 5, phase 0
CH_PH          set     0
CH_SHORT       set     $0AC600
CH_LONG        set     $0AD800
        include "listt_chain.i"
        dc.w    $FFFF,$FFFE
        org     $0AB400                  ; state 5, phase 1
CH_PH          set     1
CH_SHORT       set     $0AD800
CH_LONG        set     $0AC600
        include "listt_chain.i"
        dc.w    $FFFF,$FFFE
        org     $0AC600                  ; state 6, phase 0
CH_PH          set     0
CH_SHORT       set     $0AEA00
CH_LONG        set     $0AFC00
        include "listt_chain.i"
        dc.w    $FFFF,$FFFE
        org     $0AD800                  ; state 6, phase 1
CH_PH          set     1
CH_SHORT       set     $0AFC00
CH_LONG        set     $0AEA00
        include "listt_chain.i"
        dc.w    $FFFF,$FFFE
        org     $0AEA00                  ; state 7, phase 0
CH_PH          set     0
CH_SHORT       set     $0B0E00
CH_LONG        set     $0B2000
        include "listt_chain.i"
        dc.w    $FFFF,$FFFE
        org     $0AFC00                  ; state 7, phase 1
CH_PH          set     1
CH_SHORT       set     $0B2000
CH_LONG        set     $0B0E00
        include "listt_chain.i"
        dc.w    $FFFF,$FFFE
        org     $0B0E00                  ; state 8, phase 0
CH_PH          set     0
CH_SHORT       set     $0B4400
CH_LONG        set     $0B3200
        include "listt_chain.i"
        dc.w    $FFFF,$FFFE
        org     $0B2000                  ; state 8, phase 1
CH_PH          set     1
CH_SHORT       set     $0B3200
CH_LONG        set     $0B4400
        include "listt_chain.i"
        dc.w    $FFFF,$FFFE
        org     $0B3200                  ; state 9, phase 0
CH_PH          set     0
CH_SHORT       set     $0B3200
CH_LONG        set     $0B4400
        include "listt_chain.i"
        dc.w    $FFFF,$FFFE
        org     $0B4400                  ; state 9, phase 1
CH_PH          set     1
CH_SHORT       set     $0B4400
CH_LONG        set     $0B3200
        include "listt_chain.i"
        dc.w    $FFFF,$FFFE
        endif
