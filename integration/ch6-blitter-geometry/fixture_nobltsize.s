; fixture_nobltsize.s -- A0 mutation: withhold the final BLTSIZE write, so
; the blit never starts; the destination must stay at its seed value.
NOBLTSIZE       equ     1
        include "fixture.s"
