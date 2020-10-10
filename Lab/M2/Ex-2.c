#include <msp430.h> 


/**
 * main.c
 */
#define DBC 1000


void io_config(void);
void debounce(volatile int valor);

void main(void)
{
    io_config();
    WDTCTL = WDTPW | WDTHOLD;    // stop watchdog timer

   while(1) {

       while ((P2IN&BIT1) == BIT1); //Acionou S1? //SE CHAVE ACIONADA S1
           debounce(DBC);
           P1OUT ^= BIT0;      //INVERTE O LED RED;

        }

}

// Debounce
void debounce(int valor){
    volatile int x; //volatile evita optimizador do compilador
        for (x=0; x<valor; x++); //Apenas gasta tempo
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