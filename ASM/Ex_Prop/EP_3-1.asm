Atraso			.equ	65535

				call	#P_VM
				jmp		$

P_VM:
        bis.b	#BIT0, &P1DIR ;definindo P1.1 como saída
VM_loop:
				
				bis.b	#BIT0, &P1OUT
				call	#espera
				xor.b	#BIT0, &P1OUT
				call	#espera
				jmp		VM_loop

espera:
				mov		#Atraso, R4
espera_loop:
				dec		R4
				jnz		espera_loop
				ret
