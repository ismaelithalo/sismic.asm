;Rotina R-2.10
    call #SUM8
		jmp $
;
SUM8:	clr.w R10
		  mov.w #vetor, R5
		  mov.b @R5+, R6

LOOP:	add.b @R5+, R10
		  dec.b R6
 		  jnz LOOP
		  ret

		    .data
vetor:	.byte 0x05,0x04,0x07,0x03,0x09,0x02
