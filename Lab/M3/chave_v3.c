#include <msp430.h>

//void delay();
void io_config();
volatile int sw2_flag = 0;

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;

    io_config();
	
	// Apaga o vetor de interrupções antes de habilitar as interrupções!
	// As flags são desabilitadas em ordem, uma por uma.
	do {
		P1IFG = 0;  
	while (P1IFG |= 0);
	
	// Liga as interrupções no SR.
	_enable_interrupt();
	
	sw2_flag = 0;
	while(1)
	{
		if (sw2_flag)
		{
		P1OUT ^= BIT0;
		// delay();
		sw2_flag = 0;
		}
	}
}

// Rotina de tratamento de interrupção
// Tentar manter o mais rápido possível!
#pragma vector = PORT1_VECTOR // Instrui o compilador onde alocar a ISR.
__interrupt void PORT1_ISR()
{
	// P1IV codifica a interrupção de maior prioridade.
	// _even_in_range é uma macro para aumentar a eficiência da checagem
	switch (_even_in_range(P1IV, 0x10)) 
	{
		// Tabela 12-3 Family Guide.
		case 0x00: // Nenhuma interrupção pendente
			break;
		case 0x02: // Interrupção do P1.0
			break;
		case 0x04: // Interrupção do P1.1
//			P1OUT ^= BIT0; 
			sw2_flag = 1;
			break;
		case 0x06: // ...
			break;
		case 0x08:
			break;
		case 0x0A:
			break;
		case 0x0C:
			break;
		case 0x0E:
			break;
		case 0x10:
			break;
		default:
			break;
}

void io_config(){

	// Chave P2 como entrada.
	P1SEL &= ~BIT1;
	P1DIR &= ~BIT1;
	P1REN |= BIT1;
	P1OUT |= BIT1;

	// Configurar o LED
	P1SEL &= ~BIT0;
	P1DIR |= BIT0;
	P1OUT &= ~BIT0;

	// Configurar as interrupções
	P1IE |= BIT1;
	P1IES &= ~BIT1; // borda positiva

}