; wordonly_wrongword.s -- A11 mutation: the pointer's even part addresses
; the next word instead (still an odd byte address, LSB still ignored).
WRONGWORD       equ     1
        include "wordonly.s"
