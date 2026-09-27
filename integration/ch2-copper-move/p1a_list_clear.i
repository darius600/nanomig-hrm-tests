        ; a_0
        move.w  #$3001,(a1)+           ; WAIT line $30
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_0 (P_0=$0001)
        move.w  #$3201,(a1)+           ; WAIT line $32
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$0001,(a1)+
        ; a_1
        move.w  #$3401,(a1)+           ; WAIT line $34
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_1 (P_1=$0002)
        move.w  #$3601,(a1)+           ; WAIT line $36
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$0002,(a1)+
        ; a_2
        move.w  #$3801,(a1)+           ; WAIT line $38
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_2 (P_2=$0004)
        move.w  #$3A01,(a1)+           ; WAIT line $3A
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$0004,(a1)+
        ; a_3
        move.w  #$3C01,(a1)+           ; WAIT line $3C
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_3 (P_3=$0008)
        move.w  #$3E01,(a1)+           ; WAIT line $3E
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$0008,(a1)+
        ; a_4
        move.w  #$4001,(a1)+           ; WAIT line $40
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_4 (P_4=$0010)
        move.w  #$4201,(a1)+           ; WAIT line $42
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$0010,(a1)+
        ; a_5
        move.w  #$4401,(a1)+           ; WAIT line $44
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_5 (P_5=$0020)
        move.w  #$4601,(a1)+           ; WAIT line $46
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$0020,(a1)+
        ; a_6
        move.w  #$4801,(a1)+           ; WAIT line $48
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_6 (P_6=$0040)
        move.w  #$4A01,(a1)+           ; WAIT line $4A
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$0040,(a1)+
        ; a_7
        move.w  #$4C01,(a1)+           ; WAIT line $4C
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_7 (P_7=$0080)
        move.w  #$4E01,(a1)+           ; WAIT line $4E
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$0080,(a1)+
        ; a_8
        move.w  #$5001,(a1)+           ; WAIT line $50
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_8 (P_8=$0100)
        move.w  #$5201,(a1)+           ; WAIT line $52
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$0100,(a1)+
        ; a_9
        move.w  #$5401,(a1)+           ; WAIT line $54
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_9 (P_9=$0200)
        move.w  #$5601,(a1)+           ; WAIT line $56
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$0200,(a1)+
        ; a_10
        move.w  #$5801,(a1)+           ; WAIT line $58
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_10 (P_10=$0400)
        move.w  #$5A01,(a1)+           ; WAIT line $5A
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$0400,(a1)+
        ; a_11
        move.w  #$5C01,(a1)+           ; WAIT line $5C
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_11 (P_11=$0800)
        move.w  #$5E01,(a1)+           ; WAIT line $5E
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$0800,(a1)+
        ; a_12
        move.w  #$6001,(a1)+           ; WAIT line $60
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_12 (P_12=$1000)
        move.w  #$6201,(a1)+           ; WAIT line $62
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$1000,(a1)+
        ; a_13
        move.w  #$6401,(a1)+           ; WAIT line $64
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_13 (P_13=$2000)
        move.w  #$6601,(a1)+           ; WAIT line $66
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$2000,(a1)+
        ; a_14
        move.w  #$6801,(a1)+           ; WAIT line $68
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_14 (P_14=$4000)
        move.w  #$6A01,(a1)+           ; WAIT line $6A
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$4000,(a1)+
        ; a_15
        move.w  #$6C01,(a1)+           ; WAIT line $6C
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_15 (P_15=$7FFE)
        move.w  #$6E01,(a1)+           ; WAIT line $6E
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$7FFE,(a1)+
        ; a_16
        move.w  #$7001,(a1)+           ; WAIT line $70
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_16 (P_16=$7FFD)
        move.w  #$7201,(a1)+           ; WAIT line $72
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$7FFD,(a1)+
        ; a_17
        move.w  #$7401,(a1)+           ; WAIT line $74
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_17 (P_17=$7FFB)
        move.w  #$7601,(a1)+           ; WAIT line $76
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$7FFB,(a1)+
        ; a_18
        move.w  #$7801,(a1)+           ; WAIT line $78
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_18 (P_18=$7FF7)
        move.w  #$7A01,(a1)+           ; WAIT line $7A
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$7FF7,(a1)+
        ; a_19
        move.w  #$7C01,(a1)+           ; WAIT line $7C
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_19 (P_19=$7FEF)
        move.w  #$7E01,(a1)+           ; WAIT line $7E
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$7FEF,(a1)+
        ; a_20
        move.w  #$8001,(a1)+           ; WAIT line $80
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_20 (P_20=$7FDF)
        move.w  #$8201,(a1)+           ; WAIT line $82
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$7FDF,(a1)+
        ; a_21
        move.w  #$8401,(a1)+           ; WAIT line $84
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_21 (P_21=$7FBF)
        move.w  #$8601,(a1)+           ; WAIT line $86
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$7FBF,(a1)+
        ; a_22
        move.w  #$8801,(a1)+           ; WAIT line $88
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_22 (P_22=$7F7F)
        move.w  #$8A01,(a1)+           ; WAIT line $8A
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$7F7F,(a1)+
        ; a_23
        move.w  #$8C01,(a1)+           ; WAIT line $8C
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_23 (P_23=$7EFF)
        move.w  #$8E01,(a1)+           ; WAIT line $8E
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$7EFF,(a1)+
        ; a_24
        move.w  #$9001,(a1)+           ; WAIT line $90
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_24 (P_24=$7DFF)
        move.w  #$9201,(a1)+           ; WAIT line $92
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$7DFF,(a1)+
        ; a_25
        move.w  #$9401,(a1)+           ; WAIT line $94
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_25 (P_25=$7BFF)
        move.w  #$9601,(a1)+           ; WAIT line $96
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$7BFF,(a1)+
        ; a_26
        move.w  #$9801,(a1)+           ; WAIT line $98
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_26 (P_26=$77FF)
        move.w  #$9A01,(a1)+           ; WAIT line $9A
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$77FF,(a1)+
        ; a_27
        move.w  #$9C01,(a1)+           ; WAIT line $9C
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_27 (P_27=$6FFF)
        move.w  #$9E01,(a1)+           ; WAIT line $9E
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$6FFF,(a1)+
        ; a_28
        move.w  #$A001,(a1)+           ; WAIT line $A0
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_28 (P_28=$5FFF)
        move.w  #$A201,(a1)+           ; WAIT line $A2
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$5FFF,(a1)+
        ; a_29
        move.w  #$A401,(a1)+           ; WAIT line $A4
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_29 (P_29=$3FFF)
        move.w  #$A601,(a1)+           ; WAIT line $A6
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$3FFF,(a1)+
        ; a_30
        move.w  #$A801,(a1)+           ; WAIT line $A8
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_30 (P_30=$0000)
        move.w  #$AA01,(a1)+           ; WAIT line $AA
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$0000,(a1)+
        ; a_31
        move.w  #$AC01,(a1)+           ; WAIT line $AC
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$FFFF,(a1)+
        ; b_31 (P_31=$7FFF)
        move.w  #$AE01,(a1)+           ; WAIT line $AE
        move.w  #$FF00,(a1)+
        move.w  #INTENA,(a1)+
        move.w  #$7FFF,(a1)+
