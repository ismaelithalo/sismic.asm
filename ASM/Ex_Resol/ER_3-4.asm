tp1			.equ	100
tp2			.equ	1736

				call	#P_VM_VD
				jmp		$

P_VM_VD:
				bis.b	#BIT0, &P1DIR ;definindo P1.1 como saída
				bis.b	#BIT7, &P4DIR ;definindo P4.7 como saída
loop:

				bis.b	#BIT0, &P1OUT
				call	#espera
				xor.b	#BIT0, &P1OUT
				bis.b	#BIT7, &P4OUT
				call	#espera
				xor.b	#BIT7, &P4OUT
				jmp		loop

espera:
				mov		#tp2, R5
tempo1:
				mov		#tp1, R4
tempo2:
				dec		R4
				jnz		tempo2
				dec		R5
				jnz		tempo1
				ret
