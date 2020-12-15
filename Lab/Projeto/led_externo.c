#include <msp430.h> 

#define FREQ   20000

void io_config();
void ta0_config(void);


int main(void){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    io_config();
    ta0_config();

    // P8OUT |= BIT2;
    __enable_interrupt();
    while(1);
}

 // Configurar I/O
void io_config() {

    // Configura os LEDS
    P8DIR |= BIT2;              //Led1 = P1.0 = saída
    P8OUT &= ~BIT2;             //Led1 apagado

    P3DIR |= BIT7;              //Led1 = P1.0 = saída
    P3OUT &= ~BIT7;             //Led1 apagado

    P4DIR |= BIT0;
    P4OUT &= ~BIT0;
    P4SEL &= ~BIT0;

    P4DIR |= BIT3;
    P4OUT &= ~BIT3;
    P4SEL &= ~BIT3;

    // P2DIR |= BIT5;	//Saída PWM
    // P2OUT &= ~BIT5;
    // P2SEL |= BIT5;
}

#pragma vector = TIMER0_A0_VECTOR                   // Interrupcao do timer
__interrupt void TA0_CCR0_ISR()                     // Quando o CCR0 chega ao maximo, acontece a interrupcao
{
    // ADC12CTL0  &=  ~ADC12SC;                        // Envia um trigger ao ADC12
    // ADC12CTL0  |=   ADC12SC;                        // para que ele realize uma medicao
    P8OUT ^= BIT2;
    P3OUT ^= BIT7;
    P4OUT ^= BIT0;
    P4OUT ^= BIT3;
    // P1OUT ^= BIT0;                               // Teste de frequência do timer
}

// Configurar TA0
void ta0_config(void){ 
    TA0CTL   = TASSEL__ACLK | MC__UP;               // ACLK e modo 1
    TA0CCR0  = FREQ;                                // Ponto de disparo da interrupção
    TA0CCTL0 = CCIE;                                // Habilita interrupcao do timer

    P1DIR |= BIT2;                                  //Sair TA0.1
    P1SEL |= BIT2;
 }