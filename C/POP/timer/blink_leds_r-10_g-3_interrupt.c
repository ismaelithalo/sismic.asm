// T09p04.c
// Led vermelho piscar em 10 Hz (TA0CCTL1.CCIFG) e
// Led verde    piscar em  3 Hz (TA0CCTL2.CCIFG)

#include <msp430.h> 


#define T_VM  1637 //(1/20) ms com ACLK
#define T_VD  5460 //(1/6) ms com ACLK
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

//#pragma vector = 52
#pragma vector = TIMER0_A1_VECTOR
__interrupt void ta0_ccifg12(void){
    int x;
    x = __even_in_range (TA0IV,0xE);    //Quem interrompeu?
    switch(x){
    case 2:
        TA0CCR1 += T_VM; //Próxima interrupção
        P1OUT ^= BIT0;   //Inverter vermelho
        break;
    case 4:
        TA0CCR2 += T_VD; //Próxima interrupção
        P4OUT ^= BIT7;   //Inverter verde
        break;
    }
}

void ta0_config(void){
    TA0CTL = TASSEL_1 |  // ACLK = 32.768 Hz
             MC_2;       // Modo 2 = Continuous
    TA0CCR1=T_VM;        //Primeira interrupção CCR1
    TA0CCTL1=CCIE;       //Hab Interrup CCIFG
    TA0CCR2=T_VD;        //Primeira interrupção CCR2
    TA0CCTL2=CCIE;       //Hab Interrup CCIFG
}

void leds_config(void){
    P1DIR |= BIT0;      //Led1 = P1.0 = saída
    P1OUT &= ~BIT0;     //Led1 apagado
    P4DIR |= BIT7;      //Led2 = P4.7 = saída
    P4OUT &= ~BIT7;     //Led1 apagado
}
