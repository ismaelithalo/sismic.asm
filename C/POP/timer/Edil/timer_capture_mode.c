#include <msp430.h> 
#include "clock.h"

void delay();

volatile unsigned int tempo;
volatile int flag_stop;

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// ACLK = 32768 Hz
	clockInit();

	// Configura o LED
	P4SEL &= ~BIT7;
	P4DIR |= BIT7;
	P4OUT &= ~BIT7;

	// Configura a chave SW1
	P2SEL &= ~BIT1;
	P2DIR &= ~BIT1;
	P2REN |= BIT1;
	P2OUT |= BIT1;

	// Configura a chave SW2
	P1SEL |= BIT1;     // Direciona a chave para o módulo.
	P1DIR &= ~BIT1;
	P1REN |= BIT1;
	P1OUT |= BIT1;

	// Configura o timer
	TA0CTL = TASSEL__ACLK + // ACLK
	         TAIDEX_0 +     // dividor = 1
	         MC__CONTINUOUS; // Modo contínuo

	TA0CCTL0 = CM_2 +    // Borda negativa
	           CCIS_0 +  // Pino P1.1
	           SCS +     // Captura síncrona
	           CAP +     // Modo de captura
	           CCIE;     // Habilita interrupções

	tempo = 0;
	flag_stop = 0;

	// Ligo as interrupções na CPU
	_enable_interrupts();

	while(1)
	{
	    if((P2IN & BIT1) == 0)  // Botão SW1 está pressionado
	    {
	        if(flag_stop == 1)
	        {
	            tempo = 0;
	            flag_stop = 0;
	            TA0CTL |= TACLR; // Zera o timer
	        }
	        delay();
	    }
	}

	return 0;
}

void delay()
{
    volatile unsigned int x = 60000;

    while(x>0)
    x--;

}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void CCR0_ISR()
{
    tempo = TA0CCR0;
    P4OUT |= BIT7;
    flag_stop = 1;
}
