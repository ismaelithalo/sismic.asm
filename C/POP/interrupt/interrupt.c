// P3.c

#include <msp430.h> 

// Constante para facilitar programação
#define BIT65432 (BIT6|BIT5|BIT4|BIT3|BIT2) //0x7C = 0111 1100
#define TRUE        1

void io_config(void);

int main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    io_config();
    __enable_interrupt();       //GIE=1
    while(TRUE);                //Laço infinito
    return 0;
}

// ISR para a Porta P1
#pragma vector=PORT1_VECTOR
__interrupt void isr_p1(void){
    int x;
    x=__even_in_range (P1IV,16);    //Quem interrompeu?
    switch(x){
        case  0:                 break;
        case  2:                 break;
        case  4:                 break;
        case  6: P1OUT &= ~BIT0; break;  //Apagar  Vermelho
        case  8: P1OUT |=  BIT0; break;  //Acender Vermelho
        case 10: P4OUT &= ~BIT7; break;  //Apagar  Verde
        case 12: P4OUT |=  BIT7; break;  //Acender Verde
        case 14: P1OUT ^=  BIT0; P4OUT ^=  BIT7; break;  //Inverter
        case 16:                 break;
    }
}

// Configurar GPIO
void io_config(void){
    P1DIR |= BIT0;      //Led1 = P1.0 = saída
    P1OUT &= ~BIT0;     //Led1 apagado

    P4DIR |= BIT7;      //Led2 = P4.7 = saída
    P4OUT &= ~BIT7;     //Led1 apagado

    P1DIR &= ~BIT65432;  //P2.65432 = entrada
    P1REN |=  BIT65432;  //Habilitar resistor
    P1OUT |=  BIT65432;  //Habilitar pullup
    P1IES |=  BIT65432;  //Flanco de descida
    P1IFG &= ~BIT65432;  //Apagar possíveis pedidos
    P1IE  |=  BIT65432;  //Habilitar interrupção
}
