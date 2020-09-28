;CONSTANTES
TP1			.equ	100
TP2			.equ	1736
FECHADA		.equ	0						;Zero representa a chave fechada
ABERTA		.equ	1						;Um representa a chave aberta
DBC			.equ	10000					;Chute para o debounce

;VARIAVEIS
VAR			.equ	0x2400					;Área de variaveis, todas de 16 bits
PS2			.equ	VAR						;Passado da chave S2
ES2			.equ	VAR+2					;Vai para 1 quando S2 passa de A-->F
PS1			.equ	VAR+4					;Passado da chave S1
ES1			.equ	VAR+6					;Vai para 1 quando S1 passa de A-->F

			call	#LEDS
			jmp		$

LEDS:		call	#CONFIG
			mov		#ABERTA,&PS1			;Inicia o estado anterior já como aberto
			mov		#ABERTA,&PS2			;Inicia o estado anterior já como aberto
LB1:		clr		&ES1
			clr		&ES2
			call	#MON_S1					;Chama rotina de monitoramento
			tst		&ES1					;testo valor de ES1
			jz		LB2						;Se 0 saltar e monitorar APENAS chave 2
			jmp		LB3						;Se 1 é necessario ver se a chave s2 também está acionada

LB2:		call	#MON_S2
			tst		&ES2
			jz		LB1
			xor.b	#BIT7,&P4OUT
			call	#DEBOUNCE
			CLR		&ES2
			jmp		LB1

LB3:		call	#MON_S2
			tst		&ES2
			jz		LB4
			bic.b	#BIT0,&P1OUT           ;para piscar alternadamente?
			bis.b	#BIT7,&P4OUT
LB5:		xor.b	#BIT0,&P1OUT
			xor.b	#BIT7,&P4OUT
			call	#TEMPO

LB6:		bit.b	#BIT1,&P2IN
			jnz		LB1
			jmp		LB7

LB7:		bit.b	#BIT1,&P1IN
			jnz		LB1
			jmp		LB5


LB4:		xor.b	#BIT0,&P1OUT
			call	#DEBOUNCE
			CLR		&ES1
			jmp		LB1


MON_S1:		bit.b	#BIT1,&P2IN				;Testo a chave 1 em seu estado atual
			jnz		MON1					;Se aberta, saltar
			cmp		#FECHADA,&PS1			;s1 atual está fechado, comparar com seu estado passado se está fechado
			jz		MON2					;Se estado passado fechado, seguir
			mov		#1,&ES1					;Estado passado é aberto, então A-->F, ES1=1
			call	#DEBOUNCE
			mov		#FECHADA,&PS1			;Novo estado passado = fechado
MON2:		RET

MON1:
			cmp		#ABERTA,&PS1			;Estado atual aberto, comparar se o estado passado tbm é aberto
			jz		MON3					;se aberta, seguir (não mudou nada)
			call	#DEBOUNCE
			mov		#ABERTA,&PS1			;Novo estado passado = aberta
MON3:		RET


MON_S2:		bit.b	#BIT1,&P1IN				;Testo a chave 2 em seu estado atual
			jnz		MON4					;Se aberta, saltar
			cmp		#FECHADA,&PS2			;s2 atual está fechado, comparar com seu estado passado se está fechado
			jz		MON5					;Se estado passado fechado, seguir
			mov		#1,&ES2					;Estado passado é aberto, então A-->F, ES2=1
			call	#DEBOUNCE
			mov		#FECHADA,&PS2			;Novo estado passado = fechado
MON5:		RET
MON4:
			cmp		#ABERTA,&PS2			;Estado atual aberto, comparar se o estado passado tbm é aberto
			jz		MON6					;se aberta, seguir (não mudou nada)
			call	#DEBOUNCE
			mov		#ABERTA,&PS2			;Novo estado passado = aberta
MON6:		RET

DEBOUNCE:	mov		#DBC,R5
DB1:		dec		R5
			jnz		DB1
			ret

TEMPO:		mov		#TP2,R6					;Tempo para 1hz
TMP1:		mov		#TP1,R5
TMP2:		dec		R5
			jnz		TMP2
			dec		R6
			jnz		TMP1
			ret

CONFIG:		bis.b	#BIT0,&P1DIR
			bic.b	#BIT0,&P1OUT

			bis.b	#BIT7,&P4DIR
			bic.b	#BIT7,&P4OUT

			bic.b	#BIT1,&P2DIR
			bis.b	#BIT1,&P2REN
			bis.b	#BIT1,&P2OUT

			bic.b	#BIT1,&P1DIR
			bis.b	#BIT1,&P1REN
			bis.b	#BIT1,&P1OUT
			ret
