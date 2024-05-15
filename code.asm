START 1000                / start
start:  MOV ax,worda
        MOV bx,wordb     / in the b wordb
        CALL compr
        MUL ax,bx
        MOV cx,16
cycle:  ADD ax,1          / add ax one
        LOOP cycle
compr:  CMP bx,ax         / is bx more ax
        JE  exit
        RET
exit:   MOV bytew,ax
        HLT
worda:  BYTE 15
wordb:  BYTE 7
bytew:  WORD 1
END start