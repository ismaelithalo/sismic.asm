#include <msp430.h> 


/**
 * main.c
 */

void io_config(void);

void main(void)
{
    io_config();
    WDTCTL = WDTPW | WDTHOLD;    // stop watchdog timer

    while(1) {

        if((P2IN&BIT1) == 0)      //Se chave S1 acionada
            P1OUT |= BIT0;      //acende LED RED
        else
             P1OUT &= ~BIT0;       //apaga led red

        if((P1IN&BIT1) == 0)
            P4OUT |= BIT7;      //acende LED RED
        else
            P4OUT &= ~BIT7;       //apaga led red

    }

}

void io_config(void){

        P1DIR |= BIT0;              //Led1 = P1.0 = saída
        P1OUT &= ~BIT0;             //Led1 apagado

        P4DIR |= BIT7;              //Led2 = P4.7 = saída
        P4OUT &= ~BIT7;             //Led1 apagado

        P2DIR &= ~BIT1;             //S1 = P2.1 = entrada
        P2REN |= BIT1;              //Habilitar resistor
        P2OUT |= BIT1;              //Habilitar pullup

        P1DIR &= ~BIT1;             //S2 = P2.2 = entrada
        P1REN |= BIT1;              //Habilitar resistor
        P1OUT |= BIT1;              //Habilitar pullup

}