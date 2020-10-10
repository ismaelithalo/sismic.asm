#include <msp430.h> 
#include "clock.h"

void delay();

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
	P1SEL &= ~BIT1;
	P1DIR &= ~BIT1;
	P1REN |= BIT1;
	P1OUT |= BIT1;

	while(1)
	{
	    if((P2IN & BIT1) == 0)
	    {
	        P4OUT ^= BIT7;
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