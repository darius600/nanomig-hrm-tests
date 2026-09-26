; prog_diskrate_b3.s -- prog_diskrate.s with MUT_B3 set: precondition
; B3's own contrasting case (run F's steps 5 and 6 use LENGTH=100,
; DSKLEN=$C064, instead of 544 -- only 100 reads occur, fewer than 529).
MUT_B3  equ     1
        include "prog_diskrate.s"
