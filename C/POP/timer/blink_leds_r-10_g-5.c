// T09p01.c
// Led vermelho piscar em 10 Hz e
// Led verde    piscar em 5 Hz

#include <msp430.h> 

#define T50ms  1637 //50 ms com ACLK
#define TRUE    1

void leds_config(void);
void ta0_config(void);

void main(void){
    int cont=0;
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	leds_config();
	ta0_config();
	while(TRUE){
	    while ((TA0CTL&TAIFG)==0); //Esperar TAIFG=1
	    TA0CTL &= ~TAIFG;
	    P1OUT ^= BIT0;          //Inverter vermelho
	    cont++;
	    if ((cont&BIT0) == 0)   //A cada 2 vezes
	        P4OUT ^= BIT7;      //Inverter verde
	}
	return 0;
}

void ta0_config(void){
    TA0CTL = TASSEL_1 |  // ACLK = 32.768 Hz
             MC_1;       // Modo 1 = Up
    TA0CCR0 = T50ms;     //Zerar a cada 1638 contagens
}

void leds_config(void){
    P1DIR |= BIT0;      //Led1 = P1.0 = saída
    P1OUT &= ~BIT0;     //Led1 apagado
    P4DIR |= BIT7;      //Led2 = P4.7 = saída
    P4OUT &= ~BIT7;     //Led1 apagado
}
