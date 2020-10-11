A0:      MOV   #COVID_MM7, R4
         MOV   #COVID_MAR,R5
	      MOV.b #31, R6
         CALL  #MAR_MM7
         JMP   $
         NOP

; SUA SOLUÇÃO
MAR_MM7: mov.w  0(R5), R10
         add.w -2(R5), R10
         add.w -4(R5), R10
         add.w -6(R5), R10
         add.w -8(R5), R10
         add.w -10(R5), R10
         add.w -12(R5), R10
	      clr.w R11
;
DIV:     sub.w #7, R10
         inc  R11
         cmp  #7, R10
         jc  DIV
;
         mov.w R11, 0(R4)
         incd R5
         incd R4
         dec.b R6
         jnz MAR_MM7
         ret

             .data
; Casos nos últimos 6 dias de fevereiro
COVID_FEV:   .word     0,    0,    1,    1,    1,    2
;
; Casos durante os 31 dias se março
COVID_MAR:   .word     2,    2,    2,    3,    7,   13,   19,   25,  25,    34
             .word    52,   77,   98,  121,  200,  234,  291,  428,  621,  904
             .word  1128, 1546, 1891, 2201, 2433, 2915, 3417, 3903, 4256, 4579, 5717
;
; Média móvel de março (sua resposta)
COVID_MM7:   .word    0,     0,    0,    0,    0,    0,    0,    0,    0,    0
             .word    0,     0,    0,    0,    0,    0,    0,    0,    0,    0
             .word    0,     0,    0,    0,    0,    0,    0,    0,    0,    0,    0
