;CONSTANTES
TP1			.equ	32500
FECHADA		.equ	0						;Zero representa a chave fechada
ABERTA		.equ	1						;Um representa a chave aberta
DBC			.equ	10000					;Chute para o debounce

;VARIAVEIS
VAR			.equ	0x2400					;Área de variaveis, todas de 16 bits
PS2			.equ	VAR						;Passado da chave S2
ES2			.equ	VAR+2					;Vai para 1 quando S2 passa de A-->F
CONT		.equ	VAR+4

			call	#CONTA
			jmp		$

CONTA:		call	#CONFIG
			mov		#ABERTA,&PS2			;Inicia o estado anterior já como aberto
			clr		&CONT
			clr		&ES2
LB1:		call	#MON_S2					;Chama rotina de monitoramento
			tst		&ES2					;testo valor de ES1
			jz		LB1						;Se 0 monitorar novamente
			jmp		LB2						;Se 1 incrementar contador

LB2:		inc		&CONT
			call	#CONT_LEDS
			clr		&ES2
			jmp		LB1

CONT_LEDS:	bit		#BIT0,&CONT
			jz		CL1						;se bit0=0 saltar
			bis.b	#BIT7,&P4OUT			;se bit0=1 led 2 acesso
			jmp		CL2
CL1:		bic.b	#BIT7,&P4OUT			;se bit0=0 led 2 apagado
CL2:		bit		#BIT1,&CONT
			jz		CL3						;se bit1=0 saltar
			bis.b	#BIT0,&P1OUT			;se bit1=1 led 1 acesso
			ret
CL3:		bic.b	#BIT0,&P1OUT			;se bit1=0 led 1 apagado
			ret

MON_S2:		bit.b	#BIT1,&P1IN				;Testo a chave 2 em seu estado atual
			jnz		MON4					;Se aberta, saltar
			cmp		#FECHADA,&PS2			;s2 atual está fechado, comparar com seu estado passado se está fechado
			jz		MON5					;Se estado passado fechado, seguir
			mov		#1,&ES2					;Estado passado é aberto, então A-->F, ES2=1
			call	#DEBOUNCE
			mov		#FECHADA,&PS2			;Novo estado passado = fechado
MON5:		RET
MON4:		cmp		#ABERTA,&PS2			;Estado atual aberto, comparar se o estado passado tbm é aberto
			jz		MON6					;se aberta, seguir (não mudou nada)
			call	#DEBOUNCE
			mov		#ABERTA,&PS2			;Novo estado passado = aberta
MON6:		RET

DEBOUNCE:	mov		#DBC,R5
DB1:		dec		R5
			jnz		DB1
			ret

TEMPO:		mov		#TP1,R6
LB8:		dec		R6
			jnz		LB8
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
