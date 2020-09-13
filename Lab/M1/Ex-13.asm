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
; R6 = ponteiro para os dados
; R7 = caracter atual
; R8 = acumulador
; R5 = retorno

			mov		#NUM_ROM, R6	; inicializa o ponteiro
			call 	#ROM_ARAB		; chama a subrotina
			jmp 	$				; loop infinito no final do programa

ROM_ARAB:
			mov		#0, R8			; zera o acumulador

LOOP:
			mov.b	@R6+, R7		; pega o caracter atual do vetor

			cmp.b	#0, R7			; se(fim do vetor)
			jz		FIM				; salta para o fim da subrotina

CASE_M:	; M
			cmp.b	#'M', R7		; se R7 é diferente de M
			jne		CASE_D			; analisa o próximo caso
			add		#1000, R8		; senão soma 1000 no acumulador
			jmp 	LOOP			; repete do início
CASE_D: ; D
			cmp.b	#'D', R7		; se R7 é diferente de D
			jne		CASE_C			; analisa o próximo caso
			add		#500, R8		; senão soma 500 no acumulador
			jmp 	LOOP			; repete do início
CASE_C:
			cmp.b	#'C', R7		; se R7 é diferente de C
			jne		CASE_L			; analisa o próximo caso
		;CM							; se R7 é igual a C
			cmp.b	#'M', 0(R6)		; se o próximo caracter é diferente de M
			jne		CASE_C2			; analisa o próximo caso
			add		#900, R8		; se não soma 900
			inc		R6				; consome o próximo caracter do vetor
			jmp		LOOP			; repete do início
CASE_C2: ;CD
			cmp.b	#'D', 0(R6)		; de o próximo caracter é diferente de D
			jne		CASE_C3			; analisa o próximo caso
			add		#400, R8		; senão soma 400
			inc		R6				; consome o próximo caracter do vetor
			jmp		LOOP			; repete do início
CASE_C3: ;C							
			add		#100, R8		; soma 100 
			jmp 	LOOP			; repete do início

CASE_L:
			cmp.b	#'L', R7
			jne		CASE_X
			add		#50, R8
			jmp 	LOOP

CASE_X:
			cmp.b	#'X', R7
			jne		CASE_V

			cmp.b	#'C', 0(R6)
			jne		CASE_X2
			add		#400, R8
			inc		R6
			jmp		LOOP
CASE_X2:
			cmp.b	#'L', 0(R6)
			jne		CASE_X3
			add		#40, R8
			inc		R6
			jmp		LOOP
CASE_X3:
			add		#10, R8
			jmp 	LOOP

CASE_V:
			cmp.b	#'V', R7
			jne		CASE_I
			add		#5, R8
			jmp 	LOOP

CASE_I:
			cmp.b	#'I', R7
			jne		ERRO

			cmp.b	#'X', 0(R6)
			jne		CASE_I2
			add		#9, R8
			inc		R6
			jmp		LOOP
CASE_I2:
			cmp.b	#'V', 0(R6)
			jne		CASE_I3
			add		#4, R8
			inc		R6
			jmp		LOOP
CASE_I3:
			add		#1, R8
			jmp 	LOOP

ERRO:
			mov		#0xFFFF, R8		; coloca 0xFFFF no acumulador e termina o programa

FIM:
			mov		R8, R5			; retorna o resultado do acumulador
			ret						

                                            
			.data
			.retain
NUM_ROM:
			.byte "MMXIX",0		; 2019
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
