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
		MOV	#SEQTRIB,R5 ;ponteiro para o vetor
		CALL #TRIB
		JMP	$
        NOP

TRIB:	incd R5 ;Incrementa R5 para colocar na terceira posição
        incd R5
        incd R5
        mov #3, R6 ;Já somando as três posições fixas
        mov #500, R7 ;Valor de comparação

T_LOOP:
		mov -2(R5), 0(R5) ;Colocando o elemento anterior na proxima posição
		mov -4(R5), R4
		rla	R4		;rotação aritmetica para a esquerda é multiplicar por 2
		add R4, 0(R5)
        sub -6(R5), 0(R5) ;Fazendo três subtrações sucessivas
		sub -6(R5), 0(R5) ; Fazer uma subrotina gastaria mais linhas
		sub -6(R5), 0(R5)
        inc R6 ;Inecrementa o contador
        cmp R7, 0(R5) ;Compara o valor com R7
        jge FIM ;Se for igual ou mais vai para o fim
        incd R5 ;Avança uma posição de memoria
        jmp T_LOOP ;Repere o loop

FIM:    mov 0(R5), R7
		ret

	     .data
SEQTRIB: .word	1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		 .word	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		 .word	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0


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
            
