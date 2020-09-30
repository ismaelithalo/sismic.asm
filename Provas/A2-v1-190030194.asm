;A2 - Ismael Ithalo Barbosa Pinto - 190030194
;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
;A2 - Ismael Ithalo Barbosa Pinto - 190030194


				MOV		#SEGREDO1,R5
				MOV		R5, R4
				CALL	#TRANCA
				JMP	$

TRANCA:
				bis.b	#BIT0, &P1DIR ;definindo P1.1 como saída
				bic.b	#BIT0, &P1OUT ;desliga o led
				bis.b	#BIT7, &P4DIR ;definindo P4.7 como saída
				bic.b	#BIT7, &P4OUT ;desliga o led

				bic.b	#BIT1, &P2DIR	;definindo P2.1 como entrada
				bis.b	#BIT1, &P2REN	;ligando resistor de P2.1
				bis.b	#BIT1, &P2OUT	;setando como resistor de pull up

				bic.b	#BIT1, &P1DIR	;definindo P1.1 como entrada
				bis.b	#BIT1, &P1REN	;ligando resistor de P1.1
				bis.b	#BIT1, &P1OUT	;setando como resistor de pull up

senha:
				bis.b	#BIT0, &P1OUT	;liga o led vermelho

				cmp.b	#0, 0(R5)		;testa se o elemento do vetor é zero
				jz		fim				;se for, pula para o fim
				cmp.b	#'D', 0(R5)		;testa se o elemento do vetor é D
				jz		D_test
				cmp.b	#'E', 0(R5)		;teste se é E
				jz		E_test
				jmp		fim_e			;se não for nenhum deles vai para o fim


E_test:
				mov.b	1(R5),	R6		;coloca o numero de vezes que a chave deve ser apertada em R6
				bic.b	#BIT0, &P1OUT	;desliga o led vermelho
E_loop:
				bit.b 	#BIT1,&P2IN		;testa se o botão esta apertado
				jnz		senha			;se não estiver volta para o inicio dos testes
				dec		R6				;descrementa o contador
				jnz		E_loop			;se o contador não for zero eu volto para testar de novo
				incd	R5				;incrementa duas vezes pois tem que pular a letra e o numero
				jmp		senha			;volta para o inicio dos testes

D_test:
				mov.b	1(R5),	R6
				bic.b	#BIT0, &P1OUT
D_loop:
				bit.b 	#BIT1,&P1IN		;testa se o botão esta apertado
				jnz		senha			;se não estiver volta para o inicio dos testes
				dec		R6				;descrementa o contador
				jnz		D_loop			;se o contador não for zero eu volto para testar de novo
				incd	R5				;incrementa duas vezes pois tem que pular a letra e o numero
				jmp		senha			;volta para o inicio dos testes


fim:
				bis.b	#BIT7, &P4OUT	;acende o led verde
fim_e:			RET

			.data														;0x2400
			.space	0x20												;Pular 32 bytes (espaço para suas variáveis)
SEGREDO1:	.byte	'E', 2, 'D', 3, 'E', 1, 'D', 2, 0					;SEGREDO1 = 0x2420
SEGREDO2:	.byte	'D', 3, 'E', 3, 'D', 1, 'E', 5, 'D', 1, 'E', 2, 0
SEGREDO3:	.byte	'D', 1, 'E', 1, 'D', 2, 'E', 2, 'D', 3, 'E', 3, 'D', 1, 0

;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack
            
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET
            
