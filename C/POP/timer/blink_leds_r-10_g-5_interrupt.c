// T09p02.c
// Led vermelho piscar em 10 Hz e
// Led verde    piscar em 5 Hz
// Usar Interrupção TA0CCTL0.CCIFG

#include <msp430.h> 


#define T50ms  1637 //50 ms com ACLK
#define TRUE    1

void leds_config(void);
void ta0_config(void);

void main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    leds_config();
    ta0_config();
    __enable_interrupt();
    while(TRUE);
}

//#pragma vector = 53
#pragma vector = TIMER0_A0_VECTOR
__interrupt void ta0_ccifg0(void){
    static int cont=0;
    P1OUT ^= BIT0;  //Inverter vermelho
    cont++;
    if ((cont&BIT0) == 0)   //A cada 2 vezes
        P4OUT ^= BIT7;      //Inverter verde
}

void ta0_config(void){
    TA0CTL = TASSEL_1 |  // ACLK = 32.768 Hz
             MC_1;       // Modo 1 = Up
    TA0CCTL0 = CCIE;     //Hab Interrup CCIFG
    TA0CCR0 = T50ms;     //Zerar a cada 1638 contagens
}

void leds_config(void){
    P1DIR |= BIT0;      //Led1 = P1.0 = saída
    P1OUT &= ~BIT0;     //Led1 apagado
    P4DIR |= BIT7;      //Led2 = P4.7 = saída
    P4OUT &= ~BIT7;     //Led1 apagado
}