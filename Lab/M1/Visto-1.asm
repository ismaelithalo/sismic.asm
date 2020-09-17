;Ismael Ithalo Barbosa Pinto - 190030194
;A1
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
;Ismael Ithalo Barbosa Pinto - 190030194
;
; R4 = backup do R5
; R5 = recebe o numero a ser convertido
; R6 = recebe o ponteiro para o vetor que armazena a resposta

NUM			.equ 	2019			; Indicar número a ser convertido
;
			mov		#NUM, R5		; coloca o valor a ser convertido em r5
			mov 	R5, R4			; guarda backup do valor para operar com r5
			mov 	#RESP, R6		; inicializa o ponteiro da resposta
			call	#ALG_ROM		; chama a subrotina ALG_ROM
			jmp 	$				; loop infinito no final do programa
			nop

ALG_ROM:
			cmp.b	#0, R5			; verifica se o valor atual eh zero
			jz		FIM				; salta para o fim da subrotina

CASE_1000:
			sub		#1000, R5		; subtrai o valor 1000 de r5 para verificar se faz parte
			jn		CASE_900		; se der negativo analisa o próximo caso
			mov 	R5, R4			; senao guarda backup do valor atual para operar com r5
			mov.b	#'M', 0(R6)		; coloca a letra M no vetor
			inc		R6				; incrementa o ponteiro da resposta
			jmp 	ALG_ROM			; retorna para o loop para verificar se eh zero

CASE_900:
			mov 	R4, R5			; usa o backup de r5 pois um valor foi subtraido anteriormente
			sub		#900, R5		; subtrai o valor 900 de r5 para verificar se faz parte
			jn		CASE_500		; case de negativo analisa o próximo caso
			mov 	R5, R4			; senao guarda backup do valor para operar com r5
			mov.b	#'C', 0(R6)		; coloca a letra C no vetor resposta
			mov.b	#'M', 1(R6)		; coloca a letra M no vetor resposta
			incd	R6				; incrementa o ponteiro da resposta duas vezes pois foram duas letras
			jmp 	ALG_ROM			; retorna para o loop para verificar se eh zero

CASE_500:
			mov 	R4, R5			; usa o backup de r5 pois um valor foi subtraido anteriormente
			sub		#500, R5		; subtrai o valor 500 de r5 para verificar se faz parte
			jn		CASE_400		; se der negativo analisa o próximo caso
			mov 	R5, R4			; senao guarda backup do valor para operar com r5
			mov.b	#'D', 0(R6)		; coloca a letra D no vetor resposta
			inc		R6				; incrementa o ponteiro da resposta
			jmp 	ALG_ROM			; retorna para o loop para verificar se eh zero

CASE_400:
			mov 	R4, R5			; usa o backup de r5 pois um valor foi subtraido anteriormente
			sub		#400, R5		; subtrai o valor 400 de r5 para verificar se faz parte
			jn		CASE_100		; caso negativo analisa o próximo caso
			mov 	R5, R4			; senao guarda backup do valor para operar com r5
			mov.b	#'C', 0(R6)		; coloca a letra C no vetor resposta
			mov.b	#'D', 1(R6)		; coloca a letra D no vetor resposta
			incd	R6				; incrementa o ponteiro da resposta duas vezes pois foram duas letras
			jmp 	ALG_ROM			; retorna para o loop para verificar se eh zero

CASE_100:
			mov 	R4, R5			; usa o backup de r5 pois um valor foi subtraido anteriormente
			sub		#100, R5		; subtrai o valor 100 de r5 para verificar se faz parte
			jn		CASE_90			; caso negativo analisa o próximo caso
			mov 	R5, R4			; senao guarda backup do valor para operar com r5
			mov.b	#'C', 0(R6)		; coloca a letra C no vetor resposta
			inc		R6				; incrementa o ponteiro da resposta
			jmp 	ALG_ROM			; retorna para o loop para verificar se eh zero

CASE_90:
			mov 	R4, R5			; usa o backup de r5 pois um valor foi subtraido anteriormente
			sub		#90, R5			; subtrai o valor 90 de r5 para verificar se faz parte
			jn		CASE_50			; caso negativo analisa o próximo caso
			mov 	R5, R4			; senao guarda backup do valor para operar com r5
			mov.b	#'X', 0(R6)		; coloca a letra X no vetor resposta
			mov.b	#'C', 1(R6)		; coloca a letra C no vetor resposta
			incd	R6				; incrementa o ponteiro da resposta duas vezes pois foram duas letras
			jmp 	ALG_ROM			; retorna para o loop para verificar se eh zero

CASE_50:
			mov 	R4, R5			; usa o backup de r5 pois um valor foi subtraido anteriormente
			sub		#50, R5			; subtrai o valor 50 de r5 para verificar se faz parte
			jn		CASE_40			; caso negativo analisa o próximo caso
			mov 	R5, R4			; senao guarda backup do valor para operar com r5
			mov.b	#'L', 0(R6)		; coloca a letra L no vetor resposta
			inc		R6				; incrementa o ponteiro da resposta
			jmp 	ALG_ROM			; retorna para o loop para verificar se eh zero

CASE_40:
			mov 	R4, R5			; usa o backup de r5 pois um valor foi subtraido anteriormente
			sub		#40, R5			; subtrai o valor 40 de r5 para verificar se faz parte
			jn		CASE_10			; caso negativo analisa o próximo caso
			mov 	R5, R4			; senao guarda backup do valor para operar com r5
			mov.b	#'X', 0(R6)		; coloca a letra X no vetor resposta
			mov.b	#'L', 1(R6)		; coloca a letra L no vetor resposta
			incd	R6				; incrementa o ponteiro da resposta duas vezes pois foram duas letras
			jmp 	ALG_ROM			; retorna para o loop para verificar se eh zero

CASE_10:
			mov 	R4, R5			; usa o backup de r5 pois um valor foi subtraido anteriormente
			sub		#10, R5			; subtrai o valor 10 de r5 para verificar se faz parte
			jn		CASE_9			; caso negativo analisa o próximo caso
			mov 	R5, R4			; senao guarda backup do valor para operar com r5
			mov.b	#'X', 0(R6)		; coloca a letra X no vetor resposta
			inc		R6				; incrementa o ponteiro da resposta
			jmp 	ALG_ROM			; retorna para o loop para verificar se eh zero

CASE_9:
			mov 	R4, R5			; usa o backup de r5 pois um valor foi subtraido anteriormente
			sub		#9, R5			; subtrai o valor 9 de r5 para verificar se faz parte
			jn		CASE_5			; caso negativo analisa o próximo caso
			mov 	R5, R4			; senao guarda backup do valor para operar com r5
			mov.b	#'I', 0(R6)		; coloca a letra I no vetor resposta
			mov.b	#'X', 1(R6)		; coloca a letra X no vetor resposta
			incd	R6				; incrementa o ponteiro da resposta duas vezes pois foram duas letras
			jmp 	ALG_ROM			; retorna para o loop para verificar se eh zero

CASE_5:
			mov 	R4, R5			; usa o backup de r5 pois um valor foi subtraido anteriormente
			sub		#5, R5			; subtrai o valor 5 de r5 para verificar se faz parte
			jn		CASE_4			; caso negativo analisa o próximo caso
			mov 	R5, R4			; senao guarda backup do valor para operar com r5
			mov.b	#'V', 0(R6)		; coloca a letra V no vetor resposta
			inc		R6				; incrementa o ponteiro da resposta
			jmp 	ALG_ROM			; retorna para o loop para verificar se eh zero

CASE_4:
			mov 	R4, R5			; usa o backup de r5 pois um valor foi subtraido anteriormente
			sub		#4, R5			; subtrai o valor 4 de r5 para verificar se faz parte
			jn		CASE_1			; caso negativo analisa o próximo caso
			mov 	R5, R4			; senao guarda backup do valor para operar com r5
			mov.b	#'I', 0(R6)		; coloca a letra I no vetor resposta
			mov.b	#'V', 1(R6)		; coloca a letra V no vetor resposta
			incd	R6				; incrementa o ponteiro da resposta duas vezes pois foram duas letras
			jmp 	ALG_ROM			; retorna para o loop para verificar se eh zero

CASE_1:
			mov 	R4, R5			; usa o backup de r5 pois um valor foi subtraido anteriormente
			sub		#1, R5			; subtrai o valor 1 de r5 para verificar se faz parte
			jn		FIM				; caso negativo analisa o próximo caso
			mov 	R5, R4			; senao guarda backup do valor para operar com r5
			mov.b	#'I', 0(R6)		; coloca a letra I no vetor resposta
			inc		R6				; incrementa o ponteiro da resposta
			jmp 	ALG_ROM			; retorna para o loop para verificar se eh zero

FIM:		mov.b	#0, 0(R6)		; coloca 0 no fim do vetor resposta
			ret						; sai da subrotina

			.data
; Local para armazenar a resposta (RESP = 0x2400)
			.retain
RESP:		.byte "RRRRRRRRRRRRRRRRRR",0

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
            
