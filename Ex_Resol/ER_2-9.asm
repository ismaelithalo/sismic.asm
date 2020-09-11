;Rotina 2.9
		mov.w #5, R6
		call #SOMA
		jmp $
;
SOMA:	clr.w R10
		clr.w R11
		mov.w #DT, R5
;
LOOP:	add.w @R5+, R10
		adc R11
		dec.w R6
 		jnz LOOP
		ret
;
		.data
DT:		.word 0xABCD, 0xEEEE, 0x1111, 0xBA98, 0xAAAA

   
